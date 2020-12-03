#include "FanControlUI.h"

#include <QMutexLocker>
#include <QTimer>

#include <unordered_map>
namespace
{
   const int MAIN_WINDOW_UPDATE_RATE_MS{200};

   const int MAX_AVAILABLE_MOCK_FANS_REGISTERS{ 10 };
   const int MAX_AVAILABLE_SUB_SYSTEMS{ 10 };
}

FanControlUI::FanControlUI(QWidget *parent)
    : QMainWindow(parent)
    , mockRegisters(MAX_AVAILABLE_MOCK_FANS_REGISTERS, 0.0f)
    , localSubSystemTemps(MAX_AVAILABLE_SUB_SYSTEMS, 0.0f)
{
    ui.setupUi(this);
    QPushButton* myBtnStartStop;
    myBtnStartStop = ui.btnStartStop;
    connect(myBtnStartStop, SIGNAL(released()), this, SLOT( doButton() ) );

    QTimer* updateTimer = new QTimer;
    connect( updateTimer, SIGNAL(timeout()), this, SLOT( updateDisplay() ) );
    updateTimer->start(MAIN_WINDOW_UPDATE_RATE_MS);
}

void FanControlUI::doButton()
{
   if( !hasBeenInitialized )
   {
      initialize();
   }
   else if( hasBeenStarted)
   {
      stop();
   }
   else
   {
      start();
   }
}

void FanControlUI::stop()
{
   stopSubSystems();
   ui.btnStartStop->setText(QString("Start"));
   hasBeenStarted = false;
}
void FanControlUI::start()
{
   startSubSystems();
   ui.btnStartStop->setText(QString("Stop"));
   hasBeenStarted = true;
}

void FanControlUI::initialize()
{
   if (!hasBeenInitialized)
   {
      ui.sbNumSubSystems->setDisabled(true);
      ui.sbNumFans->setDisabled(true);

      generateIds();
      generateIds( true );

      initializeFanControl();
      initializeSubSystems();
      ui.btnStartStop->setText(QString("Start"));
      hasBeenInitialized = true;
   }
}

void FanControlUI::initializeSubSystems()
{
   for (int x{ 0 }; x < subSystemIds.size(); ++x)
   {
      subSystems.push_back(std::move(std::unique_ptr<SubSystem>(new SubSystem(subSystemIds[x]))));
      subSystems[x]->initialize();
   }
}

void FanControlUI::initializeFanControl()
{
   auto pMockRegisters = mockRegisters.data();
   for (int x{ 0 }; x < fanIds.size(); ++x)
   {
      FanIdMemAddresses[fanIds[x]] = reinterpret_cast<uint64_t>(&(pMockRegisters[x]));
   }

   fanControl = std::unique_ptr<FanControl >(new FanControl(subSystemIds, fanIds, FanIdMemAddresses, this ) );

   fanControl->initialize();
}

void FanControlUI::startSubSystems()
{
   for (int x{ 0 }; x < subSystems.size(); ++x)
   {
      fanControl->start();
   }
}

void FanControlUI::stopSubSystems()
{
   for (int x{ 0 }; x < subSystems.size(); ++x)
   {
      fanControl->stop();
   }
}

void FanControlUI::updateSubSystemTemp(int ssid, float temp)
{
   --ssid;
   QMutexLocker locker(&fanDataMux);
   if( ssid < localSubSystemTemps.size())
   {
      localSubSystemTemps[ssid] = temp;
   }
}

void FanControlUI::updateFanData(UiUpdater::FanData& fandata)
{
   QMutexLocker locker(&fanDataMux);
   localFanData = std::move(fandata);
}

void FanControlUI::updateDisplay()
{
   if(hasBeenInitialized && hasBeenStarted)
   {
      updateSubSysTempsDisplay();
      updatefanDataDisplay();
   }
   
}

void FanControlUI::updateSubSysTempsDisplay()
{
   std::vector<float> data;
   {
      QMutexLocker locker(&fanDataMux);
      data = localSubSystemTemps;
   }

   for( int idx{0}; idx < localSubSystemTemps.size(); ++idx)
   {
      auto ssid = idx+1;
      QString lcdName = "numSubSys";
      if (10 > ssid)
      {
         lcdName += "0";
      }
      lcdName += QString::number(ssid);
      auto lcdObj = FanControlUI::findChild<QLCDNumber*>(lcdName);
      if (lcdObj)
      {
         lcdObj->display(localSubSystemTemps[idx]);
      }
   }
}

void FanControlUI::updatefanDataDisplay()
{
   UiUpdater::FanData data;
   {
      QMutexLocker locker(&fanDataMux);
      data = std::move(localFanData);
   }

   ui.numDutyCycle->display(data.dutyCycle);
   ui.numHighTemp->display(data.temp);

   for( auto fan : data.fans )
   {
      QString lcdName = "numPwmc";
      if (10 > fan.first)
      {
         lcdName += "0";
      }
      lcdName += QString::number(fan.first);
      auto lcdObj = FanControlUI::findChild<QLCDNumber*>(lcdName);
      if(lcdObj)
      {
         lcdObj->display(fan.second);
      }
   }
}

void FanControlUI::generateIds( bool genFanIds )
{
   auto numItems{ (genFanIds) ? ui.sbNumFans->value() : ui.sbNumSubSystems->value() };

   std::vector<bool> usedIds(11, false);
   int addedIds{ 0 };

   while( addedIds < numItems )
   {
      auto curId = rand() % (11);
      if( !usedIds[curId] && ( 0 < curId ) )
      {
         (genFanIds) ? fanIds.push_back(curId) : subSystemIds.push_back(curId);
         usedIds[curId] = true;
         ++addedIds;
      }
   }
}