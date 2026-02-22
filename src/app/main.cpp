// Opengl
#include <glad/glad.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "object.hpp"
#include "cpuRender/cpuRender.hpp"
#include "gpuRender/text.hpp"
#include "gpuRender/window.hpp"
#include "gpuRender/gpuRender.hpp"

#include "utils/matrix.hpp"
#include "utils/utils.hpp"
#include <math.h>
#include <string>

int main(int argc, char* argv[])
{

   // Global random number generator (global so everything shares the same seed)
   randObj rander(false, 13412234);
   bool ogl = true;

   // ----------------------------- CREATE WINDOW AND OpenGL CONEXT -------------------------------
   //Create scope here so objects can call destructors on open gl objects before opengl is terminated
   {
      window gl_window(400);
      camera cam(gl_window);

      model yoshi("../resources/objects/yoshi/yoshi.obj",true);
      model Arcanine("../resources/objects/Arcanine/Arcanine.obj",true);

      light light1 = createLight(vec3(15,5,5),vec3(0.6,0.3,0.3));
      light light2 = createLight(vec3(-15,5,5),vec3(0.3,0.3,0.6));

      object yoshi1(yoshi);
      object arcanine1(Arcanine);
      arcanine1.move(-10,0,-10);
      arcanine1.scale(10,10,10);
      yoshi1.move(0,0,-10);
      yoshi1.rotate(0,5,0);


      gpuRenderObject gpuRend(cam);
      gpuRend.addLight(light1);
      gpuRend.addLight(light2);

      textEngine text;
      text.loadFont("../resources/font/small_pixel.ttf");

      GLuint shaderProgramUI = createShaderProgram("../src/shaders/text_vertex.glsl", "../src/shaders/text_fragment.glsl");


      gpuRend.bindObject(yoshi1);
      gpuRend.bindObject(arcanine1);

      double lastTime = glfwGetTime();
      int frameCount = 0;
      int fps;


      double movement;
      double rotate;
      double speed = 10.0f;
      double rotation = 3.0f;


      cpuRenderObject cpuRend(cam);
      cpuRend.addLight(light1);
      cpuRend.addLight(light2);

      bool blocked = false;
      bool enterPreviouslyPressed = false;
      // ------------------------------ MAIN WINDOW LOOP ---------------------------------
      // Main loop for the window
      while(!glfwWindowShouldClose(gl_window.win)){
         // Get keyboard inputs
         if (glfwGetKey(gl_window.win, GLFW_KEY_ESCAPE) == GLFW_PRESS) {glfwSetWindowShouldClose(gl_window.win, true);}

         bool enterPressed = glfwGetKey(gl_window.win, GLFW_KEY_ENTER) == GLFW_PRESS;
         if (enterPressed && !enterPreviouslyPressed) {ogl = !ogl;}
         enterPreviouslyPressed = enterPressed;


         double currentTime = glfwGetTime();
         frameCount++;
         // If a second has passed
         if (currentTime - lastTime >= 0.2f)
         {
            // Calculate FPS and display it (e.g., in the window title or console)
            fps = (int)frameCount / (currentTime - lastTime);
            movement = speed /fps;
            rotate = rotation /fps;
            frameCount = 0;
            lastTime = currentTime;
         }


         if (glfwGetKey(gl_window.win, GLFW_KEY_S) == GLFW_PRESS) {cam.move(0, 0, -movement);}
         if (glfwGetKey(gl_window.win, GLFW_KEY_W) == GLFW_PRESS) {cam.move(0, 0,  movement);}
         if (glfwGetKey(gl_window.win, GLFW_KEY_A) == GLFW_PRESS) {cam.rotate(0,  -rotate, 0);}
         if (glfwGetKey(gl_window.win, GLFW_KEY_D) == GLFW_PRESS) {cam.rotate(0,   rotate, 0);}


         if(!ogl){

            cpuRend.render(yoshi1);
            cpuRend.render(arcanine1);
            // // Draw the camera 2D projection to the window
            gl_window.draw(cpuRend.m_pixelBuffer);
            cpuRend.draw();
         }
         else {
            // vao.bindRender();
            gpuRend.render();

         }

         text.RenderText(shaderProgramUI,gl_window,"X: " + std::to_string(cam.position[0]), 10, 10);
         text.RenderText(shaderProgramUI,gl_window,"Y: " + std::to_string(cam.position[1]), 10, 20);
         text.RenderText(shaderProgramUI,gl_window,"Z: " + std::to_string(cam.position[2]), 10, 30);
         text.RenderText(shaderProgramUI,gl_window,"FPS: " + std::to_string(fps), 10, 40);

         if(ogl)text.RenderText(shaderProgramUI,gl_window,"GPU", gl_window.fboWidth/2.0f, 10);
         else text.RenderText(shaderProgramUI,gl_window,"CPU", gl_window.fboWidth/2.0f, 10);
         // text.RenderText(shaderProgramUI,vao.fbo.fbo,"Hello", 10, 10, window.width, window.height);

         gl_window.draw();

         glfwSwapBuffers(gl_window.win);
         glfwPollEvents();

         gl_window.resize();
      }
      // text.cleanup();
      glfwDestroyWindow(gl_window.win);
      glfwSetWindowShouldClose(gl_window.win, true);
      glfwMakeContextCurrent(nullptr);
   }
   // Clear all of the GLFW assets
   glfwTerminate();




   return 0;
}

