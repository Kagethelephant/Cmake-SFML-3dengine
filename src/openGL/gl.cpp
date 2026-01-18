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

   fbo.init(width, height);
}




// FixedFBO::FixedFBO(int fboWidth, int fboHeight) : width(fboWidth), height(fboHeight){
//    create();
// }

void FixedFBO::init(int _width, int _height){
   destroy();
   width = _width;
   height = _height;
   create();
   
}

FixedFBO::~FixedFBO(){
   destroy();
}

void FixedFBO::resize(int fboWidth, int fboHeight){
   if (fboWidth == width && fboHeight == height)
      return;

   destroy();
   width  = fboWidth;
   height = fboHeight;
   create();
}

void FixedFBO::bind(){
   glBindFramebuffer(GL_FRAMEBUFFER, fbo);
   glViewport(0, 0, width, height);
}

void FixedFBO::unbind(){
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FixedFBO::create(){
   glGenFramebuffers(1, &fbo);
   glBindFramebuffer(GL_FRAMEBUFFER, fbo);

   // Color attachment
   glGenTextures(1, &colorTex);
   glBindTexture(GL_TEXTURE_2D, colorTex);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);



   glGenTextures(1, &depthRbo);
   glBindTexture(GL_TEXTURE_2D, depthRbo);
   glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,width, height,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
   // REQUIRED settings for depth textures
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   // Attach depth texture
   glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,depthRbo,0);


   // // Depth-stencil renderbuffer
   // glGenRenderbuffers(1, &depthRbo);
   // glBindRenderbuffer(GL_RENDERBUFFER, depthRbo);
   // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

   // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthRbo);


   GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
   glDrawBuffers(1, buffers);

   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cerr << "FixedFBO incomplete\n";

   GLint currentFbo;
   glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFbo);
   std::cout << "FBO ID: " << fbo << "\n";
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FixedFBO::destroy(){
   if (depthRbo) glDeleteTextures(1, &depthRbo);
   if (colorTex) glDeleteTextures(1, &colorTex);
   if (fbo)      glDeleteFramebuffers(1, &fbo);

   depthRbo = 0;
   colorTex = 0;
   fbo = 0;
}
