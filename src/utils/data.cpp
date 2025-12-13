
//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include "data.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>

// ******** GLOBAL VARIABLE EXAMPLE ********
sf::Color red;
sf::Color blue;
sf::Color green;
sf::Color yellow;
sf::Color purple;
sf::Color orange;
sf::Color black;
sf::Color white;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Define global variables (need to be initialized with extern in the header file first)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void defineGlobal() {

   red = sf::Color(201, 62, 52); //rgb(201, 62, 52)
   blue = sf::Color(43, 77, 161); //rgb(43, 77, 161)
   green = sf::Color(40, 133, 97); //rgb(40, 133, 97)
   yellow = sf::Color(222, 178, 69); //rgb(222, 178, 69)
   purple = sf::Color(103, 66, 199); //rgb(103, 66, 199)
   orange = sf::Color(214, 142, 60); //rgb(214, 142, 60)
   black = sf::Color(15, 18, 23); //rgb(15, 18, 23)
   white = sf::Color(224, 222, 215); //rgb(224, 222, 215)
}

