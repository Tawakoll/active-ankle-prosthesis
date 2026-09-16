
void Communication(void*) {         

  //-----------------------------------------------

  VOID SETUP() {              
  }

  VOID LOOP() {               
    if (Serial.available() > 0)
    {
      float KP = Serial.parseFloat();
      float KI = Serial.parseFloat();
      float KD = Serial.parseFloat();
      DELAY(10);
      Serial.flush();
      Serial.print(KP);Serial.print("  ");Serial.print(KI);Serial.print("  ");Serial.print(KD);Serial.println("  ");
      myPID.SetTunings(KP, KI, KD);
      myPID.SetMode(AUTOMATIC);
//      Serial.print("Target");Serial.print("Actual");

    }
    Serial.print("Target:"); Serial.print(target);
    Serial.print(" Actual:"); Serial.print(raw);
    Serial.print(" PIDOut:"); Serial.println(Output);
    DELAY(20);
  }
}
