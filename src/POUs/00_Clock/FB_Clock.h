//
// Created by bernhard on 09.09.26.
//

#ifndef CLOCK_H
#define CLOCK_H

#include "Ton.h"
#include "EdgePosNeg.h"
#include "app_config.h"
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <RTClib.h>
#include "../../DUT's/Interface/Interface.h"

class FB_Clock {
public:
    FB_Clock();

    void run(TimestampForMotors &sTimestamp);

private:
    // inputs
    bool ixButtonMenu;
    bool ixButtonSetting;

    // outputs

    // local
    int liStep;
    int liNStep;
    int liStepOld;
    bool lxEntryAction;
    bool lxTemp;
    int liTemp;

    Ton TonStep;
    Ton TonStepF;
    Ton TonStepG;
    Ton TonDebounceButtonMenu;
    Ton TonDebounceButtonSetting;

    EdgePosNeg EdgeButtonMenu;
    EdgePosNeg EdgeButtonSetting;

    Adafruit_SSD1306 display;
    RTC_DS3231 rtc;

    DateTime ldtNow;
    String lstHour;
    String lstMin;
    String lstSec;
    String lstTimestamp;
    int liHour;
    int liMin;
    int liSec;

    void readInputs();

    void updateTimers();

    void updateEdges();

    void debugStepChange();

    String IntToString(const int i);

    void showMessage(const String &line1, const String &line2, const int textSize);
};


#endif
