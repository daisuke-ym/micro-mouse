/*
  機体固有の値を定義するヘッダファイル
  ここを変更することで、機体に合わせた調整が可能
*/

// ----------------------------------------------------------------------
// ピンの定義
// センサと赤外線LED
const int SENSOR_FL = 35;
const int SENSOR_L  = 34;
const int SENSOR_R  = 39;
const int SENSOR_FR = 36;
const int IRLED_R_FL = 27;
const int IRLED_L_FR = 14;
// センサの値(現在値)
volatile int SS_FL = 0;
volatile int SS_L = 0;
volatile int SS_R = 0;
volatile int SS_FR = 0;
// スイッチとLED
const int SW1 = 19;
const int SW2 = 13;
const int LED1 = 23;
const int LED2 = 12;
// ステッピングモータ左
const int STP1P1 = 16;
const int STP1P2 = 17;
const int STP1P3 = 5;
const int STP1P4 = 18;
// ステッピングモータ右
const int STP2P1 = 32;
const int STP2P2 = 33;
const int STP2P3 = 25;
const int STP2P4 = 26;

// ----------------------------------------------------------------------
// ステッピングモータ
const double STEPS = 4096.0; // 1回転するのに必要なステップ数
const unsigned long DEFAULT_STEP_DELAY = 1000; // 回転速度（unistep2_mod でのディレイ値：小さいほど回転が速い）

// ----------------------------------------------------------------------
// 車体の数値
const double WHEEL_D = 50.5; // ホイールの直径(mm)
const double TREAD = 98.0;  // トレッド
const double MMPS = M_PI * WHEEL_D / STEPS; // 1ステップで進む距離(milli meter per step)

// ----------------------------------------------------------------------
// 壁があるかどうかの閾値
const int WALL_TV = 1000; // 壁があるとみなすセンサ値の閾値
const int FL_LIMIT = 3860; // センサ値の上限(これ以上行くと壁に衝突する)
const int FR_LIMIT = 3955; // センサ値の上限(これ以上行くと壁に衝突する)

// ----------------------------------------------------------------------
// 壁センサ(SS_L)の値と壁までの距離マップ
const int WALL_DIST[] = {
  3810, // 0mm
  3730, // 5mm
  3600, // 10mm
  3140, // 15mm
  2590, // 20mm
  2280, // 25mm
  1990, // 30mm
  1765, // 35mm
  1525, // 40mm
  1390, // 45mm
  1260, // 50mm
  1170, // 53mm
};
// 通路中央に置いたときの SS_L の値(壁まで28㎜)
const int SS_L_CENTER = 2100;

