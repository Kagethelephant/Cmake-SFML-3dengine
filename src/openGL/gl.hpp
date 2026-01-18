#pragma once
#include <glad/glad.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>



class gl_window {

public:

   gl_window(int _height);

   GLFWwindow* window;
   int width;
   int height;
   int windowWidth;
   int windowHeight;
   float aspectRatio;

};

class gl_fbo {

public:
   gl_fbo() {}
   gl_fbo(GLuint _width, GLuint _height);
   ~gl_fbo() {deleteFbo();}
   
   GLuint texture;
   GLuint depth;

   GLuint height;
   GLuint width;
   GLuint fbo;

   void resize(GLuint width, GLuint height);

private:
   void createFbo(GLuint width, GLuint height);
   void deleteFbo();
};
