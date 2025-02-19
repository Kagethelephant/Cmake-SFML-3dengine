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

   // float  u = 0, v = 0, w = 0; //x = 0, y = 0, z = 0,
   
   // Position along x, y, z
   // Rotation about x, y, z (u-pitch, v-roll, w-yaw)
   vec3 position = vec3(0,0,0);
   vec3 rotation = vec3(0,0,0);
   
   // Up - vector storing up direction
   // Target - the position of the look at target
   // direction - vector storing the forward direction
   vec3 up = vec3(0,1,0);
   vec3 target = vec3(0,0,0);
   vec3 direction = vec3(0,0,1);


   mat4x4 point; 
   mat4x4 view; 
   
   // Alter the position and rotation vectors
   // the move function moves relitive to the look direction
   void move(float _x, float _y, float _z);
   void rotate(float _u, float _v, float _w);

private:

   // This updates all of the matrices based on the new position and rotation
   // This gets called by the move and rotate functions
   mat4x4 update();
};
