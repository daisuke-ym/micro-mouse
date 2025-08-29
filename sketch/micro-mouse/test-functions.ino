// ----------------------------------------------------------------------
void test_decide_direction() {
  if (digitalRead(SW1) == LOW) {
    delay(1000);
    // ゴールを目指す
    while (decide_direction_lefthand(MAZE_GOAL_X, MAZE_GOAL_Y) != 0) {
      delay(250);
    }
    flash_alternate(20); // 目標到達時にLEDを点滅
    delay(2000);
    // スタート地点を目指す
    while (decide_direction_lefthand(0, 0) != 0) {
      delay(250);
    }
    flash_alternate(20); // 目標到達時にLEDを点滅
    delay(2000);
  }
}

// ----------------------------------------------------------------------
void test_run_shortest_path() {
  if (digitalRead(SW2) == LOW) {
    delay(1000);
    run_shortest_path();
    flash_alternate(20); // 最短経路実行時にLEDを点滅
    delay(2000);
  }
}

// ----------------------------------------------------------------------
void test_make_steps_map() {
  if (digitalRead(SW2) == LOW) {
    delay(1000);
    make_steps_map(MAZE_GOAL_X, MAZE_GOAL_Y);
    resolve_shortest_path(0, 0, DIR_NORTH);
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

// ----------------------------------------------------------------------
void test_get_sensor_init_value() {
  if (digitalRead(SW2) == LOW) {
    delay(1000);
    get_sensor_init_value();
    flash_led(2, 5);
  }
}

// ----------------------------------------------------------------------
void test_get_sensor_value_while_rotating() {
  if (digitalRead(SW1) == LOW) {
    delay(1000);
    get_sensor_value_while_rotating();
    flash_led(1, 5);
  }
}

// ----------------------------------------------------------------------
void test_adjust_to_center() {
  if (digitalRead(SW2) == LOW) {
    delay(1000);
    adjust_to_center();
    flash_led(2, 5);
  }
}

// ----------------------------------------------------------------------
void test_decide_direction_tremaux() {
  if (digitalRead(SW1) == LOW) {
    delay(1000);
    // ゴールを目指す
    while (decide_direction_tremaux(MAZE_GOAL_X, MAZE_GOAL_Y) != 0) {
      delay(250);
    }
    flash_alternate(20); // 目標到達時にLEDを点滅
    delay(2000);
    // スタート地点を目指す
    while (decide_direction_tremaux(0, 0) != 0) {
      delay(250);
    }
    flash_alternate(20); // 目標到達時にLEDを点滅
    delay(2000);
  }
}
