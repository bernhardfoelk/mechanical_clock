//
// Created by bernhard on 12.09.26.
//

#ifndef FB_COMM_TMC2209_H
#define FB_COMM_TMC2209_H

#include "TMCStepper.h"

#include "Ton.h"
#include "EdgePosNeg.h"
#include "app_config.h"
#include "StepperDriverModulo.h"

struct TMCDriverStatus {
    bool xConnected = false;
    bool xOpenLoadA = false;
    bool xOpenLoadB = false;
    bool xOvertemperatureWarning = false;
    bool xOvertemperatureShutdown = false;
    int iStallGuardResult = 0;
};

class FB_Comm_TMC2209 {
public:
    FB_Comm_TMC2209(HardwareSerial &isSerial, const int iiUartRxPin, const int iiUartTxPin,
                    const float irRSense);

    void run(TMCDriverStatus &sStatusDriver);

private:
    // inputs
    int liUartRxPin;
    int liUartTxPin;
    float lrRSense;

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

    const int liRunCurrentMa;
    const int liHoldCurrentPercent;
    const int liMicrosteps;
    const bool lxStealthChop;

    HardwareSerial &lsSerial;
    TMC2209Stepper driver;
    TMCDriverStatus lsStatusDriver;

    void updateTimers();

    void updateEdges();

    void debugStepChange();
};

#endif //FB_COMM_TMC2209_H
