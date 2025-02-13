#include "math.hpp"


//////////////////////////////////////////////////////////////////
int randRange(int min, int max)
{
	// Obtain a random number from hardware
	std::random_device rd;
	// Seed the generator
	std::mt19937 gen(rd());
	// Define the range
	std::uniform_int_distribution<> distr(min, max);

	return distr(gen);
}


mat4x4 identity_matrix()
{
	mat4x4 m;
	m.m[0][0] = 1.0f; m.m[1][0] = 0.0f; m.m[2][0] = 0.0f; m.m[3][0] = 0.0f;
	m.m[0][1] = 0.0f; m.m[1][1] = 1.0f; m.m[2][1] = 0.0f; m.m[3][1] = 0.0f;
	m.m[0][2] = 0.0f; m.m[1][2] = 0.0f; m.m[2][2] = 1.0f; m.m[3][2] = 0.0f;
	m.m[0][3] = 0.0f; m.m[1][3] = 0.0f; m.m[2][3] = 0.0f; m.m[3][3] = 1.0f;	
	return m;
}

mat4x4 rotate_x_matrix(float u)
{
	mat4x4 m;
	m.m[0][0] = 1.0f; m.m[1][0] = 0.0f;        m.m[2][0] = 0.0f;         m.m[3][0] = 0.0f;
	m.m[0][1] = 0.0f; m.m[1][1] = math_cos(u); m.m[2][1] = -math_sin(u); m.m[3][1] = 0.0f;
	m.m[0][2] = 0.0f; m.m[1][2] = math_sin(u); m.m[2][2] = math_cos(u);  m.m[3][2] = 0.0f;
	m.m[0][3] = 0.0f; m.m[1][3] = 0.0f;        m.m[2][3] = 0.0f;         m.m[3][3] = 1.0f;
	return m;
}

mat4x4 rotate_y_matrix(float v)
{
	mat4x4 m;
	m.m[0][0] = math_cos(v); m.m[1][0] = 0.0f; m.m[2][0] = -math_sin(v); m.m[3][0] = 0.0f;
	m.m[0][1] = 0.0f;        m.m[1][1] = 1.0f; m.m[2][1] = 0.0f;         m.m[3][1] = 0.0f;
	m.m[0][2] = math_sin(v); m.m[1][2] = 0.0f; m.m[2][2] = math_cos(v);  m.m[3][2] = 0.0f;
	m.m[0][3] = 0.0f;        m.m[1][3] = 0.0f; m.m[2][3] = 0.0f;         m.m[3][3] = 1.0f;
	return m;
}

mat4x4 rotate_z_matrix(float w)
{
	mat4x4 m;
	m.m[0][0] = math_cos(w); m.m[1][0] = -math_sin(w); m.m[2][0] = 0.0f; m.m[3][0] = 0.0f;
	m.m[0][1] = math_sin(w); m.m[1][1] = math_cos(w);  m.m[2][1] = 0.0f; m.m[3][1] = 0.0f;
	m.m[0][2] = 0.0f;        m.m[1][2] = 0.0f;         m.m[2][2] = 1.0f; m.m[3][2] = 0.0f;
	m.m[0][3] = 0.0f;        m.m[1][3] = 0.0f;         m.m[2][3] = 0.0f; m.m[3][3] = 1.0f;
	return m;
}

mat4x4 translate_matrix(float x, float y, float z)
{
	mat4x4 m;
	m.m[0][0] = 1.0f; m.m[1][0] = 0.0f; m.m[2][0] = 0.0f; m.m[3][0] = x;
	m.m[0][1] = 0.0f; m.m[1][1] = 1.0f; m.m[2][1] = 0.0f; m.m[3][1] = y;
	m.m[0][2] = 0.0f; m.m[1][2] = 0.0f; m.m[2][2] = 1.0f; m.m[3][2] = z;
	m.m[0][3] = 0.0f; m.m[1][3] = 0.0f; m.m[2][3] = 0.0f; m.m[3][3] = 1.0f;
	return m;
}

mat4x4 project_matrix(float fFovDegrees, float fAspectRatio, float fNear, float fFar)
{
	
	float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);
	float fDisRatio = fFar / (fFar - fNear);
	mat4x4 m;
	m.m[0][0] = fAspectRatio * fFovRad; m.m[1][0] = 0.0f;    m.m[2][0] = 0.0f;      m.m[3][0] = 0.0f;
	m.m[0][1] = 0.0f;                   m.m[1][1] = fFovRad; m.m[2][1] = 0.0f;      m.m[3][1] = 0.0f;
	m.m[0][2] = 0.0f;                   m.m[1][2] = 0.0f;    m.m[2][2] = fDisRatio; m.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	m.m[0][3] = 0.0f;                   m.m[1][3] = 0.0f;    m.m[2][3] = 1.0f;      m.m[3][3] = 0.0f;
	return m;
}

mat4x4 point_matrix(vec3 &pos, vec3 &target, vec3 &up)
{
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

mat4x4 view_matrix(mat4x4 &m) // Only for Rotation/Translation Matrices
{
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

vec3 mat_multiply(vec3& v, mat4x4& m)
{
    vec3 v2;
    v2.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + v.w * m.m[3][0];
    v2.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + v.w * m.m[3][1];
    v2.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + v.w * m.m[3][2];
    v2.w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + v.w * m.m[3][3];
    return v2;
}




//////////////////////////////////////////////////////////////////
/// \brief Array containing the the lookup values for sin
/// This accepts 1/128th of a pi Radian
//////////////////////////////////////////////////////////////////
uint16_t SIN[129] =
{
	0, 1571, 3140, 4708, 6273, 7834, 9391, 10942, 12486, 14022, 15551,
	17070, 18578, 20076, 21561, 23033, 24492, 25935, 27364, 28775, 30169,
	31545, 32903, 34240, 35556, 36852, 38125, 39375, 40601, 41803, 42980,
	44131, 45255, 46352, 47421, 48461, 49473, 50454, 51405, 52325, 53214,
	54071, 54895, 55686, 56443, 57166, 57855, 58509, 59128, 59712, 60259, 
	60770, 61244, 61682, 62082, 62445, 62770, 63058, 63307, 63519, 63692, 
	63827, 63923, 63981, 64000, 63981, 63923, 63827, 63692, 63519, 63307, 
	63058, 62770, 62445, 62082, 61682, 61244, 60770, 60259, 59712, 59128, 
	58509, 57855, 57166, 56443, 55686, 54895, 54071, 53214, 52325, 51405, 
	50454, 49473, 48461, 47421, 46352, 45255, 44131, 42980, 41803, 40601, 
	39375, 38125, 36852, 35556, 34240, 32903, 31545, 30169, 28775, 27364, 
	25935, 24492, 23033, 21561, 20076, 18578, 17070, 15551, 14022, 12486, 
	10942, 9391, 7834, 6273, 4708, 3140, 1571, 0
};


//////////////////////////////////////////////////////////////////
float math_sin(float theta)
{
	float sign = 1.0f;

	// Turn the radians into pi-radians
	theta /= 3.141592653;
	// Get the amount of whole 2 pi radians off 
	int half = (int)abs(theta/2);

	// If it is more than 2 pi radians or les then zero, wrap it
	if (theta < 0) theta += half*2+2;
	else if (theta > 2) theta -= half*2;

	// If it is the second half of the sine curve make it negative
	if (theta >= 1) { theta -= 1; sign = -1.0f;}
	theta *= 128;

	int deg = (int)theta;
	float extrp = theta - (float)deg;
	float va = sign*SIN[deg]/64000;
	float vb = sign*SIN[deg + 1]/64000;

	float result = va + (vb-va)*extrp;

    return result;
}

//////////////////////////////////////////////////////////////////
float math_cos(float theta)
{
	theta += .5*3.141592653;
    return math_sin(theta);
}

//////////////////////////////////////////////////////////////////
float math_tan(float theta)
{
	float result = math_sin(theta)/math_cos(theta);
    return result;
}


