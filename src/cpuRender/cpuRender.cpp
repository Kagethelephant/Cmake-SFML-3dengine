#include "cpuRender.hpp"
// Standard Libraries
#include <algorithm>
#include <cstdint>
#include <strings.h>
#include <vector>
#include <math.h>
// Program Headers
#include "gpuRender/window.hpp"
#include "gpuRender/RAIIWrapper.hpp"
#include "utils/data.hpp"
#include "utils/matrix.hpp"
#include "utils/utils.hpp"
#include "app/object.hpp"



//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// CONSTRUCTOR
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
cpuRenderObject::cpuRenderObject(camera& _cam) : cam{_cam}, gl_window{_cam.gl_window}{

   // Calculate the planes that make up the frustum (box that represents the field of view)
   m_resolution = vec2(gl_window.fboWidth,gl_window.fboHeight);
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
}

void cpuRenderObject::bindObject(const object& obj) {
   objects.push_back(obj);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// TRIANGLE PROJECTION
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void cpuRenderObject::render() {
 
   for (const object& obj : objects){

      // Fetch model associated with object
      model mod = obj.mod;
      // Model * View * Project matrix
      mat4x4 vp = cam.mat_view * m_matProject;
      mat4x4 m = obj.matScale * obj.matTransform;

      vertAttribs.resize(mod.vertices.size());

      // Vertex shader (Model View Projection matrix multiplication)
      for(int i=0; i<mod.vertices.size(); i ++){
         vertex newVert = mod.vertices[i];
         newVert.fragPos = newVert.pos * m;
         newVert.pos *= (m * vp);
         newVert.clipPos = newVert.pos;
         vertAttribs[i] = newVert;
      }

      colRef = hexColorToRGB(obj.color);

      for (const auto& mesh : mod.subMeshes) {

         texRef = mesh.tex;
         hasTexRef = mesh.textured;
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
            triangle3d& p = primatives[i];
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

   GLScopedTexture2D tempTexture(gl_window.colorTex);
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, gl_window.fboWidth, gl_window.fboHeight, GL_RGBA, GL_UNSIGNED_BYTE, m_pixelBuffer.data());
   // Clear the pixel buffer with the background color before drawing each triangle
   m_pixelBuffer = m_clearBuffer;
   // Clear the z buffer with far depth
   m_zBuffer.assign(m_zBuffer.size(), 1.0f);
}



//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// TRIANGLE CLIPPING
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void cpuRenderObject::clipTriangles() {
   
   // Create buffer to hold triangles that need to go through the split function and 
   // a buffer of triangles that have already been through the split function
   std::vector<triangle3d> splitBuffer;
   splitBuffer.reserve(primatives.size() * 2);

   // Cycle through each plane and each triangle so we can clip them against each plane
   int next, last;
   float t1, t2;
   vertex p1, p2;

   for (vec4& plane : m_planes) {
      for(triangle3d& t : primatives) {

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
void cpuRenderObject::raster(const triangle3d& pr) {
   // Bounding box triangle filling method with barycentric coordinates to interpolate 
   // between points. This is the trickiest part of the pipeline
   const vertex& v0 = pr.v[0];
   const vertex& v1 = pr.v[1];
   const vertex& v2 = pr.v[2];
   // Screen-space positions
   float invW0 = 1.0f / v0.clipPos.w;
   float invW1 = 1.0f / v1.clipPos.w;
   float invW2 = 1.0f / v2.clipPos.w;

   float ndcZ0 = v0.clipPos.z * invW0;
   float ndcZ1 = v1.clipPos.z * invW1;
   float ndcZ2 = v2.clipPos.z * invW2;

   vec4 fragOverW0 = v0.fragPos * invW0;
   vec4 fragOverW1 = v1.fragPos * invW1;
   vec4 fragOverW2 = v2.fragPos * invW2;

   vec2 uvOverW0 = v0.uv * invW0;
   vec2 uvOverW1 = v1.uv * invW1;
   vec2 uvOverW2 = v2.uv * invW2;

   int xmin = std::max((int)std::min({ v0.pos.x, v1.pos.x, v2.pos.x }), 1);
   int xmax = std::min((int)std::max({ v0.pos.x, v1.pos.x, v2.pos.x }), (int)m_resolution.x - 1);
   int ymin = std::max((int)std::min({ v0.pos.y, v1.pos.y, v2.pos.y }), 1);
   int ymax = std::min((int)std::max({ v0.pos.y, v1.pos.y, v2.pos.y }), (int)m_resolution.y - 1);

   vec2 p0(v0.pos.x, v0.pos.y);
   vec2 p1(v1.pos.x, v1.pos.y);
   vec2 p2(v2.pos.x, v2.pos.y);


   // --- Edge function setup

   float area = edgeFunction(p0, p1, p2);
   if (area == 0.0f) return;
   float invArea = 1.0f / area;

   float alpha_dx = (p2.y - p1.y) * invArea;
   float alpha_dy = (p1.x - p2.x) * invArea;
   float beta_dx  = (p0.y - p2.y) * invArea;
   float beta_dy  = (p2.x - p0.x) * invArea;
   float gamma_dx = (p1.y - p0.y) * invArea;
   float gamma_dy = (p0.x - p1.x) * invArea;

   // Initial barycentrics at first pixel center
   vec2 start(xmin + 0.5f, ymin + 0.5f);
   float alpha_row = edgeFunction(p1, p2, start) * invArea;
   float beta_row  = edgeFunction(p2, p0, start) * invArea;
   float gamma_row = edgeFunction(p0, p1, start) * invArea;

   // --- Approximate geometric normal from screen-space derivatives

   vec2 AB = (v1.pos - v0.pos).xy();
   vec2 AC = (v2.pos - v0.pos).xy();

   float AB_AB = AB.dot(AB);
   float AC_AC = AC.dot(AC);
   float AB_AC = AB.dot(AC);
   float invDet = 1.0f / (AB_AB * AC_AC - AB_AC * AB_AC);

   vec2 dBeta ( AC_AC * invDet, -AB_AC * invDet);
   vec2 dGamma(-AB_AC * invDet,  AB_AB * invDet);
   vec2 dAlpha = (dBeta + dGamma) * -1.0f;

   vec3 dFdx = (v0.fragPos * dAlpha.x + v1.fragPos * dBeta.x + v2.fragPos * dGamma.x).xyz();
   vec3 dFdy = (v0.fragPos * dAlpha.y + v1.fragPos * dBeta.y + v2.fragPos * dGamma.y).xyz();
   vec3 normal = dFdx.cross(dFdy).normal();

   // ======================================================================================
   // Raster loop
   // ======================================================================================

   for (int y = ymin; y <= ymax; ++y)
   {
      float alpha = alpha_row;
      float beta  = beta_row;
      float gamma = gamma_row;

      for (int x = xmin; x <= xmax; ++x)
      {
         if (alpha >= 0.0f && beta >= 0.0f && gamma >= 0.0f)
         {
            float depth = (alpha * ndcZ0 + beta  * ndcZ1 + gamma * ndcZ2) * 0.5f + 0.5f;

            int index = x + y * m_resolution.x;

            if (depth <= m_zBuffer[index])
            {
               // Perspective-correct attribute reconstruction
               float invW = 1.0f / (alpha * invW0 + beta  * invW1 + gamma * invW2);

               vec3 fragPos = (fragOverW0 * alpha + fragOverW1 * beta + fragOverW2 * gamma).xyz() * invW;

               float r = 1, g = 1, b = 1, a = 1;

               if (hasTexRef) {

                  vec2 uv = (uvOverW0 * alpha + uvOverW1 * beta + uvOverW2 * gamma) * invW;

                  // Texture wrap
                  uv.x -= std::floor(uv.x);
                  uv.y -= std::floor(uv.y);

                  int tx = (int)(uv.x * texRef.w);
                  int ty = (int)(uv.y * texRef.h);
                  int texel = (ty * texRef.w + tx) * texRef.channels;

                  if (texRef.channels >= 3) {
                     r = texRef.data[texel + 0];
                     g = texRef.data[texel + 1];
                     b = texRef.data[texel + 2];
                     if (texRef.channels == 4)
                        a = texRef.data[texel + 3];
                  }
                  else { // grayscale
                     r = g = b = texRef.data[texel] / 255.0f;
                  }
               }
               else {
                  r = (std::uint8_t)colRef.x;
                  g = (std::uint8_t)colRef.y;
                  b = (std::uint8_t)colRef.z;
                  a = (std::uint8_t)colRef.w;
               }

               vec4 texColor(r, g, b, a);

               // Diffuse + constant ambient
               vec3 lighting(0,0,0);

               for (const light& l : lights){
                  vec3 L = (l.position - fragPos).normal();
                  float diff = std::max(normal.dot(L), 0.0f);
                  lighting += l.color * (diff + 0.2f);
               }

               vec4 result = texColor * vec4(lighting, 1.0f);
               m_pixelBuffer[index*4 + 0] = std::clamp(result[0], 0.0f, 255.0f);
               m_pixelBuffer[index*4 + 1] = std::clamp(result[1], 0.0f, 255.0f);
               m_pixelBuffer[index*4 + 2] = std::clamp(result[2], 0.0f, 255.0f);
               m_pixelBuffer[index*4 + 3] = std::clamp(result[3], 0.0f, 255.0f);

               m_zBuffer[index] = depth;
            }
         }
         alpha += alpha_dx;
         beta  += beta_dx;
         gamma += gamma_dx;
      }
      alpha_row += alpha_dy;
      beta_row  += beta_dy;
      gamma_row += gamma_dy;
   }
}



//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// HELPER FUNCTIONS
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
bool cpuRenderObject::pointOutOfPlane(const vec4& p, const vec4& plane){
   // plane.xyz = normal, plane.w = D
   return plane.dot(p) < 0.0f;
}

float cpuRenderObject::planeIntersect(const vec4& a, const vec4& b, const vec4& plane) {
   return plane.dot(a) / (plane.dot(a) - plane.dot(b));
}

float cpuRenderObject::edgeFunction(const vec2& a, const vec2& b, const vec2& p){
    return (p[0] - a[0]) * (b[1] - a[1]) - (p[1] - a[1]) * (b[0] - a[0]);
}

bool cpuRenderObject::backFaceCulling(const triangle3d& tri) {
    // Use only X/Y in screen space.
    const vec2& a = tri.v[0].pos.xy();
    const vec2& b = tri.v[1].pos.xy();
    const vec2& c = tri.v[2].pos.xy();
    // Compute signed area (2D cross product)
    float area = (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]);
    // Area will return negative if the triangle has CCW winding
    return area < 0.0f;
}
