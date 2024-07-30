/*
   micro mouse
*/
//#include <BluetoothSerial.h>

// センサと赤外線LED
const int SENSOR_FL = 36;
const int SENSOR_L = 39;
const int SENSOR_R = 34;
const int SENSOR_FR = 35;
const int IRLED_R_FL = 32;
const int IRLED_L_FR = 33;
// センサの値
volatile int SS_FL = 0;
volatile int SS_L = 0;
volatile int SS_R = 0;
volatile int SS_FR = 0;
// スイッチとLED
const int SW1 = 23;
const int SW2 = 25;
const int LED1 = 27;
const int LED2 = 26;
// モータドライバ
const int M1A = 19;
const int M1B = 18;
const int M2A = 5;
const int M2B = 17;
// ロータリーエンコーダ
const int ENC_1A = 16;
const int ENC_1B = 4;
const int ENC_2A = 14;
const int ENC_2B = 12;
// ロータリーエンコーダの値
volatile int ENC1 = 0;
volatile int ENC2 = 0;

// ----------------------------------------------------------------------
void setup() {
  Serial.begin(115200);

  // センサの初期設定
  analogReadResolution(12);
  pinMode(IRLED_L_FR, OUTPUT);
  pinMode(IRLED_R_FL, OUTPUT);
  // スイッチとLEDの初期設定
	pinMode(SW1, INPUT_PULLUP);
	pinMode(SW2, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  // モータドライバの初期設定
  analogWrite(M1A, 0);
  analogWrite(M1B, 0);
  analogWrite(M2A, 0);
  analogWrite(M2B, 0);
  // PWM 周波数の設定
  analogWriteFrequency(M1A, 5e3);
  analogWriteFrequency(M1B, 5e3);
  analogWriteFrequency(M2A, 5e3);
  analogWriteFrequency(M2B, 5e3);
  // ロータリーエンコーダの初期設定
  pinMode(ENC_1A, INPUT);
  pinMode(ENC_1B, INPUT);
  pinMode(ENC_2A, INPUT);
  pinMode(ENC_2B, INPUT);

  // センサの値を読むタスクを追加
  xTaskCreateUniversal(read_sensor, "task_read_sensor", 8192, NULL, 1, NULL, APP_CPU_NUM);
}

// ----------------------------------------------------------------------
void loop() {
  delay(1);
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