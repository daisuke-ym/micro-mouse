// ----------------------------------------------------------------------
// 回転しながらセンサの値を得る
// 少しずつ回転しながらセンサの値を得ることで、ある角度におけるセンサのピーク値が得られる。
// ピーク値が得られた角度が、そのセンサが真北からどれくらいずれているかを示す。
// その値を使って、マウスの方向を調整することができる。
// この操作は、迷路探索中に行うのではなく、迷路探索前の機体調整の一環として行うことを想定している。
// この操作を行って得られた値を defines.h の OFFSET_SS_L, OFFSET_SS_FL, OFFSET_SS_FR, OFFSET_SS_R で定義する。
void get_sensor_value_while_rotating() {
  double a;
  double search_angle = 45.0;
  double step_angle = 0.5;
  int peak_ss[4] = {0, 0, 0, 0}; // センサのピーク値を格納する配列(L, FL, FR, R)
  double peak_angle[4] = {0, 0, 0, 0};

  run_to_mm(-50.0);
  delay(500);
  run_to_mm(25.0);
  delay(500);
  //STP_L.stepDelay(DEFAULT_STEP_DELAY * 5);
  //STP_R.stepDelay(DEFAULT_STEP_DELAY * 5);

  a = -1 * step_angle / 360.0 * TREAD * M_PI / MMPS;
  for (int i = 0; i < (search_angle / step_angle); i++) {
    STP_L.move(a);
    STP_R.move(-1 * a);
    while (STP_L.distanceToGo() != 0 && STP_R.distanceToGo() != 0) {
      STP_L.run();
      STP_R.run();
    }
  }
  STP_L.move(0); // 2回目以降呼び出したときに蛇行しないようにする処理
  STP_R.move(0);
  STP_L.disableOutputs(); // 停止後モーターの発熱を抑える処理
  STP_R.disableOutputs();
  delay(500);

  a = step_angle / 360.0 * TREAD * M_PI / MMPS;
  for (int i = 0; i < (search_angle * 2.0 / step_angle); i++) {
    STP_L.move(a);
    STP_R.move(-1 * a);
    while (STP_L.distanceToGo() != 0 && STP_R.distanceToGo() != 0) {
      STP_L.run();
      STP_R.run();
    }
    if (SS_L > peak_ss[0]) {
      peak_ss[0] = SS_L;
      peak_angle[0] = -1 * search_angle + i * step_angle;
    }
    if (SS_FL > peak_ss[1]) {
      peak_ss[1] = SS_FL;
      peak_angle[1] = -1 * search_angle + i * step_angle;
    }
    if (SS_FR > peak_ss[2]) {
      peak_ss[2] = SS_FR;
      peak_angle[2] = -1 * search_angle + i * step_angle;
    }
    if (SS_R > peak_ss[3]) {
      peak_ss[3] = SS_R;
      peak_angle[3] = -1 * search_angle + i * step_angle;
    }
    SERIAL_OUT.printf("%.1f %d %d %d %d\n", -1 * search_angle + i * step_angle, SS_L, SS_FL, SS_FR, SS_R);
  }
  delay(500);

  a = -1 * step_angle / 360.0 * TREAD * M_PI / MMPS;
  for (int i = 0; i < (search_angle / step_angle); i++) {
    STP_L.move(a);
    STP_R.move(-1 * a);
    while (STP_L.distanceToGo() != 0 && STP_R.distanceToGo() != 0) {
      STP_L.run();
      STP_R.run();
    }
  }
  STP_L.move(0); // 2回目以降呼び出したときに蛇行しないようにする処理
  STP_R.move(0);
  STP_L.disableOutputs(); // 停止後モーターの発熱を抑える処理
  STP_R.disableOutputs();

  //STP_L.stepDelay(DEFAULT_STEP_DELAY);
  //STP_R.stepDelay(DEFAULT_STEP_DELAY);

  SERIAL_OUT.println("Peak sensor values:");
  SERIAL_OUT.printf("L: %d at %.1f degrees\n", peak_ss[0], peak_angle[0]);
  SERIAL_OUT.printf("FL: %d at %.1f degrees\n", peak_ss[1], peak_angle[1]);
  SERIAL_OUT.printf("FR: %d at %.1f degrees\n", peak_ss[2], peak_angle[2]);
  SERIAL_OUT.printf("R: %d at %.1f degrees\n", peak_ss[3], peak_angle[3]);
}