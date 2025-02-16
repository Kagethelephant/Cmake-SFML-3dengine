
//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include "matrix.hpp"


mat4x4 identity_matrix() {

   mat4x4 m;
   m.m[0][0] = 1.0f; m.m[1][0] = 0.0f; m.m[2][0] = 0.0f; m.m[3][0] = 0.0f;
   m.m[0][1] = 0.0f; m.m[1][1] = 1.0f; m.m[2][1] = 0.0f; m.m[3][1] = 0.0f;
   m.m[0][2] = 0.0f; m.m[1][2] = 0.0f; m.m[2][2] = 1.0f; m.m[3][2] = 0.0f;
   m.m[0][3] = 0.0f; m.m[1][3] = 0.0f; m.m[2][3] = 0.0f; m.m[3][3] = 1.0f;	
   return m;
}

mat4x4 rotate_x_matrix(float u) {

   mat4x4 m;
   m.m[0][0] = 1.0f; m.m[1][0] = 0.0f;    m.m[2][0] = 0.0f;     m.m[3][0] = 0.0f;
   m.m[0][1] = 0.0f; m.m[1][1] = cosf(u); m.m[2][1] = -sinf(u); m.m[3][1] = 0.0f;
   m.m[0][2] = 0.0f; m.m[1][2] = sinf(u); m.m[2][2] = cosf(u);  m.m[3][2] = 0.0f;
   m.m[0][3] = 0.0f; m.m[1][3] = 0.0f;    m.m[2][3] = 0.0f;     m.m[3][3] = 1.0f;
   return m;
}

mat4x4 rotate_y_matrix(float v) {

   mat4x4 m;
   m.m[0][0] = cosf(v); m.m[1][0] = 0.0f; m.m[2][0] = -sinf(v); m.m[3][0] = 0.0f;
   m.m[0][1] = 0.0f;    m.m[1][1] = 1.0f; m.m[2][1] = 0.0f;     m.m[3][1] = 0.0f;
   m.m[0][2] = sinf(v); m.m[1][2] = 0.0f; m.m[2][2] = cosf(v);  m.m[3][2] = 0.0f;
   m.m[0][3] = 0.0f;    m.m[1][3] = 0.0f; m.m[2][3] = 0.0f;     m.m[3][3] = 1.0f;
   return m;
}

mat4x4 rotate_z_matrix(float w) {

   mat4x4 m;
   m.m[0][0] = cosf(w); m.m[1][0] = -sinf(w); m.m[2][0] = 0.0f; m.m[3][0] = 0.0f;
   m.m[0][1] = sinf(w); m.m[1][1] = cosf(w);  m.m[2][1] = 0.0f; m.m[3][1] = 0.0f;
   m.m[0][2] = 0.0f;    m.m[1][2] = 0.0f;     m.m[2][2] = 1.0f; m.m[3][2] = 0.0f;
   m.m[0][3] = 0.0f;    m.m[1][3] = 0.0f;     m.m[2][3] = 0.0f; m.m[3][3] = 1.0f;
   return m;
}

mat4x4 translate_matrix(float x, float y, float z) {

   mat4x4 m;
   m.m[0][0] = 1.0f; m.m[1][0] = 0.0f; m.m[2][0] = 0.0f; m.m[3][0] = x;
   m.m[0][1] = 0.0f; m.m[1][1] = 1.0f; m.m[2][1] = 0.0f; m.m[3][1] = y;
   m.m[0][2] = 0.0f; m.m[1][2] = 0.0f; m.m[2][2] = 1.0f; m.m[3][2] = z;
   m.m[0][3] = 0.0f; m.m[1][3] = 0.0f; m.m[2][3] = 0.0f; m.m[3][3] = 1.0f;
   return m;
}

mat4x4 project_matrix(float fFovDegrees, float fAspectRatio, float fNear, float fFar) {
   // m[1][1] is not normally negative but since we are drawing as y = 0 is at the top of the screen
   // we need to invert the y values since y = 0 should be towards the bottom of the screen for most models
   float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);
   float fDisRatio = fFar / (fFar - fNear);
   mat4x4 m;
   m.m[0][0] = fAspectRatio * fFovRad; m.m[1][0] = 0.0f;     m.m[2][0] = 0.0f;      m.m[3][0] = 0.0f;
   m.m[0][1] = 0.0f;                   m.m[1][1] = -fFovRad; m.m[2][1] = 0.0f;      m.m[3][1] = 0.0f;
   m.m[0][2] = 0.0f;                   m.m[1][2] = 0.0f;     m.m[2][2] = fDisRatio; m.m[3][2] = (-fFar * fNear) / (fFar - fNear);
   m.m[0][3] = 0.0f;                   m.m[1][3] = 0.0f;     m.m[2][3] = 1.0f;      m.m[3][3] = 0.0f;
   return m;
}

mat4x4 point_matrix(vec3 &pos, vec3 &target, vec3 &up) {

   mat4x4 m;
   // Calculate new forward direction
   vec3 newForward = target - pos;
   newForward = newForward.norm();

   // Calculate new Up direction
   vec3 a = newForward * up.dot(newForward);
   vec3 newUp = up - a;
   newUp = newUp.norm();

   // New Right direction is easy, its just cross product
   vec3 newRight = newUp.cross(newForward);

   m.m[0][0] = newRight.x; m.m[1][0] = newUp.x; m.m[2][0] = newForward.x; m.m[3][0] = pos.x;
   m.m[0][1] = newRight.y; m.m[1][1] = newUp.y; m.m[2][1] = newForward.y; m.m[3][1] = pos.y;
   m.m[0][2] = newRight.z; m.m[1][2] = newUp.z; m.m[2][2] = newForward.z; m.m[3][2] = pos.z;
   m.m[0][3] = 0.0f;       m.m[1][3] = 0.0f;    m.m[2][3] = 0.0f;         m.m[3][3] = 1.0f;
   return m;
}

mat4x4 view_matrix(mat4x4 &m) { // Only for Rotation/Translation Matrices

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

vec3 mat_multiply(vec3& v, mat4x4& m) {

   vec3 v2;
   v2.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + v.w * m.m[3][0];
   v2.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + v.w * m.m[3][1];
   v2.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + v.w * m.m[3][2];
   v2.w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + v.w * m.m[3][3];
   return v2;
}
