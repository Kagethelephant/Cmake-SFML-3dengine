#include "display.hpp"




//-------CONSTRUCTOR AND DESTRUCTOR--------

// Object constructor
Canvas::Canvas(int _w, int _h)
{
	m_renderer.create(_w, _h);

	for (int i = 0; i < m_n; i++)
	{
		m_renderer.clear(sf::Color::Transparent);
		m_renderer.display();
		m_layers[i] = sf::Texture();
		m_reset[i] = true;
	}
}

// Object destructor
Canvas::~Canvas()
{
	delete[] m_layers;
}

	

	

//------CANVAS FUNCTIONS-------

// Draw an sf::drawable to one of the layer textures
void Canvas::draw(int _layer, sf::Drawable& _sprite)
{
	m_renderer.clear(sf::Color::Transparent);
	m_renderer.draw(sf::Sprite(m_layers[_layer]));
	m_renderer.draw(_sprite);
	m_renderer.display();
	m_layers[_layer] = m_renderer.getTexture();
}


// Clear one of the layers (layers are automatically cleared every frame)                                                                       
void Canvas::clear(int _layer, sf::Color _clear)
{
	m_renderer.clear(_clear);
	m_renderer.display();
	m_layers[_layer] = m_renderer.getTexture();
}


// Set a layer to to clear every frame (true) or stay static (false)
void Canvas::reset(int _layer, bool reset)
{
	m_reset[_layer] = reset;
}


// Draw all of the layers to a given window
void Canvas::display(sf::RenderWindow& _window, int _x, int _y)
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
	m_sprite.setPosition(_x,_y);
	
	// Draw the sprite to the display
	_window.draw(m_sprite);


	// Clear the layers that need to be cleared
	m_renderer.clear(sf::Color::Transparent);
	for (int i = 0; i < m_n; i++)
	{
		if(m_reset[i] == true) m_layers[i] = m_renderer.getTexture();
	}
}





//-----INITIALIZE VIEW------

sf::Vector2i windowSetup(sf::RenderWindow& _window, int _height, bool _fullscreen = true, sf::String _title = "Window", int _fps = 60)
{
	
	//get the display dimmensions and calculate the aspect ratio
	float displayWidth = sf::VideoMode::getDesktopMode().width;
	float DisplayHeight = sf::VideoMode::getDesktopMode().height;
	float aspectRatio = displayWidth / DisplayHeight;

	//create a static window width in pixels and calculate the width based on the aspect ratio
	int bufferH = _height;
	int bufferW = abs(bufferH * aspectRatio);

	//center position of the window (needed for view setup)
	int bufferX = bufferW / 2;
	int bufferY = bufferH / 2;

	//initialize the view with the calculated resolution 
	if (_fullscreen) _window.create(sf::VideoMode(bufferW, bufferH), _title, sf::Style::Fullscreen);
	else _window.create(sf::VideoMode(bufferW, bufferH), _title, sf::Style::Default);

	//set the frame rate and hide the cursor so we can draw our own
	_window.setFramerateLimit(60);
	_window.setMouseCursorVisible(false);

	//set the size and position of the view
	sf::View view;
	view.setCenter(sf::Vector2f(bufferX, bufferY));
	view.setSize(sf::Vector2f(bufferW, bufferH));

	//asign the view to the window
	_window.setView(view);

	//return the calculated width and height of the window in pixels
	return sf::Vector2i(bufferW, bufferH);
}


