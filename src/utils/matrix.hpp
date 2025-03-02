#pragma once

//////////////////////////////////////////////////////////////////
/// HEADERS
//////////////////////////////////////////////////////////////////
#include <cmath>



//////////////////////////////////////////////////////////////////
/// \brief Simple 4x4 matrix, good for 3D rendering
//////////////////////////////////////////////////////////////////
struct mat4x4 {

   float m[4][4] = {0.0f};

   mat4x4 operator * (const mat4x4 &m2){
      mat4x4 matrix;
      for (int c = 0; c < 4; c++)
         for (int r = 0; r < 4; r++)
            matrix.m[r][c] = this->m[r][0] * m2.m[0][c] + this->m[r][1] * m2.m[1][c] + this->m[r][2] * m2.m[2][c] + this->m[r][3] * m2.m[3][c];
      return matrix;
   }
};



//////////////////////////////////////////////////////////////////
/// \brief 3d vector with an optional constructor
//////////////////////////////////////////////////////////////////
struct vec2 {

   float x,y;

   // Constructors
   vec2() : x(0), y(0) {}
   vec2(float _x, float _y) : x(_x), y(_y) {}

   // Non overload functions
   // Gives the magnitude of the vector
   float mag() { return std::sqrt(std::pow(x,2) + std::pow(y,2)); }
   // This does not give a normal, this normalizes the vector so its magnitude is 1
   vec2 normal() { float m = mag(); return vec2(x/m, y/m); }
   // This does not give a normal, this normalizes the vector so its magnitude is 1
   void normalize() { float m = mag(); x /= m; y /= m; }
   // This will give the likeness of 2 vectors, this only makes sense with normalized direction vectors
   float dot(const vec2& v) { return ((this->x * v.x) + (this->y * v.y)); }

   // Overload functions
   // Scalars
   vec2 operator + (const vec2& v) {return vec2(this->x + v.x, this->y + v.y); }
   vec2 operator - (const vec2& v) {return vec2(this->x - v.x, this->y - v.y); }
   vec2 operator * (const float& f) {return vec2(this->x * f, this->y * f); }
   vec2 operator / (const float& f) {return vec2(this->x / f, this->y / f); }
   // Scalar modifiers
   void operator += (const vec2& v) { this->x += v.x; this->y += v.y; }
   void operator -= (const vec2& v) { this->x -= v.x; this->y -= v.y; }
   void operator *= (const float& f) { this->x *= f; this->y *= f; }
   void operator /= (const float& f) { this->x /= f; this->y /= f; }
   // Dot product overload
   float operator * (const vec2& v) {return (this->x * v.x + this->y * v.y); }
};


//////////////////////////////////////////////////////////////////
/// \brief 3d vector with an optional constructor
//////////////////////////////////////////////////////////////////
struct vec3 {

   float x,y,z,w;

   // Constructors
   vec3() : x(0), y(0), z(0), w(1) {}
   vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z), w(1) {}
   vec3(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

   // Non overload functions
   // This gives the magnitude of a given vector
   float mag() { return std::sqrt(std::pow(x,2) + std::pow(y,2) + std::pow(z,2)); }
   // This does not give a normal, this normalizes a vector so the magnitude is 1
   vec3 normal() { float m = mag(); return vec3(x/m, y/ m, z/m, w); }
   // This does not give a normal, this normalizes a vector so the magnitude is 1
   void normalize() { float m = mag(); x /= m; y /= m; z /= m; }
   // This gives the likeness of the 2 vectors, this only makes sense on normalized direction vectors
   float dot(const vec3& v) { return ((this->x * v.x) + (this->y * v.y) + (this->z * v.z)); }
   // This gives the the normal line between the 2 given interscecting vectors (at least i think they need to interscect)
   vec3 cross(const vec3& v) { return vec3(this->y * v.z - this->z * v.y, this->z * v.x - this->x * v.z, this->x * v.y - this->y * v.x, this->w); }

   // Overload functions
   // Scalars
   vec3 operator + (const vec3& v) {return vec3(this->x + v.x, this->y + v.y, this->z + v.z, this->w); }
   vec3 operator - (const vec3& v) {return vec3(this->x - v.x, this->y - v.y, this->z - v.z, this->w); }
   vec3 operator * (const float& f) {return vec3(this->x * f, this->y * f, this->z * f, this->w); }
   vec3 operator / (const float& f) {return vec3(this->x / f, this->y / f, this->z / f, this->w); }
   // Scalar modifiers
   void operator += (const vec3& v) { this->x += v.x; this->y += v.y; this->z += v.z; }
   void operator -= (const vec3& v) { this->x -= v.x; this->y -= v.y; this->z -= v.z; }
   void operator *= (const float& f) { this->x *= f; this->y *= f; this->z *= f; }
   void operator /= (const float& f) { this->x /= f; this->y /= f; this->z /= f; }
   // Dot product overload
   float operator * (const vec3& v) {return (this->x * v.x) + (this->y * v.y) + (this->z * v.z); }


   // Overload for multiplying a vector against a matrix
   vec3 operator * (const mat4x4& m) {
      vec3 v;
      v.x = this->x * m.m[0][0] + this->y * m.m[1][0] + this->z * m.m[2][0] + this->w * m.m[3][0];
      v.y = this->x * m.m[0][1] + this->y * m.m[1][1] + this->z * m.m[2][1] + this->w * m.m[3][1];
      v.z = this->x * m.m[0][2] + this->y * m.m[1][2] + this->z * m.m[2][2] + this->w * m.m[3][2];
      v.w = this->x * m.m[0][3] + this->y * m.m[1][3] + this->z * m.m[2][3] + this->w * m.m[3][3];
      // Create a psuedo 4th variable to multiply by the 4x4 matrix and treat it as if it where part of the vector
      // float w = (this->x * m.m[0][3] + this->y * m.m[1][3] + this->z * m.m[2][3] + m.m[3][3]);
      if ( v.w != 0.0f) {v.x /= v.w; v.y /= v.w; v.z /= v.w;}
      return v;
   }
   
   // Overload for multiplying a vector against a matrix
   void operator *= (const mat4x4& m) {
      vec3 v;
      v.x = this->x * m.m[0][0] + this->y * m.m[1][0] + this->z * m.m[2][0] + this->w * m.m[3][0];
      v.y = this->x * m.m[0][1] + this->y * m.m[1][1] + this->z * m.m[2][1] + this->w * m.m[3][1];
      v.z = this->x * m.m[0][2] + this->y * m.m[1][2] + this->z * m.m[2][2] + this->w * m.m[3][2];
      v.w = this->x * m.m[0][3] + this->y * m.m[1][3] + this->z * m.m[2][3] + this->w * m.m[3][3];
      // // Create a psuedo 4th variable to multiply by the 4x4 matrix and treat it as if it where part of the vector
      // float w = (this->x * m.m[0][3] + this->y * m.m[1][3] + this->z * m.m[2][3] + m.m[3][3]);
      if ( v.w != 0.0f) {v.x /= v.w; v.y /= v.w; v.z /= v.w;}
      this-> x = v.x;
      this-> y = v.y;
      this-> z = v.z;
   }
};


//////////////////////////////////////////////////////////////////
/// Matrix Functions
//////////////////////////////////////////////////////////////////

mat4x4 transformation_matrix(float x, float y, float z, float u, float v, float w);

mat4x4 project_matrix(float fFovDegrees, float fAspectRatio, float fNear, float fFar);

mat4x4 point_matrix(vec3 &pos, vec3 &target, vec3 &up); // These last 2 i dont really understand

mat4x4 view_matrix(mat4x4 &m); // Only for Rotation/Translation Matrices





