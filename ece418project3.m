%ECE 418 Project 3
%Jonathan Perthel
close all
clc

%Step 1
mu = 0;
sigma = 1;
N = 200;
re = normrnd(mu, sigma, [1 N+1]);
r = re(2:N+1);
d = filter([1 -(1/2)], [1 -(1/6) -(1/3)], r);
de = filter([1 -(1/2)], [1 -(1/6) -(1/3)], re);
figure()
plot(d)
title('d[n]')

%Step 2
v = normrnd(mu, sigma, [1 N]);
y = d + v;
figure()
plot(d)
hold on
plot(y)
legend({'d[n]','y[n]'},'Location','southwest')
title('y[n]')

%Step 3
denom = [(1/6) (1/3)];
numer = [1 1/2];
A = [denom(1) denom(2) 0; 1 0 0; 0 1 0];
w = zeros(3, N);
c = [0 1 0];

for n = 1:N
    w(1, n) = numer(1)*re(n+1) + numer(2)*r(n);
end
print
xa = zeros(3, 1, N);
Pa = zeros(3, 3, N);
Pa(1,1,1) = 1;
Pa(2,2,1) = 1;
Pa(3,3,1) = 1;


%Step 4
maxLag = 1;
rcorr = xcorr(r,maxLag, 'normalized');
Qw = [((numer(1)^2 + numer(2)^2)*rcorr(2)) 0 0; 0 0 0; 0 0 0];
Qv = xcorr(v, maxLag, 'normalized');
Qv = Qv(2);

%Step 5
xb = zeros(3,1,N);
%n=1 case
n=1;
xb(:,n) = A*xa(:,1);
Pb(:,:,n) = A*Pa(:,:, 1)*A.' + Qw;
k(:,n) = Pb(:,:,n)*(c.')/(c*Pb(:,:,n)*c.' + Qv);
xa(:, n) = (xb(:,n)) + k(:,n)*(y(n)-c*xb(:,n));
Pa(:,:,n) = (eye(3)- k(n)*c)*Pb(:,:,n);

%for loop
for n = 2:N
    xb(:,n) = A*xa(:,n-1);
    Pb(:,:,n) = A*Pa(:,:, n-1)*A.' + Qw;
    k(:,n) = Pb(:,:,n)*(c.')/(c*Pb(:,:,n)*c.' + Qv);
    xa(:, n) = xb(:,n) + k(:,n)*(y(n)-c*xb(:,n));
    Pa(:,:,n) = (eye(3)- k(:,n)*c)*Pb(:,:,n);
end

%Step 6
d_p1 = de(1:N);
figure()
plot(xa(1,:))
hold on
plot(d_p1)
legend({'d_hat[n+1]','d[n+1]'},'Location','southwest')
title('d_hat[n+1]')


xa = xa(1:N);
diff = d_p1 - xa(1,:);
figure()
plot(diff)
title('Error Signal')

meansquaredhat = immse(xa, d_p1)

% %Step 7
n = [1:N];
n1 = [1:N+1];
d = 10*sin(n/30);
de = 10*sin(n/30);

%Step 7.2
v = normrnd(mu, sigma, [1 N]);
y = d + v;
figure()
plot(d)
hold on
plot(y)
legend({'d[n]','y[n]'},'Location','southwest')
title('y[n]')

%Step 7.3
denom = [(1/6) (1/3)];
numer = [1 1/2];
A = [denom(1) denom(2) 0; 1 0 0; 0 1 0];
%A = [pi/3 0 0; 1 0 0; 0 1 0];
w = zeros(3, N);
c = [0 1 0];

for n = 1:N
    w(1, n) = numer(1)*re(n+1) + numer(2)*r(n);
end
print
xa = zeros(3, 1, N);
Pa = zeros(3, 3, N);
Pa(1,1,1) = 1;
Pa(2,2,1) = 1;
Pa(3,3,1) = 1;


%Step 7.4
maxLag = 1;
rcorr = xcorr(r,maxLag, 'normalized');
Qw = [((numer(1)^2 + numer(2)^2)*rcorr(2)) 0 0; 0 0 0; 0 0 0];
Qv = xcorr(v, maxLag, 'normalized');
Qv = Qv(2);

%Step 7.5

%n=1 case
n=1;
xb(:,n) = A*xa(:,1);
Pb(:,:,n) = A*Pa(:,:, 1)*A.' + Qw;
k(:,n) = Pb(:,:,n)*(c.')/(c*Pb(:,:,n)*c.' + Qv);
xa(:, n) = (xb(:,n)) + k(:,n)*(y(n)-c*xb(:,n));
Pa(:,:,n) = (eye(3)- k(n)*c)*Pb(:,:,n);

%for loop
for n = 2:N
    xb(:,n) = A*xa(:,n-1);
    Pb(:,:,n) = A*Pa(:,:, n-1)*A.' + Qw;
    k(:,n) = Pb(:,:,n)*(c.')/(c*Pb(:,:,n)*c.' + Qv);
    xa(:, n) = xb(:,n) + k(:,n)*(y(n)-c*xb(:,n));
    Pa(:,:,n) = (eye(3)- k(:,n)*c)*Pb(:,:,n);
end

%Step 7.6
d_p1 = de(1:N);
figure()
plot(xa(1,:))
hold on
plot(d_p1)
legend({'d_hat[n+1]','d[n+1]'},'Location','southwest')
title('d_hat[n+1]')


xa = xa(1:N);
diff = d_p1 - xa(1,:);
figure()
plot(diff)
title('Error Signal')

meansquaredhat = immse(xa, d_p1)
