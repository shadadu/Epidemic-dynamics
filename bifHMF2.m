function inf_HMF

global lambda sigma gamma nu alpha beta infstate M P 

init_avgdeg=3.33;

ninf=101;
n_nu=ninf;
nu_array=zeros(1,n_nu);
inf_array=zeros(1,ninf);
I_SS=zeros(1,ninf);
N_SS=zeros(1,ninf);
k_SS=zeros(1,ninf);

inf_array(1)=0.001;
 nu_array(1)=0.001;

for n=2:ninf
    inf_array(n)=inf_array(n-1)+.0003;
     nu_array(n)= nu_array(n-1)+.0003;
end

 


M=100;                      %M is the maximum node degree
N0=6000;                   %number of nodes in the network

alpha=10;
beta=1;
d=1;
k0=10;
lambda=.005;
sigma=50;
gamma=0;
nu=.01;

infstate=2;

%for run=1:ninf
    
    %for run2=1:n_nu
    
     
    %run
    
    %run2
    
%    lambda=inf_array(run);
    
   % nu=nu_array(run2);
   
   lambda=.005
        
    
    
    c=1/k0;


%initial degree distribution
ysum0=zeros(1,M+1);
x0=zeros(infstate,M+1);
for k=1:M+1
   ysum0(k)= ( ( (init_avgdeg^(k-1))*exp(-init_avgdeg) )/factorial(k-1))*N0;
end


%x(1,k) is for susceptible nodes with degree k
%x(2,k) is for infected nodes with degree k
I0=40;

init_infected=I0/N0;    %sigma is the initial fraction of the population that is susceptible

   for k=1:M+1
       x0(1,k)=(1-init_infected)*ysum0(k);
       x0(2,k)=(init_infected)*ysum0(k);     
   end

y0=zeros(1,2*(M+1));
for i=1:M+1
    y0(i)=x0(1,i);
end

for i=M+2:2*(M+1)
    y0(i)=x0(2,i-(M+1));
end


%ydot=zeros(M+1,1);       
%declare array to hold differential equations
%y=y0;           
%degree distribution

P=zeros(infstate,M+1);           %array of payoff values for degree/benefit class


for j=1:infstate
   for k=1:M+1
      P(j,k)=1/(1 + (c*(k-1))^d);
   end
end


% the formula for payoff for each degree  --   Matlab has no zero array index
% so k here corresponds to k-1 in C++ simulation 

 
degree=zeros(M+1,1);
degree(1)=0;
for i=1:(M+1)
    degree(i)=i-1;
end

%figure
%plot(degree,y0/N)   %plots initial degree distribution
%hold on

sq0=0;
   for n=1:M+1
       sq0=sq0 + (n-1)*(y0(n));
   end
   
   for n=M+2:2*(M+1)
      sq0= sq0 + ((n-1)-(M+1))*(y0(n)); 
   end
   
avedeg0=sq0/N0;
  

%end time
tf=4500;


opts=odeset('reltol',1.e-6, 'abstol',1.e-6);

%compute solution using built in ode23 solver

[tout,  yout]= ode23s('infdydt', [0, tf], y0);

z1=zeros(M+1,1);
z2=zeros(M+1,1);
z=zeros(M+1,1);

youtend=yout(end,:)';

for i=1:M+1
     z1(i)= z1(i) + yout(end,i);
end

z1out=z1;

for i=M+2:2*(M+1)
    z2(i-(M+1))=  z2(i-(M+1)) + yout(end,i);
end

z2out=z2;
for i=1:M+1
   z(i)=z1(i) + z2(i); 
end


sum(yout(end,:));
sum(yout(end,:) );
(yout(end,:))';

[T numN]=size(yout);
sq=zeros(T,1);
sq1=zeros(T,1);
sq2=zeros(T,1);
susceptibles=zeros(T,1);
infecteds=zeros(T,1);

w=zeros(T,M+1);
w1=zeros(T,M+1);
w2=zeros(T,M+1);

for j=1:T
   for i=1:M+1
    w(j,i)=yout(j,i);
    w1(j,i)=w(j,i);
    susceptibles(j) = susceptibles(j) +  w1(j,i);
    end
end

for j=1:T
 for i=M+2:2*(M+1)
   w(j,i-(M+1))= w(j,i-(M+1)) + yout(j,i); 
   w2(j,i-(M+1))=yout(j,i);
   infecteds(j)  =  infecteds(j)  +  yout(j,i);
 end
end

for i=1:T
  for n=1:M+1
        sq(i) =  sq(i) + (n-1)*w(i,n);
       sq1(i) = sq1(i) + (n-1)*w1(i,n);
       sq2(i) = sq2(i) + (n-1)*w2(i,n);  
  end
end

population=zeros(T,1);

for i=1:T
   for n=1:numN
        population(i)=population(i) + yout(i,n);
   end
   
   sq(i) =  sq(i)/population(i);
  sq1(i) = sq1(i)/population(i);
  sq2(i) = sq2(i)/population(i);
  
  avedeg(i)=abs(sq(i));
  
end

%lasty =( (yout(end,:))')/population(T);

susceptibles=susceptibles./population;
infecteds=infecteds./population;

[susceptibles infecteds];

% I_SS(run)=(infecteds(T));
% N_SS(run)=(population(T));
% k_SS(run)=avedeg(T);

yout(end,:)';


% figure
% plot(degree,abs(z1)/population(T),'r-*')
% hold on
% plot(degree,abs(z2)/population(T),'c-*')
% hold on
% plot(degree,abs(z)/population(T),'g-*')
% xlabel('degree')
% ylabel('number of nodes with this degree')



% hold on
% plot(tout, sq1,'r')
% hold on 
% plot(tout, sq2,'b')


  figure
  plot(tout, population,'r')
  xlabel('time')
  ylabel('population')
  %hold on
  figure
  %plot(tout, susceptibles,'b')
  %hold on
  plot(tout, infecteds,'b')
  xlabel('time')
  ylabel('i*')
  
  
%   if run==ninf & run2==n_nu
    figure
    plot(tout, sq,'b')
    xlabel('t')
    ylabel('<k>')
%   end
   
    clear ysum0 x0 w1 w2 w z1 z2 z z2out sq1 sq2 sq P susceptibles infecteds population tout yout T numN
  
    %end
% end
% 
%  
% 
% figure
% plot(inf_array,I_SS)
% xlabel('infection rate')
% ylabel('steady state fraction infected')
% 
% figure
% plot(inf_array,N_SS)
% xlabel('infection rate')
% ylabel('steady state population')
% 
% figure
% plot(inf_array,k_SS)
% xlabel('infection rate')
% ylabel('steady state population')


  
%   figure
%   surf(inf_array,nu_array,I_SS)
%   xlabel('infection rate')
%   ylabel('rate of death from infection, \nu')
%   zlabel('steady state fraction infected, I*')
%   
%   figure
%   surf(inf_array,nu_array,N_SS)
%   xlabel('infection rate, \lambda')
%   ylabel('rate of death from infection, \nu')
%   zlabel('N*')
%   
%   figure
%   surf(inf_array,nu_array,k_SS)
%   xlabel('infection rate, \lambda')
%   ylabel('rate of death from infection, \nu')
%   zlabel('steady state average degree, <k>*')
  
  
  
   







