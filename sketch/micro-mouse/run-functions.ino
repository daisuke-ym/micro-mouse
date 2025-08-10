// ----------------------------------------------------------------------
// mm単位で指定した距離だけ走る
// マイナス値は後退
void run_to_mm(double distance) {
  stepperL.move(distance / MMPS);
  stepperR.move(distance / MMPS);
  while (stepperL.stepsToGo() != 0 && stepperR.stepsToGo() != 0) {
    stepperL.run();
    stepperR.run();
  }
  stepperL.stop();
  stepperR.stop();
}

// ----------------------------------------------------------------------
// 指定した角度だけ回転する（トレッドの中心で旋回）
// angle: 角度(度単位)
// 右回りは正、左回りは負
void spinturn(double angle) {
  double a = angle / 360.0 * TREAD * M_PI / MMPS;
  stepperL.move(a);
  stepperR.move(-1 * a);
  while (stepperL.stepsToGo() != 0 && stepperR.stepsToGo() != 0) {
    stepperL.run();
    stepperR.run();
  }
  stepperL.stop();
  stepperR.stop();
}

// ----------------------------------------------------------------------
// 指定した角度だけ回転する（車輪の中心で旋回）
// angle: 角度(度単位)
// 右回りは正、左回りは負
void pivoturn(double angle) {
  double a = angle / 360.0 * TREAD * 2.0 * M_PI / MMPS;
  if (angle > 0) {
    stepperL.move(a);
    while (stepperL.stepsToGo() != 0) {
      stepperL.run();
    }
    stepperL.stop();
  } else {
    stepperR.move(-1 * a);
    while (stepperR.stepsToGo() != 0) {
      stepperR.run();
    }
    stepperR.stop();
  }
  stepperL.stop();
  stepperR.stop();
}

// ----------------------------------------------------------------------
// IMUのオイラー角を見て指定した角度だけ回転する（トレッドの中心で旋回）
// angle: 角度(度単位)
// 右回りは正、左回りは負
void spinturn_imu(double angle) {
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
    stepperL.move(4000);
    stepperR.move(-4000);
    while ((DIR_X + 360.0) < target_dir_x) {
      stepperL.run();
      stepperR.run();
    }
  }
  else {
    stepperL.move(-4000);
    stepperR.move(4000);
    while ((DIR_X + 360.0) > target_dir_x) {
      stepperL.run();
      stepperR.run();
    }
  }
  stepperL.move(0);
  stepperR.move(0);
  stepperL.stop();
  stepperR.stop();
}

