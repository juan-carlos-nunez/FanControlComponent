#include "FanRegisters.h"
#include "log.h"

//
// Name: FanRegister (ctor)
//
// Description: Constructor
//
// Params: fanAddresses - map of pairs (FanId, register address)
//
FanRegisters::FanRegisters( const std::unordered_map<int, uint64_t>& fanAddresses )
   : fanAddressLut(fanAddresses)
{
   DEBUG_STD_OUT("FanRegisters::ctor() - EXIT");
}

//
// Name: ~FanRegister (dtor)
//
// Description: Destructor
//
FanRegisters::~FanRegisters()
{
   DEBUG_STD_OUT("FanRegisters::dtor() - EXIT");
}

//
// Name: WriteRegister
//
// Description: Writes the pwmc value to the fanId register.
//
// Params: fanId - Fan ID to be used for address lookup.
//         pwmc  - PWM Counts to be written into the address.
//
void FanRegisters::writeRegister(int fanId, unsigned int pwmc) const
{
   auto fanAddr{ getFanRegAddr(fanId) };
   if( nullptr != fanAddr )
   {
      *fanAddr = pwmc;
   }
}

//
// Name: readRegister
//
// Description: returns the pwmc value found in the FanID register.
//
// Params: fanId - Fan ID to be used for address lookup.
//
// Return: int - PWMC found in the FanID provided.
//
int FanRegisters::readRegister(int fanId) const
{
   int rVal{0};
   auto fanAddr{ getFanRegAddr(fanId) };
   if (fanAddr)
   {
      rVal = *fanAddr;
   }
   return rVal;
}

//
// Name: clearRegister
//
// Description: sets the FanID register to zero
//
// Params: fanId - Fan ID to be used for address lookup and clearing.
//
void FanRegisters::clearRegister(int fanId) const
{
   writeRegister( fanId );
}

//
// Name: getFanRegAddr
//
// Description: Returns a pointer to the FanID register.
//
// Params: fanId - Fan ID to be used for address lookup.
//
// Return: uin32_t* - Pointer to the FanID register address.
//
volatile uint32_t* FanRegisters::getFanRegAddr(int fanId) const
{
   volatile uint32_t* rVal{ nullptr };

   auto fanAddrItr = fanAddressLut.find(fanId);
   if(fanAddressLut.end() != fanAddrItr)
   {
      rVal = fanRegAddr( fanAddrItr->second );
   }
   return rVal;
}

//
// Name: fanRegAddr
//
// Description: Converts the numeric address to an actual ponter.
//
// Params: fanAddress - Numeric address
//
// Return: uin32_t* - Pointer versoin of the provided numeric address.
//
inline volatile uint32_t* FanRegisters::fanRegAddr(uint64_t fanAddress) const
{
   return reinterpret_cast<volatile uint32_t*>(fanAddress);
}

//
// Name: checkFanIds
//
// Description: Checks that the FanIds provided all have fan addresses.
//
// Params: fanIds - Vector of FanIds to be checked.
//
// Return: GeneralConstants::ReturnCodes.
//
GeneralConstants::ReturnCodes FanRegisters::checkFanIds(const std::vector<int>& fanIds) const
{
   auto rVal{ GeneralConstants::ReturnCodes::RETURN_CODE_NOT_SET };

   if (fanIds.size() > fanAddressLut.size())
   {
      rVal = GeneralConstants::ReturnCodes::FAN_CONTROL_TOO_MANY_FAN_IDS_ERROR;
   }
   else
   {
      for (auto fanId : fanIds)
      {
         auto fanRegItr{ fanAddressLut.find(fanId) };
         if (fanRegItr == fanAddressLut.end())
         {
            rVal = GeneralConstants::ReturnCodes::UNKNOWN_FAN_ID;
            break;
         }

         auto fanMultItr{ fanAddressLut.find(fanId) };
         if (fanMultItr == fanAddressLut.end())
         {
            rVal = GeneralConstants::ReturnCodes::NO_FAN_MULTIPLIER;
            break;
         }
      }
   }

   if (GeneralConstants::ReturnCodes::RETURN_CODE_NOT_SET == rVal)
   {
      rVal = GeneralConstants::ReturnCodes::SUCCESS;
   }

   return rVal;
}