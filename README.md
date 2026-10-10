# 🕰️ Mechanical Digit Clock

A mechanical HH:MM:SS wall clock with one stepper-driven digit wheel per character, each referenced by its own Hall sensor. The firmware runs on an ESP32-S3 and is structured like an industrial PLC program: non-blocking function blocks with explicit step chains, no `delay()`.

![Clock front](images/Clock_front.png)
![Clock back](images/Clock_back.png)

## Status

| Part | State |
|---|---|
| 6-channel PCB, v9 (4 layers, 308 × 62 mm) | designed, source files in `pcb/` |
| Firmware, one digit channel (seconds, ones digit) | runs on the one-motor prototype |
| Firmware, all six channels | next step: one `FB_Motors` instance per digit |

![Prototype with one motor](images/Prototype_OneMotor.png)

## The engineering behind it

A digital clock is easy. Making mechanical digit wheels land on the correct number every second, without a single `delay()` blocking the others, is the actual problem this project solves.

* **Function-block architecture:** The firmware is built from self-contained, non-blocking function blocks (`FB_Clock`, `FB_Motors`, ...) in an IEC 61131-3 style. Each block runs its own step chain and communicates only through small structs (`TimestampForMotors`, `TMCDriverStatus`). Same pattern as in [filament_buffer](https://github.com/bernhardfoelk/filament_buffer).
* **PLC-style utilities:** Timing and edge detection use my own `Ton` (on-delay) and `EdgePosNeg` classes instead of `delay()` or ad-hoc flags.
* **`StepperDriverModulo`:** A wrapper around `AccelStepper` that turns absolute angle targets (0–360°) into step counts modulo one revolution. The wheel only ever moves forward, so a digit never has to reverse to reach the next number.
* **Microstep-independent motion:** Speed and acceleration are defined once as full-step values and scaled by the active microstepping factor, so the angular speed stays constant when the resolution changes.
* **Hardware journey, UART vs. STEP/DIR:** I planned to run the TMC2209 drivers over single-wire UART for diagnostics (StallGuard, temperature, connection state). After ruling out wiring, addressing and driver faults, I fell back to plain STEP/DIR with microstepping set via the MS1/MS2 pins. The UART diagnostics block (`FB_Comm_TMC2209`) stays in the repo, disabled.

## Hardware

Custom 4-layer PCB (308 × 62 mm), designed in Fusion Electronics. Source in `pcb/`.

* ESP32-S3-PICO, DS3231 RTC module, 0.96" I²C OLED, two buttons
* 6× TMC2209 stepper drivers (STEP/DIR), 6× JST-XH 4-pin motor connectors
* 6× A3144 Hall sensors for homing, one per digit wheel
* USB-PD power input (PDC004-PD) and a DSN-MINI-360 buck converter
* NEMA steppers, 200 steps/rev, direct drive, currently 1/64 microstepping

## Project structure

```
src/main.cpp                 instantiates the function blocks, runs them each scan cycle
src/app_config.h             pin mapping and build-time configuration
src/POUs/00_Clock/           RTC, OLED, buttons, time setting
src/POUs/01_Motors/          per-digit positioning
src/POUs/_Comm/              TMC2209 UART diagnostics (currently disabled)
src/DUT's/Interface/         shared data structures between blocks
lib/StepperDriverModulo/     modulo-aware positioning on top of AccelStepper
lib/Ton, lib/EdgePosNeg/     PLC-style timer and edge detection
pcb/                         board design (Fusion Electronics)
```

## Build

PlatformIO, environment `esp32-s3-devkitc-1`: `pio run -t upload`. Pins are set in `src/app_config.h`.

## Known limitations

* The step counter is a 32-bit value that only counts up. After about 19 days of continuous running it wraps around and the wheel is out of phase until the next restart. Fix: renormalize the position modulo one revolution while idle.
* Homing has no timeout and no error state. A missing magnet or a broken sensor makes the motor spin forever.
* `AccelStepper` produces pulses only while `run()` is called. A blocking I²C display refresh in the same loop can stall them. Next step: measure the worst-case cycle time and update the display only when the second changes.

## License

MIT
