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
   m_zBuffer = std::vector<float>(m_resolution.x * m_resolution.y, far);

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

   newModel.start = m_modelBuffer.size();
   // Try to open the file
   std::ifstream obj(filename);
   // Create an array to hold the chars of each line
   // cycle through all the lines in the file until we are at the end
   std::vector<vec4> tris;
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
         vec4 v;
         stream >> junk >> v[0] >> v[1] >> v[2];
         tris.push_back(v);
      }
      if(line[0] == 'f') {
         // If it is a triangle then get the corosponding vertices and load it into the mesh
         if(ccwWinding){
            int f[3];
            stream >> junk >> f[0] >> f[2] >> f[1];
            m_modelBuffer.push_back(tri3d(tris[f[0] - 1], tris[f[1] - 1], tris[f[2] - 1]));
         }
         else {
            int f[3];
            stream >> junk >> f[0] >> f[1] >> f[2];
            m_modelBuffer.push_back(tri3d(tris[f[0] - 1], tris[f[1] - 1], tris[f[2] - 1]));
         }
      }
   }

   newModel.size = m_modelBuffer.size()-newModel.start-1;
   models.push_back(newModel);

   return models.size()-1;

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
   vec4 p1, p2;

   int pl = 0;
   int clippedP = 0;
   for (vec4& plane : m_planes) {
      for(triangleAttrib& attrib : m_triangleAttribs) {
         tri3d& t = attrib.triangle;
         // For consiseness check what points are out of the current plane ahead of time
         bool clipped[3] = {pointOutOfPlane(t.v[0], plane), pointOutOfPlane(t.v[1], plane), pointOutOfPlane(t.v[2], plane)};
         // If all of the points are not clipped by this plane then pass along the triangle to the next step
         if (clipped[0]+clipped[1]+clipped[2] == 0){splitBuffer.push_back(attrib); continue;}
         if (clipped[0]+clipped[1]+clipped[2] == 3){
            if (clippedP != pl) {
               clippedP = pl;
               // std::cout << "Clipped: " << pl << std::endl;
            }
            continue;
         }

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
                  p1 = planeIntersect(t.v[i], t.v[last], plane);
                  p2 = planeIntersect(t.v[next], t.v[last], plane);
                  // Make a new triangle with the 2 new points and the one unclipped point
                  // emplace_back avoids making another copy like push_back
                  splitBuffer.push_back({tri3d(p1,p2,t.v[last]),attrib.color});
                  break;
               }
               // If there is only one point and it is the current point
               else {
                  // Find new points where the edges of the triangles intercect the plane
                  p1 = planeIntersect(t.v[i], t.v[last], plane);
                  p2 = planeIntersect(t.v[i], t.v[next], plane);
                  // Create 2 new triangles with the 2 new points and the 2 unclipped points
                  splitBuffer.push_back({tri3d(p1,p2,t.v[next]),attrib.color});
                  splitBuffer.push_back({tri3d(p1,t.v[next],t.v[last]),attrib.color});
                  break;
               }
            }
         }
      }
      pl ++;
      // Pass triangles from the working buffer back into the loop for the next plane (and clear working buffer)
      m_triangleAttribs.swap(splitBuffer);
      splitBuffer.clear();
   }
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// TRIANGLE PROJECTION
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void camera::render(object& object) {
  
   // Create the direction of the light used to shade our triangles
   sf::Color color = hexColorToSFML(object.color);
   // Transform the light into viewspace
   vec3 lightPosView = (lightPos * m_matView);

   model mod = models[*object.model];
   mat4x4 modelView = object.matScale * object.matTransform * m_matView;

   // Backface Culling and viewspace transformation
   for(int i=mod.start; i<mod.start + mod.size; i++){
      tri3d triangle = m_modelBuffer[i] * modelView;

      // GET SHADE OF TRIANGLE
      //---------------------------------------------------------------------------------------------
      // Determine shade of color based on the angle of the triangle face compared to light direction
      vec3 ambient = lightCol * 0.2;
      
      vec3 norm = triangle.normal().xyz();

      vec3 lightDir = (lightPosView - triangle.v[0].xyz()).normal();
      float diff = std::max(norm.dot(lightDir), 0.0f);
      vec3 diffuse = lightCol * diff;

      vec3 result = hexColorToRGB(object.color).xyz() * (ambient + diffuse);
      result[0] = std::clamp(result[0], 0.0f, 255.0f);
      result[1] = std::clamp(result[1], 0.0f, 255.0f);
      result[2] = std::clamp(result[2], 0.0f, 255.0f);

      // float shade = triangle.normal().dot(lightPos);
      sf::Color color(
         static_cast<std::uint8_t>(result[0]),
         static_cast<std::uint8_t>(result[1]),
         static_cast<std::uint8_t>(result[2])
      );

      triangle *= m_matProject;
      m_triangleAttribs.push_back({triangle, color, triangle});
   }

   // Clip triangles.
   clipTriangles();

   for(int i=0; i< m_triangleAttribs.size(); i++) {
      camera::triangleAttrib& attrib = m_triangleAttribs[i];
      tri3d& triangle = attrib.triangle;

      triangle.perspectiveDivide();
      if(!backFaceCulling(triangle)){

         for (int i=0; i<3; i++){
            // Projection results are between -1 and 1. So shift to the positive and scale to fit screen
            // Y in SFML is +y = down so we need to reverse the y direction
            triangle.v[i][0] = (triangle.v[i][0] + 1.0f) * 0.5f * m_resolution.x;
            triangle.v[i][1] = (1.0f - triangle.v[i][1]) * 0.5f * m_resolution.y;
         }
         raster(attrib);
      }
   }
   // Clear the buffer of triangles for the next itteration
   m_triangleAttribs.clear();
}


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// FILL TRIANGLE
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void camera::raster(camera::triangleAttrib& attrib) {
   // Bounding box triangle filling method with barycentric coordinates to interpolate 
   // between points (only used to interpolate z)
   tri3d& tri = attrib.triangle;
   tri3d& clipTri = attrib.clipSpace;
   sf::Color& color = attrib.color;

   // Calculate the NDC space Z value corrected with W for interpolation
   float ndcZ0 = clipTri.v[0][2] * 1/clipTri.v[0][3];
   float ndcZ1 = clipTri.v[1][2] * 1/clipTri.v[1][3];
   float ndcZ2 = clipTri.v[2][2] * 1/clipTri.v[2][3];
   // Calculate the inverse W value for interpolation
   // float invW0 = 1/clipTri.v[0][3];
   // float invW1 = 1/clipTri.v[1][3];
   // float invW2 = 1/clipTri.v[2][3];

   // Get the coordinates of the rectangle that will cover the triangle (clamped to the buffer size)
   int xmax = std::min(std::max(std::max(tri.v[0][0], tri.v[1][0]), tri.v[2][0]), m_resolution.x - 1.0f);
   int xmin = std::max(std::min(std::min(tri.v[0][0], tri.v[1][0]), tri.v[2][0]), 1.0f);
   int ymax = std::min(std::max(std::max(tri.v[0][1], tri.v[1][1]), tri.v[2][1]), m_resolution.y - 1.0f);
   int ymin = std::max(std::min(std::min(tri.v[0][1], tri.v[1][1]), tri.v[2][1]), 1.0f);

   // For point p on a triangle: p = alpha*A + beta*B + gamma*C
   // A, B and C representing the points and alpha, beta and gamma representing weight
   // of how close they are to the points as a percentage
   // we also have the constraint: alpha + beta + gamma = 1 or (alpha = 1 - beta - gamma)
   // Subsitute: p = (1 - beta - gamma)*A + beta*B + gamma*C
   // Simplify: (p - A) = beta*(B - A) + gamma*(C - A)

   // Create the vectors difined above
   vec2 AB = (tri.v[1]-tri.v[0]).xy(); // B - A
   vec2 AC = (tri.v[2]-tri.v[0]).xy(); // C - A

   // These are used for the 2x2 system of linear equations to find beta and gamma
   // [ AB_AB  AB_AC ] [ beta  ] = [ AP_AB ]
   // [ AB_AC  AC_AC ] [ gamma ]   [ AP_AC ]
   // This makes determinant = AB_AB * AC_AC - AB_AC * AB_AC
   // beta  = (AC_AC * AP_AB - AB_AC * AP_AC) / determinant
   // gamma = (AB_AB * AP_AC - AB_AC * AP_AB) / determinant
   float AB_AB = AB.dot(AB); 
   float AC_AC = AC.dot(AC);
   float AB_AC = AB.dot(AC);

   // Determinant for system of linear of equations
   float determ = AB_AB * AC_AC - AB_AC * AB_AC;

   // Allocate memory for value calculated in inner loop
   float AP_AB, AP_AC, alpha, beta, gamma, interpz,interpw, baryz;
   vec2 AP;

   // Itterate through each pixel in that rectangle
   for (int y=ymin; y<=ymax; y++){
      for (int x=xmin; x<=xmax; x++){

         // Generate the remainder of the values for the system of equations
         AP = vec2(x-tri.v[0][0], y-tri.v[0][1]); // P - A
         AP_AB = AP.dot(AB);
         AP_AC = AP.dot(AC);
        
         // Outputs of barycentric coords (escencially a percentage of how close the point is to each vertice)
         // This is solved with cramers rule for a 2x2 system of linear equations above
         beta  = (AC_AC * AP_AB - AB_AC * AP_AC)/ determ;
         gamma = (AB_AB * AP_AC - AB_AC * AP_AB)/ determ;
         alpha = 1.0f - beta - gamma;

         // Exit the loop if the alpha beta or gamma are less than 0. This means the pixel is out of triangle
         if (alpha < 0 || beta < 0 || gamma < 0) continue;

         // Insert the calculated coeficients into the barycentric coord formula for just the 
         // z values to find the depth of the pixel. This can also be used for texture coords
         interpz = alpha * ndcZ0 + beta * ndcZ1 + gamma * ndcZ2; 
         // interpw = alpha * invW0 + beta * invW1 + gamma * invW2; 
         baryz = interpz ; // Convert NDC to depth buffer value [0 - 1]

         // Get the position of the pixel in the z-buffer and only draw a pixel if the pixel should be on top
         int index = (m_resolution.x * y + x);
         if (baryz <= m_zBuffer[index]){
            m_pixelBuffer[index*4] = color.r;
            m_pixelBuffer[index*4 + 1] = color.g; 
            m_pixelBuffer[index*4 + 2] = color.b; 
            m_pixelBuffer[index*4 + 3] = color.a; 
            m_zBuffer[index] = baryz; 
         }
      }
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
   m_zBuffer.assign(m_zBuffer.size(), far);
   // Allow for an SFML render texture/window to draw the pixel buffer containing the 3D projections
   sf::Sprite tempSpr(m_pixelTexture);
   window.draw(tempSpr);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// UTILITY FUNCTIONS
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
bool camera::pointOutOfPlane(vec4& p, vec4& plane){
   // plane.xyz = normal, plane.w = D
   return plane.dot(p) < 0.0f;
}

vec4 camera::planeIntersect(vec4& a, vec4& b, vec4& plane) {
   float t = plane.dot(a) / (plane.dot(a) - plane.dot(b));
   return a + (b - a) * t;
}


bool camera::backFaceCulling(const tri3d& tri) {
    // Use only X/Y in projected space
    const vec2& a = tri.v[0].xy();
    const vec2& b = tri.v[1].xy();
    const vec2& c = tri.v[2].xy();
    // Compute signed area (2D cross product)
    float area =
        (b[0] - a[0]) * (c[1] - a[1]) -
        (b[1] - a[1]) * (c[0] - a[0]);
    // Assuming CCW = front-facing
    return area <= 0.0f;
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


