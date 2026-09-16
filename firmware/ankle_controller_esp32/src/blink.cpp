/*
 * blink.cpp - heartbeat LED, used during bring-up to confirm the scheduler was
 * running when the motor was disconnected. Not started by default.
 */

#include "config.h"

void myblink(void *) {
  int led = 2;

  VOID SETUP() { pinMode(led, OUTPUT); }

  VOID LOOP() {
    digitalWrite(led, HIGH);
    DELAY(1000);
    digitalWrite(led, LOW);
    DELAY(1000);
  }
}
