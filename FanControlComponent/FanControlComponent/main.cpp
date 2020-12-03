
#include "FanControl.h"
#include "TempMonitor.h"
#include "SubSystem.h"
#include "log.h"

#include <vector>
#include <chrono>

void printMenu()
{
   PRINT_STD_OUT("---------\nP - Menu\nG - Go\nS - Stop\nE - exit\n---------");
}

void doMenu(std::vector<std::unique_ptr<SubSystem>>& subSystems, FanControl& fanCntrl )
{
   char input = 'P';
   printMenu();
   while( ( input != 'E' ) && ( input != 'e') )
   {
      std::cin >> input;

      switch (input)
      {
         case 'P': // Fallthrough
         case 'p':
         {
            printMenu();
            break;
         }
         case 'G': // Fallthrough
         case 'g':
         {
            for (int x{ 0 }; x < subSystems.size(); ++x)
            {
               fanCntrl.start();
            }
            break;
         }
         case 'S': // Fallthrough
         case 's':
         {
            for (int x{ 0 }; x < subSystems.size(); ++x)
            {
               fanCntrl.stop();
            }
            printMenu();
            break;
         }
         default:
         {
            for (int x{ 0 }; x < subSystems.size(); ++x)
            {
               fanCntrl.stop();
            }
            return;
         }
      }
   }
}

int main()
{
   std::vector<int> subSystemIds{ 1, 2, 3, 4,  5,  6,  7,  8,  9, 10 };
   std::vector<int> fanIds{ 8, 6, 2, 4, 20, 18, 14, 16, 10, 12 };
   uint32_t         mockRegisters[10]{ 0 };

   std::unordered_map<int, uint64_t> FanIdMemAddresses;

   for (int x{ 0 }; x < 10; ++x)
   {
      FanIdMemAddresses[fanIds[x]] = reinterpret_cast<uint64_t>(&(mockRegisters[x]));
   }

   FanControl fanCntrl(subSystemIds, fanIds, FanIdMemAddresses);
   auto rVal{ fanCntrl.initialize() };

   DEBUG_STD_OUT("Main() - INFO: FanControl Initialization returned: [" << GeneralConstants::ReturnCodesStrings.find(rVal)->second << "]")

   std::vector<std::unique_ptr<SubSystem>> subSystems;
   for( auto ssid : subSystemIds )
   {
      subSystems.push_back( std::move( std::unique_ptr<SubSystem>( new SubSystem(ssid) ) ) );
   }

   for (int x{ 0 }; x < subSystems.size(); ++x)
   {
      subSystems[x]->initialize();
   }

   doMenu(subSystems, fanCntrl);

   return 0;
}