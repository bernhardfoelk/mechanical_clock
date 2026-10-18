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
      lxSetReference(false),
      lxReady(false),
      lxFinished(true),
      lxReferenced(false),
      liStep(0),
      llTargetStep(0),
      llReferencePosition(0),
      llModuloStepsPerRotation(0),
      llDifferenceTargetToCurrent(0) {
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
}

void StepperDriverModulo::setStartReference() {
    lxStartReference = true;

    lxFinished = false;
    lxReferenced = false;
}

void StepperDriverModulo::setReference(long ilValue) {
    lxSetReference = true;

    lxFinished = false;
    lxReferenced = false;

    llReferencePosition = ilValue;
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

            lxReady = false;

            liStep = 20;
            break;

        case 20:
            lxReady = true;

            if (EdgeStartReference.EdgePos()) {
                lxStartReference = false;

                liStep = 200;
            } else if (lxSetReference) {
                lxSetReference = false;

                liStep = 300;
            } else if (EdgeStart.EdgePos()) {
                // calculate target step
                llTargetStep = DegreeToSteps(lrTargetPosition);
                llDifferenceTargetToCurrent = llTargetStep - fbStepper.currentPosition();

                if (llDifferenceTargetToCurrent < 0) {
                    llDifferenceTargetToCurrent = llDifferenceTargetToCurrent + llStepsPerRotation;
                }
                llTargetStep = fbStepper.currentPosition() + llDifferenceTargetToCurrent;

                lxStart = false;

                liStep = 100;
            }
            break;

        // Position motor
        case 100:
            fbStepper.setMaxSpeed(lrSpeed);
            fbStepper.moveTo(llTargetStep);
            fbStepper.run();

            if (fbStepper.distanceToGo() == 0) {
                // calculate modulo steps and set the current position to a value between 0 and 360
                llModuloStepsPerRotation = fbStepper.currentPosition() % llStepsPerRotation;
                if (llModuloStepsPerRotation < 0) {
                    llModuloStepsPerRotation += llStepsPerRotation;
                }
                fbStepper.setCurrentPosition(llModuloStepsPerRotation);

                lxFinished = true;
                liStep = 0;
            }
            break;

        // Reference motor
        case 200:
            fbStepper.setMaxSpeed(lrReferenceSpeed);
            fbStepper.setSpeed(lrReferenceSpeed);
            fbStepper.runSpeed(); // konstante Geschwindigkeit, ohne Rampe
            if (EdgeReferenceSensor.EdgeNeg()) {
                fbStepper.setSpeed(0);
                fbStepper.setCurrentPosition(0);

                liStep = 210;
            }
            break;

        case 210:

            fbStepper.setMaxSpeed(lrReferenceSpeed);
            fbStepper.moveTo(DegreeToSteps(-10));
            fbStepper.run();

            if (fbStepper.distanceToGo() == 0) {
                liStep = 220;
            }
            break;

        case 220:
            fbStepper.setMaxSpeed(lrReferenceSpeed / 10);
            fbStepper.setSpeed(lrReferenceSpeed / 10);
            fbStepper.runSpeed(); // konstante Geschwindigkeit, ohne Rampe
            if (EdgeReferenceSensor.EdgeNeg()) {
                fbStepper.setSpeed(0);
                fbStepper.setCurrentPosition(llOffsetToReference);

                liStep = 230;
            }
            break;

        case 230:
            fbStepper.setMaxSpeed(lrSpeed);
            fbStepper.moveTo(0);
            fbStepper.run();

            if (fbStepper.distanceToGo() == 0) {
                lxReferenced = true;
                lxFinished = true;
                liStep = 0;
            }
            break;

        // Set Position
        case 300:

            fbStepper.setCurrentPosition(llReferencePosition);

            lxFinished = true;

            liStep = 0;
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
    float trStepsPerRotation = static_cast<float>(llStepsPerRotation);
    float trStepsPerDegree = (trStepsPerRotation / 360.0f);

    float trStepsToTake = irValue * trStepsPerDegree;
    long tlStepsToTake = static_cast<long>(trStepsToTake);

    return tlStepsToTake;
}

float StepperDriverModulo::StepToDegrees(long ilValue) {
    float trTemp = 0.0f;
    float trValue = static_cast<float>(ilValue);
    float trStepsPerRotation = static_cast<float>(llStepsPerRotation);

    if (llStepsPerRotation == 0) {
        trTemp = 0;
    } else {
        trTemp = ((trValue * 360.0f) / trStepsPerRotation);
    }
    return trTemp;
}
