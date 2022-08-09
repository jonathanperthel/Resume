%ECE 418 Project 2
%Jonathan Perthel
close all

%Step 1
mu = 0;
sigma = 1;
r = normrnd(mu, sigma, [1 200]);
v = normrnd(mu, sigma, [1 200]);
figure()
plot(r)
title('r[n]')
figure()
plot(v)
title('v[n]')


%Step 2: ARMA filter
d = filter([1 -(1/2)], [1 -(1/6)], r);
size(d)
figure()
plot(d)
hold on 
plot(r)
legend({'d[n]','r[n]'},'Location','southwest')
title('d[n]')

%Step 3: x[n]
x = d + v;
figure()
plot(d)
hold on
plot(x)
legend({'d[n]','x[n]'},'Location','southwest')
title('x[n]')

%Step 4: Autocorrelations
maxLag = 20
[r_x, lagx] = xcorr(x, maxLag, 'normalized');
[r_v, lagv] = xcorr(v, maxLag, 'normalized');
figure()
stem(lagx, r_x)
title('x(k) Autocorrelation')
figure()
stem(lagv, r_v)
title('v(k) Autocorrelation')



%Step 5: Rx matrix
rx1 =[r_x(21) r_x(22) r_x(23) r_x(24) r_x(25)]; 
rx2 =[r_x(20) r_x(21) r_x(22) r_x(23) r_x(24)]; 
rx3 =[r_x(19) r_x(20) r_x(21) r_x(22) r_x(23)]; 
rx4 =[r_x(18) r_x(19) r_x(20) r_x(21) r_x(22)]; 
rx5 =[r_x(17) r_x(18) r_x(19) r_x(20) r_x(21)];
size(rx1)

Rx = [rx1; rx2; rx3; rx4; rx5]


%Step 6: Wiener-Hopf Equation
rdx = xcorr(d, x, maxLag, 'normalized')
rdx = transpose(rdx(21:25))
w = inv(Rx)*rdx

%Step 7: Filter x(n) by w(z)
size(x)
hatd = filter(w, 1, x);
figure()
plot(hatd)
hold on
plot(d)
hold on
plot(x)
legend({'dhat','d[n]', 'x[n]'},'Location','southwest')
title('dhat')

%Step 8: mean square error
errdx = immse(d, x)
errdhat = immse(d, hatd)

%Step 9: Wiener Filter With order 20
%need 21x21 Rx
Rx = toeplitz(r_x(21:41));
rdx = xcorr(d, x, maxLag, 'normalized');
rdx = transpose(rdx(21:41));
w = inv(Rx)*rdx;
hatd = filter(w, 1, x);

figure()
plot(hatd)
hold on
plot(d)
hold on
plot(x)
legend({'dhat','d[n]', 'x[n]'},'Location','southwest')
title('dhat with Wiener Filter Order 20')

errdx = immse(d, x)
errdhat = immse(d, hatd)

%Step 10: Set d[n] to be 10*sin(n/30)
n = [1:200];
d = 10*sin(n/30);
x = d + v;
figure()
plot(d)
hold on
plot(x)
legend({'d[n]','x[n]'},'Location','southwest')
title('x[n]')

%Autocorreleations
maxLag = 20
[r_x, lagx] = xcorr(x, maxLag, 'normalized');
[r_v, lagv] = xcorr(v, maxLag, 'normalized');
figure()
stem(lagx, r_x)
title('x(k) Autocorrelation')
figure()
stem(lagv, r_v)
title('v(k) Autocorrelation')

%Calculate Rx, and apply wiener filter
Rx = toeplitz(r_x(21:41));
rdx = xcorr(d, x, maxLag, 'normalized');
rdx = transpose(rdx(21:41))
w = inv(Rx)*rdx
hatd = filter(w, 1, x);

figure()
plot(hatd)
hold on
plot(d)
hold on
plot(x)
legend({'dhat','d[n]', 'x[n]'},'Location','southwest')
title('dhat with Wiener Filter Order 20')

%Mean Square error
errdx = immse(d, x)
errdhat = immse(d, hatd)





