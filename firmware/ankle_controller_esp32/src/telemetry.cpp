/*
 * telemetry.cpp - serial telemetry and live gain tuning.
 *
 * Sending three floats over serial sets Kp, Ki and Kd at run time, which is
 * how the loop was tuned: change one gain, watch the step response in the
 * Arduino plotter, repeat. Tuned values were Kp 0.14, Ki 0.2, Kd 0.
 */

#include "config.h"

void Communication(void *) {
  VOID SETUP() {}

  VOID LOOP() {
    if (Serial.available() > 0) {
      float KP = Serial.parseFloat();
      float KI = Serial.parseFloat();
      float KD = Serial.parseFloat();
      DELAY(10);
      Serial.flush();
      Serial.print(KP); Serial.print("  ");
      Serial.print(KI); Serial.print("  ");
      Serial.print(KD); Serial.println("  ");
      myPID.SetTunings(KP, KI, KD);
      myPID.SetMode(AUTOMATIC);
    }

    Serial.print("Target:"); Serial.print(target);
    Serial.print(" Actual:"); Serial.print(raw);
    Serial.print(" PIDOut:"); Serial.println(Output);
    DELAY(20);
  }
}
