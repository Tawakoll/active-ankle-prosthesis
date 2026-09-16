/*
 * load_cells.cpp - ground contact detection.
 *
 * Four 50 kg full-bridge strain gauge cells on HX711 amplifiers: cells 1 and 2
 * under the toe, cells 3 and 4 under the heel. Each pair is summed and
 * compared against a fixed threshold to give a boolean contact state, which is
 * what the trajectory generator uses to work out the gait phase.
 *
 * Calibration factors were measured per cell with known weights. Sending 't'
 * over serial re-tares all four.
 */

#include "config.h"

void toe_cells(void *) {
  VOID SETUP() {
    DELAY(10);

    float calibrationValue_1 = 696.0;
    float calibrationValue_2 = 733.0;

    LoadCell_1.begin();
    LoadCell_2.begin();

    unsigned long stabilizingtime = 2000; /* longer settle gives a better tare */
    boolean _tare = true;
    byte loadcell_1_rdy = 0;
    byte loadcell_2_rdy = 0;

    while ((loadcell_1_rdy + loadcell_2_rdy) < 2) { /* start both together */
      if (!loadcell_1_rdy) loadcell_1_rdy = LoadCell_1.startMultiple(stabilizingtime, _tare);
      if (!loadcell_2_rdy) loadcell_2_rdy = LoadCell_2.startMultiple(stabilizingtime, _tare);
    }

    LoadCell_1.setCalFactor(calibrationValue_1);
    LoadCell_2.setCalFactor(calibrationValue_2);
  }

  VOID LOOP() {
    static boolean newDataReady = 0;
    const int serialPrintInterval = 0;

    if (LoadCell_1.update()) newDataReady = true;
    LoadCell_2.update();

    if (newDataReady) {
      if (millis() > t + serialPrintInterval) {
        float front_weight = LoadCell_1.getData() + LoadCell_2.getData();
        toe_state = (front_weight >= threshold);
        newDataReady = 0;
        t = millis();
      }
    }

    if (Serial.available() > 0) { /* 't' re-tares */
      char inByte = Serial.read();
      if (inByte == 't') {
        LoadCell_1.tareNoDelay();
        LoadCell_2.tareNoDelay();
      }
    }
  }
}

void heel_cells(void *) {
  VOID SETUP() {
    DELAY(10);

    float calibrationValue_3 = 696.0;
    float calibrationValue_4 = 733.0;

    LoadCell_3.begin();
    LoadCell_4.begin();

    unsigned long stabilizingtime = 2000;
    boolean _tare = true;
    byte loadcell_3_rdy = 0;
    byte loadcell_4_rdy = 0;

    while ((loadcell_3_rdy + loadcell_4_rdy) < 2) {
      if (!loadcell_3_rdy) loadcell_3_rdy = LoadCell_3.startMultiple(stabilizingtime, _tare);
      if (!loadcell_4_rdy) loadcell_4_rdy = LoadCell_4.startMultiple(stabilizingtime, _tare);
    }

    LoadCell_3.setCalFactor(calibrationValue_3);
    LoadCell_4.setCalFactor(calibrationValue_4);
  }

  VOID LOOP() {
    static boolean newDataReady = 0;
    const int serialPrintInterval = 0;

    if (LoadCell_3.update()) newDataReady = true;
    LoadCell_4.update();

    if (newDataReady) {
      if (millis() > t + serialPrintInterval) {
        float heel_weight = LoadCell_3.getData() + LoadCell_4.getData();
        heel_state = (heel_weight >= threshold);
        newDataReady = 0;
        t = millis();
      }
    }

    if (Serial.available() > 0) {
      char inByte = Serial.read();
      if (inByte == 't') {
        LoadCell_3.tareNoDelay();
        LoadCell_4.tareNoDelay();
      }
    }
  }
}
