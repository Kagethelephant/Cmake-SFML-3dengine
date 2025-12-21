#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "matrix.hpp"
#include "polygon.hpp"


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
/// \brief 3d camera
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
class camera {

public:
   
   // Position along x, y, z
   // Rotation about x, y, z (u-pitch, v-roll, w-yaw)
   vec3 position = vec3(0,0,-5);
   vec3 rotation = vec3(0,0,0);
   
   // Vector storing the forward direction
   vec3 direction;

   // The point matrix is just used to create the view matrix
   // but the view matrix is used by all objects to put themselvs in the view
   mat4x4 point; 
   mat4x4 view; 
   
   // This updates all of the matrices based on the new position and rotation
   // This gets called by the move and rotate functions
   void update(float x, float y, float z, float u, float v, float w);

private:

};
// !!!!! USE RESERVE FUNCTIONS TO PREVENT VECTOR GROWTH !!!!!!!
// !!!!! if you are going to pop values pop them from tail to prevent shifting data in vector

/// @brief: 3D object with storing triangle mesh and the means to render it
class object3d {

public:

   object3d();

   /// Object origin position
   vec3 position = vec3(0,0,0);
   /// Object direction in degrees
   vec3 direction = vec3(0,0,0);

   /// Stores the mesh of tiangles loaded from the OBJ file
   std::vector<tri3d> mesh;

   /// @brief: Load an OBJ file. OBJ file must only contain triangle polygons and no textures
   /// @param filename: filename and directory of the obj file
   void load(std::string fileName);

   /// @brief: Update the rotation matrix of the object with the member position and direction variables
   void update(){ m_matTransform = transformation_matrix(position.x, position.y, position.z, direction.x, direction.y, direction.z);}

   /// @brief: Called externally to draw the triangles in the mesh 
   /// @param texture: std::vector of uint8_t representing a pixel array to be drawn to the screen
   /// @param tex: SFML texture to draw the pixel aray to 
   /// @param res: SFML 2d vector containing the resolution of the window
   /// @param camera: camera object that will be used to view the object to be drawn
   /// @param col: Color to draw the object (this will be the brightest color, 
   /// individual triangles will be shaded according to their orientation to the light)
   void draw(std::vector<std::uint8_t>& texture, sf::RenderTexture& tex, sf::Vector2u res, camera camera, sf::Color col);

private:

   /// @brief: Checks if a point is on one side of a plane
   /// @param point: Point in 3d space
   /// @param plain: Plain in 3d space represented by its normal vecor
   bool pointOutOfPlane(vec3 point, vec3 plane);

   /// @brief: Used to get the position on a line betweeen 2 3d points where 
   /// that line intersects the given plane
   /// @param p1: Point in 3d space
   /// @param p2: 2nd point in 3d space to create a theoretical line with the 1st point
   /// @param plane: Plain in 3d space that intersects the theoretical line
   vec3 splitPoint(vec3 p1, vec3 p2, vec3 plane);

   // Matrices for rotating and projecting vertices
   mat4x4 m_matProj;
   mat4x4 m_matTransform;

   // Aspect ratio of the window
   float m_aspectRatio;     
};




