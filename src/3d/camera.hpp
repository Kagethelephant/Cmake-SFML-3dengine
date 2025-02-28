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
   
   // Position along x, y, z
   // Rotation about x, y, z (u-pitch, v-roll, w-yaw)
   vec3 position = vec3(0,0,0);
   vec3 rotation = vec3(0,0,0);
   
   // Vector storing the forward direction
   vec3 direction = vec3(0,0,1);

   // The point matrix is just used to create the view matrix
   // but the view matrix is used by all objects to put themselvs in the view
   mat4x4 point; 
   mat4x4 view; 
   
   // This updates all of the matrices based on the new position and rotation
   // This gets called by the move and rotate functions
   void update(float x, float y, float z, float u, float v, float w);

private:

};
