/*
 * config.h - pins, constants and shared state for the active ankle controller.
 *
 * Ported from the 2021 Arduino IDE sketch with no change in behaviour. The
 * original .ino files are archived under firmware/arduino-ide-originals/.
 *
 * The .ino format put every global in one file and let the IDE generate the
 * function prototypes. Splitting the tasks into separate translation units
 * means the globals are declared extern here and defined once in main.cpp.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <AS5600.h>
#include <HX711_ADC.h>
#include <PID_v1.h>
#include <TridentTD_EasyFreeRTOS32.h>
#include <WiFi.h>
#include <Wire.h>

/* ---- Motor PWM (ESP32 LEDC) ---------------------------------------------
 * LEDC_TIMER_13_BIT is the resolution argument passed to ledcSetup(). Despite
 * the name it is set to 8, so duty runs 0..255.
 */
#define LEDC_CHANNEL_0 0
#define LEDC_TIMER_13_BIT 8
#define LEDC_BASE_FREQ 5000
#define LED_PIN 13 /* PWM output to the Cytron MD10C */
#define LED_Dir 14 /* direction output to the Cytron MD10C */

#define operation_pin 0 /* start/stop, driven by the WiFi task */

/* ---- HX711 load cell pins ------------------------------------------------
 * Cells 1 and 2 sit under the toe, cells 3 and 4 under the heel.
 */
extern const int HX711_dout_1;
extern const int HX711_sck_1;
extern const int HX711_dout_2;
extern const int HX711_sck_2;
extern const int HX711_dout_3;
extern const int HX711_sck_3;
extern const int HX711_dout_4;
extern const int HX711_sck_4;

extern HX711_ADC LoadCell_1;
extern HX711_ADC LoadCell_2;
extern HX711_ADC LoadCell_3;
extern HX711_ADC LoadCell_4;

extern const int calVal_eepromAdress_1;
extern const int calVal_eepromAdress_2;

/* ---- Encoder -------------------------------------------------------------
 * AS5600 is 12-bit, so 4096 counts per revolution. The controller runs on raw
 * counts, not degrees: multiply by 360/4096 (about 0.0879) to get degrees.
 * ZeroClaib is the measured offset of the assembled joint.
 */
extern AMS_5600 ams5600;
extern int last_raw;
extern int raw_angle;
extern int raw;
extern int revolution;
extern int ZeroClaib;
extern float ang;

extern long NewAngle;
extern long OldAngle;
extern long D_angle;

/* ---- Trajectory ----------------------------------------------------------
 * 101 samples over one gait cycle, in raw encoder counts.
 */
extern int WalkingTraj[101];
extern float target;
extern float p;

/* ---- Gait state ----------------------------------------------------------
 * threshold is in load cell units; flip1..flip4 latch each phase so the
 * trajectory segment for a phase plays once per step.
 */
extern bool toe_state;
extern bool heel_state;
extern int threshold;
extern int flip1, flip2, flip3, flip4;
extern unsigned long t;

/* ---- PID -----------------------------------------------------------------
 * Gains are set at run time over serial; the constructor values are
 * placeholders. Tuned values from testing: Kp 0.14, Ki 0.2, Kd 0.
 */
extern const double Kp;
extern const double Ki;
extern const double Kd;
extern double Setpoint, Input, Output;
extern PID myPID;

/* ---- WiFi ---------------------------------------------------------------- */
extern const char *ssid;
extern const char *password;
extern WiFiServer server;

/* ---- Tasks --------------------------------------------------------------- */
void TrajGen(void *);
void Communication(void *);
void read_ANGLE(void *);
void pid(void *);
void myblink(void *);
void toe_cells(void *);
void heel_cells(void *);
void wifi_communication(void *);

extern TridentOS task1, task2, task3, task4, task5, task6, task7, task8;

#endif /* CONFIG_H */
