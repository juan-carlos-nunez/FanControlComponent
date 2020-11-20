#include "gtest/gtest.h"
#include "FanControl.h"
#include "FanConstants.h"

TEST(FanControlUT, ctor)
{
   std::vector<int> subSystemIds{ 1, 2, 3, 4,  5,  6,  7,  8,  9, 10 };
   std::vector<int> fanIds{ 8, 6, 2, 4, 20, 18, 14, 16, 10, 12 };
   uint32_t         mockRegisters[10]{ 0 };

   std::unordered_map<int, uint64_t> FanIdMemAddresses;

   for (int x{ 0 }; x < 10; ++x)
   {
      FanIdMemAddresses[fanIds[x]] = reinterpret_cast<uint64_t>(&(mockRegisters[x]));
   }

   ASSERT_NO_THROW(
   { 
      FanControl fanCntrl(subSystemIds, fanIds, FanIdMemAddresses);
   });
}

TEST(FanControlUT, initialize)
{
   std::vector<int> subSystemIds{ 1, 2, 3, 4,  5,  6,  7,  8,  9, 10 };
   std::vector<int> fanIds{ 8, 6, 2, 4, 20, 18, 14, 16, 10, 12 };
   uint32_t         mockRegisters[10]{ 0 };

   std::unordered_map<int, uint64_t> FanIdMemAddresses;

   for (int x{ 0 }; x < 10; ++x)
   {
      FanIdMemAddresses[fanIds[x]] = reinterpret_cast<uint64_t>(&(mockRegisters[x]));
   }

   ASSERT_NO_THROW(
   {
      FanControl fanCntrl(subSystemIds, fanIds, FanIdMemAddresses);
      ASSERT_EQ(GeneralConstants::ReturnCodes::SUCCESS, fanCntrl.initialize());
   });
}

TEST(FanControlUT, NewMaxTemp)
{
   std::vector<int> subSystemIds{ 1, 2, 3, 4,  5,  6,  7,  8,  9, 10 };
   std::vector<int> fanIds{ 8, 6, 2, 4, 20, 18, 14, 16, 10, 12 };
   uint32_t         mockRegisters[10]{ 0 };

   std::unordered_map<int, uint64_t> FanIdMemAddresses;

   for (int x{ 0 }; x < 10; ++x)
   {
      FanIdMemAddresses[fanIds[x]] = reinterpret_cast<uint64_t>(&(mockRegisters[x]));
   }

   FanControl fanCntrl(subSystemIds, fanIds, FanIdMemAddresses);
   ASSERT_EQ(GeneralConstants::ReturnCodes::SUCCESS, fanCntrl.initialize());

   float temp{ 71.94 };
   float dc{ 95.10 };
   int idx{ 5 };
   auto multiplier{ FanConstants::FAN_PWMC_PROPORTIONALITY.find(fanIds[idx])->second };

   auto roundedDc{ static_cast<int>(std::round(dc)) };

   auto rVal( roundedDc * multiplier);

   fanCntrl.notifyNewMaxTemp( temp );
   
   std::this_thread::sleep_for(std::chrono::milliseconds(10));
   auto rVal2{ mockRegisters[idx] };
   ASSERT_EQ( rVal, rVal2 ) << "rVal=[" << rVal << "], rVal2=[" << rVal2 << "]";

}