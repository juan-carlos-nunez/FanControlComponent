#pragma once

#include <QtWidgets/QMainWindow>
#include <QMutex>
#include "ui_FanControlUI.h"

#include "FanControl.h"
#include "UiUpdater.h"
#include "SubSystem.h"

class FanControlUI final : public QMainWindow, public UiUpdater
{
   Q_OBJECT
   std::vector<int>   subSystemIds; // List of SubSystem Ids
   std::vector<int>   fanIds;       // List of FanIds

   QMutex fanDataMux;
   QMutex subSysDataMux;
   UiUpdater::FanData localFanData;        // InBound fan data from FanControl
   std::vector<float> localSubSystemTemps; // InBound temps from SubSystems

   std::vector<std::unique_ptr<SubSystem>> subSystems;        // Mock SubSystems
   std::vector<uint32_t>                   mockRegisters;     // Mock Registers
   std::unordered_map<int, uint64_t>       FanIdMemAddresses; // Map of FanID and Mock Registers 

   std::unique_ptr<FanControl> fanControl;

   bool hasBeenInitialized{ false };
   bool hasBeenStarted{ false };

   void stopSubSystems();
   void startSubSystems();

   void initializeSubSystems();
   void initializeFanControl();

   void updateSubSysTempsDisplay();
   void updatefanDataDisplay();

   void generateIds(bool genFanIds = false );

   void stop();
   void start();

public:
    FanControlUI(QWidget *parent = Q_NULLPTR);
    
    void initialize();

    void updateSubSystemTemp(int ssid, float temp);
    void updateFanData(UiUpdater::FanData& fandata);

private:
    Ui::FanControlUIClass ui;

private slots:
   void doButton();
   void updateDisplay();
};
