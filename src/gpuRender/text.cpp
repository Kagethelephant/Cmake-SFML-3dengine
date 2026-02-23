#include "text.hpp"
#include "RAIIWrapper.hpp"
#include "utils/data.hpp"
#include "window.hpp"
#include <ft2build.h>
#include <iostream>
#include <utility>
#include FT_FREETYPE_H
#include "utils/matrix.hpp"

textRenderObject::textRenderObject(window& _win) : gl_window{_win} {

   if (FT_Init_FreeType(&library))
   {
      std::cout << "Could not init FreeType" << std::endl;
   }

   shaderProgramText = createShaderProgram("../src/shaders/text_vertex.glsl", "../src/shaders/text_fragment.glsl");

   glGenVertexArrays(1, &vao);
   glGenBuffers(1, &vbo);

   GLScopedVAO tempVAO(vao);
   GLScopedVBO tempVBO(vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

textRenderObject::~textRenderObject(){
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

void textRenderObject::loadFont(const char *filePath){

   FT_Face face;
   if (FT_New_Face(library, filePath, 0, &face))
   {
      std::cout << "Failed to load font" << std::endl;
   }
   // Set font pixel size
   FT_Set_Pixel_Sizes(face, 0, 8);

   fontFaces.push_back(face);

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

   for (unsigned char c = 0; c < 128; c++)
   {
      if (FT_Load_Char(face, c, FT_LOAD_RENDER))
         continue;

      GLuint texture;
      glGenTextures(1, &texture);
      GLScopedTexture2D tempTexture(texture);

      glTexImage2D(GL_TEXTURE_2D,0,GL_RED,face->glyph->bitmap.width,face->glyph->bitmap.rows,0,GL_RED,GL_UNSIGNED_BYTE,face->glyph->bitmap.buffer);

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


void textRenderObject::RenderText(std::string text, float x, float y, Color col){

   vec3 color = hexColorToFloat(col).xyz();

   GLScopedFBO temFBO(gl_window.fbo);
   GLScopedViewport tempViewPort(0, 0, gl_window.fboWidth, gl_window.fboHeight);
   GLScopedProgram tempProgram(shaderProgramText);
   GLScopedVAO tempVAO(vao);

   GLScopedCapability tempBlendEnable(GL_BLEND, true);
   GLScopedBlendFunc tempBlendMode(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glUniform1i(glGetUniformLocation(shaderProgramText, "text"), 0);

   int shaderColor = glGetUniformLocation(shaderProgramText, "textColor");
   glUniform3fv(shaderColor,1,&color[0]);

   GLScopedActiveTexture tempActiveTex(GL_TEXTURE0);

   for (char c : text)
   {
      character ch = characters[c];

      float xpos = x + ch.bearing[0];
      float ypos = gl_window.fboHeight - y - ch.bearing[1];

      float w = ch.size[0];
      float h = ch.size[1];

      float xmin = (2*(xpos  )/gl_window.fboWidth ) -1;
      float xmax = (2*(xpos+w)/gl_window.fboWidth ) -1;
      float ymin = (2*(ypos  )/gl_window.fboHeight) -1;
      float ymax = (2*(ypos+h)/gl_window.fboHeight) -1;

      vertices = {
         xmin, ymax,   0.0f, 0.0f,
         xmin, ymin,   0.0f, 1.0f,
         xmax, ymin,   1.0f, 1.0f,

         xmin, ymax,   0.0f, 0.0f,
         xmax, ymin,   1.0f, 1.0f,
         xmax, ymax,   1.0f, 0.0f
      };

        GLScopedTexture2D tempTexture(ch.textureID);
        GLScopedVBO tempVBO(vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size()* sizeof(GLfloat), vertices.data());

        glDrawArrays(GL_TRIANGLES, 0, 6);

      x += (ch.advance >> 6); // Convert from 1/64 pixels
   }
}
