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
  //xTaskCreateUniversal(read_sensor, "task_read_sensor", 8192, NULL, 1, NULL, APP_CPU_NUM);
  xTaskCreateUniversal(read_sensor_sma, "task_read_sensor_sma", 8192, NULL, 1, NULL, APP_CPU_NUM);
  // センサの値を出力するタスク
  xTaskCreateUniversal(print_sensor, "task_print_sensor", 8192, NULL, 1, NULL, APP_CPU_NUM);
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
  static unsigned long start_time = millis();

  if (millis() - start_time > 2000) {
    SERIAL_OUT.println(millis());
    start_time = millis();
  }
  test_decide_direction_tremaux();
  test_run_shortest_path();
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
// センサの値を読み取るタスク(単純移動平均版)
void read_sensor_sma(void *pvParameters) {
  int ir_on[2], ir_off[2];
  int ss_l[10], ss_r[10], ss_fl[10], ss_fr[10];
  int index = 0;

  while (1) {
    if (index >= 10) {
      index = 0;
    }
    // 全部の IRLED を OFF にしてセンサの値を読む
    digitalWrite(IRLED_L_FR, LOW);
    digitalWrite(IRLED_R_FL, LOW);
    ir_off[0] = analogRead(SENSOR_R);
    ir_off[1] = analogRead(SENSOR_FL);
    // R と FL の IRLED を ON にしてセンサの値を読む
    digitalWrite(IRLED_R_FL, HIGH);
    ir_on[0] = analogRead(SENSOR_R);
    ir_on[1] = analogRead(SENSOR_FL);
    ss_r[index] = ir_on[0] - ir_off[0];
    ss_fl[index] = ir_on[1] - ir_off[1];
    // 全部の IRLED を OFF にしてセンサの値を読む
    digitalWrite(IRLED_L_FR, LOW);
    digitalWrite(IRLED_R_FL, LOW);
    ir_off[0] = analogRead(SENSOR_L);
    ir_off[1] = analogRead(SENSOR_FR);
    // L と FR の IRLED を ON にしてセンサの値を読む
    digitalWrite(IRLED_L_FR, HIGH);
    ir_on[0] = analogRead(SENSOR_L);
    ir_on[1] = analogRead(SENSOR_FR);
    ss_l[index] = ir_on[0] - ir_off[0];
    ss_fr[index] = ir_on[1] - ir_off[1];
    // 全部の IRLED を OFF にする
    digitalWrite(IRLED_L_FR, LOW);
    digitalWrite(IRLED_R_FL, LOW);
    // 単純移動平均を求める
    SS_L = 0;
    SS_R = 0;
    SS_FL = 0;
    SS_FR = 0;
    for (int i = 0; i < 10; i++) {
      SS_L += ss_l[i];
      SS_R += ss_r[i];
      SS_FL += ss_fl[i];
      SS_FR += ss_fr[i];
    }
    SS_L = SS_L / 10;
    SS_R = SS_R / 10;
    SS_FL = SS_FL / 10;
    SS_FR = SS_FR / 10;
    index++;
    // Task を回すうえで必ず必要な delay
    delay(1);
  }
}

// ----------------------------------------------------------------------
void print_sensor(void *pvParameters) {
  while (1) {
    lcd.setCursor(7, 0);
    lcd.printf("%4d %4d", SS_FL, SS_FR);
    lcd.setCursor(7, 1);
    lcd.printf("%4d %4d", SS_L, SS_R);
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
