//////////////////////////////////////////////////////////////////
// Header
//////////////////////////////////////////////////////////////////
#include "obj3d.hpp"
#include "3d/polygon.hpp"
#include "utils/matrix.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream> // Use this instead of strstream, strstream is depricated because it returns
#include <vector>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor for object, sets some default variables and creates the projection matrix
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
object3d::object3d() {

   update();
   m_aspectRatio = ((float)sf::VideoMode::getDesktopMode().width) / ((float)sf::VideoMode::getDesktopMode().height);
   m_matProj = project_matrix(90.0f,m_aspectRatio,0.1f,1000.0f);  
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Update the transformation matrix so the objects can move based on updated x,y,z,u,v,w values
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void object3d::update(){ m_matTransform = transformation_matrix(position.x, position.y, position.z, direction.x, direction.y, direction.z);}


bool pointOutOfPlane(vec3 point, vec3 plane){
   return ((point.dot(plane) + plane.w) < 0);
}

vec3 splitPoint(vec3 p1, vec3 p2, vec3 plane){
   float t = (p1.dot(plane) + plane.w) / (plane.x*(p1.x-p2.x) + plane.y*(p1.y-p2.y) + plane.z*(p1.z-p2.z));

   return((p1 + ((p2 - p1) * t)));
}

int wrap(int i, int limit){
   if (i >= limit) i -= limit;
   return i;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// This is the main function and probably needs to be broken up into more functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void object3d::draw(std::vector<sf::Uint8>& texture, sf::Texture tex, sf::Vector2i res, camera camera, vec3 color) {

   // ************************** MOVE AND ROTATE **************************
   update();

   // Create a vector for the camera and light direction (not position)
   vec3 light(-1,1,-1);
   light.normalize();

   std::vector<tri3d> buffer;
   for(tri3d i: mesh) {

      // Transform the objects position and rotation by multiplying it by
      // this objects transformation matrix populated by "update()"
      tri3d tri = i * m_matTransform;

      // ************************** CHECK VISABILITY, MOVE TO CAMERA **************************
      // Get the normal vector to the triangle and the vector pointing
      // from the camera to the triangle to determine if we should draw it
      vec3 triNorm = tri.normal();
      vec3 camToTri = tri.v[0] - camera.position;
      // This makes sure we only draw triangles are facing us and are in our line-of-sight 
      if((camToTri * triNorm < 0.1) && (camToTri * camera.direction > 0)) {

         tri *= camera.view;
         buffer.push_back(tri);
      }
   }

   // ************************** SORT TRIANGLE BY Z **************************
   // Sort all of the triangles in the buffer by their mmidpoint z value to draw the farther triangles first
   std::sort(buffer.begin(),buffer.end(), [](tri3d &t1, tri3d &t2) {
      float z1 = (t1.v[0].z + t1.v[1].z + t1.v[2].z)/3.0f;
      float z2 = (t2.v[0].z + t2.v[1].z + t2.v[2].z)/3.0f;
      return z1 > z2;
   });


   float n = 0.1f;
   float r = n * tanf(88.0f * 0.5f / 180.0f * 3.14159f);
   float t = r / m_aspectRatio;
   float f = 1000.0f;

   vec3 topRight = vec3(r,t,n,0).normal();
   vec3 topLeft = vec3(-r,t,n,0).normal();
   vec3 bottomRight = vec3(r,-t,n,0).normal();
   vec3 bottomLeft = vec3(-r,-t,n,0).normal();

   vec3 top = topLeft.cross(topRight).normal();
   vec3 bottom = bottomRight.cross(bottomLeft).normal();
   vec3 right = topRight.cross(bottomRight).normal();
   vec3 left = bottomLeft.cross(topLeft).normal();
   vec3 near = vec3(0,0,-1,n);
   vec3 far = vec3(0,0,1,-f);


   // vec3 bottom = vec3(0,1,0);
   // vec3 newPoint = splitPoint(vec3(-20.0f,-20.0f,0.0f), vec3(5.0f,40.0f,0.0f), vec3(1.0f,0.0f,0.0f, 1.0f));
   

   //project and draw
   for(tri3d tri : buffer) {

      // ************************** GET COLOR AND DRAW TRIANGLE **************************
      // Take the normal of the triangle and compare it to 
      // the direction of the light source to get the shade
      float shade = tri.normal().dot(light);
      int r = (color.x/2.0)*(shade+1);
      int g = (color.y/2.0)*(shade+1);
      int b = (color.z/2.0)*(shade+1);
      // if(tri.clippedToPlain(top, 0)) color = blue;
      // if(tri.clippedToPlain(bottom, 0)) color = red;
      // if(tri.clippedToPlain(right, 0)) color = yellow;
      // if(tri.clippedToPlain(left, 0)) color = green;

      std::vector<tri3d> splitBuffer;
      std::vector<tri3d> toSplitBuffer;
      vec3 planes[6] = {
         top,
         bottom,
         right,
         left,
         near,
         far
      };
      toSplitBuffer.push_back(tri);

      for (vec3 plane : planes) {
         for (tri3d t : toSplitBuffer){


            bool clipped[3] = {
               not pointOutOfPlane(t.v[0], plane),
               not pointOutOfPlane(t.v[1], plane),
               not pointOutOfPlane(t.v[2], plane)
            };

            int clipCount = clipped[0]+clipped[1]+clipped[2];
            vec3 p1,p2;
            int next,last;

            if (clipCount == 2) {
               for(int i=0; i<3; i++){
                  next = wrap(i+1,3);
                  if (clipped[i] and clipped[next]){
                     last = wrap(i+2,3);
                     p1 = splitPoint(t.v[i], t.v[last], plane);
                     p2 = splitPoint(t.v[next], t.v[last], plane);
                     splitBuffer.push_back(tri3d(p1,p2,t.v[last]));
                     break;
                  }
               }
            }
            else if (clipCount == 1) {
               for(int i=0; i<3; i++){
                  if (clipped[i]){
                     next = wrap(i+1,3);
                     last = wrap(i+2,3);
                     p1 = splitPoint(t.v[i], t.v[last], plane);
                     p2 = splitPoint(t.v[i], t.v[next], plane);
                     splitBuffer.push_back(tri3d(p1,p2,t.v[next]));
                     splitBuffer.push_back(tri3d(p1,t.v[next],t.v[last]));
                     break;
                  }
               }
            }
            else if (clipCount == 0){
               splitBuffer.push_back(t);
            }
         }
         toSplitBuffer = splitBuffer;
         splitBuffer = std::vector<tri3d>();
      }

      // if(tri.clippedToPlain(bottom, -1)) color = red;
      




      for (tri3d triangle : toSplitBuffer){
         // ************************** PROJECT, SHIFT, AND SCALE POINTS **************************
         // Use projection matrix to convert 3D points to 2D points on the screen
         triangle.v[0] *= m_matProj;
         triangle.v[1] *= m_matProj;
         triangle.v[2] *= m_matProj;


         // This is centering the view on the screen by scaling the aspect ratio
         // Get the color based on the light angle relitive to the face normal
         triangle.v[0].x += 1;
         triangle.v[0].y += 1;
         triangle.v[1].x += 1;
         triangle.v[1].y += 1;
         triangle.v[2].x += 1;
         triangle.v[2].y += 1;

         // This scales the x and y to position in the center of the screen
         triangle.v[0].x *= 0.5f * res.x;
         triangle.v[0].y *= 0.5f * res.y;
         triangle.v[1].x *= 0.5f * res.x;
         triangle.v[1].y *= 0.5f * res.y;
         triangle.v[2].x *= 0.5f * res.x;
         triangle.v[2].y *= 0.5f * res.y;

         // Draw the triangle after the projection is done
         triangle.draw(texture, tex, res, rgb(r,g,b,255));
      }
   }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load an OBJ file into the mesh vector
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void object3d::load(std::string fileName) {

   // Try to open the files
   std::ifstream obj(fileName);
   if (!obj.is_open()) {
      std::cout << "error: cannot load 3d object" << std::endl;
      return;
   }
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
         stream >> junk >> v.x >> v.y >> v.z;
         verts.push_back(v);
      }
      if(line[0] == 'f') {
         // If it is a triangle then get the corosponding vertices and load it into the mesh
         int f[3];
         stream >> junk >> f[0] >> f[1] >> f[2];
         mesh.push_back(tri3d(verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1]));
      }
   }
}





