#include "TempMonitor.h"
#include "log.h"

#include <utility>

namespace
{
   const int SUB_SYSTEM_TEMP_CHECK_INTERVAL_MS{ 200 };
}

//
// Name: TempMonitor (ctor)
//
// Description: Constructor
//
// Params: ssids - Vector of subsystem ids.
//         updater - Callback to UI to update subsystem temperatures.
//
TempMonitor::TempMonitor( const std::vector<int>& ssIds, UiUpdater* updater)
   : subSystemIds( ssIds ), uiUpdater(updater)
{
   DEBUG_STD_OUT("TempMonitor::ctor() - EXIT");
}

//
// Name: ~TempMonitor (dtor)
//
// Description: Causes the tempThread to exit.
//
TempMonitor::~TempMonitor()
{
   DEBUG_STD_OUT("TempMonitor::dtor() - ENTER");

   if (tempThread.joinable())
   {
      stop();
      tempThreadKeepAlive.store(false);
      start();
      tempThread.join();
   }


   DEBUG_STD_OUT("TempMonitor::dtor() - EXIT");
}

//
// Name: initialize
//
// Description: Starts the tempThread.
//
// Return: GeneralConstants::ReturnCodes
//
GeneralConstants::ReturnCodes TempMonitor::initialize()
{
   buildSubSystemConnections();

   tempThreadKeepAlive.store(true);
   tempThread = std::thread(&TempMonitor::updateTempsThread, this);

   return GeneralConstants::ReturnCodes::SUCCESS;
}

//
// Name: buildSubSystemConnections
//
// Description: Builds the needed connections to the SubSystem gRPC servers.
//
// {HAZARD}: Insecure connection to server.
//
// {HAZARD_MEDIATION}: Setup proper security credentials for the given environment.
//
// {HAZARD_TODO}: Hazard Mediation.
//
void TempMonitor::buildSubSystemConnections()
{
   for( auto ssid : subSystemIds )
   {
      auto channel{ grpc::CreateChannel(GeneralConstants::GRPC_SERVER_ADDRESSES[ssid], grpc::InsecureChannelCredentials()) };

      subSystemStubs[ssid] = SubSystemSink::SubSystemServer::NewStub(channel);
   }
}

//
// Name: updateTempsThread
//
// Description: Main for the tempThread, calling the worker method
//    to get the temp and update tables.
//
void TempMonitor::updateTempsThread()
{
   while( tempThreadKeepAlive.load() )
   {
      std::unique_lock<std::mutex> lock(tempThreadMux);
      tempThreadCond.wait(lock, [this]() { return tempThreadRun.load(); });

      if (!tempThreadKeepAlive.load())
      {
         break;
      }

      querySubSystems();

      std::this_thread::sleep_for(std::chrono::milliseconds(SUB_SYSTEM_TEMP_CHECK_INTERVAL_MS));

   }
}

// Name: querySubSystems
//
// Description: Polls every SubSystem for their temperatures, 
//       updating internal tables to keep track and identify
//       the highest temperature. If a new high temperature is
//       identified, it will notify FanControl.
//
void TempMonitor::querySubSystems()
{
   SubSystemSink::empty_param    emptyParam;
   SubSystemSink::SubSystemTemp  rVal;
   
   for (auto ssid : subSystemIds)
   {
      grpc::ClientContext context;
      grpc::Status status = subSystemStubs[ssid]->GetSubSystemTemp(&context, emptyParam, &rVal);

      if( status.ok() && ( 0 < rVal.temp() ) )
      {
         updateTempTables( ssid, rVal.temp() );
         if (updateCurMaxTemp())
         {
            // New Max temp.
            notifyNewMaxTemp();
         }
      }
   }
}

//
// Name: updateTempTables
//
// Description: Check if the temp changed. If it did, remove the old temp 
// from the sorted list and insert the new temp.
//
// Params: ssid - The SubSystem Id the temp is associated with.
//         temp - The temperature from the SubSystem.
//
// Notes: Find T: O(logn), Erase Itr T: O(1), Insert is T: O(logn)
//
void TempMonitor::updateTempTables(int ssid, float temp)
{

   // [Post Submittal Observation] Precision was not taken into account
   // when doing the floating point comparision. For the demo, changes in
   // temp were all 1-tenth of a degree, making the below simple check sufficient.
   // If higher precision is needed, the below check may need to be changed.
   // E.g. Get the absolute-value of difference and compare the difference to the desired precision.

   // No need to do anything if the temp hasn't changed.
   if (subSystemTemps[ssid] != temp)
   {
      if(uiUpdater)
      {
         uiUpdater->updateSubSystemTemp( ssid, temp );
      }

      // Find a matching temp in the ordered list of temps.
      // Remove the old temp and insert the new temp.
      auto Itr{ curTemps.find(subSystemTemps[ssid]) };
      if( curTemps.end() != Itr )
      {
         curTemps.erase(Itr); // Old
      }
      curTemps.insert(temp); // New

      subSystemTemps[ssid] = temp; // Update
   }
}

//
// Name: updateCurMaxTemp
//
// Description: Checks to see if there is a new max temp.
//
// Return: bool - True if a new max temp was identified. False otherwise.
//
bool TempMonitor::updateCurMaxTemp()
{
   auto rVal{ false };
   auto maxTemp{ *(--curTemps.end()) };
   if( maxTemp != curMaxTemp )
   {
      curMaxTemp = maxTemp;
      rVal = true;
   }
   return rVal;
}

//
// Name: notifyNewMaxTemp
//
// Description: Notifies registered listeners of a new max temperature.
//
// {RISK} As the number of listeners increase, the longer it will take
//    for this thread to finish. Also, any listener not returning immediatly
//    will cause delays and interrupt processing of temperatures.
//
// {RISK_MITIGATION} Consider a seperate thread (e.g. async). However,
//    care needs to be taken since, while the thread runs, a new max temp may be
//    identified.
//
void TempMonitor::notifyNewMaxTemp()
{
   const std::lock_guard<std::mutex> lock(listenersMux); 
   for( auto listener : listeners )
   {
      listener->notifyNewMaxTemp(curMaxTemp);
   }
}

//
// Name: registerListener
//
// Description: API Implementation that adds the listeners to the list of listeners.
//
// Return: GeneralConstants::ReturnCodes
//
GeneralConstants::ReturnCodes TempMonitor::registerListener(TempMonitorListener& listener)
{
   auto rVal{ GeneralConstants::ReturnCodes::TEMP_MONITOR_LISTENER_REG_FAILED };

   const std::lock_guard<std::mutex> lock(listenersMux);

   auto Itr{ std::find(listeners.begin(), listeners.end(), &listener) };
   if (listeners.end() == Itr)
   {
      listeners.push_back( &listener );
      rVal = GeneralConstants::ReturnCodes::SUCCESS;
   }
   return rVal;
}

//
// Name: unregisterListener
//
// Description: API Implementation that removes the listener to the list of listeners.
//
// Return: GeneralConstants::ReturnCodes
//
GeneralConstants::ReturnCodes TempMonitor::unregisterListener(TempMonitorListener& listener)
{
   auto rVal{ GeneralConstants::ReturnCodes::TEMP_MONITOR_LISTENER_UNREG_FAILED };
   
   const std::lock_guard<std::mutex> lock(listenersMux);

   auto Itr{ std::find(listeners.begin(), listeners.end(), &listener) };
   if (listeners.end() != Itr)
   {
      listeners.erase(Itr);
      rVal = GeneralConstants::ReturnCodes::SUCCESS;
   }
   return rVal;
}

// Name: start
//
// Description: Causes the thread to start running.
//
// Return: GeneralConstants::ReturnCodes
//
GeneralConstants::ReturnCodes TempMonitor::start()
{
   tempThreadRun.store(true);
   tempThreadCond.notify_one();
   return GeneralConstants::ReturnCodes::UNKNOWN_ERROR;
}

//
// Name: stop
//
// Description: Causes the thread to stop running.
//
// Return: GeneralConstants::ReturnCodes
//
GeneralConstants::ReturnCodes TempMonitor::stop()
{
   tempThreadRun.store(false);
   return GeneralConstants::ReturnCodes::SUCCESS;
}