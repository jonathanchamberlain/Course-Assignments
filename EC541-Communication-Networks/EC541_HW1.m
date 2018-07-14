%% 
% Jonathan Chamberlain
% U43212828
% EC 541 HW 1
%% 
% Question 1

% Create two sets of 10^6 samples of random varables X1, X2, which are
% uniformly distributed between 0 and 1

X1 = unifrnd(0,1,10^6,1);
X2 = unifrnd(0,1,10^6,1);

% a. Estimate the average of X1

fprintf("Mean of X1: %.3f\n",mean(X1));

% b. Estimate the average of X3 = min(X1,X2)

X3 = min(X1,X2);
fprintf("Mean of X3: %.3f\n",mean(X3));

% c. Estimate the average of X4 = max(X1, X2)

X4 = max(X1,X2);
fprintf("Mean of X4: %.3f\n",mean(X4));

%%
% d. Plot the ccdf of X5 = X1 + X2

X5 = X1 + X2;
h = cdfplot(X5);
CCDFX = h.XData;
CCDFY = 1 - h.YData;
figure(2)
plot(CCDFX, CCDFY)
title('Plot of CCDF of X5')

CCDFX2 = CCDFX + 6;
figure(3)
plot(CCDFX2, CCDFY)
hold on
X = 0:.01:10;
Y = zeros(1001);
Y(1:601) = 1;
Y(602:701) = 1 - ((X(602:701)-6).^2)./2;
Y(702:801) = 2 + ((X(702:801)-6).^2)./2 - 2*(X(702:801)-6);
plot(X,Y,'r')
title('Simulated vs Analytical CCDF curve')
legend('Simmulated','Analytical')

%%
X = unifrnd(0,5,10^6,1);
Y = unifrnd(0,5,10^6,1);
Xblock = 0;
Yblock = 0;
for i = 1:10^6
    X_i = X(i);
    Y_i = Y(i);
    if (X_i > Y_i)
        if (X_i < Y_i + 2)
            Xblock = Xblock + 1;
        end
    else
        if (Y_i < X_i + 1)
            Yblock = Yblock + 1;
        end
    end
end
fprintf('X Block probability: %.3f\n',Xblock/10^6)
fprintf('Y Block probability: %.3f\n',Yblock/10^6)
