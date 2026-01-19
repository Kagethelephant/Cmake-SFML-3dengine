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

#include <glad/glad.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <string>
#include "freeType.hpp"
#include "gl.hpp"
#include "glObject.hpp"
#include "matrix.hpp"
#include <iostream>

int main(int argc, char* argv[])
{

   std::string opengl;
   std::cout << "OpenGL?" << std::endl;
   std::cin >> opengl;

   if(opengl == "N" || opengl == "n"){

      // Global random number generator (global so everything shares the same seed)
      randObj rander(false, 13412234);

      // Create windowMaster that handles all of the background SFML window stuff and simplifies drawing
      windowMaster game(400,false);
      // Camera handles all of the 3d rendering
      camera cam(game.resolution, 70);

      // Create the 3D objects to be rendered
      object3d object("../resources/objects/cow.obj", vec3(-12, 0, -10), vec3(1, 1, 1), sf::Color(ColorToHex(Color::White)));
      object3d object2("../resources/objects/cow.obj", vec3(12, 0, -10), vec3(1, 1, 1), sf::Color(ColorToHex(Color::Red)));
      object3d object3("../resources/objects/cow.obj", vec3(0, 0, -10), vec3(1, 1, 1), sf::Color(ColorToHex(Color::Blue)));
      object3d teapot("../resources/objects/teapot.obj", vec3(-10, 0, 5), vec3(.01, .01, .01), sf::Color(ColorToHex(Color::Green)), sf::Color(ColorToHex(Color::Transperant)), true);

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
         game.drawText("X: " + std::to_string(cam.position[0]), 10, 10);
         game.drawText("Y: " + std::to_string(cam.position[1]), 10, 20);
         game.drawText("Z: " + std::to_string(cam.position[2]), 10, 30);
         game.drawText("FPS: " + std::to_string(game.framerate), 10, 40);

         // Display everything that was drawn to the screen
         game.render();
      }

      std::cout << "*******PROGRAM TERMINATED******* " << std::endl;
   }
   else {


      // ----------------------------- CREATE WINDOW AND OpenGL CONEXT -------------------------------
      //Create scope here so objects can call destructors on open gl objects before opengl is terminated
      {

         gl_window window(400);

         gl_vertexObject vao(window);

         unsigned int cowModel = vao.createModel("../resources/objects/cow.obj");
         vao.bindObjects();

         object object1(cowModel);
         object object2(cowModel);
         object object3(cowModel);
         object1.move(-12,0,-10); 
         object2.move(0,0,-10); 
         object3.move(12,0,-10); 
         object1.color = hexColorToFloat(Color::White);
         object2.color = hexColorToFloat(Color::Blue);
         object3.color = hexColorToFloat(Color::Red);

         vec3 lightPos(0.0f,0.0f,0.0f);
         vec3 lightColor(1.6f,1.0f,1.8f);
         vec3 objColor(0.6f,0.4f,0.2f);

         textEngine text;
         text.loadFont("../resources/font/small_pixel.ttf");

         GLuint shaderProgramUI = createShaderProgram("../src/glShaders/textVertex.glsl", "../src/glShaders/textFragment.glsl");

         double lastTime = glfwGetTime();
         int frameCount = 0;
         int fps;

         // ------------------------------ MAIN WINDOW LOOP ---------------------------------
         // Main loop for the window
         while(!glfwWindowShouldClose(window.window)){
            // Get keyboard inputs
            if (glfwGetKey(window.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window.window, true);
            if (glfwGetKey(window.window, GLFW_KEY_S) == GLFW_PRESS) {vao.move(0, 0, 0.1);}
            if (glfwGetKey(window.window, GLFW_KEY_W) == GLFW_PRESS) {vao.move(0, 0, -0.1);}
            if (glfwGetKey(window.window, GLFW_KEY_A) == GLFW_PRESS) {vao.rotate(0, -0.05, 0);}
            if (glfwGetKey(window.window, GLFW_KEY_D) == GLFW_PRESS) {vao.rotate(0, 0.05, 0);}

            vao.bindRender();

            vao.render(object1);
            vao.render(object2);
            vao.render(object3);

            double currentTime = glfwGetTime();
            frameCount++;
            // If a second has passed
            if (currentTime - lastTime >= 1.0)
            {
               // Calculate FPS and display it (e.g., in the window title or console)
               fps = (int)frameCount / (currentTime - lastTime);
               frameCount = 0;
               lastTime = currentTime;
            }

            text.RenderText(shaderProgramUI,window,"X: " + std::to_string(vao.camPosition[0]), 10, 10);
            text.RenderText(shaderProgramUI,window,"Y: " + std::to_string(vao.camPosition[1]), 10, 20);
            text.RenderText(shaderProgramUI,window,"Z: " + std::to_string(vao.camPosition[2]), 10, 30);
            text.RenderText(shaderProgramUI,window,"FPS: " + std::to_string(fps), 10, 40);
            // text.RenderText(shaderProgramUI,vao.fbo.fbo,"Hello", 10, 10, window.width, window.height);

            vao.draw();

            glfwSwapBuffers(window.window);
            glfwPollEvents();

            window.resize();
         }

         // text.cleanup();
         glfwDestroyWindow(window.window);
      }
      // Clear all of the GLFW assets
      glfwTerminate();

   }


   return 0;
}

