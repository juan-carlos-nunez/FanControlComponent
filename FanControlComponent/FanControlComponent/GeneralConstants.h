/*
* File: GeneralConstants
*
* Description: General constants used by the project.
* 
*/

#pragma once

#include <unordered_map>

namespace GeneralConstants
{
   const std::vector<std::string> GRPC_SERVER_ADDRESSES
   {
        "localhost:50050"
      , "localhost:50051"
      , "localhost:50052"
      , "localhost:50053"
      , "localhost:50054"
      , "localhost:50055"
      , "localhost:50056"
      , "localhost:50057"
      , "localhost:50058"
      , "localhost:50059"
      , "localhost:50060"
   };

   enum class ReturnCodes
   {
        RETURN_CODE_NOT_SET
      , SUCCESS
      , UNKNOWN_FAN_ID
      , UNKNOWN_SUBSYSTEM_ID
      , NO_FAN_REGISTER
      , NO_FAN_MULTIPLIER
      , FAN_CONTROL_INIT_FAILED
      , FAN_CONTROL_TOO_MANY_FAN_IDS_ERROR
      , TEMP_MONITOR_INIT_FAILED
      , TEMP_MONITOR_LISTENER_REG_FAILED
      , TEMP_MONITOR_LISTENER_UNREG_FAILED
      , UNKNOWN_ERROR
      , UNKNOWN_RETURN_CODE
   };

   const std::unordered_map<ReturnCodes,std::string> ReturnCodesStrings
   {
        { ReturnCodes::RETURN_CODE_NOT_SET                , "Return Code has not been set." }
      , { ReturnCodes::SUCCESS                            , "Success." }
      , { ReturnCodes::UNKNOWN_FAN_ID                     , "An unknown fan id was provided." }
      , { ReturnCodes::UNKNOWN_SUBSYSTEM_ID               , "An unknown subsytem id was provided." }
      , { ReturnCodes::NO_FAN_REGISTER                    , "No fan register was found." }
      , { ReturnCodes::NO_FAN_MULTIPLIER                  , "No fan PWMC Multiplier was found." }
      , { ReturnCodes::FAN_CONTROL_INIT_FAILED            , "Fan Control Initialization failed." }
      , { ReturnCodes::FAN_CONTROL_TOO_MANY_FAN_IDS_ERROR , "Fan Control was given more fan Ids than are supported." }
      , { ReturnCodes::TEMP_MONITOR_INIT_FAILED           , "TempMonitor initialization failed." }
      , { ReturnCodes::TEMP_MONITOR_LISTENER_REG_FAILED   , "TempMonitor was unable to register the listener (possible duplicate)." }
      , { ReturnCodes::TEMP_MONITOR_LISTENER_UNREG_FAILED , "TempMonitor was unable to unregister the listener becaues it could not be found." }
      , { ReturnCodes::UNKNOWN_ERROR                      , "Unknown Error occured." }
      , { ReturnCodes::UNKNOWN_RETURN_CODE                , "Unknown Return Code" }
   };
}