#include <Unistep2_mod.h>
#include <ST7032_asukiaaa.h>
#include <math.h>

// センサと赤外線LED
const int SENSOR_FL = 35;
const int SENSOR_L  = 34;
const int SENSOR_R  = 39;
const int SENSOR_FR = 36;
const int IRLED_R_FL = 27;
const int IRLED_L_FR = 14;
// センサの値
volatile int SS_FL = 0;
volatile int SS_L = 0;
volatile int SS_R = 0;
volatile int SS_FR = 0;
// スイッチとLED
const int SW1 = 19;
const int SW2 = 13;
const int LED1 = 23;
const int LED2 = 12;
// ステッピングモータ左
const int STP1P1 = 5;
const int STP1P2 = 18;
const int STP1P3 = 17;
const int STP1P4 = 16;
// ステッピングモータ右
const int STP2P1 = 33;
const int STP2P2 = 32;
const int STP2P3 = 25;
const int STP2P4 = 26;

// 車体の数値
const double WHEEL_D = 50.0; // ホイールの直径(mm)
const double TREAD = 103.0;  // トレッド
const double STEPS = 4096.0; // 1回転するのに必要なステップ数
const double MMPS = M_PI * WHEEL_D / STEPS; // 1ステップで進む距離(milli meter per step)
const double TURN90 = M_PI * TREAD * 0.25 / MMPS; // 90度ターンに必要なステップ数

// ステッピングモータ
const unsigned long DEFAULT_STEP_DELAY = 1000;
Unistep2_mod stepperL(STP1P1, STP1P4, STP1P3, STP1P2, 4096, DEFAULT_STEP_DELAY); // stepperR と回転方向を合わせるため P2 と P4 を入れ替えている
Unistep2_mod stepperR(STP2P1, STP2P2, STP2P3, STP2P4, 4096, DEFAULT_STEP_DELAY);

// AQM1602Y LCD
ST7032_asukiaaa lcd;

TaskHandle_t thp[1];

// ----------------------------------------------------------------------
void setup() {
  // serial
  Serial.begin(115200);
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

  // センサの値を読むタスク
  xTaskCreateUniversal(read_sensor, "task_read_sensor", 8192, NULL, 1, NULL, APP_CPU_NUM);
  // センサの値を出力するタスク
  //xTaskCreateUniversal(print_sensor, "task_print_sensor", 8192, NULL, 1, NULL, APP_CPU_NUM);
  // ステッパの値を出力するタスク
  xTaskCreatePinnedToCore(print_steps, "task_print_steps", 8192, NULL, 1, NULL, APP_CPU_NUM);
}

// ----------------------------------------------------------------------
void loop() {
  test_run_to_mm();
  test_pivotturn();
}

// ----------------------------------------------------------------------
void test_run_to_mm() {
  if (digitalRead(SW2) == LOW) {
    delay(1000);
    run_to_mm(180.0); // 180mm 前進
    flash_led(2, 5);
  }
}

// ----------------------------------------------------------------------
void test_spinturn() {
  if (digitalRead(SW1) == LOW) {
    delay(1000);
    spinturn(90.0); // 90度右回り
    flash_led(1, 5);
  }
}

// ----------------------------------------------------------------------
void test_pivotturn() {
  if (digitalRead(SW1) == LOW) {
    delay(1000);
    pivoturn(-90.0); // 90度左回り
    flash_led(1, 5);
  }
}

// ----------------------------------------------------------------------
// mm単位で指定した距離だけ走る
// マイナス値は後退
void run_to_mm(double distance) {
  stepperL.move(distance / MMPS);
  stepperR.move(distance / MMPS);
  while (stepperL.stepsToGo() != 0 && stepperR.stepsToGo() != 0) {
    stepperL.run();
    stepperR.run();
  }
  stepperL.stop();
  stepperR.stop();
}

// ----------------------------------------------------------------------
//　指定した角度だけ回転する（トレッドの中心で旋回）
// angle: 角度(度単位)
// 右回りは正、左回りは負
void spinturn(double angle) {
  double a = angle / 360.0 * TREAD * M_PI / MMPS;
  stepperL.move(a);
  stepperR.move(-1 * a);
  while (stepperL.stepsToGo() != 0 && stepperR.stepsToGo() != 0) {
    stepperL.run();
    stepperR.run();
  }
  stepperL.stop();
  stepperR.stop();
}

// ----------------------------------------------------------------------
// 指定した角度だけ回転する（車輪の中心で旋回）
// angle: 角度(度単位)
// 右回りは正、左回りは負
void pivoturn(double angle) {
  double a = angle / 360.0 * TREAD * 2.0 * M_PI / MMPS;
  if (angle > 0) {
    stepperL.move(a);
    while (stepperL.stepsToGo() != 0) {
      stepperL.run();
    }
    stepperL.stop();
  } else {
    stepperR.move(-1 * a);
    while (stepperR.stepsToGo() != 0) {
      stepperR.run();
    }
    stepperR.stop();
  }
  stepperL.stop();
  stepperR.stop();
}

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
//    if (millis() - lcd_period >= 100) {
      lcd.setCursor(0, 0);
      lcd.print(stepperL.stepsToGo());
      lcd.print("   ");
      lcd.setCursor(8, 0);
      lcd.print(stepperR.stepsToGo());
      lcd.print("   ");
      lcd_period = millis();
//    }
    delay(100);
  }
}
