// ----------------------------------------------------------------------
void draw_menu(uint8_t selected) {
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print(">");
  LCD.print(MENU_LABELS[selected]);
  LCD.setCursor(0, 1);
  LCD.print(" ");
  LCD.print(MENU_LABELS[(selected + 1) % MENU_ITEM_COUNT]);
}

// ----------------------------------------------------------------------
MenuItem menu_control() {
  uint8_t selected = 0;
  MENU_ACTIVE = true;
  draw_menu(selected);

  while (true) {
    if (digitalRead(SW2) == LOW) {
      delay(30);
      if (digitalRead(SW2) == LOW) {
        selected = (selected + 1) % MENU_ITEM_COUNT;
        draw_menu(selected);
        while (digitalRead(SW2) == LOW) {
          delay(10);
        }
        delay(30);
      }
    }

    if (digitalRead(SW1) == LOW) {
      delay(30);
      if (digitalRead(SW1) == LOW) {
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("Selected:");
        LCD.setCursor(0, 1);
        LCD.print(MENU_LABELS[selected]);
        while (digitalRead(SW1) == LOW) {
          delay(10);
        }
        if (MENU_LABELS[selected] == MENU_LABELS[MENU_SENSOR_DISPLAY]) {
          delay(500);
        }
        else {
          delay(1500);
        }
        LCD.clear();
        MENU_ACTIVE = false;
        return static_cast<MenuItem>(selected);
      }
    }

    delay(10);
  }
}

// ----------------------------------------------------------------------
void display_sensor_values() {
  MENU_ACTIVE = true;
  LCD.clear();

  while (true) {
    LCD.setCursor(0, 0);
    LCD.printf("FL:%4d  FR:%4d", SS_FL, SS_FR);
    LCD.setCursor(0, 1);
    LCD.printf(" L:%4d   R:%4d", SS_L, SS_R);

    if (digitalRead(SW2) == LOW) {
      delay(30);
      if (digitalRead(SW2) == LOW) {
        while (digitalRead(SW2) == LOW) {
          delay(10);
        }
        delay(30);
        MENU_ACTIVE = false;
        return;
      }
    }

    delay(250);
  }
}
