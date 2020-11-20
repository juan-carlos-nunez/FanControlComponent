/*
* Class: TempMonitorGenerator
*
* Description: This is not used or needed by the FanControl. Intended to 
*     help facilitate demo and testing. Connects to the TempMonitor gRPC
*     server and sends a <SubSystemID / temperature> pair to the server.
*
*/

#pragma once

#include "GeneralConstants.h"

#include <grpcpp/grpcpp.h>
#include "SubSystem.grpc.pb.h"

class SubSystem final : public SubSystemSink::SubSystemServer::Service
{
   int subSystemId{INT_MIN};

   int   tempIncDirection{ 1 };
   float runningTemp{ 30.0f };
   float tempInc{ 0.1f };

   grpc::ServerBuilder           builder;
   std::unique_ptr<grpc::Server> server;

   grpc::Status GetSubSystemTemp(grpc::ServerContext* context, const SubSystemSink::empty_param* idTemp, SubSystemSink::SubSystemTemp* noResponse) override;
   void RunServer();

   bool  unitTestEnabled{ false };
   float unitTestTemp{ 0.0f };

public:
   SubSystem( int ssid, bool utEnabled =false );
   ~SubSystem();

   GeneralConstants::ReturnCodes initialize();
   void setUnitTestTemp( float temp );

};