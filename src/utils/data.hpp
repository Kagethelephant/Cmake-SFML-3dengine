#pragma once

#include <SFML/Graphics.hpp>
#include <utils.hpp>


// GLOBAL VARIABLES
//---------------------------------------------------------------------------------------------

// This is an alternate way to declare some global colors
// I used the extern method because i am using SFML color class
// this way i dont have to create another color object everytime
// enum class Color : unsigned int {
//    Red   = 0xc93e34,
//    Green = 0x288561,
//    Blue  = 0x2b4da1,
//    White = 0xe0ded7,
//    Black = 0x0f1217,
//    Yellow = 0xdeb245,
//    Purple = 0x6742c7,
//    Orange = 0xd68e3c,
// };

// You can use "const" to define variables like this but
constexpr double pi = 3.14159265358979323846;

// Global randomizer object so we dont have to make multiple instances
extern randObj rander;

// Initialize global color objects
extern sf::Color red;
extern sf::Color blue;
extern sf::Color green;
extern sf::Color yellow;
extern sf::Color purple;
extern sf::Color orange;
extern sf::Color black;
extern sf::Color white;

/// @brief Initialize global variables (colors)
void defineGlobal();
