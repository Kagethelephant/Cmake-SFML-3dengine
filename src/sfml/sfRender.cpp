#include "sfRender.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <math.h>
#include "utils/data.hpp"
#include "utils/matrix.hpp"
#include "utils/utils.hpp"
#include "app/object.hpp"
#include "sfWindow.hpp"
#include <fstream>


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// CONSTRUCTOR
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
camera::camera(windowMaster& _window) : window{_window}{

   // Calculate the planes that make up the frustum (box that represents the field of view)
   m_resolution = window.resolution;
   float fov = 70;
   float aspectRatio = (float)m_resolution.x/(float)m_resolution.y;
   float n = 0.1f;
   far = 1000.0f;
  
   m_planes[0] = vec4( 1, 0, 0, 1); // Left
   m_planes[1] = vec4(-1, 0, 0, 1); // Right 
   m_planes[2] = vec4( 0, 1, 0, 1); // Bottom
   m_planes[3] = vec4( 0, -1, 0, 1); // Top 
   m_planes[4] = vec4( 0, 0, 1, 1); // Near // 
   m_planes[5] = vec4( 0, 0, -1, 1); // Far

   
   // Create the projection matrix that will be used to project 3D points to a 2D view
   m_matProject = matrix_project(fov,aspectRatio,n,far);  
   // Texture to draw our 3D stuff to an sfml window 
   m_pixelTexture = sf::Texture(m_resolution);

   // Create a background buffer the size of the window to clear the pixel buffer with a color
   m_clearBuffer = std::vector<std::uint8_t>(m_resolution.x * m_resolution.y * 4, 0);
   m_zBuffer = std::vector<float>(m_resolution.x * m_resolution.y, 1.0f);

   sf::Color bgColor(hexColorToSFML(Color::Black));
   int index = 0;
   for (int y = 0; y < m_resolution.y; y++)
   {
      for (int x = 0; x < m_resolution.x; x++)
      {
         index = (y * m_resolution.x + x);
         m_clearBuffer[index*4] = bgColor.r;
         m_clearBuffer[index*4 + 1] = bgColor.g;
         m_clearBuffer[index*4 + 2] = bgColor.b;
         m_clearBuffer[index*4 + 3] = bgColor.a;
      }
   }
   m_pixelBuffer = m_clearBuffer;
}

unsigned int camera::createModel(std::string filename, bool ccwWinding) {

   model newModel;

   int vertStart = m_vertices.size()/3;
   newModel.start = m_indices.size();
   // Try to open the file
   std::ifstream obj(filename);
   // Create an array to hold the chars of each line
   // cycle through all the lines in the file until we are at the end
   while(!obj.eof()) {
      // Create a char array to store the line from the file
      char line[128];
      obj.getline(line,128);
      // Pass the line from the file "stream" into the line
      std::stringstream stream;
      stream << line;
      // Check if the line is a vertice or a triangle
      char junk;
      if(line[0] == 'v') {
         // If it is a vertice then pull the xyz values from the string and put it in the vert array
         float x,y,z;
         stream >> junk >> x >> y >> z;
         m_vertices.push_back(x);
         m_vertices.push_back(y);
         m_vertices.push_back(z);
      }
      if(line[0] == 'f') {
         // If it is a triangle then get the corosponding vertices and load it into the mesh
         
         // If it is a triangle then get the corosponding vertices and load it into the mesh
         int f[3];
         if(ccwWinding){ stream >> junk >> f[0] >> f[2] >> f[1]; }
         else {          stream >> junk >> f[0] >> f[1] >> f[2]; }

         m_indices.push_back(f[0]+vertStart-1);
         m_indices.push_back(f[1]+vertStart-1);
         m_indices.push_back(f[2]+vertStart-1);
      }
   }

   newModel.size = m_indices.size()-newModel.start;
   models.push_back(newModel);
   return models.size()-1;
}




//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// TRIANGLE PROJECTION
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void camera::render(const object& object) {
  

   // Transform the light into viewspace
   lightPosView = (lightPos * m_matView);
   // Create the direction of the light used to shade our triangles
   tri3d color(hexColorToRGB(object.color), hexColorToRGB(object.color), hexColorToRGB(object.color));

   model mod = models[*object.model];
   // Model * View * Project matrix
   mat4x4 mvp = object.matScale * object.matTransform * m_matView * m_matProject;
   mat4x4 mv = object.matScale * object.matTransform * m_matView;

   std::vector<vec4> clipVertices;
   clipVertices.resize(m_vertices.size());

   std::vector<vec4> viewVertices;
   viewVertices.resize(m_vertices.size());

   // Backface Culling and viewspace transformation
   for(int i=mod.start; i<mod.start + mod.size; i ++){
      int index = m_indices[i] * 3;

      clipVertices[m_indices[i]] = (vec4(m_vertices[index], m_vertices[index+1], m_vertices[index+2]) * mvp);
      viewVertices[m_indices[i]] = (vec4(m_vertices[index], m_vertices[index+1], m_vertices[index+2]) * mv);
   }

   for(int i=mod.start; i<mod.start + mod.size; i += 3){
      int i0 = m_indices[i];
      int i1 = m_indices[i+1];
      int i2 = m_indices[i+2];
      tri3d clipTri(clipVertices[i0], clipVertices[i1], clipVertices[i2]);
      tri3d viewTri(viewVertices[i0], viewVertices[i1], viewVertices[i2]);
      m_triangleAttribs.push_back({clipTri, color, clipTri, viewTri});
   }
   // Clip triangles.
   clipTriangles();

   for(int i=0; i< m_triangleAttribs.size(); i++) {
      camera::triangleAttrib& attrib = m_triangleAttribs[i];
      tri3d& triangle = attrib.triangle;

      triangle.perspectiveDivide();

      for (int i=0; i<3; i++){
         // Projection results are between -1 and 1. So shift to the positive and scale to fit screen
         // Y in SFML is +y = down so we need to reverse the y direction
         triangle.v[i][0] = (triangle.v[i][0] + 1.0f) * 0.5f * m_resolution.x;
         triangle.v[i][1] = (1.0f - triangle.v[i][1]) * 0.5f * m_resolution.y;
      }

      if(!backFaceCulling(triangle)){ raster(attrib);}
   }
   // Clear the buffer of triangles for the next itteration
   m_triangleAttribs.clear();
}



//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// TRIANGLE CLIPPING
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void camera::clipTriangles() {
   
   // Create buffer to hold triangles that need to go through the split function and 
   // a buffer of triangles that have already been through the split function
   std::vector<triangleAttrib> splitBuffer;
   splitBuffer.reserve(m_triangleAttribs.size() * 2);
   // Cycle through each plane and each triangle so we can clip them against each plane
   int next, last;
   float t1, t2;
   vec4 p1, p2;

   for (vec4& plane : m_planes) {
      for(triangleAttrib& attrib : m_triangleAttribs) {
         tri3d& t = attrib.triangle;
         tri3d& c = attrib.color;
         tri3d& f = attrib.fragPos;
         // For consiseness check what points are out of the current plane ahead of time
         bool clipped[3] = {pointOutOfPlane(t.v[0], plane), pointOutOfPlane(t.v[1], plane), pointOutOfPlane(t.v[2], plane)};
         // If all of the points are not clipped by this plane then pass along the triangle to the next step
         if (clipped[0]+clipped[1]+clipped[2] == 0){splitBuffer.push_back(attrib); continue;}
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
                  t1 = planeIntersectT(t.v[i], t.v[last], plane);
                  t2 = planeIntersectT(t.v[next], t.v[last], plane);
                  //    return a + (b - a) * t;
                  p1 = t.v[i] + (t.v[last] - t.v[i]) * t1;
                  p2 = t.v[next] + (t.v[last] - t.v[next]) * t2;
                  tri3d tri1(p1,p2,t.v[last]);

                  p1 = c.v[i] + (c.v[last] - c.v[i]) * t1;
                  p2 = c.v[next] + (c.v[last] - c.v[next]) * t2;
                  tri3d color1(p1,p2,c.v[last]);

                  p1 = f.v[i] + (f.v[last] - f.v[i]) * t1;
                  p2 = f.v[next] + (f.v[last] - f.v[next]) * t2;
                  tri3d frag1(p1,p2,f.v[last]);
                  // Make a new triangle with the 2 new points and the one unclipped point
                  // emplace_back avoids making another copy like push_back
                  splitBuffer.push_back({tri1,color1,tri1, frag1});
                  break;
               }
               // If there is only one point and it is the current point
               else {
                  // Find new points where the edges of the triangles intercect the plane
                  t1 = planeIntersectT(t.v[i], t.v[last], plane);
                  t2 = planeIntersectT(t.v[i], t.v[next], plane);
                  // Create 2 new triangles with the 2 new points and the 2 unclipped points
                  p1 = t.v[i] + (t.v[last] - t.v[i]) * t1;
                  p2 = t.v[i] + (t.v[next] - t.v[i]) * t2;
                  tri3d tri1(p1,p2,t.v[next]);
                  tri3d tri2(p1,t.v[next],t.v[last]);

                  p1 = c.v[i] + (c.v[last] - c.v[i]) * t1;
                  p2 = c.v[i] + (c.v[next] - c.v[i]) * t2;
                  tri3d color1(p1,p2,c.v[next]);
                  tri3d color2(p1,c.v[next],c.v[last]);

                  p1 = f.v[i] + (f.v[last] - f.v[i]) * t1;
                  p2 = f.v[i] + (f.v[next] - f.v[i]) * t2;
                  tri3d frag1(p1,p2,f.v[next]);
                  tri3d frag2(p1,f.v[next],f.v[last]);

                  splitBuffer.push_back({tri1,color1,tri1, frag1});
                  splitBuffer.push_back({tri2,color2,tri2, frag2});
                  break;
               }
            }
         }
      }
      // Pass triangles from the working buffer back into the loop for the next plane (and clear working buffer)
      m_triangleAttribs.swap(splitBuffer);
      splitBuffer.clear();
   }
}




//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// FILL TRIANGLE
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void camera::raster(const camera::triangleAttrib& attrib) {
   // Bounding box triangle filling method with barycentric coordinates to interpolate 
   // between points. This is the trickiest part of the pipeline
   const tri3d& tri = attrib.triangle;
   const tri3d& clipTri = attrib.clipPos;
   const tri3d& color = attrib.color;

   // Screen-space positions
   vec2 p0(tri.v[0][0], tri.v[0][1]);
   vec2 p1(tri.v[1][0], tri.v[1][1]);
   vec2 p2(tri.v[2][0], tri.v[2][1]);
   // Calculate the inverse W value for interpolation on attributes
   float invW0 = 1/clipTri.v[0][3];
   float invW1 = 1/clipTri.v[1][3];
   float invW2 = 1/clipTri.v[2][3];
   // Calculate the NDC space Z value corrected with W for interpolation
   float ndcZ0 = clipTri.v[0][2] * invW0;
   float ndcZ1 = clipTri.v[1][2] * invW1;
   float ndcZ2 = clipTri.v[2][2] * invW2;
   // Calculate the fragPos / w for perspective correction
   vec4 fragOverW0 = attrib.fragPos.v[0] * invW0;
   vec4 fragOverW1 = attrib.fragPos.v[1] * invW1;
   vec4 fragOverW2 = attrib.fragPos.v[2] * invW2;
   // Calculate the fragPos / w for perspective correction
   vec4 colOverW0 = attrib.color.v[0] * invW0;
   vec4 colOverW1 = attrib.color.v[1] * invW1;
   vec4 colOverW2 = attrib.color.v[2] * invW2;

   // Get the coordinates of the rectangle that will cover the triangle (clamped to the buffer size)
   int xmax = std::min(std::max(std::max(tri.v[0][0], tri.v[1][0]), tri.v[2][0]), m_resolution.x - 1.0f);
   int xmin = std::max(std::min(std::min(tri.v[0][0], tri.v[1][0]), tri.v[2][0]), 1.0f);
   int ymax = std::min(std::max(std::max(tri.v[0][1], tri.v[1][1]), tri.v[2][1]), m_resolution.y - 1.0f);
   int ymin = std::max(std::min(std::min(tri.v[0][1], tri.v[1][1]), tri.v[2][1]), 1.0f);


   // For point p on a triangle: p = alpha*A + beta*B + gamma*C
   // A, B and C representing the points on triangle and alpha, beta and 
   // gamma representing weight of how close they are to the points (by areas)
   // we also have the constraint: alpha + beta + gamma = 1 or (alpha = 1 - beta - gamma)

   // Screen-space edge vectors originating from vertex A
   vec2 AB = (tri.v[1]-tri.v[0]).xy(); // B - A
   vec2 AC = (tri.v[2]-tri.v[0]).xy(); // C - A

   // These are used for the 2x2 system of linear equations used
   // to find the jacobian of the barycentric coordinates
   // [ AB_AB  AB_AC ] [ beta  ] = [ AP_AB ]
   // [ AB_AC  AC_AC ] [ gamma ]   [ AP_AC ]
   // This makes determinant = AB_AB * AC_AC - AB_AC * AB_AC
   float AB_AB = AB.dot(AB); 
   float AC_AC = AC.dot(AC);
   float AB_AC = AB.dot(AC);

   // Determinant for system of linear of equations
   float determ = AB_AB * AC_AC - AB_AC * AB_AC;
   float invDet = 1.0f / determ;

   // Delta in all barycentric coordinates for changes in x and y
   vec2 dBeta(  AC.dot(AC) * invDet, -AB.dot(AC) * invDet);
   vec2 dGamma(-AB.dot(AC) * invDet,  AB.dot(AB) * invDet);
   vec2 dAlpha = vec2(0,0) - dBeta - dGamma;

   // Partial derivitive of screenspace xy as it relates to viewspace coords (frag pos) or for
   // change of 1 pixel in x or y how much does the viewspace coord change
   vec3 dFdx_fragPos = (attrib.fragPos.v[0] * dAlpha[0] + attrib.fragPos.v[1] * dBeta[0] + attrib.fragPos.v[2] * dGamma[0]).xyz();
   vec3 dFdy_fragPos = (attrib.fragPos.v[0] * dAlpha[1] + attrib.fragPos.v[1] * dBeta[1] + attrib.fragPos.v[2] * dGamma[1]).xyz();

   // Since these are orthogonal vectors on the surface of the triangle
   // we can calculate the normal from these with the cross product
   vec3 norm = dFdx_fragPos.cross(dFdy_fragPos).normal();


   // Area of the triangle used to find barycentric weights in the loop
   float area = edgeFunction(p0, p1, p2);
   if (area == 0.0f) return;
   float invArea = 1.0f / area;

   // dx and dy represent how much the barycentric areas change
   // for a change in x or y of one pixel for each edge
   float E0_dx = p2[1] - p1[1];
   float E0_dy = p1[0] - p2[0];
   float E1_dx = p0[1] - p2[1];
   float E1_dy = p2[0] - p0[0];
   float E2_dx = p1[1] - p0[1];
   float E2_dy = p0[0] - p1[0];

   // +0.5.f is consistant with opengl pixel-center rules
   vec2 p(xmin + 0.5f, ymin + 0.5f);

   // Initial barycentric areas for the first pixel for each edge
   float E0_row = edgeFunction(p1, p2, p);
   float E1_row = edgeFunction(p2, p0, p);
   float E2_row = edgeFunction(p0, p1, p);

   // Allocate memory for value calculated in inner loop
   float alpha, beta, gamma, interpz,interpw, baryz;

   // Itterate through each pixel in that rectangle
   for (int y=ymin; y<=ymax; y++){

      // Get initial edge areas for the current row y (E_row is incremented below for change in y)
      float E0 = E0_row;
      float E1 = E1_row;
      float E2 = E2_row;

      for (int x=xmin; x<=xmax; x++){

         // Exit the loop if any of the barycentric areas are less than 0. This means the pixel is out of triangle
         if (E0 >= 0 && E1 >= 0 && E2 >= 0) {

            // Divide by the area of the triangle to get the barycentric weight
            alpha = E0 * invArea;
            beta  = E1 * invArea;
            gamma = E2 * invArea;

            // Find interpolated NDC z value with barycentric formula
            interpz = alpha * ndcZ0 + beta * ndcZ1 + gamma * ndcZ2; 
            baryz = interpz * 0.5f + 0.5f; // Convert NDC to depth buffer value [0 - 1]

            // Get the position of the pixel in the z-buffer and only draw a pixel if the pixel should be on top
            int index = (m_resolution.x * y + x);
            if (baryz <= m_zBuffer[index]){

               // Find interpolated W value for perspective correction on attributes (not x,y,z)
               interpw = alpha * invW0 + beta * invW1 + gamma * invW2; 

               vec3 fragPos = ((fragOverW0 * alpha + fragOverW1 * beta + fragOverW2 * gamma)/interpw).xyz();
               vec4 interpColor = ((colOverW0 * alpha + colOverW1 * beta + colOverW2 * gamma)/interpw);

               vec3 lightDir = (lightPosView - fragPos).normal();
               float diff = std::max(norm.dot(lightDir), 0.0f);

               vec3 ambient = lightCol * 0.2;
               vec3 diffuse = lightCol * diff;

               vec4 result = interpColor * vec4((ambient + diffuse),1.0f);
               result[0] = std::clamp(result[0], 0.0f, 255.0f);
               result[1] = std::clamp(result[1], 0.0f, 255.0f);
               result[2] = std::clamp(result[2], 0.0f, 255.0f);
               result[3] = std::clamp(result[3], 0.0f, 255.0f);

               m_pixelBuffer[index*4] = result[0];
               m_pixelBuffer[index*4 + 1] = result[1]; 
               m_pixelBuffer[index*4 + 2] = result[2]; 
               m_pixelBuffer[index*4 + 3] = result[3]; 
               m_zBuffer[index] = baryz; 
            }
         }
         
         // Move along the edge for the given x position using dx
         E0 += E0_dx;
         E1 += E1_dx;
         E2 += E2_dx;
      }

      // Move along the edge for the given y position using dy
      E0_row += E0_dy;
      E1_row += E1_dy;
      E2_row += E2_dy;
   }
}


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// SF::DRAWABLE IMPLIMENTATION
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void camera::draw() {
   // Add pixel buffer data to the SFML texture so it can be drawn to the SFML window
   m_pixelTexture.update(m_pixelBuffer.data());
   // Clear the pixel buffer with the background color before drawing each triangle
   m_pixelBuffer = m_clearBuffer;
   // Clear the z buffer with far depth
   m_zBuffer.assign(m_zBuffer.size(), 1.0f);
   // Allow for an SFML render texture/window to draw the pixel buffer containing the 3D projections
   sf::Sprite tempSpr(m_pixelTexture);
   window.draw(tempSpr);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// UTILITY FUNCTIONS
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
bool camera::pointOutOfPlane(const vec4& p, const vec4& plane){
   // plane.xyz = normal, plane.w = D
   return plane.dot(p) < 0.0f;
}

float camera::planeIntersectT(const vec4& a, const vec4& b, const vec4& plane) {
   return plane.dot(a) / (plane.dot(a) - plane.dot(b));
}
// vec4 camera::planeIntersect(const vec4& a, const vec4& b, const vec4& plane) {
//    float t = plane.dot(a) / (plane.dot(a) - plane.dot(b));
//    return a + (b - a) * t;
// }
float camera::edgeFunction(const vec2& a, const vec2& b, const vec2& p){
    return (p[0] - a[0]) * (b[1] - a[1]) - (p[1] - a[1]) * (b[0] - a[0]);
}

bool camera::backFaceCulling(const tri3d& tri) {
    // Use only X/Y in screen space.
    const vec2& a = tri.v[0].xy();
    const vec2& b = tri.v[1].xy();
    const vec2& c = tri.v[2].xy();
    // Compute signed area (2D cross product)
    float area = (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]);
    // Area will return negative if the triangle has CCW winding
    return area > 0.0f;
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// MOVE AND ROTATE CAMERA
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void camera::move(float x, float y, float z, float u, float v, float w) {
   
   // Get up / Y  and forward / Z component of the camera after rotating
   vec3 up = vec3(0,1,0) * matrix_transform(0, 0, 0, rotation[0], rotation[1], rotation[2]);
   pointDirection = (vec3(0,0,-1) * matrix_transform(0, 0, 0, rotation[0], rotation[1], rotation[2])).normal();

   // Adjust position of camera. Movement is based on camera direction. z means in and out x means side to side
   // xyz parameters are not world xyz. Position is world xyz so we need to translate
   position += (pointDirection * z);
   position += (up.cross(pointDirection) * x);
   position[1] += y;
   rotation += vec3(u,v,w);

   // Generate the new "point at" matrix and "view" matrix based on the new orientation of the camera
   m_matPointAt = matrix_pointAt(position, pointDirection, up);
   m_matView = matrix_view(m_matPointAt);
}


