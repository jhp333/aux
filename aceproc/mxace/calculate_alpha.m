function [y, err]=calculate_alpha(p)

interval = min(max(1000*log(p.Q/5), 100),10000);
alphas = logspace(.000004,6, interval);
gam = gamma (p);
Qs = 100*(1-log (1 + gam * alphas ) ./ log (1 + alphas));

maxInd = max(find(Qs>p.Q));
y = alphas(maxInd);
err = Qfunc(p, y, gam) - p.Q;


function y = gamma (p)
y = (p.sat_level/sqrt(10)-p.base_level) /  (p.sat_level-p.base_level);

function y = Qfunc (p, alpha, gamma)
y = 100*(1-log (1 + gamma * alpha ) / log (1 + alpha));
