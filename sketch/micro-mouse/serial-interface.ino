void serial_interface() {
  if (SERIAL_OUT.available() == 0) {
    return;
  }
  char c = toupper(SERIAL_OUT.read());
  switch (c) {
    case 'F': // 直進
      go_forward();
      break;
    case 'R': // 右行き
      go_right();
      break;
    case 'L': // 左行き
      go_left();
      break;
    case 'T': // Uターン
      go_uturn();
      break;
    default:
      break;
  }
}
