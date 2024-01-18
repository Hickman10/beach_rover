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
        lift_bin();
        break;
      case 'R':
        lower_bin();
        break;
      }
  }
  
   int maxval = 0;
 
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
      delayMicroseconds(30);
      digitalWrite(pin_pulse, LOW); //takes 3.5 microseconds
      delayMicroseconds(30);
    }
    //read the charge on the capacitor
    int val = analogRead(pin_cap); //takes 13x8=104 microseconds
    minval = min(val, minval);
    maxval = max(val, maxval);
    sum += val;
 
    //determine if LEDs should be on or off
    long unsigned int timestamp = millis();
    
    if (timestamp < prev_flash +12) {
      if (diff > 0)ledstat = 2;
      if (diff < 0)ledstat = 1;
    }
    if (timestamp > prev_flash + flash_period) {
      if (diff > 0)ledstat = 2;
      if (diff < 0)ledstat = 1;
      prev_flash = timestamp;
    }
    if (flash_period > 1000)ledstat = 0;
 
    
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
  delay(4000);
  //switch the LEDs to this setting
  if (ledstat == 0) {
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("low");
  }
  if (ledstat == 1) {
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("low1");
  }
  if (ledstat == 2) {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("high");
    
    for (pos = 90; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15 ms for the servo to reach the position
    }
    
    Serial.println("hjjej");
    for (pos = 180; pos >= 90; pos -= 1) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15 ms for the servo to reach the position
    }
    delay(2000);
    
  }
    
  Serial.println("end");
  Serial.print(ledstat);
  ledstat == 0; int minval = 2000;

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
  delay(800);
}

void lower_bin(){
  // lower bin to collect incomming trash
  digitalWrite(LiftUp, LOW);
  digitalWrite(LiftDown, HIGH);\
  delay(500);
  digitalWrite(LiftUp, LOW);\
  digitalWrite(LiftDown, LOW);
}

void shake(){
  // shake to let the sand fall
}