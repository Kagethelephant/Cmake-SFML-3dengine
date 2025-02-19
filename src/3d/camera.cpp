//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include "camera.hpp"
#include "utils/matrix.hpp"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Update the camera view matrix based on the new position and angle
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void camera::update(float x, float y, float z, float u, float v, float w) {
   
   // Update the xyz and uvw values (the position is changing direction based on look direction)
   position += (direction * z) + (direction.cross(up) * x);
   rotation += vec3(u,v,w);
   // Rotate using the rotation matricies and normalize it
   // Start with the difault direction vector which is just 1 in the z direction
   direction = vec3(0,0,1) * transformation_matrix(0, 0, 0, rotation.x, rotation.y, rotation.z);
   direction.normalize();
   // Target can be any distance away from the object as long as it is the correct direction
   target = (position + direction);
   // Calculate the point at matrix and view matrix (black box)
   point = point_matrix(position, target, up);
   view = view_matrix(point);
}
