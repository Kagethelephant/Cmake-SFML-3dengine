#include "gpuRender.hpp"
#include "utils/data.hpp"
#include "utils/matrix.hpp"
#include "window.hpp"
#include "app/object.hpp"
#include "shaders/shader.hpp"

#include <cstdint>
#include <glad/glad.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <vector>


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// INTITIALIZE THE RENDERER
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
gl_vertexObject::gl_vertexObject(gl_window& _window) : window{_window}{
   // This is the VAO that is used to bind the VBO
   width = window.fboWidth;
   height = window.fboHeight;

   mat_project = matrix_project(70.0f, window.targetAspect, 0.1f, 1000.0f);
   quadVertices = {
      -1.0f,  1.0f,  0.0f,  1.0f, // x, y, u, v
      -1.0f, -1.0f,  0.0f,  0.0f,
       1.0f, -1.0f,  1.0f,  0.0f,

      -1.0f,  1.0f,  0.0f,  1.0f,
       1.0f, -1.0f,  1.0f,  0.0f,
       1.0f,  1.0f,  1.0f,  1.0f
   };

   shaderProgram3D = createShaderProgram("../src/shaders/3d_vertex.glsl", "../src/shaders/3d_fragment.glsl");
   shaderProgramUI = createShaderProgram("../src/shaders/ui_vertex.glsl", "../src/shaders/ui_fragment.glsl");

   move(0,0,0);
   rotate(0,0,0);

   // Create the VBO for the quad we will draw our screen to
   glGenVertexArrays(1, &UIvao);  
   glGenBuffers(1, &UIvbo);
   glBindVertexArray(UIvao);
   // Setup the VBO using the VAO
   glBindBuffer(GL_ARRAY_BUFFER, UIvbo);
   glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(GLfloat), quadVertices.data(), GL_STATIC_DRAW);
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));
   // This tells GL to use the vertex attributes defined above (it does not do this by default)
   glEnableVertexAttribArray(0);  
   glEnableVertexAttribArray(1);  
}


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// MOVE AND ROTATE CAMERA
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void gl_vertexObject::move(float x, float y, float z) {
   
   vec3 up = vec3(0,1,0) * matrix_transform(0, 0, 0, camRotation[0], camRotation[1], camRotation[2]);
   camPosition += (camDirection.cross(up) * x);
   camPosition += (camDirection * z);
   camPosition[1] += y;
   mat_view = matrix_view(matrix_pointAt(camPosition, camDirection, up));
}


void gl_vertexObject::rotate(float u, float v, float w) {
   
   camRotation += vec3(u, v, w);
   vec3 up = vec3(0,1,0) * matrix_transform(0, 0, 0, camRotation[0], camRotation[1], camRotation[2]);
   camDirection = (vec3(0,0,-1) * matrix_transform(0, 0, 0, camRotation[0], camRotation[1], camRotation[2])).normal();
   mat_view = matrix_view(matrix_pointAt(camPosition, camDirection, up));
}




//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// SETUP VAO FOR RENDERING
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

void gl_vertexObject::bindObject(const object& obj){
  
   std::cout << "new Object" << std::endl;
   gpuMesh gpuObject(obj);
   GLuint& vao = gpuObject.vao;
   GLuint& vbo = gpuObject.vbo;


   glGenBuffers(1, &vbo);
   glGenVertexArrays(1, &vao);  
   // Bind Vertex Array Object
   glBindVertexArray(vao);
   // Setup the VBO using the VAO
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, obj.mod.verticesRaw.size() * sizeof(GLfloat), obj.mod.verticesRaw.data(), GL_STATIC_DRAW);

   // 1) Shader layout location, 2) Qty of vert attributes, 3) Size of attribute, 4) normaliize btwn -1 to 1, 5)span btwn verts in bytes, 6) start of buffer
   // positions at location 0
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   // normals at location 1
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);

   // UVs at location 2
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
   glEnableVertexAttribArray(2);


   for (const subMesh& mesh : obj.mod.subMeshes) {
      

      std::cout << "new Mesh" << std::endl;
      gpuSubMesh gpuSub;
      GLuint& ebo = gpuSub.ebo;
      GLuint& tex = gpuSub.tex;

      gpuSub.indiceCount = mesh.indices.size();

      glGenBuffers(1, &gpuSub.ebo);
      glGenTextures(1, &gpuSub.tex);

      glBindTexture(GL_TEXTURE_2D, tex);

      texture texObj = mesh.tex;
      GLenum format = (texObj.channels == 4) ? GL_RGBA : GL_RGB;

      glTexImage2D(GL_TEXTURE_2D, 0, format, texObj.w, texObj.h, 0, format, GL_UNSIGNED_BYTE, texObj.data);
      glGenerateMipmap(GL_TEXTURE_2D);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), mesh.indices.data(), GL_STATIC_DRAW);

      gpuObject.subMeshes.push_back(gpuSub);
   }
   meshes.push_back(gpuObject);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// DRAW MODELS AT LOCATIONS DICTATED BY OBJECTS
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void gl_vertexObject::render(){

   vec4 bgColor = hexColorToFloat(Color::Black);
   window.fbo.bind();
   glUseProgram(shaderProgram3D);
   glClearColor(bgColor[0],bgColor[1],bgColor[2],bgColor[3]);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glEnable(GL_CULL_FACE);
   glCullFace(GL_FRONT);
   glEnable(GL_DEPTH_TEST);

   int lightCount = std::min(MAX_LIGHTS, (unsigned int)lights.size());

   float lightPosBuffer[MAX_LIGHTS * 3];
   float lightColBuffer[MAX_LIGHTS * 3];

   for(int i = 0; i<lightCount; i ++){
      lightPosBuffer[i*3  ] = lights[i].position.x;
      lightPosBuffer[i*3+1] = lights[i].position.y;
      lightPosBuffer[i*3+2] = lights[i].position.z;

      lightColBuffer[i*3  ] = lights[i].color.x;
      lightColBuffer[i*3+1] = lights[i].color.y;
      lightColBuffer[i*3+2] = lights[i].color.z;
   }

   for (const gpuMesh& mesh : meshes){
      
      const object& obj = mesh.obj;
      const GLuint& vao = mesh.vao;
      const GLuint& vbo = mesh.vbo;


      vec4 color = hexColorToFloat(obj.color);

      // Setup the VBO using the VAO
      glBindVertexArray(vao);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);

      // update the uniform color
      glUniformMatrix4fv(glGetUniformLocation(shaderProgram3D, "view"),1,GL_FALSE,&mat_view.m[0][0]);
      glUniformMatrix4fv(glGetUniformLocation(shaderProgram3D, "project"),1,GL_FALSE,&mat_project.m[0][0]);

      glUniform1i(glGetUniformLocation(shaderProgram3D, "lightCount"),lightCount);
      glUniform3fv(glGetUniformLocation(shaderProgram3D, "lightPos"),lightCount,&lightPosBuffer[0]);
      glUniform3fv(glGetUniformLocation(shaderProgram3D, "lightCol"),lightCount,&lightColBuffer[0]);
      // glUniform3fv(glGetUniformLocation(shaderProgram3D, "light"),1,&lightPos[0]);
      // glUniform3fv(glGetUniformLocation(shaderProgram3D, "lightCol"),1,&lightCol[0]);

      glUniform3fv(glGetUniformLocation(shaderProgram3D, "objCol"),1,&color[0]);
      glUniformMatrix4fv(glGetUniformLocation(shaderProgram3D, "scale"),1,GL_FALSE,&obj.matScale.m[0][0]);
      glUniformMatrix4fv(glGetUniformLocation(shaderProgram3D, "transform"),1,GL_FALSE,&obj.matTransform.m[0][0]);
      

      for (const gpuSubMesh& sub : mesh.subMeshes) {

         const GLuint& ebo = sub.ebo;
         const GLuint& tex = sub.tex;

         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

         glActiveTexture(GL_TEXTURE0);
         glBindTexture(GL_TEXTURE_2D, tex);
         glUniform1i(glGetUniformLocation(shaderProgram3D, "diffuseTex"), 0);

         glDrawElements(GL_TRIANGLES,sub.indiceCount, GL_UNSIGNED_INT, 0);
      }

   }
   glDisable(GL_CULL_FACE);
   glCullFace(GL_BACK);
   glDisable(GL_DEPTH_TEST);
}


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// DRAW THE RENDER TEXTURE TO THE GLFW BUFFER
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void gl_vertexObject::draw() {

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glViewport(0, 0, window.windowWidth, window.windowHeight);
   glUseProgram(shaderProgramUI);
   glBindVertexArray(UIvao);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, window.fbo.getTexture());
   glUniform1i(glGetUniformLocation(shaderProgramUI, "screenTexture"), 0);
   glDrawArrays(GL_TRIANGLES, 0, 6);
}


void gl_vertexObject::draw(const std::vector<std::uint8_t> buf) {

   glBindTexture(GL_TEXTURE_2D, window.fbo.getTexture());
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buf.data());
}
