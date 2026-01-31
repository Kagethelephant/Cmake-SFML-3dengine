#pragma once

#include <cmath>
#include <array>
#include <iostream>


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Vector / Matrix objects with overloads
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

/// @brief: Simple 4x4 matrix with multiplication overload
struct mat4x4 {

   float m[4][4] = {0.0f};
   
   // Overload for multiplying two matrices
   mat4x4 operator * (const mat4x4 &m2) const {
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

   // Components
   std::array<float, 2> c;
   // Constructors (Member initializer list)
   vec2() : c{0,0} {}
   vec2(float x, float y) : c{x,y} {}
   
   // Swizzle functions
   float& x() {return c[0];}
   float x() const {return c[0];}

   float& y() {return c[1];}
   float y() const {return c[1];}

   // Member Functions
   //---------------------------------------------------------------------------------------------
   /// @brief: returns the magnitude of the vector
   float mag() const { return std::sqrt(std::pow(c[0],2) + std::pow(c[1],2)); }
   /// @brief: Returns the normalized vector so the magnitude is 1
   vec2 normal() const { float m = mag(); return vec2(c[0]/m, c[1]/m); }
   /// @brief: Normalizes the vector so the magnitude is 2
   void normalize() { float m = mag(); c[0] /= m; c[1] /= m; }
   /// @brief: Dot producto of 2 vectors. This is escencially the likeness of 2 normalized vectors
   float dot(const vec2& v) const { return ((this->c[0] * v.c[0]) + (this->c[1] * v.c[1])); }
   // @brief: There is not necessarily a 2D cross product but you can use this to determine if a vector points to the left or right of another vector
   float cross(const vec2& v) const { return ((this->c[0] * v.c[1]) - (this->c[1] * v.c[0])); }
   // @brief: Print the vector parameters
   void print() const {std::cout << "{" << c[0] << ", " << c[1] << ", " << "}" << std::endl;}

   // Overload functions
   //---------------------------------------------------------------------------------------------
   //Subscript operator for compatability with opengl
   float& operator [] (int i) { return c.at(i);}
   float operator [] (int i) const { return c.at(i);}
   // Standard Operators
   vec2 operator + (const vec2& v) const {return vec2(this->c[0] + v.c[0], this->c[1] + v.c[1]); } // Add 2 vectors
   vec2 operator - (const vec2& v) const {return vec2(this->c[0] - v.c[0], this->c[1] - v.c[1]); } // Subtract 2 vectors
   vec2 operator * (const float& f) const {return vec2(this->c[0] * f, this->c[1] * f); } // Scale vector by float
   vec2 operator / (const float& f) const {return vec2(this->c[0] / f, this->c[1] / f); } // Scale vector by float
   // Compound Operators
   void operator += (const vec2& v) { this->c[0] += v.c[0]; this->c[1] += v.c[1]; } // Add 2 vectors
   void operator -= (const vec2& v) { this->c[0] -= v.c[0]; this->c[1] -= v.c[1]; } // Subtract 2 vectors
   void operator *= (const float& f) { this->c[0] *= f; this->c[1] *= f; } // Scale vector by float
   void operator /= (const float& f) { this->c[0] /= f; this->c[1] /= f; } // Scale vector by float
   // Componentwise multiplication overload
   vec2 operator * (const vec2& v) const {return vec2(this->c[0] * v.c[0], this->c[1] * v.c[1]); }
};


/// @brief 3d vector with an optional constructor
/// @param x: x component of vector (Default: 0)
/// @param y: y component of vector (Default: 0)
/// @param z: z component of vector (Default: 0) 
/// @param w: Extra component used for multiplication with 4x4 matrices (Default = 1)
struct vec3 {

   std::array<float, 3> c;
   // Constructors (member initializer list)
   vec3() : c{0,0,0} {}
   vec3(float x, float y, float z) : c{x,y,z} {}
   // Swizzle constructors
   vec3(vec2 v2, float z) : c{v2[0], v2[1], z} {}
   // Swizzle functions
   float& x() {return c[0];}
   float x() const {return c[0];}

   float& y() {return c[1];}
   float y() const {return c[1];}

   float& z() {return c[2];}
   float z() const {return c[2];}

   vec2 xy() const {return vec2(c[0], c[1]);}

   // Member functions
   //---------------------------------------------------------------------------------------------
   /// @brief: returns the magnitude of the vector
   float mag() const { return std::sqrt(std::pow(c[0],2) + std::pow(c[1],2) + std::pow(c[2],2)); }
   /// @brief: Returns the normalized vector so the magnitude is 1
   vec3 normal() const { float m = mag(); return vec3(c[0]/m, c[1]/ m, c[2]/m); }
   /// @brief: Normalizes the vector so the magnitude is 2
   void normalize() { float m = mag(); c[0] /= m; c[1] /= m; c[2] /= m; }
   /// @brief: Dot producto of 2 vectors. This is escencially the likeness of 2 normalized vectors
   float dot(const vec3& v) const { return ((this->c[0] * v.c[0]) + (this->c[1] * v.c[1]) + (this->c[2] * v.c[2])); }
   // @brief: Cross product of 2 vectors that will return the normal vector of the plane created from the 2 vectors
   vec3 cross(const vec3& v) const { return vec3(this->c[1] * v.c[2] - this->c[2] * v.c[1], this->c[2] * v.c[0] - this->c[0] * v.c[2], this->c[0] * v.c[1] - this->c[1] * v.c[0]); }
   // @brief: Print the vector parameters
   void print() const {std::cout << "{" << c[0] << ", " << c[1] << ", " << c[2] << ", " << "}" << std::endl;}

   // Operator overloads
   //---------------------------------------------------------------------------------------------
   //Subscript operator for compatability with opengl
   float& operator [] (int i) { return c.at(i);}
   float operator [] (int i) const { return c.at(i);}
   // Standard Operators 
   vec3 operator + (const vec3& v) const {return vec3(this->c[0] + v.c[0], this->c[1] + v.c[1], this->c[2] + v.c[2]); } // Add 2 vectors
   vec3 operator - (const vec3& v) const {return vec3(this->c[0] - v.c[0], this->c[1] - v.c[1], this->c[2] - v.c[2]); } // Subtract 2 vectors
   vec3 operator * (const float& f) const {return vec3(this->c[0] * f, this->c[1] * f, this->c[2] * f); } // Scale vector by float
   vec3 operator / (const float& f) const {return vec3(this->c[0] / f, this->c[1] / f, this->c[2] / f); } // Scale vector by float
   // Compound Operators
   void operator += (const vec3& v) { this->c[0] += v.c[0]; this->c[1] += v.c[1]; this->c[2] += v.c[2]; } // Add 2 vectors
   void operator -= (const vec3& v) { this->c[0] -= v.c[0]; this->c[1] -= v.c[1]; this->c[2] -= v.c[2]; } // Subtract 2 vectors
   void operator *= (const float& f) { this->c[0] *= f; this->c[1] *= f; this->c[2] *= f; } // Scale vector by float
   void operator /= (const float& f) { this->c[0] /= f; this->c[1] /= f; this->c[2] /= f; } // Scale vector by float
   // Componentwise multiplication overload
   vec3 operator * (const vec3& v) {return vec3(this->c[0] * v.c[0], this->c[1] * v.c[1], this->c[2] * v.c[2]); }

   // Overload for multiplying a vector against a matrix
   vec3 operator * (const mat4x4& m) const {
      vec3 v;
      v.c[0]  = this->c[0] * m.m[0][0] + this->c[1] * m.m[1][0] + this->c[2] * m.m[2][0] + 1.0f * m.m[3][0];
      v.c[1]  = this->c[0] * m.m[0][1] + this->c[1] * m.m[1][1] + this->c[2] * m.m[2][1] + 1.0f * m.m[3][1];
      v.c[2]  = this->c[0] * m.m[0][2] + this->c[1] * m.m[1][2] + this->c[2] * m.m[2][2] + 1.0f * m.m[3][2];
      float w = this->c[0] * m.m[0][3] + this->c[1] * m.m[1][3] + this->c[2] * m.m[2][3] + 1.0f * m.m[3][3];
      return v;
   }
   
   // Overload for multiplying a vector against a matrix
   void operator *= (const mat4x4& m) {
      vec3 v;
      v.c[0]  = this->c[0] * m.m[0][0] + this->c[1] * m.m[1][0] + this->c[2] * m.m[2][0] + 1.0f * m.m[3][0];
      v.c[1]  = this->c[0] * m.m[0][1] + this->c[1] * m.m[1][1] + this->c[2] * m.m[2][1] + 1.0f * m.m[3][1];
      v.c[2]  = this->c[0] * m.m[0][2] + this->c[1] * m.m[1][2] + this->c[2] * m.m[2][2] + 1.0f * m.m[3][2];
      float w = this->c[0] * m.m[0][3] + this->c[1] * m.m[1][3] + this->c[2] * m.m[2][3] + 1.0f * m.m[3][3];
      this-> c[0] = v.c[0];
      this-> c[1] = v.c[1];
      this-> c[2] = v.c[2];
   }
};


/// @brief 3d vector with an optional constructor
/// @param x: x component of vector (Default: 0)
/// @param y: y component of vector (Default: 0)
/// @param z: z component of vector (Default: 0) 
/// @param w: Extra component used for multiplication with 4x4 matrices (Default = 1)
struct vec4 {

   
   std::array<float, 4> c;
   // Constructors (member initializer list)
   vec4() : c{0.0f, 0.0f, 0.0f, 1.0f} {}
   vec4(float x, float y, float z) : c{x, y, z, 1.0f} {}
   vec4(float x, float y, float z, float w) : c{x, y, z, w} {}
   // Swizzle constructors
   vec4(vec2 v2, float z, float w) : c{v2[0], v2[1], z, w} {}
   vec4(vec3 v3, float w) : c{v3[0], v3[1], v3[2], w} {}

   // Swizzle functions
   float& x() {return c[0];}
   float x() const {return c[0];}

   float& y() {return c[1];}
   float y() const {return c[1];}

   float& z() {return c[2];}
   float z() const {return c[2];}

   float& w() {return c[3];}
   float w() const {return c[3];}

   vec2 xy() const {return vec2(c[0], c[1]);}
   vec3 xyz() const {return vec3(c[0], c[1], c[2]);}

   // Member functions
   //---------------------------------------------------------------------------------------------
   /// @brief: returns the magnitude of the vector
   float mag() const { return std::sqrt(std::pow(c[0],2) + std::pow(c[1],2) + std::pow(c[2],2)); }
   /// @brief: Returns the normalized vector so the magnitude is 1
   vec4 normal() const { float m = mag(); return vec4(c[0]/m, c[1]/ m, c[2]/m, c[3]); }
   /// @brief: Normalizes the vector so the magnitude is 2
   void normalize() { float m = mag(); c[0] /= m; c[1] /= m; c[2] /= m; }
   /// @brief: Dot producto of 2 vectors. This is escencially the likeness of 2 normalized vectors
   float dot(const vec4& v) const { return ((this->c[0] * v.c[0]) + (this->c[1] * v.c[1]) + (this->c[2] * v.c[2])+ (this->c[3] * v.c[3])); }
   // @brief: Cross product of 2 vectors that will return the normal vector of the plane created from the 2 vectors
   vec4 cross(const vec4& v) const { return vec4(this->c[1] * v.c[2] - this->c[2] * v.c[1], this->c[2] * v.c[0] - this->c[0] * v.c[2], this->c[0] * v.c[1] - this->c[1] * v.c[0], this->c[3]); }
   // @brief: Devide by the w value (viewspace z value) after projection to give perspective, making far away objects look smaller
   void perspectiveDivide() {if (c[3] != 0.0f) {c[0] /= c[3]; c[1] /= c[3]; c[2] /= c[3];}}
   // @brief: Print the vector parameters
   void print() const {std::cout << "{" << c[0] << ", " << c[1] << ", " << c[2] << ", " << c[3] << "}" << std::endl;}


   // Operator overloads
   //---------------------------------------------------------------------------------------------
   //Subscript operator for compatability with opengl
   float& operator [] (int i) { return c.at(i);}
   float operator [] (int i) const { return c.at(i);}
   // Standard Operators 
   vec4 operator + (const vec4& v) const {return vec4(this->c[0] + v.c[0], this->c[1] + v.c[1], this->c[2] + v.c[2], this->c[3] + v.c[3]); } // Add 2 vectors
   vec4 operator - (const vec4& v) const {return vec4(this->c[0] - v.c[0], this->c[1] - v.c[1], this->c[2] - v.c[2], this->c[3] - v.c[3]); } // Subtract 2 vectors
   vec4 operator * (const float& f) const {return vec4(this->c[0] * f, this->c[1] * f, this->c[2] * f, this->c[3] * f); } // Scale vector by float
   vec4 operator / (const float& f) const {return vec4(this->c[0] / f, this->c[1] / f, this->c[2] / f, this->c[3] / f); } // Scale vector by float
   // Compound Operators
   void operator += (const vec4& v) { this->c[0] += v.c[0]; this->c[1] += v.c[1]; this->c[2] += v.c[2]; this->c[3] += v.c[3];} // Add 2 vectors
   void operator -= (const vec4& v) { this->c[0] -= v.c[0]; this->c[1] -= v.c[1]; this->c[2] -= v.c[2]; this->c[3] -= v.c[3]; } // Subtract 2 vectors
   void operator *= (const float& f) { this->c[0] *= f; this->c[1] *= f; this->c[2] *= f; this->c[3] *= f; } // Scale vector by float
   void operator /= (const float& f) { this->c[0] /= f; this->c[1] /= f; this->c[2] /= f; this->c[3] /= f; } // Scale vector by float
   // Dot product overloa
   vec4 operator * (const vec4& v) {return vec4(this->c[0] * v.c[0], this->c[1] * v.c[1], this->c[2] * v.c[2], this->c[3] * v.c[3]); }

   // Overload for multiplying a vector against a matrix
   vec4 operator * (const mat4x4& m) const {
      vec4 v;
      v.c[0] = this->c[0] * m.m[0][0] + this->c[1] * m.m[1][0] + this->c[2] * m.m[2][0] + this->c[3] * m.m[3][0];
      v.c[1] = this->c[0] * m.m[0][1] + this->c[1] * m.m[1][1] + this->c[2] * m.m[2][1] + this->c[3] * m.m[3][1];
      v.c[2] = this->c[0] * m.m[0][2] + this->c[1] * m.m[1][2] + this->c[2] * m.m[2][2] + this->c[3] * m.m[3][2];
      v.c[3] = this->c[0] * m.m[0][3] + this->c[1] * m.m[1][3] + this->c[2] * m.m[2][3] + this->c[3] * m.m[3][3];
      return v;
   }
   
   // Overload for multiplying a vector against a matrix
   void operator *= (const mat4x4& m) {
      vec4 v;
      v.c[0] = this->c[0] * m.m[0][0] + this->c[1] * m.m[1][0] + this->c[2] * m.m[2][0] + this->c[3] * m.m[3][0];
      v.c[1] = this->c[0] * m.m[0][1] + this->c[1] * m.m[1][1] + this->c[2] * m.m[2][1] + this->c[3] * m.m[3][1];
      v.c[2] = this->c[0] * m.m[0][2] + this->c[1] * m.m[1][2] + this->c[2] * m.m[2][2] + this->c[3] * m.m[3][2];
      v.c[3] = this->c[0] * m.m[0][3] + this->c[1] * m.m[1][3] + this->c[2] * m.m[2][3] + this->c[3] * m.m[3][3];
      this-> c[0] = v.c[0];
      this-> c[1] = v.c[1];
      this-> c[2] = v.c[2];
      this-> c[3] = v.c[3];
   }
};


/// @brief: Container for 3 3D vectors with functions for drawing to a 2D pixel array.
/// drawing is done with scanline filling and bresenham line function. This is intended for 
/// educational purposes so it is inefficient and lacks advanced features like z-buffers
/// @param _v0: Vector 0 (vec3)
/// @param _v1: Vector 1 (vec3)
/// @param _v2: Vector 2 (vec3)
/// @param parent: Pointer to a parent objec3d object
struct tri3d {

public:

   vec4 v[3];

   tri3d() : v{vec4(),vec4(),vec4()}{};
   tri3d(vec4 v0, vec4 v1, vec4 v2) : v{v0,v1,v2}{};

   // @brief: Generates a vector normal to the triangles face starting from the triangles 0 point
   vec4 normal() const {return ((this->v[0] - this->v[1]).cross(this->v[0] - this->v[2])).normal(); }
   // @brief: Devide by the w value (viewspace z value) after projection to give perspective, making far away objects look smaller
   void perspectiveDivide() {v[0].perspectiveDivide(); v[1].perspectiveDivide(); v[2].perspectiveDivide();}
   // @brief: Print the vector parameters
   void print() const {v[0].print(); v[1].print(); v[2].print();}

   // Operator overloads for multiplying a whole triagle by a matrix (just multiplies the underlying vectors)
   tri3d operator * (const mat4x4& m) const { return tri3d(this->v[0] * m, this->v[1] * m, this->v[2] * m); }
   void operator *= (const mat4x4& m) { this->v[0] *= m; this->v[1] *= m; this->v[2] *= m; }

private:

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
mat4x4 matrix_pointAt(vec3 pos, vec3 target, vec3 up);

/// @brief: Creates a matrix that will move a vertex in 3D space to a position that reflects its
/// position reletive to the camera view. Essentially moving the world around a camera instead 
/// of moving the camera. The camera view is represented by a matrix created with the "point_matrix"
/// the direction and position given to the "point_matrix" represents the camera
/// @param m: point_matrix result representing the camera (by reference)
/// @return mat4x4
///
mat4x4 matrix_view(mat4x4 m);
