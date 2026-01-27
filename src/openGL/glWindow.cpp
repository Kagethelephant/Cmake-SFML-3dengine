#include "glWindow.hpp"

#include <glad/glad.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <iostream>


gl_window::gl_window(int _height){

   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   // Get primary monitor size
   const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
   windowWidth = mode->width;
   windowHeight = mode->height;

   fboHeight = _height;
   targetAspect = float(windowWidth) / float(windowHeight);  // initial estimate
   fboWidth = int(fboHeight * targetAspect);
   offsetX = 0;
   offsetY = 0;

   window = glfwCreateWindow(windowWidth, windowHeight, "The Game", NULL, NULL);
   glfwMakeContextCurrent(window);
   // Maximum width and height are set to GLFW_DONT_CARE to allow unlimited expansion
   glfwSetWindowSizeLimits(window, GLFW_DONT_CARE, fboHeight, GLFW_DONT_CARE, GLFW_DONT_CARE);
   glfwSwapInterval(0);

   gladLoadGL();

   glViewport(0, 0, windowWidth, windowHeight);

   // Setup callbacks
   glfwSetWindowUserPointer(window, this);
   glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int w, int h){

      auto* self = static_cast<gl_window*>(glfwGetWindowUserPointer(window));
      if (!self) return;

      self->windowWidth  = w;
      self->windowHeight = h;
      self->windowAspect  = float(w) / float(h);

      self->resizePending = true;
   });

   fbo.init(fboWidth, fboHeight);
}


void gl_window::resize(){
   if (resizePending){

      fbo.resize(fboWidth, fboHeight, offsetX, offsetY);
      resizePending = false;
   }
}




void FixedFBO::init(int fboWidth, int fboHeight, int fboX, int fboY){
   destroy();
   width  = fboWidth;
   height = fboHeight;
   x = fboX;
   y = fboY;
   create();

}

FixedFBO::~FixedFBO(){
   destroy();
}

void FixedFBO::resize(int fboWidth, int fboHeight, int fboX, int fboY){
   if (fboWidth == width && fboHeight == height && fboX == x && fboY == y)
      return;

   destroy();
   width  = fboWidth;
   height = fboHeight;
   x = fboX;
   y = fboY;
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


   glGenTextures(1, &depth);
   glBindTexture(GL_TEXTURE_2D, depth);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
   // REQUIRED settings for depth textures
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   // Attach depth texture
   glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,depth,0);

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
   if (depth) glDeleteTextures(1, &depth);
   if (colorTex) glDeleteTextures(1, &colorTex);
   if (fbo)      glDeleteFramebuffers(1, &fbo);

   depth = 0;
   colorTex = 0;
   fbo = 0;
}
