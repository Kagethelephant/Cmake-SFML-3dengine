#pragma once

#include "data.hpp"
#include "matrix.hpp"
#include "gl.hpp"

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
   vec3 camDirection = vec3(0,0,1);
   
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
   // FixedFBO fbo;
   // GLuint fbo;
   // GLuint texture;
   // GLuint depth;

   GLuint UIvao;
   GLuint UIvbo;

   std::vector<GLfloat> vertices; 
   std::vector<GLuint> indices; 
  

   void bindObjects();
   void bindRender();
   void render(object& obj);
   void draw();


   // Object to store the location of each of the meshes in the vertex data
   struct gl_model {
      GLuint start;
      GLuint end;
   };

   struct light {
      vec3 position = vec3(0,0,0);
      vec4 color = vec4(1.0f,1.0f,1.0f,1.0f);
   };

   std::vector<gl_model> models;
   std::vector<light> lights;

   unsigned int createModel (std::string filename);
   light& createLight(vec3 position, vec4 color = vec4(1.0f,1.0f,1.0f,1.0f));


private:

};


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// OBJECT
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

/// @brief: 3D object that contains the poition orientation and scale of the model to be drawn
/// @param _model: The index of the model to be used from the VAO
struct object {

   object(unsigned int _model) : model(_model) {}

   /// @brief: Index of the model in whe render VAO
   unsigned int model;

   /// @brief: Matrix used to scale to the object
   mat4x4 matScale = matrix_scale(1, 1, 1);
   /// @brief: Matrix used to transform the object (move and rotate)
   mat4x4 matTransform = matrix_transform(0, 0, 0, 0, 0, 0);

   /// @brief: The scale of the object in all axis
   vec3 scales = vec3(1,1,1);
   /// @brief: The position of the origin of the object
   vec3 position = vec3(0,0,0);
   /// @brief: The rotation of the object around its origin
   vec3 rotation = vec3(0,0,0);

   vec4 color = hexColorToFloat(Color::Red);

   /// @brief: Scale the object in given axis (absolute)
   /// @param sx: Scale in x axis
   /// @param sy: Scale in y axis
   /// @param sz: Scale in z axis
   void scale(float sx, float sy, float sz);

   /// @brief: Move the object relitively
   /// @param x: Move in x axis
   /// @param y: Move in y axis
   /// @param z: Move in z axis
   void move(float x, float y, float z);

   /// @brief: Rotate the object around the origin
   /// @param u: Rotate around the x axis (in radians)
   /// @param v: Rotate around the y axis (in radians)
   /// @param w: Rotate around the z axis (in radians)
   void rotate(float u, float v, float w);
};
