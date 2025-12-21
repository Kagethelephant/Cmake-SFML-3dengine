#include "utils.hpp"

#include <random>


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Random object Implementation
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

randObj::randObj(bool _persistant, int _seed){
   //!!!!! Creating new random engine is slow, only create one !!!!!!!!
   // If there was no seed provided then get one from hardware
   if (_persistant) {persistant = true;}
   if (_seed == -1) { 
      _seed = m_rd();
      seed = _seed;
   }
   // Create and seed the generator (Was using mt19937 but that was pretty overkill)
   m_gen.seed(seed); 
}


int randObj::iRand(int min, int max) {
   // Use the random number against a distribution range
   std::uniform_int_distribution<> distr(min, max);
   // If persistant is false then re-seed the random generator   
   if (!persistant){
      seed = m_rd();
      m_gen.seed(seed);
   }

   return distr(m_gen);
}


float randObj::fRand(float min, float max) {
   // Use the random number against a distribution range
   std::uniform_real_distribution<> distr(min, max);
   // If persistant is false then re-seed the random generator   
   if (!persistant){
      seed = m_rd();
      m_gen.seed(seed);
   }

   return distr(m_gen);
}


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Other Implementation
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

int wrap(int n, int max){
   if (n >= max) n -= max;
   return n;
}
