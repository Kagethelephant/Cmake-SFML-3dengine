//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include "camera.hpp"
#include "utils/matrix.hpp"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Update the camera view matrix based on the new position and angle
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void camera::update(float x, float y, float z, float u, float v, float w) {
   
   // Rotate using the rotation matricies and normalize it
   // Start with the difault direction vector which is just 1 in the z direction
   vec3 up = vec3(0,1,0) * transformation_matrix(0, 0, 0, rotation.x, rotation.y, rotation.z);
   direction = vec3(0,0,1) * transformation_matrix(0, 0, 0, rotation.x, rotation.y, rotation.z);

   // Update the xyz and uvw values (the position is changing direction based on look direction)
   position += (direction * z) + (direction.cross(up) * x);
   position.y += y;
   rotation += vec3(u,v,w);

   // Calculate the point at matrix and view matrix (black box)
   point = point_matrix(position, direction, up);
   view = view_matrix(point);
}
