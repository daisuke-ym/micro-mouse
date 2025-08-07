// ----------------------------------------------------------------------
void flash_led(int led, int count) {
  int LED;

  switch (led) {
    case 1:
      LED = LED1;
      break;
    case 2:
      LED = LED2;
      break;
    default:
      LED = LED1;
  }
  for (int i = 0; i < count; i++) {
    digitalWrite(LED, HIGH);
    delay(50);
    digitalWrite(LED, LOW);
    delay(50);
  }
}

// ----------------------------------------------------------------------
void flash_alternate(int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(LED1, HIGH);
    delay(50);
    digitalWrite(LED1, LOW);
    delay(50);
    digitalWrite(LED2, HIGH);
    delay(50);
    digitalWrite(LED2, LOW);
    delay(50);
  }
}
