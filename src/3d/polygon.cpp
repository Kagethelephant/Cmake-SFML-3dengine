//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include "polygon.hpp"
#include "utils/data.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <vector>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Draw the triangle in 2D with 2D coordinates and clip on the screen
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tri3d::draw(std::vector<sf::Uint8>& texture,sf::Vector2i res, vec3 col) {

   // for cliping count points outside of view,
   // 2 points in view means need to create another triangle
   // for tris that clip 2 or more edges itterate through all edges and clip it multiple times

   // fillTri(texture,res.x,col,v[0].x,v[0].y,v[1].x,v[1].y,v[2].x,v[2].y);

   // sf::VertexArray triangle(sf::Triangles, 3);

   // triangle[0].color = sf::Color(col.x,col.y,col.z,col.w);
   // triangle[1].color = sf::Color(col.x,col.y,col.z,col.w); 
   // triangle[2].color = sf::Color(col.x,col.y,col.z,col.w); 

   // triangle[0].position = sf::Vector2f(v[0].x, v[0].y);
   // triangle[1].position = sf::Vector2f(v[1].x, v[1].y);
   // triangle[2].position = sf::Vector2f(v[2].x, v[2].y);

   // texture.draw(triangle);

   // // Draw the lines
   // sf::Color col2 = black;

   // sf::Vertex line1[2], line2[2], line3[2];

   // line1[0].color = col2; line1[1].color = col2;
   // line2[0].color = col2; line2[1].color = col2;
   // line3[0].color = col2; line3[1].color = col2;

   // line1[0].position = sf::Vector2f(v[0].x, v[0].y); line1[1].position = sf::Vector2f(v[1].x, v[1].y);
   // line2[0].position = sf::Vector2f(v[1].x, v[1].y); line2[1].position = sf::Vector2f(v[2].x, v[2].y);
   // line3[0].position = sf::Vector2f(v[2].x, v[2].y); line3[1].position = sf::Vector2f(v[0].x, v[0].y);

   // texture.draw(line1,2,sf::Lines);
   // texture.draw(line2,2,sf::Lines);
   // texture.draw(line3,2,sf::Lines);
}


void fillTri(std::vector<sf::Uint8>& buffer, int width, vec3 color, int x0, int y0, int x1, int y1, int x2, int y2){

   int xmax = std::max(std::max(x0,x1),x2);
   int xmin = std::min(std::min(x0,x1),x2);
   int ymax = std::max(std::max(y0,y1),y2);
   int ymin = std::min(std::min(y0,y1),y2);

   int vx0 = x1-x0;
   int vy0 = y1-y0;
   int vx1 = x2-x1;
   int vy1 = y2-y1;
   int vx2 = x0-x2;
   int vy2 = y0-y2;

   int dec1, dec2, dec3;

   for (int y=ymin; y<=ymax; y++){
      for (int x=xmin; x<=xmax; x++){
         int cross0 = ((x-x0)*vy0 - (y-y0)*vx0);
         int cross1 = ((x-x1)*vy1 - (y-y1)*vx1);
         int cross2 = ((x-x2)*vy2 - (y-y2)*vx2);

         if (cross0 < 0 and cross1 < 0 and cross2 < 0){

            int index = (width * y + x) * 4;
            buffer [index] = color.x; 
            buffer [index + 1] = color.y; 
            buffer [index + 2] = color.z; 
            // buffer [index + 3] = color.w; 
         }
      }
   }
}

