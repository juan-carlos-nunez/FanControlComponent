#include "FanControl.h"
#include "TempToDutyCycle.h"
#include "FanConstants.h"
#include "log.h"

//
// Name: FanControl
//
// Description: Constructor, instantiating the FanRegister and TempMonitor objects.
//
// Params: ssIds - Vector of supported SubSystemIDs
//         fanIds - Vector of fanIds which the FanControl will control.
//         updater - Callback to the UI, to update the fan related Temps.
//
FanControl::FanControl(const std::vector<int>& ssIds, const std::vector<int>& fanIds, UiUpdater* updater)
   : fanIds(fanIds)
   , uiUpdater(updater)
   , fanRegisters(FanConstants::FAN_REGISTER_ADDRESSES)
   , tempMonitor(ssIds, updater)
{
   DEBUG_STD_OUT("FanControl::ctor() - EXIT");
}

//
// Name: FanControl
//
// Description: Constructor, instantiating the FanRegister and TempMonitor objects.
//
// Params: ssIds - Vector of supported SubSystemIDs
//         fanIds - Vector of fanIds which the FanControl will control.
//         fanAddresses - Map of <FanId, Memory Addresses>, allowing the FanControl
//             to use passed in memory addresses instead of hard-coded ones.
//         updater - Callback to the UI, to update the fan related Temps.
//
FanControl::FanControl( const std::vector<int>& ssIds,
                        const std::vector<int>& fanIds, 
                        const std::unordered_map<int, uint64_t>& fanAddresses,
                        UiUpdater* updater )
   : fanIds(fanIds)
   , uiUpdater(updater)
   , fanRegisters(fanAddresses)
   , tempMonitor(ssIds, updater)
{
   PRINT_STD_OUT("FanControl::ctor() - USING MOCK MEMORY ADDRESSES FOR REGISTERS")
}

//
// Name: ~FanControl (dtor)
//
// Description: Causes the fanThread to exit.
//
FanControl::~FanControl()
{
   DEBUG_STD_OUT("FanControl::dtor() - ENTER");

   if(fanThread.joinable() )
   {
      {
         std::lock_guard<std::mutex> guard(fanThreadMux);
         haveNewCurrentTemp = true;
         fanThreadKeepAlive.store(false);
      }

      fanThreadCond.notify_one();
      fanThread.join();
   }

   DEBUG_STD_OUT("FanControl::dtor() - EXIT");
}

// Name: initialize
//
// Description: 
//    1. Check to make sure all provided FanIds have a PWMC Multiplier and Fan Register.
//    2. Set all fans to default speeds.
//    3. Register with TempMonitor as a listener.
//    4. Start Listener thread so it is ready to process new temps.
//    4. Initialize the TempMonitor.
//
// Return: GeneralConstants::ReturnCodes
//
GeneralConstants::ReturnCodes FanControl::initialize()
{
   auto rVal{ fanRegisters.checkFanIds( fanIds ) };

   if (GeneralConstants::ReturnCodes::SUCCESS == rVal)
   {
      rVal = setFansToDefault();
   }

   if( GeneralConstants::ReturnCodes::SUCCESS == rVal)
   {
      rVal = tempMonitor.registerListener(*this);
   }

   if (GeneralConstants::ReturnCodes::SUCCESS == rVal)
   {
      fanThreadKeepAlive.store(true);
      fanThread = std::thread(&FanControl::updateFansThread, this);
   }

   if (GeneralConstants::ReturnCodes::SUCCESS == rVal)
   {
      rVal = tempMonitor.initialize();
   }

   return rVal;
}

//
// Name: setFansToDefault
//
// Description: Sets all fans to the default dudty cycle.
//
// Return: GeneralConstants::ReturnCodes
//
GeneralConstants::ReturnCodes FanControl::setFansToDefault()
{
   std::lock_guard<std::mutex> guard(fanThreadMux);
   currentTemp = FanConstants::INITIAL_FAN_DUTY_CYCLE;
   updateFans( currentTemp );

   return GeneralConstants::ReturnCodes::SUCCESS;
}

// Name: updateFans
//
// Description: 
//    1. Calculate the new duty cycle using the given temp.
//    2. Loop over all fans.
//    3. Calculate the PWM Count for the current fan.
//    4. write the new pwm count to the fan register.
//    5. Repeat for all fan in the FanPwmcMultiplier LUT.
//
// Note: If there is no multiplier, don't adjust the fan. 
//       The Ctor does an initial check and there should 
//       be a multiplier for each pwmc.
//
// Params: temp - The temperature used to calculate the duty cycle for all fans.
//
// {RISK}: The duty cycle is rounded-to-zero for translation to PWMCs.
//
// {HAZARD}: Proper error handling needs to be implemented in case a
//           multiplier is not found. At this point, the component (and system) 
//           is running. The fan in question cannot be safely adjusted.
//
// {HAZARD_MEDIATION}: Implement proper error handling for the given system.
//
// {HAZARD_TODO}: Hazard Mediation.
//
void FanControl::updateFans( float temp ) const
{
   UiUpdater::FanData fanData;

   auto dutyCycle{ TempToDutyCycle::getDutyCycle(temp) };

   fanData.temp = temp;
   fanData.dutyCycle = dutyCycle;

   PRINT_STD_OUT( "FanControl::updateFans(): CurTemp=[" << temp << "], DC=[" << dutyCycle << "]" )

   for( auto fanId : fanIds )
   {
      auto pwmcMultiplier{ FanConstants::FAN_PWMC_PROPORTIONALITY.find(fanId) };
      if( FanConstants::FAN_PWMC_PROPORTIONALITY.end() != pwmcMultiplier)
      {
         auto roundedDc{ static_cast<int>( std::round(dutyCycle) ) };
         auto pwmc{ roundedDc * pwmcMultiplier->second };

         fanRegisters.writeRegister( fanId, pwmc );
         fanData.fans.push_back( std::make_pair( fanId, pwmc ));
      }
   }

   if (uiUpdater)
   {
      uiUpdater->updateFanData(fanData);
   }

}

//
// Name: updateFansThread
//
// Description: FanControl main thread for updating Fan duty cycles.
//
void FanControl::updateFansThread()
{
   while( fanThreadKeepAlive.load() )
   {
      std::unique_lock<std::mutex> guard( fanThreadMux );
      fanThreadCond.wait( guard, [this](){ return haveNewCurrentTemp; } );
      
      auto localCopyTemp = currentTemp;
      haveNewCurrentTemp = false;

      guard.unlock();

      if( !fanThreadKeepAlive.load() )
      {
         break;
      }

      updateFans(localCopyTemp);
   }
}

//
// Name: notifyNewMaxTemp
//
// Description: TempMonitorListener API - callback, notifying 
//       the FanControl of a new MaxTemperature.
//
// Params: temp - The temperature being notified.
//
void FanControl::notifyNewMaxTemp(float temp)
{
   std::lock_guard<std::mutex> guard(fanThreadMux);
   currentTemp = temp;
   haveNewCurrentTemp = true;

   fanThreadCond.notify_one();
}

// Name: start
//
// Description: Causes the thread to start running.
//
// Return: GeneralConstants::ReturnCodes
//
GeneralConstants::ReturnCodes FanControl::start()
{
   tempMonitor.start();
   return GeneralConstants::ReturnCodes::UNKNOWN_ERROR;
}

//
// Name: stop
//
// Description: Causes the thread to stop running.
//
// Return: GeneralConstants::ReturnCodes
//
GeneralConstants::ReturnCodes FanControl::stop()
{
   tempMonitor.stop();
   return GeneralConstants::ReturnCodes::SUCCESS;
}