#pragma once

//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include <random>
#include <string>



//////////////////////////////////////////////////////////////////
/// \brief Simple 4x4 matrix, good for 3D rendering
//////////////////////////////////////////////////////////////////
struct mat4x4 
{
    float m[4][4] = {0.0f};

    mat4x4 operator * (const mat4x4 &m2)
	{
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
struct vec2
{
    float x,y;

    // Intitialization options
    vec2() : x(0), y(0) {}
    vec2(float _x, float _y) : x(_x), y(_y) {}
    vec2(const vec2& v) : x(v.x), y(v.y) {}
    
    // Non overload functions
    float mag() { return std::sqrt(std::pow(x,2) + std::pow(y,2)); }
    vec2 norm() { float m = mag(); return vec2(x /= m, y /= m); }
    float dot(const vec2& v) { return ((this->x * v.x) + (this->y * v.y)); }

    // Overload functions
    vec2 operator + (const vec2& v) {return vec2(this->x + v.x, this->y + v.y); }
    vec2 operator - (const vec2& v) {return vec2(this->x - v.x, this->y - v.y); }
    vec2 operator * (const float& f) {return vec2(this->x * f, this->y * f); }
    vec2 operator / (const float& f) {return vec2(this->x / f, this->y / f); }
};


//////////////////////////////////////////////////////////////////
/// \brief 3d vector with an optional constructor
//////////////////////////////////////////////////////////////////
struct vec3
{
    float x,y,z,w;

    // Intitialization options
    vec3() : x(0), y(0), z(0), w(1) {}
    vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z), w(1) {}
    vec3(const vec3& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
    
    // Non overload functions
    float mag() { return std::sqrt(std::pow(x,2) + std::pow(y,2) + std::pow(z,2)); }
    vec3 norm() { float m = mag(); return vec3(x /= m, y /= m, z /= m); }
    float dot(const vec3& v) { return ((this->x * v.x) + (this->y * v.y) + (this->z * v.z)); }
    vec3 cross(const vec3& v) { return vec3(this->y * v.z - this->z * v.y, this->z * v.x - this->x * v.z, this->x * v.y - this->y * v.x); }

    // Overload functions
    vec3 operator + (const vec3& v) {return vec3(this->x + v.x, this->y + v.y, this->z + v.z); }
    vec3 operator - (const vec3& v) {return vec3(this->x - v.x, this->y - v.y, this->z - v.z); }
    vec3 operator * (const float& f) {return vec3(this->x * f, this->y * f, this->z * f); }
    vec3 operator / (const float& f) {return vec3(this->x / f, this->y / f, this->z / f); }

    // Multiple object overloads
    vec3 operator * (const mat4x4& m) 
    {
        vec3 v;
        v.x = this->x * m.m[0][0] + this->y * m.m[1][0] + this->z * m.m[2][0] + m.m[3][0];
        v.y = this->x * m.m[0][1] + this->y * m.m[1][1] + this->z * m.m[2][1] + m.m[3][1];
        v.z = this->x * m.m[0][2] + this->y * m.m[1][2] + this->z * m.m[2][2] + m.m[3][2];
        float w = (this->x * m.m[0][3] + this->y * m.m[1][3] + this->z * m.m[2][3] + m.m[3][3]);
        if ( w != 0.0f) {v.x /= w; v.y /= w; v.z /= w;}
        return v;
    }
};

vec3 mat_multiply (vec3& v, mat4x4& m);

mat4x4 identity_matrix();

mat4x4 rotate_x_matrix(float u);

mat4x4 rotate_y_matrix(float v);

mat4x4 rotate_z_matrix(float w);

mat4x4 translate_matrix(float x, float y, float z);

mat4x4 project_matrix(float fFovDegrees, float fAspectRatio, float fNear, float fFar);

mat4x4 point_matrix(vec3 &pos, vec3 &target, vec3 &up);

mat4x4 view_matrix(mat4x4 &m); // Only for Rotation/Translation Matrices



//////////////////////////////////////////////////////////////////
/// \brief Gets a random integer between the 2 given integers
//////////////////////////////////////////////////////////////////
int randRange(int min, int max);




//////////////////////////////////////////////////////////////////
/// \brief Use a look up table to calculate the sin faster
//////////////////////////////////////////////////////////////////
float math_sin(float theta);

//////////////////////////////////////////////////////////////////
/// \brief Calculate the cosine based on the sine funcion minus 1/2 pi radians
//////////////////////////////////////////////////////////////////
float math_cos(float theta);

//////////////////////////////////////////////////////////////////
/// \brief Calculate the tanget based on the sine and cosine functions
//////////////////////////////////////////////////////////////////
float math_tan(float theta);