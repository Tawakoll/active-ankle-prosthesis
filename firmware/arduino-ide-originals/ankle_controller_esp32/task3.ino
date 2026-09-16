void read_ANGLE(void*) {
  int led = 2;
 
 VOID SETUP() {
//    ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
//    ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);

  }

  VOID LOOP()  {


    
    raw_angle = ams5600.getRawAngle();

    if ((last_raw - raw_angle) > 2047)
      revolution++;

    if ((last_raw - raw_angle) < -2047)
      revolution--;

    last_raw = raw_angle;
    raw = revolution * 4096 + raw_angle;
    raw = raw - ZeroClaib;
//    ang = raw * 0.087;

    DELAY(10);

  }
  
}
