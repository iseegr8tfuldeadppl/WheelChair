#include <Servo.h>

Servo servo;

void setup() {
  Serial.begin(9600);
  servo.attach(4, 100, 2900);
}

void loop() {
  servo.write(0);
  delay(3000);
  servo.write(180);
  delay(3000);
  Serial.println(millis());
  
}
