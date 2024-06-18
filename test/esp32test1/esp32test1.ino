#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

const int LED = 23;
const int AIN1 = 16;
const int AIN2 = 17;
const int VR1 = 26;

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
}

void loop() {
	motor_vr_test();
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
	int sensor = analogRead(VR1);
	int duty = map(sensor, 0, 4095, 0, 255);

	/*
	SerialBT.print(sensor);
	SerialBT.print("\t");
	SerialBT.println(duty);
	*/
	analogWrite(AIN1, duty);
	analogWrite(AIN2, 0);

	delay(100);
}
