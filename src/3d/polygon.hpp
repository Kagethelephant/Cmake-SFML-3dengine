#pragma once

//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include "utils/matrix.hpp"
#include "utils/data.hpp"


//////////////////////////////////////////////////////////////////
/// \brief Triangle for rendering 3d objects
//////////////////////////////////////////////////////////////////
struct tri3d {

   vec3 v[3];

   // Constructors
   tri3d(vec3 _x, vec3 _y, vec3 _z) : v{_x,_y,_z}{};
   tri3d() : v{vec3(0,0,0),vec3(0,0,0),vec3(0,0,0)}{};

   // Gets the normal vector of a triangle by getting the lines branching from the tris 0 point and getting the normal of the 2 lines
   vec3 norm() {return ((this-> v[0] - this-> v[1]).cross(this-> v[0] - this-> v[2])).norm(); }

   /// simple draw 2d triangles
   void draw(sf::RenderTexture& texture, sf::Vector2i res, sf::Color col = blue);
};
