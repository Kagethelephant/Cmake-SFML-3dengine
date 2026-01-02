#pragma once

#include <SFML/Graphics.hpp>
#include <string>
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
   vec3 scale = vec3(1,1,1);
   /// @brief: Winding of triangle mesh. default clockwise
   bool ccwWinding = false;

   /// @brief: Base color to draw the object (this will be shaded by the camera)
   sf::Color color;
   /// @brief: Optional line color to outline all of the triangles
   sf::Color lineColor;

   /// @brief: Stores the mesh of tiangles loaded from the OBJ file
   std::vector<tri3d> mesh;


   object3d(std::string filename = "", vec3 _position = vec3(0,0,0), vec3 _scale = vec3(1,1,1), sf::Color _color = sf::Color::White, sf::Color _lineColor = sf::Color::Transparent, bool _ccwWinding = false);

   /// @brief: Load an OBJ file. OBJ file must only contain triangle polygons and no textures
   /// @param filename: Relative file location of the obj file
   void load(std::string filename);

   /// @brief: Update the rotation matrix of the object (relative adjustment)
   void update(vec3 position, vec3 rotation, vec3 scale);

private:


};
