#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <window.hpp>

#include <SFML/Graphics.hpp>
#include <math.h>


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// CONSTRUCTOR
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
windowMaster::windowMaster(unsigned int height, bool fullscreen){

   // SETUP THE SFML WINDOW
   //---------------------------------------------------------------------------------------------
   // Get the display dimmensions and calculate the aspect ratio and window resolution
   sf::Vector2u displayRes = sf::VideoMode::getDesktopMode().size;
   float aspectRatio = ((float)displayRes.x / (float)displayRes.y);
   resolution =  {(unsigned int)std::fabs(height *aspectRatio), height};

   // Initialize the view with the calculated resolution and window mode
   if (fullscreen) window.create(sf::VideoMode::getDesktopMode(), m_title, sf::Style::None);
   else window.create(sf::VideoMode::getDesktopMode(), m_title, sf::Style::Resize);

   // Set the max frame rate and hide the cursor so we can draw our own
   window.setFramerateLimit(m_framerateLimit);
   window.setMouseCursorVisible(false);

   // Set the size and position of the view
   m_view.setCenter(sf::Vector2f(resolution.x/ 2.0f, resolution.y / 2.0f));
   m_view.setSize(sf::Vector2f(resolution.x, resolution.y));
   // Asign the view to the window
   window.setView(m_view);

   // SETUP REUSABLE SFML OBJECTS
   //---------------------------------------------------------------------------------------------
   // Setup the default font. Cast return value as void (we dont care about the return value)
   (void)m_font.openFromFile("../resources/font/small_pixel.ttf");
   m_font.setSmooth(false);
  
   // Setup the defualt text
   m_text.setCharacterSize(8);
   m_text.setFillColor(sf::Color::White);

   // CREATE ALL LAYERS (TEXTURES)
   //---------------------------------------------------------------------------------------------
   // Recreate all of the layers with the correct resolution
   for (sf::RenderTexture& layer : m_layers){
      layer = sf::RenderTexture(resolution);
   }
}


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// RENDER TO WINDOW
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void windowMaster::render(){
   
   // Handle SFML window events
   while (const std::optional event = window.pollEvent()){
      if (event ->is<sf::Event::Closed>()) { window.close(); break;}
   }

   // Count the frames per second
   m_fpsCounter++;
   if (m_fpsCounter >= 5){
      framerate = std::trunc((5 / (m_clock.getElapsedTime().asSeconds())));
      m_clock.restart();
      m_fpsCounter = 0;
   }

   //Draw everything in the rendertexture array
   for (sf::RenderTexture& layer : m_layers){
      layer.display();
      window.draw(sf::Sprite(layer.getTexture()));
   }
   // Display everything drawn to the buffer
   window.display();
}


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// DRAW SFML OBJECTS
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void windowMaster::draw(const sf::Drawable& drawable, int layer){
   m_layers[layer].draw(drawable);
}

void windowMaster::drawText(std::string t, float x, float y, int layer){
   m_text.setString(t);
   m_text.setPosition({x,y});
   m_layers[layer].draw(m_text);
}
