#include "polygon.hpp"

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>
#include "matrix.hpp"

// Draw triangle
void tri3d::draw(std::vector<std::uint8_t>& buffer, sf::Vector2u res, sf::Color color, sf::Color lineColor) {
   // Scanline tiangle filling method. This is the most common way to fill triangles with a color
   // It is more efficient to do this with graphics acceleration because cross products are expensive
   // for the CPU to calculate. If you comment this out you will see the FPS go up substantially.

   // Get the coordinates of the rectangle that will cover the triangle
   int xmax = std::max(std::max(v[0].x,v[1].x),v[2].x);
   int xmin = std::min(std::min(v[0].x,v[1].x),v[2].x);
   int ymax = std::max(std::max(v[0].y,v[1].y),v[2].y);
   int ymin = std::min(std::min(v[0].y,v[1].y),v[2].y);

   // Itterate through each pixel in that rectangle
   for (int y=ymin; y<=ymax; y++){
      for (int x=xmin; x<=xmax; x++){
         // Use the cross product to determine if the point is on the outside 
         // or inside of each of the edges 
         int cross0 = ((x-v[0].x)*(v[1].y-v[0].y) - (y-v[0].y)*(v[1].x-v[0].x));
         int cross1 = ((x-v[1].x)*(v[2].y-v[1].y) - (y-v[1].y)*(v[2].x-v[1].x));
         int cross2 = ((x-v[2].x)*(v[0].y-v[2].y) - (y-v[2].y)*(v[0].x-v[2].x));
         // check if the point is on the inside of all of the edges. If so, add the pixel to the buffer
         if (cross0 <= 0 and cross1 <= 0 and cross2 <= 0){
            int index = (res.x * y + x) * 4;
            buffer [index] = color.r; 
            buffer [index + 1] = color.g; 
            buffer [index + 2] = color.b; 
         }
      }
   }
   // If there is a lineColor provided draw the outline using bresenham function
   if(!(lineColor == sf::Color::Transparent)){
      m_bresenhamLine(v[0], v[1], buffer, res, lineColor);
      m_bresenhamLine(v[1], v[2], buffer, res, lineColor);
      m_bresenhamLine(v[2], v[0], buffer, res, lineColor);
   }
}

// Bresenham's line algorithm for all octants
void tri3d::m_bresenhamLine(vec3 p0, vec3 p1, std::vector<std::uint8_t>& buffer, sf::Vector2u res, sf::Color color) {
   // Cast the vector coordinates as integers, this function cannot handle floats
   int x0 = (int)p0.x; 
   int x1 = (int)p1.x; 
   int y0 = (int)p0.y; 
   int y1 = (int)p1.y; 
   // Get delta x and the delta y
   int dx = std::abs(x1 - x0);
   int dy = std::abs(y1 - y0);
   // Get the direction to iterate (sign x and sign y)
   int sx = (x0 < x1) ? 1 : -1;
   int sy = (y0 < y1) ? 1 : -1;

   // Used to determine the ratio of where the line lies between the center of the two potential next pixels
   // ie. if sx and sy are positive and dx = 12 and dy = 2, the initial err would be 10. This means that
   // when the line passes the center of the first square to right, it will be 2/12 from the center of the 
   // bottom square to the center of the top square. The bottom square will be chosen
   int err = dx - dy; 

   while (true) {
      // Plot point
      int index = (res.x * y0 + x0) * 4;
      buffer [index] = color.r; 
      buffer [index + 1] = color.g; 
      buffer [index + 2] = color.b; 

      // Last point was plotted
      if (x0 == x1 && y0 == y1) break;
      // Multiply the error by 2 because we want to know if the line
      // intercets over halfway between the two squares in question
      int e2 = 2 * err;

      // Check if the line is now closer to the next y cell, if so move a level
      // and adjust the error to account for the position change
      if (e2 > -dy) {
         err -= dy;
         // Move x once in the direction determined by sx
         x0 += sx;
      }
      // Check if the line is now closer to the next x cell, if so move a level
      // and adjust the error to account for the position change
      if (e2 < dx){
         err += dx;
         // Move y once in the direction determined by sy
         y0 += sy;
      }
   }
}


