#pragma once

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Headers
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#include <random>

/// @brief Object to return random numbers persistantly or not
class randObj {

public:

   int seed;
   bool persistant = false;

   randObj(int _seed = -1, bool _persistant = false);
   
   /// @brief: Gets a random float between the 2 given floats
   float fRand(float min, float max);

   /// @brief: Gets a random integer between the 2 given integers
   int iRand(int min, int max);

private:

   std::random_device m_rd;
   std::minstd_rand m_gen;
};
