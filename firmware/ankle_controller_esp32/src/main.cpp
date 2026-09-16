/*
 * main.cpp - global definitions and startup for the active ankle controller.
 *
 * Ported from shimi.ino (2021). Behaviour is unchanged: the same tasks start
 * in the same order with the same parameters.
 */

#include "config.h"

/* ---- HX711 load cell pins ---- */
const int HX711_dout_1 = 17;
const int HX711_sck_1 = 16;
const int HX711_dout_2 = 32;
const int HX711_sck_2 = 33;
const int HX711_dout_3 = 25;
const int HX711_sck_3 = 26;
const int HX711_dout_4 = 19;
const int HX711_sck_4 = 18;

HX711_ADC LoadCell_1(HX711_dout_1, HX711_sck_1);
HX711_ADC LoadCell_2(HX711_dout_2, HX711_sck_2);
HX711_ADC LoadCell_3(HX711_dout_3, HX711_sck_3);
HX711_ADC LoadCell_4(HX711_dout_4, HX711_sck_4);

const int calVal_eepromAdress_1 = 0;
const int calVal_eepromAdress_2 = 4;

/* ---- Encoder ---- */
AMS_5600 ams5600;
int last_raw = 0;
int raw_angle = 0;
int raw = 0;
int revolution = 0;
int ZeroClaib = 1629 + 221 - 559 - 43;
float ang = 0;

long NewAngle = 0;
long OldAngle = 0;
long D_angle = 0;

/* ---- Trajectory: one gait cycle, 101 samples, raw encoder counts ---- */
int WalkingTraj[101] = {
    14,  13,  14,  16,  18,  21,  24,  28,  32,  38,  44,  51,  58,  65,  73,
    80,  87,  93,  100, 105, 111, 115, 120, 124, 128, 132, 135, 138, 141, 143,
    145, 148, 149, 151, 153, 154, 155, 156, 156, 156, 155, 154, 151, 146, 141,
    133, 124, 113, 100, 85,  70,  53,  35,  18,  0,   -16, -31, -45, -56, -64,
    -70, -73, -74, -72, -68, -61, -53, -44, -33, -22, -11, 0,   11,  22,  31,
    40,  48,  55,  61,  66,  69,  72,  73,  74,  74,  73,  71,  69,  67,  64,
    61,  57,  53,  49,  45,  40,  35,  30,  25,  20,  14};

float target = 0;
float p = 0;

/* ---- Gait state ---- */
bool toe_state = false;
bool heel_state = false;
int threshold = 400;
int flip1 = 1, flip2 = 1, flip3 = 1, flip4 = 1;
unsigned long t = 0;

/* ---- PID ---- */
const double Kp = 0.0, Ki = 0.0, Kd = 0.0;
double Setpoint, Input, Output;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

/* ---- WiFi ---- */
const char *ssid = "YOUR_SSID";
const char *password = "YOUR_PASSWORD";
WiFiServer server(80);

/* ---- Tasks ---- */
TridentOS task1, task2, task3, task4, task5, task6, task7, task8;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinMode(LED_Dir, OUTPUT);

  task4.start(pid);
  task1.start(TrajGen);
  task3.start(read_ANGLE);
  task2.start(Communication);
  task6.start(toe_cells);
  task7.start(heel_cells);
  /* task5 (myblink) and task8 (wifi_communication) are build-time optional;
   * they were used for bring-up and remote start/stop testing. */

  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);
}

void loop() { DELAY(1); }
