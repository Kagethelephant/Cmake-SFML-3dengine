#pragma once

#include <cmath>


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Vector / Matrix objects with overloads
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

/// @brief: Simple 4x4 matrix with multiplication overload
struct mat4x4 {

   float m[4][4] = {0.0f};
   
   // Overload for multiplying two matrices
   mat4x4 operator * (const mat4x4 &m2){
      mat4x4 matrix;
      for (int i = 0; i < 4; i++)
         for (int j = 0; j < 4; j++)
            matrix.m[j][i] = this->m[j][0] * m2.m[0][i] + this->m[j][1] * m2.m[1][i] + this->m[j][2] * m2.m[2][i] + this->m[j][3] * m2.m[3][i];
      return matrix;
   }
};


/// @brief: 3d vector with operator overloads
/// @param x: x component of vector (Default: 0)
/// @param y: y component of vector (Default: 0)
struct vec2 {

   float x,y;

   // Constructors (Member initializer list)
   vec2() : x(0), y(0) {}
   vec2(float _x, float _y) : x(_x), y(_y) {}

   // Member Functions
   //---------------------------------------------------------------------------------------------
   /// @brief: returns the magnitude of the vector
   float mag() { return std::sqrt(std::pow(x,2) + std::pow(y,2)); }
   /// @brief: Returns the normalized vector so the magnitude is 1
   vec2 normal() { float m = mag(); return vec2(x/m, y/m); }
   /// @brief: Normalizes the vector so the magnitude is 2
   void normalize() { float m = mag(); x /= m; y /= m; }
   /// @brief: Dot producto of 2 vectors. This is escencially the likeness of 2 normalized vectors
   float dot(const vec2& v) { return ((this->x * v.x) + (this->y * v.y)); }
   // @brief: There is not necessarily a 2D cross product but you can use this to determine if a vector points to the left or right of another vector
   float cross(const vec2& v) { return ((this->x * v.y) - (this->y * v.x)); }

   // Overload functions
   //---------------------------------------------------------------------------------------------
   // Standard Operators
   vec2 operator + (const vec2& v) {return vec2(this->x + v.x, this->y + v.y); } // Add 2 vectors
   vec2 operator - (const vec2& v) {return vec2(this->x - v.x, this->y - v.y); } // Subtract 2 vectors
   vec2 operator * (const float& f) {return vec2(this->x * f, this->y * f); } // Scale vector by float
   vec2 operator / (const float& f) {return vec2(this->x / f, this->y / f); } // Scale vector by float
   // Compound Operators
   void operator += (const vec2& v) { this->x += v.x; this->y += v.y; } // Add 2 vectors
   void operator -= (const vec2& v) { this->x -= v.x; this->y -= v.y; } // Subtract 2 vectors
   void operator *= (const float& f) { this->x *= f; this->y *= f; } // Scale vector by float
   void operator /= (const float& f) { this->x /= f; this->y /= f; } // Scale vector by float
   // Dot product overload
   float operator * (const vec2& v) {return (this->x * v.x + this->y * v.y); }
};


/// @brief 3d vector with an optional constructor
/// @param x: x component of vector (Default: 0)
/// @param y: y component of vector (Default: 0)
/// @param z: z component of vector (Default: 0) 
/// @param w: Extra component used for multiplication with 4x4 matrices (Default = 1)
struct vec3 {

   float x,y,z,w;

   // Constructors (member initializer list)
   vec3() : x(0), y(0), z(0), w(1) {}
   vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z), w(1) {}
   vec3(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

   // Member functions
   //---------------------------------------------------------------------------------------------
   /// @brief: returns the magnitude of the vector
   float mag() { return std::sqrt(std::pow(x,2) + std::pow(y,2) + std::pow(z,2)); }
   /// @brief: Returns the normalized vector so the magnitude is 1
   vec3 normal() { float m = mag(); return vec3(x/m, y/ m, z/m, w); }
   /// @brief: Normalizes the vector so the magnitude is 2
   void normalize() { float m = mag(); x /= m; y /= m; z /= m; }
   /// @brief: Dot producto of 2 vectors. This is escencially the likeness of 2 normalized vectors
   float dot(const vec3& v) { return ((this->x * v.x) + (this->y * v.y) + (this->z * v.z)); }
   // @brief: Cross product of 2 vectors that will return the normal vector of the plane created from the 2 vectors
   vec3 cross(const vec3& v) { return vec3(this->y * v.z - this->z * v.y, this->z * v.x - this->x * v.z, this->x * v.y - this->y * v.x, this->w); }

   // Operator overloads
   //---------------------------------------------------------------------------------------------
   // Standard Operators 
   vec3 operator + (const vec3& v) {return vec3(this->x + v.x, this->y + v.y, this->z + v.z, this->w); } // Add 2 vectors
   vec3 operator - (const vec3& v) {return vec3(this->x - v.x, this->y - v.y, this->z - v.z, this->w); } // Subtract 2 vectors
   vec3 operator * (const float& f) {return vec3(this->x * f, this->y * f, this->z * f, this->w); } // Scale vector by float
   vec3 operator / (const float& f) {return vec3(this->x / f, this->y / f, this->z / f, this->w); } // Scale vector by float
   // Compound Operators
   void operator += (const vec3& v) { this->x += v.x; this->y += v.y; this->z += v.z; } // Add 2 vectors
   void operator -= (const vec3& v) { this->x -= v.x; this->y -= v.y; this->z -= v.z; } // Subtract 2 vectors
   void operator *= (const float& f) { this->x *= f; this->y *= f; this->z *= f; } // Scale vector by float
   void operator /= (const float& f) { this->x /= f; this->y /= f; this->z /= f; } // Scale vector by float
   // Dot product overload
   float operator * (const vec3& v) {return (this->x * v.x) + (this->y * v.y) + (this->z * v.z); }

   // Overload for multiplying a vector against a matrix
   vec3 operator * (const mat4x4& m) {
      vec3 v;
      v.x = this->x * m.m[0][0] + this->y * m.m[1][0] + this->z * m.m[2][0] + this->w * m.m[3][0];
      v.y = this->x * m.m[0][1] + this->y * m.m[1][1] + this->z * m.m[2][1] + this->w * m.m[3][1];
      v.z = this->x * m.m[0][2] + this->y * m.m[1][2] + this->z * m.m[2][2] + this->w * m.m[3][2];
      v.w = this->x * m.m[0][3] + this->y * m.m[1][3] + this->z * m.m[2][3] + this->w * m.m[3][3];
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
      if ( v.w != 0.0f) {v.x /= v.w; v.y /= v.w; v.z /= v.w;}
      this-> x = v.x;
      this-> y = v.y;
      this-> z = v.z;
   }
};


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Matrix Functions
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

/// @brief: Creates a matrix that will move a vertex in 3D space to a position that reflects its
/// position reletive to the camera view. Essentially moving the world around a camera instead 
/// of moving the camera. The camera view is represented by a matrix created with the "point_matrix"
/// the direction and position given to the "point_matrix" represents the camera
/// @param m: point_matrix result representing the camera (by reference)
/// @return mat4x4
///
mat4x4 matrix_scale(float sx, float sy, float sz);

/// @brief: Creates a 4x4 matrix that can be used to translate and rotate (in radians) a vertex 
/// @param x: x translation
/// @param y: y translation
/// @param z: z translation
/// @param u: u rotation around origin in radians
/// @param v: v rotation around origin in radians
/// @param w: w rotation around origin in radians
/// @return mat4x4
///
mat4x4 matrix_transform(float x, float y, float z, float u, float v, float w);

/// @brief: Creates the 3d projection matrix that transforms a 3D vertex to screen space
/// @param fov: Field of view in degrees
/// @param a: Aspect ratio of the display
/// @param n: Position in pixels scale of the near plane
/// @param f: Position in pixel scale of the far plane
/// @return mat4x4
///
mat4x4 matrix_project(float fov, float a, float n, float f);

/// @brief: Creates a matrix that will rotate a 3D vertex around its origin so the z axis
/// points towards the provided 3D vertex
/// @param pos: vec3 of the origin position of the object (by reference)
/// @param target: vec3 position of the point for the object to point at (by reference)
/// @param up: vec3 of the direction of the y axis (by reference)
/// @return mat4x4
///
mat4x4 matrix_pointAt(vec3 &pos, vec3 &target, vec3 &up);

/// @brief: Creates a matrix that will move a vertex in 3D space to a position that reflects its
/// position reletive to the camera view. Essentially moving the world around a camera instead 
/// of moving the camera. The camera view is represented by a matrix created with the "point_matrix"
/// the direction and position given to the "point_matrix" represents the camera
/// @param m: point_matrix result representing the camera (by reference)
/// @return mat4x4
///
mat4x4 matrix_view(mat4x4 &m);
