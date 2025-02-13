#pragma once

//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <cstdint>
#include <iostream>



//////////////////////////////////////////////////////////////////
/// \brief Struct to hold color data, this is mainly used for color 
/// helpers in the IDE, otherwise we could use sf::Color
/// \param r: red
/// \param b: blue
/// \param g: green
/// \param a: alpha
/// \return color object
//////////////////////////////////////////////////////////////////
struct rgb {
  
  uint8_t r = 0;
  uint8_t b = 0;
  uint8_t g = 0;
  uint8_t a = 0;

  rgb(uint8_t _r, uint8_t _b, uint8_t _g, uint8_t _a = 255) : r(_r), b(_b), g(_g), a(_a) {};
};






//////////////////////////////////////////////////////////////////
/// \brief Built in colors that look better then SFML colors
//////////////////////////////////////////////////////////////////
enum COLOR {Black, Blue, Dark_Blue, Light_Blue, Green, Dark_Green, Tan, Purple, Dark_Purple, White};


//////////////////////////////////////////////////////////////////
/// \brief Get a built in color in the form of an SFML color object
/// \param color: Color
/// \param alpha: Alpha (optional) opaque (255) by default
/// \return SFML color object
//////////////////////////////////////////////////////////////////
sf::Color c_color(COLOR color, int alpha = 255);



//////////////////////////////////////////////////////////////////
/// \brief Get a built in color in the form of an SFML color object
/// \param color: Color
/// \param alpha: Alpha (optional) opaque (255) by default
/// \return SFML color object
//////////////////////////////////////////////////////////////////
sf::Color to_sfml_color(rgb color);


