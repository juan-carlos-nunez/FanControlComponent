/*
* Class: FanControl
*
* Description: FanControl is responsible for taken a temperature it is given and 
*     setting the PWM Counts for every fan it is made aware of.
*
*     It implemnts the TempMonitorListener interface to facilitate receiving new
*     temperatures from the TempMonitor.
* 
*     It uses a FanRegister object to write to Fan registers, setting the PWM counts
*     for every fan. The PWM Counts is calculated by multiplying the duty cycle by 
*     the fan's proportionality constant.
*
*/

#pragma once

#include "TempMonitorListener.h"
#include "FanRegisters.h"
#include "GeneralConstants.h"
#include "TempMonitor.h"
#include "UiUpdater.h"

#include <unordered_map>   
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

class FanControl final : public TempMonitorListener
{
   const std::vector<int>  fanIds;
   UiUpdater*              uiUpdater{ nullptr };

   bool                    haveNewCurrentTemp{ false };
   float                   currentTemp{ 0.0 };

   FanRegisters            fanRegisters;
   TempMonitor             tempMonitor;

   std::thread             fanThread;
   std::condition_variable fanThreadCond;
   std::mutex              fanThreadMux;
   std::atomic<bool>       fanThreadKeepAlive{ false };

   void updateFansThread();
   void updateFans( float temp ) const;

   GeneralConstants::ReturnCodes setFansToDefault();

public:
   FanControl( const std::vector<int>& subSystemIds,
               const std::vector<int>& fanIds,
               UiUpdater* updater = nullptr);

   FanControl( const std::vector<int>& ssIds,
               const std::vector<int>& fanIds,
               const std::unordered_map<int, uint64_t>& fanAddresses,
               UiUpdater* updater = nullptr);
   
   ~FanControl();

   GeneralConstants::ReturnCodes initialize();
   void notifyNewMaxTemp(float temp);

   GeneralConstants::ReturnCodes start();
   GeneralConstants::ReturnCodes stop();
};