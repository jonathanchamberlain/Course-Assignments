%% Appendix A: Code to generate Pareto distributed samples, and statistics

% Part b. Write a short program to generate 10,000 samples of Pareto
% distributed RV, where paramaters alpha = 1, beta = 3

% generate 10,000 random samples of a uniform RV distributed between 0 and
% 1
U = unifrnd(0,1,1,10000);
% use result from part a. to convert to Pareto RV X
X = (1./U).^(1/3) - 1;

% Part C. find sample expectation, 95% confidence interval

% sample expectation is simply the mean of the samples
M = mean(X);
fprintf("Sample expectation: %.3f\n",M)

% to get 95% confidence interval, get sample stddev, multiply by Z_1-0.05/2
s = std(X)/100; % sample std dev, divided by square root of number of samples
delta = 1.96*s; % Z_1-0.05/2 = 1.96
% display upper and lower bounds of confidence interval
fprintf("Lower Bound of CI: %.3f\n",M - delta)
fprintf("Upper Bound of CI: %.3f\n",M + delta)
