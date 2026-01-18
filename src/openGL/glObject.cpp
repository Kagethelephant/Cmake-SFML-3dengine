#include "glObject.hpp"
#include "data.hpp"
#include "matrix.hpp"
#include "gl.hpp"

#include "glShader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// INTITIALIZE THE RENDERER
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
gl_vertexObject::gl_vertexObject(gl_window& _window) : window{_window}{
   // This is the VAO that is used to bind the VBO
   width = window.width;
   height = window.height;

   mat_project = matrix_project(70.0f, (float)width/(float)height, 0.1f, 1000.0f);
   quadVertices = {
      -1.0f,  1.0f, 0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f,
      1.0f, -1.0f, 1.0f, 0.0f,

      -1.0f,  1.0f, 0.0f, 1.0f,
      1.0f, -1.0f, 1.0f, 0.0f,
      1.0f,  1.0f, 1.0f, 1.0f
   };

   shaderProgram3D = createShaderProgram("../src/glShaders/vertex.glsl", "../src/glShaders/fragment.glsl");
   shaderProgramUI = createShaderProgram("../src/glShaders/uiVertex.glsl", "../src/glShaders/uiFragment.glsl");

   createLight(vec3(0,5,0));

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

   // fbo.init(width, height);

   // glGenFramebuffers(1, &fbo);
   // glBindFramebuffer(GL_FRAMEBUFFER, fbo);
   // // generate texture
   // glGenTextures(1, &texture);
   // glBindTexture(GL_TEXTURE_2D, texture);
   // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
   // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   // // attach it to currently bound framebuffer object
   // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);  

   // glGenTextures(1, &depth);
   // glBindTexture(GL_TEXTURE_2D, depth);
   // glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,width, height,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
   // // REQUIRED settings for depth textures
   // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   // // Attach depth texture
   // glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,depth,0);

   // GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
   // glDrawBuffers(1, drawBuffers);
   // 
   // if(!(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)){
   //    std::cout <<"FRAME BUFFER NOT COMPLETE" << std::endl;
   // }
   // // Reset to default frame buffer and texture
   // glBindTexture(GL_TEXTURE_2D, 0);
   // glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
   camDirection = (vec3(0,0,1) * matrix_transform(0, 0, 0, camRotation[0], camRotation[1], camRotation[2])).normal();
   mat_view = matrix_view(matrix_pointAt(camPosition, camDirection, up));
}


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// CREATE NEW LIGHT
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
gl_vertexObject::light& gl_vertexObject::createLight(vec3 position, vec4 color){
   gl_vertexObject::light light;
   light.position = position;
   light.color = color;
   lights.push_back(light);
   return lights[lights.size()-1];
}


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// LOAD NEW MODEL INTO THE RENDERER
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
unsigned int gl_vertexObject::createModel(std::string filename) {

   gl_model newObject;

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
// BIND VBO AND EBO AFTER MODELS ARE LOADED
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void gl_vertexObject::bindObjects(){
   // Bind Vertex Array Object
   glBindVertexArray(vao);
   // Setup the VBO using the VAO
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
   // Set up the attributes for the vertices (how is the data aranged)
   // 1) Shader layout location, 2) Qty of vert attributes, 3) Size of attribute, 4) normaliize btwn -1 to 1, 5)span btwn verts in bytes, 6) start of buffer
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
   // This tells GL to use the vertex attributes defined above (it does not do this by default)
   glEnableVertexAttribArray(0);  

   // Setup the EBO using the VAO
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLint), indices.data(), GL_STATIC_DRAW);
}



//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// SETUP VAO FOR RENDERING
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void gl_vertexObject::bindRender(){

   vec4 bgColor = hexColorToFloat(Color::Black);
   // glBindFramebuffer(GL_FRAMEBUFFER, fbo);
   // glBindFramebuffer(GL_FRAMEBUFFER, fbo.fbo);
   // glViewport(0, 0, width, height);
   window.fbo.bind();
   glUseProgram(shaderProgram3D);
   glBindVertexArray(vao);
   glEnable(GL_DEPTH_TEST);
   glClearColor(bgColor[0],bgColor[1],bgColor[2],bgColor[3]);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   int m_view = glGetUniformLocation(shaderProgram3D, "view");
   glUniformMatrix4fv(m_view,1,GL_FALSE,&mat_view.m[0][0]);

   int m_project = glGetUniformLocation(shaderProgram3D, "project");
   glUniformMatrix4fv(m_project,1,GL_FALSE,&mat_project.m[0][0]);


   int m_light = glGetUniformLocation(shaderProgram3D, "light");
   glUniform3fv(m_light,1,&lights[0].position[0]);

   int m_lightCol = glGetUniformLocation(shaderProgram3D, "lightCol");
   glUniform3fv(m_lightCol,1,&lights[0].color[0]);
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// DRAW MODELS AT LOCATIONS DICTATED BY OBJECTS
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void gl_vertexObject::render(object& obj){

   int m_objCol = glGetUniformLocation(shaderProgram3D, "objCol");
   glUniform3fv(m_objCol,1,&obj.color[0]);

   // update the uniform color
   int m_scale = glGetUniformLocation(shaderProgram3D, "scale");
   glUniformMatrix4fv(m_scale,1,GL_FALSE,&obj.matScale.m[0][0]);
   int m_trans = glGetUniformLocation(shaderProgram3D, "transform");
   glUniformMatrix4fv(m_trans,1,GL_FALSE,&obj.matTransform.m[0][0]);
   glDrawElements(GL_TRIANGLES,models[obj.model].end, GL_UNSIGNED_INT, (void*)(models[obj.model].start * sizeof(uint32_t)));
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
      // glUniform1i(glGetUniformLocation(shaderProgramUI, "screenTexture"), 0);
      glDrawArrays(GL_TRIANGLES, 0, 6);
}


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// OBJECT FUNCTIONS
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void object::scale(float sx, float sy, float sz){
   scales = vec3(sx,sy,sz);
   matScale = matrix_scale(scales[0], scales[1], scales[2]);
}


void object::move(float x, float y, float z){
   position += vec3(x,y,z);
   matTransform = matrix_transform(position[0], position[1], position[2], rotation[0], rotation[1], rotation[2]);
}


void object::rotate(float u, float v, float w){
   rotation += vec3(u,v,w);
   matTransform = matrix_transform(position[0], position[1], position[2], rotation[0], rotation[1], rotation[2]);
}

