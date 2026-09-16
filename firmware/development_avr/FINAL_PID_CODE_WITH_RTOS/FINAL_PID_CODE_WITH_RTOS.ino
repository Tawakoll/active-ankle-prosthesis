#include <Arduino_FreeRTOS.h>
#include <Wire.h>
#include <AS5600.h>
#include <PID_v1.h>

AMS_5600 ams5600;

int ang, lang = 0;

int WalkingTraj [101] = {14, 13, 14, 16, 18, 21, 24, 28, 32, 38, 44, 51, 58, 65, 73, 80, 87, 93, 100, 105, 111, 115, 120, 124, 128, 132, 135, 138, 141, 143, 145, 148, 149, 151, 153, 154, 155, 156, 156, 156, 155, 154, 151, 146, 141, 133, 124, 113, 100, 85, 70, 53, 35, 18, 0, -16, -31, -45, -56, -64, -70, -73, -74, -72, -68, -61, -53, -44, -33, -22, -11, 0, 11, 22, 31, 40, 48, 55, 61, 66, 69, 72, 73, 74, 74, 73, 71, 69, 67, 64, 61, 57, 53, 49, 45, 40, 35, 30, 25, 20, 14};


const double Kp = 0.14, Ki = 0.2, Kd = 0.0;

double Setpoint, Input, Output;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);


float target, p;
long last_raw, raw_angle, raw;
int revolution = 0;
int PWM = 9;
int IN1 = 8;



void read_ANGLE(void *param);
void serial_print(void *param);
void pid(void *param);
void target_angle(void *param);

TaskHandle_t Task_Handle1;
TaskHandle_t Task_Handle2;
TaskHandle_t Task_Handle3;
TaskHandle_t Task_Handle4;


void setup() {
  // put your setup code here, to run once
  pinMode(PWM, OUTPUT);
  pinMode(IN1, OUTPUT);


  Serial.begin(115200);
  Wire.begin();

  delay (10);

  xTaskCreate(read_ANGLE, "Task1", 100, NULL, 2, &Task_Handle1);
  xTaskCreate(serial_print, "Task2", 100, NULL, 2, &Task_Handle2);
  xTaskCreate(pid, "Task3", 100, NULL, 2, &Task_Handle3);
  xTaskCreate(target_angle, "Task4", 100, NULL, 2, &Task_Handle4);

}

void loop() {

  // put your main code here, to run repeatedly:

}


void read_ANGLE(void *param) {
  (void) param;
  while (1) {
    raw_angle = ams5600.getRawAngle();

    if ((last_raw - raw_angle) > 2047)
      revolution++;

    if ((last_raw - raw_angle) < -2047)
      revolution--;

    last_raw = raw_angle;
    raw = revolution * 4096 + raw_angle;
    ang = raw * 0.087;

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }

}

void serial_print(void *param) {

  (void) param;
  while (1) {
    if (Serial.available() > 0)
    {
      p = Serial.parseFloat();
      Serial.println();
      myPID.SetTunings(p, 0.3, Kd);
    }


    Serial.print(Input);

    Serial.print("  ");
    Serial.print(Output);

    Serial.print("  ");
    Serial.println(Setpoint);



    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void target_angle(void *param) {

  (void) param;
  while (1) {
//        target = 150;
//           vTaskDelay(5000/portTICK_PERIOD_MS);
//           target=0;
//           vTaskDelay(5000/portTICK_PERIOD_MS);
    for (int i = 1; i < 100; i++) {
      target = WalkingTraj [i];
    vTaskDelay(50 / portTICK_PERIOD_MS);
    }
  }

}


void pid(void *param) {

  (void) param;
  //delay (50);
  Setpoint = ang;
  myPID.SetOutputLimits(-100, 100);
  myPID.SetMode(AUTOMATIC);
  myPID.SetSampleTime(200);

  while (1) {
    vTaskDelay(200 / portTICK_PERIOD_MS);
    Input = ang;
    Setpoint = target;
    myPID.Compute();
    float pwm_output = fabs(Output);

    analogWrite(PWM, pwm_output);
    if (Output < 0)
    {
      digitalWrite(IN1, HIGH);
    }
    else
    {
      digitalWrite(IN1, LOW);
    }
  }


}
