%% this takes the 'TIMEACC.xls' data as imput and outputs w(t) and p(t) from t0 to tf so that the controler has the trajectory data
%% acceleration curve - convert data from exel val to a trajectory
ACC = readtable('TIMEACC.xls');
tt = ACC.Var1;
acc = ACC.Var2;
acc = -smoothdata(acc,'loess');

dt = 0.005; %s
h = 1/dt;
t0 = 0;
tf = 15;
N = (tf-t0)/dt;

% *trajectory data *
tt = tt(1:N);
x_2dot = acc(1:N);
%plot(tt,x_2dot)


%% trajectory_converter
r = 20*0.01; %m

IC = [0 ; 0];
opts = odeset('RelTol',1e-4,'AbsTol',1e-6);
[tt_trajectory_angular,angular_trajectory] = ode45(@(t,theta)  angular_trajectory_ode(t,theta,x_2dot,r,h), tt, IC, opts);

omega = angular_trajectory(:,2);

% find discrete time corresponding to apogee
domega = diff(omega);
k2_possibles = find(domega<0,10);
k_apogee = k2_possibles(find([k2_possibles;0]- [0;k2_possibles] ==1,1)) %the time to close first valve and open second

i_assembly = 3; %reductor in assembly of motor

rpm_cmd = omega*(9.5493)*i_assembly; %motor control

figure(1);
plot(tt_trajectory_angular,rpm_cmd);
title("Omega vs t");
xlabel("time [s]")
ylabel("motor velocity [rpm]")

writematrix(rpm_cmd,'rpm_cmd.txt');


%% functions
function dthetadt = angular_trajectory_ode(t,theta,x_2dot,r,h)
dthetadt(1,1)=theta(2);
current_time_discrete = floor(t*h);
dthetadt(2,1) = sign( ((x_2dot(current_time_discrete)/r).^2) -theta(2)^4)*sqrt(abs( ((x_2dot(current_time_discrete)/r).^2) -theta(2)^4));% use sign and abs to avoid getting imaginary numbers
end

function dxdt = linear_trajectory_ode(t,x,x_2dot,h)
dxdt(1,1)=x(2);
current_time_discrete = floor(t*h);
dxdt(2,1)=x_2dot(current_time_discrete);
end

function [ro,g,T,P,V_sound] = atm_icao(z)
R = 286.99236; % air gaz constant
% sea level
T0 = 288.1667; %K
P0 = 101314.628; %Pa

% ICAO temp and pressure as function of height z above sea level
if(z<=0) % remove z<0 case
    T = T0;
    P = P0;
else
    if(z<=11000) %zone 1
        T = T0 - (0.006499708).*z;
        P = P0*((1-(z*2.255692257.*10^(-5))).^(5.2561));
    else
        if((z>=11000)&&(z<=25000)) %zone 2
            T = 216.66666667;
            P = P0*0.223358.*exp( (-1.576883202.*10^(-4)).*(z-11000));
        else % zone 3
            T = 216.66666667 +(3.000145816.*10^(-3)).*(z-25000);
            P = 2489.773467.*(exp( (-1.576883202.*10^(-4)).*(z-25000) ));
        end 
    end
end

ro = P./(R.*T);
V_sound = 20.037673.*sqrt(T);

% calculate g (not a part of ICAO but useful)
G = 6.674e-11; % grav constant
r_earth = 6371e3;%m
M_earth = 5.972e24; %kg

g = G*M_earth./((r_earth+z).^2);

end





