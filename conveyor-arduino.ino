const uint8_t pusher_servo_pin = 9;
const uint8_t sorter_servo_pin = 10;
const uint8_t conveyor_servo_pin = 11;

void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  pinMode(pusher_servo_pin, OUTPUT);
  pinMode(sorter_servo_pin, OUTPUT);
  pinMode(conveyor_servo_pin, OUTPUT);
}

void setLight(int num) {
  for (int i = 2; i <= 5; ++i) {
    digitalWrite(i, num == i);
  }
}

void loop() {
  if (Serial.available() > 0) {
    int num = Serial.read() - '0' + 2;
    setLight(num);
    delay(500);
    setLight(-1);
  }
}
