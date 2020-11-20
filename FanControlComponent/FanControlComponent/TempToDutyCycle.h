/*
* Struct: TempToDutyCycle
* 
* Description: Provides the mechanism for caulculating the duty cycle
*     for the given temp, linearly interpolated between DC_MIN and DC_MAX.
*     Temps at or below TEMP_MIN are given a duty cycle of DC_MIN. Temps
*     at or above TEMP_MAX are given a duty cycle of DC_MAX.
* 
*/

#pragma once

struct TempToDutyCycle
{
   static constexpr float DC_MAX{ 100 };
   static constexpr float DC_MIN{ 20 };

   static constexpr float TEMP_MAX{ 75.0 };
   static constexpr float TEMP_MIN{ 25.0 };

   static constexpr float DIVISOR{ (TEMP_MAX - TEMP_MIN) };

   //
   // Name: getDutyCycle
   //
   // Param(s): float temp - Temperature.
   //
   // Description: Calculates and returns the duty cycle for the provided temp.
   //
   inline static float getDutyCycle(float temp)
   {
      float rVal{ 0.0 };

      if (temp <= TEMP_MIN)
      {
         rVal = DC_MIN;
      }
      else if (temp >= TEMP_MAX)
      {
         rVal = DC_MAX;
      }
      else
      {
         const auto exp1 = (DC_MIN * (TEMP_MAX - temp));
         const auto exp2 = (DC_MAX * (temp - TEMP_MIN));
         rVal = (exp1 + exp2) / DIVISOR;
      }

      return rVal;
   }
};