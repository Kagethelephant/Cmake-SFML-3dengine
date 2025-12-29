#pragma once

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <stdint.h>
#include <string>


class windowMaster{

   public:

   sf::RenderWindow window;
   
   // Resolution of the window
   sf::Vector2u resolution;
   // The framerate the window is currently running at
   float framerate;

   // Constructors
   windowMaster(unsigned int height, bool fullscreen = false);



   sf::Vector2f getMousePixel() { return window.mapPixelToCoords(sf::Mouse::getPosition(window));};

   void drawText(std::string t, float x, float y, int layer = 0);

   void draw(const sf::Drawable& drawable, int layer = 0);

   void render();



   private:

   sf::Font font;
   sf::Text text = sf::Text(font);
   sf::RectangleShape rectangle;

   std::uint8_t fps = 60;
   std::string title = "window";
   bool fullscreen;


   sf::Vector2u displayRes;
   sf::View view;

   int fpsCounter = 0;
   sf::Clock clock;

   sf::RenderTexture layers[10];
};
