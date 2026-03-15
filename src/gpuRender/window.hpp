#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <unordered_map>


class window {

public:

   window(int _height);
   ~window();

   GLFWwindow* win;

   // FixedFBO fbo;
   int fboWidth, fboHeight;

   int windowWidth, windowHeight;
   float aspectRatio;

   std::unordered_map<int, int> prevKeyState;
   enum class KeyMode { Pressed, PressedOnce, Released };
   bool checkKey(int key, KeyMode mode = KeyMode::Pressed);

   GLuint fbo = 0;
   GLuint colorTex = 0;
   GLuint depth = 0;

   GLuint UIvao;
   GLuint UIvbo;


   double lastTime = glfwGetTime();
   double frameTime;
   double currentTime;
   int frameCount = 0;
   int fps;

   /// @brief: Shader program to render 2D quads with textures
   GLuint shaderProgramUI;

   std::vector<float> quadVertices;
   void frameUpdate();
   // void bindFBO();

   bool resizePending = false;

   void resize();
};
