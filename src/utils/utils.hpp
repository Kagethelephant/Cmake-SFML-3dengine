#pragma once

#include <random>

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Random number generator
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

/// @brief: Object to return random numbers persistantly or not
/// @param persistant: If true, the same sequence of numbers will be generated
/// the same each time the program is run, given the seed is the same (default: false)
/// @param seed: Random seed as an integar (default: Random seed from hardware)
class randObj {

public:
   /// @brief: Random seed
   int seed;

   randObj(bool _persistant = false, int _seed = -1);
   
   /// @brief: Gets a random float between the 2 given floats
   /// @param min: Inclusive minimum of random range
   /// @param max: Inclusive maximum of random range
   float fRand(float min, float max);

   /// @brief: Gets a random integer between the 2 given integers
   /// @param min: Inclusive minimum of random range
   /// @param max: Inclusive maximum of random range
   int iRand(int min, int max);

private:
   
   /// @brief: Determines if the same sequence of numbers will be generated
   bool persistant = false;
   /// @brief: Source for machine entropy
   std::random_device m_rd;
   /// @brief: Random number engine
   std::minstd_rand m_gen;
};


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Other functions
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

///@brief: Wraps an integar with an uninclusive max limit
///@param n: Value to check against limit
///@param max: Uninclusive max
int wrap(int n, int max);


