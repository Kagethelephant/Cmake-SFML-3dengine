#pragma once

#include "gl.hpp"
#include <glad/glad.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include <vector>
#include FT_FREETYPE_H
#include "matrix.hpp"
#include <map>
#include <glShader.hpp>



class textEngine {

public:
   textEngine();
   ~textEngine();


   FT_Library library;

   vec3 color;

   GLuint vao;
   GLuint vbo;

   std::vector<FT_Face> fontFaces;

   std::vector<GLfloat> vertices;
   void loadFont(const char * filepath);

   struct character{
      GLuint textureID;
      vec2 size;
      vec2 bearing;
      GLuint advance;
   };
   void RenderText(GLuint shaderProgram, FixedFBO& fbo, std::string text, float x, float y, int width, int height);

   std::map<char, character> characters;
   float quad[6][4];

private:
};
