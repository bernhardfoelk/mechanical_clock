//
// Created by bernhard on 12.09.26.
//

#include "../_Comm/FB_Comm_TMC2209.h"
#include "Arduino.h"

FB_Comm_TMC2209::FB_Comm_TMC2209(HardwareSerial &isSerial, const int iiUartRxPin, const int iiUartTxPin,
                                 const float irRSense)
    : liUartRxPin(iiUartRxPin),
      liUartTxPin(iiUartTxPin),
      lrRSense(irRSense),

      liStep(0),
      liNStep(0),
      liStepOld(0),
      lxEntryAction(false),
      lxTemp(false),
      liTemp(0),

      liRunCurrentMa(800),
      liHoldCurrentPercent(50),
      liMicrosteps(8),
      lxStealthChop(true),
      lsSerial(isSerial),
      driver(&lsSerial, lrRSense, 1) {
}

void FB_Comm_TMC2209::run(TMCDriverStatus &sDriverStatus) {
    switch (liStep) {
        //***************************************
        // Reset
        //***************************************
        case 0:
            if (lxEntryAction) {
                lxEntryAction = false;
            }

            if (true) {
                lsSerial.begin(115200, SERIAL_8N1, liUartRxPin, liUartTxPin);
                liStep = 4;
            }
            break;

        case 4:
            if (lxEntryAction) {
                lxEntryAction = false;
            }

            driver.begin();

            driver.toff(4);
            driver.pdn_disable(true);
            driver.mstep_reg_select(true);
            driver.I_scale_analog(false);

            driver.rms_current(liRunCurrentMa);
            driver.ihold(31UL * liHoldCurrentPercent / 100);
            driver.TPOWERDOWN(20);

            driver.microsteps(liMicrosteps);
            driver.en_spreadCycle(!lxStealthChop);

            if (true) {
                debugln("FB_Comm_TMC2209 - Driver initialisiert.");
                liStep = 20;
            }
            break;


        //***************************************
        // Main step
        //***************************************
        case 20:
            if (lxEntryAction) {
                lxEntryAction = false;
            }

            TonStep.IN(true);
            TonStep.PT(2000);

            if (TonStep.Q()) {
                liStep = 1000; // Get data from driver
            }
            break;


        //***************************************
        // Get data from driver
        //***************************************
        case 1000:
            if (lxEntryAction) {
                lxEntryAction = false;
            }

            lsStatusDriver.xConnected = (driver.test_connection() == 0);
            lsStatusDriver.xOpenLoadA = driver.ola();
            lsStatusDriver.xOpenLoadB = driver.olb();
            lsStatusDriver.xOvertemperatureWarning = driver.otpw();
            lsStatusDriver.xOvertemperatureShutdown = driver.ot();
            lsStatusDriver.iStallGuardResult = driver.SG_RESULT();

            debug("FB_Comm_TMC2209 - xConnected: ");
            debugln(lsStatusDriver.xConnected);
            debug("FB_Comm_TMC2209 - xOpenLoadA: ");
            debugln(lsStatusDriver.xOpenLoadA);
            debug("FB_Comm_TMC2209 - xOpenLoadB: ");
            debugln(lsStatusDriver.xOpenLoadB);
            debug("FB_Comm_TMC2209 - xOvertemperatureWarning: ");
            debugln(lsStatusDriver.xOvertemperatureWarning);
            debug("FB_Comm_TMC2209 - xOvertemperatureShutdown: ");
            debugln(lsStatusDriver.xOvertemperatureShutdown);
            debug("FB_Comm_TMC2209 - iStallGuardResult: ");
            debugln(lsStatusDriver.iStallGuardResult);
            debugln(driver.IOIN());

            if (true) {
                liStep = 20;
            }
            break;
    }

    // write status to interface
    sDriverStatus.xConnected = lsStatusDriver.xConnected;
    sDriverStatus.xOpenLoadA = lsStatusDriver.xOpenLoadA;
    sDriverStatus.xOpenLoadB = lsStatusDriver.xOpenLoadB;
    sDriverStatus.xOvertemperatureWarning = lsStatusDriver.xOvertemperatureWarning;
    sDriverStatus.xOvertemperatureShutdown = lsStatusDriver.xOvertemperatureShutdown;;
    sDriverStatus.iStallGuardResult = lsStatusDriver.iStallGuardResult;

    updateTimers();
    debugStepChange();
}

void FB_Comm_TMC2209::updateTimers() {
    // Timer call
    if (liStep != liStepOld) {
        TonStep.IN(false);
        TonStepF.IN(false);
    }

    TonStep.run();
    TonStep.IN(false);

    TonStepF.run();
    TonStepF.IN(false);
}

void FB_Comm_TMC2209::debugStepChange() {
    if (liStep != liStepOld) {
        liStepOld = liStep;
        lxEntryAction = true;
        debug("FB_Comm_TMC2209 - Aktueller Schritt: ");
        debugln(liStep);
    }
}
