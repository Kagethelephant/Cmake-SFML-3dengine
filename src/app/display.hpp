#pragma once

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Headers
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#include <SFML/Graphics.hpp>


/// @brief Sets up a simple sfml window and view 
/// @param window: sfml render window passed by reference to modify
/// @param height: height of the window in pixels, the width will be determined by the aspect ratio
/// @param fullscreen: if window should be fullscreen. Default = false
/// @param title: title of the window. Default = Window
/// @param fps: frames per second of the window. Default = 60
/// @return SFML 2d unsigned integer vector with the pixel height and width of the window
sf::Vector2u windowSetup(sf::RenderWindow& window, int height, bool fullscreen = true, sf::String title = "Window", int fps = 60);

