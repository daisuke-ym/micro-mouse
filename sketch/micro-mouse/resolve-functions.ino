/*
    最短経路を求める関数群
*/

// ----------------------------------------------------------------------
void run_shortest_path(int start_x, int start_y, int goal_x, int goal_y, Direction start_dir) {
  int i = 0;

  // 歩数図を作成
  make_steps_map(start_x, start_y, goal_x, goal_y);
  // 最短経路を求める
  resolve_shortest_path(start_x, start_y, start_dir);
  // 最短経路を実行
  while (SHORTEST_PATH[i] != -1) {
    // 最短経路に従って移動
    switch (SHORTEST_PATH[i]) {
      case GOTO_FORWARD:
        update_direction(MAZE.direction, REL_FORWARD);
        go_forward();
        break;
      case GOTO_LEFT:
        update_direction(MAZE.direction, REL_LEFT);
        go_left();
        break;
      case GOTO_RIGHT:
        update_direction(MAZE.direction, REL_RIGHT);
        go_right();
        break;
      case GOTO_BACK:
        update_direction(MAZE.direction, REL_BACK);
        go_uturn();
        break;
    }
    update_pos_and_walls();
    i++;
    delay(250);
  }
}

// ----------------------------------------------------------------------
// 歩数図を基に指定位置からゴールまでの最短経路を求める関数
// 
void resolve_shortest_path(int start_x, int start_y, Direction start_dir) {
  // 歩数図から最短経路を求める
  int x = start_x;
  int y = start_y;
  int next_x = 0;
  int next_y = 0;
  int path_length = 0;
  int path_value = STEPS_MAP[start_x][start_y]; // スタート地点の値
  int direction = start_dir; // 今向いている方向

  // SHORTEST_PATH を初期化
  for (int i = 0; i < MAZE_SIZE * MAZE_SIZE; i++) {
    SHORTEST_PATH[i] = -1; // -1 は未設定を示す
  }

  SERIAL_OUT.println("Resolving shortest path...");
  do {
    //SERIAL_OUT.printf("  Searching value: %d from (%d, %d)\n", path_value - 1, x, y);
    if (y + 1 < MAZE_SIZE && STEPS_MAP[x][y + 1] == path_value - 1) { // 北
      next_x = x;
      next_y = y + 1;
      switch (direction) {
        case DIR_NORTH: // 北向きから北に進む
          SHORTEST_PATH[path_length] = GOTO_FORWARD;
          break;
        case DIR_EAST: // 東向きから北に進む
          SHORTEST_PATH[path_length] = GOTO_LEFT;
          break;
        case DIR_SOUTH: // 南向きから北に進む(未使用)
          SHORTEST_PATH[path_length] = GOTO_BACK; // 後退(未使用)
          break;
        case DIR_WEST: // 西向きから北に進む
          SHORTEST_PATH[path_length] = GOTO_RIGHT;
          break;
      }
      direction = DIR_NORTH;
    }
    if (x - 1 >= 0 && STEPS_MAP[x - 1][y] == path_value - 1) { // 西
      next_x = x - 1;
      next_y = y;
      switch (direction) {
        case DIR_NORTH: // 北向きから西に進む
          SHORTEST_PATH[path_length] = GOTO_LEFT;
          break;
        case DIR_EAST: // 東向きから西に進む
          SHORTEST_PATH[path_length] = GOTO_BACK; // 後退(未使用)
          break;
        case DIR_SOUTH: // 南向きから西に進む
          SHORTEST_PATH[path_length] = GOTO_RIGHT;
          break;
        case DIR_WEST: // 西向きから西に進む
          SHORTEST_PATH[path_length] = GOTO_FORWARD;
          break;
      }
      direction = DIR_WEST;
    }
    if (y - 1 >= 0 && STEPS_MAP[x][y - 1] == path_value - 1) { // 南
      next_x = x;
      next_y = y - 1;
      switch (direction) {
        case DIR_NORTH: // 北向きから南に進む
          SHORTEST_PATH[path_length] = GOTO_BACK; // 後退(未使用)
          break;
        case DIR_EAST: // 東向きから南に進む
          SHORTEST_PATH[path_length] = GOTO_RIGHT;
          break;
        case DIR_SOUTH: // 南向きから南に進む
          SHORTEST_PATH[path_length] = GOTO_FORWARD;
          break;
        case DIR_WEST: // 西向きから南に進む
          SHORTEST_PATH[path_length] = GOTO_LEFT;
          break;
      }
      direction = DIR_SOUTH;
    }
    if (x + 1 < MAZE_SIZE && STEPS_MAP[x + 1][y] == path_value - 1) { // 東
      next_x = x + 1;
      next_y = y;
      switch (direction) {
        case DIR_NORTH: // 北向きから東に進む
          SHORTEST_PATH[path_length] = GOTO_RIGHT;
          break;
        case DIR_EAST: // 東向きから東に進む
          SHORTEST_PATH[path_length] = GOTO_FORWARD;
          break;
        case DIR_SOUTH: // 南向きから東に進む
          SHORTEST_PATH[path_length] = GOTO_LEFT;
          break;
        case DIR_WEST: // 西向きから東に進む
          SHORTEST_PATH[path_length] = GOTO_BACK; // 後退(未使用)
          break;
      }
      direction = DIR_EAST;
    }
    path_length++;
    path_value--;
    x = next_x;
    y = next_y;
  } while (path_value > 0);

  print_shortest_path();
}

// ----------------------------------------------------------------------
void print_shortest_path() {
  int i = 0;

  SERIAL_OUT.println("Shortest path:");
  do {
    SERIAL_OUT.printf("Step %d: %d (%c)\n", i, SHORTEST_PATH[i], GOTO_STR[SHORTEST_PATH[i]]);
    i++;
  } while (SHORTEST_PATH[i] != -1);
}

// ----------------------------------------------------------------------
void make_steps_map(int start_x, int start_y, int goal_x, int goal_y) {
  // テストデータをコピー
  //MAZE = TMAZE;
  print_maze();

  // 歩数図を初期化
  for (uint8_t y = 0; y < MAZE_SIZE; y++) {
    for (uint8_t x = 0; x < MAZE_SIZE; x++) {
      STEPS_MAP[x][y] = 255;
    }
  }
  STEPS_MAP[goal_x][goal_y] = 0; // ゴール位置は0

  int current_value = 0;
  while (update_steps_map(start_x, start_y, current_value) != 0) {
    current_value++;
    SERIAL_OUT.printf("Current value: %d", current_value);
    SERIAL_OUT.println();
  }

  print_steps_map(); // デバッグ用に歩数図を表示
}

// ----------------------------------------------------------------------
int update_steps_map(int start_x, int start_y, uint8_t value) {
  for (int y = 0; y < MAZE_SIZE; y++) {
    for (int x = 0; x < MAZE_SIZE; x++) {
      if (STEPS_MAP[x][y] == value) {
        //SERIAL_OUT.printf("Updating contour map at (%d, %d) with value %d", x, y, value);
        //SERIAL_OUT.println();
        // 周囲の壁を調査
        if ((MAZE.walls[x][y] & 0b00010001) == 0) { // 北の壁
          if (y + 1 < MAZE_SIZE) { // 配列の範囲外チェック
            if (MAZE.passed[x][y + 1] != 0 && STEPS_MAP[x][y + 1] == 255) { // 通過済みセル and 等高線が更新されてない場合
              STEPS_MAP[x][y + 1] = value + 1;
              if (x == start_x && (y + 1) == start_y) return 0; // スタート地点に到達したら終了
            }
          }
        }
        if ((MAZE.walls[x][y] & 0b00100010) == 0) { // 西の壁
          if (x - 1 >= 0) { // 配列の範囲外チェック
            if (MAZE.passed[x - 1][y] != 0 && STEPS_MAP[x - 1][y] == 255) { // 通過済みセル and 等高線が更新されてない場合
              STEPS_MAP[x - 1][y] = value + 1;
              if ((x - 1) == start_x && y == start_y) return 0; // スタート地点に到達したら終了
            }
          } 
        }
        if ((MAZE.walls[x][y] & 0b01000100) == 0) { // 南の壁
          if (y - 1 >= 0) { // 配列の範囲外チェック
            if (MAZE.passed[x][y - 1] != 0 && STEPS_MAP[x][y - 1] == 255) { // 通過済みセル and 等高線が更新されてない場合
              STEPS_MAP[x][y - 1] = value + 1;
              if (x == start_x && (y - 1) == start_y) return 0; // スタート地点に到達したら終了
            }
          }
        }
        if ((MAZE.walls[x][y] & 0b10001000) == 0) { // 東の壁
          if (x + 1 < MAZE_SIZE) { // 配列の範囲外チェック
            if (MAZE.passed[x + 1][y] != 0 && STEPS_MAP[x + 1][y] == 255) { // 通過済みセル and 等高線が更新されてない場合
              STEPS_MAP[x + 1][y] = value + 1;
              if ((x + 1) == start_x && y == start_y) return 0; // スタート地点に到達したら終了
            }
          }
        }
      }
    }
  }
  return -1;
}

// ----------------------------------------------------------------------
void print_steps_map() {
  for (int y = MAZE_SIZE -1 ; y >= 0; y--) {
    for (int x = 0; x < MAZE_SIZE; x++) {
      SERIAL_OUT.printf("%4d ", STEPS_MAP[x][y]);
    }
    SERIAL_OUT.println();
  }
  SERIAL_OUT.println();
}
