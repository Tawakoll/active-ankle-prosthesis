void read_ANGLE(void*) {
  
int ZeroClaib = 1430;


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

    DELAY(20);

  }
  
}
