const int LeftMotorForward = 2;
const int LeftMotorBackward = 3;
const int LiftUp = 4;
const int LiftDown = 5;

const int dirPin = 8;
const int stepPin = 9;
const int stepsPerRevolution = 10;

char command;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(LiftDown, OUTPUT);
  pinMode(LiftUp,OUTPUT);

  pinMode(LeftMotorBackward, OUTPUT);
  pinMode(LeftMotorForward,OUTPUT);
  pinMode(stepPin, OUTPUT);
	pinMode(dirPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  bool sorttime;
  if(Serial.available() > 0){ 
      command = Serial.read(); 
      stop();
      Serial.println(command);
      switch(command){
      case 'F':  
        move_forward();
        break;
      case 'B':  
        move_backward();
        break;
      case 'L':
        stop(); 
        lift_bin();
        break;
      case 'R':
        lower_bin();
        break;
      }
      Serial.println("hene");
  }
  shake();
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
  digitalWrite(LiftUp, LOW);
  digitalWrite(LiftDown, LOW);
  delay(500);
}

void lift_bin(){
  // lift the trash for sorting using 
  digitalWrite(LiftUp, HIGH);
  digitalWrite(LiftDown, LOW);
  delay(500);
  digitalWrite(LiftUp, LOW);
  digitalWrite(LiftDown, LOW);
  delay(500);
  Serial.println("lifti");
}

void lower_bin(){
  // lower bin to collect incomming trash
  digitalWrite(LiftUp, LOW);
  digitalWrite(LiftDown, HIGH);\
  delay(500);
  digitalWrite(LiftUp, LOW);\
  digitalWrite(LiftDown, LOW);
  Serial.println("return");
}

void shake(){
  // shake to let the sand fall
  Serial.println("gelll");
  
	// Set motor direction clockwise
	digitalWrite(dirPin, HIGH);

	// Spin motor slowly
	for(int x = 0; x < stepsPerRevolution; x++)
	{
		digitalWrite(stepPin, HIGH);
		delayMicroseconds(500);
		digitalWrite(stepPin, LOW);
		delayMicroseconds(500);
	}
	delay(1000); // Wait a second
	
	// Set motor direction counterclockwise
	digitalWrite(dirPin, LOW);

	// Spin motor quickly
	for(int x = 0; x < stepsPerRevolution; x++)
	{
		digitalWrite(stepPin, HIGH);
		delayMicroseconds(500);
		digitalWrite(stepPin, LOW);
		delayMicroseconds(500);
	}
	delay(1000); // Wait a second
}