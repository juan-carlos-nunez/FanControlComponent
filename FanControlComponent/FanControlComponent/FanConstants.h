/*
* File: FanConstants
*
* Description: Constant values related to the FANs - their register
*     address and the proportionalit constant.
*
*/

#pragma once

#include <unordered_map>

namespace FanConstants
{
   const int INITIAL_FAN_DUTY_CYCLE{ 25 };

   // Fan ID, Register Address
   const std::unordered_map<int, uint64_t> FAN_REGISTER_ADDRESSES
   {
        { 1, 0xB200'FB38 }
      , { 2, 0xB200'FB3C }
      , { 3, 0xB200'FB40 }
      , { 4, 0xB200'FB44 }
      , { 5, 0xB200'FB48 }
      , { 6, 0xB200'FB4C }
      , { 7, 0xB200'FB50 }
      , { 8, 0xB200'FB54 }
      , { 9, 0xB200'FB58 }
      , {10, 0xB200'FB5C }
      , {11, 0xB200'FB60 }
      , {12, 0xB200'FB64 }
      , {13, 0xB200'FB68 }
      , {14, 0xB200'FB6C }
      , {15, 0xB200'FB70 }
      , {16, 0xB200'FB74 }
      , {17, 0xB200'FB78 }
      , {18, 0xB200'FB7C }
      , {19, 0xB200'FB80 }
      , {20, 0xB200'FB84 }
      , {21, 0xB200'FB88 }
   };

   // Fan ID, Proportionality Const.
   const std::unordered_map<int, int> FAN_PWMC_PROPORTIONALITY
   {
        { 1, 5 }
      , { 2, 2 }
      , { 3, 2 }
      , { 4, 6 }
      , { 5, 6 }
      , { 6, 8 }
      , { 7, 8 }
      , { 8, 3 }
      , { 9, 3 }
      , {10, 7 }
      , {11, 4 }
      , {12, 2 }
      , {13, 2 }
      , {14, 6 }
      , {15, 6 }
      , {16, 7 }
      , {17, 7 }
      , {18, 4 }
      , {19, 4 }
      , {20, 6 }
      , {21, 2 }
   };
};