
#include "data.hpp"


sf::Color red;
sf::Color blue;
sf::Color green;
sf::Color yellow;
sf::Color purple;
sf::Color orange;
sf::Color black;
sf::Color white;

//////////////////////////////////////////////////////////////////
sf::Color rgb(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
  return sf::Color(r,g,b,a);
};

//////////////////////////////////////////////////////////////////
void defineGlobal() {

  red = rgb(201, 62, 52);
  blue = rgb(43, 77, 161);
  green = rgb(40, 133, 97);
  yellow = rgb(222, 178, 69);
  purple = rgb(103, 66, 199);
  orange = rgb(214, 142, 60);
  black = rgb(15, 18, 23);
  white = rgb(224, 222, 215);

}
