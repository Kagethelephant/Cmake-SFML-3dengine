#include "obj3d.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <iostream>
#include <fstream>
#include <sstream> // Use this instead of strstream, strstream is depricated because it returns
#include <string>
#include <vector>
#include <math.h>
#include "polygon.hpp"
#include "matrix.hpp"


// CONSTRUCTOR
//---------------------------------------------------------------------------------------------
object3d::object3d(std::string filename, sf::Color col, sf::Color lineCol, vec3 pos) {

   if (!(filename == "")) {load(filename);}
   color = col;
   lineColor = lineCol;
   position = pos;
}

// LOAD OBJECT
//---------------------------------------------------------------------------------------------
void object3d::load(std::string fileName) {

   // Try to open the file
   std::ifstream obj(fileName);
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
         mesh.push_back(tri3d(verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1], this));
      }
   }
}
