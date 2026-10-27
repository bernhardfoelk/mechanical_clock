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
      //lrSpeed(625 * liMicrostepping),
      lrSpeed(500 * liMicrostepping),
      lrReferenceSpeed(60 * liMicrostepping),
      lrAcceleration(1250 * liMicrostepping),
      motor1(1,
             PIN_MOTOR_1_STEP,
             PIN_MOTOR_1_DIR,
             PIN_MOTOR_1_HALLSENSOR_DATA,
             200 * liMicrostepping),
      motor2(2,
             PIN_MOTOR_2_STEP,
             PIN_MOTOR_2_DIR,
             PIN_MOTOR_2_HALLSENSOR_DATA,
             200 * liMicrostepping),
      motor3(3,
             PIN_MOTOR_3_STEP,
             PIN_MOTOR_3_DIR,
             PIN_MOTOR_3_HALLSENSOR_DATA,
             200 * liMicrostepping),
      motor4(4,
             PIN_MOTOR_4_STEP,
             PIN_MOTOR_4_DIR,
             PIN_MOTOR_4_HALLSENSOR_DATA,
             200 * liMicrostepping),
      motor5(5,
             PIN_MOTOR_5_STEP,
             PIN_MOTOR_5_DIR,
             PIN_MOTOR_5_HALLSENSOR_DATA,
             200 * liMicrostepping),
      motor6(6,
             PIN_MOTOR_6_STEP,
             PIN_MOTOR_6_DIR,
             PIN_MOTOR_6_HALLSENSOR_DATA,
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
                motor1.reset();
                motor2.reset();
                motor3.reset();
                motor4.reset();
                motor5.reset();
                motor6.reset();
                debugln("FB_Motor - Motor reset.");
                liStep = 4;
            }
            break;

        case 4:
            if (lxEntryAction) {
                lxEntryAction = false;
            }

            if (motor1.isReady() && motor2.isReady() && motor3.isReady() &&
                motor4.isReady() && motor5.isReady() && motor6.isReady()) {
                debugln("FB_Motor - Motor bereit.");

                liStep = 8;
            }
            break;

        case 8:
            if (lxEntryAction) {
                motor1.setSpeed(lrSpeed);
                motor1.setAcceleration(lrAcceleration);
                motor1.setSpeedReference(lrReferenceSpeed);
                motor1.setOffsetToReference(354.5);
                motor1.setInvertDirection(false);

                motor2.setSpeed(lrSpeed);
                motor2.setAcceleration(lrAcceleration);
                motor2.setSpeedReference(lrReferenceSpeed);
                motor2.setOffsetToReference(355.5);
                motor2.setInvertDirection(true);

                motor3.setSpeed(lrSpeed);
                motor3.setAcceleration(lrAcceleration);
                motor3.setSpeedReference(lrReferenceSpeed);
                motor3.setOffsetToReference(354.5);
                motor3.setInvertDirection(false);

                motor4.setSpeed(lrSpeed);
                motor4.setAcceleration(lrAcceleration);
                motor4.setSpeedReference(lrReferenceSpeed);
                motor4.setOffsetToReference(355.0);
                motor4.setInvertDirection(true);

                motor5.setSpeed(lrSpeed);
                motor5.setAcceleration(lrAcceleration);
                motor5.setSpeedReference(lrReferenceSpeed);
                motor5.setOffsetToReference(354.5);
                motor5.setInvertDirection(false);

                motor6.setSpeed(lrSpeed);
                motor6.setAcceleration(lrAcceleration);
                motor6.setSpeedReference(lrReferenceSpeed);
                motor6.setOffsetToReference(353.5);
                motor6.setInvertDirection(true);

                lxEntryAction = false;
            }

            if (true) {
                liStep = 20;
                if (!motor1.isReferenced()) {
                    motor1.setStartReference();
                    debugln("FB_Motor - Motor 1 referenzieren.");
                    liStep = 12;
                }
                if (!motor2.isReferenced()) {
                    motor2.setStartReference();
                    debugln("FB_Motor - Motor 2 referenzieren.");
                    liStep = 12;
                }
                if (!motor3.isReferenced()) {
                    motor3.setStartReference();
                    debugln("FB_Motor - Motor 3 referenzieren.");
                    liStep = 12;
                }
                if (!motor4.isReferenced()) {
                    motor4.setStartReference();
                    debugln("FB_Motor - Motor 4 referenzieren.");
                    liStep = 12;
                }
                if (!motor5.isReferenced()) {
                    motor5.setStartReference();
                    debugln("FB_Motor - Motor 5 referenzieren.");
                    liStep = 12;
                }
                if (!motor6.isReferenced()) {
                    motor6.setStartReference();
                    debugln("FB_Motor - Motor 6 referenzieren.");
                    liStep = 12;
                }
            }
            break;

        case 12:
            if (lxEntryAction) {
                lxEntryAction = false;
            }

            if (motor1.isReferenced() && motor1.isFinished() &&
                motor2.isReferenced() && motor2.isFinished() &&
                motor3.isReferenced() && motor3.isFinished() &&
                motor4.isReferenced() && motor4.isFinished() &&
                motor5.isReferenced() && motor5.isFinished() &&
                motor6.isReferenced() && motor6.isFinished()) {
                debugln("FB_Motor - Motor referenziert.");

                liStep = 14;
            }
            break;

        case 14:
            if (lxEntryAction) {
                lxEntryAction = false;
            }

            TonStep.IN(true);
            TonStep.PT(1000);

            if (TonStep.Q()) {
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
                motor1.setSpeed(lrSpeed);
                motor1.setAcceleration(lrAcceleration);
                motor1.setTargetPosition(static_cast<float>(sTimestamp.iSecond % 10) * 36);
                // Einerstelle,  z.B. 14 % 10 = 4
                motor1.setStart();
                motor2.setSpeed(lrSpeed);
                motor2.setAcceleration(lrAcceleration);
                motor2.setTargetPosition(static_cast<float>(sTimestamp.iSecond / 10) * 36);
                // Zehnerstelle, z.B. 14 / 10 = 1
                motor2.setStart();
                // Motors for minute
                motor3.setSpeed(lrSpeed);
                motor3.setAcceleration(lrAcceleration);
                motor3.setTargetPosition(static_cast<float>(sTimestamp.iMinute % 10) * 36);
                // Einerstelle,  z.B. 14 % 10 = 4
                motor3.setStart();
                motor4.setSpeed(lrSpeed);
                motor4.setAcceleration(lrAcceleration);
                motor4.setTargetPosition(static_cast<float>(sTimestamp.iMinute / 10) * 36);
                // Zehnerstelle, z.B. 14 / 10 = 1
                motor4.setStart();
                // Motors for hour
                motor5.setSpeed(lrSpeed);
                motor5.setAcceleration(lrAcceleration);
                motor5.setTargetPosition(static_cast<float>(sTimestamp.iHour % 10) * 36);
                // Einerstelle,  z.B. 14 % 10 = 4
                motor5.setStart();
                motor6.setSpeed(lrSpeed);
                motor6.setAcceleration(lrAcceleration);
                motor6.setTargetPosition(static_cast<float>(sTimestamp.iHour / 10) * 36);
                // Zehnerstelle, z.B. 14 / 10 = 1
                motor6.setStart();

                lxEntryAction = false;
            }

            if (motor1.isFinished() && motor2.isFinished() && motor3.isFinished() &&
                motor4.isFinished() && motor5.isFinished() && motor6.isFinished()) {
                lsTimestampOld = sTimestamp;

                liStep = 20;
            }
            break;
    }

    motor1.run();
    motor2.run();
    motor3.run();
    motor4.run();
    motor5.run();
    motor6.run();

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
