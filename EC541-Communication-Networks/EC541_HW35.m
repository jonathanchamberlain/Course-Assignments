%% Appendix B: Question 5 code

% Determine number of circuts where blocking probability is < 0.01

% define rho = lambda/mu = incoming rate * mean call length
rho = 150;

% define invPB = inverse of blocking probabiliy
invPB = 1;

% define m = number of channels
m = 0;

% loop over m to determine number of channels necessary to have P_b < 0.01,
% which is equivlant to invPB > 100

while (invPB <= 100)
    m = m + 1;
    invPB = 1 + (m/rho)*invPB;
end

disp(m)
