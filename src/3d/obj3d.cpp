//////////////////////////////////////////////////////////////////
// Header
//////////////////////////////////////////////////////////////////
#include "obj3d.hpp"
#include "utils/matrix.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream> // Use this instead of strstream, strstream is depricated because it returns


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor for object, sets some default variables and creates the projection matrix
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
object3d::object3d() {
   
   u = 0, v = 0, w = 0 , x = 0 , y = 0 , z = 0;
   update();
   m_aspectRatio = ((float)sf::VideoMode::getDesktopMode().width) / ((float)sf::VideoMode::getDesktopMode().height);
   std::cout << m_aspectRatio << std::endl;

   m_matProj = project_matrix(90.0f,m_aspectRatio,0.1f,1000.0f);  
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Update the transformation matrix so the objects can move based on updated x,y,z,u,v,w values
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void object3d::update(){ m_matTransform = transformation_matrix(x, y, z, u, v, w);}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// This is the main function and probably needs to be broken up into more functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void object3d::draw(sf::RenderTexture& texture, sf::Vector2i res, camera c, sf::Color col) {
   
   // This is some code that will create a test cube instead of a mesh
   // mesh.push_back(tri3d(vec3(-1.0f,-1.0f,0.0f),vec3(-1.0f,1.0f,0.0f),vec3(1.0f,-1.0f,0.0f)));
   // mesh.push_back(tri3d(vec3(1.0f,1.0f,0.0f),vec3(1.0f,-1.0f,0.0f),vec3(-1.0f,1.0f,0.0f)));

   // ************************** MOVE AND ROTATE **************************
   update();

   // Create a vector for the camera and light direction (not position)
   vec3 light(-1,1,-1);
   light.normalize();

   std::vector<tri3d> buffer;
   for(tri3d i: mesh) {
      
      // Transform the objects position and rotation by multiplying it by
      // this objects transformation matrix populated by "update()"
      tri3d tri = i;
      tri.v[0] *= m_matTransform;
      tri.v[1] *= m_matTransform;
      tri.v[2] *= m_matTransform;


      // ************************** CHECK VISABILITY, MOVE TO CAMERA **************************
      // Get the normal vector to the triangle and the vector pointing
      // from the camera to the triangle to determine if we should draw it
      vec3 triNorm = tri.normal();
      vec3 camToTri = vec3(tri.v[0].x-c.position.x,tri.v[0].y-c.position.y,tri.v[0].z-c.position.z);
      // This makes sure we only draw triangles are facing us and are in our line-of-sight 
      if((camToTri.dot(triNorm) < 0.1) && (camToTri.dot(c.direction) > 0)) {

         tri.v[0] *= c.view;
         tri.v[1] *= c.view;
         tri.v[2] *= c.view;
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


   //project and draw
   for(tri3d tri : buffer) {

   // ************************** PROJECT, SHIFT, AND SCALE POINTS **************************
      for (int i = 0; i < 3; i++) {
         // Project 2D
         tri.v[i] *= m_matProj;
         // This is centering the view on the screen by scaling the aspect ratio
         tri.v[i].x += 1;
         tri.v[i].y += 1;
         // This scales the x and y to position in the center of the screen
         tri.v[i].x *= 0.5f * res.x;
         tri.v[i].y *= 0.5f * res.y;
      }

   // ************************** GET COLOR AND DRAW TRIANGLE **************************
      // Take the normal of the triangle and compare it to 
      // the direction of the light source to get the shade
      float shade = tri.normal().dot(light);
      // Get the color based on the light angle relitive to the face normal
      int r = (col.r/2.0)*(shade+1);
      int g = (col.g/2.0)*(shade+1);
      int b = (col.b/2.0)*(shade+1);

      tri.draw(texture, res, rgb(r,g,b));
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





