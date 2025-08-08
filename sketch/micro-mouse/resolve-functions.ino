/*
    最短経路を求める関数群
*/

// ----------------------------------------------------------------------
void resolve_shortest_path() {
  // A*アルゴリズムを用いて最短経路を探索する
  // ここに実装を追加
}

// ----------------------------------------------------------------------
void make_contour_map() {
  // テストデータをコピー
  //MAZE = TMAZE;
  //print_maze();

  // 等高線図を初期化
  for (uint8_t y = 0; y < MAZE_SIZE; y++) {
    for (uint8_t x = 0; x < MAZE_SIZE; x++) {
      CONTOUR_MAP[x][y] = 255;
    }
  }
  CONTOUR_MAP[MAZE_GOAL_X][MAZE_GOAL_Y] = 0; // ゴール位置は0

  int current_value = 0;
  while (update_contour_map(current_value) != 0) {
    current_value++;
    SERIAL_OUT.printf("Current value: %d", current_value);
    SERIAL_OUT.println();
  }

  print_contour_map(); // デバッグ用に等高線図を表示
}

// ----------------------------------------------------------------------
int update_contour_map(uint8_t value) {
  for (int y = 0; y < MAZE_SIZE; y++) {
    for (int x = 0; x < MAZE_SIZE; x++) {
      if (CONTOUR_MAP[x][y] == value) {
        //SERIAL_OUT.printf("Updating contour map at (%d, %d) with value %d", x, y, value);
        //SERIAL_OUT.println();
        // 周囲の壁を調査
        if ((MAZE.walls[x][y] & 0b00010001) == 0) { // 北の壁
          if (y + 1 < MAZE_SIZE) { // 配列の範囲外チェック
            if (MAZE.passed[x][y + 1] != 0 && CONTOUR_MAP[x][y + 1] == 255) { // 通過済みセル and 等高線が更新されてない場合
              CONTOUR_MAP[x][y + 1] = value + 1;
              if (x == 0 && (y + 1) == 0) return 0; // スタート地点に到達したら終了
            }
          }
        }
        if ((MAZE.walls[x][y] & 0b00100010) == 0) { // 西の壁
          if (x - 1 >= 0) { // 配列の範囲外チェック
            if (MAZE.passed[x - 1][y] != 0 && CONTOUR_MAP[x - 1][y] == 255) { // 通過済みセル and 等高線が更新されてない場合
              CONTOUR_MAP[x - 1][y] = value + 1;
              if ((x - 1) == 0 && y == 0) return 0; // スタート地点に到達したら終了
            }
          } 
        }
        if ((MAZE.walls[x][y] & 0b01000100) == 0) { // 南の壁
          if (y - 1 >= 0) { // 配列の範囲外チェック
            if (MAZE.passed[x][y - 1] != 0 && CONTOUR_MAP[x][y - 1] == 255) { // 通過済みセル and 等高線が更新されてない場合
              CONTOUR_MAP[x][y - 1] = value + 1;
              if (x == 0 && (y - 1) == 0) return 0; // スタート地点に到達したら終了
            }
          }
        }
        if ((MAZE.walls[x][y] & 0b10001000) == 0) { // 東の壁
          if (x + 1 < MAZE_SIZE) { // 配列の範囲外チェック
            if (MAZE.passed[x + 1][y] != 0 && CONTOUR_MAP[x + 1][y] == 255) { // 通過済みセル and 等高線が更新されてない場合
              CONTOUR_MAP[x + 1][y] = value + 1;
              if ((x + 1) == 0 && y == 0) return 0; // スタート地点に到達したら終了
            }
          }
        }
      }
    }
  }
  return -1;
}

// ----------------------------------------------------------------------
void print_contour_map() {
  for (int y = MAZE_SIZE -1 ; y >= 0; y--) {
    for (int x = 0; x < MAZE_SIZE; x++) {
      SERIAL_OUT.printf("%4d ", CONTOUR_MAP[x][y]);
    }
    SERIAL_OUT.println();
  }
  SERIAL_OUT.println();
}
