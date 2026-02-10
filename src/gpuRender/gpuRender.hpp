#pragma once

#include "utils/matrix.hpp"
#include "window.hpp"
#include "app/object.hpp"

#include <glad/glad.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <string>
#include <sys/types.h>
#include <vector>





// Forward declaration of object for use in vertex object
struct object;

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// RENDERER
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX


/// @brief: Loads and renders the meshes to the screen
/// @param _width: Width of the rendering view (not always the same as window)
/// @param _height: Height of the rendering view (not always the same as window)
class gl_vertexObject {

public:

   gl_vertexObject(gl_window& _window);

   /// @brief: Shader program to render 3d objects
   GLuint shaderProgram3D;
   /// @brief: Shader program to render 2D quads with textures
   GLuint shaderProgramUI;

   /// @brief height: Height of the rendering view (not always the same as window)
   int height;
   /// @brief: Width of the rendering view (not always the same as window)
   int width;

   /// @brief: Position of the camera in world coordinates
   vec3 camPosition = vec3(0,0,0);
   /// @brief: Rotation from facing in the negative z direction (absolute)
   vec3 camRotation = vec3(0,0,0);
   /// @brief: Vector of the camera direction created from the rotation
   vec3 camDirection = vec3(0,0,-1);

   vec3 lightPos = vec3(0,0,0);
   vec3 lightPosview = vec3(0,0,0);
   vec3 lightCol = vec3(1,1,1);
   
   /// @brief: Matrix that moves vertices into the correct position in camera space
   mat4x4 mat_view;
   /// @brief: Projects 3d vertices onto a 2D surface (the screen)
   mat4x4 mat_project;

   // Quad used to render 2d textures on to the screen (used for UI)
   std::vector<GLfloat> quadVertices;

   void move(float x, float y, float z);
   void rotate(float u, float v, float w);

   GLuint vao;
   GLuint vbo;
   GLuint ebo;

   gl_window& window;

   GLuint UIvao;
   GLuint UIvbo;

   std::vector<GLfloat> vertices; 
   std::vector<GLuint> indices; 
  

   void bindRender();
   void render(object& obj);
   void draw();
   void draw(const std::vector<std::uint8_t> buf);

   // Object to store the location of each of the meshes in the vertex data
   struct model {
      GLuint start;
      GLuint end;
   };

   std::vector<model> models;

   unsigned int createModel (std::string filename);

private:

};

