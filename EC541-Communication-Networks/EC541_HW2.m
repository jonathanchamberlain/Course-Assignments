%% 
% Jonathan Chamberlain
% U43212828
% EC 541 HW 2
%% Question 1-1

% Given that each user transmits at 1 Mb/s, the probability that the
% aggregate transmission rate exceeds 2 Mb/s is eqivalent to finding the
% probability that more than 2 users are transmitting at once. 
% Factorials computed via calculator due to MATLAB being unable to handle
% the large numbers invovled

x1 = (.999)^1000; % P(N = 0)
x1 = x1 + 1000 * (.999)^999*(.001)^1; % P(N = 1)
x1 = x1 + 499500 * (.999)^998*(.001)^2; % P(N = 2)
x1 = 1 - x1; % % P(N > 2)

fprintf('Analytical result P(N > 2) %.3f\n', x1)

% verify via expermientation
X1 = unifrnd(0,1,100000,1000);
total = 0;
for i = 1:100000
    Y = X1(i,:) <= .001;
    if (sum(Y) > 2)
        total = total + 1;
    end
end

fprintf('Experimental result P(N>2) %.3f\n',total/100000)

%% Question 1-2

% By similar argument to before, we want the probability that more than 5
% users are transmitting at once

x2 = (.999)^1000; % P(N = 0)
x2 = x2 + 1000 * (.999)^999*(.001)^1; % P(N = 1)
x2 = x2 + 499500 * (.999)^998*(.001)^2; % P(N = 2)
x2 = x2 + 166167000*(.999)^997*(.001)^3; % P(N = 3)
x2 = x2 + 41417124750*(.999)^996*(.001)^4; % P(N = 4)
x2 = x2 + 8250291250200*(.999)^995*(.001)^5; % P(N = 5)
x2 = 1 - x2; % P(N > 5)

fprintf('Analytical result P(N > 5) %.3f\n', x2)

% verifiy via expermientation

total = 0;
for i = 1:100000
    Y = X1(i,:) <= .001;
    if (sum(Y) > 5)
        total = total + 1;
    end
end

fprintf("Experimental result P(N>5) %.3f\n", total/100000)

%% Question 1-3

% Compute under Poission approximation and compare

% Possion approximation is of form exp(-l)*l^k/k!, where l = number of
% possible occurences*probability of occurence, k = number of occurences. 
% As there are 1000 users, and the probability any transmitter is active is
% .001, l = 1000*.001 = 1.

% For N > 2, compute Possion approximation N <= 2, and then subtract

x3 = 0;
for i = 0:2
    x3 = x3 + exp(-1)/factorial(i);
end
x3 = 1 - x3;

% For N > 5, perform similar process
x4 = 0;

for i = 0:5
    x4 = x4 + exp(-1)/factorial(i);
end
x4 = 1 - x4;

fprintf('Possion approximation N>2: %.3f\n',x3)
fprintf('Possion approximation N>5: %.3f\n',x4)


%% Question 8

% script.m code provided by Anas

data = fscanf(fopen("blocks_timestamps_2017_sorted_seconds.txt", "r"), '%d');

% get difference between successive elements
datadiff = diff(data);
% compute (empirical) CDF
[f, x] = ecdf(datadiff);
% Plot CCDF, where CCDF = 1 - CDF
plot(x,1-f,'--')
set(gca, 'YScale', 'log');
title('plots of inter-arrival time between blocks')
xlabel('inter arrival time')
ylabel('% X > x')

% Estimate rate parameter of exponential distribution of CCDF
b1 = x\f;
fprintf('Estimated rate parameter: %.5f\n',b1)

% Plot regression curve on same plot as CCDF
yreg = b1*x;
hold on
plot(x,yreg, '-.')
legend('CCDF plot', 'Linear Regression Curve')

% E[X] for an exponetial distribution = 1/rate
fprintf('Estimated expectation of distribution: %.3f\n', 1/b1)

