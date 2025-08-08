// ----------------------------------------------------------------------
void test_decide_direction() {
  if (digitalRead(SW1) == LOW) {
    delay(1000);
    // ゴールを目指す
    while (decide_direction(MAZE_GOAL_X, MAZE_GOAL_Y) != 0) {
      delay(250);
    }
    flash_alternate(20); // 目標到達時にLEDを点滅
    delay(2000);
    // スタート地点を目指す
    while (decide_direction(0, 0) != 0) {
      delay(250);
    }
    flash_alternate(20); // 目標到達時にLEDを点滅
    delay(2000);
  }
}

// ----------------------------------------------------------------------
void test_run_to_mm() {
  if (digitalRead(SW2) == LOW) {
    delay(1000);
    run_to_mm(180.0); // 180mm 前進
    flash_led(2, 5);
  }
}

// ----------------------------------------------------------------------
void test_spinturn() {
  if (digitalRead(SW1) == LOW) {
    delay(1000);
    spinturn(90.0); // 90度右回り
    flash_led(1, 5);
  }
}

// ----------------------------------------------------------------------
void test_pivotturn() {
  if (digitalRead(SW1) == LOW) {
    delay(1000);
    pivoturn(-90.0); // 90度左回り
    flash_led(1, 5);
  }
}

// ----------------------------------------------------------------------
void test_spinturn_imu() {
  if (digitalRead(SW1) == LOW) {
    delay(1000);
    spinturn_imu(-90.0); // 90度左回り
    flash_led(1, 5);
  }
  if (digitalRead(SW2) == LOW) {
    delay(1000);
    spinturn_imu(90.0); // 90度右回り
    flash_led(2, 5);
  }
}
