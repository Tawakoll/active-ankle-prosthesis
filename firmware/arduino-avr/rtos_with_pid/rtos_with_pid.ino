#include <Arduino_FreeRTOS.h>

#include <util/atomic.h> // For the ATOMIC_BLOCK macro
#include <Wire.h>
#include <AS5600.h>
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
  #define SERIAL SerialUSB
  #define SYS_VOL   3.3
#else
  #define SERIAL Serial
  #define SYS_VOL   5
#endif

const float kp = 1,kd = 0.025 ,ki = 0.0;
 

  
 
AMS_5600 ams5600;

int target =0;

int ang, lang = 0;

int PWM =9;
int IN1= 8;

volatile int posi = 0; // specify posi as volatile: https://www.arduino.cc/reference/en/language/variables/variable-scope-qualifiers/volatile/
long prevT = 0;
float eprev = 0;
float eintegral = 0;

void read_ANGLE(void *param);
void serial_print(void *param);
void pid(void *param);

TaskHandle_t Task_Handle1;
TaskHandle_t Task_Handle2;
TaskHandle_t Task_Handle3;

void setup() {
  // put your setup code here, to run once
  pinMode(PWM,OUTPUT);
  pinMode(IN1,OUTPUT);
  
   SERIAL.begin(115200);
  Wire.begin();
  SERIAL.println(">>>>>>>>>>>>>>>>>>>>>>>>>>> ");
  if(ams5600.detectMagnet() == 0 ){
    while(1){
        if(ams5600.detectMagnet() == 1 ){
           // SERIAL.print("Current Magnitude: ");
           // SERIAL.println(ams5600.getMagnitude());
            break;
        }
        else{
            //SERIAL.println("Can not detect magnet");
        }
        delay(1000);
    }
 
  }
  



  xTaskCreate(read_ANGLE,"Task1",100,NULL,1,&Task_Handle1);
  xTaskCreate(serial_print,"Task2",100,NULL,1,&Task_Handle2);
  xTaskCreate(pid,"Task3",100,NULL,1,&Task_Handle3);
}

void loop() {
  // put your main code here, to run repeatedly:

}


void read_ANGLE(void *param){
  (void) param;
  while(1){
      float retVal = ams5600.getRawAngle()* 0.087;
      ang = retVal;
    
    //vTaskDelay(1000/portTICK_PERIOD_MS);
    }
  
  }

  void serial_print(void *param){
  
  (void) param;
  while(1){
   Serial.print("real_angle = ");
   Serial.println(ang);
   
    Serial.print("Target = ");
    target = 0;
    Serial.println(target);
  
   vTaskDelay(1000/portTICK_PERIOD_MS);
     Serial.print("Target = ");
     target = 150;
    Serial.println(target);
    }
  }


    void pid(void *param){
  
  (void) param;
  while(1){
   
  // time difference
  long currT = micros();
  float deltaT = ((float) (currT - prevT))/( 1.0e6 );
  prevT = currT;

  // Read the position in an atomic block to avoid a potential
  // misread if the interrupt coincides with this code running
  // see: https://www.arduino.cc/reference/en/language/variables/variable-scope-qualifiers/volatile/
  int pos = 0; 
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    pos = posi;
  }
  posi = ang ;
  
  // error
  int e = pos - target;

  // derivative
  float dedt = (e-eprev)/(deltaT);

  // integral
  eintegral = eintegral + e*deltaT;

  // control signal
  float u = kp*e + kd*dedt + ki*eintegral;

  // motor power
  float pwr = fabs(u);
  if( pwr > 50 ){
    pwr = 50;
  }

  // motor direction
  int dir = 1;
  if(u<0){
    dir = -1;
  }

  // signal the motor
 analogWrite(PWM,pwr);
  if(dir == 1){
    digitalWrite(IN1,HIGH);
    
  }
  else if(dir == -1){
    digitalWrite(IN1,LOW);
    
  }
  else{
    analogWrite(PWM,0);
  }  
   
   // store previous error
  eprev = e;
    }





  }
