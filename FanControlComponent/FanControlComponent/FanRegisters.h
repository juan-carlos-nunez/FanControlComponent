/*
* Class: FanRegisters
*
* Description: Provides read, write and clearing to the memory addresses
*     provided in the map of [FanIds, Fan Memory Address].
*
* WARNING: Not thread safe. 
* 
*/

#pragma once

#include "GeneralConstants.h"

#include <unordered_map>

class FanRegisters final
{
   const std::unordered_map<int,uint64_t> fanAddressLut;

   inline volatile uint32_t* fanRegAddr(uint64_t fanAddress) const;
   volatile uint32_t* getFanRegAddr(int fanId) const;

public:

   FanRegisters(const std::unordered_map<int, uint64_t>& fanAddresses );
   ~FanRegisters();

   void writeRegister( int fanId, unsigned int pwmc = 0 ) const;
   int  readRegister ( int fanId ) const;
   void clearRegister( int fanId ) const;

   GeneralConstants::ReturnCodes checkFanIds(const std::vector<int>& fanIds) const;
};