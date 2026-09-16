void myblink(void*) {

  //-----------------------------------------------
//  int brightness = 0;    // how bright the LED is
//  int fadeAmount = 5;    // how many points to fade the LED by
int led = 2;
//
  VOID SETUP() {
//    ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
//    ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);
 pinMode(2, OUTPUT);
  }
//
  VOID LOOP() {

digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  DELAY(1000);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  DELAY(1000);               // wait for a second

//    // set the brightness on LEDC channel 0
//
//    // change the brightness for next time through the loop:
//    brightness = brightness + fadeAmount;
//
//    // reverse the direction of the fading at the ends of the fade:
//    if (brightness <= 0 || brightness >= 255) {
//      fadeAmount = -fadeAmount;
//    }
//
//    uint32_t duty = (8191 / 255) * min(brightness, 255);
//
//    // write duty to LEDC
//    ledcWrite(LEDC_CHANNEL_0, duty);
//
//
//
//    // wait for 30 milliseconds to see the dimming effect
//    DELAY(30);
  }
}

//KP 0.0 KI 0.0 KD 0.0
//KP 3.11 KI 0.0 KD 0.00 @freq 5k res 8 bit sample time 10 delay(2) in pid loop
//KP 0.18 KI 1.0 KD 0.001
