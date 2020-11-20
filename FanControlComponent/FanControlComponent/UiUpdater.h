/*
* Class: UiUpdater
*
* Description:  Pure Virtual interface class, providing a callback
*     to the UI to allow for reporting subsystem temp and fan data
*     for display.
*
*/

#pragma once

class UiUpdater
{
public:

   struct FanData
   {
      float temp{ 0.0f };
      float dutyCycle{0.0f};
      std::vector<std::pair<int,int>> fans;
   };

   virtual void updateSubSystemTemp(int ssid, float temp) = 0;
   virtual void updateFanData(UiUpdater::FanData& fandata ) = 0;
};