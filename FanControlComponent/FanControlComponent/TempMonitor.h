/*
* Class: TempMonitor
*
* Description: Reponsible for receiving temperatures from multiple subsystems
*     asynchronously. It makes use of a double buffer (with a queue as the buffer)
*     to allow the draining of one buffer while the second buffer is being filled.
* 
*     It is also responsible for monitoring the max temp across all subsystems.
*     When a new max temp is identified, it will notify all the listeners of the 
*     the new max temp value.
*
*/

#pragma once
#include "TempMonitorListener.h"
#include "GeneralConstants.h"
#include "UiUpdater.h"

#include <unordered_map>   // LUT of SSID & gRPC Channels.
#include <set>             // Order set of temps.
#include <vector>          // Listeners list.
#include <mutex>
#include <atomic>
#include <thread>

#include <grpcpp/grpcpp.h>
#include "SubSystem.grpc.pb.h"

class TempMonitor final
{
   const std::vector<int>  subSystemIds;
   UiUpdater*              uiUpdater{ nullptr };

   std::unordered_map<int, std::unique_ptr<SubSystemSink::SubSystemServer::Stub> > subSystemStubs; // gRPC Stubs.

   std::unordered_map<int, float> subSystemTemps; // Keep track of each SubSystem temp.
   std::multiset<float>           curTemps; // Ordered, so O(1) to get highest temp.
   float                          curMaxTemp{ 0.0 };
                                     
   std::thread             tempThread;
   std::condition_variable tempThreadCond;
   std::mutex              tempThreadMux;
   std::atomic<bool>       tempThreadKeepAlive{ false };
   std::atomic<bool>       tempThreadRun{ false };

   std::vector<TempMonitorListener*> listeners;
   std::mutex                        listenersMux;
  
   void buildSubSystemConnections();

   void querySubSystems();
   void updateTempTables(int ssid, float temp);
   bool updateCurMaxTemp();
   void notifyNewMaxTemp();

   void updateTempsThread();

public:
   TempMonitor( const std::vector<int>& subSystemIds, UiUpdater* updater = nullptr);
   ~TempMonitor();

   GeneralConstants::ReturnCodes initialize();

   GeneralConstants::ReturnCodes registerListener(TempMonitorListener& listener);
   GeneralConstants::ReturnCodes unregisterListener(TempMonitorListener& listener);

   GeneralConstants::ReturnCodes start();
   GeneralConstants::ReturnCodes stop();
};

