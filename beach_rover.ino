const int LeftMotorForward = 2;
const int LeftMotorBackward = 3;
const int RightMotorForward = 4;
const int RightMotorBackward = 5;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(RightMotorBackward, OUTPUT);
  pinMode(RightMotorForward,OUTPUT);

  pinMode(LeftMotorBackward, OUTPUT);
  pinMode(LeftMotorForward,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() > 0){ 
      command = Serial.read(); 
      Stop();
      switch(command){
      case 'F':  
        forward();
        break;
      case 'B':  
        back();
        break;
      case 'L':  
        lift_bin();
        break;
      case 'R':
        lower_bin();
        break;
      }
}

void move_forward(){
  digitalWrite(RightMotorForward, HIGH);
  digitalWrite(RightMotorBackward, LOW);

  //delay(2000);

  digitalWrite(LeftMotorForward, HIGH);
  digitalWrite(LeftMotorBackward, LOW);;
  delay(5000);
}

void move_backward(){
  digitalWrite(RightMotorForward, LOW);
  digitalWrite(RightMotorBackward, HIGH);

  //delay(2000);

  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(LeftMotorBackward, HIGH);;
  delay(5000);
}

void lift_bin(){
  // lift the trash for sorting using 
}

void lower_bin(){
  // lower bin to collect incomming trash
}

void shake(){
  // shake to let the sand fall
}