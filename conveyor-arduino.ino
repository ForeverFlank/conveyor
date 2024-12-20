#include "Servo.h"

const uint8_t pusher_servo_pin = 10;
const uint8_t conveyor_motor_pin = 11;
const uint8_t sorter_servo_pin = 9;

Servo pusherServo, sorterServo;

uint8_t pusherServoRetractedAngle = 0;
uint8_t pusherServoExtendedAngle = 180;
int pusherActivationDuration = 1500;

int sorterServoDuration[4] = {
  0,
  500,
  1000,
  1500
};

struct Opcode {
  const uint8_t pusher_push    = 0b01000000;
  const uint8_t conveyor_off   = 0b01000010;
  const uint8_t conveyor_on    = 0b01000011;
  const uint8_t sorter_label_0 = 0b01100000;
  const uint8_t sorter_label_1 = 0b01100001;
  const uint8_t sorter_label_2 = 0b01100010;
  const uint8_t sorter_label_3 = 0b01100011;
};
constexpr Opcode opcode;

void setup() {
  Serial.begin(9600);
  pinMode(4, OUTPUT);

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);

  pusherServo.attach(pusher_servo_pin);
  sorterServo.attach(sorter_servo_pin);
  pinMode(conveyor_motor_pin, OUTPUT);

  for (int i = 2; i <= 6; ++i) {
    digitalWrite(i, 1);
  }
  delay(500);
  for (int i = 2; i <= 6; ++i) {
    digitalWrite(i, 0);
  }
}

void setLight(int num) {
  for (int i = 2; i <= 5; ++i) {
    digitalWrite(i, num == i);
  }
}

void activatePusher() {
  int t = 0, dt = 20;
  int halfDuration = pusherActivationDuration / 2;
  auto setAngle = [halfDuration](int x) {
    int angle = map(x, 0, halfDuration, pusherServoRetractedAngle, pusherServoExtendedAngle);
    pusherServo.write(angle);
    return;
  };
  for (; t < halfDuration; t += dt) {
    setAngle(t);
    delay(dt);
  }
  for (; t > 0; t -= dt) {
    setAngle(t);
    delay(dt);
  }
}

void moveSorter(int from, int to) {
  int duration = abs(sorterServoDuration[to] - sorterServoDuration[from]);
  int sign = (from > to) ? -1 : 1;
  sorterServo.write(90 + (90 * sign));
  delay(duration);
}

int sorterState = 0;

void loop() {
  if (Serial.available() > 0) {
    uint8_t code = Serial.read();

    digitalWrite(2, 1 & (code >> 7));
    digitalWrite(3, 1 & (code >> 6));
    digitalWrite(5, 1 & (code >> 1));
    digitalWrite(6, 1 & (code >> 0));

    switch (code) {
      case opcode.pusher_push:
        activatePusher();
        break;
      case opcode.conveyor_off:
        digitalWrite(conveyor_motor_pin, 0);
        break;
      case opcode.conveyor_on:
        digitalWrite(conveyor_motor_pin, 1);
        break;
      case opcode.sorter_label_0:
        moveSorter(sorterState, 0);
        sorterState = 0;
        break;
      case opcode.sorter_label_1:
        moveSorter(sorterState, 1);
        sorterState = 1;
        break;
      case opcode.sorter_label_2:
        moveSorter(sorterState, 2);
        sorterState = 2;
        break;
      case opcode.sorter_label_3:
        moveSorter(sorterState, 3);
        sorterState = 3;
        break;
      default:
        break;
    }
  }
}
