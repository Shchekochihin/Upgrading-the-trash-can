#include <Servo.h>
Servo myservo;


volatile uint32_t mainTimer = 0;//
volatile int openCover = -1;
int step = 1;
int homePos = 87;
int openPos = 25;
int target = homePos;
int counter = 0;
int currentPosition = 0;
volatile uint32_t debounce = 0;
volatile bool start = 0;
bool err = 0;

void setup() {
  Serial.begin(9600);
  attachInterrupt(1, btnIsr, CHANGE);
  counter = readCurrentPosition();
}




int readCurrentPosition(){
  int value = 0;
  int count = 3;
  for (int i=0; i < count; i++){
    value = value + map(analogRead(A0),65,645,0,180);
  }
  return value / count;
}

void goToPoint(int val){
  if(!myservo.attached() && !err){
    myservo.attach(2);
  }
  myservo.write(val);  
}


void btnIsr() {
  if(digitalRead(3) && !start && openCover != 2){
    start = 1;
    openCover = 1;
    debounce = millis();
  }else if(!digitalRead(3) && start && millis() - debounce > 8000 && openCover != 2){
    debounce = millis();
    openCover = 2;
  }
}

void loop() {
  if(openCover == 1){
    counter = readCurrentPosition();
    target = openPos;
    openCover = -1;
  }else if(openCover == 2 &&  millis() - debounce > 700){
    counter = readCurrentPosition();
    target = homePos;
    openCover = -1;
    start = 0;
  }
  if (millis() - mainTimer > 50) {
    mainTimer = millis();
    
    if((abs(homePos - counter) < 5) || (abs(openPos - counter) < 5)){
      step = 1;
    }else if((abs(homePos - counter) < 10) || (abs(openPos - counter) < 10)){
      step = 2;
    }else{
      step = 4;
    }
    if(target < counter){
      counter = counter - step;
      goToPoint(counter);
    }else if(target > counter){
      counter = counter + step;
      goToPoint(counter);
    }else{
      if(myservo.attached()){
        myservo.detach();
      }
    }
  }

}