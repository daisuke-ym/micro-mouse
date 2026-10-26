// ----------------------------------------------------------------------
// 車体固有の情報

// ----------------------------------------------------------------------
// Bluetooth のデバイス名
const char BT_NAME[] = "EAM_daisuke-han";

// ----------------------------------------------------------------------
// 車体の数値
const double STEPS = 524.0; // 1回転するのに必要なステップ数
const double WHEEL_D = 54.8; // ホイールの直径(mm)
const double TREAD = 98.0; // トレッド(mm)
const double MMPS = M_PI * WHEEL_D / STEPS; // 1ステップで進む距離(milli meter per step)
int MAX_SPEED = 400;
int MAX_ACCEL = 400;

// ----------------------------------------------------------------------
// センサーの値
volatile int SS_FL = 0;
volatile int SS_L = 0;
volatile int SS_R = 0;
volatile int SS_FR = 0;
// センサの初期値（スタート位置での値）
int INIT_SS_FL;
int INIT_SS_L;
int INIT_SS_R;
int INIT_SS_FR;

// ----------------------------------------------------------------------
// 壁があるかどうかの閾値
const int WALL_TV = 300; // 壁があるとみなすセンサ値の閾値
const int FL_LIMIT = 3760; // センサ値の上限(これ以上行くと壁に衝突する)
const int FR_LIMIT = 3855; // センサ値の上限(これ以上行くと壁に衝突する)

// ----------------------------------------------------------------------
// 壁センサ(SS_L)の値と壁までの距離マップ
const int WALL_DIST[] = {
  3670, // 0mm
  3650, // 5mm
  3370, // 10mm
  2800, // 15mm
  2320, // 20mm
  1895, // 25mm
  1565, // 30mm
  1320, // 35mm
  1140, // 40mm
   965, // 45mm
   845, // 50mm
   735, // 55mm
   655, // 60mm
};
// 通路中央に置いたときの壁までの距離
const int WALL_DIST_CENTER = 35;
// 通路中央に置いたときの SS_L の値
const int SS_L_CENTER = 1395;
