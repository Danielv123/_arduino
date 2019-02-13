#include <Servo.h>


int value = 700; // set values you need to zero
int targetValue = 700;
Servo firstESC, secondESC; //Create as much as Servoobject you want. You can controll 2 or more Servos at the same time

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 500;  //the value is a number of milliseconds

void setup() {
  pinMode(3, OUTPUT);
  pinMode(4, INPUT);
  firstESC.attach(9);    // attached to pin 9 I just do this with 1 Servo
  Serial.begin(9600);    // start serial at 9600 baud
  Serial.setTimeout(100);
  firstESC.writeMicroseconds(700);
  delay(2000);
  digitalWrite(3, LOW);
  delay(2500);
  startMillis = millis();
}


void loop() {
  currentMillis = millis();
  //First connect your ESC WITHOUT Arming. Then Open Serial and follo Instructions
  
  firstESC.writeMicroseconds(value);
  
  if(Serial.available()){
    int tempVal;
    tempVal = Serial.parseInt();    // Parse an Integer from Serial
    if(tempVal >= 700){
      Serial.print("Changed value");
      Serial.println(tempVal);
      targetValue = tempVal;
    }
  }
  if(digitalRead(4)){
    delay(100);
    targetValue += 10;
  } else {
    targetValue -= 10;
  }
  if(targetValue < 700){
    targetValue = 700;
  } else if(targetValue > 2000){
    targetValue = 2000;
  }
  if(targetValue != value){ // make speed move towards setpoint
    if(targetValue - value > 0){
      value += min(targetValue - value, 10);
    } else {
      value += max(targetValue - value, -10);
    }
  }
}
