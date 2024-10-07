#include "math.hpp"

vector3 crossProd(vector3 v1, vector3 v2)
{
	vector3 vout;
	vout.x = v1.y * v2.z - v1.z *v2.y;
    vout.y = v1.z * v2.x - v1.x *v2.z;
    vout.z = v1.x * v2.y - v1.y *v2.x;
    return vout;
}

//////////////////////////////////////////////////////////////////
float dotProd(vector3 v1, vector3 v2, vector3 v3)
{
    return ((v1.x * (v2.x - v3.x)) + (v1.y * (v2.y - v3.y)) + (v1.z * (v2.z - v3.z)));
}


//////////////////////////////////////////////////////////////////
vector3 normalize(vector3 vin)
{
    vector3 vout = vin;

    float l = std::sqrtf(vin.x*vin.x + vin.y*vin.y + vin.z*vin.z);
    vout.x /= l; vout.y /= l; vout.z /= l;

    return vout;
}


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
float mat_sin(float theta)
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
float mat_cos(float theta)
{
	theta += .5*3.141592653;
    return mat_sin(theta);
}

//////////////////////////////////////////////////////////////////
float mat_tan(float theta)
{
	float result = mat_sin(theta)/mat_cos(theta);
    return result;
}


