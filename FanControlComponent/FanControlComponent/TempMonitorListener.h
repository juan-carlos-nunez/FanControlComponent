/*
* Class: TempMonitorListener
*
* Description: Pure Virtual interface class. Provides mechansim for
*     objects to register with TempMonitor to be notified of a new 
*     max temp.
*
*/

#pragma once

class TempMonitorListener
{
public:
   virtual void notifyNewMaxTemp( float temp ) = 0;
};