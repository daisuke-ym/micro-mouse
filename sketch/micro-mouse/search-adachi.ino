/*
  足立法による迷路探索
*/

// ----------------------------------------------------------------------
// 指定した方角に対応する壁情報のビットマスクを返す
uint8_t adachi_wall_mask(Direction direction) {
  switch (direction) {
    case DIR_NORTH:
      return 0b00010001;
    case DIR_WEST:
      return 0b00100010;
    case DIR_SOUTH:
      return 0b01000100;
    case DIR_EAST:
      return 0b10001000;
  }
  return 0;
}

// ----------------------------------------------------------------------
// 指定した区画の隣にある区画を求める
// 迷路の外側を指す場合は false を返す
bool adachi_get_neighbor(uint8_t x, uint8_t y, Direction direction, uint8_t *next_x, uint8_t *next_y) {
  *next_x = x;
  *next_y = y;

  switch (direction) {
    case DIR_NORTH:
      if (y + 1 >= MAZE_SIZE) return false;
      *next_y = y + 1;
      return true;
    case DIR_WEST:
      if (x == 0) return false;
      *next_x = x - 1;
      return true;
    case DIR_SOUTH:
      if (y == 0) return false;
      *next_y = y - 1;
      return true;
    case DIR_EAST:
      if (x + 1 >= MAZE_SIZE) return false;
      *next_x = x + 1;
      return true;
  }
  return false;
}

// ----------------------------------------------------------------------
// 壁情報を現在の区画と隣接区画の両方に記録する
// これにより、どちらの区画から参照しても壁の状態が一致する
void adachi_record_wall(Direction direction) {
  uint8_t next_x, next_y;
  MAZE.walls[MAZE.x][MAZE.y] |= adachi_wall_mask(direction);

  if (adachi_get_neighbor(MAZE.x, MAZE.y, direction, &next_x, &next_y)) {
    const Direction opposite = (Direction)(((uint8_t)direction + 2) % 4);
    MAZE.walls[next_x][next_y] |= adachi_wall_mask(opposite);
  }
}

// ----------------------------------------------------------------------
// センサーが検知した現在位置の壁を、迷路上の方角に変換して記録する
void adachi_record_current_walls() {
  uint8_t relative_walls = 0;
  if (SS_FL > WALL_TV) relative_walls |= 0b00010001;
  if (SS_L > WALL_TV) relative_walls |= 0b00100010;
  if (SS_R > WALL_TV) relative_walls |= 0b10001000;

  // 車体の向きに合わせて、前後左右の壁情報を迷路の方角に変換する
  const uint8_t walls = rotate_left(relative_walls, MAZE.direction);
  for (uint8_t i = 0; i < 4; i++) {
    const Direction direction = (Direction)i;
    if (walls & adachi_wall_mask(direction)) {
      adachi_record_wall(direction);
    }
  }
}

// ----------------------------------------------------------------------
// ゴールを起点に幅優先探索し、各区画からゴールまでの歩数図を作る
// 壁が未記録の区間は通行可能として扱う
void adachi_make_steps_map(uint8_t goal_x, uint8_t goal_y) {
  uint8_t queue_x[MAZE_SIZE * MAZE_SIZE];
  uint8_t queue_y[MAZE_SIZE * MAZE_SIZE];
  uint8_t head = 0;
  uint8_t tail = 0;

  // 未到達を255として初期化する
  for (uint8_t x = 0; x < MAZE_SIZE; x++) {
    for (uint8_t y = 0; y < MAZE_SIZE; y++) {
      STEPS_MAP[x][y] = 255;
    }
  }

  // ゴールの歩数を0にして、探索待ちキューに入れる
  STEPS_MAP[goal_x][goal_y] = 0;
  queue_x[tail] = goal_x;
  queue_y[tail++] = goal_y;

  // ゴールに近い区画から順に隣接区画の歩数を確定する
  while (head < tail) {
    const uint8_t x = queue_x[head];
    const uint8_t y = queue_y[head++];
    for (uint8_t i = 0; i < 4; i++) {
      const Direction direction = (Direction)i;
      uint8_t next_x, next_y;
      if ((MAZE.walls[x][y] & adachi_wall_mask(direction)) == 0 &&
          adachi_get_neighbor(x, y, direction, &next_x, &next_y) &&
          STEPS_MAP[next_x][next_y] == 255) {
        STEPS_MAP[next_x][next_y] = STEPS_MAP[x][y] + 1;
        queue_x[tail] = next_x;
        queue_y[tail++] = next_y;
      }
    }
  }

  // 歩数図を出力する
  SERIAL_OUT.println("Steps map (adachi):");
  for (int8_t y = MAZE_SIZE - 1; y >= 0; y--) {
    for (uint8_t x = 0; x < MAZE_SIZE; x++) {
      SERIAL_OUT.printf("%2d ", STEPS_MAP[x][y]);
    }
    SERIAL_OUT.println();
  }
}

// ----------------------------------------------------------------------
// 足立法で次の移動先を決めて1区画進む
// 戻り値: 0=ゴール到達、-1=探索継続、1=既知の通路ではゴールに到達不能
int decide_direction_adachi(uint8_t goal_x, uint8_t goal_y) {
  if (MAZE.x == goal_x && MAZE.y == goal_y) {
    SERIAL_OUT.println("Goal Reached!");
    return 0;
  }

  // 現在位置の壁を反映してから、最新の歩数図を作る
  adachi_record_current_walls();
  adachi_make_steps_map(goal_x, goal_y);

  // 同じ歩数の候補がある場合に直進、左、右、後退の順で選ぶ
  const uint8_t relative_priority[] = {
    REL_FORWARD, REL_LEFT, REL_RIGHT, REL_BACK
  };
  uint8_t best_x = MAZE.x;
  uint8_t best_y = MAZE.y;
  uint8_t best_distance = 255;
  uint8_t best_relative = 255;
  bool best_is_unvisited = false;

  // 通行可能な隣接区画から、ゴールまでの歩数が最小の区画を選ぶ
  // 同じ歩数なら未通過の区画を優先して探索範囲を広げる
  for (uint8_t i = 0; i < 4; i++) {
    const uint8_t relative = relative_priority[i];
    const Direction direction = (Direction)(((uint8_t)MAZE.direction +
      (relative == REL_LEFT ? 1 : relative == REL_RIGHT ? 3 : relative == REL_BACK ? 2 : 0)) % 4);
    uint8_t next_x, next_y;
    if ((MAZE.walls[MAZE.x][MAZE.y] & adachi_wall_mask(direction)) != 0 ||
        !adachi_get_neighbor(MAZE.x, MAZE.y, direction, &next_x, &next_y)) {
      continue;
    }

    const uint8_t distance = STEPS_MAP[next_x][next_y];
    const bool is_unvisited = MAZE.passed[next_x][next_y] == 0;
    if (distance < best_distance ||
        (distance == best_distance && is_unvisited && !best_is_unvisited)) {
      best_x = next_x;
      best_y = next_y;
      best_distance = distance;
      best_relative = relative;
      best_is_unvisited = is_unvisited;
    }
  }

  if (best_relative == 255 || best_distance == 255) {
    SERIAL_OUT.println("Adachi: no known route to goal.");
    return 1;
  }

  // 選んだ相対方向に応じて車体を旋回・移動する
  update_direction(MAZE.direction, best_relative);
  switch (best_relative) {
    case REL_FORWARD:
      go_forward();
      break;
    case REL_LEFT:
      go_left();
      break;
    case REL_RIGHT:
      go_right();
      break;
    case REL_BACK:
      go_uturn();
      break;
  }

  // 移動後の位置と通過回数を更新し、新しい位置の壁も記録する
  MAZE.x = best_x;
  MAZE.y = best_y;
  if (MAZE.passed[MAZE.x][MAZE.y] < 255) {
    MAZE.passed[MAZE.x][MAZE.y]++;
  }
  adachi_record_current_walls();
  print_maze();

  return (MAZE.x == goal_x && MAZE.y == goal_y) ? 0 : -1;
}
