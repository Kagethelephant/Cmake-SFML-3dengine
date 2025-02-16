
//////////////////////////////////////////////////////////////////
// Headers
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

   m_matProj = project_matrix(90.0f,m_aspectRatio,0.1f,1000.0f);  
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void object3d::update(){ m_matTransform = translate_matrix(x,y,z) * rotate_x_matrix(u) * rotate_y_matrix(v) * rotate_z_matrix(w);}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void object3d::draw(sf::RenderTexture& texture, sf::Vector2i res, camera c, sf::Color col) {

   update();
   // Create a vector for the camera and light direction (not position)
   vec3 light(-1,1,-1);
   light = light.norm();

   std::vector<tri3d> buffer;

   for(tri3d i: mesh) {

      tri3d triIn;

      triIn.v[0] = i.v[0] * m_matTransform;
      triIn.v[1] = i.v[1] * m_matTransform;
      triIn.v[2] = i.v[2] * m_matTransform;

      // Get the normal vector to the triangle
      vec3 triNorm = triIn.norm();
      vec3 camToTri = vec3(triIn.v[0].x-c.position.x,triIn.v[0].y-c.position.y,triIn.v[0].z-c.position.z);

      // This makes sure we only draw triangles are facing us and are in our line-of-sight 
      if((camToTri.dot(triNorm) < 0.1) && (camToTri.dot(c.direction) > 0)) {

         triIn.v[0] = triIn.v[0] * c.view;
         triIn.v[1] = triIn.v[1] * c.view;
         triIn.v[2] = triIn.v[2] * c.view;

         buffer.push_back(triIn);
      }
   }

   // sort by z midpoint
   std::sort(buffer.begin(),buffer.end(), [](tri3d &t1, tri3d &t2) {

      float z1 = (t1.v[0].z + t1.v[1].z + t1.v[2].z)/3.0f;
      float z2 = (t2.v[0].z + t2.v[1].z + t2.v[2].z)/3.0f;
      return z1 > z2;
   });

   int testCount = 0;

   //project and draw
   for(tri3d tri : buffer) {

      vec3 norm = tri.norm();
      float shade = norm.dot(light);


      for (int i = 0; i < 3; i++) {

         // Project 2D
         tri.v[i] = tri.v[i] * m_matProj;

         // Why i have to multiply this by the aspect ratio i have no idea
         // Center on screen
         tri.v[i].x += 1*m_aspectRatio;
         tri.v[i].y += 1/m_aspectRatio;

         // Why these are swapped i have no idea
         // Scale to size
         tri.v[i].x *= 0.5f * res.y;
         tri.v[i].y *= 0.5f * res.x;
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





