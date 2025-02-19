
//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include "polygon.hpp"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tri3d::draw(sf::RenderTexture& texture,sf::Vector2i res, sf::Color col) {

   // for cliping count points outside of view,
   // 2 points in view means need to create another triangle
   // for tris that clip 2 or more edges itterate through all edges and clip it multiple times
   sf::VertexArray triangle(sf::Triangles, 3);

   triangle[0].color = col;
   triangle[1].color = col; 
   triangle[2].color = col; 

   triangle[0].position = sf::Vector2f(v[0].x, v[0].y);
   triangle[1].position = sf::Vector2f(v[1].x, v[1].y);
   triangle[2].position = sf::Vector2f(v[2].x, v[2].y);

   // Simple clipping check, we can improve this later
   bool inframe1 = (triangle[0].position.x > 0 && triangle[0].position.x < res.x && triangle[0].position.y > 0 && triangle[0].position.y < res.y);
   bool inframe2 = (triangle[1].position.x > 0 && triangle[1].position.x < res.x && triangle[1].position.y > 0 && triangle[1].position.y < res.y);
   bool inframe3 = (triangle[2].position.x > 0 && triangle[2].position.x < res.x && triangle[2].position.y > 0 && triangle[2].position.y < res.y);

   if(inframe1 || inframe2 || inframe3) texture.draw(triangle);



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
