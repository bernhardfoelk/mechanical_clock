/**
 *
 * @brief Configuration file for all parameters
 *
 * @file app_config.h
 * @author Bernhard Fölk
 * @date 19.10.2025
 *
 */

#pragma once

#include <Arduino.h>

#include "Ton.h"
#include "Tof.h"
#include "EdgePosNeg.h"

#define DEBUGGING true

#if DEBUGGING == true
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

// IOs
#define PIN_MOTOR_STEP 35
#define PIN_MOTOR_DIR 34
#define PIN_HALLSENSOR_DATA 8 //7

#define PIN_I2C_SDA 17
#define PIN_I2C_SCL 18

#define PIN_BUTTON_MENU 10
#define PIN_BUTTON_SETTING 11
