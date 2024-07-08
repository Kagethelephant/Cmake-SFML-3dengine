#include "display.hpp"




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


