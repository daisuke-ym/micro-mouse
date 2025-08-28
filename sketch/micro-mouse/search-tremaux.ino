// ----------------------------------------------------------------------
// トレモー法による方向決定関数
int decide_direction_tremaux(uint8_t goal_x, uint8_t goal_y) {
  // デバッグ用
  SERIAL_OUT.printf("L wall: %d   passed: %d\n", SS_L <= WALL_TV ? 0 : 1, get_passed(MAZE.x, MAZE.y, MAZE.direction, REL_LEFT));
  SERIAL_OUT.printf("F wall: %d   passed: %d\n", SS_FL <= WALL_TV ? 0 : 1, get_passed(MAZE.x, MAZE.y, MAZE.direction, REL_FORWARD));
  SERIAL_OUT.printf("R wall: %d   passed: %d\n", SS_R <= WALL_TV ? 0 : 1, get_passed(MAZE.x, MAZE.y, MAZE.direction, REL_RIGHT));
  SERIAL_OUT.println();

  if (SS_L <= WALL_TV && get_passed(MAZE.x, MAZE.y, MAZE.direction, REL_LEFT) == 0) {
    // 左に壁がなく、一度も通ってないなら左に曲がる
    SERIAL_OUT.printf("if 1: get_passed: %d\n", get_passed(MAZE.x, MAZE.y, MAZE.direction, REL_LEFT));
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
  else if (SS_FL <= WALL_TV && get_passed(MAZE.x, MAZE.y, MAZE.direction, REL_FORWARD) == 0) {
    // 前方に壁がなく、一度も通ってないなら前に進む
    SERIAL_OUT.printf("if 2: get_passed: %d\n", get_passed(MAZE.x, MAZE.y, MAZE.direction, REL_FORWARD));
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
  else if (SS_R <= WALL_TV && get_passed(MAZE.x, MAZE.y, MAZE.direction, REL_RIGHT) == 0) {
    // 右に壁がなく、一度も通ってないなら右に曲がる
    SERIAL_OUT.printf("if 3: get_passed: %d\n", get_passed(MAZE.x, MAZE.y, MAZE.direction, REL_RIGHT));
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
  else if (SS_L > WALL_TV && SS_R > WALL_TV && SS_FL > WALL_TV && get_passed(MAZE.x, MAZE.y, MAZE.direction, REL_BACK) == 1) {
    // 三方壁で、後ろが一度だけ通ったならUターン
    SERIAL_OUT.printf("if 4: get_passed: %d\n", get_passed(MAZE.x, MAZE.y, MAZE.direction, REL_BACK));
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
  else if (SS_L <= WALL_TV && 
            get_passed(MAZE.x, MAZE.y, MAZE.direction, REL_LEFT) < get_passed(MAZE.x, MAZE.y, MAZE.direction, REL_FORWARD) &&
            get_passed(MAZE.x, MAZE.y, MAZE.direction, REL_LEFT) < get_passed(MAZE.x, MAZE.y, MAZE.direction, REL_RIGHT)) {
    // 左に壁がなく、左の通過回数が前・右より少ないなら左に曲がる
    SERIAL_OUT.printf("if 5: get_passed: %d %d %d\n", get_passed(MAZE.x, MAZE.y, MAZE.direction, REL_LEFT), get_passed(MAZE.x, MAZE.y, MAZE.direction, REL_FORWARD), get_passed(MAZE.x, MAZE.y, MAZE.direction, REL_RIGHT));
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
  else if (SS_FL <= WALL_TV && get_passed(MAZE.x, MAZE.y, MAZE.direction, REL_FORWARD) < get_passed(MAZE.x, MAZE.y, MAZE.direction, REL_RIGHT)) {
    // 前に壁がなく、前の通過回数が右より少ないなら前に進む
    SERIAL_OUT.printf("if 6: get_passed: %d %d\n", get_passed(MAZE.x, MAZE.y, MAZE.direction, REL_FORWARD), get_passed(MAZE.x, MAZE.y, MAZE.direction, REL_RIGHT));
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
    // 右に壁がないなら右に曲がる
    SERIAL_OUT.printf("if 7: \n");
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
    // それ以外はUターン
    SERIAL_OUT.printf("if 8: \n");
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
  // 現在位置と壁の状態を更新
  update_pos_and_walls();
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
// x, y で示された座標から direction1 で示されるマウスの進行方向と
// direction2 で示される方向の通過回数を返す
// ただし、境界外の場合、壁がある場合は255を返す
int get_passed(uint8_t x, uint8_t y, uint8_t direction1, uint8_t direction2) {
  switch (direction1) {
    case DIR_NORTH:
      switch (direction2) {
        case REL_FORWARD:
          if (y + 1 < MAZE_SIZE && (MAZE.walls[x][y] & 0b00010001) == 0) {
            return MAZE.passed[x][y + 1];
          }
          else {
            return 255; // 境界外 or 壁あり
          }
          break;
        case REL_LEFT:
          if (x - 1 >= 0 && (MAZE.walls[x][y] & 0b00100010) == 0) {
            return MAZE.passed[x - 1][y];
          }
          else {
            return 255; // 境界外 or 壁あり
          }
          break;
        case REL_RIGHT:
          if (x + 1 < MAZE_SIZE && (MAZE.walls[x][y] & 0b10001000) == 0) {
            return MAZE.passed[x + 1][y];
          }
          else {
            return 255; // 境界外 or 壁あり
          }
          break;
        case REL_BACK:
          if (y - 1 >= 0 && (MAZE.walls[x][y] & 0b01000100) == 0) {
            return MAZE.passed[x][y - 1];
          }
          else {
            return 255; // 境界外 or 壁あり
          }
          break;
      }
      break;
    case DIR_EAST:
      switch (direction2) {
        case REL_FORWARD:
          if (x + 1 < MAZE_SIZE && (MAZE.walls[x][y] & 0b10001000) == 0) {
            return MAZE.passed[x + 1][y];
          }
          else {
            return 255; // 境界外 or 壁あり
          }
          break;
        case REL_LEFT:
          if (y + 1 < MAZE_SIZE && (MAZE.walls[x][y] & 0b00010001) == 0) {
            return MAZE.passed[x][y + 1];
          }
          else {
            return 255; // 境界外 or 壁あり
          }
          break;
        case REL_RIGHT:
          if (y - 1 >= 0 && (MAZE.walls[x][y] & 0b01000100) == 0) {
            return MAZE.passed[x][y - 1];
          }
          else {
            return 255; // 境界外 or 壁あり
          }
          break;
        case REL_BACK:
          if (x - 1 >= 0 && (MAZE.walls[x][y] & 0b00100010) == 0) {
            return MAZE.passed[x - 1][y];
          }
          else {
            return 255; // 境界外 or 壁あり
          }
          break;
      }
      break;
    case DIR_SOUTH:
      switch (direction2) {
        case REL_FORWARD:
          if (y - 1 >= 0 && (MAZE.walls[x][y] & 0b01000100) == 0) {
            return MAZE.passed[x][y - 1];
          }
          else {
            return 255; // 境界外 or 壁あり
          }
          break;
        case REL_LEFT:
          if (x + 1 < MAZE_SIZE && (MAZE.walls[x][y] & 0b10001000) == 0) {
            return MAZE.passed[x - 1][y];
          }
          else {
            return 255; // 境界外 or 壁あり
          }
          break;
        case REL_RIGHT:
          if (x -1 >= 0 && (MAZE.walls[x][y] & 0b00100010) == 0) {
            return MAZE.passed[x + 1][y];
          }
          else {
            return 255; // 境界外 or 壁あり
          }
          break;
        case REL_BACK:
          if (y + 1 < MAZE_SIZE && (MAZE.walls[x][y] & 0b00010001) == 0) {
            return MAZE.passed[x][y + 1];
          }
          else {
            return 255; // 境界外 or 壁あり
          }
          break;
      }
      break;
    case DIR_WEST:
      switch (direction2) {
        case REL_FORWARD:
          if (x - 1 >= 0 && (MAZE.walls[x][y] & 0b00100010) == 0) {
            return MAZE.passed[x - 1][y];
          }
          else {
            return 255; // 境界外 or 壁あり
          }
          break;
        case REL_LEFT:
          if (y - 1 >= 0 && (MAZE.walls[x][y] & 0b01000100) == 0) {
            return MAZE.passed[x][y - 1];
          }
          else {
            return 255; // 境界外 or 壁あり
          }
          break;
        case REL_RIGHT:
          if (y + 1 < MAZE_SIZE && (MAZE.walls[x][y] & 0b00010001) == 0) {
            return MAZE.passed[x][y + 1];
          }
          else {
            return 255; // 境界外 or 壁あり
          }
          break;
        case REL_BACK:
          if (x + 1 < MAZE_SIZE && (MAZE.walls[x][y] & 0b10001000) == 0) {
            return MAZE.passed[x + 1][y];
          }
          else {
            return 255; // 境界外 or 壁あり
          }
          break;
      }
      break;
  }
}

// ----------------------------------------------------------------------
