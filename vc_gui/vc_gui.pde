import processing.serial.*;

Serial myPort;        // serial port

float current_omega=0;
int k = 0; //discrete time
int dt = 3;// [ms] time step

int k_max = 3000; //final time in trajectory
int send_traj = 0;

String[] omega_traj;
int button[] = {10,20,50,20};



void setup () {
size(300, 100);
myPort = new Serial(this, Serial.list()[0], 9600);
background(0);
stroke(255,255, 255);
text("Vaccum centrifuge Trajectory Sender", 10, 10);
text("Launch",button[0],button[1]+20);

}

void draw () {
  if (mousePressed == true) 
  { 
    if(send_traj==0){
      if((mouseY<(button[1]+button[3]))&&(mouseY>(button[1]))){
  if((mouseX<(button[0]+button[2]))&&(mouseX>(button[0]))){
       k=0;
       send_traj = 1;
       println("loading omega traj");  
       omega_traj = loadStrings("pTraj.txt");
  }
  }
      
      
       

    }
 
  }
  
  if(k>=k_max){
    k=0;
    send_traj = 0;
    println("omega traj ended");     
  }
  
  if(send_traj==1){
    
    current_omega = float(omega_traj[k]);
    
    println(k);
    myPort.write(int(k));
    
    k = k+1; //avance discrete time
    delay(dt);

  }
  
  
  
  
}
