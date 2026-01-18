#pragma once
#include <glad/glad.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>





class FixedFBO {
public:
   FixedFBO() : fbo(0), colorTex(0), depthRbo(0) {}
   // FixedFBO(int fboWidth, int fboHeight);
   ~FixedFBO();

   
   FixedFBO(const FixedFBO&) = delete;
   FixedFBO& operator=(const FixedFBO&) = delete;


   void init(int width, int height);

   void resize(int fboWidth, int fboHeight);

   void bind();
   static void unbind();

   GLuint getTexture() const { return colorTex; }
   int getWidth()  const { return width; }
   int getHeight() const { return height; }

private:
   void create();
   void destroy();

   GLuint fbo = 0;
   GLuint colorTex = 0;
   GLuint depthRbo = 0;

   int width = 0;
   int height = 0;
};




class gl_window {

public:

   gl_window(int _height);

   GLFWwindow* window;
   
   FixedFBO fbo;
   int width;
   int height;
   int windowWidth;
   int windowHeight;
   float aspectRatio;

};
