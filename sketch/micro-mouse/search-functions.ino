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
int decide_direction(uint8_t goal_x, uint8_t goal_y) {
  // 現在の方向を決定（左手法）
  if (SS_L <= WALL_TV ) {
    switch (MAZE.direction) {
      case DIR_NORTH:
        MAZE.direction = DIR_WEST; // 北から西に進む
        break;
      case DIR_EAST:
        MAZE.direction = DIR_NORTH; // 東から北に進む
        break;
      case DIR_SOUTH:
        MAZE.direction = DIR_EAST; // 南から東に進む
        break;
      case DIR_WEST:
        MAZE.direction = DIR_SOUTH; // 西から南に進む
        break;
    }
    go_left();
  }
  else if (SS_FL <= WALL_TV) {
    switch (MAZE.direction) {
      case DIR_NORTH:
        MAZE.direction = DIR_NORTH;
        break;
      case DIR_EAST:
        MAZE.direction = DIR_EAST;
        break;
      case DIR_SOUTH:
        MAZE.direction = DIR_SOUTH;
        break;
      case DIR_WEST:
        MAZE.direction = DIR_WEST;
        break;
    }
    go_forward();
  }
  else if (SS_R <= WALL_TV) {
    switch (MAZE.direction) {
      case DIR_NORTH:
        MAZE.direction = DIR_EAST; // 北から東に進む
        break;
      case DIR_EAST:
        MAZE.direction = DIR_SOUTH; // 東から南に進む
        break;
      case DIR_SOUTH:
        MAZE.direction = DIR_WEST; // 南から西に進む
        break;
      case DIR_WEST:
        MAZE.direction = DIR_NORTH; // 西から北に進む
        break;
    }
    go_right();
  }
  else {
    switch (MAZE.direction) {
      case DIR_NORTH:
        MAZE.direction = DIR_SOUTH;
        break;
      case DIR_EAST:
        MAZE.direction = DIR_WEST;
        break;
      case DIR_SOUTH:
        MAZE.direction = DIR_NORTH;
        break;
      case DIR_WEST:
        MAZE.direction = DIR_EAST;
        break;
    }
    go_uturn();
  }
  update_pos_and_walls(); // 現在位置と壁の状態を更新
  // デバッグ用
  print_maze();

  // ゴール判定
  if (MAZE.x == goal_x && MAZE.y == goal_y) {
    SERIAL_OUT.println("Goal Reached!");
    return 0;
  }
  else {
    return -1;
  }
}

// ----------------------------------------------------------------------
void go_forward() {
  // 180mm 前進
  run_to_mm(180.0);
}

// ----------------------------------------------------------------------
void go_left() {
  run_to_mm(40.0);
  delay(100);
  spinturn(-90.0);
  delay(100);
  //run_to_mm(-40.0);
  //run_to_mm(180.0);
  run_to_mm(-40.0 + 180.0);
}

// ----------------------------------------------------------------------
void go_right() {
  run_to_mm(40.0);
  delay(100);
  spinturn(90.0);
  delay(100);
  //run_to_mm(-40.0);
  //run_to_mm(180.0);
  run_to_mm(-40.0 + 180.0);
}

// ----------------------------------------------------------------------
void go_uturn() {
  // Uターン
  run_to_mm(40.0);
  delay(100);
  spinturn(180.0);
  delay(100);
  //run_to_mm(-40.0);
  //run_to_mm(180.0);
  run_to_mm(-40.0 + 180.0);
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
}

// ----------------------------------------------------------------------
// nビット左ローテーションする関数
uint8_t rotate_left(uint8_t x, uint8_t n) {
    return (x << n) | (x >> (8 - n));
}

// ----------------------------------------------------------------------
void print_maze() {
  lcd.setCursor(0, 0);
  lcd.print(MAZE.x);
  lcd.print(" ");
  lcd.setCursor(3, 0);
  lcd.print(MAZE.y);
  lcd.print(" ");
  // 通過済みセルの状態を表示
  SERIAL_OUT.println("Passed Cells:");
  for (int y = MAZE_SIZE - 1; y >= 0; y--) {
    for (int x = 0; x < MAZE_SIZE; x++) {
      if (MAZE.passed[x][y]) {
        SERIAL_OUT.printf("%2d", MAZE.passed[x][y]);
      } else {
        SERIAL_OUT.printf("%2d", 0);
      }
    }
    SERIAL_OUT.println();
  }
  // 迷路の状態を表示
  SERIAL_OUT.println("Maze:");
  for (int y = MAZE_SIZE - 1; y >= 0; y--) {
    for (int x = 0; x < MAZE_SIZE; x++) {
      // 北の壁
      if (MAZE.passed[x][y]) { // 通過済みセルは壁を表示する
        if (MAZE.walls[x][y] & 0b00010001) {
          SERIAL_OUT.print("###");
        }
        else {
          SERIAL_OUT.print("# #");
        }
      }
      else {
        SERIAL_OUT.print("   ");
      }
    }
    SERIAL_OUT.println();
    for (uint8_t x = 0; x < MAZE_SIZE; x++) {
      if (MAZE.passed[x][y]) { // 通過済みセルは壁を表示する
        if (MAZE.walls[x][y] & 0b00100010) { // 西の壁
          SERIAL_OUT.print("#");
        }
        else {
          SERIAL_OUT.print(" ");
        }
        if (MAZE.x == x && MAZE.y == y) { // マウスの位置
          switch (MAZE.direction) {
            case DIR_NORTH:
              SERIAL_OUT.print("^");
              break;
            case DIR_EAST:
              SERIAL_OUT.print(">");
              break;
            case DIR_SOUTH:
              SERIAL_OUT.print("v");
              break;
            case DIR_WEST:
              SERIAL_OUT.print("<");
              break;
          }
        }
        else {
          SERIAL_OUT.print(" ");
        }
        if (MAZE.walls[x][y] & 0b10001000) { // 東の壁
          SERIAL_OUT.print("#");
        }
        else {
          SERIAL_OUT.print(" ");
        }
      }
      else {
        SERIAL_OUT.print("   ");
      }
    }
    SERIAL_OUT.println();
    for (int x = 0; x < MAZE_SIZE; x++) {
      // 南の壁
      if (MAZE.passed[x][y]) { // 通過済みセルは壁を表示する
        if (MAZE.walls[x][y] & 0b01000100) {
          SERIAL_OUT.print("###");
        }
        else {
          SERIAL_OUT.print("# #");
        }
      }
      else {
        SERIAL_OUT.print("   ");
      }
    }
    SERIAL_OUT.println();
  }
}
