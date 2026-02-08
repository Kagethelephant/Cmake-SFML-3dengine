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



   unsigned int cowModel;
   unsigned int potModel;
   unsigned int cowModelcpu;
   unsigned int potModelcpu;

   // Create the 3D objects to be rendered
   object cow1(&cowModel);
   object cow2(&cowModel);
   object cow3(&cowModel);
   object teapot(&potModel);

   object cow1cpu(&cowModelcpu);
   object cow2cpu(&cowModelcpu);
   object cow3cpu(&cowModelcpu);
   object teapotcpu(&potModelcpu);
   cow1.color = Color::White;
   cow2.color = Color::Red;
   cow3.color = Color::Blue;
   teapot.color = Color::Green;
   cow1.move(-12, 0, -10);
   cow2.move(0, 0, -10);
   cow3.move(12, 0, -10);
   teapot.scale(.02,.02,.02);
   teapot.move(-12, 0, 0);

   cow1cpu.color = Color::White;
   cow2cpu.color = Color::Red;
   cow3cpu.color = Color::Blue;
   teapotcpu.color = Color::Green;
   cow1cpu.move(-12, 0, -10);
   cow2cpu.move(0, 0, -10);
   cow3cpu.move(12, 0, -10);
   teapotcpu.scale(.02,.02,.02);
   teapotcpu.move(-12, 0, 0);

   // Global random number generator (global so everything shares the same seed)
   randObj rander(false, 13412234);


   bool ogl = true;



   // ----------------------------- CREATE WINDOW AND OpenGL CONEXT -------------------------------
   //Create scope here so objects can call destructors on open gl objects before opengl is terminated
   {

      gl_window window(400);

      gl_vertexObject vao(window);

      cowModel = vao.createModel("../resources/objects/cow.obj");
      potModel = vao.createModel("../resources/objects/teapot.obj");
      vao.bindObjects();

      textEngine text;
      text.loadFont("../resources/font/small_pixel.ttf");

      GLuint shaderProgramUI = createShaderProgram("../src/shaders/text_vertex.glsl", "../src/shaders/text_fragment.glsl");

      double lastTime = glfwGetTime();
      int frameCount = 0;
      int fps;


      double movement;
      double rotate;
      double speed = 10.0f;
      double rotation = 3.0f;


      camera cam(window);

      cowModelcpu = cam.createModel("../resources/objects/cow.obj", true);
      potModelcpu = cam.createModel("../resources/objects/teapot.obj");

      bool blocked = false;
      bool enterPreviouslyPressed = false;
      // ------------------------------ MAIN WINDOW LOOP ---------------------------------
      // Main loop for the window
      while(!glfwWindowShouldClose(window.window)){
         // Get keyboard inputs
         if (glfwGetKey(window.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {glfwSetWindowShouldClose(window.window, true);}

         bool enterPressed = glfwGetKey(window.window, GLFW_KEY_ENTER) == GLFW_PRESS;
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


         if (glfwGetKey(window.window, GLFW_KEY_S) == GLFW_PRESS) {vao.move(0, 0, -movement);    cam.move(0, 0, -movement);}
         if (glfwGetKey(window.window, GLFW_KEY_W) == GLFW_PRESS) {vao.move(0, 0, movement);     cam.move(0, 0, movement);}
         if (glfwGetKey(window.window, GLFW_KEY_A) == GLFW_PRESS) {vao.rotate(0, -rotate, 0); cam.rotate(0, -rotate, 0);}
         if (glfwGetKey(window.window, GLFW_KEY_D) == GLFW_PRESS) {vao.rotate(0, rotate, 0);  cam.rotate(0, rotate, 0);}


         if(!ogl){

            cam.render(cow1cpu);
            cam.render(cow2cpu);
            cam.render(cow3cpu);
            cam.render(teapotcpu);
            // Draw the camera 2D projection to the window
            vao.draw(cam.m_pixelBuffer);
            cam.draw();
         }
         else {
            vao.bindRender();

            vao.render(cow1);
            vao.render(cow2);
            vao.render(cow3);
            vao.render(teapot);
         }

         text.RenderText(shaderProgramUI,window,"X: " + std::to_string(vao.camPosition[0]), 10, 10);
         text.RenderText(shaderProgramUI,window,"Y: " + std::to_string(vao.camPosition[1]), 10, 20);
         text.RenderText(shaderProgramUI,window,"Z: " + std::to_string(vao.camPosition[2]), 10, 30);
         text.RenderText(shaderProgramUI,window,"FPS: " + std::to_string(fps), 10, 40);

         if(ogl)text.RenderText(shaderProgramUI,window,"GPU", window.fboWidth/2.0f, 10);
         else text.RenderText(shaderProgramUI,window,"CPU", window.fboWidth/2.0f, 10);
         // text.RenderText(shaderProgramUI,vao.fbo.fbo,"Hello", 10, 10, window.width, window.height);

         vao.draw();

         glfwSwapBuffers(window.window);
         glfwPollEvents();

         window.resize();
      }
      // text.cleanup();
      glfwDestroyWindow(window.window);
      glfwSetWindowShouldClose(window.window, true);
      glfwMakeContextCurrent(nullptr);
   }
   // Clear all of the GLFW assets
   glfwTerminate();




   return 0;
}

