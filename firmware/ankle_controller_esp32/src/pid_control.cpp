/*
 * pid_control.cpp - position PID loop.
 *
 * Output is signed: magnitude becomes PWM duty on the Cytron MD10C, sign
 * becomes the direction pin. Duty is limited to +/-125 of 255 (about 49 per
 * cent) to protect the drivetrain, which is a motor salvaged from a cordless
 * drill driving a ball screw.
 *
 * The travel limit below is a hard stop: outside -80..170 counts the motor is
 * cut regardless of what the loop asks for.
 */

#include "config.h"

void pid(void *) {
  VOID SETUP() {
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
    ledcWrite(LEDC_CHANNEL_0, pwm_output);

    if (raw > 170 || raw < -80) { /* travel limit: cut the motor */
      ledcWrite(LEDC_CHANNEL_0, 0);
    }

    digitalWrite(LED_Dir, (Output < 0) ? LOW : HIGH);
  }
}
