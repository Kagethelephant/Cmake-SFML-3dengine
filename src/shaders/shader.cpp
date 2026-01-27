#include "shaders/shader.hpp"
#include <glad/glad.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <string>

#include <iostream>
#include <fstream>
#include <string>




// Function to parse shader files
std::string readShaderFile(const std::string& filePath) {
   // Input file stream
   std::ifstream shaderFile(filePath);
   if (!shaderFile.is_open()) {
      std::cerr << "Could not open shader file: " << filePath << std::endl;
      return "";
   }
   // Copy all lines into "shaderCode" as one long string
   std::string line;
   std::string shaderCode;
   while (std::getline(shaderFile, line)) {
      shaderCode += line + '\n';
   }
   shaderFile.close();
   return shaderCode;
}

GLuint gl_loadShader(std::string filename, GLuint type){
   // ------------------------------ CREATE FRAGMENT SHADER -------------------------------
   // Work around string vertex shader that just adds a 4th value to the vertex
   std::string shaderSource = readShaderFile(filename).c_str();
   const char *shaderSourceStirng = shaderSource.c_str();
   // Create a shder object for the vertex shader
   GLuint shader = glCreateShader(type);
   // This attatches the shader string to the shader, 2nd parameter is how many strings
   glShaderSource(shader, 1, &shaderSourceStirng, NULL);
   glCompileShader(shader);

   int  success;
   char infoLog[512];
   // Check if the compilation of the shader was successfull
   glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
   // If the compilation was not successful then retrieve the error message
   if(!success){
      glGetShaderInfoLog(shader, 512, NULL, infoLog);
      std::cout << filename << "--- Failed Compilation ---" << infoLog <<  std::endl;
   }
   return shader;
}


GLuint createShaderProgram(std::string vertexPath, std::string fragmentPath){
   // Create the shader program to link all of the shders together for excecution
   GLuint shaderProgram = glCreateProgram();
   GLuint vertexShader =  gl_loadShader(vertexPath,GL_VERTEX_SHADER);
   GLuint fragmentShader =  gl_loadShader(fragmentPath, GL_FRAGMENT_SHADER);
   // Attatch both of the shaders to the program and link the program
   glAttachShader(shaderProgram, vertexShader);
   glAttachShader(shaderProgram, fragmentShader);
   glLinkProgram(shaderProgram); // This links all the previously attatched shaders with outputs and inputs
   // Make sure everything compiled correctly
   int  success;
   char infoLog[512];
   glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
   if(!success) {
      glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
      std::cout << "--- Shader Program Failed Compilation ---" << infoLog <<  std::endl;
   }
   // After the shaders are linked in the program we no longer need them
   glDeleteShader(vertexShader);
   glDeleteShader(fragmentShader);  
   return shaderProgram;
}
