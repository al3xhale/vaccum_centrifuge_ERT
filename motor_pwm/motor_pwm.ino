#define ENCA 2 //Interrupt pin for the A entry of the Encoder
#define ENCB 3 //Interrupt pin for the B entry of the Encoder
#define pulseRot 32.0 //Number of pulation per rotation, given by pololu: 32

#define DIR 13
#define PWM 5
#define FLT 10
#define SLP 11

#define VALVE 8



volatile int aState = 0; //State of ENCA
volatile int bState = 0; //State of ENCB

volatile int dir = 0; //Motor direction
volatile int nbPulse = 0; //Number of pulse per deltaT

const float i_gear = 6.25; // the gear reductor inside the motor

float rpm =0;
float rpm_c = 50;
float pwm_cmd = 0;
float err = 0;
float cmd = 0;

const float err_max = 300;
const float cmd_max = 255;
const float cmd_min = 0;

const float kp = 0.015;

unsigned long t1;
unsigned long t2;
unsigned long dt =25; //this is dynamicaly calculated


// Precalculated Trajectory (serial receive)
int omega_cmd = 0;
char val = '0'; //=omega_ref (serial vars are char)


void setup() {

  Serial.begin(9600);
  
  pinMode(ENCA, INPUT);
  pinMode(ENCB, INPUT);
  pinMode(FLT,INPUT);//this is set to low if a fault is detected
  pinMode(VALVE,OUTPUT);

  attachInterrupt(digitalPinToInterrupt(ENCA), readEncoderA, RISING); //Attaching interrupt pins
  attachInterrupt(digitalPinToInterrupt(ENCB), readEncoderB, RISING); 

  //setup pins for controler
  digitalWrite(DIR,HIGH); //default direction

  digitalWrite(SLP,HIGH);// high=normal,low=lowpower(sleep)
  
}

void loop() {

  if (Serial.available()) 
   { 
       val = Serial.read(); // trajectory from serial
       omega_cmd=val-'0'; // convert char to serial
    
   }

  t1 = millis();
  nbPulse = 0;

  delay(5);

  t2 = millis();
  dt = t2 -t1;

  rpm = 60.0*(nbPulse/pulseRot)*(1.0/(i_gear*dt*0.001)); //calculates RPM using motor encoder


  err = omega_cmd-rpm; //PID Error

  if(err>err_max){ //limut controler input
    err=err_max;
  }else{if(err<-err_max){
      err =-err_max;
    }
  }

  pwm_cmd =( kp*err); // the pid control law

  cmd += pwm_cmd; // add the pid command to the current pwm command (because cmd goes from 0 to 255 while the pid command regulates the pwm_cmd value to 0)
  if(cmd>cmd_max){ //limut motor input
    cmd=cmd_max;
  }else{if(cmd<cmd_min){
      cmd=cmd_min;
    }
  }

  analogWrite(PWM,(int)cmd); // send pwm command to motor

  // START OF DEBBUGIG CODE
  
  //serial debug
  #if 0
   Serial.print(rpm);
   Serial.print("\t");
   Serial.print(err);
   Serial.print("\t");
   Serial.println(rpm_c);  
   Serial.print("\t");
   Serial.println(cmd);
  #endif

  // END OF DEBBUGIG CODE
}

void readEncoderA(){
  nbPulse += 1; 

}
void readEncoderB(){
  nbPulse += 1;
}

void motorDirection(){
  aState = digitalRead(ENCA);
  bState = digitalRead(ENCB);

  if(aState > 0 && bState == 0){ //Both impulse overlap so only need to check if both active or not
    dir = 1;
  }
  else{
    dir = 2;
  }
  if(dir == 1){
    //Serial.println("Rotating clockwise");
    Serial.println(rpm);
  }
  else{
    Serial.println("Rotating counterclockwise");
    Serial.println(rpm);
  }
}
