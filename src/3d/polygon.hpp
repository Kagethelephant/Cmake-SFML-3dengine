#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "data.hpp"
#include "matrix.hpp"

// Forward declaration for parent pointer
class object3d;

/// @brief: Container for 3 3D vectors with functions for drawing to a 2D pixel array.
/// drawing is done with scanline filling and bresenham line function. This is intended for 
/// educational purposes so it is inefficient and lacks advanced features like z-buffers
/// @param _v0: Vector 0 (vec3)
/// @param _v1: Vector 1 (vec3)
/// @param _v2: Vector 2 (vec3)
/// @param parent: Pointer to a parent objec3d object
struct tri3d {

public:

   vec3 v[3];
   object3d* parent;

   tri3d() : v{vec3(),vec3(),vec3()}{};
   tri3d(vec3 v0, vec3 v1, vec3 v2) : v{v0,v1,v2}{};
   tri3d(vec3 v0, vec3 v1, vec3 v2, object3d* parent) : v{v0,v1,v2}, parent(parent){};

   // Deep copy (copy constructor). Shallow copy will not copy the parent pointer
   tri3d(const tri3d& other) : v{other.v[0], other.v[1],other.v[2]}, parent(other.parent){};

   // @brief: Generates a vector normal to the triangles face starting from the triangles 0 point
   vec3 normal() {return ((this->v[0] - this->v[1]).cross(this->v[0] - this->v[2])).normal(); }
   // @brief: Gets centerpoint x value (takes average of 3 vertices x value)
   float centerx() {return (this->v[0][0]+this->v[1][0]+this->v[2][0])/3.0f; }
   // @brief: Gets centerpoint y value (takes average of 3 vertices y value)
   float centery() {return (this->v[0][1]+this->v[1][1]+this->v[2][1])/3.0f; }
   // @brief: Gets centerpoint z value (takes average of 3 vertices z value)
   float centerz() {return (this->v[0][2]+this->v[1][2]+this->v[2][2])/3.0f; }

   // Operator overloads for multiplying a whole triagle by a matrix (just multiplies the underlying vectors)
   tri3d operator * (const mat4x4& m) { return tri3d(this->v[0] * m, this->v[1] * m, this->v[2] * m); }
   void operator *= (const mat4x4& m) { this->v[0] *= m; this->v[1] *= m; this->v[2] *= m; }

   /// @brief: Scanline triangle fill algorithm. This normally should not run on the CPU because it's slow
   /// but this is a simple way to demonstrate how rasterization works
   /// @param buffer: Pixel array for 32 bit trucolor + alpha (8 bits for r,g,b and alpha)
   /// @param res: The resolution of the window
   /// @param color: Color to draw the triangle
   /// @param lineColor: Color to draw the outline 1 pixel wide (default: Transperent)
   void draw(tri3d oldTri, std::vector<std::uint8_t>& buffer, std::vector<float>& zbuffer, sf::Vector2u res, sf::Color color, sf::Color lineColor = sf::Color(ColorToHex(Color::Transperant)));

private:

   /// @brief: efficiently draws a line in pixels btwn 2 points using only integers
   /// @param p0: First point
   /// @param p1: Second point
   /// @param buffer: Pixel array for 32 bit trucolor + alpha (8 bits for r,g,b and alpha)
   /// @param res: The resolution of the window
   /// @param color: Color to draw the line
   void m_bresenhamLine(vec3 p0, vec3 p1, std::vector<std::uint8_t>& buffer, sf::Vector2u res, sf::Color color);
};
