
#include <AS5600.h>
#include <TridentTD_EasyFreeRTOS32.h>
#include <Wire.h>
#include <PID_v1.h>

#define LEDC_CHANNEL_0     0
#define LEDC_TIMER_13_BIT  8
#define LEDC_BASE_FREQ     5000
#define LED_PIN            13
#define LED_Dir            14
/////// 


#define operation_pin  0

#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

#include <WiFi.h>

const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

WiFiServer server(80);


//pins:

const int HX711_dout_1 = 17; //mcu > HX711 no 1 dout pin
const int HX711_sck_1 = 16; //mcu > HX711 no 1 sck pin

const int HX711_dout_2 = 32; //mcu > HX711 no 2 dout pin
const int HX711_sck_2 = 33; //mcu > HX711 no 2 sck pin

const int HX711_dout_3 = 25; //mcu > HX711 no 2 dout pin
const int HX711_sck_3= 26; //mcu > HX711 no 2 sck pin

const int HX711_dout_4 = 19; //mcu > HX711 no 2 dout pin
const int HX711_sck_4 = 18; //mcu > HX711 no 2 sck pin

//HX711 constructor (dout pin, sck pin)
HX711_ADC LoadCell_1(HX711_dout_1, HX711_sck_1); //HX711 1
HX711_ADC LoadCell_2(HX711_dout_2, HX711_sck_2); //HX711 2
HX711_ADC LoadCell_3(HX711_dout_3, HX711_sck_3); //HX711 3
HX711_ADC LoadCell_4(HX711_dout_4, HX711_sck_4); //HX711 4

const int calVal_eepromAdress_1 = 0; // eeprom adress for calibration value load cell 1 (4 bytes)
const int calVal_eepromAdress_2 = 4; // eeprom adress for calibration value load cell 2 (4 bytes)
unsigned long t = 0;
 bool toe_state = false;
 bool heel_state = false;
 

TridentOS   task1, task2, task3, task4, task5 ,task6,task7,task8;  // สร้าง task กี่ตัวก็ใส่ไป

void pid(void*), Communication(void*), read_ANGLE(void*), TrajGen(void*), myblink(void*),toe_cells(void*),heel_cells(void*),wifi_communication(void*);

float ang;
int WalkingTraj [101] = {14, 13, 14, 16, 18, 21, 24, 28, 32, 38, 44, 51, 58, 65, 73, 80, 87, 93, 100, 105, 111, 115, 120, 124, 128, 132, 135, 138, 141, 143, 145, 148, 149, 151, 153, 154, 155, 156, 156, 156, 155, 154, 151, 146, 141, 133, 124, 113, 100, 85, 70, 53, 35, 18, 0, -16, -31, -45, -56, -64, -70, -73, -74, -72, -68, -61, -53, -44, -33, -22, -11, 0, 11, 22, 31, 40, 48, 55, 61, 66, 69, 72, 73, 74, 74, 73, 71, 69, 67, 64, 61, 57, 53, 49, 45, 40, 35, 30, 25, 20, 14};



const double Kp = 0.0, Ki = 0.0, Kd = 0.0;
double Setpoint, Input, Output;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);


float target, p;
int last_raw, raw_angle, raw;
int revolution = 0;

long NewAngle, OldAngle, D_angle;
int ZeroClaib = 1629+221-559-43;

int flip1=1,flip2=1,flip3=1,flip4=1;
int threshold=400;
AMS_5600 ams5600;


//---------------------------------------------------------

void setup() {
  Serial.begin(115200);
   Wire.begin();
  pinMode(LED_Dir, OUTPUT);
  //myPID.SetMode(MANUAL);
  task4.start( pid );
  //task5.start( myblink );
  task1.start( TrajGen );
  task3.start( read_ANGLE );
  task2.start( Communication );
  task6.start( toe_cells );
  task7.start(heel_cells);
  //task8.start(wifi_communication);
  
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);
}

void loop() {
  DELAY (1);
}
