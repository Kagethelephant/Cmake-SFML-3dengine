#pragma once

//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include "data.hpp"





//////////////////////////////////////////////////////////////////
/// \brief An easy interface for saving and layering 2d buffers
/// \param canvasSize sf::vector2i with the size of the canvas
/// \param layers the amout of layers in the canvas (default and max = 20)
//////////////////////////////////////////////////////////////////
class Canvas
{
public:

	//////////////////////////////////////////////////////////////////
	/// Constructor
	//////////////////////////////////////////////////////////////////
	Canvas(sf::Vector2i canvasSize, int layers = 20);

	//////////////////////////////////////////////////////////////////
	// Destrucor
	//////////////////////////////////////////////////////////////////
	~Canvas();





	//////////////////////////////////////////////////////////////////
	/// \brief Draw something to one of the layers in the canvas
	/// \param layer layer to draw to. 0 being bottom and 10 being top
	/// \param sprite The sf::Drawable to draw to the layer
	//////////////////////////////////////////////////////////////////
	void draw(int layer, sf::Drawable& sprite);


	//////////////////////////////////////////////////////////////////
	/// \brief Clear the layer manually, clears transparent by default
	/// \param layer layer to clear
	/// \param clear sf::Color to clear the layer with
	//////////////////////////////////////////////////////////////////
	void clear(int layer, sf::Color clear = sf::Color::Transparent);


	//////////////////////////////////////////////////////////////////
	/// \brief Sets the layer to reset or not reset every frame
	/// \param layer Layer to toggle setting
	/// \param sprite false keeps the frame until it is cleared, this
	/// is set to true by default to draw moving sprites
	//////////////////////////////////////////////////////////////////
	void reset(int layer, bool reset);


	//////////////////////////////////////////////////////////////////
	/// \brief Although this says display it actually draws the canvas
	/// and all of it layers to the given window. still need to call
	/// window.display()
	/// \param layer Window to draw the canvas to
	/// \param x x position of window from top left
	/// \param y y position of window from top left
	//////////////////////////////////////////////////////////////////
	void display(sf::RenderWindow& window, int x = 0, int y = 0);


	// Layers/textures to be used to draw (10 max)
	int const m_n = 20;
	bool* m_reset = new bool[m_n];
	sf::Texture* m_layers = new sf::Texture [m_n];

private:

	//////////////////////////////////////////////////////////////////
	// Member Variables
	//////////////////////////////////////////////////////////////////

	// This is the place holder texture to transfer content to layer textures
	sf::RenderTexture m_renderer;

	// Sprite to be drawn
	sf::Sprite m_sprite;
};

//sets up window with a pixel scale and returns the dimmensions of the window
//in a 2 dimmensional vector
sf::Vector2i windowSetup(sf::RenderWindow&, int, bool, sf::String, int);

