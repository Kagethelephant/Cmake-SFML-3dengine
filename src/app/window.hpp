#pragma once

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <stdint.h>
#include <string>


/// @brief: Handles the SFML window and simplifies drawing simple items like text and rectangles
/// @param height: Height of window in pixels, the width is determined by the aspect ratio of the display
/// @param fullscreen: Boolean for fullscreen (default: false)
class windowMaster{

public:

   /// @brief: SFML window object
   sf::RenderWindow window;
   /// @brief: Resolution of the window
   sf::Vector2u resolution;
   /// @brief: Current framerate of the program
   float framerate;


   windowMaster(unsigned int height, bool fullscreen = false);

   /// @brief: Gets the position of the mouse in pixels in the window
   /// @return: 2D vector with x and y position of the mouse
   sf::Vector2f getMousePixel() { return window.mapPixelToCoords(sf::Mouse::getPosition(window));};

   /// @brief: Function to draw text to the window with a single line
   /// @param t: Text to be drawn as std::string
   /// @param x: x position in pixels
   /// @param y: x position in pixels
   /// @param layer: Texture layer to draw the text to (default: 0)
   void drawText(std::string t, float x, float y, int layer = 0);

   /// @brief: Draws an sf::drawable the same way a standard SFML window would but with optional layers
   /// @param drawable: sf::drawable to draw 
   /// @param layer: Texture layer to draw the text to (default: 0)
   void draw(const sf::Drawable& drawable, int layer = 0);

   /// @brief: Draws all layer textures to the renderWindow and displays it to the window
   void render();



private:

   /// @brief: View for the SFML window (only view)
   sf::View m_view;
   /// @brief: RenderTexture array so we can create layers (max 10)
   sf::RenderTexture m_layers[10];

   
   /// @brief: SFML font to use
   sf::Font m_font;
   /// @brief: Text object used to draw all text
   sf::Text m_text = sf::Text(m_font);
   /// @brief: Rectangle object used to draw all rectangles
   sf::RectangleShape m_rectangle;


   /// @brief: Max framerate
   std::uint8_t m_framerateLimit = 60;
   /// @brief: Title of the window
   std::string m_title = "SFML-GPU";


   /// @brief: Counts frames to calculate framerate
   int m_fpsCounter = 0;
   /// @brief: Real time clock to calculate framerate
   sf::Clock m_clock;

};
