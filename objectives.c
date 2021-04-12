 x = [float x[],float x_dot[]]; // trajectory in terms of altitude and speed
 
 struct controler_x =  [float omega_dot[], float p[]]; //trajectory in terms of angular velocity
	
controler_x = flight_path(x); // the "simulator" that translates one into another
 
 [float omega] = motor_extimator(encoder_data); //extimate motor state given raw encoder data
 
 [float pressure] = valve_extimator(float pressure_raw[],valve_control,float current time); // takes buffer of pressure mesurements, and valve dynamics to extimate current pressure
 
 [motor_control, valve_control ] = controler(float current_time,struct controler_x,float omega,float pressure); //closed look to make sure motor and valve control the variables as desired in "controler_x" trajectory
 
 NEED TO FIND SUITABLE DATATYPES FOR encoder_data, valve_control,motor_control
 