#pragma once

//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include "matrix.hpp"

extern sf::Color red;
extern sf::Color blue;
extern sf::Color green;
extern sf::Color yellow;
extern sf::Color purple;
extern sf::Color orange;
extern sf::Color black;
extern sf::Color white;


//////////////////////////////////////////////////////////////////
/// \brief This function creates an rgb sf::color object, the only purpose
/// of this function is lsp color recognition with the rgb(r,g,b) format
/// \param b: blue
/// \param g: green
/// \param a: alpha
/// \return color object
//////////////////////////////////////////////////////////////////
sf::Color rgb(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);



//////////////////////////////////////////////////////////////////
/// \brief This initializes all of the variables above
//////////////////////////////////////////////////////////////////
void defineGlobal();
