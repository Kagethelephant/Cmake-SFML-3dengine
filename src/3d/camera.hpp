#pragma once

//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include "utils/matrix.hpp"


//////////////////////////////////////////////////////////////////
/// \brief 3d camera
//////////////////////////////////////////////////////////////////
class camera {

public:

   float  u = 0, v = 0, w = 0; //x = 0, y = 0, z = 0,

   vec3 direction = vec3(0,0,1);
   vec3 position = vec3(0,0,0);
   vec3 up = vec3(0,1,0);
   vec3 target = vec3(0,0,0);

   mat4x4 point = identity_matrix();
   mat4x4 view = identity_matrix();

   mat4x4 update();
   void move(float _x, float _y, float _z);
};
