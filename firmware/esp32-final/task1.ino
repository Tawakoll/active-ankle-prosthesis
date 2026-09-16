
void TrajGen(void*) {

  //-----------------------------------------------
 // int WalkingTraj [101] = {14, 13, 14, 16, 18, 21, 24, 28, 32, 38, 44, 51, 58, 65, 73, 80, 87, 93, 100, 105, 111, 115, 120, 124, 128, 132, 135, 138, 141, 143, 145, 148, 149, 151, 153, 154, 155, 156, 156, 156, 155, 154, 151, 146, 141, 133, 124, 113, 100, 85, 70, 53, 35, 18, 0, -16, -31, -45, -56, -64, -70, -73, -74, -72, -68, -61, -53, -44, -33, -22, -11, 0, 11, 22, 31, 40, 48, 55, 61, 66, 69, 72, 73, 74, 74, 73, 71, 69, 67, 64, 61, 57, 53, 49, 45, 40, 35, 30, 25, 20, 14};
int del=20;
  while (1) {              // เหมือน loop() แต่ใส่ชื่อเป็น LOOP() พิมพ์ใหญ่แทน

    //            target = 150*(4095/360);
    //               DELAY (5000);
    //               target=0;
    //               DELAY (5000);

 for (int i = 0; i < 101; i++) {
        target = WalkingTraj [i];
        DELAY (20);
      }

    






//    if ( heel_state == true && toe_state == false && flip1 == 1) //we are in heel strike moving to flat foot phase
//    {
//      for (int i = 0; i < 20; i++) {
//        target = WalkingTraj [i];
//        DELAY (del);
//        if (flip1 == 1) 
//        {
//          flip1 = 0;
//          flip2=1;
//          }
//       
//      }
//    }
//
//    else if ( heel_state == true && toe_state == true && flip2 == 1) //we are in flat foot moving to heel off phase
//    {
//      for (int i = 21; i < 41; i++) {
//        target = WalkingTraj [i];
//        DELAY (del);
//
//            if (flip2 == 1) 
//        {
//          flip2 = 0;
//          flip3=1;
//          }
//      }
//    }
//    else if ( heel_state == false && toe_state == true && flip3 == 1) //we are in heel off moving to toe off phase
//    {
//      for (int i = 41; i < 61; i++) {
//        target = WalkingTraj [i];
//        DELAY (del);
//         if (flip3 == 1) 
//        {
//          flip3 = 0;
//          flip4=1;
//          }
//      }
//    }
//    else if ( heel_state == false && toe_state == false && flip4 == 1) //we are in heel off moving to toe off phase
//    {
//      for (int i = 61; i < 101; i++) {
//        target = WalkingTraj [i];
//        DELAY (del);
//        if (flip4 == 1) {
//          flip4 = 0;
//          flip1 = 1;
//        }
//
//
//      }
//    }
//    else target = 0;
  }
}

//1 0 HS to FF
//1 1 FF to HO
//0 1 HO to TO
//0 0 Swing
