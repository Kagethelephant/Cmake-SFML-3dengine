#include <SFML/Graphics/RenderTexture.hpp>
#include <window.hpp>

#include <SFML/Graphics.hpp>
#include <math.h>

windowMaster::windowMaster(unsigned int height, bool fullscreen){

   // SETUP THE SFML WINDOW
   //---------------------------------------------------------------------------------------------
   // Get the display dimmensions and calculate the aspect ratio and screen resolution
   displayRes = sf::VideoMode::getDesktopMode().size;
   float aspectRatio = ((float)displayRes.x / (float)displayRes.y);
   resolution =  {(unsigned int)std::fabs(height *aspectRatio), height};

   // Initialize the view with the calculated resolution 
   // If not fullscreen then we need to set the window size as well as the resolution
   if (fullscreen) window.create(sf::VideoMode::getDesktopMode(), title, sf::Style::None);
   else window.create(sf::VideoMode::getDesktopMode(), title, sf::Style::Resize);

   // Set the frame rate and hide the cursor so we can draw our own
   window.setFramerateLimit(fps);
   // window.setVerticalSyncEnabled(true);
   window.setMouseCursorVisible(false);

   // Set the size and position of the view
   view.setCenter(sf::Vector2f(resolution.x/ 2.0f, resolution.y / 2.0f));
   view.setSize(sf::Vector2f(resolution.x, resolution.y));
   // Asign the view to the window
   window.setView(view);


   // SETUP REUSABLE SFML OBJECTS
   //---------------------------------------------------------------------------------------------
   // Cast return value as void (we dont care about the return value)
   (void)font.openFromFile("../resources/font/small_pixel.ttf");
   font.setSmooth(false);
   
   text.setCharacterSize(16);
   text.setFillColor(sf::Color::White);

   // Recreate all of the layers with the correct resolution
   for (sf::RenderTexture& layer : layers){
      layer = sf::RenderTexture(resolution);
   }
}


void windowMaster::handleEvents(){

   bool user_input;
   while (const std::optional event = window.pollEvent())
   {
      user_input = 0;
      if (event ->is<sf::Event::Closed>()) 
      {
         window.close();
         break;
      }

      else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
      {
         user_input = 1;
      }
   }

   fpsCounter++;
   if (fpsCounter >= 30)
   {
      Time = clock.getElapsedTime();
      clock.restart();
      framerate = (int)(30 / (Time.asSeconds()));
      fpsCounter = 0;
   }
}

void windowMaster::drawText(std::string t, float x, float y, int layer){
   text.setString(t);
   text.setPosition({x,y});
   layers[layer].draw(text);
}

void windowMaster::draw(const sf::Drawable& drawable, int layer){
   layers[layer].draw(drawable);
}

void windowMaster::render(){

   for (sf::RenderTexture& layer : layers){

      layer.display();
      window.draw(sf::Sprite(layer.getTexture()));
   }
   window.display();
}

