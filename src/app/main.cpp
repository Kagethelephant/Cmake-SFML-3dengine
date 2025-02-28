
//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <ostream>
#include <string>
#include <iostream>
#include "display.hpp"
#include "utils/data.hpp"
#include "3d/obj3d.hpp"
#include "utils/random.hpp"
// #include <chrono>
#include <cmath>



int main() {

   defineGlobal();
   randObj rander(13412234);

   //////////////////////////////////////////////////////////////////
   // Setup for SFML window and resolution
   //////////////////////////////////////////////////////////////////
   sf::Vector2f mousePos;
   sf::Vector2i resWindow;

   sf::RenderWindow window;
   resWindow = windowSetup(window, 400, false, "CORONA",60);
   
   sf::RenderTexture rendWindow;
   rendWindow.create(resWindow.x,resWindow.y);

   camera cam;

   // ******************** CREATE 3D OBJECTS **********************
   object3d object;
   object.load("../resources/objects/cow.obj");
   object.z = 15;
   object.x = 20;

   object3d object2;
   object2.load("../resources/objects/cow.obj");
   object2.z = 10;
   object2.x = 0;

   object3d object3;
   object3.load("../resources/objects/cow.obj");
   object3.z = 5;
   object3.x = -10;


   //////////////////////////////////////////////////////////////////
   // DRAW STATIC
   //////////////////////////////////////////////////////////////////
   sf::Font fontRegular;
   fontRegular.loadFromFile("../resources/font/small_pixel.ttf");
   fontRegular.setSmooth(false);

   sf::Text textSmall;
  textSmall.setString("Hello World");
  textSmall.setFont(fontRegular);
  textSmall.setFillColor(white);
  textSmall.setCharacterSize(8);
  textSmall.setPosition(5,5);


   //////////////////////////////////////////////////////////////////
   // WINDOW EVENT HANDLER
   //////////////////////////////////////////////////////////////////

   std::cout << "*****LOOP START*****" << std::endl;

   while (window.isOpen()) {

      bool up = 0, down = 0, right = 0, left = 0, space = 0, keyA = 0, keyD = 0, user_input = 0;

      // Event handler
      sf::Event event; 
      while (window.pollEvent(event)) {

         switch (event.type) {
            // If the X window button is pressed exit
            case sf::Event::Closed:
               window.close();
               break;
            // Keyboard input
            case sf::Event::KeyPressed:
               if (event.key.code == sf::Keyboard::Escape) { window.close(); }
               else if (event.key.code == sf::Keyboard::Up) { up = 1; user_input = 1;}
               else if (event.key.code == sf::Keyboard::Down) { down = 1; user_input = 1;}
               else if (event.key.code == sf::Keyboard::Right) { right = 1; user_input = 1;}
               else if (event.key.code == sf::Keyboard::Left) { left = 1; user_input = 1;}
               else if (event.key.code == sf::Keyboard::Space) { space = 1; user_input = 1;}
               else if (event.key.code == sf::Keyboard::A) { keyA = 1; user_input = 1;}
               else if (event.key.code == sf::Keyboard::D) { keyD = 1; user_input = 1;}
               break;
            default:
               // Do nothing
               break;
         }
      }



      //////////////////////////////////////////////////////////////////
      // UPDATE
      //////////////////////////////////////////////////////////////////    
      // Create a vector with the pixel coord's in the actual window not the display
      mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

      float move_x = 0;
      float move_z = 0;
      float rot_v = 0;

      move_z += (up - down)*.1;
      move_x += (left - right)*.1;
      rot_v += (keyA - keyD)*.1;

      // Update if a key is pressed
      cam.update(move_x,0,move_z,0,rot_v,0);


      //////////////////////////////////////////////////////////////////
      // DRAW
      ////////////////////////////////////////////////////////////////// 
      // Move the rectangle to the correct position before drawing
      rendWindow.clear(black);

      object.draw (rendWindow,resWindow,cam,red);
      object2.draw (rendWindow,resWindow,cam,yellow);
      object3.draw (rendWindow,resWindow,cam,green);

      textSmall.setString("X: "+std::to_string(cam.position.x));
      textSmall.setPosition(5,10);
      rendWindow.draw(textSmall);
      textSmall.setString("Y: "+std::to_string(cam.position.y));
      textSmall.setPosition(5,20);
      rendWindow.draw(textSmall);
      textSmall.setString("Z: "+std::to_string(cam.position.z));
      textSmall.setPosition(5,30);
      rendWindow.draw(textSmall);
      textSmall.setString("V: "+std::to_string(cam.rotation.y));
      textSmall.setPosition(5,40);
      rendWindow.draw(textSmall);

      //////////////////////////////////////////////////////////////////
      // DISPLAY TO SCREEN
      //////////////////////////////////////////////////////////////////
      // Display canvas layers to screen
      rendWindow.display();
      window.draw(sf::Sprite(rendWindow.getTexture()));
      window.display();
   }

   //////////////////////////////////////////////////////////////////
   // DEBUG OUTPUTS
   /////////////////////////////////////////////////////////////////

   std::cout << "*******END******* "<< std::endl;

   return 0;
}
