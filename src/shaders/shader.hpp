#include <glad/glad.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <string>



GLuint createShaderProgram(std::string vertexPath, std::string fragmentPath);

GLuint gl_loadShader(std::string filename, GLuint type);

std::string readShaderFile(const std::string& filePath);
