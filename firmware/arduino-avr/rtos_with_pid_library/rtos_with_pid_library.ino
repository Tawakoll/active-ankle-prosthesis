#include <Arduino_FreeRTOS.h>
#include <Wire.h>
#include <AS5600.h>
#include <PID_v1.h>

const double Kp = 1, Ki = 0.05, Kd = 0.01;

AMS_5600 ams5600;

double Setpoint, Input, Output;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);


float ang , target;


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
  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>> ");
  if (ams5600.detectMagnet() == 0 ) {
    while (1) {
      if (ams5600.detectMagnet() == 1 ) {
         Serial.print("Current Magnitude: ");
         Serial.println(ams5600.getMagnitude());
        break;
      }
      else {
        Serial.println("Can not detect magnet");
      }
      delay(1000);
    }

  }

  xTaskCreate(read_ANGLE, "Task1", 100, NULL, 1, &Task_Handle1);
  xTaskCreate(serial_print, "Task2", 100, NULL, 2, &Task_Handle2);
  xTaskCreate(pid, "Task3", 100, NULL, 3, &Task_Handle3);
  xTaskCreate(target_angle, "Task4", 100, NULL, 4, &Task_Handle4);

  delay (10);
  Setpoint = ang;
  myPID.SetOutputLimits(-25, 25);
  myPID.SetMode(AUTOMATIC);
}

void loop() {

  // put your main code here, to run repeatedly:

}


void read_ANGLE(void *param) {
  (void) param;
  while (1) {
    float retVal = ams5600.getRawAngle() * 0.087;
    ang = retVal;

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }

}

void serial_print(void *param) {

  (void) param;
  while (1) {

    Serial.print(ang);

//    Serial.print("  ");
//    Serial.print(Output);

    Serial.print("  ");
    Serial.println(target);

    

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void target_angle(void *param) {

  (void) param;
  while (1) {
    target = 150;
       vTaskDelay(2000/portTICK_PERIOD_MS);
    //   target=0;
    //   vTaskDelay(2000/portTICK_PERIOD_MS);
  }

}


void pid(void *param) {

  (void) param;
  while (1) {
    vTaskDelay(10/portTICK_PERIOD_MS);
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
