const int LeftMotorForward = 2;
const int LeftMotorBackward = 3;
const int LiftUp = 4;
const int LiftDown = 5;

char command;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(LiftDown, OUTPUT);
  pinMode(LiftUp,OUTPUT);

  pinMode(LeftMotorBackward, OUTPUT);
  pinMode(LeftMotorForward,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() > 0){ 
      command = Serial.read(); 
      stop();
      switch(command){
      case 'F':  
        move_forward();
        break;
      case 'B':  
        move_backward();
        break;
      case 'L':  
        lift_bin();
        break;
      case 'R':
        lower_bin();
        break;
      }
  }
}

void move_forward(){
  //delay(2000);

  digitalWrite(LeftMotorForward, HIGH);
  digitalWrite(LeftMotorBackward, LOW);
  delay(500);
}

void move_backward(){
  

  //delay(2000);

  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(LeftMotorBackward, HIGH);;
  delay(500);
}

void stop(){
  //delay(2000);

  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(LeftMotorBackward, LOW);;
  delay(500);
}

void lift_bin(){
  // lift the trash for sorting using 
  digitalWrite(LiftUp, HIGH);
  digitalWrite(LiftDown, LOW);
  delay();
}

void lower_bin(){
  // lower bin to collect incomming trash
  digitalWrite(LiftUp, LOW);
  digitalWrite(LiftDown, HIGH);\
  delay();
}

void shake(){
  // shake to let the sand fall
}