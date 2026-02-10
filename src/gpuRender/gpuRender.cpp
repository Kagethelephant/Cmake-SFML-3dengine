#include "gpuRender.hpp"
#include "utils/data.hpp"
#include "utils/matrix.hpp"
#include "window.hpp"
#include "app/object.hpp"
#include "shaders/shader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// INTITIALIZE THE RENDERER
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
gl_vertexObject::gl_vertexObject(gl_window& _window) : window{_window}{
   // This is the VAO that is used to bind the VBO
   width = window.fboWidth;
   height = window.fboHeight;

   mat_project = matrix_project(70.0f, window.targetAspect, 0.1f, 1000.0f);
   quadVertices = {
      -1.0f,  1.0f, 0.0f, 1.0f, // x, y, u, v
      -1.0f, -1.0f, 0.0f, 0.0f,
      1.0f, -1.0f, 1.0f, 0.0f,

      -1.0f,  1.0f, 0.0f, 1.0f,
      1.0f, -1.0f, 1.0f, 0.0f,
      1.0f,  1.0f, 1.0f, 1.0f
   };

   shaderProgram3D = createShaderProgram("../src/shaders/3d_vertex.glsl", "../src/shaders/3d_fragment.glsl");
   shaderProgramUI = createShaderProgram("../src/shaders/ui_vertex.glsl", "../src/shaders/ui_fragment.glsl");

   move(0,0,0);
   rotate(0,0,0);
   glGenVertexArrays(1, &vao);  
   glGenBuffers(1, &vbo);
   glGenBuffers(1, &ebo);


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

   std::cout << "VAO Initialized" << std::endl;
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
// LOAD NEW MODEL INTO THE RENDERER
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
unsigned int gl_vertexObject::createModel(std::string filename) {

   model newObject;

   int vertStart = vertices.size()/3;
   newObject.start = indices.size();
   // Try to open the file
   std::ifstream obj(filename);
   // Create an array to hold the chars of each line
   // cycle through all the lines in the file until we are at the end
   while(!obj.eof()) {
      // Create a char array to store the line from the file
      char line[128];
      obj.getline(line,128);
      // Pass the line from the file "stream" into the line
      std::stringstream stream;
      stream << line;
      // Check if the line is a vertice or a triangle
      char junk;
      if(line[0] == 'v') {
         // If it is a vertice then pull the xyz values from the string and put it in the vert array
         GLfloat x,y,z;
         stream >> junk >> x >> y >> z;
         vertices.push_back(x);
         vertices.push_back(y);
         vertices.push_back(z);
      }
      if(line[0] == 'f') {
         // If it is a triangle then get the corosponding vertices and load it into the mesh
         GLint v0,v1,v2;
         stream >> junk >> v0 >> v1 >> v2;
         indices.push_back(v0+vertStart-1);
         indices.push_back(v1+vertStart-1);
         indices.push_back(v2+vertStart-1);
      }
   }
   newObject.end = indices.size()-newObject.start-1;
   models.push_back(newObject);
   return models.size()-1;
}





//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// SETUP VAO FOR RENDERING
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void gl_vertexObject::bindRender(){

   // lightPosview = lightPos * mat_view;
   vec4 bgColor = hexColorToFloat(Color::Black);
   window.fbo.bind();
   glUseProgram(shaderProgram3D);
   glBindVertexArray(vao);
   glEnable(GL_DEPTH_TEST);
   glClearColor(bgColor[0],bgColor[1],bgColor[2],bgColor[3]);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// DRAW MODELS AT LOCATIONS DICTATED BY OBJECTS
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void gl_vertexObject::render(object& obj){

   int w, h, channels;
   stbi_set_flip_vertically_on_load(true);

   unsigned char* data = stbi_load(
      "../resources/objects/yoshi/yoshi_grp.png",
      &w, &h,
      &channels,
      0
   );

   if (!data) {
      throw std::runtime_error("Failed to load texture");
   }


   GLuint tex;
   glGenTextures(1, &tex);
   glBindTexture(GL_TEXTURE_2D, tex);

   GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

   glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);

   glGenerateMipmap(GL_TEXTURE_2D);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   stbi_image_free(data);

   // Bind Vertex Array Object
   glBindVertexArray(vao);
   // Setup the VBO using the VAO
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, obj.verts.verticesRaw.size() * sizeof(GLfloat), obj.verts.verticesRaw.data(), GL_STATIC_DRAW);

   // positions at location 0
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);

   // normals at location 1
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);

   // UVs at location 2
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
   glEnableVertexAttribArray(2);


   // // Set up the attributes for the vertices (how is the data aranged)
   // // 1) Shader layout location, 2) Qty of vert attributes, 3) Size of attribute, 4) normaliize btwn -1 to 1, 5)span btwn verts in bytes, 6) start of buffer
   // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
   // // This tells GL to use the vertex attributes defined above (it does not do this by default)
   // glEnableVertexAttribArray(0);  
   // Setup the EBO using the VAO
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.verts.indices.size() * sizeof(float), obj.verts.indices.data(), GL_STATIC_DRAW);




   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, tex);
   glUniform1i(glGetUniformLocation(shaderProgram3D, "diffuseTex"), 0);




   vec4 color = hexColorToFloat(obj.color);
   // update the uniform color
   glUniformMatrix4fv(glGetUniformLocation(shaderProgram3D, "view"),1,GL_FALSE,&mat_view.m[0][0]);
   glUniformMatrix4fv(glGetUniformLocation(shaderProgram3D, "project"),1,GL_FALSE,&mat_project.m[0][0]);
   glUniform3fv(glGetUniformLocation(shaderProgram3D, "light"),1,&lightPos[0]);
   glUniform3fv(glGetUniformLocation(shaderProgram3D, "lightCol"),1,&lightCol[0]);

   glUniform3fv(glGetUniformLocation(shaderProgram3D, "objCol"),1,&color[0]);
   glUniformMatrix4fv(glGetUniformLocation(shaderProgram3D, "scale"),1,GL_FALSE,&obj.matScale.m[0][0]);
   glUniformMatrix4fv(glGetUniformLocation(shaderProgram3D, "transform"),1,GL_FALSE,&obj.matTransform.m[0][0]);
   // glDisable(GL_CULL_FACE);
   // glEnable(GL_DEPTH_TEST);
   glDrawElements(GL_TRIANGLES,obj.verts.indices.size(), GL_UNSIGNED_INT, 0);

   // delete the texture you created to avoid leaking VRAM
   glDeleteTextures(1, &tex);
}


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// DRAW THE RENDER TEXTURE TO THE GLFW BUFFER
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void gl_vertexObject::draw() {

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glViewport(0, 0, window.windowWidth, window.windowHeight);
   glUseProgram(shaderProgramUI);
   glBindVertexArray(UIvao);
   glDisable(GL_DEPTH_TEST);

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
