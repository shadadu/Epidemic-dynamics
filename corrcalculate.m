function corrcalculate

%% maxX and maxY set the size of the 2D space for k+ and k-
km=55;
km2=500;
maxX=km;
maxY=km;
maxX2=km2;
maxY2=km2;

jointdata=load('JointDistributionFixedPositivity.txt');
Pxy=jointdata(1:maxX,1:maxY);  %% Pxy is the joint distribution from the simulation
N =sum(sum(Pxy))
m=5;
%%k+ is x, and k- is y
x=0:1:maxX-1;
y=0:1:maxY-1;

[a sizeX]=size(x);
[b sizeY]=size(y);


a=1+(.5/m);
c=2*a*(a+1);
w = 4 + 2*m + 1/m;
Gxy = zeros(sizeX,sizeY);  %% Gxy is the joint distribution we will find for the 
Fxy = zeros(sizeX,sizeY);
for i=m+1:sizeX
    for j=m+1:sizeY
       Gxy(i,j)= (gamma(x(i) + y(j) -2*m  +1)*gamma(x(i) + 1)*gamma(y(j)+1) )/(gamma(c/a + x(i) + y(j) +1)*gamma(x(i)-m+1)*gamma(y(j)-m+1) );
       Fxy(i,j)= (x(i)*y(j))^m./( x(i)+y(j) )^w; 
    end
end


Gxy;
Gxy=Gxy/sum(sum(Gxy));
Fxy=Fxy/sum(sum(Fxy));
N_Gxy=sum(sum(Gxy))
N_Fxy=sum(sum(Fxy))
newx=log(x);
newy=log(y);
newPxy=log(Pxy);
newGxy=log(Gxy);

% figure
% pcolor(x,y,newPxy)
% xlabel('log k_+')
% ylabel('log k_-')
% title('log P(k_+,k_-) vs. k_+, k_- for the network simulation; m=5, k_{max}=55')
% % 
% figure
% pcolor(x,y,newGxy)
% xlabel('log k_+')
% ylabel('log k_-')
% title('log P(k_+,k_-) vs. k_+, k_- for the exact analytic expression; m=5, k_{max}=55')

%  figure
%  surf(x,y,Gxy)
%  
%  figure
%  surf(Pxy)
%  
%  
% figure
% pcolor(x,y,Pxy)
% xlabel('k_+')
% ylabel('k_-')
% figure
% 
% pcolor(x,y,Gxy)
% xlabel('k_+')
% ylabel('k_-')


sizeX
sizeY
size(Pxy)
meanX=0;

meanX2=0;

meanX3=0;

%%find mean of k+
for i=m:sizeX
    for j=m:sizeY
        meanX = meanX + x(i)*Pxy(i,j);
        meanX2 = meanX2 + x(i)*Gxy(i,j);
        meanX3 = meanX3 + x(i)*Fxy(i,j);
    end
end
meanX
meanX2
meanX3
meanY=0;
meanY2=0;
meanY3=0;

%% find mean of k-
for i=m:sizeX
    for j=m:sizeY
        meanY = meanY + y(i)*Pxy(i,j);
        meanY2 = meanY2 + y(i)*Gxy(i,j);
        meanY3 = meanY3 + y(i)*Fxy(i,j);
          
    end
end
meanY
stdX=0; stdY=0;
stdX2=0; stdY2=0;
stdX3=0; stdY3=0;

%% find standard deviations
for i=m:sizeX
    for j=m:sizeY
        stdX = stdX + ((x(i) - meanX)^2)*Pxy(i,j);
        stdY = stdY + ((y(j) - meanY)^2)*Pxy(i,j);
        stdX2 = stdX2 + ((x(i) - meanX2)^2)*Gxy(i,j);
        stdY2 = stdY2 + ((y(j) - meanY2)^2 )*Gxy(i,j);
        stdX3 = stdX3 + ((x(i)-meanX3)^2)*Fxy(i,j);
        stdY3 = stdY3 + ((y(j)-meanY3)^2)*Fxy(i,j);
    end
end

stdX=stdX^(0.5)
stdY=stdY^(0.5)


stdX2 = stdX2^(0.5)
stdY2 = stdY2^(0.5)

stdX3 = stdX3^(0.5)
stdY3 = stdY3^(0.5)

crr2 = 0;
crr =0;
crr3=0;

%%calculate the correlations
for i=m:sizeX
    for j=m:sizeY
        
        crr = crr + (x(i) - meanX )*(y(j) - meanY )/( (stdX*stdY) )*Pxy(i,j);
       crr2 = crr2 + (x(i) - meanX2 )*(y(j) - meanY2 )/( (stdX2*stdY2) )*Gxy(i,j); 
       crr3 = crr3 + (x(i) - meanX3 )*(y(j) - meanY3 )/( (stdX3*stdY3) )*Fxy(i,j); 
        
    end
end

crr  %%gives the correlation from the simulation results of joint degree distribution

crr2 %% gives the correlation from the exact analytic formula for joint degree distribution

crr3




