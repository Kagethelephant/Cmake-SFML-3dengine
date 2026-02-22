#include "window.hpp"
#include "shaders/shader.hpp"

#include <cstdint>
#include <glad/glad.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <cstdint>


window::window(int _height){

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

   win = glfwCreateWindow(windowWidth, windowHeight, "The Game", NULL, NULL);
   glfwMakeContextCurrent(win);
   // Maximum width and height are set to GLFW_DONT_CARE to allow unlimited expansion
   glfwSetWindowSizeLimits(win, GLFW_DONT_CARE, fboHeight, GLFW_DONT_CARE, GLFW_DONT_CARE);
   glfwSwapInterval(0);

   gladLoadGL();

   glViewport(0, 0, windowWidth, windowHeight);

   // Setup callbacks
   glfwSetWindowUserPointer(win, this);
   glfwSetFramebufferSizeCallback(win, [](GLFWwindow* _win, int w, int h){

      auto* self = static_cast<window*>(glfwGetWindowUserPointer(_win));
      if (!self) return;

      self->windowWidth  = w;
      self->windowHeight = h;
      self->windowAspect  = float(w) / float(h);

      self->resizePending = true;
   });



   quadVertices = {
      -1.0f,  1.0f,  0.0f,  1.0f, // x, y, u, v
      -1.0f, -1.0f,  0.0f,  0.0f,
       1.0f, -1.0f,  1.0f,  0.0f,

      -1.0f,  1.0f,  0.0f,  1.0f,
       1.0f, -1.0f,  1.0f,  0.0f,
       1.0f,  1.0f,  1.0f,  1.0f
   };

   shaderProgramUI = createShaderProgram("../src/shaders/ui_vertex.glsl", "../src/shaders/ui_fragment.glsl");

   fbo.init(fboWidth, fboHeight);


   // Create the VBO for the quad we will draw our screen to
   glGenVertexArrays(1, &UIvao);  
   glGenBuffers(1, &UIvbo);
   glBindVertexArray(UIvao);
   // Setup the VBO using the VAO
   glBindBuffer(GL_ARRAY_BUFFER, UIvbo);
   glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(GLfloat), quadVertices.data(), GL_STATIC_DRAW);
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));
   // This tells GL to use the vertex attributes defined above (it does not do this by default)
   glEnableVertexAttribArray(0);  
   glEnableVertexAttribArray(1);  
}


void window::resize(){
   if (resizePending){

      fbo.resize(fboWidth, fboHeight, offsetX, offsetY);
      resizePending = false;
   }
}

void window::draw(){

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glViewport(0, 0, windowWidth, windowHeight);
   glUseProgram(shaderProgramUI);
   glBindVertexArray(UIvao);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, fbo.getTexture());
   glUniform1i(glGetUniformLocation(shaderProgramUI, "screenTexture"), 0);
   glDrawArrays(GL_TRIANGLES, 0, 6);
}


void window::draw(const std::vector<std::uint8_t> buf) {

   glBindTexture(GL_TEXTURE_2D, fbo.getTexture());
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, fboWidth, fboHeight, GL_RGBA, GL_UNSIGNED_BYTE, buf.data());
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
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

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
   if (depth)    glDeleteTextures(1, &depth);
   if (colorTex) glDeleteTextures(1, &colorTex);
   if (fbo)      glDeleteFramebuffers(1, &fbo);

   depth = 0;
   colorTex = 0;
   fbo = 0;
}
