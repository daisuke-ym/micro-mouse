/*
  グローバル変数やマクロの定義
*/

// センサの値(現在値)
volatile int SS_FL = 0;
volatile int SS_L = 0;
volatile int SS_R = 0;
volatile int SS_FR = 0;
// センサの初期値（スタート位置での値）
int INIT_SS_FL;
int INIT_SS_L;
int INIT_SS_R;
int INIT_SS_FR;

// ステッピングモータ
Unistep2_mod stepperL(STP1P1, STP1P4, STP1P3, STP1P2, (int)STEPS, DEFAULT_STEP_DELAY); // stepperR と回転方向を合わせるため P2 と P4 を入れ替えている
Unistep2_mod stepperR(STP2P1, STP2P2, STP2P3, STP2P4, (int)STEPS, DEFAULT_STEP_DELAY);

// Bluetooth シリアル
BluetoothSerial SerialBT;
#define SERIAL_OUT SerialBT
//#define SERIAL_OUT Serial

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

// マウスから見た相対方向を示すenum
enum RelativeDirection {
  REL_FORWARD = 0, // 前方
  REL_LEFT    = 1, // 左方
  REL_RIGHT   = 2, // 右方
  REL_BACK    = 3, // 後方
};

// 最短経路を辿るときの方向を示すenum
enum ShortestPathToGo {
  GOTO_FORWARD = 0, // 前進
  GOTO_LEFT    = 1, // 左回り
  GOTO_RIGHT   = 2, // 右回り
  GOTO_BACK    = 3, // 後退(未使用)
};
char GOTO_STR[] = {'F', 'L', 'R', 'B'};

// 迷路の大きさ
#define MAZE_SIZE 4
// 迷路のゴール座標
#define MAZE_GOAL_X 3
#define MAZE_GOAL_Y 3
// 迷路の構造を表す構造体
struct _MAZE {
  uint8_t x; // マウスの現在位置 X
  uint8_t y; // マウスの現在位置 Y
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
  .direction = DIR_NORTH
};

// 歩数図
uint8_t STEPS_MAP[MAZE_SIZE][MAZE_SIZE];

// 最短経路
int SHORTEST_PATH[MAZE_SIZE * MAZE_SIZE];

// 歩数図作成時の作成種類
enum StepsMapType {
  SMT_NORMAL = 1, // 通常の歩数図
  SMT_FF = 3, // 直進優先の歩数図
};

// 歩数図作成用のテストデータ
// +-+-+-+-+
// | |     |
// + +-+ +-+
// | | | | |
// + + + + +
// |   |   |
// + + +-+ +
// | |     |
// +-+-+-+-+
struct _MAZE TMAZE = {
  .x = 0,
  .y = 0,
  .walls = {{0b11101110, 0b00100010, 0b10101010, 0b10111011},
            {0b01100110, 0b10001000, 0b10111011, 0b01110111},
            {0b01010101, 0b01100110, 0b10101010, 0b00010001},
            {0b11001100, 0b10001000, 0b10111011, 0b11011101}},
  .passed = {{1, 1, 1, 1},
             {1, 1, 1, 1},
             {1, 1, 1, 1},
             {1, 1, 1, 1}},
  .direction = DIR_NORTH
};

// 歩数図作成用のテストデータ2
// +-+-+-+-+
// |     | |
// + +-+-+ +
// |       |
// + +-+ + +
// |     | |
// + + +-+ +
// | |   | |
// +-+-+-+-+
struct _MAZE TMAZE2 = {
  .x = 0,
  .y = 0,
  .walls = {{0b11101110, 0b00100010, 0b00100010, 0b00110011},
            {0b01100110, 0b00010001, 0b01010101, 0b01010101},
            {0b11011101, 0b11001100, 0b00010001, 0b11011101},
            {0b11101110, 0b10101010, 0b10001000, 0b10111011}},
  .passed = {{1, 1, 1, 1},
             {1, 1, 1, 1},
             {1, 1, 1, 1},
             {1, 1, 1, 1}},
  .direction = DIR_NORTH
};

// ----------------------------------------------------------------------
// マウスの状態を示すフラグ
uint8_t STATE_FLAG = 0;
// ビットフラグの定義 
#define SF_RUNNING_O        0b10000000 // マウスが走行中（往路）
#define SF_RUNNING_R        0b01000000 // マウスが走行中（復路）
#define SF_RUNNING_S        0b00100000 // マウスが走行中（最短経路）
#define SF_SEARCH_COMPLETE  0b00010000 // 迷路全探索完了
#define SF_ADJUST_TO_CENTER 0b00001000 // マウスを通路中央に調整中
