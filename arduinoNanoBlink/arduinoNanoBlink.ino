int runningLed = 2;
int cnt = 0;
bool dir = true;
void setup() {
  // put your setup code here, to run once:
  pinMode(runningLed, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(dir){
    cnt++;
  } else {
    cnt--;
  } 
  digitalWrite(runningLed, HIGH);
  delayMicroseconds(abs(cnt-1000));
  digitalWrite(runningLed, LOW);
  delayMicroseconds(abs(cnt+100));
  if(cnt > 1000){
    dir = false;
  } else if(cnt < 1){
    dir = true;
  }
}
