#include "math.hpp"



//////////////////////////////////////////////////////////////////
int randRange(int min, int max)
{
	// obtain a random number from hardware
	std::random_device rd;
	// seed the generator
	std::mt19937 gen(rd());
	// define the range
	std::uniform_int_distribution<> distr(min, max);

	return distr(gen);
}

