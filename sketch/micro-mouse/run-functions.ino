// ----------------------------------------------------------------------
void go_forward() {
  //run_to_mm(180.0);
  run_to_mm_keep_wall_dist(180.0);
}

// ----------------------------------------------------------------------
void go_left() {
  run_to_mm(40.0);
  delay(100);
  spin_turn(-90.0);
  delay(100);
  run_to_mm(-40.0 + 180.0);
}

// ----------------------------------------------------------------------
void go_right() {
  run_to_mm(40.0);
  delay(100);
  spin_turn(90.0);
  delay(100);
  run_to_mm(-40.0 + 180.0);
}

// ----------------------------------------------------------------------
void go_uturn() {
  run_to_mm(40.0);
  delay(100);
  spin_turn(180.0);
  delay(100);
  MAX_SPEED = 200; // Uターン時は速度を落とす
  MAX_ACCEL = 200; // Uターン時は加速度も落とす
  adjust_to_center();
  MAX_SPEED = 400; // Uターン後は速度を元に戻す
  MAX_ACCEL = 400; // Uターン後は加速度も元に戻す
  run_to_mm(180.0);
}

// ----------------------------------------------------------------------
void go_left_pivot() {
  pivot_turn(-90.0);
  delay(100);
  run_to_mm(86.0);
}

// ----------------------------------------------------------------------
void go_right_pivot() {
  pivot_turn(90.0);
  delay(100);
  run_to_mm(86.0);
}

// ----------------------------------------------------------------------
// mm単位で指定した距離だけ走る
// マイナス値は後退
void run_to_mm(double distance) {
  STP_L.setMaxSpeed(MAX_SPEED);
  STP_R.setMaxSpeed(MAX_SPEED);
  STP_L.setAcceleration(MAX_ACCEL);
  STP_R.setAcceleration(MAX_ACCEL);
  STP_L.move(distance / MMPS);
  STP_R.move(distance / MMPS);
  while (STP_L.distanceToGo() != 0 && STP_R.distanceToGo() !=0) {
    STP_L.run();
    STP_R.run();
    if (SS_FL > FL_LIMIT || SS_FR > FR_LIMIT) {
      STP_L.setCurrentPosition(0); // 2回目以降呼び出したとき急加速しないようにする処理
      STP_R.setCurrentPosition(0);
      break;
    }
  }
  STP_L.move(0); // 2回目以降呼び出したときに蛇行しないようにする処理
  STP_R.move(0);
  STP_L.disableOutputs(); // 停止後モーターの発熱を抑える処理
  STP_R.disableOutputs();
}

// ----------------------------------------------------------------------
// 左壁との距離を保ちながら指定距離だけ移動する
void run_to_mm_keep_wall_dist(double distance) {
  const int base_speed = MAX_SPEED;
  const int min_speed = base_speed / 2;
  const int wall_dist_correction_divisor = 4; // 補正係数
  const int direction = distance < 0 ? -1 : 1;

  // 左右のモーターに加速度と移動目標を設定する
  STP_L.setAcceleration(MAX_ACCEL);
  STP_R.setAcceleration(MAX_ACCEL);
  STP_L.move(distance / MMPS);
  STP_R.move(distance / MMPS);

  // 目標位置に着くまで、壁センサーの値に応じて左右の速度を補正する
  while (STP_L.distanceToGo() != 0 || STP_R.distanceToGo() != 0) {
    int left_speed = base_speed;
    int right_speed = base_speed;

    // 左壁を検知している場合、中央時のセンサー値との差を速度補正量にする
    if (SS_L > WALL_TV) {
      const int correction = (SS_L - SS_L_CENTER) / wall_dist_correction_divisor;
      left_speed += direction * correction;
      right_speed -= direction * correction;
      // 補正後の速度が基準速度を超えたり、下がりすぎたりしないよう制限する
      left_speed = constrain(left_speed, min_speed, base_speed);
      right_speed = constrain(right_speed, min_speed, base_speed);
    }

    // 補正した速度で左右のモーターを駆動する
    STP_L.setMaxSpeed(left_speed);
    STP_R.setMaxSpeed(right_speed);
    STP_L.run();
    STP_R.run();

    // 前方センサーが衝突上限を超えたら移動を中断する
    if (SS_FL > FL_LIMIT || SS_FR > FR_LIMIT) {
      STP_L.setCurrentPosition(0);
      STP_R.setCurrentPosition(0);
      break;
    }
  }

  // 移動目標を現在位置に戻し、停止後にモーター出力を無効化する
  STP_L.move(0);
  STP_R.move(0);
  STP_L.disableOutputs();
  STP_R.disableOutputs();
}

// ----------------------------------------------------------------------
// 指定した角度だけ回転する（トレッドの中心で旋回）
// angle: 角度(度単位)
// 右回りは正、左回りは負
void spin_turn(double angle) {
  // 右回りを正とする
  double dist = TREAD * M_PI * angle / 360.0 / MMPS;
  STP_L.setMaxSpeed(MAX_SPEED);
  STP_R.setMaxSpeed(MAX_SPEED);
  STP_L.setAcceleration(MAX_ACCEL);
  STP_R.setAcceleration(MAX_ACCEL);
  STP_L.move(dist);
  STP_R.move(-dist);
  while (STP_L.distanceToGo() != 0 && STP_R.distanceToGo() !=0) {
    STP_L.run();
    STP_R.run();
  }
  STP_L.move(0); // 2回目以降呼び出したときに蛇行しないようにする処理
  STP_R.move(0);
  STP_L.disableOutputs(); // 停止後モーターの発熱を抑える処理
  STP_R.disableOutputs();
}

// ----------------------------------------------------------------------
// 指定した角度だけ回転する（車輪の中心で旋回）
// angle: 角度(度単位)
// 右回りは正、左回りは負
void pivot_turn(double angle) {
  double a = angle / 360.0 * TREAD * 2.0 * M_PI / MMPS;
  if (angle > 0) {
    STP_L.move(a);
    while (STP_L.distanceToGo() != 0) {
      STP_L.run();
    }
    STP_L.move(0);
    STP_L.disableOutputs();
  } else {
    STP_R.move(-1 * a);
    while (STP_R.distanceToGo() != 0) {
      STP_R.run();
    }
    STP_R.move(0);
    STP_R.disableOutputs();
  }
  STP_L.move(0); // 2回目以降呼び出したときに蛇行しないようにする処理
  STP_R.move(0);
  STP_L.disableOutputs(); // 停止後モーターの発熱を抑える処理
  STP_R.disableOutputs();
}

// ----------------------------------------------------------------------
// IMUのオイラー角を見て指定した角度だけ回転する（トレッドの中心で旋回）
// angle: 角度(度単位)
// 右回りは正、左回りは負
void spin_turn_imu(double angle) {
  angle = fmod(angle, 360.0);
  double init_dir_x = DIR_X + 360.0; // 初期方向
  double target_dir_x = init_dir_x + angle; // 目標方向
  /*
  // デバッグ用にシリアル出力
  Serial.print("init_dir_x: ");
  Serial.println(init_dir_x);
  Serial.print("target_dir_x: ");
  Serial.println(target_dir_x);
  Serial.print("angle: ");
  Serial.println(angle);
  */
  if (angle >= 0.0) {
    STP_L.move(4000);
    STP_R.move(-4000);
    while ((DIR_X + 360.0) < target_dir_x) {
      STP_L.run();
      STP_R.run();
    }
  }
  else {
    STP_L.move(-4000);
    STP_R.move(4000);
    while ((DIR_X + 360.0) > target_dir_x) {
      STP_L.run();
      STP_R.run();
    }
  }
  STP_L.move(0); // 2回目以降呼び出したときに蛇行しないようにする処理
  STP_R.move(0);
  STP_L.disableOutputs(); // 停止後モーターの発熱を抑える処理
  STP_R.disableOutputs();
}
