#pragma once

#include "matrix.hpp"
#include "utils.hpp"


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// GLOBAL OBJECTS
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

// You can use "const" to define variables like this but
constexpr double pi = 3.14159265358979323846;

// Global randomizer object so we dont have to make multiple instances
extern randObj rander;


enum class Color : unsigned int {
   Red   = 0xc93e34ff, //0xc93e34
   Green = 0x288561ff, //0x288561
   Blue  = 0x2b4da1ff, //0x2b4da1
   White = 0xe0ded7ff, //0xe0ded7
   Black = 0x0f1217ff, //0x0f1217
   Yellow = 0xdeb245ff, //0xdeb245
   Purple = 0x6742c7ff, //0x6742c7
   Orange = 0xd68e3cff, //0xd68e3c
   Transperant = 0xffffff00,
};


vec4 hexColorToFloat(Color hexValue);


vec4 hexColorToRGB(Color color);



