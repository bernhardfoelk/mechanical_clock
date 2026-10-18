//
// Created by bernhard on 11.09.26.
//

#include "../01_Motors/FB_Motors.h"
#include "Arduino.h"

FB_Motors::FB_Motors()
    : liStep(0),
      liNStep(0),
      liStepOld(0),
      lxEntryAction(false),
      lxTemp(false),
      liTemp(0),
      liMicrostepping(64),
      // values for 1/1 step configuration
      lrSpeed(625 * liMicrostepping),
      lrReferenceSpeed(325 * liMicrostepping),
      lrAcceleration(1250 * liMicrostepping),
      motor(PIN_MOTOR_STEP,
            PIN_MOTOR_DIR,
            PIN_HALLSENSOR_DATA,
            200 * liMicrostepping) {
}

void FB_Motors::run(TimestampForMotors &sTimestamp) {
    switch (liStep) {
        //***************************************
        // Reset
        //***************************************
        case 0:
            if (lxEntryAction) {
                lxEntryAction = false;
            }

            if (true) {
                motor.reset();
                debugln("FB_Motor - Motor reset.");
                liStep = 4;
            }
            break;

        case 4:
            if (lxEntryAction) {
                lxEntryAction = false;
            }

            if (motor.isReady()) {
                debugln("FB_Motor - Motor bereit.");

                liStep = 8;
            }
            break;

        case 8:
            if (lxEntryAction) {
                motor.setSpeed(lrSpeed);
                motor.setAcceleration(lrAcceleration);

                motor.setSpeedReference(lrReferenceSpeed);
                motor.setOffsetToReference(0);

                lxEntryAction = false;
            }

            if (true) {
                liStep = 20;
                if (!motor.isReferenced()) {
                    debugln("FB_Motor - Motor referenzieren.");
                    liStep = 12;
                }
            }
            break;

        case 12:
            if (lxEntryAction) {
                motor.setStartReference();

                lxEntryAction = false;
            }

            if (motor.isReferenced() && motor.isFinished()) {
                debugln("FB_Motor - Motor referenziert.");

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

            if ((sTimestamp.iHour != lsTimestampOld.iHour) ||
                (sTimestamp.iMinute != lsTimestampOld.iMinute) ||
                (sTimestamp.iSecond != lsTimestampOld.iSecond)) {
                liStep = 1000; // Position stepper
            }
            break;


        //***************************************
        // Position stepper
        //***************************************
        case 1000:
            if (lxEntryAction) {
                // Motors for second
                motor.setSpeed(lrSpeed);
                motor.setAcceleration(lrAcceleration);
                //motor.setTargetPosition((sTimestamp.iSecond / 10) * 36); // Zehnerstelle, z.B. 14 / 10 = 1
                motor.setTargetPosition((sTimestamp.iSecond % 10) * 36); // Einerstelle,  z.B. 14 % 10 = 4

                motor.setStart();

                lxEntryAction = false;
            }

            if (motor.isFinished()) {
                lsTimestampOld = sTimestamp;

                liStep = 20;
            }
            break;
    }

    motor.run();

    updateTimers();
    debugStepChange();
}

void FB_Motors::updateTimers() {
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

void FB_Motors::debugStepChange() {
    if (liStep != liStepOld) {
        liStepOld = liStep;
        lxEntryAction = true;
        debug("FB_Motors - Aktueller Schritt: ");
        debugln(liStep);
    }
}
