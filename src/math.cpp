#include "math.hpp"



//////////////////////////////////////////////////////////////////
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

