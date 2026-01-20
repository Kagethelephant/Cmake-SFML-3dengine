#include "polygon.hpp"

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>
#include <iostream>
#include "data.hpp"
#include "matrix.hpp"


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// FILL TRIANGLE
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void tri3d::draw(std::vector<std::uint8_t>& buffer, std::vector<float>& zbuffer, sf::Vector2u res, sf::Color color) {
   // Scanline tiangle filling method. This is the most common way to fill triangles with a color
   // It is more efficient to do this with graphics acceleration because cross products are expensive
   // for the CPU to calculate. If you comment this out you will see the FPS go up substantially.

   // Get the coordinates of the rectangle that will cover the triangle
   int xmax = std::max(std::max(v[0][0],v[1][0]),v[2][0]);
   int xmin = std::min(std::min(v[0][0],v[1][0]),v[2][0]);
   int ymax = std::max(std::max(v[0][1],v[1][1]),v[2][1]);
   int ymin = std::min(std::min(v[0][1],v[1][1]),v[2][1]);

   // Get the z coordinates of the triangle before projection
   float z0 = v[0][2];
   float z1 = v[1][2];
   float z2 = v[2][2];

   // std::cout << "old Z:  " << oldTri.v[0][2] <<std::endl;
   // std::cout << "proj Z: " << v[0][2] <<std::endl;

   // berycentric coordinate variables
   vec2 v0, v1, vp;
   float d00, d01, d11, dp0, dp1, denom, alpha, beta, gamma, baryz;

   // Create vectors used in barycentric coordinate calculation
   v0 = vec2(v[1][0]-v[0][0], v[1][1]-v[0][1]);
   v1 = vec2(v[2][0]-v[0][0], v[2][1]-v[0][1]);
   d00 = v0.dot(v0);
   d01 = v0.dot(v1);
   d11 = v1.dot(v1);
   // Barycentric coord denominator
   denom = d00 * d11 - d01 * d01;

   // Itterate through each pixel in that rectangle
   for (int y=ymin; y<=ymax; y++){
      for (int x=xmin; x<=xmax; x++){

         // Calculate vector from 0 vertice to the pixel
         vp = vec2(x-v[0][0], y-v[0][1]);

         dp0 = vp.dot(v0);
         dp1 = vp.dot(v1);
         
         // Outputs of barycentric coords (escencially a percentage of how close the point is to each vertice)
         beta = (d11 * dp0 - d01 * dp1)/ denom;
         gamma = (d00 * dp1 - d01 * dp0)/ denom;
         alpha = 1.0f - beta - gamma;

         // check if the point is on the inside of all of the edges. If so, add the pixel to the buffer
         if (alpha >= 0 and beta >= 0 and gamma >= 0){

            // barycentric coord calc
            baryz = alpha * z0 + beta * z1 + gamma * z2; 
            
            // if barycentric coord is egative than point is out of triangle
            int index = (res.x * y + x);
            if (baryz <= zbuffer[index]){
               buffer[index*4] = color.r; 
               buffer[index*4 + 1] = color.g; 
               buffer[index*4 + 2] = color.b; 
               zbuffer[index] = baryz; 
            }
         }
      }
   }
}


