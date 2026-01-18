#include "freeType.hpp"
#include <ft2build.h>
#include <iostream>
#include <utility>
#include FT_FREETYPE_H
#include "matrix.hpp"

textEngine::textEngine(){

   if (FT_Init_FreeType(&library))
   {
      std::cout << "Could not init FreeType" << std::endl;
   }

   color = vec3(.1,.3,.5);

   glGenVertexArrays(1, &vao);
   glGenBuffers(1, &vbo);

   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);

}

textEngine::~textEngine(){
   for (FT_Face face : fontFaces){
      FT_Done_Face(face);
   }

   for (auto& [c, ch] : characters){
       glDeleteTextures(1, &ch.textureID);
   }

   characters.clear();
   fontFaces.clear();

   glDeleteBuffers(1, &vbo);
   glDeleteVertexArrays(1, &vao);

   FT_Done_FreeType(library);

}

void textEngine::loadFont(const char *filePath){

   FT_Face face;
   if (FT_New_Face(library, filePath, 0, &face))
   {
      std::cout << "Failed to load font" << std::endl;
   }
   // Set font pixel size
   FT_Set_Pixel_Sizes(face, 0, 16);

   fontFaces.push_back(face);

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

   for (unsigned char c = 0; c < 128; c++)
   {
      if (FT_Load_Char(face, c, FT_LOAD_RENDER))
         continue;

      GLuint texture;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);

      glTexImage2D(
         GL_TEXTURE_2D,
         0,
         GL_RED,
         face->glyph->bitmap.width,
         face->glyph->bitmap.rows,
         0,
         GL_RED,
         GL_UNSIGNED_BYTE,
         face->glyph->bitmap.buffer
      );

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      character character = {
         texture,
         vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
         vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
         static_cast<GLuint>(face->glyph->advance.x)
      };

      characters.insert(std::make_pair(c, character));
   }
}


void textEngine::RenderText(GLuint shaderProgram, GLuint fbo, std::string text, float x, float y, int width, int height){

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glUseProgram(shaderProgram);
   glUniform1i(glGetUniformLocation(shaderProgram, "text"), 0);

   int shaderColor = glGetUniformLocation(shaderProgram, "textColor");
   glUniform3fv(shaderColor,1,&color[0]);

   glBindFramebuffer(GL_FRAMEBUFFER, fbo);
   glActiveTexture(GL_TEXTURE0);
   glBindVertexArray(vao);

   for (char c : text)
   {
      character ch = characters[c];

      // float baseY = y - characters['H'].bearing[1];

      float xpos = x + ch.bearing[0];
      float ypos = y - (ch.size[1] - ch.bearing[1]);

      float w = ch.size[0];
      float h = ch.size[1];

      float xmin = (2*(xpos)/width) -1;
      float xmax = (2*(xpos+w)/width) -1;
      float ymin = (2*(ypos)/height) -1;
      float ymax = (2*(ypos+h)/height) -1;


      // std::cout << "Xmax: " << xmax << std::endl;
      // std::cout << "Ymax: " << ymax << std::endl;
      // std::cout << "Xmin: " << xmin << std::endl;
      // std::cout << "Ymin: " << ymin << std::endl;
      
      vertices = {
         xmin, ymax,   0.0f, 0.0f,
         xmin, ymin,   0.0f, 1.0f,
         xmax, ymin,   1.0f, 1.0f,

         xmin, ymax,   0.0f, 0.0f,
         xmax, ymin,   1.0f, 1.0f,
         xmax, ymax,   1.0f, 0.0f
      };

        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size()* sizeof(GLfloat), vertices.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

      x += (ch.advance >> 6); // Convert from 1/64 pixels
   }

   glDisable(GL_BLEND);
   glBindVertexArray(0);
   glBindTexture(GL_TEXTURE_2D, 0);
}
