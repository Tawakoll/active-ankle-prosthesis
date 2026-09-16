void pid(void *) {

 
    VOID SETUP() {
 //delay (50);
  Setpoint = raw;
  myPID.SetOutputLimits(-125, 125);
  myPID.SetSampleTime(20);
  }

  VOID LOOP() {
    DELAY(2);
    Input = raw;
    Setpoint = target;
    myPID.Compute();
    int pwm_output = fabs(Output);

//    NewAngle = raw;
//    D_angle = OldAngle - NewAngle;
//    OldAngle = NewAngle;
//
//   if (pwm_output > 10 && abs(D_angle) == 0)
//  {
//     myPID.SetMode(MANUAL);
//      ledcWrite(LEDC_CHANNEL_0, 0);
//   }
//    else 
      
      ledcWrite(LEDC_CHANNEL_0, pwm_output);
    

    if (raw > 170 || raw < -80) {
     
      ledcWrite(LEDC_CHANNEL_0, 0);
    }



    if (Output < 0)
    {
      digitalWrite(LED_Dir, LOW);
    }
    else
    {
      digitalWrite(LED_Dir, HIGH);
    }
  }
}
