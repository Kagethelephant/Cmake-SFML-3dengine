
//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include <random>
#include "random.hpp"

//////////////////////////////////////////////////////////////////
int iRand(int min, int max, int seed) {

   // !!!! TRY FLAME GRAPH TO FIND CPU DRAGS !!!!!!!!!!!
   //!!!!! DO NOT CREATE A NEW ENGINE EVERYTIME !!!!!!!!
   if (seed == -1) { // If there was no seed provided then get one from hardware
      std::random_device rd; // Obtain a random number from hardware
      seed = rd();
   }
   std::minstd_rand gen(seed); // Create and seed the generator (Was using mt19937 but that is a little unecessary
   std::uniform_int_distribution<> distr(min, max); // Use the random number against a distribution range
   return distr(gen);
}

//////////////////////////////////////////////////////////////////
float fRand(float min, float max, int seed) {

   if (seed == -1) { // If there was no seed provided then get one from hardware
      std::random_device rd; // Obtain a random number from hardware
      seed = rd();
   }
   std::minstd_rand gen(seed); // Create and seed the generator (Was using mt19937 but that is a little unecessary
   std::uniform_real_distribution<> distr(min, max); // Use the random number against a distribution range
   return distr(gen);
}

