clear all
close all

%Step 1: Loading in audio file
load handel.mat

start = 160000;

filename = 'file1.wav';
audiowrite(filename,y,Fs);
clear y Fs

[y,Fs] = audioread('piano.wav');
length = size(y);
length = length(1);
r = transpose(y);
%sound(y,Fs);

%Step 2 Creating Noise
mu = 0;
sigma = 0.01;
v = normrnd(mu, sigma, [1 length]);
figure()
plot(r')
title('r[n]')
figure()
plot(v')
title('Noise: v[n]')



%Step 3: x[n]
x = r + v;
figure()
plot(r')
title('r[n]')
figure()
plot(x')
title('x[n]')


%Step 4: Autocorrelations
maxLag = 20
[r_x, lagx] = xcorr(x, maxLag, 'normalized');
[r_v, lagv] = xcorr(v, maxLag, 'normalized');

figure()
stem(r_x)
title('x(k) Autocorrelation')
figure()
stem(r_v)
title('v(k) Autocorrelation')


%Step 5: Calculate Rx, and apply wiener filter
Rx = toeplitz(r_x(21:41));
rdx = xcorr(r, x, maxLag, 'normalized');
rdx = transpose(rdx(21:41))
w = inv(Rx)*rdx
hatd = filter(w, 1, x);

figure()
plot(hatd')
title('rhat')
figure()
plot(r')
title('r[n]')
figure()
plot(x')
title('x[n]')


%Step 6: mean square error
errdx = immse(r, x)
errdhat = immse(r, hatd)

%sound(hatd,Fs);
audiowrite('noisy.wav',x',Fs);
audiowrite('filtered.wav',hatd',Fs);
audiowrite('original.wav',r',Fs);
