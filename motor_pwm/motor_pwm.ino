/*  Arduino code for Vaccum Centrifuge project
 *  Autors: Alexandre Guerra, William Cotier, Bruno Liard
 *  Licence:GPL2 when applicable
 */


#define ENCB 3 //Interrupt pin for the B entry of the Encoder
#define pulseRot 16.0 //Number of pulation per rotation, given by pololu: 16 per encoder output


// Pins used in wiring
#define DIR 8
#define PWM 9
#define FLT 6
#define SLP 2
#define VALVE1 4
#define VALVE2 5


// the variable where the omega(t) trajectory is stored
const float omega_ref[] = {0,
1.66868272055243,
3.17353908697815,
4.50755209226184,
5.66574182009535,
6.64325160899012,
7.40927758513531,
8.05235214505146,
8.52837325187913,
8.80923215242007,
8.94564863650303,
8.90943178677519,
9.08142420282706,
9.43169193936724,
9.93797774128573,
10.6136248144939,
11.4575146901505,
12.4531100199062,
13.5971283034003,
14.8977896262992,
16.3884555938847,
18.0154129512731,
19.8137381257781,
21.7488040636217,
23.8310146077149,
26.0815657773895,
28.5284276579698,
31.0867443718839,
33.8228819365679,
36.6375408349653,
39.5922058869019,
42.6276748183226,
45.7081873616881,
48.9984613526517,
52.3702296015671,
55.852172659659,
59.3421237842793,
62.8694699245702,
66.461023621333,
70.0862705710585,
73.6562932107367,
77.1754784568816,
80.6662152115449,
84.0508092748939,
87.3649162316409,
90.4685183493285,
93.3801475075844,
96.1635388342795,
98.7121194821813,
101.04772628694,
103.297778970184,
105.181351844878,
106.867279024264,
108.460316048382,
109.932444858713,
111.192756878429,
112.334679097416,
113.502125645311,
114.680205898125,
115.828379801741,
116.894976235999,
117.905567829603,
118.892691202459,
119.866417889234,
120.950738246832,
122.149926512392,
123.135849848386,
124.02634067562,
124.953745884892,
125.778520552433,
126.644451456572,
127.615981219145,
128.492942766493,
129.341763200052,
130.202660848039,
131.166955064667,
132.102039209812,
132.684381119762,
133.663680364044,
134.396136044964,
135.216228186872,
135.971713761319,
136.705599462291,
137.508977031139,
138.202597848575,
139.029325378261,
139.859526138582,
140.558410885757,
141.22528231514,
141.901305478176,
142.646477376783,
143.446363017929,
143.870927176458,
144.838295637219,
145.433449037752,
146.103495300371,
146.70754983151,
147.331615860075,
148.006943396753,
148.632568853979,
149.272123614005,
149.928388091494,
150.525658448765,
151.127740170838,
151.710495123019,
152.347531535253,
152.919472564144,
153.502811094165,
154.065957809514,
154.547500642246,
155.22944777724,
155.796584791492,
156.244042372335,
156.859168095177
};

volatile int bState = 0; //State of ENCB
volatile int nbPulse = 0; //Number of pulse per deltaT

const float i_gear = 6.25; // the gear reductor inside the motor


// variables used to time the loop() function
unsigned long t1 = 0;
unsigned long t2 = 0;
unsigned long dt =25; //this is dynamicaly calculated


int k = 0; // discrete time variable 
int k_max = 130; // trajectory ending time discrete
int k_valve2 = 50; //discrete time to open second valve (calculated by matlab)

//float omega_cmd = 0; // the variable that stores the current reference omega [rad/s]
float rpm_cmd = 0; // the variable that stores the current reference omega [rpm]

float rpm =0; // variable used to store the encoder data


void setup() {
  Serial.begin(9600);

  // set pin
  pinMode(ENCB, INPUT);
  pinMode(FLT,INPUT);//this is set to low if a fault is detected
  pinMode(VALVE1,OUTPUT);
  pinMode(VALVE2,OUTPUT);
  
  // set pins for motor
  digitalWrite(DIR,HIGH); //default direction
  digitalWrite(SLP,HIGH);// high=normal,low=lowpower(sleep)

  // sets the interupt for the encoder data
  attachInterrupt(digitalPinToInterrupt(ENCB), readEncoderB, RISING); 

  //close both valves (to be sure)
  digitalWrite(VALVE1,LOW);
  digitalWrite(VALVE2,LOW);
  
}

void loop() {
    t1 = millis();
    nbPulse = 0; // counter for encoder rises during a loop() call

    //start trajectory, open first valve
    if(k=0){
    Serial.println("Trajectory Started:");
  
    Serial.println("open first valve");  
    digitalWrite(VALVE1,HIGH);
    }

    //Open second valve at apogee, close first
    if(k>=k_valve2){
    Serial.println("close first valve");
    digitalWrite(VALVE1,LOW);
    Serial.println("open second valve");  
    digitalWrite(VALVE2,HIGH);
    }

   if(k<k_max){
    rpm_cmd = omega_ref[k]*9.5493; //convert the [rad/s] value to [rpm]
    k = k+1; //propagate the discrete time
   }
   

  if(t2==0){
    t2 = t1 + 20; //initiate delta (used in first loop() call)
  }

   // send PWM signal to motor driver, the pid is not used, it is replaced by a experimental constant found by equating the reference rpm to the encoder rpm
   analogWrite(PWM,(int)((rpm_cmd/500)*255)); // send pwm command to motor


   //if discrete time is larger than what is should, stop
   if(k>=k_max){
    k=0;
    Serial.println("Trajectory ended");    
   }


  
  rpm = 60.0*(nbPulse/pulseRot)*(1.0/(i_gear*dt*0.001)); //calculates RPM using motor encoder


  //serial logger
  #if 1
   

   Serial.print(rpm);
   Serial.print(" ");
   Serial.print(rpm_cmd);
   Serial.println(" ");  
   //Serial.print("\t");
   //Serial.println(cmd);
  #endif

  
  //propagate the loop time lenght
  t2 = millis();
  dt = t2 -t1;
}


// encoder interrupt functions (encoder A not used because the pin is used for the motor shield)
void readEncoderB(){
  nbPulse += 1;
}
