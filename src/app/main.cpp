#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <ostream>
#include <string>
#include <iostream>
#include "data.hpp"
#include "obj3d.hpp"
#include "camera.hpp"
#include "utils.hpp"
#include <window.hpp>



int main(int argc, char* argv[])
{
   // Defines all of the colors (global so we dont have to rewrite all of the colors)
   defineGlobal();
   // Global random number generator (global so everything shares the same seed)
   randObj rander(false, 13412234);
   
   // Create the windowMaster that handles all of the background SFML window stuff and simplifies drawing
   windowMaster game(1080,false);
   // Camera handles all of the 3d rendering
   camera cam(game.resolution);
   
   // Create 3d objects. Need to be alocated on the stach so we have a defined locations for their triangles to point back to
   object3d* object = new object3d("../resources/objects/cow.obj", white, black, vec3(-12, 0, 0));
   object3d* object2 = new object3d("../resources/objects/cow.obj", red, white, vec3(12, 0, 0));
   object3d* object3 = new object3d("../resources/objects/cow.obj", green, yellow, vec3(0, 0, 0));

   std::cout << "*****LOOP START*****" << std::endl;

   while (game.window.isOpen())
   {
      // Get user input
      bool up, down, right, left, space, keyA, keyD, user_input, keyB;

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)){game.window.close();}
      up = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);
      down = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down);
      right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
      left = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
      keyB = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::B);
      keyA = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
      keyD = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);

      // Update position of camera based on input
      cam.update((left-right)*.1, (space-keyB)*.1, (up-down)*.1, 0, (keyA-keyD)*.1, 0);

      // Reload all of the objects into the camera incase they moved
      cam.loadObject(*object);
      cam.loadObject(*object2);
      cam.loadObject(*object3);
      // Draw the objects to the camera texture
      cam.draw();
      // Draw the camera texture to the window
      game.draw(sf::Sprite(cam.pixelBuff));

      // Draw some text to the window
      game.drawText("X: " + std::to_string(cam.position.x), 10, 10);
      game.drawText("Y: " + std::to_string(cam.position.y), 10, 30);
      game.drawText("Z: " + std::to_string(cam.position.z), 10, 50);
      game.drawText("FPS: " + std::to_string(game.framerate), 10, 70);

      // Display everything that was drawn to the screen
      game.render();
   }
   // Free memory we allocated on the heap
   delete object; 
   delete object2; 
   delete object3; 

   std::cout << "*******PROGRAM TERMINATED******* " << std::endl;

   return 0;
}
