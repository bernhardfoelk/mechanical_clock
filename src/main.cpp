#include "app_config.h"
#include "DUT's/Interface/Interface.h"
#include "POUs/_Comm/FB_Comm_TMC2209.h"
#include "POUs//00_Clock/FB_Clock.h"
#include "POUs/01_Motors/FB_Motors.h"

// -------------------------------------------------------------
// Global variables (only used in this file)
// -------------------------------------------------------------

// General --> Definition der FB's/Klassen
//static FB_Comm_TMC2209 fbCommTMC2209(Serial1, PIN_UART1_RX, PIN_UART1_TX, 0.11);
static FB_Clock fbClock;
static FB_Motors fbMotors;

// Interface variables
static TimestampForMotors sTimeStamp;
//static TMCDriverStatus sDriverStatus;

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
    //fbCommTMC2209.run(sDriverStatus);

    fbClock.run(sTimeStamp);
    fbMotors.run(sTimeStamp);
}
