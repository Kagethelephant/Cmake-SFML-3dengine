#pragma once

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Headers
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#include <SFML/Graphics.hpp>
#include <string>
#include "utils/matrix.hpp"
#include "camera.hpp"
#include "polygon.hpp"


// !!!!! USE RESERVE FUNCTIONS TO PREVENT VECTOR GROWTH !!!!!!!
// !!!!! if you are going to pop values pop them from tail to prevent shifting data in vector

/// @brief 3D object with storing triangle mesh and the means to render it
class object3d {

public:

   object3d();

   /// @brief Object position and orientation variables
   vec3 position = vec3(0,0,0);
   vec3 direction = vec3(0,0,0);

   /// @brief Holds trianles or points for 3D mesh or point cloud
   std::vector<tri3d> mesh;

   /// @brief Update the rotation matrix of the object
   void update(){ m_matTransform = transformation_matrix(position.x, position.y, position.z, direction.x, direction.y, direction.z);}

   /// @brief Called externally to draw the triangles in the mesh 
   void draw(std::vector<std::uint8_t>& texture, sf::RenderTexture& tex, sf::Vector2u res, camera camera, sf::Color col);

   /// @brief Load an OBJ file
   void load(std::string fileName);



private:

   // Member variables: Matrices for rotating and projecting vertices/triangles
   mat4x4 m_matProj;
   mat4x4 m_matTransform;
   float m_aspectRatio;     
};

