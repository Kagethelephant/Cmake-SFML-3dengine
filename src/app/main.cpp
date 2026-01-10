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
   // Global random number generator (global so everything shares the same seed)
   randObj rander(false, 13412234);
   
   // Create windowMaster that handles all of the background SFML window stuff and simplifies drawing
   windowMaster game(400,false);
   // Camera handles all of the 3d rendering
   camera cam(game.resolution, 70);
  
   // Create the 3D objects to be rendered
   object3d object("../resources/objects/cow.obj", vec3(-12, 0, 0), vec3(1, 1, 1), white);
   object3d object2("../resources/objects/cow.obj", vec3(12, 0, 0), vec3(1, 1, 1), red);
   object3d object3("../resources/objects/cow.obj", vec3(0, 0, 0), vec3(1, 1, 1), green);
   object3d teapot("../resources/objects/teapot.obj", vec3(-10, 0, -10), vec3(.01, .01, .01), blue, sf::Color::Transparent, true);

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
      cam.move((left-right)*.1, (space-keyB)*.1, (up-down)*.1, 0, (keyA-keyD)*.1, 0);

      // Object meshes are cleared from camera every frame so we need to load them each frame
      cam.loadObject(object);
      cam.loadObject(object2);
      cam.loadObject(object3);
      cam.loadObject(teapot);
      // Update the 2D projection
      cam.update();
      // Draw the camera 2D projection to the window
      game.draw(cam);

      // Draw some text to the window
      game.drawText("X: " + std::to_string(cam.position.x), 10, 10);
      game.drawText("Y: " + std::to_string(cam.position.y), 10, 20);
      game.drawText("Z: " + std::to_string(cam.position.z), 10, 30);
      game.drawText("FPS: " + std::to_string(game.framerate), 10, 40);

      // Display everything that was drawn to the screen
      game.render();
   }

   std::cout << "*******PROGRAM TERMINATED******* " << std::endl;

   return 0;
}
