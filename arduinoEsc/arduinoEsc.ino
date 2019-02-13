#include <Servo.h>


int value = 0; // set values you need to zero

Servo firstESC, secondESC; //Create as much as Servoobject you want. You can controll 2 or more Servos at the same time

void setup() {
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  delay(1000);
  firstESC.attach(9);    // attached to pin 9 I just do this with 1 Servo
  Serial.begin(9600);    // start serial at 9600 baud
  firstESC.writeMicroseconds(2000);
  delay(2000);
  digitalWrite(3, LOW);
  delay(2500);
  firstESC.writeMicroseconds(700);
  delay(3000);
}

void loop() {
  delay(15000);
  firstESC.writeMicroseconds(800);
  delay(5000);
  firstESC.writeMicroseconds(700);
}
