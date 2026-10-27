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
#define PIN_MOTOR_1_STEP 5
#define PIN_MOTOR_1_DIR 6
#define PIN_MOTOR_1_HALLSENSOR_DATA 9
#define PIN_MOTOR_2_STEP 2
#define PIN_MOTOR_2_DIR 4
#define PIN_MOTOR_2_HALLSENSOR_DATA 8
#define PIN_MOTOR_3_STEP 41
#define PIN_MOTOR_3_DIR 1
#define PIN_MOTOR_3_HALLSENSOR_DATA 7
#define PIN_MOTOR_4_STEP 40
#define PIN_MOTOR_4_DIR 38
#define PIN_MOTOR_4_HALLSENSOR_DATA 14
#define PIN_MOTOR_5_STEP 37
#define PIN_MOTOR_5_DIR 36
#define PIN_MOTOR_5_HALLSENSOR_DATA 13
#define PIN_MOTOR_6_STEP 35
#define PIN_MOTOR_6_DIR 34
#define PIN_MOTOR_6_HALLSENSOR_DATA 12

#define PIN_I2C_SDA 17
#define PIN_I2C_SCL 18

#define PIN_BUTTON_MENU 10
#define PIN_BUTTON_SETTING 11
