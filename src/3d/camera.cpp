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
#include "polygon.hpp"
#include "matrix.hpp"
#include "utils.hpp"
#include "obj3d.hpp"


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// CONSTRUCTOR
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
camera::camera(sf::Vector2u res, float fov, sf::Color bgColor) {

   m_resolution = res;
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
   m_planes[4] = vec4(0,0,-1,n); // Near plane
   m_planes[5] = vec4(0,0,1,-far); // Far plane
   
   // Create the projection matrix that will be used to project 3D points to a 2D view
   m_matProject = matrix_project(fov,aspectRatio,n,far);  
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


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// LOAD NEW OBJECT AND SORT TRIANGLES
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void camera::loadObject(object3d& object){
   
   for(tri3d i: object.mesh) {
      // Call deep copy constructor to copy parent object3d pointer. Pointer would be ignored in default copy constructor
      tri3d tri = i;
      // Direction vector pointing from triangle to camera
      vec3 camToTri = (tri.v[0] - position).normal();
      // Use dot product to check if (left) triangle is facing the camera and (right) if it is in field of view, otherwise ignore
      if((camToTri * tri.normal() < 0.0f) && (camToTri * pointDirection > 0.0f)) {
         // Multiply the triangle by the view matrix to correct its position based on the camera angle
         tri *= m_matView;
         m_triangleBuffer.push_back(tri);
      }
   }
}


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// TRIANGLE CLIPPING
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
std::vector<tri3d> camera::clipTriangles(std::vector<tri3d> triangles) {
   
   // Clear the pixel buffer with the background color before drawing each triangle
   m_pixelBuffer = m_clearBuffer;
   m_zBuffer = std::vector<float>(m_resolution.x * m_resolution.y, far);
   // Create the direction of the light used to shade our triangles
   vec3 light = vec3(-1,1,-1).normal();
      
   // Create buffer to hold triangles that need to go through the split function and 
   // a buffer of triangles that have already been through the split function
   std::vector<tri3d> splitBuffer;
   // Cycle through each plane and each triangle so we can clip them against each plane
   for (vec4& plane : m_planes) {
      for(tri3d& t : triangles) {
         // For consiseness check what points are out of the current plane ahead of time
         bool clipped[3] = {pointOutOfPlane(t.v[0], plane), pointOutOfPlane(t.v[1], plane), pointOutOfPlane(t.v[2], plane)};
         // If all of the points are not clipped by this plane then pass along the triangle to the next step
         if (clipped[0]+clipped[1]+clipped[2] == 0){splitBuffer.push_back(t); continue;}

         // Cycle through all of the points to find the clipped points
         for(int i=0; i<3; i++){
            // Get the position of the next and last point clockwise from the current point
            int next = wrap(i+1,3);
            int last = wrap(i+2,3);
            // We make sure the last point is not clipped so we exclude triangles with all points out of the plane
            if (clipped[i] and not clipped[last]){
               // If there are 2 points and they are the current and next point
               if (clipped[next]){
                  // Find new points where the edges of the triangles intercect the plane
                  vec3 p1 = planeIntercect(t.v[i], t.v[last], plane);
                  vec3 p2 = planeIntercect(t.v[next], t.v[last], plane);
                  // Make a new triangle with the 2 new points and the one unclipped point
                  splitBuffer.push_back(tri3d(p1,p2,t.v[last],t.parent));
                  break;
               }
               // If there is only one point and it is the current point
               else {
                  // Find new points where the edges of the triangles intercect the plane
                  vec3 p1 = planeIntercect(t.v[i], t.v[last], plane);
                  vec3 p2 = planeIntercect(t.v[i], t.v[next], plane);
                  // Create 2 new triangles with the 2 new points and the 2 unclipped points
                  splitBuffer.push_back(tri3d(p1,p2,t.v[next],t.parent));
                  splitBuffer.push_back(tri3d(p1,t.v[next],t.v[last],t.parent));
                  break;
               }
            }
         }
      }
      // Pass triangles from the working buffer back into the loop for the next plane (and clear working buffer)
      triangles = splitBuffer;
      splitBuffer = std::vector<tri3d>();
   }
   return triangles;
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// TRIANGLE PROJECTION
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void camera::update() {

   // Clear the pixel buffer with the background color before drawing each triangle
   m_pixelBuffer = m_clearBuffer;
   m_zBuffer = std::vector<float>(m_resolution.x * m_resolution.y, far);
   // Create the direction of the light used to shade our triangles
   vec3 light = vec3(-1,1,-1).normal();

   // Clip triangles
   std::vector<tri3d> splitTriangles = clipTriangles(m_triangleBuffer);

   for(tri3d triangle : splitTriangles) {

      // GET SHADE OF TRIANGLE
      //---------------------------------------------------------------------------------------------
      // Determine shade of color based on the angle of the triangle face compared to light direction
      float shade = triangle.normal().dot(light);
      sf::Color shadeColor((triangle.parent->color.r/2.0)*(shade+1), (triangle.parent->color.g/2.0)*(shade+1), (triangle.parent->color.b/2.0)*(shade+1));
      
      // PROJECT 3D TRIANGLES TO SCREEN SPACE
      //---------------------------------------------------------------------------------------------
      // Use projection matrix to convert 3D points to 2D points in the camera view
      tri3d oldTri = triangle;
      triangle.v[0] *= m_matProject;
      triangle.v[1] *= m_matProject;
      triangle.v[2] *= m_matProject;

      for (int i=0; i<3; i++){
         // Projection results are between -1 and 1. So shift to the positive and scale to fit screen
         triangle.v[i][0] += 1;
         triangle.v[i][1] += 1;
         triangle.v[i][0] *= 0.5f * m_resolution.x;
         triangle.v[i][1] *= 0.5f * m_resolution.y;
      }
      triangle.draw(oldTri, m_pixelBuffer, m_zBuffer, m_resolution, shadeColor, triangle.parent->lineColor);
   }
   // Add pixel buffer data to the SFML texture so it can be drawn to the SFML window
   m_pixelTexture.update(m_pixelBuffer.data());
   // Clear the buffer of triangles for the next itteration
   m_triangleBuffer = std::vector<tri3d>();
}



//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// UTILITY FUNCTIONS
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
bool camera::pointOutOfPlane(vec3& point, vec4& plane){
   return ((point.dot(plane.xyz()) + plane[3]) > 0);
}

vec3 camera::planeIntercect(vec3& p0, vec3& p1, vec4& plane){
   // t is where the plane inersects the line. scale of 0-1, 0 being p0 and 1 being p1
   float t = (p0.dot(plane.xyz()) + plane[3]) / (plane[0]*(p0[0]-p1[0]) + plane[1]*(p0[1]-p1[1]) + plane[2]*(p0[2]-p1[2]));
   return((p0 + ((p1 - p0) * t)));
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
   position += (pointDirection.cross(up) * x);
   position[1] += y;
   rotation += vec3(u,v,w);

   // Generate the new "point at" matrix and "view" matrix based on the new orientation of the camera
   m_matPointAt = matrix_pointAt(position, pointDirection, up);
   m_matView = matrix_view(m_matPointAt);
}


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// SF::DRAWABLE IMPLIMENTATION
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void camera::draw(sf::RenderTarget& target, sf::RenderStates states) const {
   // Allow for an SFML render texture/window to draw the pixel buffer containing the 3D projections
   sf::Sprite tempSpr(m_pixelTexture);
   target.draw(tempSpr, states);
}
