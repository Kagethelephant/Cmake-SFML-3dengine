
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Headers
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#include "random.hpp"


randObj::randObj(int seed, bool _persistant){
   //!!!!! Creating new random engine is slow, only create one !!!!!!!!
   // If there was no seed provided then get one from hardware
   if (_persistant) {persistant = true;}
   if (seed == -1) { 
      seed = m_rd();
   }
   // Create and seed the generator (Was using mt19937 but that was pretty overkill)
   m_gen.seed(seed); 
}


int randObj::iRand(int min, int max) {
   // Use the random number against a distribution range
   std::uniform_int_distribution<> distr(min, max);
   
   if (!persistant){
      int newSeed = m_rd();
      seed = newSeed;
      m_gen.seed(seed);
   }

   return distr(m_gen);
}


float randObj::fRand(float min, float max) {
   // Use the random number against a distribution range
   std::uniform_real_distribution<> distr(min, max);

   if (!persistant){
      int newSeed = m_rd();
      seed = newSeed;
      m_gen.seed(seed);
   }

   return distr(m_gen);
}
