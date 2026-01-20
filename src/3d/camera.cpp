#include "camera.hpp"

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
#include "data.hpp"
#include "polygon.hpp"
#include "matrix.hpp"
#include "utils.hpp"
#include "obj3d.hpp"
#include "window.hpp"
#include <fstream>


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// CONSTRUCTOR
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
camera::camera(windowMaster& _window) : window{_window}{

   m_resolution = window.resolution;
   float fov = 70;
   sf::Color bgColor(hexColorToSFML(Color::Black));
   // Calculate the planes that make up the frustum (box that represents the field of view)
   float aspectRatio = (float)m_resolution.x/(float)m_resolution.y;
   float n = 0.1f;
   far = 1000.0f;
   float border = 2.0f; // degrees to cutoff edge of view to visualize the clipping function 
   float halfFovRadians = tanf((fov - border) * (M_PI / 180.0f)/2); // Convert degrees to radians
   
   float t = halfFovRadians * n;
   float r = t * aspectRatio;

   vec4 topRight = vec4(r,t,n,0).normal();
   vec4 topLeft = vec4(-r,t,n,0).normal();
   vec4 bottomRight = vec4(r,-t,n,0).normal();
   vec4 bottomLeft = vec4(-r,-t,n,0).normal();

   m_planes[0] = topLeft.cross(topRight).normal(); // Top plane
   m_planes[1] = bottomRight.cross(bottomLeft).normal(); // Bottom plane
   m_planes[2] = topRight.cross(bottomRight).normal(); // Right plane
   m_planes[3] = bottomLeft.cross(topLeft).normal(); // Left plane
   m_planes[4] = vec4(0,0,-1,-n); // Near plane
   m_planes[5] = vec4(0,0,1,far); // Far plane
   
   // Create the projection matrix that will be used to project 3D points to a 2D view
   m_matProject = matrix_project(fov,aspectRatio,n,-far);  
   // Texture to draw our 3D stuff to an sfml window 
   m_pixelTexture = sf::Texture(m_resolution);

   // Create a background buffer the size of the window to clear the pixel buffer with a color
   m_clearBuffer = std::vector<std::uint8_t>(m_resolution.x * m_resolution.y * 4, 0);
   m_zBuffer = std::vector<float>(m_resolution.x * m_resolution.y, far);

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
   std::vector<vec3> verts;
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
         vec3 v;
         stream >> junk >> v[0] >> v[1] >> v[2];
         verts.push_back(v);
      }
      if(line[0] == 'f') {
         // If it is a triangle then get the corosponding vertices and load it into the mesh
         if(ccwWinding){
            int f[3];
            stream >> junk >> f[0] >> f[2] >> f[1];
            m_modelBuffer.push_back(tri3d(verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1]));
         }
         else {
            int f[3];
            stream >> junk >> f[0] >> f[1] >> f[2];
            m_modelBuffer.push_back(tri3d(verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1]));
         }
      }
   }

   newModel.size = m_modelBuffer.size()-newModel.start-1;
   models.push_back(newModel);


   std::cout << "New model created" << std::endl;
   std::cout << "Model Triangles Total: " << m_modelBuffer.size() << std::endl;
   std::cout << "Model Start: " << newModel.start << std::endl;
   std::cout << "Model Size: " << newModel.size << std::endl;
   std::cout << "Model End: " << newModel.start + newModel.size << std::endl;
   return models.size()-1;

}



//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// TRIANGLE CLIPPING
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void camera::clipTriangles(std::vector<tri3d>& triangles) {
   
   // Create buffer to hold triangles that need to go through the split function and 
   // a buffer of triangles that have already been through the split function
   std::vector<tri3d> splitBuffer;
   splitBuffer.reserve(triangles.size() * 2);
   // Cycle through each plane and each triangle so we can clip them against each plane
   int next, last;
   vec3 p1, p2;

   for (vec4& plane : m_planes) {
      for(tri3d& t : triangles) {
         // For consiseness check what points are out of the current plane ahead of time
         bool clipped[3] = {pointOutOfPlane(t.v[0], plane), pointOutOfPlane(t.v[1], plane), pointOutOfPlane(t.v[2], plane)};
         // If all of the points are not clipped by this plane then pass along the triangle to the next step
         if (clipped[0]+clipped[1]+clipped[2] == 0){splitBuffer.push_back(t); continue;}

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
                  p1 = planeIntercect(t.v[i], t.v[last], plane);
                  p2 = planeIntercect(t.v[next], t.v[last], plane);
                  // Make a new triangle with the 2 new points and the one unclipped point
                  // emplace_back avoids making another copy like push_back
                  splitBuffer.emplace_back(p1,p2,t.v[last]);
                  break;
               }
               // If there is only one point and it is the current point
               else {
                  // Find new points where the edges of the triangles intercect the plane
                  p1 = planeIntercect(t.v[i], t.v[last], plane);
                  p2 = planeIntercect(t.v[i], t.v[next], plane);
                  // Create 2 new triangles with the 2 new points and the 2 unclipped points
                  splitBuffer.emplace_back(p1,p2,t.v[next]);
                  splitBuffer.emplace_back(p1,t.v[next],t.v[last]);
                  break;
               }
            }
         }
      }
      // Pass triangles from the working buffer back into the loop for the next plane (and clear working buffer)
      triangles.swap(splitBuffer);
      splitBuffer.clear();
   }
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// TRIANGLE PROJECTION
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void camera::renderObject(object3d& object) {
  
   // Create the direction of the light used to shade our triangles




   sf::Color color = hexColorToSFML(object.color);

   model mod = models[*object.model];
   mat4x4 modelView = object.matScale * object.matTransform * m_matView;

   for(int i=mod.start; i<mod.start + mod.size; i++){
      tri3d triangle = m_modelBuffer[i] * modelView;
      if (backFaceCulling(triangle)) continue;
      m_triangleBuffer.emplace_back(triangle);
   }

   // Clip triangles
   clipTriangles(m_triangleBuffer);

   vec3 lightPosView = (lightPos * m_matView);

   for(tri3d& triangle : m_triangleBuffer) {

      // GET SHADE OF TRIANGLE
      //---------------------------------------------------------------------------------------------
      // Determine shade of color based on the angle of the triangle face compared to light direction
      
      vec3 ambient = lightCol * 0.2;
      
      vec3 norm = triangle.normal();

      vec3 lightDir = (lightPosView - triangle.v[0]).normal();
      float diff = std::max(norm.dot(lightDir), 0.0f);
      vec3 diffuse = lightCol * diff;

      vec3 result = hexColorToRGB(object.color).xyz() * (ambient + diffuse);
      result[0] = std::clamp(result[0], 0.0f, 255.0f);
      result[1] = std::clamp(result[1], 0.0f, 255.0f);
      result[2] = std::clamp(result[2], 0.0f, 255.0f);

      // float shade = triangle.normal().dot(lightPos);
      sf::Color shadeColor(
         static_cast<std::uint8_t>(result[0]),
         static_cast<std::uint8_t>(result[1]),
         static_cast<std::uint8_t>(result[2])
      );
      
      // PROJECT 3D TRIANGLES TO SCREEN SPACE
      //---------------------------------------------------------------------------------------------
      // Use projection matrix to convert 3D points to 2D points in the camera view
      triangle.v[0] *= m_matProject;
      triangle.v[1] *= m_matProject;
      triangle.v[2] *= m_matProject;


      for (int i=0; i<3; i++){
         // Projection results are between -1 and 1. So shift to the positive and scale to fit screen
         // X: left (-1) → right (+1)
         triangle.v[i][0] = (triangle.v[i][0] + 1.0f) * 0.5f * m_resolution.x;
         // Y: top (+1) → bottom (-1), then map to screen space
         triangle.v[i][1] = (1.0f - triangle.v[i][1]) * 0.5f * m_resolution.y;
      }
      triangle.draw(m_pixelBuffer, m_zBuffer, m_resolution, shadeColor);
   }
   // Clear the buffer of triangles for the next itteration
   m_triangleBuffer.clear();
}


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// SF::DRAWABLE IMPLIMENTATION
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void camera::draw() {
   // Add pixel buffer data to the SFML texture so it can be drawn to the SFML window
   m_pixelTexture.update(m_pixelBuffer.data());
   // Clear the pixel buffer with the background color before drawing each triangle
   m_pixelBuffer = m_clearBuffer;
   m_zBuffer.assign(m_zBuffer.size(), far);
   // Allow for an SFML render texture/window to draw the pixel buffer containing the 3D projections
   sf::Sprite tempSpr(m_pixelTexture);
   window.draw(tempSpr);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// UTILITY FUNCTIONS
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
bool camera::pointOutOfPlane(vec3& p, vec4& plane){
    // plane.xyz = normal, plane.w = D
    return (plane.xyz().dot(p) + plane[3]) < 0.0f;
}

vec3 camera::planeIntercect(vec3& p0, vec3& p1, vec4& plane){
   // t is where the plane inersects the line. scale of 0-1, 0 being p0 and 1 being p1
   float t = (p0.dot(plane.xyz()) + plane[3]) / (plane[0]*(p0[0]-p1[0]) + plane[1]*(p0[1]-p1[1]) + plane[2]*(p0[2]-p1[2]));
   return((p0 + ((p1 - p0) * t)));
}

bool camera::backFaceCulling(tri3d& tri){
   // Assumes triangle is in view space, so camera is at 0,0,0
   return (tri.normal().dot(tri.v[0] * -1) < 0.0f);
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


