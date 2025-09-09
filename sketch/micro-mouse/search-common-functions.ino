/*
  迷路探索アルゴリズム共通の関数群
*/

// ----------------------------------------------------------------------
void init_maze() {
  // MAZEの初期化
  for (uint8_t i = 0; i < MAZE_SIZE; i++) {
    for (uint8_t j = 0; j < MAZE_SIZE; j++) {
        MAZE.walls[i][j] = 0; // 初期状態は壁はない
        MAZE.passed[i][j] = 0; // セルは未通過
    }
  }
  // 壁の状態を設定
  // 初期位置は北以外は壁がある
  MAZE.walls[0][0] = 0b11101110;
  // 初期位置は通過済み
  MAZE.passed[0][0] = 1;

  // 迷路の状態を表示
  print_maze();
}

// ----------------------------------------------------------------------
void update_pos_and_walls() {
  // 現在位置の更新
  switch (MAZE.direction) {
    case DIR_NORTH:
      MAZE.y++; // 北に進んだ場合
      break;
    case DIR_EAST:
      MAZE.x++; // 東に進んだ場合
      break;
    case DIR_SOUTH:
      MAZE.y--; // 南に進んだ場合
      break;
    case DIR_WEST:
      MAZE.x--; // 西に進んだ場合
      break;
  }
  // セルを通過済みにする
  MAZE.passed[MAZE.x][MAZE.y] += 1;
  // 壁の状態を調べる
  uint8_t walls = 0;
  if (SS_FL > WALL_TV) {
    walls |= 0b00010001; // 北の壁がある
  }
  if (SS_R > WALL_TV) {
    walls |= 0b10001000; // 東の壁がある
  }
  if (SS_L > WALL_TV) {
    walls |= 0b00100010; // 西の壁がある
  }
  // 壁の状態を更新
  switch (MAZE.direction) {
    case DIR_NORTH:
      MAZE.walls[MAZE.x][MAZE.y] = walls;
      break;
    case DIR_EAST:
      MAZE.walls[MAZE.x][MAZE.y] = rotate_left(walls, DIR_EAST);
      break;
    case DIR_SOUTH:
      MAZE.walls[MAZE.x][MAZE.y] = rotate_left(walls, DIR_SOUTH);
      break;
    case DIR_WEST:
      MAZE.walls[MAZE.x][MAZE.y] = rotate_left(walls, DIR_WEST);
      break;
  }
  // LCDに現在座標を表示
  lcd.setCursor(0, 0);
  lcd.print(MAZE.x);
  lcd.print(" ");
  lcd.setCursor(3, 0);
  lcd.print(MAZE.y);
  lcd.print(" ");
}

// ----------------------------------------------------------------------
// 車体の方角と次の進行方向から次の方角を更新する関数
void update_direction(int direction, int rel_dir) {
  switch (direction) {
    case DIR_NORTH:
      switch (rel_dir) {
        case REL_FORWARD:
          MAZE.direction = DIR_NORTH;
          break;
        case REL_LEFT:
          MAZE.direction = DIR_WEST;
          break;
        case REL_RIGHT:
          MAZE.direction = DIR_EAST;
          break;
        case REL_BACK:
          MAZE.direction = DIR_SOUTH;
          break;
      }
      break;
    case DIR_EAST:
      switch (rel_dir) {
        case REL_FORWARD:
          MAZE.direction = DIR_EAST;
          break;
        case REL_LEFT:
          MAZE.direction = DIR_NORTH;
          break;
        case REL_RIGHT:
          MAZE.direction = DIR_SOUTH;
          break;
        case REL_BACK:
          MAZE.direction = DIR_WEST;
          break;
      }
      break;
    case DIR_SOUTH:
      switch (rel_dir) {
        case REL_FORWARD:
          MAZE.direction = DIR_SOUTH;
          break;
        case REL_LEFT:
          MAZE.direction = DIR_EAST;
          break;
        case REL_RIGHT:
          MAZE.direction = DIR_WEST;
          break;
        case REL_BACK:
          MAZE.direction = DIR_NORTH;
          break;
      }
      break;
    case DIR_WEST:
      switch (rel_dir) {
        case REL_FORWARD:
          MAZE.direction = DIR_WEST;
          break;
        case REL_LEFT:
          MAZE.direction = DIR_SOUTH;
          break;
        case REL_RIGHT:
          MAZE.direction = DIR_NORTH;
          break;
        case REL_BACK:
          MAZE.direction = DIR_EAST;
          break;
      }
      break;
  }
}

// ----------------------------------------------------------------------
// 迷路の未探索区画数を返す関数 
int count_unsearched_cells() {
  int i = 0;

  for (int x = 0; x < MAZE_SIZE; x++) {
    for (int y = 0; y < MAZE_SIZE; y++) {
      if (MAZE.passed[x][y] == 0) {
        i++;
      }
    }
  }
  return i;
}

// ----------------------------------------------------------------------
// nビット左ローテーションする関数
uint8_t rotate_left(uint8_t x, uint8_t n) {
    return (x << n) | (x >> (8 - n));
}

// ----------------------------------------------------------------------
// 迷路の状態を表示する関数（改良版）
void print_maze() {
  // 迷路の状態を表示
  SERIAL_OUT.println("Maze:");
  // 北の外壁（北西隅）
  SERIAL_OUT.print("+");
  // 北の外壁
  for (int x = 0; x < MAZE_SIZE; x++) {
    SERIAL_OUT.print("---+");
  }
  SERIAL_OUT.println();

  for (int y = MAZE_SIZE - 1; y >= 0; y--) {
    // 西の外壁
    SERIAL_OUT.print("|");
    // 東の壁（東隣のセルの西の壁）
    for (int x = 0; x < MAZE_SIZE; x++) {
      if ((MAZE.walls[x][y] & 0b10001000) != 0 || (x < MAZE_SIZE - 1 && (MAZE.walls[x + 1][y] & 0b00100010) != 0)) {
        if (MAZE.x == x && MAZE.y == y) { // マウスの位置
          switch (MAZE.direction) {
            case DIR_NORTH:
              SERIAL_OUT.print(" ^ |");
              break;
            case DIR_EAST:
              SERIAL_OUT.print(" > |");
              break;
            case DIR_SOUTH:
              SERIAL_OUT.print(" v |");
              break;
            case DIR_WEST:
              SERIAL_OUT.print(" < |");
              break;
          }
        }
        else {
          SERIAL_OUT.print("   |");
        }
      }
      else {
        if (MAZE.x == x && MAZE.y == y) { // マウスの位置
          switch (MAZE.direction) {
            case DIR_NORTH:
              SERIAL_OUT.print(" ^  ");
              break;
            case DIR_EAST:
              SERIAL_OUT.print(" >  ");
              break;
            case DIR_SOUTH:
              SERIAL_OUT.print(" v  ");
              break;
            case DIR_WEST:
              SERIAL_OUT.print(" <  ");
              break;
          }
        }
        else {
          SERIAL_OUT.print("    ");
        }
      }
    }
    SERIAL_OUT.println();

    // 南の壁（南西端）
    SERIAL_OUT.print("+");
    // 南の壁（南隣のセルの北の壁）
    for (int x = 0; x < MAZE_SIZE; x++) {
      if ((MAZE.walls[x][y] & 0b01000100) != 0 || (y < MAZE_SIZE - 1 && (MAZE.walls[x][y - 1] & 0b00010001) != 0)) {
        SERIAL_OUT.print("---+");
      }
      else {
        SERIAL_OUT.print("   +");
      }
    }
    SERIAL_OUT.println();
  }
}
