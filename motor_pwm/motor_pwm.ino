#define ENCA 2 //Interrupt pin for the A entry of the Encoder
#define ENCB 3 //Interrupt pin for the B entry of the Encoder
#define pulseRot 32 //Number of pulation per rotation, given by pololu: 32

// PWM and DIR pins
#define DIR 13
#define PWM 5

volatile int aState = 0; //State of ENCA
volatile int bState = 0; //State of ENCB
volatile int dir = 0; //Motor direction
volatile int nbPulse = 0; //Number of pulse per deltaT

long prevT = 0;

void setup() {

  Serial.begin(9600);
  
  pinMode(ENCA, INPUT);
  pinMode(ENCB, INPUT);

  attachInterrupt(digitalPinToInterrupt(ENCA), readEncoderA, RISING); //Attaching interrupt pins
  attachInterrupt(digitalPinToInterrupt(ENCB), readEncoderB, RISING); 
}

void loop() {


  //Using the pulse counting approach
  long currT = micros(); //Current time in microseconds
  float deltaT = ((float)(currT-prevT))/(1.0e6);
  prevT = currT;

  
  //Serial.println(nbPulse);
  //Serial.println(deltaT);
  float omega = 2*PI*nbPulse/(pulseRot*deltaT); //Taking formula, seems to make sense
  //Serial.println("Current Speed, taking measure every second [rpm]:");
  Serial.println(omega);
  nbPulse = 0;


  int target_omega = 50; // [rpm]

  float kp = 1; // proportionl controler

  // error
  int e = target_omega-omega;

  // control signal
  float u = kp*e;

  // motor power
  float pwr = fabs(u);
  if( pwr > 255 ){
    pwr = 255;
  }

  //sends pwm signal (pwr) to motor
  analogWrite(PWM,pwr);

  //sets direction for motor in controler
  if(dir==1){
      digitalWrite(DIR,HIGH);
  }else{
      digitalWrite(DIR,LOW);
  }
  
  delay(500); //Because i'm too slow (is this necessary ??)
  
}

void readEncoderA(){
  //Serial.println("ENCA detected");
  motorDirection(); //Only enters this once as it is sufficient. Both impulse overlap.
  nbPulse += 16; //Just because i'm too slow with fat fingers
}

void readEncoderB(){
  //Serial.println("ENCB detected");
  nbPulse += 16;
}

void motorDirection(){
  aState = digitalRead(ENCA);
  bState = digitalRead(ENCB);
  Serial.println("Entering motorDirection");
  Serial.println(aState);
  Serial.println(bState);
  if(aState > 0 && bState == 0){ //Both impulse overlap so only need to check if both active or not
    dir = 1;
  }
  else{
    dir = 2;
  }
  if(dir == 1){
    Serial.println("Rotating clockwise");
  }
  else{
    Serial.println("Rotating counterclockwise");
  }
}
