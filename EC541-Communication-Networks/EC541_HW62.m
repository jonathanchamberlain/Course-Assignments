%% Appendix A: script to solve problem 2

%%

% define variables to track the optimum x, delay values
x_opt = 0;
W_opt = inf;

X = 1:35; % vector of possible values of items
X2 = X.^2; % vector of values squared - used for second moment computation
% loop over x = 1:34 to compute average delay (x=35 would represent
% everyone in a single queue, which clearly would have a longer wait)
for x = 1:34
    l_1 = x/35; % lambda of class 1
    l_2 = (35-x)/35; % lambda of class 2
    SM_1 = (1/x)*sum(X2(1:x)); % second moment of class 1
    W1 = (l_1*SM_1)/(2*(1-l_1/15)); % delay of class 1
    SM_2 = (1/(35-x))*sum(X2(x+1:35)); % second moment of class 1
    W2 = (l_2*SM_2)/(2*(1-l_2/15)); % delay of class 1
    W = (x/35)*W1 + ((35-x)/35)*W2; % average delay
    if W < W_opt
        W_opt = W;
        x_opt = x;
    end
end

disp(W_opt)
disp(x_opt)