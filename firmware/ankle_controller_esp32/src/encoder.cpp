/*
 * encoder.cpp - AS5600 joint angle task.
 *
 * The AS5600 is absolute within one turn only, so wrap-around is detected by
 * watching for a jump of more than half a revolution (2047 counts) between
 * samples, and a revolution counter accumulates multi-turn travel.
 *
 * The controller runs on raw counts. To read these as degrees, multiply by
 * 360/4096 (about 0.0879); the 2021 AVR build did exactly that.
 */

#include "config.h"

void read_ANGLE(void *) {
  VOID SETUP() {}

  VOID LOOP() {
    raw_angle = ams5600.getRawAngle();

    if ((last_raw - raw_angle) > 2047) revolution++;
    if ((last_raw - raw_angle) < -2047) revolution--;

    last_raw = raw_angle;
    raw = revolution * 4096 + raw_angle;
    raw = raw - ZeroClaib; /* offset of the assembled joint at neutral */

    DELAY(10);
  }
}
