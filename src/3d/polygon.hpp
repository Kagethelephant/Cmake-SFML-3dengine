#pragma once

//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <vector>
#include "utils/matrix.hpp"
#include "utils/data.hpp"


//////////////////////////////////////////////////////////////////
/// \brief Triangle for rendering 3d objects
//////////////////////////////////////////////////////////////////
struct tri3d {

   vec3 v[3];

   // Constructors (default and with vectors)
   tri3d() : v{vec3(),vec3(),vec3()}{};
   tri3d(vec3 _x, vec3 _y, vec3 _z) : v{_x,_y,_z}{};

   // Gets the normal vector of a triangle by getting the lines branching from the tris 0 point and getting the normal of the 2 lines
   vec3 normal() {return ((this-> v[0] - this-> v[1]).cross(this-> v[0] - this-> v[2])).normal(); }
   float lineClipToPlain(vec3 point, vec3 norm, vec3 line1, vec3 line2);
   std::vector<tri3d> clipToPlain(vec3 point, vec3 norm);
   bool clippedToPlain(vec3 point, vec3 norm);

   // Operator overloads for multiplying a whole triagle by a matrix
   tri3d operator * (const mat4x4& m) { return tri3d(this->v[0] * m, this->v[1] * m, this->v[2] * m); }
   void operator *= (const mat4x4& m) { this->v[0] *= m; this->v[1] *= m; this->v[2] *= m; }

   /// simple draw 2d triangles
   void draw(sf::RenderTexture& texture, sf::Vector2i res, sf::Color col = blue);
};
