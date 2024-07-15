#include "data.hpp"



//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
sf::Color c_color(COLOR color, int alpha)
{
    if(color == COLOR::Black) return sf::Color(15,15,25,alpha);
	else if(color == COLOR::Dark_Blue) return sf::Color(25,45,105,alpha);
	else if(color == COLOR::Blue) return sf::Color(45,80,115,alpha);
	else if(color == COLOR::Light_Blue) return sf::Color(65,95,120,alpha);
	else if(color == COLOR::Tan) return sf::Color(115,115,90,alpha);
	else if(color == COLOR::Green) return sf::Color(50,85,75,alpha);
	else if(color == COLOR::Dark_Green) return sf::Color(35,55,60,alpha);
	else if(color == COLOR::Purple) return sf::Color(75,60,85,alpha);
	else if(color == COLOR::Dark_Purple) return sf::Color(45,40,50,alpha);
	else if(color == COLOR::White) return sf::Color(180,180,180,alpha);

	else {return sf::Color(255,255,255,alpha);}
}