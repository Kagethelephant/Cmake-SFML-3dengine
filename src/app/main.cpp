#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <ostream>
#include <string>
#include <iostream>
#include <vector>
#include "data.hpp"
#include "obj3d.hpp"
#include "camera.hpp"
#include "utils.hpp"
#include <window.hpp>



int main(int argc, char* argv[])
{

   defineGlobal();
   randObj rander(false, 13412234);

   //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
   // Setup for SFML window and resolution
   //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
   windowMaster game(1080,false);
   
   camera cam;

   sf::Texture pixelBuff(game.resolution);
   // Create a pixel buffer
   std::vector<std::uint8_t> pixels(game.resolution.x * game.resolution.y * 4, 255);
   std::vector<std::uint8_t> bg = pixels;

   int index = 0;
   for (int y = 0; y < game.resolution.y; y++)
   {
      for (int x = 0; x < game.resolution.x; x++)
      {
         index = (y * game.resolution.x + x) * 4;
         bg[index] = black.r;
         bg[index + 1] = black.g;
         bg[index + 2] = black.b;
         bg[index + 3] = black.a;
      }
   }


   // CREATE 3D OBJECTS
   //---------------------------------------------------------------------------------------------
   object3d* object = new object3d("../resources/objects/cow.obj", white, black, vec3(-12, 0, 0));
   object3d* object2 = new object3d("../resources/objects/cow.obj", red, white, vec3(12, 0, 0));
   object3d* object3 = new object3d("../resources/objects/cow.obj", green, yellow, vec3(0, 0, 0));

   //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
   // WINDOW EVENT HANDLER
   //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
   std::cout << "*****LOOP START*****" << std::endl;

   while (game.window.isOpen())
   {

      game.handleEvents();
      //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
      // UPDATE
      //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
      // Create a vector with the pixel coord's in the actual window not the display
      bool up, down, right, left, space, keyA, keyD, user_input, keyB;

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)){game.window.close();}
      up = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);
      down = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down);
      right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
      left = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
      keyB = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::B);
      keyA = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
      keyD = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);

      // Update if a key is pressed
      cam.update((left-right)*.1, (space-keyB)*.1, (up-down)*.1, 0, (keyA-keyD)*.1, 0);


      //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
      // DRAW
      //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
      // Move the rectangle to the correct position before drawing
      // rendWindow.clear(sf::Color(black.x,black.y,black.z,black.w));

      pixelBuff.update(pixels.data());
      game.draw(sf::Sprite(pixelBuff));

      pixels = bg;
      
      cam.loadObject(*object);
      cam.loadObject(*object2);
      cam.loadObject(*object3);
      cam.draw(pixels, game.resolution);

      game.drawText("X: " + std::to_string(cam.position.x), 10, 10);
      game.drawText("Y: " + std::to_string(cam.position.y), 10, 30);
      game.drawText("Z: " + std::to_string(cam.position.z), 10, 50);
      game.drawText("FPS: " + std::to_string(game.framerate), 10, 70);

      //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
      // DISPLAY TO SCREEN
      //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
      game.render();
   }

   delete object; 
   delete object2; 
   delete object3; 
   //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
   // DEBUG OUTPUTS
   //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

   std::cout << "*******PROGRAM TERMINATED******* " << std::endl;

   return 0;
}
