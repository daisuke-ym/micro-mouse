#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

const int LED = 23;
const int AIN1 = 16;
const int AIN2 = 17;
const int VR1 = 26;
const int ENC_1A = 27;
const int ENC_1B = 14;
volatile int ENC = 0; // ロータリーエンコーダの値

const int DELAY = 2000;

void setup() {
	SerialBT.begin("ESP32test");
	pinMode(LED, OUTPUT);

	// analogWriteFrequency() を設定するには，その前に analogWrite() が必要
	analogWrite(AIN1, 0);
	analogWrite(AIN2, 0);
	analogWriteFrequency(AIN1, 5e3);
	analogWriteFrequency(AIN2, 3e3);

	analogReadResolution(12);

	// ロータリーエンコーダの割り込み
	pinMode(ENC_1A, INPUT);
	pinMode(ENC_1B, INPUT);
	attachInterrupt(ENC_1A, isr_rotary_encoder, FALLING);
}

void loop() {
	motor_vr_test();

	SerialBT.println(ENC);
}

void blink() {
	SerialBT.println("HIGH");
	digitalWrite(LED, HIGH);
  delay(1000);
	SerialBT.println("LOW");
	digitalWrite(LED, LOW);
  delay(1000);
}

void motor_test() {
	for (int i = 0; i <= 100; i += 10) {
		SerialBT.println(i);
		analogWrite(AIN1, map(i, 0, 100, 0, 255));
		analogWrite(AIN2, 0);
		delay(DELAY);
	}
	analogWrite(AIN1, 0);
	delay(DELAY);

	for (int i = 0; i <= 100; i += 10) {
		SerialBT.println(-i);
		analogWrite(AIN1, 0);
		analogWrite(AIN2, map(i, 0, 100, 0, 255));
		delay(DELAY);
	}
	analogWrite(AIN2, 0);
	delay(DELAY);
}

void motor_vr_test() {
	int sensor = analogRead(VR1) - 2047;
	int duty = map(abs(sensor), 0, 2047, 0, 255);
	
	/*
	SerialBT.print(sensor);
	SerialBT.print("\t");
	SerialBT.println(duty);
	*/
	if (sensor > 0) {
		analogWrite(AIN1, duty);
		analogWrite(AIN2, 0);
	}
	else {
		analogWrite(AIN1, 0);
		analogWrite(AIN2, duty);
	}

	delay(100);
}

void isr_rotary_encoder() {
	if (digitalRead(ENC_1B) == HIGH) {
		ENC++;
	}
	else {
		ENC--;
	}
}
