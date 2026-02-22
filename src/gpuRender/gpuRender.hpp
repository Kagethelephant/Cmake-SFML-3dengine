#pragma once

#include "utils/matrix.hpp"
#include "window.hpp"
#include "app/object.hpp"

#include <cstddef>
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
class gpuRenderObject {

public:

   gpuRenderObject(camera& _cam);

   /// @brief: Shader program to render 3d objects
   GLuint shaderProgram3D;

   /// @brief height: Height of the rendering view (not always the same as window)
   int height;
   /// @brief: Width of the rendering view (not always the same as window)
   int width;


   vec3 lightPos = vec3(0,0,0);
   vec3 lightPosview = vec3(0,0,0);
   vec3 lightCol = vec3(1,1,1);
   
   /// @brief: Projects 3d vertices onto a 2D surface (the screen)
   mat4x4 mat_project;

   // Quad used to render 2d textures on to the screen (used for UI)
   std::vector<GLfloat> quadVertices;


   struct gpuSubMesh{
      GLuint tex;
      GLuint ebo;
      std::size_t indiceCount;
   };

   struct gpuMesh {
      gpuMesh(const object& o) : obj{o} {}
      const object& obj;
      GLuint vao;
      GLuint vbo;
      std::vector<gpuSubMesh> subMeshes;
   };

   std::vector<gpuMesh> meshes;

   std::vector<light> lights;

   void addLight(const light& newLight){
      lights.push_back(newLight);
   }
   window& gl_window;
   camera& cam;


   std::vector<GLfloat> vertices; 
   std::vector<GLuint> indices; 
  

   void render();
   void bindObject(const object& obj);

   // Object to store the location of each of the meshes in the vertex data
   struct model {
      GLuint start;
      GLuint end;
   };

   std::vector<model> models;

   unsigned int createModel (std::string filename);

private:

};

