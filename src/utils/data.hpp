#pragma once

//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include "matrix.hpp"

extern vec3 red;
extern vec3 blue;
extern vec3 green;
extern vec3 yellow;
extern vec3 purple;
extern vec3 orange;
extern vec3 black;
extern vec3 white;


//////////////////////////////////////////////////////////////////
/// \brief This function creates an rgb sf::color object, the only purpose
/// of this function is lsp color recognition with the rgb(r,g,b) format
/// \param b: blue
/// \param g: green
/// \param a: alpha
/// \return color object
//////////////////////////////////////////////////////////////////
vec3 rgb(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);



//////////////////////////////////////////////////////////////////
/// \brief This initializes all of the variables above
//////////////////////////////////////////////////////////////////
void defineGlobal();
