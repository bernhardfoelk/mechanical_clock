//
// Created by bernhard on 09.09.26.
//

#include "../00_Clock//FB_Clock.h"
#include "Arduino.h"

FB_Clock::FB_Clock() : display(128, 64, &Wire, -1), ldtNow(0, 0, 0, 0, 0, 0) {
    ixButtonMenu = false;
    ixButtonSetting = false;

    liStep = 0;
    liNStep = 0;
    liStepOld = 0;
    lxEntryAction = false;
    lxTemp = false;
    liTemp = 0;

    lstHour = "";
    lstMin = "";
    lstSec = "";
    lstTimestamp = "00:00:00";
    liHour = 0;
    liMin = 0;
    liSec = 0;

    // set pinmode for input variables
    pinMode(PIN_BUTTON_MENU, INPUT_PULLUP);
    pinMode(PIN_BUTTON_SETTING, INPUT_PULLUP);
}

void FB_Clock::run(TimestampForMotors &sTimestamp) {
    readInputs();

    switch (liStep) {
        //***************************************
        // Reset
        //***************************************
        case 0:
            if (lxEntryAction) {
                lxEntryAction = false;
            }

            if (true) {
                Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
                liStep = 2;
            }
            break;

        case 2:
            if (lxEntryAction) {
                lxEntryAction = false;
            }

            liStep = 4;
            if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
                ; //debugln("FB_Clock - Display nicht gefunden! Bitte neu starten.");
                liStep = -20;
            }
            break;

        case 4:
            if (lxEntryAction) {
                showMessage("RTC wird", "initialisiert", 1);

                lxEntryAction = false;
            }

            TonStep.IN(true);
            TonStep.PT(1000);

            if (TonStep.Q()) {
                liStep = 6;
            }
            break;

        case 6:
            if (lxEntryAction) {
                lxEntryAction = false;
            }

            liStep = 20;

            if (!rtc.begin()) {
                showMessage("RTC nicht gefunden! Bitte neu starten.", "", 1);
                liStep = -20;
            }

            break;


        //***************************************
        // Main step
        //***************************************
        case 20:
            if (lxEntryAction) {
                lxEntryAction = false;
            }

            TonStep.IN(!ixButtonMenu);
            TonStep.PT(200);

            TonStepF.IN(ixButtonMenu);
            TonStepF.PT(500);

            if (TonStep.Q()) {
                liStep = 1000; // Show Time
            } else if (TonStepF.Q()) {
                liStep = 9000;
                liNStep = 2000; // Adjust Time
            }
            break;

        //***************************************
        // Show time
        //***************************************
        case 1000:
            if (lxEntryAction) {
                ldtNow = rtc.now();

                liHour = ldtNow.hour();
                liMin = ldtNow.minute();
                liSec = ldtNow.second();

                lstHour = IntToString(liHour);
                lstMin = IntToString(liMin);
                lstSec = IntToString(liSec);
                lstTimestamp = lstHour + ":" + lstMin + ":" + lstSec;

                showMessage("Uhrzeit", lstTimestamp, 2);

                lxEntryAction = false;
            }

            if (true) {
                liStep = 20;
            }
            break;

        //***************************************
        // Adjust time
        //***************************************
        case 2000: // get current timestamp
            if (lxEntryAction) {
                ldtNow = rtc.now();

                liHour = ldtNow.hour();
                liMin = ldtNow.minute();
                liSec = ldtNow.second();

                liTemp = 1;

                lxEntryAction = false;
            }

            if (true) {
                liStep = 2100;
            }
            break;

        //***************************************
        case 2100: // show timestamp
            if (lxEntryAction) {
                // show set timestamp
                lstHour = IntToString(liHour);
                lstMin = IntToString(liMin);
                lstSec = IntToString(liSec);
                lstTimestamp = lstHour + ":" + lstMin + ":" + lstSec;

                showMessage("Neu", lstTimestamp, 2);

                lxEntryAction = false;
            }

            if (true) {
                liStep = 2200;
            }
            break;

        //***************************************
        case 2200: // adjust timestamp
            if (lxEntryAction) {
                lxEntryAction = false;
            }

            // back to show time after 5s of no input
            TonStepF.IN(!ixButtonMenu && !ixButtonSetting);
            TonStepF.PT(3000);

            // save timestamp
            TonStep.IN(ixButtonMenu);
            TonStep.PT(500);

            // switch between hours/minutes/seconds
            if (EdgeButtonMenu.EdgeNeg()) {
                liTemp++;
                if (liTemp > 3) {
                    liTemp = 1;
                }
            }

            if (TonStepF.Q()) {
                liStep = 9000;
                liNStep = 20; // change aborted
            } else if (TonStep.Q()) {
                rtc.adjust(DateTime(ldtNow.year(), ldtNow.month(), ldtNow.day(), liHour, liMin, liSec));
                liStep = 9000;
                liNStep = 20; // change finished
            } else if (EdgeButtonSetting.EdgeNeg()) {
                switch (liTemp) {
                    case 1:
                        liHour++;
                        if (liHour > 23) {
                            liHour = 0;
                        }
                        break;

                    case 2:
                        liMin++;
                        if (liMin > 59) {
                            liMin = 0;
                        }
                        break;

                    case 3:
                        liSec++;
                        if (liSec > 59) {
                            liSec = 0;
                        }
                        break;
                }

                liStep = 2100; // show new timestamp
            }
            break;


        //***************************************
        // Wait for buttons released
        //***************************************
        case 9000:
            if (lxEntryAction) {
                display.clearDisplay();
                display.display();
                lxEntryAction = false;
            }

            TonStep.IN(!ixButtonMenu && !ixButtonSetting);
            TonStep.PT(50);

            if (TonStep.Q()) {
                liStep = liNStep;
            }
            break;
    }

    sTimestamp.iHour = ldtNow.hour();
    sTimestamp.iMinute = ldtNow.minute();
    sTimestamp.iSecond = ldtNow.second();

    updateTimers();
    updateEdges();
    debugStepChange();
}

void FB_Clock::readInputs() {
    TonDebounceButtonMenu.IN(!(digitalRead(PIN_BUTTON_MENU)));
    TonDebounceButtonMenu.PT(10);
    TonDebounceButtonMenu.run();
    ixButtonMenu = TonDebounceButtonMenu.Q();

    TonDebounceButtonSetting.IN(!(digitalRead(PIN_BUTTON_SETTING)));
    TonDebounceButtonSetting.PT(10);
    TonDebounceButtonSetting.run();
    ixButtonSetting = TonDebounceButtonSetting.Q();
}

void FB_Clock::updateTimers() {
    // Timer call
    if (liStep != liStepOld) {
        TonStep.IN(false);
        TonStepF.IN(false);
        TonStepG.IN(false);
    }

    TonStep.run();
    TonStep.IN(false);

    TonStepF.run();
    TonStepF.IN(false);

    TonStepG.run();
    TonStepG.IN(false);
}

void FB_Clock::updateEdges() {
    EdgeButtonMenu.run(ixButtonMenu);
    EdgeButtonSetting.run(ixButtonSetting);
    if (EdgeButtonMenu.EdgeNeg()) {
        ; //debugln("FB_Clock - Button Menu: Negative Flanke!");
    }
    if (EdgeButtonSetting.EdgeNeg()) {
        ; //debugln("FB_Clock - Button Setting: Negative Flanke!");
    }
}

void FB_Clock::debugStepChange() {
    if (liStep != liStepOld) {
        liStepOld = liStep;
        lxEntryAction = true;; //debug("FB_Clock - Aktueller Schritt: ");
        ; //debugln(liStep);
    }
}

String FB_Clock::IntToString(const int i) {
    String s = "";
    if (i < 10) {
        s = "0";
    }
    s = s + String(i);
    return s;
}

void FB_Clock::showMessage(const String &line1, const String &line2, const int textSize) {
    display.clearDisplay();
    display.setTextSize(textSize);
    display.setTextColor(WHITE);

    int16_t x1, y1;
    uint16_t w1, h1, w2 = 0, h2 = 0;

    display.getTextBounds(line1, 0, 0, &x1, &y1, &w1, &h1);

    int lineGap = 16 * textSize;
    int blockHeight = h1;

    if (line2.length() > 0) {
        display.getTextBounds(line2, 0, 0, &x1, &y1, &w2, &h2);
        blockHeight = lineGap + h2;
    }

    int startY = (display.height() - blockHeight) / 2;

    display.setCursor((display.width() - w1) / 2, startY);
    display.println(line1);

    if (line2.length() > 0) {
        display.setCursor((display.width() - w2) / 2, startY + lineGap);
        display.println(line2);
    }

    display.display();
}
