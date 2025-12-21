#include "display.hpp"

#include <SFML/Graphics.hpp>
#include <cmath>


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// This is just to move the window setup stuff out of the main loop because all of this will generally be the same
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
sf::Vector2u windowSetup(sf::RenderWindow& window, int height, bool fullscreen, sf::String title, int fps) {

   // Get the display dimmensions and calculate the aspect ratio and screen resolution
   sf::Vector2u size = sf::VideoMode::getDesktopMode().size;
   float aspectRatio = ((float)size.x / (float)size.y);
   sf::Vector2u res (std::fabs(height *aspectRatio), height);

   // Initialize the view with the calculated resolution 
   // If not fullscreen then we need to set the window size as well as the resolution
   if (fullscreen) window.create(sf::VideoMode::getDesktopMode(), title, sf::Style::None);
   else window.create(sf::VideoMode::getDesktopMode(), title, sf::Style::Resize);

   // Set the frame rate and hide the cursor so we can draw our own
   window.setFramerateLimit(fps);
   // window.setVerticalSyncEnabled(true);
   window.setMouseCursorVisible(false);

   // Set the size and position of the view
   sf::View view;
   view.setCenter(sf::Vector2f(res.x/ 2.0f, res.y / 2.0f));
   view.setSize(sf::Vector2f(res.x, res.y));
   // Asign the view to the window
   window.setView(view);

   // Return the calculated width and height of the window in pixels
   return res;
}


