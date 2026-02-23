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

   // INITIALIZE GLFW
   // -----------------------------------------------------------------------------------
   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   // Global random number generator (global so everything shares the same seed)
   randObj rander(false, 13412234);
   bool ogl = true;

   // ----------------------------- CREATE WINDOW AND OpenGL CONEXT -------------------------------
   //Create scope here so objects can call destructors on open gl objects before opengl is terminated
   {
      // Create the window
      window gl_window(400);
      // Create camera that represents view into world
      camera cam(gl_window);

      // Define model data and create instances of those models (objects)
      model yoshi("../resources/objects/yoshi/yoshi.obj",true);
      model Arcanine("../resources/objects/Arcanine/Arcanine.obj",true);

      object yoshi1(yoshi);
      object arcanine1(Arcanine);
      arcanine1.move(-10,0,-10);
      arcanine1.scale(10,10,10);
      yoshi1.move(0,0,-10);
      yoshi1.rotate(0,5,0);

      light light1 = createLight(vec3(15,5,5),vec3(0.6,0.3,0.3));
      light light2 = createLight(vec3(-15,5,5),vec3(0.3,0.3,0.6));

      // Create the rendering objects for the CPU and GPU
      gpuRenderObject gpuRend(cam);
      gpuRend.bindObject(yoshi1);
      gpuRend.bindObject(arcanine1);

      gpuRend.addLight(light1);
      gpuRend.addLight(light2);

      cpuRenderObject cpuRend(cam);
      cpuRend.bindObject(yoshi1);
      cpuRend.bindObject(arcanine1);

      cpuRend.addLight(light1);
      cpuRend.addLight(light2);

      // Create text object
      textRenderObject text(gl_window);
      text.loadFont("../resources/font/small_pixel.ttf");

      double movement;
      double rotate;
      double speed = 10.0f;
      double rotation = 3.0f;

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

         if (glfwGetKey(gl_window.win, GLFW_KEY_S) == GLFW_PRESS) {cam.move(0, 0, -movement);}
         if (glfwGetKey(gl_window.win, GLFW_KEY_W) == GLFW_PRESS) {cam.move(0, 0, movement);}
         if (glfwGetKey(gl_window.win, GLFW_KEY_A) == GLFW_PRESS) {cam.rotate(0, -rotate, 0);}
         if (glfwGetKey(gl_window.win, GLFW_KEY_D) == GLFW_PRESS) {cam.rotate(0, rotate, 0);}

         int fps = gl_window.fps;

         movement = speed * gl_window.frameTime;
         rotate = rotation * gl_window.frameTime;

         // Render 3D objects
         if(ogl){ 
            gpuRend.render();
            text.RenderText("GPU", gl_window.fboWidth/2.0f, 10,Color::Green);
         }
         else { 
            cpuRend.render();
            text.RenderText("CPU", gl_window.fboWidth/2.0f, 10, Color::Blue);
         }
         text.RenderText("FPS: " + std::to_string(fps), 10, 10);

         gl_window.frameUpdate();
      }
   }

   // Shutdown GLFW
   glfwTerminate();

   return 0;
}

