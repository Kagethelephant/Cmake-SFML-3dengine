#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "matrix.hpp"

class object3d;
/// \brief Triangle for rendering 3d objects
struct tri3d {

public:

   vec3 v[3];
   object3d* parent;

   // Constructors
   tri3d() : v{vec3(),vec3(),vec3()}{};
   tri3d(vec3 _x, vec3 _y, vec3 _z) : v{_x,_y,_z}{};
   tri3d(vec3 _x, vec3 _y, vec3 _z, object3d* _parent) : v{_x,_y,_z}, parent(_parent){};

   // @brief: Generates a vector normal to the triangles face starting from the triangles 0 point
   vec3 normal() {return ((this-> v[0] - this-> v[1]).cross(this-> v[0] - this-> v[2])).normal(); }

   // Operator overloads for multiplying a whole triagle by a matrix (just multiplies the underlying vectors)
   tri3d operator * (const mat4x4& m) { return tri3d(this->v[0] * m, this->v[1] * m, this->v[2] * m); }
   void operator *= (const mat4x4& m) { this->v[0] *= m; this->v[1] *= m; this->v[2] *= m; }

   /// @brief: Scanline triangle fill algorithm. This normally should not run on the CPU because it is slow
   /// but this is a simple way to demonstrate how rasterization works
   /// @param buffer: Pixel array for 32 bit trucolor + alpha (8 bits for r,g,b and alpha)
   /// @param res: The resolution of the window
   /// @param color: Color to draw the triangle
   /// @param lineColor: Color to draw the outline 1 pixel wide (default: Transperent)
   void draw(std::vector<std::uint8_t>& buffer, sf::Vector2u res, sf::Color color, sf::Color lineColor = sf::Color::Transparent);

private:

   /// @brief: efficiently draws a line in pixels btwn 2 points using only integers
   /// @param p0: First point
   /// @param p1: Second point
   /// @param buffer: Pixel array for 32 bit trucolor + alpha (8 bits for r,g,b and alpha)
   /// @param res: The resolution of the window
   /// @param color: Color to draw the line
   void m_bresenhamLine(vec3 p0, vec3 p1, std::vector<std::uint8_t>& buffer, sf::Vector2u res, sf::Color color);
};
