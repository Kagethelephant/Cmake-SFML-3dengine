#include "data.hpp"
#include <iostream>


// Define colors
vec4 red = vec4(201, 62, 52, 255); //rgb(201, 62, 52)
vec4 blue = vec4(43, 77, 161, 255); //rgb(43, 77, 161)
vec4 green = vec4(40, 133, 97, 255); //rgb(40, 133, 97)
vec4 yellow = vec4(222, 178, 69, 255); //rgb(222, 178, 69)
vec4 purple = vec4(103, 66, 199, 255); //rgb(103, 66, 199)
vec4 orange = vec4(214, 142, 60, 255); //rgb(214, 142, 60)
vec4 black = vec4(15, 18, 23, 255); //rgb(15, 18, 23)
vec4 white = vec4(224, 222, 215, 255); //rgb(224, 222, 215)

// sf::Color colorRGBtoSFML(vec3 color){
//    return sf::Color(color.x,color.y,color.z,color.w);
// }

unsigned int ColorToHex(Color color) {
   unsigned int hexValue = static_cast<unsigned int>(color);
    return hexValue;
}


vec4 hexColorToFloat(Color color) {
   vec4 floatColor;
   unsigned int hexValue = static_cast<unsigned int>(color);
   // Extract the R, G, B, and A bytes using bit shifting and masking
   floatColor[0] = ((hexValue >> 24) & 0xFF)/256.0f; // Extract the AA byte
   floatColor[1] = ((hexValue >> 16) & 0xFF)/256.0f; // Extract the RR byte
   floatColor[2] = ((hexValue >> 8) & 0xFF)/256.0f;  // Extract the GG byte
   floatColor[3] = ((hexValue) & 0xFF)/256.0f;       // Extract the BB byte
   return floatColor;
}
