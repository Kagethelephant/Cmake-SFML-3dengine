#include "display.hpp"





//////////////////////////////////////////////////////////////////
/// Canvas Functions
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
/// Constructor
//////////////////////////////////////////////////////////////////
Canvas::Canvas(sf::Vector2i canvasSize, int layers)
{
	m_renderer.create(canvasSize.x,canvasSize.y);

	// Create all of the layers (max is initialization size m_n)
	if (layers > m_n) layers = m_n;
	for (int i = 0; i < layers; i++)
	{
		m_renderer.clear(sf::Color::Transparent);
		m_renderer.display();
		m_layers[i] = sf::Texture();
		m_reset[i] = true;
	}
}

//////////////////////////////////////////////////////////////////
/// Destructor
//////////////////////////////////////////////////////////////////
Canvas::~Canvas()
{
	delete[] m_layers;
}





//////////////////////////////////////////////////////////////////
void Canvas::draw(int layer, sf::Drawable& sprite)
{
	m_renderer.clear(sf::Color::Transparent);
	m_renderer.draw(sf::Sprite(m_layers[layer]));
	m_renderer.draw(sprite);
	m_renderer.display();
	m_layers[layer] = m_renderer.getTexture();
}


//////////////////////////////////////////////////////////////////                                                                 
void Canvas::clear(int layer, sf::Color clear)
{
	m_renderer.clear(clear);
	m_renderer.display();
	m_layers[layer] = m_renderer.getTexture();
}


//////////////////////////////////////////////////////////////////
void Canvas::reset(int layer, bool reset)
{
	m_reset[layer] = reset;
}


//////////////////////////////////////////////////////////////////
void Canvas::display(sf::RenderWindow& window, int x, int y)
{
	// Draw all of the layers to the render texture
	m_renderer.clear(sf::Color::Transparent);
	for(int i = 0; i < m_n; i++)
	{
		m_renderer.draw(sf::Sprite(m_layers[i]));
	}

	// Make the texture into a sprite
	m_sprite.setTexture(m_renderer.getTexture());
	m_sprite.setOrigin(0,0);
	m_sprite.setPosition(x,y);
	
	// Draw the sprite to the display
	window.draw(m_sprite);


	// Clear the layers that need to be cleared
	m_renderer.clear(sf::Color::Transparent);
	for (int i = 0; i < m_n; i++)
	{
		if(m_reset[i] == true) m_layers[i] = m_renderer.getTexture();
	}
}





//////////////////////////////////////////////////////////////////
// Setup the window initially
//////////////////////////////////////////////////////////////////
sf::Vector2i windowSetup(sf::RenderWindow& window, int height, bool fullscreen = true, sf::String title = "Window", int fps = 60)
{
	
	// Get the display dimmensions and calculate the aspect ratio
	float aspectRatio = ((float)sf::VideoMode::getDesktopMode().width) / ((float)sf::VideoMode::getDesktopMode().height);

	// Create a static window width in pixels and calculate the width based on the aspect ratio
	int bufferH = height;
	int bufferW = abs(bufferH * aspectRatio);

	// Initialize the view with the calculated resolution 
	if (fullscreen) window.create(sf::VideoMode(bufferW, bufferH), title, sf::Style::Fullscreen);
	else window.create(sf::VideoMode(bufferW, bufferH), title, sf::Style::Default);

	// Set the frame rate and hide the cursor so we can draw our own
	window.setFramerateLimit(fps);
	window.setMouseCursorVisible(false);

	// Set the size and position of the view
	sf::View view;
	view.setCenter(sf::Vector2f(bufferW / 2, bufferH / 2));
	view.setSize(sf::Vector2f(bufferW, bufferH));

	// Asign the view to the window
	window.setView(view);

	// Return the calculated width and height of the window in pixels
	return sf::Vector2i(bufferW, bufferH);
}


