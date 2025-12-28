#include "camera.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <algorithm>
#include <string>
#include <vector>
#include <math.h>
#include <iostream>
#include "polygon.hpp"
#include "matrix.hpp"
#include "utils.hpp"
#include "obj3d.hpp"



camera::camera() {

   sf::Vector2u size = sf::VideoMode::getDesktopMode().size;
   m_aspectRatio = ((float)size.x / (float)size.y);
   m_matProj = project_matrix(90.0f,m_aspectRatio,0.1f,1000.0f);  

   float n = 0.1f;
   float r = n * tanf(88.0f * 0.5f / 180.0f * 3.14159f);
   float t = r / m_aspectRatio;
   float f = 1000.0f;

   vec3 topRight = vec3(r,t,n,0).normal();
   vec3 topLeft = vec3(-r,t,n,0).normal();
   vec3 bottomRight = vec3(r,-t,n,0).normal();
   vec3 bottomLeft = vec3(-r,-t,n,0).normal();

   m_planes[0] = topLeft.cross(topRight).normal(); // Top plane
   m_planes[1] = bottomRight.cross(bottomLeft).normal(); // Bottom plane
   m_planes[2] = topRight.cross(bottomRight).normal(); // Right plane
   m_planes[3] = bottomLeft.cross(topLeft).normal(); // Left plane
   m_planes[4] = vec3(0,0,-1,n); // Near plane
   m_planes[5] = vec3(0,0,1,-f); // Far plane
}


// UTILITY FUNCTIONS
//---------------------------------------------------------------------------------------------
bool camera::pointOutOfPlane(vec3 point, vec3 plane){
   return ((point.dot(plane) + plane.w) > 0);
}

vec3 camera::splitPoint(vec3 p1, vec3 p2, vec3 plane){
   float t = (p1.dot(plane) + plane.w) / (plane.x*(p1.x-p2.x) + plane.y*(p1.y-p2.y) + plane.z*(p1.z-p2.z));
   return((p1 + ((p2 - p1) * t)));
}

void camera::update(float x, float y, float z, float u, float v, float w) {
   
   // Get the camera up and forward directions. We are only using the rotations in the transformation 
   // because we only care about the orientation for these 2 direction vectors
   // Y direction
   vec3 up = vec3(0,1,0) * transformation_matrix(0, 0, 0, rotation.x, rotation.y, rotation.z);
   // Z direction (direction camera is facing)
   direction = (vec3(0,0,1) * transformation_matrix(0, 0, 0, rotation.x, rotation.y, rotation.z)).normal();

   // The position will change relitive to the camera angle not the world orientation.
   // Z direction is the same as the camera direction so multiply it by the z delta and add it to the position 
   position += (direction * z);
   // X direction (left and right) is the normal of the forward and up direction so multiply 
   // the cross product of those 2 vectors by the x delta and add it to the position vector
   position += (direction.cross(up) * x);
   // Y is the axis we rotate around so we can add our y delta straight to the position
   position.y += y;
   // Add the rotation deltas to the rotation
   rotation += vec3(u,v,w);

   // Generate the new "point at" matrix and "view" matrix based on the new orientation of the camera
   point = point_matrix(position, direction, up);
   view = view_matrix(point);
}

void camera::loadObject(object3d& object){
   
   object.update();
   int count = 0;
   for(tri3d i: object.mesh) {
      // Create triangle moved into correct position in world (based on the object position)
      tri3d tri = i * object.m_matTransform;
      // Transfer the parent object because this doesnt copy over for some reason
      tri.parent = i.parent;

      // CHECK VISABILITY, MOVE TO CAMERA 
      //---------------------------------------------------------------------------------------------
      // Get the normal vector to the triangle
      vec3 camToTri = (tri.v[0] - position).normal();
      // First expression checks that the face is facing the oposite direction of the vector pointing from the camera to the triangle
      // Second expression checks that the camera is facing the same direction of the vector pointing from the camera to the triangle
      // This makes sure the triangle is facing us and in the view
      if((camToTri * tri.normal() < 0.0f) && (camToTri * direction > 0.0f)) {
         // Multiply the triangle by the view matrix to correct its position based on the camera angle
         tri *= view;
         m_triangleBuffer.push_back(tri);
      }
   }
   // SORT TRIANGLE BY Z
   //---------------------------------------------------------------------------------------------
   // Sort all of the triangles in the buffer by their mmidpoint z value to draw the farther triangles first
   std::sort(m_triangleBuffer.begin(),m_triangleBuffer.end(), [](tri3d &t1, tri3d &t2) {
      float z1 = (t1.v[0].z + t1.v[1].z + t1.v[2].z)/3.0f;
      float z2 = (t2.v[0].z + t2.v[1].z + t2.v[2].z)/3.0f;
      return z1 > z2;
   });
}



// DRAW FUNCTION
//---------------------------------------------------------------------------------------------
void camera::draw(std::vector<std::uint8_t>& texture, sf::Vector2u res) {
   
   //project and draw
      
   // std::cout << std::to_string(col.r) + ", " + std::to_string(col.g) + ", " + std::to_string(col.b) << std::endl;
   for(tri3d tri : m_triangleBuffer) {

      std::vector<tri3d> splitBuffer;
      std::vector<tri3d> toSplitBuffer;
      toSplitBuffer.push_back(tri);

      // Cycle through each plane and each triangle so we can clip them against each plane
      for (vec3 plane : m_planes) {
         for (tri3d t : toSplitBuffer){

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
                     vec3 p1 = splitPoint(t.v[i], t.v[last], plane);
                     vec3 p2 = splitPoint(t.v[next], t.v[last], plane);
                     // Make a new triangle with the 2 new points and the one unclipped point
                     splitBuffer.push_back(tri3d(p1,p2,t.v[last]));
                     break;
                  }
                  // If there is only one point and it is the current point
                  else {
                     // Find new points where the edges of the triangles intercect the plane
                     vec3 p1 = splitPoint(t.v[i], t.v[last], plane);
                     vec3 p2 = splitPoint(t.v[i], t.v[next], plane);
                     // Create 2 new triangles with the 2 new points and the 2 unclipped points
                     splitBuffer.push_back(tri3d(p1,p2,t.v[next]));
                     splitBuffer.push_back(tri3d(p1,t.v[next],t.v[last]));
                     break;
                  }
               }
            }
         }
         // Pass triangles from the working buffer back into the loop for the next plane (and clear working buffer)
         toSplitBuffer = splitBuffer;
         splitBuffer = std::vector<tri3d>();
      }

      col = tri.parent->color;
      lineCol = tri.parent->lineColor;
      // Create a vector for the light direction
      vec3 light = vec3(-1,1,-1).normal();
      // Calculate how directly the light would hit the triangle face with the dot product of the 
      // triangle normal and light direction. Use this shade value to alter the provided color
      float shade = tri.normal().dot(light);
      sf::Color shadeColor((col.r/2.0)*(shade+1), (col.g/2.0)*(shade+1), (col.b/2.0)*(shade+1));

      // PROJECT, SHIFT, AND SCALE POINTS
      //---------------------------------------------------------------------------------------------
      for (tri3d triangle : toSplitBuffer){
         // Use projection matrix to convert 3D points to 2D points on the screen
         triangle.v[0] *= m_matProj;
         triangle.v[1] *= m_matProj;
         triangle.v[2] *= m_matProj;

         for (int i=0; i<3; i++){
            // This is centering the view on the screen by scaling the aspect ratio
            triangle.v[i].x += 1;
            triangle.v[i].y += 1;
            // This scales the x and y to position in the center of the screen
            triangle.v[i].x *= 0.5f * res.x;
            triangle.v[i].y *= 0.5f * res.y;
         }
         // Draw the triangle after the projection is done
         triangle.draw(texture,res, shadeColor, lineCol);
      }
   }
   m_triangleBuffer = std::vector<tri3d>();
}


