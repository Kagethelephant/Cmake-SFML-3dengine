#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

// // window.hpp additions
// #include <unordered_map>

// class window {
// public:
//     GLFWwindow* win;
//     
//     // Track previous key states
//     std::unordered_map<int, int> prevKeyState;

//     enum class KeyMode { Pressed, PressedOnce, Released };

//     bool checkKey(int key, KeyMode mode = KeyMode::Pressed);
// };


//// window.cpp additions

// bool window::checkKey(int key, KeyMode mode){
//     int current = glfwGetKey(win, key);
//     int previous = prevKeyState[key];

//     bool result = false;
//     switch(mode){
//         case KeyMode::Pressed:
//             result = (current == GLFW_PRESS);
//             break;
//         case KeyMode::PressedOnce:
//             result = (current == GLFW_PRESS && previous != GLFW_PRESS);
//             break;
//         case KeyMode::Released:
//             result = (current == GLFW_RELEASE && previous == GLFW_PRESS);
//             break;
//     }

//     // update previous state for next frame
//     prevKeyState[key] = current;
//     return result;
// }

class window {

public:

   window(int _height);
   ~window();

   GLFWwindow* win;

   // FixedFBO fbo;
   int fboWidth, fboHeight;

   int windowWidth, windowHeight;
   float windowAspect;
   float targetAspect;


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
