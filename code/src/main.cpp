#include "app_config.h"
#include "DUT's/Interface/Interface.h"
#include "POUs//00_Clock/FB_Clock.h"
#include "POUs/01_Motors/FB_Motors.h"

// -------------------------------------------------------------
// Global variables (only used in this file)
// -------------------------------------------------------------

// General --> Definition der FB's/Klassen
static FB_Clock fbClock;
static FB_Motors fbMotors;

// Interface variables
static TimestampForMotors sTimeStamp;

// -------------------------------------------------------------
// Arduino setup / loop
// -------------------------------------------------------------

void setup() {
    // Start serial interface
    Serial.begin(9600);
    delay(500);

    debugln("Debugging started..");
}

void loop() {
    fbClock.run(sTimeStamp);
    fbMotors.run(sTimeStamp);
}
