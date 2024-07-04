#pragma once

// SFML
#include <SFML/Graphics.hpp>

// Local
#include "data.hpp"



class Canvas
{
public:

	Canvas(int _w,int _h);
	~Canvas();



	// Size of the screen or canvas that is being used
	sf::Vector2i m_canvasSize;

	// This is the place holder texture to transfer content to layer textures
	sf::RenderTexture m_renderer;

	// Sprite to be drawn
	sf::Sprite m_sprite;
	
	// Layers/textures to be used to draw (10 max)
	int const m_n = 10;
	bool* m_reset = new bool[m_n];
	sf::Texture* m_layers = new sf::Texture [m_n];
	



	// Draw an sf::drawable to one of the layer textures
	void draw(int _layer, sf::Drawable& _sprite);

	// Clear one of the layers (layers are automatically cleared every frame)
	void clear(int _layer, sf::Color _clear = sf::Color::Transparent);

	// Set a layer to to clear every frame (true) or stay static (false)
	void reset(int _layer, bool reset);

	// Draw all of the layers to a given window
	void display(sf::RenderWindow& _window, int _x = 0, int _y = 0);
};

//sets up window with a pixel scale and returns the dimmensions of the window
//in a 2 dimmensional vector
sf::Vector2i windowSetup(sf::RenderWindow&, int, bool, sf::String, int);

