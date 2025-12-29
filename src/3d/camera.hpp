#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include "matrix.hpp"
#include "polygon.hpp"
#include "obj3d.hpp"


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
   
   std::vector<tri3d> m_triangleBuffer;
   // Matrices for rotating and projecting vertices
   mat4x4 m_matProj;

   vec3 m_planes[6];

   // Aspect ratio of the window
   float m_aspectRatio;     

   sf::Color col;
   sf::Color lineCol;
   object3d parent;
  
   sf::Texture pixelBuff;
   sf::Vector2u resolution;
   std::vector<std::uint8_t> pixels;
   std::vector<std::uint8_t> bg;

   camera(sf::Vector2u res);

   // This updates all of the matrices based on the new position and rotation
   // This gets called by the move and rotate functions
   void update(float x, float y, float z, float u, float v, float w);

   void loadObject(object3d& object);

   /// @brief: Called externally to draw the triangles in the mesh 
   /// @param texture: std::vector of uint8_t representing a pixel array to be drawn to the screen
   /// @param tex: SFML texture to draw the pixel aray to 
   /// @param res: SFML 2d vector containing the resolution of the window
   /// @param camera: camera object that will be used to view the object to be drawn
   /// @param col: Color to draw the object (this will be the brightest color, 
   /// individual triangles will be shaded according to their orientation to the light)
   void draw(int layer = 0);

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
};
