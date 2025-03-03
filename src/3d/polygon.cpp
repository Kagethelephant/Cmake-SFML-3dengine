//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include "polygon.hpp"
#include <SFML/Graphics/Vertex.hpp>
#include <vector>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Draw the triangle in 2D with 2D coordinates and clip on the screen
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tri3d::draw(std::vector<sf::Uint8>& texture,sf::Vector2i res, vec3 col) {

   // for cliping count points outside of view,
   // 2 points in view means need to create another triangle
   // for tris that clip 2 or more edges itterate through all edges and clip it multiple times

   bresenhamLine(texture, res.x, col, v[0].x, v[0].y, v[1].x, v[1].y);
   bresenhamLine(texture, res.x, col, v[1].x, v[1].y, v[2].x, v[2].y);
   bresenhamLine(texture, res.x, col, v[2].x, v[2].y, v[0].x, v[0].y);

//    sf::VertexArray triangle(sf::Triangles, 3);

//    triangle[0].color = col;
//    triangle[1].color = col; 
//    triangle[2].color = col; 

//    triangle[0].position = sf::Vector2f(v[0].x, v[0].y);
//    triangle[1].position = sf::Vector2f(v[1].x, v[1].y);
//    triangle[2].position = sf::Vector2f(v[2].x, v[2].y);

//    // Simple clipping check, we can improve this later
//    bool inframe1 = (triangle[0].position.x > 0 && triangle[0].position.x < res.x && triangle[0].position.y > 0 && triangle[0].position.y < res.y);
//    bool inframe2 = (triangle[1].position.x > 0 && triangle[1].position.x < res.x && triangle[1].position.y > 0 && triangle[1].position.y < res.y);
//    bool inframe3 = (triangle[2].position.x > 0 && triangle[2].position.x < res.x && triangle[2].position.y > 0 && triangle[2].position.y < res.y);

//    if(inframe1 || inframe2 || inframe3) {

//       texture.draw(triangle);

//       // Draw the lines
//       sf::Color col2 = black;

//       sf::Vertex line1[2], line2[2], line3[2];

//       line1[0].color = col2; line1[1].color = col2;
//       line2[0].color = col2; line2[1].color = col2;
//       line3[0].color = col2; line3[1].color = col2;

//       line1[0].position = sf::Vector2f(v[0].x, v[0].y); line1[1].position = sf::Vector2f(v[1].x, v[1].y);
//       line2[0].position = sf::Vector2f(v[1].x, v[1].y); line2[1].position = sf::Vector2f(v[2].x, v[2].y);
//       line3[0].position = sf::Vector2f(v[2].x, v[2].y); line3[1].position = sf::Vector2f(v[0].x, v[0].y);

//       texture.draw(line1,2,sf::Lines);
//       texture.draw(line2,2,sf::Lines);
//       texture.draw(line3,2,sf::Lines);
//    }
}



void bresenhamLine(std::vector<sf::Uint8>& buffer, int width, vec3 color, int x0, int y0, int x1, int y1){

   int dx = abs(x1 - x0);
   int sx = -1;
   if (x0 < x1) sx = 1;

   int dy = -abs(y1 - y0);
   int sy = -1;
   if (y0 < y1) sy = 1;

   int error = dx + dy;

   while (true) {

      int index = (width * y0 + x0) * 4;
      buffer [index] = color.x; 
      buffer [index + 1] = color.y; 
      buffer [index + 2] = color.z; 
      // buffer [index + 3] = color.w;
      
      int e2 = 2 * error;
      if (e2 >= dy) {

         if (x0 == x1) break;
         error = error + dy;
         x0 = x0 + sx;
      }
      if (e2 <= dx) {

         if (y0 == y1) break;
         error = error + dx;
         y0 = y0 + sy;
      } 
   }
}
