#include "gl.hpp"
#include <glad/glad.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <iostream>


// Resize the viewport on window resize during the pollEvents
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
   glViewport(0, 0, width, height);
} 

gl_window::gl_window(int _height){
   // Start up GLFW
   glfwInit();

   // Set the GLFW version and use CORE profile (only modern GLFW commands)
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   // Get the aspect ratio from the primary monitor
   const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
   windowWidth = mode->width;
   windowHeight = mode->height;

   aspectRatio = (float)windowWidth/(float)windowHeight;
   height = _height;
   width = height*aspectRatio;

   // Create GLFW window
   // set 4th param = glfwGetPrimaryMonitor() to make fullscreen
   window = glfwCreateWindow(windowWidth, windowHeight, "The Game", NULL, NULL);
   // Sets this window as the context so all window functions modify this window
   glfwMakeContextCurrent(window);

   // Load glad. Glad loads openGL functions and pointers at runtime.
   gladLoadGL();

   // Create viewport. Specifies the area in the window to draw things
   glViewport(0, 0, windowWidth, windowHeight);
   // Callback function to resize the viewport when the window resizes during glfwPollEvents
   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 
}




gl_fbo::gl_fbo(GLuint _width, GLuint _height) {
   createFbo(_width, _height);
}

void gl_fbo::resize(GLuint _width, GLuint _height){
   deleteFbo();
   createFbo(_width, _height);
}

void gl_fbo::createFbo(GLuint _width, GLuint _height){

   width = _width;
   height = _height;

   glGenFramebuffers(1, &fbo);
   glBindFramebuffer(GL_FRAMEBUFFER, fbo);
   // generate texture
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   // attach it to currently bound framebuffer object
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);  

   glGenTextures(1, &depth);
   glBindTexture(GL_TEXTURE_2D, depth);
   glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,_width, _height,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
   // REQUIRED settings for depth textures
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   // Attach depth texture
   glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,depth,0);

   GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
   glDrawBuffers(1, drawBuffers);
   
   if(!(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)){
      std::cout <<"FRAME BUFFER NOT COMPLETE" << std::endl;
   }
   // Reset to default frame buffer and texture
   glBindTexture(GL_TEXTURE_2D, 0);
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void gl_fbo::deleteFbo(){

   glDeleteFramebuffers(1, &fbo);
   glDeleteRenderbuffers(1, &depth);
   glDeleteTextures(1, &texture);
}
