#include "gtest/gtest.h"
#include "TempMonitor.h"
#include "SubSystem.h"

namespace
{
   class GenericListener final : public TempMonitorListener
   {
      float curTemp{ 0 };
   public:
      GenericListener(){};
      void notifyNewMaxTemp(float temp)
      { 
         curTemp = temp;
      };

      float getCurTemp()
      {
         return curTemp;
      };
   };
};

TEST(TempMonitorUT, ctor)
{
   const std::vector<int> ssIds{1,2,3,4,5,6,7,8,9,10};

   ASSERT_NO_THROW(
      { 
         TempMonitor tm{ ssIds };
      } );

}

TEST(TempMonitorUT, initialize)
{
   ASSERT_NO_THROW(
   {
      const std::vector<int> ssIds{ 1 };
      TempMonitor tm{ ssIds };
      SubSystem subSys(ssIds[0], true);
      subSys.initialize();
      ASSERT_EQ(GeneralConstants::ReturnCodes::SUCCESS, tm.initialize() );
   });
}

TEST(TempMonitorUT, RegUnRegListener)
{
   const std::vector<int> ssIds{ 1,2,3,4,5,6,7,8,9,10 };
   TempMonitor tm{ ssIds };

   GenericListener gl;
   ASSERT_EQ(GeneralConstants::ReturnCodes::SUCCESS, tm.registerListener(gl)   );
   ASSERT_EQ(GeneralConstants::ReturnCodes::SUCCESS, tm.unregisterListener(gl) );
}

TEST(TempMonitorUT, SendReceiveTemp)
{
   const std::vector<int> ssIds{ 1 };
   TempMonitor tm{ ssIds };
   SubSystem subSys(ssIds[0], true);
   subSys.initialize();

   ASSERT_EQ(GeneralConstants::ReturnCodes::SUCCESS, tm.initialize());
   
   GenericListener gl;
   ASSERT_EQ(GeneralConstants::ReturnCodes::SUCCESS, tm.registerListener(gl));
   
   auto temp{37.48f};
   subSys.setUnitTestTemp(temp);

   tm.start();
   std::this_thread::sleep_for(std::chrono::milliseconds(100));
   tm.stop();
   auto glTemp = gl.getCurTemp();
   ASSERT_EQ(temp, glTemp ) << "Temp[" << temp << "] != [" << glTemp << "]";

   ASSERT_EQ(GeneralConstants::ReturnCodes::SUCCESS, tm.unregisterListener(gl));
}

TEST(TempMonitorUT, CheckMaxTempNotification)
{
   const std::vector<int> ssIds{ 1,2,3,4,5 };
   TempMonitor tm{ ssIds };

   SubSystem subSys1( ssIds[0], true );
   SubSystem subSys2( ssIds[1], true );
   SubSystem subSys3( ssIds[2], true );
   SubSystem subSys4( ssIds[3], true );
   SubSystem subSys5( ssIds[4], true );

   subSys1.initialize();
   subSys2.initialize();
   subSys3.initialize();
   subSys4.initialize();
   subSys5.initialize();
   
   ASSERT_EQ(GeneralConstants::ReturnCodes::SUCCESS, tm.initialize());

   GenericListener gl;
   ASSERT_EQ(GeneralConstants::ReturnCodes::SUCCESS, tm.registerListener(gl));

   const int waitForMs{200};

   // Max is testTemp
   auto testTemp{ 37.48f };
   subSys1.setUnitTestTemp(testTemp);
   tm.start();
   std::this_thread::sleep_for(std::chrono::milliseconds(waitForMs));
   tm.stop();
   {
      auto glTemp = gl.getCurTemp();
      ASSERT_EQ(testTemp, glTemp);
   }

   // Max is testTemp
   auto testTemp2{ 37.00f };
   subSys2.setUnitTestTemp(testTemp2);
   tm.start();
   std::this_thread::sleep_for(std::chrono::milliseconds(waitForMs));
   tm.stop();
   {
      auto glTemp = gl.getCurTemp();
      ASSERT_EQ(testTemp, glTemp);
   }

   // Max is testTemp
   subSys3.setUnitTestTemp(testTemp2);
   tm.start();
   std::this_thread::sleep_for(std::chrono::milliseconds(waitForMs));
   tm.stop();
   {
      auto glTemp = gl.getCurTemp();
      ASSERT_EQ(testTemp, glTemp);
   }

   // Max is testTemp
   subSys4.setUnitTestTemp(testTemp2);
   tm.start();
   std::this_thread::sleep_for(std::chrono::milliseconds(waitForMs));
   tm.stop();
   {
      auto glTemp = gl.getCurTemp();
      ASSERT_EQ(testTemp, glTemp);
   }

   // Max is testTemp3: Overwrite ssid 2 with testTemp3
   auto testTemp3{ 40.00f };
   subSys2.setUnitTestTemp(testTemp3);
   tm.start();
   std::this_thread::sleep_for(std::chrono::milliseconds(waitForMs));
   tm.stop();
   {
      auto glTemp = gl.getCurTemp();
      ASSERT_EQ(testTemp3, glTemp);
   }

   // Max is testTemp again: Overwrite ssid 2 with testTemp2
   subSys2.setUnitTestTemp(testTemp2);
   tm.start();
   std::this_thread::sleep_for(std::chrono::milliseconds(waitForMs));
   tm.stop();
   {
      auto glTemp = gl.getCurTemp();
      ASSERT_EQ(testTemp, glTemp);
   }

   // New Max is testTemp4: new ssid with highest temp.
   auto testTemp4{ 75.00f };
   subSys5.setUnitTestTemp(testTemp4);
   tm.start();
   std::this_thread::sleep_for(std::chrono::milliseconds(waitForMs));
   tm.stop();
   {
      auto glTemp = gl.getCurTemp();
      ASSERT_EQ(testTemp4, glTemp);
   }

   ASSERT_EQ(GeneralConstants::ReturnCodes::SUCCESS, tm.unregisterListener(gl));
}