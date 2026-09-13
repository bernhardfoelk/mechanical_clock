//
// Created by bernhard on 10.09.26.
//

#include "StepperDriverModulo.h"
#include <math.h>

StepperDriverModulo::StepperDriverModulo(uint8_t iiStepPin, uint8_t iiDirPin, uint8_t iiRefPin,
                                         long ilStepsPerRotation)
    : fbStepper(AccelStepper::DRIVER, iiStepPin, iiDirPin),
      liRefPin(iiRefPin),
      llOffsetToReference(0),
      llStepsPerRotation(ilStepsPerRotation),
      lrTargetPosition(0.0f),
      lrSpeed(400.0f),
      lrReferenceSpeed(150.0f),
      lxStart(false),
      lxStartReference(false),
      lxFinished(true),
      lxReferenced(false),
      liStep(0) {
    fbStepper.setMaxSpeed(lrSpeed);
    fbStepper.setAcceleration(800.0f);

    pinMode(liRefPin, INPUT_PULLUP);
}

// ---------------- Eingänge ----------------

void StepperDriverModulo::setTargetPosition(float irValue) {
    lrTargetPosition = irValue;
}

void StepperDriverModulo::setStart() {
    lxStart = true;

    lxFinished = false;
    lxReferenced = false;
}

void StepperDriverModulo::setStartReference() {
    lxStartReference = true;

    lxFinished = false;
    lxReferenced = false;
}

// ---------------- Parametrierung ----------------

void StepperDriverModulo::setSpeed(float irValue) {
    lrSpeed = irValue;
}

void StepperDriverModulo::setAcceleration(float irValue) {
    fbStepper.setAcceleration(irValue);
}

void StepperDriverModulo::setSpeedReference(float irValue) {
    lrReferenceSpeed = irValue;
}

void StepperDriverModulo::setOffsetToReference(long ilValue) {
    llOffsetToReference = ilValue;
}

// ---------------- zyklischer Aufruf ----------------

void StepperDriverModulo::run() {
    EdgeStart.run(lxStart);
    EdgeStartReference.run(lxStartReference);
    EdgeReferenceSensor.run(digitalRead(liRefPin));

    switch (liStep) {
        case 0:
            fbStepper.setCurrentPosition(fbStepper.currentPosition());

            lxStart = false;
            lxStartReference = false;
            lxFinished = false;

            lxReady = false;

            liStep = 20;
            break;

        case 20:
            lxReady = true;

            if (EdgeStartReference.EdgePos()) {
                // set reference speed
                fbStepper.setMaxSpeed(lrReferenceSpeed);
                fbStepper.setSpeed(lrReferenceSpeed);

                lxStartReference = false;

                liStep = 200;
            } else if (EdgeStart.EdgePos()) {
                llTargetStep = DegreeToSteps(lrTargetPosition);

                lxStart = false;

                liStep = 100;
            }
            break;

        case 100:
            fbStepper.setMaxSpeed(lrSpeed);
            fbStepper.moveTo(llTargetStep);
            fbStepper.run();

            if (fbStepper.distanceToGo() == 0) {
                lxFinished = true;
                liStep = 0;
            }
            break;

        case 200:
            fbStepper.runSpeed(); // konstante Geschwindigkeit, ohne Rampe
            if (EdgeReferenceSensor.EdgeNeg()) {
                fbStepper.setSpeed(0);
                fbStepper.setCurrentPosition(llOffsetToReference);

                lxReferenced = true;

                liStep = 210;
            }
            break;

        case 210:
            fbStepper.setMaxSpeed(lrSpeed);
            fbStepper.moveTo(0);
            fbStepper.run();

            if (fbStepper.distanceToGo() == 0) {
                lxFinished = true;
                liStep = 0;
            }
            break;
    }
}

void StepperDriverModulo::reset() {
    liStep = 0;
}

// ---------------- Ausgänge ----------------

bool StepperDriverModulo::isFinished() const {
    return lxFinished;
}

bool StepperDriverModulo::isReady() const {
    return lxReady;
}

bool StepperDriverModulo::isReferenced() const {
    return lxReferenced;
}

// ---------------- intern ----------------

long StepperDriverModulo::DegreeToSteps(float irValue) {
    float trDegreeModulo = fmod(irValue, 360.0f);
    if (trDegreeModulo < 0.0f) trDegreeModulo += 360.0f;

    long tlTargetInRevolution = (long) lround((trDegreeModulo / 360.0f) * (float) llStepsPerRotation);
    tlTargetInRevolution %= llStepsPerRotation;

    long tlCurrentStep = fbStepper.currentPosition();
    long tlCurrentInRevolution = ((tlCurrentStep % llStepsPerRotation) + llStepsPerRotation) % llStepsPerRotation;

    long tlDelta = tlTargetInRevolution - tlCurrentInRevolution;
    if (tlDelta < 0) tlDelta += llStepsPerRotation; // immer positiv = vorwärts

    return tlCurrentStep + tlDelta;
}
