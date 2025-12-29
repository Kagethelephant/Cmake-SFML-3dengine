#include <SFML/Graphics/RenderTexture.hpp>
#include <window.hpp>

#include <SFML/Graphics.hpp>
#include <math.h>

windowMaster::windowMaster(unsigned int height, bool fullscreen){

   // SETUP THE SFML WINDOW
   //---------------------------------------------------------------------------------------------
   // Get the display dimmensions and calculate the aspect ratio and window resolution
   displayRes = sf::VideoMode::getDesktopMode().size;
   float aspectRatio = ((float)displayRes.x / (float)displayRes.y);
   resolution =  {(unsigned int)std::fabs(height *aspectRatio), height};

   // Initialize the view with the calculated resolution and window mode
   if (fullscreen) window.create(sf::VideoMode::getDesktopMode(), title, sf::Style::None);
   else window.create(sf::VideoMode::getDesktopMode(), title, sf::Style::Resize);

   // Set the max frame rate and hide the cursor so we can draw our own
   window.setFramerateLimit(fps);
   window.setMouseCursorVisible(false);

   // Set the size and position of the view
   view.setCenter(sf::Vector2f(resolution.x/ 2.0f, resolution.y / 2.0f));
   view.setSize(sf::Vector2f(resolution.x, resolution.y));
   // Asign the view to the window
   window.setView(view);


   // SETUP REUSABLE SFML OBJECTS
   //---------------------------------------------------------------------------------------------
   // Setup the default font. Cast return value as void (we dont care about the return value)
   (void)font.openFromFile("../resources/font/small_pixel.ttf");
   font.setSmooth(false);
  
   // Setup the defualt text
   text.setCharacterSize(16);
   text.setFillColor(sf::Color::White);


   // CREATE ALL LAYERS (TEXTURES)
   //---------------------------------------------------------------------------------------------
   // Recreate all of the layers with the correct resolution
   for (sf::RenderTexture& layer : layers){
      layer = sf::RenderTexture(resolution);
   }
}

// Draw text with presets
void windowMaster::drawText(std::string t, float x, float y, int layer){
   text.setString(t);
   text.setPosition({x,y});
   layers[layer].draw(text);
}

// Draw sf::drawable to the given layer
void windowMaster::draw(const sf::Drawable& drawable, int layer){
   layers[layer].draw(drawable);
}

// Handle all of the window events, update fps, and draw everything to the screen
void windowMaster::render(){
   
   // Handle SFML window events
   while (const std::optional event = window.pollEvent())
   {
      if (event ->is<sf::Event::Closed>()) { window.close(); break;}
   }

   // Count the frames per second
   fpsCounter++;
   if (fpsCounter >= 30)
   {
      framerate = (int)(30 / (clock.getElapsedTime().asSeconds()));
      clock.restart();
      fpsCounter = 0;
   }

   //Draw everything in the rendertexture array
   for (sf::RenderTexture& layer : layers){

      layer.display();
      window.draw(sf::Sprite(layer.getTexture()));
   }
   // Display everything drawn to the buffer
   window.display();
}

