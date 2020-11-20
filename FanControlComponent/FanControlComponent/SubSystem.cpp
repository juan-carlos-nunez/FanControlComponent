#include "SubSystem.h"
#include "log.h"

//
// Name: SubSystem (ctor)
//
// Description: Constructor
//
// Params: channel - The gRPC Channel to use to communicate with gRpc server.
//         ssid - The SubSystem ID to use..
//         updater - Callback to the UI, to update the SubSystem Temps.
//
SubSystem::SubSystem( int ssid, bool utEnabled ) : subSystemId(ssid), unitTestEnabled( utEnabled )
{
   // Empty
}

//
// Name: ~SubSystem (dtor)
//
// Description: Destructor, causes the SubSystem thread to exit.
//
SubSystem::~SubSystem()
{
   DEBUG_STD_OUT("SubSystem::~dtor() - SSID=[" << subSystemId << "] - ENTER");

   if (nullptr != server)
   {
      server->Shutdown();
      server->Wait();
   }

   DEBUG_STD_OUT("SubSystem::~dtor() - SSID=[" << subSystemId << "] - EXIT");
}

//
// Name: initialize
//
// Description: Kicks off the SubSystem gRPC server.
//
// Return: GeneralConstants::ReturnCodes
//
GeneralConstants::ReturnCodes SubSystem::initialize()
{
   RunServer();

   return GeneralConstants::ReturnCodes::SUCCESS;
}

//
// Name: GetSubSystemTemp
//
// Description: RPC Interface that returns the current temp for the subsystem.
//
grpc::Status SubSystem::GetSubSystemTemp(grpc::ServerContext* context,
                                         const SubSystemSink::empty_param* noParam,
                                         SubSystemSink::SubSystemTemp* returnTemp)
{
   tempIncDirection = rand() % 4;

   switch (tempIncDirection)
   {
      // Down
      case 0:
      {
         runningTemp -= tempInc;
         break;
      }
      // Up
      case 1:
      case 2: // Fallthrough
      {
         runningTemp += tempInc;
         break;
      }
      // No Change
      case 3: // Fallthrough
      default:
         break;
   }

   if( unitTestEnabled )
   {
      returnTemp->set_temp(unitTestTemp);
   }
   else
   {
      returnTemp->set_temp( runningTemp );
   }
   return grpc::Status::OK;
}

//
// Name: RunServer
//
// Description: Creates and starts the gRPC server.
//
// {HAZARD_TODO} Setup server credentials. For this exercise, server is using simple insecure credentials. 
//
void SubSystem::RunServer()
{
   builder.AddListeningPort(GeneralConstants::GRPC_SERVER_ADDRESSES[subSystemId], grpc::InsecureServerCredentials());

   builder.RegisterService(this);

   server = builder.BuildAndStart();

   PRINT_STD_OUT("SubSystem[" << subSystemId << "]::RunServer() - Server listening on " << GeneralConstants::GRPC_SERVER_ADDRESSES[subSystemId]);
}

void SubSystem::setUnitTestTemp(float temp)
{
   unitTestTemp = temp;
}