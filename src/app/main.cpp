
//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <ostream>
#include <string>
#include <iostream>
#include "display.hpp"
#include "utils/data.hpp"
#include "3d/obj3d.hpp"
#include "utils/random.hpp"
// #include <chrono>
#include <cmath>
#include <vector>



int main() {

   defineGlobal();
   randObj rander(13412234);

   //////////////////////////////////////////////////////////////////
   // Setup for SFML window and resolution
   //////////////////////////////////////////////////////////////////
   sf::Vector2f mousePos;
   sf::Vector2i resWindow;

   sf::RenderWindow window;
   resWindow = windowSetup(window, 1080, true, "CORONA",60);
   
   sf::RenderTexture rendWindow;
   rendWindow.create(resWindow.x,resWindow.y);

   camera cam;


   sf::Texture pixelBuff;
   pixelBuff.create(resWindow.x,resWindow.y);
   // Create a pixel buffer
   std::vector<sf::Uint8> pixels (resWindow.x * resWindow.y * 4, 255);
   
   int index = 0;
   for (int y=0; y<resWindow.y; y++){
      for (int x=0; x<resWindow.x; x++){
         index = (y*resWindow.x + x) *4;
         pixels[index] = 140;
         pixels[index + 1] = 50;
         pixels[index + 2] = 0;
         pixels[index + 3] = 255;
      }
   }

   pixelBuff.update(pixels.data());
   sf::Sprite sprite(pixelBuff);

   // ******************** CREATE 3D OBJECTS **********************
   // // TEST OBJECT
   // float h = 0.9999;
   // float w = 0.25;
   // std::vector<tri3d> axis;

   // axis.push_back(tri3d(vec3(h,h,0),vec3(h,h-w,0),vec3(h-w,h,0)));
   // axis.push_back(tri3d(vec3(h-w,h-w,0),vec3(h-w,h,0),vec3(h,h-w,0)));
   // axis.push_back(tri3d(vec3(-h,-h,0),vec3(-h,-h+w,0),vec3(-h+w,-h,0)));
   // axis.push_back(tri3d(vec3(-h+w,-h+w,0),vec3(-h+w,-h,0),vec3(-h,-h+w,0)));

   // axis.push_back(tri3d(vec3(-h,h,0),vec3(-h+w,h,0),vec3(-h,h-w,0)));
   // axis.push_back(tri3d(vec3(-h+w,h-w,0),vec3(-h,h-w,0),vec3(-h+w,h,0)));
   // axis.push_back(tri3d(vec3(h,-h,0),vec3(h-w,-h,0),vec3(h,-h+w,0)));
   // axis.push_back(tri3d(vec3(h-w,-h+w,0),vec3(h,-h+w,0),vec3(h-w,-h,0)));



   object3d object;
   object.load("../resources/objects/cow.obj");
   object.position = vec3(12,0,0);

   object3d object2;
   object2.load("../resources/objects/cow.obj");
   object2.position = vec3(0,0,0);

   object3d object3;
   object3.load("../resources/objects/cow.obj");
   object3.position = vec3(-12,0,0);


   //////////////////////////////////////////////////////////////////
   // DRAW STATIC
   //////////////////////////////////////////////////////////////////
   sf::Font fontRegular;
   fontRegular.loadFromFile("../resources/font/small_pixel.ttf");
   fontRegular.setSmooth(false);

   sf::Text textSmall;
  textSmall.setFont(fontRegular);
  textSmall.setCharacterSize(16);
  // textSmall.setFillColor(white);
  // textSmall.setPosition(5,5);


   //////////////////////////////////////////////////////////////////
   // WINDOW EVENT HANDLER
   //////////////////////////////////////////////////////////////////

   std::cout << "*****LOOP START*****" << std::endl;

   while (window.isOpen()) {

      bool up = 0, down = 0, right = 0, left = 0, space = 0, keyA = 0, keyD = 0, user_input = 0, keyB = 0;

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
               else if (event.key.code == sf::Keyboard::B) { keyB = 1; user_input = 1;}
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
      float move_y = 0;
      float rot_v = 0;

      move_z += (up - down)*.1;
      move_x += (left - right)*.1;
      move_y += (space - keyB)*.1;
      rot_v += (keyA - keyD)*.1;

      // Update if a key is pressed
      cam.update(move_x,move_y,move_z,0,rot_v,0);


      //////////////////////////////////////////////////////////////////
      // DRAW
      ////////////////////////////////////////////////////////////////// 
      // Move the rectangle to the correct position before drawing
      rendWindow.clear(black);

      rendWindow.draw(sprite);

      object.draw (rendWindow,resWindow,cam,red);
      object2.draw (rendWindow,resWindow,cam,yellow);
      object3.draw (rendWindow,resWindow,cam,green);

      textSmall.setString("X: "+std::to_string(cam.position.x));
      textSmall.setPosition(10,10);
      rendWindow.draw(textSmall);
      textSmall.setString("Y: "+std::to_string(cam.position.y));
      textSmall.setPosition(10,30);
      rendWindow.draw(textSmall);
      textSmall.setString("Z: "+std::to_string(cam.position.z));
      textSmall.setPosition(10,50);
      rendWindow.draw(textSmall);
      textSmall.setString("V: "+std::to_string(cam.rotation.y));
      textSmall.setPosition(10,70);
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
