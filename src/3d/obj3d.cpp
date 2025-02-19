
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
object3d::object3d() {
   
   u = 0, v = 0, w = 0 , x = 0 , y = 0 , z = 0;
   update();
   m_aspectRatio = ((float)sf::VideoMode::getDesktopMode().width) / ((float)sf::VideoMode::getDesktopMode().height);
   std::cout << m_aspectRatio << std::endl;

   m_matProj = project_matrix(90.0f,m_aspectRatio,0.1f,1000.0f);  
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void object3d::update(){ m_matTransform = transformation_matrix(x, y, z, u, v, w);}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void object3d::draw(sf::RenderTexture& texture, sf::Vector2i res, camera c, sf::Color col) {
   
   // This is some code that will create a test cube instead of a mesh
   // mesh.push_back(tri3d(vec3(-1.0f,-1.0f,0.0f),vec3(-1.0f,1.0f,0.0f),vec3(1.0f,-1.0f,0.0f)));
   // mesh.push_back(tri3d(vec3(1.0f,1.0f,0.0f),vec3(1.0f,-1.0f,0.0f),vec3(-1.0f,1.0f,0.0f)));

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

   // sort by z midpoint
   std::sort(buffer.begin(),buffer.end(), [](tri3d &t1, tri3d &t2) {
      // Check the z position of the midpoint to decide what triangles to draw first
      float z1 = (t1.v[0].z + t1.v[1].z + t1.v[2].z)/3.0f;
      float z2 = (t2.v[0].z + t2.v[1].z + t2.v[2].z)/3.0f;
      return z1 > z2;
   });


   //project and draw
   for(tri3d tri : buffer) {

      vec3 norm = tri.normal();
      float shade = norm.dot(light);

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

      // Get the color based on the light angle relitive to the face normal
      int r = (col.r/2.0)*(shade+1);
      int g = (col.g/2.0)*(shade+1);
      int b = (col.b/2.0)*(shade+1);

      tri.draw(texture, res, rgb(r,g,b));
   }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void object3d::load(std::string fileName) {

   std::ifstream obj(fileName);
   if (!obj.is_open()) {

      std::cout << "error: cannot load 3d object" << std::endl;
      return;
   }

   std::vector<vec3> verts;
   while(!obj.eof()) {

      char line[128];
      obj.getline(line,128);

      std::stringstream stream;
      stream << line;

      char junk;
      if(line[0] == 'v') {

         vec3 v;
         stream >> junk >> v.x >> v.y >> v.z;
         verts.push_back(v);
      }
      if(line[0] == 'f') {

         int f[3];
         stream >> junk >> f[0] >> f[1] >> f[2];
         mesh.push_back(tri3d(verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1]));
      }
   }
}





