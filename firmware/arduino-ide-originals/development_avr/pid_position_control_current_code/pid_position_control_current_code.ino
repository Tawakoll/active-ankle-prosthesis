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
 
AMS_5600 ams5600;
 
int ang, lang = 0;

int PWM =9;
int IN1= 8;

volatile int posi = 0; // specify posi as volatile: https://www.arduino.cc/reference/en/language/variables/variable-scope-qualifiers/volatile/
long prevT = 0;
float eprev = 0;
float eintegral = 0;

void setup() {
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
  

  
  Serial.println("target pos");
} 

//////function///////
float convertRawAngleToDegrees(word newAngle)
{
  /* Raw data reports 0 - 4095 segments, which is 0.087 of a degree */
  float retVal = newAngle * 0.087;
  ang = retVal;
  return ang;
}


void loop() {

  // set target position
  int target = 0;
  /////////////int target = 250*sin(prevT/1e6);

  // PID constants
  float kp = 0.9;
  float kd = 0;
  float ki = 0.0;

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
  posi = convertRawAngleToDegrees(ams5600.getRawAngle()),DEC ;
  
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
  if( pwr > 30 ){
    pwr = 30;
  }

  // motor direction
  int dir = 1;
  if(u<0){
    dir = -1;
 }

  // signal the motor
  setMotor(dir,pwr,PWM,IN1);


  // store previous error
  eprev = e;

  Serial.print(target);
  Serial.print(" ");
  Serial.print(pos);
  Serial.println();
}

void setMotor(int dir, int pwmVal, int PWM, int in1){
  analogWrite(PWM,pwmVal);
  if(dir == 1){
    digitalWrite(in1,HIGH);
    
  }
  else if(dir == -1){
    digitalWrite(in1,LOW);
    
  }
  else{
    analogWrite(PWM,0);
  }  
}
