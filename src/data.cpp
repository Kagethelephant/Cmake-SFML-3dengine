#include "data.hpp"


sf::Color c_color(sf::String _col, int _a)
{
    if(_col == "black") return sf::Color(15,15,25,_a);
	else if(_col == "dk_blue") return sf::Color(25,45,105,_a);
	else if(_col == "blue") return sf::Color(45,80,115,_a);
	else if(_col == "lt_blue") return sf::Color(65,95,120,_a);
	else if(_col == "tan") return sf::Color(115,115,90,_a);
	else if(_col == "green") return sf::Color(50,85,75,_a);
	else if(_col == "dk_green") return sf::Color(35,55,60,_a);
	else if(_col == "purple") return sf::Color(75,60,85,_a);
	else if(_col == "dk_purple") return sf::Color(45,40,50,_a);
	else if(_col == "white") return sf::Color(180,180,180,_a);
	else if(_col == "dk_blue") return sf::Color(25,45,105,_a);
	else if(_col == "dk_blue") return sf::Color(25,45,105,_a);
	else if(_col == "dk_blue") return sf::Color(25,45,105,_a);
	else 
	{
		std::string col_str = _col;
		std::cout << "ERROR: color \"" + col_str +  "\"not supported" << std::endl;
		return sf::Color(255,255,255,_a);
	}
}