#include "object.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <math.h>
#include "utils/matrix.hpp"



// UPDATE POSITION
//---------------------------------------------------------------------------------------------
void object::scale(float sx, float sy, float sz){
   scales = vec3(sx,sy,sz);
   matScale = matrix_scale(scales[0], scales[1], scales[2]);
}


void object::move(float x, float y, float z){
   position = vec3(x,y,z);
   matTransform = matrix_transform(position[0], position[1], position[2], rotation[0], rotation[1], rotation[2]);
}


void object::rotate(float u, float v, float w){
   rotation = vec3(u,v,w);
   matTransform = matrix_transform(position[0], position[1], position[2], rotation[0], rotation[1], rotation[2]);
}
