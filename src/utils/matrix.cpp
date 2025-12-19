//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Headers
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#include "matrix.hpp"


mat4x4 transformation_matrix(float x, float y, float z, float u, float v, float w) {

   mat4x4 m;
   m.m[0][0] = (cosf(v)*cosf(w)); m.m[1][0] = ((sinf(u)*-sinf(v))*cosf(w)) + (cosf(u)*-sinf(w)); 
   m.m[0][1] = (cosf(v)*sinf(w)); m.m[1][1] = ((sinf(u)*-sinf(v))*sinf(w)) + (cosf(u)*cosf(w));  
   m.m[0][2] = sinf(v);           m.m[1][2] = (sinf(u)*cos(v));                                  
   m.m[0][3] = 0.0f;              m.m[1][3] = 0.0f;                                              

   m.m[2][0] = ((cosf(u)*-sinf(v))*cosf(w)) + (-sinf(u)*-sinf(w)); m.m[3][0] = x;
   m.m[2][1] = ((cosf(u)*-sinf(v))*sinf(w)) + (-sinf(u)*cosf(w));  m.m[3][1] = y;
   m.m[2][2] = (cos(u)*cos(v));                                    m.m[3][2] = z;
   m.m[2][3] = 0.0f;                                               m.m[3][3] = 1.0f;

   return m;
}


mat4x4 project_matrix(float fov, float a, float n, float f) {
   // m[1][1] is not normally negative but since we are drawing as y = 0 is at the top of the screen
   // we need to invert the y values since y = 0 should be towards the bottom of the screen for most models
   float r = n * tanf(fov * 0.5f / 180.0f * 3.14159f);
   float t = r / a;

   mat4x4 m;
   m.m[0][0] = -n / r; m.m[1][0] = 0.0f;  m.m[2][0] = 0.0f;         m.m[3][0] = 0.0f;
   m.m[0][1] = 0.0f;  m.m[1][1] = n / t; m.m[2][1] = 0.0f;         m.m[3][1] = 0.0f;
   m.m[0][2] = 0.0f;  m.m[1][2] = 0.0f;  m.m[2][2] = (f+n)/(n-f);  m.m[3][2] = (2*f*n)/(n-f);
   m.m[0][3] = 0.0f;  m.m[1][3] = 0.0f;  m.m[2][3] = -1.0f;        m.m[3][3] = 0.0f;
   return m;
}


mat4x4 point_matrix(vec3 &pos, vec3 &target, vec3 &up) {

   // Calculate new Up direction
   vec3 a = target * up.dot(target);
   vec3 newUp = (up - a).normal();
   // New Right direction is just the cross product
   vec3 newRight = newUp.cross(target);

   mat4x4 m;
   m.m[0][0] = newRight.x; m.m[1][0] = newUp.x; m.m[2][0] = target.x; m.m[3][0] = pos.x;
   m.m[0][1] = newRight.y; m.m[1][1] = newUp.y; m.m[2][1] = target.y; m.m[3][1] = pos.y;
   m.m[0][2] = newRight.z; m.m[1][2] = newUp.z; m.m[2][2] = target.z; m.m[3][2] = pos.z;
   m.m[0][3] = 0.0f;       m.m[1][3] = 0.0f;    m.m[2][3] = 0.0f;         m.m[3][3] = 1.0f;
   return m;
}


mat4x4 view_matrix(mat4x4 &m) {

   mat4x4 m2;
   m2.m[0][0] = m.m[0][0]; m2.m[0][1] = m.m[1][0]; m2.m[0][2] = m.m[2][0]; m2.m[0][3] = 0.0f;
   m2.m[1][0] = m.m[0][1]; m2.m[1][1] = m.m[1][1]; m2.m[1][2] = m.m[2][1]; m2.m[1][3] = 0.0f;
   m2.m[2][0] = m.m[0][2]; m2.m[2][1] = m.m[1][2]; m2.m[2][2] = m.m[2][2]; m2.m[2][3] = 0.0f;

   m2.m[3][0] = -(m.m[3][0] * m2.m[0][0] + m.m[3][1] * m2.m[1][0] + m.m[3][2] * m2.m[2][0]);
   m2.m[3][1] = -(m.m[3][0] * m2.m[0][1] + m.m[3][1] * m2.m[1][1] + m.m[3][2] * m2.m[2][1]);
   m2.m[3][2] = -(m.m[3][0] * m2.m[0][2] + m.m[3][1] * m2.m[1][2] + m.m[3][2] * m2.m[2][2]);
   m2.m[3][3] = 1.0f;
   return m2;
}
