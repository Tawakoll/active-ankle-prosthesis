void heel_cells(void*) {

  VOID SETUP(){
     DELAY(10);
//     Serial.println();
//  Serial.println("Starting...");

float calibrationValue_3; // calibration value load cell 1
float calibrationValue_4; // calibration value load cell 4

  calibrationValue_3 = 696.0; // uncomment this if you want to set this value in the sketch
  calibrationValue_4 = 733.0; // uncomment this if you want to set this value in the sketch
#if defined(ESP8266) || defined(ESP32)
  //EEPROM.begin(512); // uncomment this if you use ESP8266 and want to fetch the value from eeprom
#endif
  //EEPROM.get(calVal_eepromAdress_1, calibrationValue_1); // uncomment this if you want to fetch the value from eeprom
  //EEPROM.get(calVal_eepromAdress_2, calibrationValue_2); // uncomment this if you want to fetch the value from eeprom

  LoadCell_3.begin();
  LoadCell_4.begin();
  unsigned long stabilizingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  byte loadcell_3_rdy = 0;
  byte loadcell_4_rdy = 0;
  while ((loadcell_3_rdy + loadcell_4_rdy) < 2) { //run startup, stabilization and tare, both modules simultaniously
    if (!loadcell_3_rdy) loadcell_3_rdy = LoadCell_3.startMultiple(stabilizingtime, _tare);
    if (!loadcell_4_rdy) loadcell_4_rdy = LoadCell_4.startMultiple(stabilizingtime, _tare);
  }
  if (LoadCell_3.getTareTimeoutFlag()) {
//    Serial.println("Timeout, check MCU>HX711 no.1 wiring and pin designations");
  }
  if (LoadCell_4.getTareTimeoutFlag()) {
//    Serial.println("Timeout, check MCU>HX711 no.2 wiring and pin designations");
  }
  LoadCell_3.setCalFactor(calibrationValue_3); // user set calibration value (float)
  LoadCell_4.setCalFactor(calibrationValue_4); // user set calibration value (float)
//  Serial.println("Startup is complete");
    
    }
  VOID LOOP() {
    static boolean newDataReady = 0;
    const int serialPrintInterval = 0; //increase value to slow down serial print activity

    // check for new data/start next conversion:
    if (LoadCell_3.update()) newDataReady = true;
    LoadCell_4.update();

    //get smoothed value from data set
    if ((newDataReady)) {
      if (millis() > t + serialPrintInterval) {
        float heel_weight = LoadCell_3.getData() + LoadCell_4.getData();
        if(heel_weight >=threshold){ heel_state=true;}
        else {heel_state=false;}
        
//        Serial.print("Load_cell 3 output val: ");
//        Serial.print(a);
//        Serial.print("    Load_cell 4 output val: ");
//        Serial.println(b);
        newDataReady = 0;
        t = millis();
      }
    }

    // receive command from serial terminal, send 't' to initiate tare operation:
    if (Serial.available() > 0) {
      char inByte = Serial.read();
      if (inByte == 't') {
        LoadCell_3.tareNoDelay();
        LoadCell_4.tareNoDelay();
      }
    }

    //check if last tare operation is complete
    if (LoadCell_3.getTareStatus() == true) {
//      Serial.println("Tare load cell 1 complete");
    }
    if (LoadCell_4.getTareStatus() == true) {
//      Serial.println("Tare load cell 4 complete");
    }



  }

}
