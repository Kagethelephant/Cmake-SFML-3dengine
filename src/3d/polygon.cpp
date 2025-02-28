//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include "polygon.hpp"
#include <SFML/Graphics/Vertex.hpp>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Draw the triangle in 2D with 2D coordinates and clip on the screen
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

   if(inframe1 || inframe2 || inframe3) {

      texture.draw(triangle);

      // Draw the lines
      sf::Color col2 = black;

      sf::Vertex line1[2], line2[2], line3[2];

      line1[0].color = col2; line1[1].color = col2;
      line2[0].color = col2; line2[1].color = col2;
      line3[0].color = col2; line3[1].color = col2;

      line1[0].position = sf::Vector2f(v[0].x, v[0].y); line1[1].position = sf::Vector2f(v[1].x, v[1].y);
      line2[0].position = sf::Vector2f(v[1].x, v[1].y); line2[1].position = sf::Vector2f(v[2].x, v[2].y);
      line3[0].position = sf::Vector2f(v[2].x, v[2].y); line3[1].position = sf::Vector2f(v[0].x, v[0].y);

      texture.draw(line1,2,sf::Lines);
      texture.draw(line2,2,sf::Lines);
      texture.draw(line3,2,sf::Lines);
   }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float tri3d::lineClipToPlain(vec3 point, vec3 norm, vec3 line1, vec3 line2) {
   
   float a = norm.x;
   float b = norm.y;
   float c = norm.z;

   float d = a*-point.x + b*-point.y + c *-point.z;
   float t = (a*line1.x + b*line1.y + c*line1.z + d)/(a*(line1.x-line2.x) + b*(line1.y-line2.y) + c*(line1.z-line2.z));

   return t;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<tri3d> tri3d::clipToPlain(vec3 point, vec3 norm) {

   std::vector<tri3d> newTris;

   float a = norm.x;
   float b = norm.y;
   float c = norm.z;

   float d = a*-point.x + b*-point.y + c *-point.z;

   bool inside0 = ((a*v[0].x + b*v[0].y + c*v[0].z + d) > 0);
   bool inside1 = ((a*v[1].x + b*v[1].y + c*v[1].z + d) > 0);
   bool inside2 = ((a*v[2].x + b*v[2].y + c*v[2].z + d) > 0);

   if (inside0 || inside1 || inside2){}

   return newTris;
}


bool tri3d::clippedToPlain(vec3 point, vec3 norm) {

   std::vector<tri3d> newTris;

   float a = norm.x;
   float b = norm.y;
   float c = norm.z;

   float d = a*-point.x + b*-point.y + c *-point.z;

   bool inside0 = ((a*v[0].x + b*v[0].y + c*v[0].z + d) > 0);
   bool inside1 = ((a*v[1].x + b*v[1].y + c*v[1].z + d) > 0);
   bool inside2 = ((a*v[2].x + b*v[2].y + c*v[2].z + d) > 0);

   if ((!inside0) || (!inside1) || (!inside2)){ return true; }

   return false;
}





