#define ENCA 2 //Interrupt pin for the A entry of the Encoder
#define ENCB 3 //Interrupt pin for the B entry of the Encoder
#define pulseRot 32 //Number of pulation per rotation, given by pololu: 32

#define DIR 13
#define PWM 5
#define FLT 10
#define SLP 11

#define VALVE 8


volatile int aState = 0; //State of ENCA
volatile int bState = 0; //State of ENCB

volatile int dir = 0; //Motor direction
volatile int nbPulse = 0; //Number of pulse per deltaT

float i_gear = 6.25; // the gear reductor inside the motor

float rpm =0;
float rpm_c = 1000;
float pwm_cmd = 0;
float err = 0;
float cmd;

float kp = 0.015;

unsigned long t1;
unsigned long t2;
unsigned long dt =25; //this is dynamicaly calculated

char serial_cmd = '+';
int buttonState = -1;

void setup() {

  Serial.begin(9600);
  
  pinMode(ENCA, INPUT);
  pinMode(ENCB, INPUT);
  pinMode(FLT,INPUT);//this is set to low if a fault is detected

  pinMode(VALVE,OUTPUT);

  rpm = 0;
  nbPulse = 0;


  attachInterrupt(digitalPinToInterrupt(ENCA), readEncoderA, RISING); //Attaching interrupt pins
  attachInterrupt(digitalPinToInterrupt(ENCB), readEncoderB, RISING); 

  //setup pins for controler
  digitalWrite(DIR,HIGH); //default direction

  digitalWrite(SLP,HIGH);// high=normal,low=lowpower(sleep)
  
}

void loop() {

  t1 = millis();
  nbPulse = 0;

  delay(10);

  t2 = millis();
  dt = t2 -t1;

  rpm = 60*(nbPulse/pulseRot)*(1/(i_gear*dt*0.001)); //calculates RPM using motor encoder

  err = rpm_c-rpm; //PID Error

  pwm_cmd =( kp*err); // the pid control law

  cmd += pwm_cmd; // add the pid command to the current pwm command (because cmd goes from 0 to 255 while the pid command regulates the pwm_cmd value to 0)

  if(cmd>255){
    cmd=255;
  }else{
    if(cmd<0){
      cmd=0;
    }
  }

  analogWrite(PWM,(int)cmd); // send pwm command to motor


  //serial debug
  #if 1
   Serial.print(rpm);
   Serial.print("\t");
   Serial.print(err);
   Serial.print("\t");
   Serial.println(rpm_c);  
   Serial.print("\t");
   Serial.println(cmd);
  #endif
  


    //Debug serial "commands"
     if(Serial.available()>0)
    {
      serial_cmd = Serial.read();
    }

    if(serial_cmd=='w')
    {
      rpm_c += 10;
    }
    if(serial_cmd=='s')
    {
      rpm_c -= 10;
    }
    if(serial_cmd=='a'){
      digitalWrite(VALVE,HIGH);
    }
    if(serial_cmd=='d'){
      digitalWrite(VALVE,LOW);
    }
    serial_cmd=='+';
  
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
