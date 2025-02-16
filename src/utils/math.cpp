
#include <random>
#include "math.hpp"

//////////////////////////////////////////////////////////////////
int iRand(int min, int max, int seed)
{
    if (seed == -1) { // If there was no seed provided then get one from hardware
        std::random_device rd; // Obtain a random number from hardware
        seed = rd();
    }
	std::minstd_rand gen(seed); // Create and seed the generator (Was using mt19937 but that is a little unecessary
	std::uniform_int_distribution<> distr(min, max); // Use the random number against a distribution range
	return distr(gen);
}

//////////////////////////////////////////////////////////////////
float fRand(float min, float max, int seed)
{
    if (seed == -1) { // If there was no seed provided then get one from hardware
        std::random_device rd; // Obtain a random number from hardware
        seed = rd();
    }
	std::minstd_rand gen(seed); // Create and seed the generator (Was using mt19937 but that is a little unecessary
	std::uniform_real_distribution<> distr(min, max); // Use the random number against a distribution range
	return distr(gen);
}


int absi(int val) {
    if (val < 0) { val *= -1; }
    return val;
}

double absf(double val) {
    if (val < 0) { val *= -1.0; }
    return val;
}

double power(double val, int exp) {
    double placholder = val;
    for(int i = 1; i <= (exp -1); i += 1) {
        val *= placholder;
    }

    if (exp == 0) { val = 1; }

    return val;
}

double sqroot(double target) {
    double temp = 1;
    double rt = 2;
    // int timeout = 0;
    bool found = 0;
    bool max = false;

    if (target <= 0) { return -1;}

    while (found == 0) {
        double dif = (target - power(temp,2)); 
        if(dif > 0){ temp += rt; }
        else if (!max) {
            max = true;
            rt /= 4;
        }
        else {temp -= rt;}
        if(!max) { rt*=2; }
        else {rt /=2; }

        if ( absf(dif) < 0.000001) { return temp;} 

        // timeout += 1;
        // if (timeout >= 100) {
        //     found = 1;
        //     temp = timeout;
        // }
    }
    return -1;
}


//////////////////////////////////////////////////////////////////
/// \brief Array containing the the lookup values for sin
/// This accepts 1/128th of a pi Radian
//////////////////////////////////////////////////////////////////
uint16_t SIN[129]
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
	// Turn the radians into pi-radians (so 1 is the entire range of the sine chart)
	theta /= 3.141592653;
	// If it is more than 2 pi radians or less then zero, wrap it 
    // We need to divide this by 2 to cast the int so we capture all wrapping scenerios.
	if (theta < 0) theta += (int)fabs(theta/2)*2+2;
	else if (theta > 2) theta -= (int)fabs(theta/2)*2;
	// If it is the second half of the sine curve make it negative
	float sign = 1.0f;
	if (theta >= 1) { theta -= 1; sign = -1.0f;}
    // Find the value of the array positions before and after, find its relitive 
    // distance from the array position before and after it
	theta *= 128;
	int pos = (int)theta;
	float extrp = theta - (float)pos;
	float va = sign*SIN[pos]/64000;
	float vb = sign*SIN[pos + 1]/64000;
    // Find the extrapolated value of the input
    return va + (vb-va)*extrp;
}

//////////////////////////////////////////////////////////////////
float math_cos(float theta)
{   // Cosine is just the sine offset by 1/2pi radians
	theta += .5*3.141592653;
    return math_sin(theta);
}

//////////////////////////////////////////////////////////////////
float math_tan(float theta)
{   // Tan is just sine / cosine
	float result = math_sin(theta)/math_cos(theta);
    return result;
}

// //////////////////////////////////////////////////////////////////
// float math_asin(float theta)
// {
// 	theta += .5*3.141592653;
//     return math_sin(theta);
// }

// //////////////////////////////////////////////////////////////////
// float math_acos(float theta)
// {
// 	theta += .5*3.141592653;
//     return math_sin(theta);
// }

// //////////////////////////////////////////////////////////////////
// float math_atan(float theta)
// {
// 	float result = math_sin(theta)/math_cos(theta);
//     return result;
// }
