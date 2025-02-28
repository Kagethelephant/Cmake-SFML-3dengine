
//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include "random.hpp"


randObj::randObj(int _seed){
   // !!!! TRY FLAME GRAPH TO FIND CPU DRAGS !!!!!!!!!!!
   //!!!!! DO NOT CREATE A NEW ENGINE EVERYTIME !!!!!!!!
   if (_seed == -1) { // If there was no seed provided then get one from hardware
      // std::random_device rd; // Obtain a random number from hardware
      seed = rd();
   }
   else {seed = _seed;}
   gen.seed(seed); // Create and seed the generator (Was using mt19937 but that is a little unecessary
}
//////////////////////////////////////////////////////////////////
int randObj::iRand(int min, int max) {

   std::uniform_int_distribution<> distr(min, max); // Use the random number against a distribution range
   return distr(gen);
}

//////////////////////////////////////////////////////////////////
float randObj::fRand(float min, float max) {

   std::uniform_real_distribution<> distr(min, max); // Use the random number against a distribution range
   return distr(gen);
}




//////////////////////////////////////////////////////////////////
int iRand(int min, int max) {

   std::random_device rd; // Obtain a random number from hardware
   std::minstd_rand gen(rd()); // Create and seed the generator (Was using mt19937 but that is a little unecessary
   std::uniform_int_distribution<> distr(min, max); // Use the random number against a distribution range
   return distr(gen);
}

//////////////////////////////////////////////////////////////////
float fRand(float min, float max) {

   std::random_device rd; // Obtain a random number from hardware
   std::minstd_rand gen(rd()); // Create and seed the generator (Was using mt19937 but that is a little unecessary
   std::uniform_real_distribution<> distr(min, max); // Use the random number against a distribution range
   return distr(gen);
}
