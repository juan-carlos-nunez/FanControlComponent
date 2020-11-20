#include "gtest/gtest.h"
#include "TempToDutyCycle.h"

#include <unordered_map>

TEST(TempToDutyCycleUT, getDutyCycle)
{
   const std::unordered_map<float, float> expectedInOut
   {
        { std::numeric_limits<float>::min(), 20.00f }
      , { 0.00f ,  20.00f }
      , {20.00f ,  20.00f }
      , {24.09f ,  20.00f }
      , {25.00f ,  20.00f }
      , {34.27f ,  34.83f }
      , {31.80f ,  30.88f }
      , {43.00f ,  48.80f }
      , {52.38f ,  63.81f }
      , {63.78f ,  82.05f }
      , {71.94f ,  95.10f }
      , {75.00f , 100.00f }
      , {75.01f , 100.00f }
      , {99.99f , 100.00f }
      , { std::numeric_limits<float>::max(), 100.00f }
   };
   TempToDutyCycle ttdc;

   constexpr float EPSILON{ 0.01f };
   for( auto itr = expectedInOut.begin(); itr != expectedInOut.end(); ++itr )
   {
      auto rVal = ttdc.getDutyCycle(itr->first);
      auto tempDiff = fabs(rVal - itr->second);
      ASSERT_TRUE( tempDiff < EPSILON ) << "Temp=["<<itr->first <<"], expected=["<<itr->second <<"], rVal=["<< rVal <<"]" ;
   }
}