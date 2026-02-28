#pragma once
// OpenGl Library
#include <glad/glad.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
// Standard Libraries
#include <vector>
#include <map>
// Program Headers
#include "utils/data.hpp"
#include "window.hpp"
#include <shaders/shader.hpp>
#include "utils/matrix.hpp"
// Freetype Library
#include <ft2build.h>
#include FT_FREETYPE_H



class textRenderObject {

public:
   textRenderObject(window& _win);
   ~textRenderObject();


   FT_Library library;

   GLuint vao;
   GLuint vbo;

   window& gl_window;

   GLuint shaderProgramText;

   std::vector<FT_Face> fontFaces;

   std::vector<GLfloat> vertices;
   void loadFont(const char * filepath);

   struct character{
      GLuint textureID;
      vec2 size;
      vec2 bearing;
      GLuint advance;
   };
   void RenderText(std::string text, float x, float y, Color col = Color::White);

   std::map<char, character> characters;
   float quad[6][4];

private:
};
