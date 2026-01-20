#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include "data.hpp"
#include "matrix.hpp"
#include "polygon.hpp"


/// @brief: 3D object with storing triangle mesh and the means to render it
/// @param filename: Relitive file path of the obj file to load
/// @param _position: Starting position of the object (default {0,0,0})
/// @param _scale: Scale to apply to the object (default {1,1,1})
/// @param _color: Color to draw the object (will be shaded by camera object) (default: white)
/// @param _lineColor: Color to draw the outline of the triangles (defualt: transperant)
class object3d {

public:
   
   /// @brief: Coordinates of the object origin in 3D space
   vec3 position = vec3(0,0,0);
   /// @brief: Rotation from original orientation in radians
   vec3 rotation = vec3(0,0,0);
   /// @brief: Scale of the object
   vec3 scales = vec3(1,1,1);
   /// @brief: Winding of triangle mesh. default clockwise
   bool ccwWinding = false;

   mat4x4 matTransform = matrix_transform(position[0], position[1], position[2], rotation[0], rotation[1], rotation[2]);

   mat4x4 matScale = matrix_scale(scales[0], scales[1], scales[2]);

   unsigned int model;

   /// @brief: Base color to draw the object (this will be shaded by the camera)
   Color color = Color::White;


   object3d(unsigned int _model) : model{_model} {};


   void scale(float sx, float sy, float sz);

   void move(float x, float y, float z);

   void rotate(float u, float v, float w);

private:


};
