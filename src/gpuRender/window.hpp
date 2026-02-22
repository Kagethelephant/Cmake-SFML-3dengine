#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cstdint>





class FixedFBO {
public:
   FixedFBO() : fbo(0), colorTex(0), depth(0) {}
   // FixedFBO(int fboWidth, int fboHeight);
   ~FixedFBO();

   
   FixedFBO(const FixedFBO&) = delete;
   FixedFBO& operator=(const FixedFBO&) = delete;


   void init(int fboWidth, int fboHeight, int fboX = 0, int fboY = 0);

   void resize(int fboWidth, int fboHeight, int fboX = 0, int fboY = 0);

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
   GLuint depth = 0;

   int width = 0;
   int height = 0;
   int x = 0;
   int y = 0;
};




class window {

public:

   window(int _height);

   GLFWwindow* win;

   FixedFBO fbo;
   int fboWidth, fboHeight;
   int offsetX, offsetY;

   int windowWidth, windowHeight;
   float windowAspect;
   float targetAspect;

   GLuint UIvao;
   GLuint UIvbo;

   /// @brief: Shader program to render 2D quads with textures
   GLuint shaderProgramUI;

   std::vector<float> quadVertices;
   void draw();
   void draw(const std::vector<std::uint8_t> buf);

   bool resizePending = false;

   void resize();
};
