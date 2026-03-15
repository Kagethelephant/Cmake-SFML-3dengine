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
#include "app/object.hpp"



//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// CONSTRUCTOR
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
cpuRenderObject::cpuRenderObject(camera& cam) : m_camera{cam}, m_window{cam.getWindow()}{

   // Calculate the planes that make up the frustum (box that represents the field of view)
   m_resolution = vec2(m_window.fboWidth,m_window.fboHeight);

   m_planes[0] = vec4( 1, 0, 0, 1);  // Left
   m_planes[1] = vec4(-1, 0, 0, 1);  // Right 
   m_planes[2] = vec4( 0, 1, 0, 1);  // Bottom
   m_planes[3] = vec4( 0, -1, 0, 1); // Top 
   m_planes[4] = vec4( 0, 0, 1, 1);  // Near
   m_planes[5] = vec4( 0, 0, -1, 1); // Far

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
   m_objects.push_back(obj);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// TRIANGLE PROJECTION
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void cpuRenderObject::render() {
 
   for (const object& obj : m_objects){

      // Fetch model associated with object
      model mod = obj.getModel();
      // Model * View * Project matrix
      mat4x4 vp = m_camera.getViewMatrix() * m_camera.getProjectionMatrix();
      mat4x4 m = obj.getScaleMatrix() * obj.getTransformMatrix();

      m_vertAttribs.resize(mod.getVertices().size());

      // Vertex shader (Model View Projection matrix multiplication)
      for(int i=0; i<mod.getVertices().size(); i ++){
         vertex newVert = mod.getVertices()[i];
         newVert.fragPos = newVert.screenPos * m;
         newVert.screenPos *= (m * vp);
         newVert.clipPos = newVert.screenPos;
         m_vertAttribs[i] = newVert;
      }

      for (const auto& mesh : mod.getSubMeshes()) {
         for(int i=0; i< mesh.indices.size(); i += 3){

            // Generate triangles from vertices and indices
            int i0 = mesh.indices[i];
            int i1 = mesh.indices[i+1];
            int i2 = mesh.indices[i+2];
            m_primatives.emplace_back(m_vertAttribs[i0],m_vertAttribs[i1],m_vertAttribs[i2]);
         }
         // Clip triangles.
         clipTriangles();

         for(int i=0; i< m_primatives.size(); i++) {
            triangle3d& p = m_primatives[i];
            p.perspectiveDivide();

            for (int i=0; i<3; i++){
               // Projection results are between -1 and 1. So shift to the positive and scale to fit screen
               p.v[i].screenPos.x = (p.v[i].screenPos.x + 1.0f) * 0.5f * m_resolution.x;
               p.v[i].screenPos.y = (p.v[i].screenPos.y + 1.0f) * 0.5f * m_resolution.y;
            }
            // Do not raster if winding is incorrect (cull back faces)
            if(!backFaceCulling(p)){ raster(p, obj, mesh);}
         }
         m_primatives.clear();
      }
   }

   GLScopedTexture2D tempTexture(m_window.colorTex);
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_window.fboWidth, m_window.fboHeight, GL_RGBA, GL_UNSIGNED_BYTE, m_pixelBuffer.data());
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
   splitBuffer.reserve(m_primatives.size() * 2);

   // Cycle through each plane and each triangle so we can clip them against each plane
   int next, last;
   float t1, t2;
   vertex p1, p2;

   for (vec4& plane : m_planes) {
      for(triangle3d& t : m_primatives) {

         // For consiseness check what points are out of the current plane ahead of time
         // Dot product of plane in Ax + Bx + Cz + D form and point will tell you what side 
         // the plane it is on. negative and positive on opisite sides and 0 on the plane
         bool clipped[3] = {
            plane.dot(t.v[0].screenPos) < 0.0f,
            plane.dot(t.v[1].screenPos) < 0.0f,
            plane.dot(t.v[2].screenPos) < 0.0f};

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
                  t1 = planeIntersect(t.v[i].screenPos, t.v[last].screenPos, plane);
                  t2 = planeIntersect(t.v[next].screenPos, t.v[last].screenPos, plane);

                  p1 = t.v[i].lerp(t.v[last], t1);
                  p2 = t.v[next].lerp(t.v[last], t2);

                  splitBuffer.emplace_back(p1,p2,t.v[last]);
                  break;
               }
               // If there is only one point and it is the current point
               else {
                  // Find new points where the edges of the triangles intercect the plane
                  t1 = planeIntersect(t.v[i].screenPos, t.v[last].screenPos, plane);
                  t2 = planeIntersect(t.v[i].screenPos, t.v[next].screenPos, plane);

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
      m_primatives.swap(splitBuffer);
      splitBuffer.clear();
   }

}


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// FILL TRIANGLE
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void cpuRenderObject::raster(const triangle3d& pr, const object& obj, const model::subMesh& mesh) {
   // Bounding box triangle filling method with barycentric coordinates to interpolate 
   // between points. This is the trickiest part of the pipeline
   const vertex& v0 = pr.v[0];
   const vertex& v1 = pr.v[1];
   const vertex& v2 = pr.v[2];
   // Screen-space positions
   vec2 p0(v0.screenPos.x, v0.screenPos.y);
   vec2 p1(v1.screenPos.x, v1.screenPos.y);
   vec2 p2(v2.screenPos.x, v2.screenPos.y);

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

   int xmin = std::max((int)std::min({ p0.x, p1.x, p2.x }), 1);
   int xmax = std::min((int)std::max({ p0.x, p1.x, p2.x }), (int)m_resolution.x - 1);
   int ymin = std::max((int)std::min({ p0.y, p1.y, p2.y }), 1);
   int ymax = std::min((int)std::max({ p0.y, p1.y, p2.y }), (int)m_resolution.y - 1);


   // --- Edge function setup
   // Cross product is used to get 2x the area of the triangle
   float area = (p2 - p0).cross(p1 - p0);
   if (area == 0.0f) return;
   float invArea = 1.0f / area;



   // Start with the barycentric formula for α at point P:
   //   α(P) = (AB × AP) / (AB × AC)
   //         = [(Bx - Ax)*(Py - Ay) - (By - Ay)*(Px - Ax)] / area
   //
   // Add 1 to Px for a change of 1 pixel in x:       Px       + 1
   //   α(P_new) = [(Bx - Ax)*(Py - Ay) - (By - Ay)*((Px - Ax) + 1)] / area
   //
   // Distribute -(By - Ay) over the sum:
   //   α(P_new) = [(Bx - Ax)*(Py - Ay) - (By - Ay)*(Px - Ax) - (By - Ay)*1] / area
   //
   // Separate the original α(P) term:
   //   α(P_new) = [(Bx - Ax)*(Py - Ay) - (By - Ay)*(Px - Ax)] / area - (By - Ay)/area
   //
   // Recognize the first fraction as α(P):
   //   α(P_new) = α(P) - (By - Ay) / area
   //   alphaDx = α(P_new) - α(P) = - (By - Ay) / area
   float alphaDx = (p2.y - p1.y) * invArea;
   float alphaDy = (p1.x - p2.x) * invArea;
   float betaDx  = (p0.y - p2.y) * invArea;
   float betaDy  = (p2.x - p0.x) * invArea;
   float gammaDx = (p1.y - p0.y) * invArea;
   float gammaDy = (p0.x - p1.x) * invArea;

   // If A and B are 2 points of a triangle and P is a point inside the triangle
   // the cross product of AB and AP gives us 2x the area of the triangle formed by the 3 points.
   // Dividing by 2x area of the triangle (AB cross AC) will give the weight of the ABP area of the
   // entire triangles area. if AB cross AP / AB cross AC = 0.3 then ABP takes up 30% of the triangle
   vec2 start(xmin + 0.5f, ymin + 0.5f);
   float alphaY = (start - p1).cross(p2 - p1) * invArea;
   float betaY  = (start - p2).cross(p0 - p2) * invArea;
   float gammaY = (start - p0).cross(p1 - p0) * invArea;

   // --- Approximate geometric normal from screen-space derivatives
   vec3 dFdx = (v0.fragPos * alphaDx + v1.fragPos * betaDx + v2.fragPos * gammaDx).xyz();
   vec3 dFdy = (v0.fragPos * alphaDy + v1.fragPos * betaDy + v2.fragPos * gammaDy).xyz();

   vec3 normal = dFdx.cross(dFdy).normal();

   // ======================================================================================
   // Raster loop
   // ======================================================================================

   for (int y = ymin; y <= ymax; ++y){
      float alphaXY = alphaY;
      float betaXY  = betaY;
      float gammaXY = gammaY;

      for (int x = xmin; x <= xmax; ++x){

         if (alphaXY >= 0.0f && betaXY >= 0.0f && gammaXY >= 0.0f){

            float depth = (alphaXY * ndcZ0 + betaXY  * ndcZ1 + gammaXY * ndcZ2) * 0.5f + 0.5f;
            int index = x + y * m_resolution.x;

            if (depth <= m_zBuffer[index]){
               
               // Perspective-correct attribute reconstruction
               float invW = 1.0f / (alphaXY * invW0 + betaXY  * invW1 + gammaXY * invW2);

               vec3 fragPos = (fragOverW0 * alphaXY + fragOverW1 * betaXY + fragOverW2 * gammaXY).xyz() * invW;

               vec4 texColor;

               if (mesh.textured) {
                  vec2 uv = (uvOverW0 * alphaXY + uvOverW1 * betaXY + uvOverW2 * gammaXY) * invW;

                  // Texture wrap
                  uv.x -= std::floor(uv.x);
                  uv.y -= std::floor(uv.y);
                  int tx = (int)(uv.x * mesh.tex.w);
                  int ty = (int)(uv.y * mesh.tex.h);
                  int texel = (ty * mesh.tex.w + tx) * mesh.tex.channels;

                  if (mesh.tex.channels >= 3) {
                     int a = 1;
                     if (mesh.tex.channels == 4) a = mesh.tex.data[texel + 3];
                     texColor = vec4(mesh.tex.data[texel + 0], mesh.tex.data[texel + 1], mesh.tex.data[texel + 2], a);
                  }
                  else { // grayscale
                     float shade = mesh.tex.data[texel] / 255.0f;
                     texColor = vec4(shade,shade,shade,1.0f);
                  }
               }
               else {
                  vec4 color = hexColorToRGB(obj.getColor());
                  texColor = vec4(color.x, color.y, color.z, color.w);
               }


               // Diffuse + constant ambient
               vec3 lighting(0,0,0);

               for (const light& l : m_lights){
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
         alphaXY += alphaDx;
         betaXY  += betaDx;
         gammaXY += gammaDx;
      }
      alphaY += alphaDy;
      betaY  += betaDy;
      gammaY += gammaDy;
   }
}



//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// HELPER FUNCTIONS
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

float cpuRenderObject::planeIntersect(const vec4& a, const vec4& b, const vec4& plane) {
   return plane.dot(a) / (plane.dot(a) - plane.dot(b));
}

bool cpuRenderObject::backFaceCulling(const triangle3d& tri) {
    // Use only X/Y in screen space.
    const vec2& a = tri.v[0].screenPos.xy();
    const vec2& b = tri.v[1].screenPos.xy();
    const vec2& c = tri.v[2].screenPos.xy();
    // Compute signed area (2D cross product)
    float area = (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]);
    // Area will return negative if the triangle has CCW winding
    return area < 0.0f;
}

int cpuRenderObject::wrap(int n, int max){
   if (n >= max) n -= max;
   return n;
}
