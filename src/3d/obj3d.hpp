#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "matrix.hpp"
#include "polygon.hpp"



// !!!!! USE RESERVE FUNCTIONS TO PREVENT VECTOR GROWTH !!!!!!!
// !!!!! if you are going to pop values pop them from tail to prevent shifting data in vector

/// @brief: 3D object with storing triangle mesh and the means to render it
class object3d {

public:
   
   /// Object origin position
   vec3 position = vec3(0,0,0);
   /// Object direction in degrees
   vec3 direction = vec3(0,0,0);

   sf::Color color;
   sf::Color lineColor;


   mat4x4 m_matTransform;
   /// Stores the mesh of tiangles loaded from the OBJ file
   std::vector<tri3d> mesh;

   // Constructor
   object3d(std::string filename = "", sf::Color col = sf::Color::White, sf::Color lineCol = sf::Color::Transparent, vec3 pos = vec3(0,0,0));

   /// @brief: Load an OBJ file. OBJ file must only contain triangle polygons and no textures
   /// @param filename: filename and directory of the obj file
   void load(std::string fileName);

   /// @brief: Update the rotation matrix of the object with the member position and direction variables
   void update(){ m_matTransform = transformation_matrix(position.x, position.y, position.z, direction.x, direction.y, direction.z);}

private:


};
