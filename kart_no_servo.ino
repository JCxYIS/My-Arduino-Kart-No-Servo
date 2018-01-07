//#include <Servo.h>
#include <Ultrasonic.h>
const int W1_1 = 10, W1_2 = 11; //wheel output
const int W2_2 = 12, W2_1 = 13;
const int green = 8, yellow = 9, red = 7; //debug light
//const int servo = 9, minhz = 1000, maxhz = 2000; //servo
const int ir1 = 5, ir2 = 6; //IR sensor
const int eyetrig = 3, eyeecho = 4; //eye ca

int correct = 0;
bool canIR1, canIR2; //true: can see; false: can't see
float distance, RealDistance = 87, lastDistance;

//Servo myservo;
Ultrasonic ultrasonic(eyetrig, eyeecho);
void setup() {
  Serial.begin(9600);
  pinMode(W1_1, OUTPUT);
  pinMode(W1_2, OUTPUT);
  pinMode(W2_1, OUTPUT);
  pinMode(W2_2, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
  //myservo.attach(servo, minhz, maxhz);
  pinMode(ir1, INPUT);
  pinMode(ir2, INPUT);
  pinMode(eyetrig, OUTPUT);
  pinMode(eyeecho, INPUT);
}

void motor(){
  if(RealDistance > 5 || RealDistance == -1){
    if(!canIR1 && !canIR2) //black black
    {
      digitalWrite(W1_1, LOW);
      digitalWrite(W1_2, LOW);
      digitalWrite(W2_1, LOW);
      digitalWrite(W2_2, LOW);
      digitalWrite(green, LOW);
      digitalWrite(red, LOW);
    }
    else if(canIR1 && !canIR2) //white black: turn right
    {
      digitalWrite(W1_1, LOW);
      digitalWrite(W1_2, HIGH);
      digitalWrite(W2_1, HIGH);
      digitalWrite(W2_2, LOW);
      digitalWrite(green, HIGH);
      digitalWrite(red, LOW);
    }
    else if(!canIR1 && canIR2)   //black white: turn left
    {
      digitalWrite(W1_1, HIGH);
      digitalWrite(W1_2, LOW);
      digitalWrite(W2_1, LOW);
      digitalWrite(W2_2, HIGH);
      digitalWrite(green, LOW);
      digitalWrite(red, HIGH);
    }
    else if(canIR1 && canIR2)   // white white: keep moving!
    {
      digitalWrite(W1_1, LOW);
      digitalWrite(W1_2, HIGH);
      digitalWrite(W2_1, LOW);
      digitalWrite(W2_2, HIGH);
      digitalWrite(green, HIGH);
      digitalWrite(red, HIGH);
    }
  }
  else
  { //TURN back for 0.3s turn LEFT FOR 0.2S
      Serial.print("Too near! Now Reversing");
      digitalWrite(W1_1, LOW);
      digitalWrite(W1_2, HIGH);
      digitalWrite(W2_1, LOW);
      digitalWrite(W2_2, HIGH);
      //digitalWrite(green, HIGH);
      //digitalWrite(red, HIGH);
      delay(300);
      digitalWrite(W1_1, HIGH);
      digitalWrite(W1_2, LOW);
      digitalWrite(W2_1, LOW);
      digitalWrite(W2_2, HIGH);
      //digitalWrite(green, LOW);
      //digitalWrite(red, HIGH);
      delay(200);
  }
}

void get_ir(){
  int ir1_state = digitalRead(ir1), ir2_state = digitalRead(ir2);
  if(ir1_state == 0){ canIR1 = false;}
  else{canIR1 = true;}
  if(ir2_state == 0){ canIR2 = false;}
  else{canIR2 = true;}
}

float get_eye(){
  long microsec = ultrasonic.timing();
  distance = ultrasonic.convert(microsec, Ultrasonic::CM); // 計算距離，單位: 公分
  Serial.print("Detect: ");
  Serial.print(distance);
  Serial.print("cm | ");
  if(distance == 0)
  {
    Serial.print("=No Response=");
    correct = 0;
    distance = -1;
    return -1;
  }

  //avoid randomness
  if(abs(distance - lastDistance) <= 5){correct += 1 ;}
  else{correct = 0;}
  lastDistance = distance;

  if(correct >= 5){
    return distance;
  }
  else
  {
    return RealDistance;//原本的
  }
}

void Debug_Light() {
    if(canIR1) digitalWrite(green, HIGH);
    else digitalWrite(green, LOW);

    if(canIR2) digitalWrite(red, HIGH);
    else digitalWrite(red, LOW);

    if(RealDistance > 255) analogWrite(yellow, 255);//max 255
    else if(RealDistance > 0) analogWrite(yellow, RealDistance);
    else analogWrite(yellow, 0);
}

void loop() {
  RealDistance = get_eye();
  Serial.print(RealDistance);
  Serial.print("cm  | ");
  Serial.print(correct);
  Serial.print("combo!! | ");
  get_ir();
  Serial.print(" IR1:");
  Serial.print(canIR1);
  Serial.print(" | IR2:");
  Serial.print(canIR2);
  Serial.print("\n");
  Debug_Light();
  motor();
}
