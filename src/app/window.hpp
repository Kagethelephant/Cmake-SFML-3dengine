#pragma once

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <stdint.h>
#include <string>


class windowMaster{
   public:

   sf::RenderWindow window;
   std::uint8_t fps = 60;
   std::string title = "window";
   bool fullscreen;

   sf::Vector2u resolution;
   sf::Vector2u displayRes;
   sf::View view;
   
   sf::Font font;
   sf::Text text = sf::Text(font);
   sf::RectangleShape rectangle;

   int fpsCounter = 0;
   float framerate;
   sf::Time Time;
   sf::Clock clock;

   sf::RenderTexture layers[10];

   // Constructors
   windowMaster(unsigned int height, bool fullscreen);

   void handleEvents();

   sf::Vector2f getMousePixel() { return window.mapPixelToCoords(sf::Mouse::getPosition(window));};

   void drawText(std::string t, float x, float y, int layer = 0);

   void draw(const sf::Drawable& drawable, int layer = 0);

   void render();

   private:

};
