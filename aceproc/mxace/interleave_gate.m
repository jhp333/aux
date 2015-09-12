function [q,p] = interleave_gate(q1,q2,p)
% gatePeriod should be quantized prior to this call.
gatePeriod_us = 1.e6/p.gate.rate;
if isempty(q2) % gate only
    [q1.current_levels,newGatePeriod] = onoff(q1.current_levels, p, gatePeriod_us, q1.periods, 0);
    q = q1;
else % in this case input argument p.gate.phase is not used
    [q1.current_levels,newGatePeriod] = onoff(q1.current_levels, p, gatePeriod_us, q1.periods, 0, 0);
    q1.electrodes = onoff(q1.electrodes, p, gatePeriod_us, q1.periods, 0, 0);
    q2.current_levels = onoff(q2.current_levels, p, gatePeriod_us, q1.periods, 0, 1);
    q2.electrodes = onoff(q2.electrodes, p, gatePeriod_us, q1.periods, 0, 1);
    
    q.electrodes = q1.electrodes + q2.electrodes;
    if (length(q1.modes) * length(q2.modes)==1) && (q1.modes==q2.modes)
        q.modes = q1.modes;
    else
        if length(q1.modes)==1, q1.modes = q1.modes*ones(len1,1); end
        if length(q2.modes)==1, q2.modes = q2.modes*ones(len2,1); end
        modes1 = onoff(q1.modes, p, gatePeriod_us, q1.periods, 0, 0);
        modes2 = onoff(q2.modes, p, gatePeriod_us, q1.periods, 0, 1);
        q.modes = modes1+modes2;
    end
    q.current_levels = q1.current_levels + q2.current_levels;
    q.p.gate.phase_gaps = q1.p.gate.phase_gaps;
    q.p.gate.phase_widths = q1.p.gate.phase_widths;
    %periods must be the same.
    q.periods = q1.periods;
end
quant_gate_rate = 1.e6/newGatePeriod;
p.gate.rate = quant_gate_rate;

function [y,newGatePeriod]=onoff(x,p,gatePeriod_us,qperiod,val)
% This function divide x into nGates groups and each group is 50% gated
len=length(x);
nUpdates = len/p.num_selected; % # of processed blocks
nPulsesPerGateCycle = 2*round(gatePeriod_us/qperiod/2); % quant # of pulses in one gate cycle--this must be even
newGatePeriod = nPulsesPerGateCycle*qperiod;
nGates = ceil(len/nPulsesPerGateCycle); % # of gate cycles
x = [x; zeros(nGates*nPulsesPerGateCycle-len,1)];
A = reshape(x, nPulsesPerGateCycle/2, nGates*2);
if p.gate.phase
    unitshaper1 = [zeros(nPulsesPerGateCycle/2,1) ones(nPulsesPerGateCycle/2,1)];
    unitshaper2 = [ones(nPulsesPerGateCycle/2,1) zeros(nPulsesPerGateCycle/2,1)];
else
    unitshaper1 = [ones(nPulsesPerGateCycle/2,1) zeros(nPulsesPerGateCycle/2,1)];
    unitshaper2 = [zeros(nPulsesPerGateCycle/2,1) ones(nPulsesPerGateCycle/2,1)];
end
shaper1 = repmat(unitshaper1, 1, nGates);
shaper2 = repmat(unitshaper2, 1, nGates);
A = A.*shaper1 + val*shaper2;
y=A(1:len)';
