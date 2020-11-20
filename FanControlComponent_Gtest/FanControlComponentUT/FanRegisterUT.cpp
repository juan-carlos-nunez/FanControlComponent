#include "gtest/gtest.h"
#include "FanRegisters.h"

TEST(FanRegistersUT, ctor)
{
   std::vector<int> fanIds{ 8, 6, 2, 4, 20, 18, 14, 16, 10, 12 };
   uint32_t         mockRegisters[10]{ 0 };

   std::unordered_map<int, uint64_t> FanIdMemAddresses;

   for (int x{ 0 }; x < 10; ++x)
   {
      FanIdMemAddresses[fanIds[x]] = reinterpret_cast<uint64_t>(&(mockRegisters[x]));
   }
   
   ASSERT_NO_THROW (
   { 
      FanRegisters fr{ FanIdMemAddresses };
   } );
}

TEST(FanRegistersUT, ReadWriteClear)
{
   std::vector<int> fanIds{ 8, 6, 2, 4, 20, 18, 14, 16, 10, 12 };
   uint32_t         mockRegisters[10]{ 0 };

   std::unordered_map<int, uint64_t> FanIdMemAddresses;

   for (int x{ 0 }; x < 10; ++x)
   {
      FanIdMemAddresses[fanIds[x]] = reinterpret_cast<uint64_t>(&(mockRegisters[x]));
   }

   FanRegisters fr{ FanIdMemAddresses };

   for (int x{ 0 }; x < 10; ++x)
   {
      auto writeValue{ x + 10 };
      
      fr.writeRegister(fanIds[x], writeValue );
      ASSERT_EQ(writeValue, mockRegisters[x]);

      ASSERT_EQ(writeValue, fr.readRegister( fanIds[x] ) );
      
      fr.clearRegister(fanIds[x]);
      ASSERT_EQ(0, mockRegisters[x]);
      ASSERT_EQ(0, fr.readRegister(fanIds[x]));
   }
}