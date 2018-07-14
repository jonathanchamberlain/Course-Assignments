%% Appendix A: Question 1 code 

%% 1-b: compute expression from a. using numerical techniques

% adpated from birthday_numerical from course websirte
average = quad('1 -(1 - exp(-x./20) - (x./20).*exp(-x./20)).^20',0,10000);

fprintf("Calculated average: %.3f \n", average)

%% 1-c compute expression using simulation

% adapted from birthday_simulation from course website

K = 20; % number of coupons 
iteration_num = 10^5; % number of independent iterations
T_sum = 0; %keeps track of sum of stopping times accross all iterations (to compute average)
for i=1:iteration_num
    success = 0; %reset flag
    T = 0; %reset stopping time
    coupon_count = zeros(1,K); % each entry i in the vector counts the number of coupons of type i that were collected 
    while success == 0
        T = T+1; %increases stopping time
        coupon_type = unidrnd(K); %coupon collected uniformly at random
        coupon_count(coupon_type) = coupon_count(coupon_type) +1; %increment by 1 counter for collected coupon 
        if all(coupon_count > 1) %end when have two complete collections
            success = 1; % change stopping criterion for other problems
        end
    end
    T_sum = T_sum + T;
end
T_mean = T_sum/iteration_num; %estimate of the mean of the stopping time

fprintf("Simulated average: %.3f \n", T_mean)