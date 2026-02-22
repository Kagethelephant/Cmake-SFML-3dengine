#include "cpuRender.hpp"

#include <gpuRender/window.hpp>
#include <iostream>
#include <algorithm>
#include <cstdint>
#include <strings.h>
#include <vector>
#include <math.h>
#include "utils/data.hpp"
#include "utils/matrix.hpp"
#include "utils/utils.hpp"
#include "app/object.hpp"



//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// CONSTRUCTOR
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
camera::camera(gl_window& _window) : window{_window}{

   // Calculate the planes that make up the frustum (box that represents the field of view)
   m_resolution = vec2(window.fboWidth,window.fboHeight);
   float fov = 70;
   float aspectRatio = (float)m_resolution[0]/(float)m_resolution[1];
   float n = 0.1f;
   far = 1000.0f;

   m_planes[0] = vec4( 1, 0, 0, 1);  // Left
   m_planes[1] = vec4(-1, 0, 0, 1);  // Right 
   m_planes[2] = vec4( 0, 1, 0, 1);  // Bottom
   m_planes[3] = vec4( 0, -1, 0, 1); // Top 
   m_planes[4] = vec4( 0, 0, 1, 1);  // Near
   m_planes[5] = vec4( 0, 0, -1, 1); // Far

   // Create the projection matrix that will be used to project 3D points to a 2D view
   m_matProject = matrix_project(fov,aspectRatio,n,far);  

   // Create a background buffer the size of the window to clear the pixel buffer with a color
   m_clearBuffer = std::vector<std::uint8_t>(m_resolution[0] * m_resolution[1] * 4, 0);
   m_zBuffer = std::vector<float>(m_resolution[0] * m_resolution[1], 1.0f);

   vec4 bgColor(hexColorToRGB(Color::Black));
   int index = 0;

   for (int y = 0; y < m_resolution[1]; y++){
      for (int x = 0; x < m_resolution[0]; x++){

         index = (y * m_resolution[0] + x);
         m_clearBuffer[index*4] = bgColor[0];
         m_clearBuffer[index*4 + 1] = bgColor[1];
         m_clearBuffer[index*4 + 2] = bgColor[2];
         m_clearBuffer[index*4 + 3] = bgColor[3];
      }
   }
   m_pixelBuffer = m_clearBuffer;
   move(0,0,0);
   rotate(0, 0, 0);
}






//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// TRIANGLE PROJECTION
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void camera::render(const object& object) {
  
   // Fetch model associated with object
   model mod = object.mod;

   // Model * View * Project matrix
   mat4x4 vp = m_matView * m_matProject;
   mat4x4 m = object.matScale * object.matTransform;

   vertAttribs.resize(mod.vertices.size());

   // Vertex shader (Model View Projection matrix multiplication)
   for(int i=0; i<mod.vertices.size(); i ++){
      vertex newVert = mod.vertices[i];
      newVert.fragPos = newVert.pos * m;
      newVert.pos *= (m * vp);
      newVert.clipPos = newVert.pos;
      vertAttribs[i] = newVert;
   }

   for (subMesh mesh : mod.subMeshes) {
      
      texW = mesh.tex.w;
      texH = mesh.tex.h;
      texC = mesh.tex.channels;
      texD = mesh.tex.data;

      for(int i=0; i< mesh.indices.size(); i += 3){

         // Generate triangles from vertices and indices
         int i0 = mesh.indices[i];
         int i1 = mesh.indices[i+1];
         int i2 = mesh.indices[i+2];
         primatives.emplace_back(vertAttribs[i0],vertAttribs[i1],vertAttribs[i2]);
      }

      // Clip triangles.
      clipTriangles();

      for(int i=0; i< primatives.size(); i++) {
         prim& p = primatives[i];
         p.perspectiveDivide();
         

         for (int i=0; i<3; i++){
            // Projection results are between -1 and 1. So shift to the positive and scale to fit screen
            p.v[i].pos.x = (p.v[i].pos.x + 1.0f) * 0.5f * m_resolution.x;
            p.v[i].pos.y = (p.v[i].pos.y + 1.0f) * 0.5f * m_resolution.y;
         }

         // Do not raster if winding is incorrect (cull back faces)
         if(!backFaceCulling(p)){ raster(p);}
      }
      primatives.clear();
   }
}



//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// TRIANGLE CLIPPING
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void camera::clipTriangles() {
   
   // Create buffer to hold triangles that need to go through the split function and 
   // a buffer of triangles that have already been through the split function
   std::vector<prim> splitBuffer;
   splitBuffer.reserve(primatives.size() * 2);

   // Cycle through each plane and each triangle so we can clip them against each plane
   int next, last;
   float t1, t2;
   vertex p1, p2;

   for (vec4& plane : m_planes) {
      for(prim& t : primatives) {

         // For consiseness check what points are out of the current plane ahead of time
         bool clipped[3] = {pointOutOfPlane(t.v[0].pos, plane), pointOutOfPlane(t.v[1].pos, plane), pointOutOfPlane(t.v[2].pos, plane)};
         // If all of the points are not clipped by this plane then pass along the triangle to the next step
         if (clipped[0]+clipped[1]+clipped[2] == 0){splitBuffer.push_back(t); continue;}
         if (clipped[0]+clipped[1]+clipped[2] == 3){ continue;}

         // Cycle through all of the points to find the clipped points
         for(int i=0; i<3; i++){
            // Get the position of the next and last point clockwise from the current point
            next = wrap(i+1,3);
            last = wrap(i+2,3);
            // We make sure the last point is not clipped so we exclude triangles with all points out of the plane
            if (clipped[i] and not clipped[last]){
               // If there are 2 points and they are the current and next point
               if (clipped[next]){
                  // Find new points where the edges of the triangles intercect the plane
                  t1 = planeIntersect(t.v[i].pos, t.v[last].pos, plane);
                  t2 = planeIntersect(t.v[next].pos, t.v[last].pos, plane);

                  p1 = t.v[i].lerp(t.v[last], t1);
                  p2 = t.v[next].lerp(t.v[last], t2);

                  splitBuffer.emplace_back(p1,p2,t.v[last]);
                  break;
               }
               // If there is only one point and it is the current point
               else {
                  // Find new points where the edges of the triangles intercect the plane
                  t1 = planeIntersect(t.v[i].pos, t.v[last].pos, plane);
                  t2 = planeIntersect(t.v[i].pos, t.v[next].pos, plane);

                  p1 = t.v[i].lerp(t.v[last], t1);
                  p2 = t.v[i].lerp(t.v[next], t2);

                  // Create 2 new triangles with the 2 new points and the 2 unclipped points
                  splitBuffer.emplace_back(p1,p2,t.v[next]);
                  splitBuffer.emplace_back(p1,t.v[next],t.v[last]);
                  break;
               }
            }
         }
      }
      // Pass triangles from the working buffer back into the loop for the next plane (and clear working buffer)
      primatives.swap(splitBuffer);
      splitBuffer.clear();
   }
}




//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// FILL TRIANGLE
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void camera::raster(const prim& pr) {
   // Bounding box triangle filling method with barycentric coordinates to interpolate 
   // between points. This is the trickiest part of the pipeline

   // Transform the light into viewspace
   // lightPosView = (lightPos * m_matView);

   vertex v0 = pr.v[0];
   vertex v1 = pr.v[1];
   vertex v2 = pr.v[2];

   // Screen-space positions
   vec2 p0(v0.x, v0.y);
   vec2 p1(v1.x, v1.y);
   vec2 p2(v2.x, v2.y);
   // Calculate the inverse W value for interpolation on attributes
   float invW0 = 1/v0.clipPos.w;
   float invW1 = 1/v1.clipPos.w;
   float invW2 = 1/v2.clipPos.w;
   // Calculate the NDC space Z value corrected with W for interpolation
   float ndcZ0 = v0.clipPos.z * invW0;
   float ndcZ1 = v1.clipPos.z * invW1;
   float ndcZ2 = v2.clipPos.z * invW2;
   // Calculate the fragPos / w for perspective correction
   vec4 fragOverW0 = v0.fragPos * invW0;
   vec4 fragOverW1 = v0.fragPos * invW1;
   vec4 fragOverW2 = v0.fragPos * invW2;
   // Calculate the fragPos / w for perspective correction
   vec2 uvOverW0 = v0.uv * invW0;
   vec2 uvOverW1 = v1.uv * invW1;
   vec2 uvOverW2 = v2.uv * invW2;

   // Get the coordinates of the rectangle that will cover the triangle (clamped to the buffer size)
   int xmax = std::min(std::max(std::max(v0.pos.x, v1.pos.x), v2.pos.x), m_resolution[0] - 1.0f);
   int xmin = std::max(std::min(std::min(v0.pos.x, v1.pos.x), v2.pos.x), 1.0f);
   int ymax = std::min(std::max(std::max(v0.pos.y, v1.pos.y), v2.pos.y), m_resolution[1] - 1.0f);
   int ymin = std::max(std::min(std::min(v0.pos.y, v1.pos.y), v2.pos.y), 1.0f);


   // For point p on a triangle: P = a*A + β*B + γ*C
   // A, B and C representing the points on triangle and alpha(a), beta(β) and 
   // gamma(γ) representing weight of areas on triangle subdivided into 3 parts using p
   // we also have the constraint: a + β + γ = 1 or (a = 1 - β - γ)
   // So with substitution:        P = (1−β−γ)A + βB + γC 
   // Rearange to get:             P − A = β(B−A) + γ(C−A) or AP = βAB + γAC

   // Screen-space edge vectors originating from vertex A
   vec2 AB = (v1.pos - v0.pos).xy(); // B - A
   vec2 AC = (v2.pos - v0.pos).xy(); // C - A
   // Precompute dot products of edge vectors
   float AB_AB = AB.dot(AB); 
   float AC_AC = AC.dot(AC);
   float AB_AC = AB.dot(AC);
   // Gram matrix system obtained by dotting AP = βAB + γAC with AB and AC:
   // [ AB⋅AB  AB⋅AC ] [ β ] = [ AP⋅AB ]
   // [ AB⋅AC  AC⋅AC ] [ γ ]   [ AP⋅AC ]
   // Determinant of the Gram matrix
   float determ = AB_AB * AC_AC - AB_AC * AB_AC;
   float invDet = 1.0f / determ;
   // Rows of the inverse Gram matrix below form linear maps that convert
   // screen-space (x,y) movement into barycentric coordinate changes.
   // [ AC⋅AC -AB⋅AC]                                           
   // [-AB⋅AC  AB⋅AC]
   vec2 dBeta(  AC_AC * invDet, -AB_AC * invDet);
   vec2 dGamma(-AB_AC * invDet,  AB_AB * invDet);
   vec2 dAlpha = vec2(0,0) - dBeta - dGamma;
   // Compute ∂fragPos/∂x and ∂fragPos/∂y using barycentric derivatives
   vec3 dFdx_fragPos = (v0.fragPos * dAlpha.x + v1.fragPos * dBeta.x + v2.fragPos * dGamma.x).xyz();
   vec3 dFdy_fragPos = (v0.fragPos * dAlpha.y + v1.fragPos * dBeta.y + v2.fragPos * dGamma.y).xyz();
   // Since these vectors are tangent to the surface of the triangle
   // we can calculate the normal from these with the cross product
   vec3 norm = dFdy_fragPos.cross(dFdx_fragPos).normal();

   
   // Signed twice-area of the triangle, used to normalize edge functions
   float area = edgeFunction(p0, p1, p2);
   if (area == 0.0f) return;
   float invArea = 1.0f / area;
   // Partial derivatives of each edge function with respect to screen-space x and y.
   // These are constant across the triangle and allow incremental evaluation.
   float alpha_dx = (p2[1] - p1[1]) * invArea;
   float alpha_dy = (p1[0] - p2[0]) * invArea;
   float beta_dx =  (p0[1] - p2[1]) * invArea;
   float beta_dy =  (p2[0] - p0[0]) * invArea;
   float gamma_dx = (p1[1] - p0[1]) * invArea;
   float gamma_dy = (p0[0] - p1[0]) * invArea;
   // +0.5.f is consistant with opengl pixel-center rules
   vec2 p(xmin + 0.5f, ymin + 0.5f);
   // Initial edge function values for the first pixel
   float alpha_row = edgeFunction(p1, p2, p) * invArea;
   float beta_row = edgeFunction(p2, p0, p) * invArea;
   float gamma_row = edgeFunction(p0, p1, p) * invArea;

   // Allocate memory for value calculated in inner loop
   float alpha, beta, gamma, interpz,interpW, baryz;
   // Itterate through each pixel in that rectangle
   for (int y=ymin; y<=ymax; y++){
      // Edge function values at the start of the current scanline
      float alpha = alpha_row;
      float beta  = beta_row;
      float gamma = gamma_row;

      for (int x=xmin; x<=xmax; x++){
         // Pixel is inside the triangle if all edge functions are non-negative
         if (alpha >= 0 && beta >= 0 && gamma >= 0) {

            // Find interpolated NDC z value with barycentric formula
            interpz = alpha * ndcZ0 + beta * ndcZ1 + gamma * ndcZ2; 
            baryz = interpz * 0.5f + 0.5f; // Convert NDC to depth buffer value [0 - 1]

            // Get the position of the pixel in the z-buffer and only draw a pixel if the pixel should be on top
            // y is window-space (bottom-left)
            int index = x + y * m_resolution[0];
            if (baryz <= m_zBuffer[index]){

               // Find interpolated W value for perspective correction on attributes (not x,y,z)
               interpW = 1 / (alpha * invW0 + beta * invW1 + gamma * invW2); 
               
               // Get interpolated position in screenspace for lightDir calculation
               vec3 fragPos = ((fragOverW0 * alpha + fragOverW1 * beta + fragOverW2 * gamma) * interpW).xyz();

               vec2 uv = ((uvOverW0 * alpha + uvOverW1 * beta + uvOverW2 * gamma) * interpW);


               // Wrap instead of clamp like opengl does by default
               auto fract = [](float x)
               { return x - std::floor(x); };
               float cu = fract(uv.x);
               float cv = fract(uv.y);

               int tx = (cu >= 1.0f) ? (texW - 1) : (int)std::floor(cu * (float)texW);
               int ty = (cv >= 1.0f) ? (texH - 1) : (int)std::floor(cv * (float)texH);

               int texelIndex = (ty * texW + tx) * texC;

               float r, g, b, a = 1.0f;

               if (texC == 1) {
                  r = g = b = texD[texelIndex] / 255.0f;
               }
               else if (texC == 3) {
                  r = texD[texelIndex + 0];
                  g = texD[texelIndex + 1];
                  b = texD[texelIndex + 2];
               }
               else if (texC == 4) {
                  r = texD[texelIndex + 0];
                  g = texD[texelIndex + 1];
                  b = texD[texelIndex + 2];
                  a = texD[texelIndex + 3];
               }

               vec4 texColor(r, g, b, a);
               
               vec3 lightSum = vec3(0,0,0);
               for (light l : lights){
                  // Calculate diffuse lighting
                  vec3 lightDir = (l.position - fragPos).normal();
                  float diffuse = std::max(norm.dot(lightDir), 0.0f);
                  vec3 ambient = l.color * 0.2f;
                  lightSum += (l.color * diffuse) + ambient;
               }
               // Calculate color by multiplying object color by ambient + diffuse lighting respectively
               vec4 result = texColor * vec4(lightSum,1.0f);

               // Put pixel with clamped color components to match 32-bit color
               m_pixelBuffer[index*4] =     std::clamp(result[0], 0.0f, 255.0f);
               m_pixelBuffer[index*4 + 1] = std::clamp(result[1], 0.0f, 255.0f); 
               m_pixelBuffer[index*4 + 2] = std::clamp(result[2], 0.0f, 255.0f); 
               m_pixelBuffer[index*4 + 3] = std::clamp(result[3], 0.0f, 255.0f); 
               m_zBuffer[index] = baryz; 
            }
         }
         // Move along the edge for the given x position using dx
         alpha += alpha_dx;
         beta  += beta_dx;
         gamma += gamma_dx;
      }
      // Move along the edge for the given y position using dy
      alpha_row += alpha_dy;
      beta_row  += beta_dy;
      gamma_row += gamma_dy;
   }
}


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// SF::DRAWABLE IMPLIMENTATION
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void camera::draw() {
   // Clear the pixel buffer with the background color before drawing each triangle
   m_pixelBuffer = m_clearBuffer;
   // Clear the z buffer with far depth
   m_zBuffer.assign(m_zBuffer.size(), 1.0f);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// UTILITY FUNCTIONS
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
bool camera::pointOutOfPlane(const vec4& p, const vec4& plane){
   // plane.xyz = normal, plane.w = D
   return plane.dot(p) < 0.0f;
}

float camera::planeIntersect(const vec4& a, const vec4& b, const vec4& plane) {
   return plane.dot(a) / (plane.dot(a) - plane.dot(b));
}

float camera::edgeFunction(const vec2& a, const vec2& b, const vec2& p){
    return (p[0] - a[0]) * (b[1] - a[1]) - (p[1] - a[1]) * (b[0] - a[0]);
}

bool camera::backFaceCulling(const prim& tri) {
    // Use only X/Y in screen space.
    const vec2& a = tri.v[0].pos.xy();
    const vec2& b = tri.v[1].pos.xy();
    const vec2& c = tri.v[2].pos.xy();
    // Compute signed area (2D cross product)
    float area = (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]);
    // Area will return negative if the triangle has CCW winding
    return area > 0.0f;
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// MOVE AND ROTATE CAMERA
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void camera::move(float x, float y, float z) {
   
   vec3 up = vec3(0,1,0) * matrix_transform(0, 0, 0, camRotation[0], camRotation[1], camRotation[2]);
   camPosition += (camDirection.cross(up) * x);
   camPosition += (camDirection * z);
   camPosition[1] += y;
   m_matView = matrix_view(matrix_pointAt(camPosition, camDirection, up));
}


void camera::rotate(float u, float v, float w) {
   
   camRotation += vec3(u, v, w);
   vec3 up = vec3(0,1,0) * matrix_transform(0, 0, 0, camRotation[0], camRotation[1], camRotation[2]);
   camDirection = (vec3(0,0,-1) * matrix_transform(0, 0, 0, camRotation[0], camRotation[1], camRotation[2])).normal();
   m_matView = matrix_view(matrix_pointAt(camPosition, camDirection, up));
}


