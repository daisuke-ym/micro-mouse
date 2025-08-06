  // センサと赤外線LED
const int SENSOR_FL = 35;
const int SENSOR_L  = 34;
const int SENSOR_R  = 39;
const int SENSOR_FR = 36;
const int IRLED_R_FL = 27;
const int IRLED_L_FR = 14;
// センサの値
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

// 車体の数値
const double WHEEL_D = 50.0; // ホイールの直径(mm)
const double TREAD = 103.0;  // トレッド
const double STEPS = 4096.0; // 1回転するのに必要なステップ数
const double MMPS = M_PI * WHEEL_D / STEPS; // 1ステップで進む距離(milli meter per step)

// ステッピングモータ
const unsigned long DEFAULT_STEP_DELAY = 1000;
Unistep2_mod stepperL(STP1P1, STP1P4, STP1P3, STP1P2, 4096, DEFAULT_STEP_DELAY); // stepperR と回転方向を合わせるため P2 と P4 を入れ替えている
Unistep2_mod stepperR(STP2P1, STP2P2, STP2P3, STP2P4, 4096, DEFAULT_STEP_DELAY);

// AQM1602Y LCD
ST7032_asukiaaa lcd;

// BNO055 センサ
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);
volatile double DIR_X; // オイラー角 X軸
volatile double DIR_Y; // オイラー角 Y軸
volatile double DIR_Z; // オイラー角 Z軸

// マウスの方向を示すenum
enum Direction {
  DIR_NORTH = 0, // 北
  DIR_WEST  = 1, // 西
  DIR_SOUTH = 2, // 南
  DIR_EAST  = 3, // 東
};

// 壁があるかどうかの閾値
const int WALL_TV = 1000; // 壁があるとみなすセンサ値の閾値

// 迷路の大きさ
#define MAZE_SIZE 4
// 迷路の構造を表す構造体
struct _MAZE {
  uint8_t x; // マウスの現在位置 X
  uint8_t y; // マウスの現在位置 Y
  uint8_t goal_x; // ゴール位置 X
  uint8_t goal_y; // ゴール位置 Y
  uint8_t walls[MAZE_SIZE][MAZE_SIZE]; // 迷路の壁を表す2次元配列
  // 壁の状態を表すビットフラグ
  // 壁ありは1、壁なしは0
  // 0b東南西北東南西北
  // ビットローテーションしたときに楽なように同じパターンを使う
  uint8_t passed[MAZE_SIZE][MAZE_SIZE]; // 通過済みのセルを表す2次元配列
  Direction direction; // マウスの現在の方向
};

struct _MAZE MAZE = {
  .x = 0,
  .y = 0,
  .goal_x = 3,
  .goal_y = 3,
  .direction = DIR_NORTH
};

