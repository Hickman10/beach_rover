const int LeftMotorForward = 2;
const int LeftMotorBackward = 3;
const int LiftUp = 4;
const int LiftDown = 5;

const int dirPin = 7;
const int stepPin = 6;
const int stepsPerRevolution = 10;

const byte npulse = 12; // number of pulses to charge the capacitor before each measurement
 
const byte pin_pulse = A0; // sends pulses to charge the capacitor (can be a digital pin)
const byte pin_cap  = A1; // measures the capacitor charge
const byte pin_LED = 13; // LED that turns on when metal is detected

const int nmeas = 6; //measurements to take
long int sumsum = 0; //running sum of 64 sums
long int skip = 0; //number of skipped sums
long int diff = 0;      //difference between sum and avgsum
long int flash_period = 0; //period (in ms)
long unsigned int prev_flash = 0; //time stamp of previous flash

const int trigPin = 9;  // Trig pin of the ultrasonic sensor
const int echoPin = 10; // Echo pin of the ultrasonic sensor

#include <Servo.h>
int pos = 0; 
Servo myservo; 

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

  pinMode(pin_pulse, OUTPUT);
  digitalWrite(pin_pulse, LOW);
  pinMode(pin_cap, INPUT);
  pinMode(pin_LED, OUTPUT);
  digitalWrite(pin_LED, LOW);
  myservo.attach(12);
  myservo.write(90);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}


void loop() {
  // put your main code here, to run repeatedly:
  bool sort_time = false;
  bool bin_detected = false;
 // bool 
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
        sort_time = true;
        break;
      case 'R':
        lower_bin();
        sort_time = false;
        break;
      }
      Serial.println("hene");
  }
  shake();
  if(sort_time){ 
    delay(500);
    float distance = calculateDistance();
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    Serial.println("sorting");
    delay(300); // Add a delay for readability
    if (distance < 6){
      bin_detected = true;
    }
    
    if (bin_detected == true){
      Serial.println("testing metal");
      int bin_type = metal_test();
      if (bin_type < 2){
        // move servo right
        for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15 ms for the servo to reach the position
        }
        
        delay(1000);
        Serial.println("ddddd");
        for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
          // in steps of 1 degree
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15 ms for the servo to reach the position
        }
        delay(100);
      }
      else 
        // move servo left
        for (pos = 90; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
          // in steps of 1 degree
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15 ms for the servo to reach the position
        }
        delay(1000);
        Serial.println("hjjej");
        for (pos = 180; pos >= 90; pos -= 1) { // goes from 180 degrees to 0 degrees
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15 ms for the servo to reach the position
        }
        delay(100);

      bin_detected = false;
    }
  }
}

void move_forward(){
  //delay(2000);

  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(LeftMotorBackward, HIGH);
  delay(500);
}

void move_backward(){
  

  //delay(2000);

  digitalWrite(LeftMotorForward, HIGH);
  digitalWrite(LeftMotorBackward, LOW);;
  delay(500);
}

void stop(){
  //delay(2000);

  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(LeftMotorBackward, LOW);;
  //delay(500);
  digitalWrite(LiftUp, LOW);
  digitalWrite(LiftDown, LOW);
  delay(100);
}

void lift_bin(){
  // lift the trash for sorting using 
  for( int x = 0; x < 3 ; x++){
    digitalWrite(LiftUp, HIGH);
    digitalWrite(LiftDown, LOW);
    delay(500);
    digitalWrite(LiftUp, LOW);
    digitalWrite(LiftDown, LOW);
    delay(100);
    Serial.println("lifti");
  }
  
}

void lower_bin(){
  // lower bin to collect incomming trash
  for( int x = 0 ; x < 3 ; x++){
    digitalWrite(LiftUp, LOW);
    digitalWrite(LiftDown, HIGH);\
    delay(500);
    digitalWrite(LiftUp, LOW);\
    digitalWrite(LiftDown, LOW);
    delay(100);
    Serial.println("return");
    }
  
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
	delay(500); // Wait a second
	
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
	delay(100); // Wait a second
}

float calculateDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(3);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duration = pulseIn(echoPin, HIGH);
  // Speed of sound is approximately 343 meters/second (0.0343 cm/microsecond)
  // Divide by 2 to account for the round trip
  float distance = duration * 0.0343 / 2;

  return distance;
}


int metal_test(){

int minval = 2000;
int maxval = 0;
byte ledstat = 0;
//perform measurement
long unsigned int sum = 0;
for (int imeas = 0; imeas < nmeas + 2; imeas++) {
  //reset the capacitor
  pinMode(pin_cap, OUTPUT);
  digitalWrite(pin_cap, LOW);
  delayMicroseconds(20);
  pinMode(pin_cap, INPUT);
  //apply pulses
  for (int ipulse = 0; ipulse < npulse; ipulse++) {
    digitalWrite(pin_pulse, HIGH); //takes 3.5 microseconds
    delayMicroseconds(3);
    digitalWrite(pin_pulse, LOW); //takes 3.5 microseconds
    delayMicroseconds(3);
  }
  //read the charge on the capacitor
  int val = analogRead(pin_cap); //takes 13x8=104 microseconds
  minval = min(val, minval);
  maxval = max(val, maxval);
  sum += val;

  //determine if LEDs should be on or off
  long unsigned int timestamp = millis();
  // byte ledstat = 0;
  if (timestamp < prev_flash +12) {
    if (diff > 0)ledstat = 1;
    if (diff < 0)ledstat = 2;
  }
  if (timestamp > prev_flash + flash_period) {
    if (diff > 0)ledstat = 1;
    if (diff < 0)ledstat = 2;
    prev_flash = timestamp;
  }
  if (flash_period > 1000)ledstat = 0;

  //switch the LEDs to this setting
  if (ledstat == 0) {
    digitalWrite(pin_LED, LOW);
    Serial.println("low");
  }
  if (ledstat == 1) {
    digitalWrite(pin_LED, LOW);
    Serial.println("lala");
  }
  if (ledstat == 2) {
    digitalWrite(pin_LED, HIGH);
    Serial.println("hifn");
  }
  delay(200);
  break;
}

//subtract minimum and maximum value to remove spikes
sum -= minval; sum -= maxval;

//process
if (sumsum == 0) sumsum = sum << 6; //set sumsum to expected value
long int avgsum = (sumsum + 32) >> 6;
diff = sum - avgsum;
if (abs(diff)<avgsum >> 10) {   //adjust for small changes
  sumsum = sumsum + sum - avgsum;
  skip = 0;
} else {
  skip++;
}
if (skip > 64) {  // break off in case of prolonged skipping
  sumsum = sum << 6;
  skip = 0;
}

// one permille change = 2 ticks/s
if (diff == 0) flash_period = 1000000;
else flash_period = avgsum / (2 * abs(diff));
delay(200);

return ledstat ;
}