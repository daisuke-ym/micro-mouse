// ----------------------------------------------------------------------
// 壁センサの初期値を得る（四方全部壁に囲まれたところで調整）
void get_sensor_init_value() {
  INIT_SS_FL = SS_FL;
  INIT_SS_L = SS_L;
  INIT_SS_R = SS_R;
  INIT_SS_FR = SS_FR;
  // デバッグ用
  SERIAL_OUT.println("initial sensor values: ");
  SERIAL_OUT.println("L FL FR R: ");
  SERIAL_OUT.printf("%d %d %d %d\n", INIT_SS_L, INIT_SS_FL, INIT_SS_FR, INIT_SS_R);
}

// ----------------------------------------------------------------------
// マウスが通路の中央に来るように調整する
void adjust_to_center() {
  int now_ss_l;
  int distance; // 壁までの距離
  int diff; // 補正する距離
  double angle; // 補正に必要な回転角度
  const int center = 28; // 通路の中央に置いた時の壁までの距離
  const int margin = 5; // 調整マージン（この距離より小さい場合は補正しない）

  // 最初に壁を背にして長めに後退する（車体の角度補正のため）
  run_to_mm(-60.0);
  delay(500);
  now_ss_l = SS_L;
  // 壁までの距離マップから距離を推定（直線補完）する
  int i = 0;
  do {
    i++;
  } while (WALL_DIST[i] > now_ss_l);
  distance = (i - 1) * 5 + (WALL_DIST[i] - now_ss_l) * 5 / (WALL_DIST[i] - WALL_DIST[i - 1]);
  SERIAL_OUT.printf("SS_L: %d  approx dist to wall: %d\n", now_ss_l, distance);
  // 中央に来るように調整
  // pivoturn(LorR) -> pivoturn(RorL) -> run_to_mm(-)
  if (distance < (center - margin)) {
    // 左寄りなので右に移動して補正
    diff = center - distance;
    angle = acos(1.0 - diff / TREAD) * 180.0 / M_PI; // 補正角度(deg)
    SERIAL_OUT.printf("Adjusting to center: move to right, diff: %d, angle: %.1f\n", diff, angle);
    pivoturn(angle);
    delay(250);
    pivoturn(-1 * angle);
    delay(250);
    run_to_mm(-1 * TREAD * sin(angle * M_PI / 180.0) - 10.0);
  }
  else if (distance > (center + margin)) {
    // 右寄りなので左に移動して補正
    diff = distance - center - 5;
    angle = acos(1.0 - diff / TREAD) * 180.0 / M_PI; // 補正角度(deg)
    SERIAL_OUT.printf("Adjusting to center: move to left, diff: %d, angle: %.1f\n", diff, angle);
    pivoturn(-1 * angle);
    delay(250);
    pivoturn(angle);
    delay(250);
    run_to_mm(-1 * TREAD * sin(angle * M_PI / 180.0) - 10.0);
  }
  delay(500);
  // 補正後の距離を再度計算
  now_ss_l = SS_L;
  // 壁までの距離マップから距離を推定（直線補完）する
  i = 0;
  do {
    i++;
  } while (WALL_DIST[i] > now_ss_l);
  distance = (i - 1) * 5 + (WALL_DIST[i] - now_ss_l) * 5 / (WALL_DIST[i] - WALL_DIST[i - 1]);
  SERIAL_OUT.printf("SS_L: %d  approx dist to wall: %d\n", now_ss_l, distance);
}
