#pragma once

//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Texture.hpp>
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

   bool clippedToPlain(vec3 norm, float d);

   // Operator overloads for multiplying a whole triagle by a matrix
   tri3d operator * (const mat4x4& m) { return tri3d(this->v[0] * m, this->v[1] * m, this->v[2] * m); }
   void operator *= (const mat4x4& m) { this->v[0] *= m; this->v[1] *= m; this->v[2] *= m; }

   /// simple draw 2d triangles
   void draw(std::vector<std::uint8_t>& texture, sf::RenderTexture& tex, sf::Vector2u res, vec3 col = blue);
};

void bresenhamLine(std::vector<std::uint8_t>& buffer, int width, vec3 color, int x0, int y0, int x1, int y1);
void fillTri(std::vector<std::uint8_t>& buffer, int width, vec3 color, int x0, int y0, int x1, int y1, int x2, int y2);
