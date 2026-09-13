//
// Created by bernhard on 11.09.26.
//

#ifndef FB_MOTORS_H
#define FB_MOTORS_H

#include "Ton.h"
#include "EdgePosNeg.h"
#include "app_config.h"
#include "StepperDriverModulo.h"
#include "DUT's/Interface/Interface.h"
#include "POUs/_Comm/FB_Comm_TMC2209.h"

class FB_Motors {
public:
    FB_Motors();

    void run(TimestampForMotors &sTimestamp);

private:
    // inputs

    // outputs

    // local
    int liStep;
    int liNStep;
    int liStepOld;
    bool lxEntryAction;
    bool lxTemp;
    int liTemp;

    int liMicrostepping;
    float lrSpeed;
    float lrReferenceSpeed;
    float lrAcceleration;

    Ton TonStep;
    Ton TonStepF;

    TimestampForMotors lsTimestampOld;

    StepperDriverModulo motor;

    void updateTimers();

    void updateEdges();

    void debugStepChange();
};


#endif //FB_MOTORS_H
