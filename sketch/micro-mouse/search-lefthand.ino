// ----------------------------------------------------------------------
int decide_direction_lefthand(uint8_t goal_x, uint8_t goal_y) {
  // 現在の方向を決定（左手法）
  if (SS_L <= WALL_TV ) {
    update_direction(MAZE.direction, REL_LEFT);
    go_left();
  }
  else if (SS_FL <= WALL_TV) {
    update_direction(MAZE.direction, REL_FORWARD);
    go_forward();
  }
  else if (SS_R <= WALL_TV) {
    update_direction(MAZE.direction, REL_RIGHT);
    go_right();
  }
  else {
    update_direction(MAZE.direction, REL_BACK);
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
int decide_direction_lefthand_pivotturn(uint8_t goal_x, uint8_t goal_y) {
  // 現在の方向を決定（左手法）
  if (SS_L <= WALL_TV ) {
    update_direction(MAZE.direction, REL_LEFT);
    go_left_pivot();
  }
  else if (SS_FL <= WALL_TV) {
    update_direction(MAZE.direction, REL_FORWARD);
    go_forward();
  }
  else if (SS_R <= WALL_TV) {
    update_direction(MAZE.direction, REL_RIGHT);
    go_right_pivot();
  }
  else {
    update_direction(MAZE.direction, REL_BACK);
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
