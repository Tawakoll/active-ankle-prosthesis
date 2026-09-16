/*
 * trajectory.cpp - reference trajectory generator.
 *
 * WalkingTraj holds 101 samples of one gait cycle in raw encoder counts. This
 * task decides which part of that cycle to play, and feeds samples to the PID
 * loop as its setpoint.
 *
 * Gait phase comes from the two contact booleans:
 *
 *   heel  toe   phase
 *   ----  ---   -------------------------------
 *    1     0    heel strike, moving to flat foot
 *    1     1    flat foot, moving to heel off
 *    0     1    heel off, moving to toe off
 *    0     0    swing
 *
 * The flip flags latch each phase so a segment plays once per step rather than
 * restarting while the contact state holds.
 *
 * The plain timed sweep kept below was the bench fallback: it ignores contact
 * entirely and plays the whole cycle on a fixed delay. Useful when running the
 * ankle in the air with no load on the foot.
 */

#include "config.h"

void TrajGen(void *) {
  int del = 20;

  VOID LOOP() {
    if (heel_state == true && toe_state == false && flip1 == 1) {
      for (int i = 0; i < 20; i++) {
        target = WalkingTraj[i];
        DELAY(del);
        if (flip1 == 1) { flip1 = 0; flip2 = 1; }
      }
    }

    else if (heel_state == true && toe_state == true && flip2 == 1) {
      for (int i = 21; i < 41; i++) {
        target = WalkingTraj[i];
        DELAY(del);
        if (flip2 == 1) { flip2 = 0; flip3 = 1; }
      }
    }

    else if (heel_state == false && toe_state == true && flip3 == 1) {
      for (int i = 41; i < 61; i++) {
        target = WalkingTraj[i];
        DELAY(del);
        if (flip3 == 1) { flip3 = 0; flip4 = 1; }
      }
    }

    else if (heel_state == false && toe_state == false && flip4 == 1) {
      for (int i = 61; i < 101; i++) {
        target = WalkingTraj[i];
        DELAY(del);
        if (flip4 == 1) { flip4 = 0; flip1 = 1; }
      }
    }

    else {
      target = 0;
    }

    /* Bench fallback, no contact needed:
     *
     * for (int i = 0; i < 101; i++) {
     *   target = WalkingTraj[i];
     *   DELAY(20);
     * }
     */
  }
}
