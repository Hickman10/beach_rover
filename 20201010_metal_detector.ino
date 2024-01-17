//Metal detector based on pulse delay in LR circuit
//Connect a coil beween pin 10 and pin 8,
//a 100 Ohm resistor between pin 8 and ground,
//a speaker in series with a 10muF capacitor between pin 12 and ground,
//a reset-button between A0 and ground.
//Reference coil: 60 turns, diameter 6cm, AWG26 (0.4mm) wire. L~300muH, R~2Ohm 
//
//R. Oldeman Oct 11 2020, CC BY-NC-SA 4.0 licence

#define debug false

//some parameters
#define sensitivity  5000 //threshold as a fraction of reference      
#define aimtime    160000 //aim for measurement every 160k cycles (=10ms) 
#define LEDpulselen 16000 //length of the LED pulse in clock cycles
#define cycles         40 //number of clock cycles reserved for calculations
#define printfrac      50 //fraction of measurements printed when in debug mode

//pin definitions - cannot change freely!
#define pulsepin 10 //must be pin 10 - TIMER1B output
#define probepin  8 //must be pin 8  - input capture pin
#define LEDpin   13 //if changed also need to update the direct-port writing
#define soundpin 12 //if changed also need to update the direct-port writing
#define resetpin A0 //can be changed freely 

//global variables - shared between main loop and measurement function
long int imeas=0;  //counts the number of measurements performed 
int absdiff;       //absolute value of difference wrt reference
long int phase=0;  //tracks integral of absdiff 
long int phasemax; //value at which the phase counter turns over
int LEDcycles=0;   //number of cycles that the LED pin stays high
int LEDcycle=0;    //cycle index of the LED pulse

void setup() {
  noInterrupts(); //disable all inerrupts to avoid glitches

  if(debug)Serial.begin(9600);

  pinMode(pulsepin,OUTPUT);
  digitalWrite(pulsepin,LOW);
  pinMode(probepin,INPUT);
  pinMode(LEDpin,OUTPUT);
  digitalWrite(LEDpin,LOW);
  pinMode(soundpin,OUTPUT);
  digitalWrite(soundpin,LOW);
  pinMode(resetpin,INPUT_PULLUP);

  //setup timer1 to FASTPWM on pin 10
  TCCR1A=0B00100011;
  TCCR1B=0B00011001;
}

//perform measurement of delay of the trailing edge
long int meas(int period, int minlen, int maxlen, int steplen, int nloop){
  long int sum=0;
  int nmiss=0;
  OCR1A=period;        //set the period
  TIFR1 |= 0B00000001; //clear the timer overflow bit
  for(int iloop=0; iloop<nloop; iloop++){
    for (int len=minlen; len<maxlen; len+=steplen){
      OCR1B=len;                         //set the pulselength
      while ((TIFR1 & 0B00000001) == 0); //wait for timer overflow
      TIFR1 |= 0B00000001;               //clear the timer overflow bit
      while (TCNT1<(period-cycles));     //wait till pulse is almost finished
      if( (TIFR1&0B00100000) !=0){       //check if trailing edge has been detected
        sum+=(ICR1-len);  
        TIFR1 |= 0B00100000;             //clear the input capture flag
      } else {
        nmiss++;
      }
      //update phase
      phase+=absdiff;
      //blink and click when phase rolls through
      if(phase>phasemax){
        phase-=phasemax;
        LEDcycle=LEDcycles;
        PORTB=(PORTB|0B00100000); //switch on LED
        PORTB=(PORTB^0B00010000); //invert sound pin - click!
      }
      //switch off LED when it's been on long enough
      if(LEDcycle>0){
        LEDcycle--;
        if(LEDcycle==0)PORTB=(PORTB&(!0B00100000));
      }  
    }
  }
  if (nmiss>0)sum=0; //invalidate the result if there have been missing pulses
  return sum;
}

//flash LED with error code indefinitely if there's been an error
void go_error(byte errorcode){
  if(debug){
    Serial.print("ERROR ");
    Serial.print(errorcode);
    if(errorcode==1)Serial.println(": No pulse on probe pin - check connections");
    if(errorcode==2)Serial.println(": Delay too short - try more windings");
    if(errorcode==3)Serial.println(": Delay too long - try fewer windings");
    if(errorcode==4)Serial.println(": No robust pulse on pulselength scan - check coil resistance");
  }
  //enable in interrupts to use delay() and flash to indicate error code
  interrupts();
  while(true){  
    for(byte i=0; i<errorcode; i++){
      digitalWrite(LEDpin,HIGH);
      delay(100);
      digitalWrite(LEDpin,LOW);
      delay(100);
    }
    delay(500);
  }
}


void loop() {

  //perform initial robust measurement of average delay time
  long int tau=meas(16000,8000,8001,1,16)/16;
  if(debug)Serial.print("delay in clock cycles:");
  if(debug)Serial.println(tau);
  if(debug)Serial.print("estimated coil inductance in muH:");
  if(debug)Serial.println(1.44*tau*100.0/(16.0));
  
  if (tau==0) go_error(1);           // no pulse 
  if (tau<5)  go_error(2);           // too short 
  if (tau>800)go_error(3);           // too long

  //choose pulsing parmeters based on measured delay time
  int minlen=3*tau;
  int maxlen=5*tau;
  int period=maxlen+2*tau+cycles;
  LEDcycles=LEDpulselen/period+1;

  // repeat loop or speed up loop to approach aimed duration of measurement
  int steplen=1;
  int nloop=1;
  float tottime=float(period)*(maxlen-minlen);
  if (tottime>aimtime*(3.0/2.0)) steplen=round(tottime/aimtime);
  if (tottime<aimtime*(2.0/3.0)) nloop=round(aimtime/tottime);

  if(debug)Serial.print(" minlen: ");
  if(debug)Serial.print(minlen);
  if(debug)Serial.print(" maxlen: ");
  if(debug)Serial.print(maxlen);
  if(debug)Serial.print(" steplen: ");
  if(debug)Serial.print(steplen);
  if(debug)Serial.print(" period: ");
  if(debug)Serial.print(period);
  if(debug)Serial.print(" nloop: ");
  if(debug)Serial.print(nloop);
  if(debug)Serial.print(" LEDcycles: ");
  if(debug)Serial.println(LEDcycles);

  //perform a scan from minlen to maxlen
  long int minval=1000000000;
  for(int len=minlen; len<maxlen; len+=steplen){
    long int val=meas(period,len,len+1,1,nloop);
    minval=min(val,minval);
    if(debug)Serial.print("len,val:");
    if(debug)Serial.print(len);
    if(debug)Serial.print(" ");
    if(debug)Serial.println(val);
  }
  if(minval==0)go_error(4);

  //determine the reference value
  long int ref=meas(period,minlen,maxlen,steplen,nloop);
  
  //threshold and phasemax
  int threshold=ref/sensitivity; 
  phasemax=16000000/period*threshold; //aim for 1Hz at threshold
  if(debug)Serial.print("reference, threshold, phasemax: ");
  if(debug)Serial.print(ref);
  if(debug)Serial.print(" ");
  if(debug)Serial.print(threshold);
  if(debug)Serial.print(" ");
  if(debug)Serial.println(phasemax);

  long int sum=0;
  long int sumsq=0; 
  while(true){
    long int val=meas(period,minlen,maxlen,steplen,nloop);
 
    //check if reset of reference value is requested
    if(digitalRead(resetpin)==LOW) ref=val;
 
    int diff=val-ref;
    absdiff=abs(diff);
    if(absdiff<threshold){
      phase=0;               //reset the phase
      if(diff>0)ref+=1;      //absorb slow drifts
      if(diff<0)ref-=1;
    }

    if(debug){
      sum+=diff;
      sumsq+=long(diff)*long(diff);
      if (imeas%printfrac==0){
        float mean=float(sum)/printfrac; 
        float rms=sqrt( (float(sumsq)/printfrac) - pow(mean,2.0) ); 
        Serial.print(imeas);
        Serial.print(" val ref diff phase ");
        Serial.print(val);
        Serial.print(" ");
        Serial.print(ref);
        Serial.print(" ");
        Serial.print(diff);
        Serial.print(" ");
        Serial.print(mean);
        Serial.print(" ");
        Serial.print(rms);
        Serial.println("");
        sum=0; sumsq=0; 
      }
    }
    imeas++;
  }
}
