#include <BluetoothSerial.h>
#include <Unistep2_mod.h>
#include <ST7032_asukiaaa.h>
#include <math.h>
#include <Wire.h>
#include <Adafruit_BNO055.h>
#include "defines.h"

// ----------------------------------------------------------------------
void setup() {
  // serial
  Serial.begin(115200);
  SerialBT.begin("ESP32_MicroMouse"); // Bluetooth シリアルの初期化
  // センサの初期設定
  analogReadResolution(12);
  pinMode(SENSOR_FL, INPUT);
  pinMode(IRLED_L_FR, OUTPUT);
  pinMode(IRLED_R_FL, OUTPUT);
  // LCDの初期化
  lcd.begin(16, 2);
  lcd.setContrast(40);
  lcd.clear();
  // スイッチとLEDの初期設定
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  // BNO055 の初期化
  if (!bno.begin()) {
    Serial.print("BNO055 not detected. Check wiring.");
    while (1);
  }
  bno.setExtCrystalUse(false);
  // BNO055 の初期化完了まで待つ
  delay(1000);

  // センサの値を読むタスク
  xTaskCreateUniversal(read_sensor, "task_read_sensor", 8192, NULL, 1, NULL, APP_CPU_NUM);
  // センサの値を出力するタスク
  //xTaskCreateUniversal(print_sensor, "task_print_sensor", 8192, NULL, 1, NULL, APP_CPU_NUM);
  // ステッパの値を出力するタスク
  //xTaskCreatePinnedToCore(print_steps, "task_print_steps", 8192, NULL, 1, NULL, APP_CPU_NUM);
  // BNO055 の値を読むタスク
  xTaskCreatePinnedToCore(read_bno, "task_read_bno", 8192, NULL, 1, NULL, APP_CPU_NUM);
  // Bluetooth に値を出力するタスク
  //xTaskCreatePinnedToCore(print_serialbt, "task_print_serialbt", 8192, NULL, 1, NULL, APP_CPU_NUM);

  // 迷路の初期化
  init_maze();
}

// ----------------------------------------------------------------------
void loop() {
  test_decide_direction();
}

// ----------------------------------------------------------------------
void read_sensor(void *pvParameters) {
  int ir_on[2], ir_off[2];

  while (1) {
    // 全部の IRLED を OFF にしてセンサの値を読む
    digitalWrite(IRLED_L_FR, LOW);
    digitalWrite(IRLED_R_FL, LOW);
    ir_off[0] = analogRead(SENSOR_R);
    ir_off[1] = analogRead(SENSOR_FL);
    // R と FL の IRLED を ON にしてセンサの値を読む
    digitalWrite(IRLED_R_FL, HIGH);
    ir_on[0] = analogRead(SENSOR_R);
    ir_on[1] = analogRead(SENSOR_FL);
    SS_R = ir_on[0] - ir_off[0];
    SS_FL = ir_on[1] - ir_off[1];
    // 全部の IRLED を OFF にしてセンサの値を読む
    digitalWrite(IRLED_L_FR, LOW);
    digitalWrite(IRLED_R_FL, LOW);
    ir_off[0] = analogRead(SENSOR_L);
    ir_off[1] = analogRead(SENSOR_FR);
    // L と FR の IRLED を ON にしてセンサの値を読む
    digitalWrite(IRLED_L_FR, HIGH);
    ir_on[0] = analogRead(SENSOR_L);
    ir_on[1] = analogRead(SENSOR_FR);
    SS_L = ir_on[0] - ir_off[0];
    SS_FR = ir_on[1] - ir_off[1];
    // 全部の IRLED を OFF にする
    digitalWrite(IRLED_L_FR, LOW);
    digitalWrite(IRLED_R_FL, LOW);
    // Task を回すうえで必ず必要な delay
    delay(1);
  }
}

// ----------------------------------------------------------------------
void print_sensor(void *pvParameters) {
  while (1) {
    lcd.setCursor(0, 0);
    lcd.print(SS_FL); lcd.print("   ");
    lcd.setCursor(12, 0);
    lcd.print(SS_FR); lcd.print("   ");
    lcd.setCursor(0, 1);
    lcd.print(SS_L); lcd.print("   ");
    lcd.setCursor(12, 1);
    lcd.print(SS_R); lcd.print("   ");
    //
    lcd.setCursor(6, 0);
    lcd.print(abs(SS_FL - SS_FR)); lcd.print("  ");
    lcd.setCursor(6, 1);
    if (SS_FL >= SS_FR) {
      lcd.print(">");
    }
    else {
      lcd.print("<");
    }
    /*
    Serial.print(SS_L);
    Serial.print("\t");
    Serial.print(SS_FL);
    Serial.print("\t");
    Serial.print(SS_FR);
    Serial.print("\t");
    Serial.print(SS_R);
    Serial.println();
    */
    delay(250);
  }
}

// ----------------------------------------------------------------------
void print_steps(void *args) {
  static unsigned long lcd_period = 0;

  while (1) {
    lcd.setCursor(0, 0);
    lcd.print(stepperL.stepsToGo());
    lcd.print("   ");
    lcd.setCursor(8, 0);
    lcd.print(stepperR.stepsToGo());
    lcd.print("   ");
    lcd_period = millis();
    delay(100);
  }
}

// ----------------------------------------------------------------------
void read_bno(void *pvParameters) {

  while (1) {
    imu::Vector<3> IMU_EULER = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    DIR_X = IMU_EULER.x();
    DIR_Y = IMU_EULER.y();
    DIR_Z = IMU_EULER.z();
    /*
    // デバッグ用にシリアル出力
    Serial.print("X: ");
    Serial.print(DIR_X);
    Serial.print(" Y: ");
    Serial.print(DIR_Y);
    Serial.print(" Z: ");
    Serial.println(DIR_Z);
    */
    // Task を回すうえで必ず必要な delay
    delay(10);
  }
}

// ----------------------------------------------------------------------
void print_serialbt(void *pvParameters) {
  while (1) {
    SerialBT.println(millis());
    // Task を回すうえで必ず必要な delay
    delay(1000);
  }
}
