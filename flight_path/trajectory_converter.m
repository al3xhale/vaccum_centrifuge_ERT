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
plot(tt,x_2dot)


%% trajectory_converter
r = 20*0.01; %m

IC = [0 ; 0];
opts = odeset('RelTol',1e-4,'AbsTol',1e-6);
[tt_trajectory_angular,angular_trajectory] = ode45(@(t,theta)  angular_trajectory_ode(t,theta,x_2dot,r,h), tt, IC, opts);
[tt_trajectory_linear,lin_trajectory] = ode45(@(t,x)  linear_trajectory_ode(t,x,x_2dot,h), tt, IC, opts);


omega = angular_trajectory(:,2);
rpm = omega*(9.5493); %motor control


figure(1);
title("Omega vs t");
plot(tt_trajectory_angular,rpm);


altitude = lin_trajectory(:,1);

figure(2)
title("altitude vs t");
plot(tt_trajectory_linear,altitude);


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




