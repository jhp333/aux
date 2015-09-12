function q = intleaveseqsblock(q1, q2, p)

% BJ Kwon, Jan 24, 2012
% Release 1
%
% This interleaves q1 and q2 in period blocks. q1 and q2 must have been
% computed with the same analysis rate. The total stimulation is to be
% analysis rate times the sum of num_selected1 and num_selected2.
% (and is to be constant regardless of phasewidths. No discretion depending
% on the variation of phasewidths---e.g., if one channel has longer 
% phasewidth so it cannot be fit but could fit OK overall by reducing 
% still it doesn't adjust like this)

if p.channel_stim_rate ~= p.p.channel_stim_rate
    str = sprintf ('Two sequences must have the same analysis rate to be interleaved in analysis blocks. %d, %d', p.channel_stim_rate, p.p.channel_stim_rate);
    error(str);
end
q.periods = 1.e6/(p.analysis_rate * (p.num_selected+p.p.num_selected));
pw = max(max(q1.phase_widths),max(q2.phase_widths));
pg = max(max(q1.phase_gaps),max(q2.phase_gaps));
if q.periods - (2*pw + pg) < 11.4
    error('Two sequences cannot be interleaved because overall interpulse period is too short to accomodate to pulse trains.')
end
q.electrodes = interlv(p, q1.electrodes, q2.electrodes);
q.current_levels = interlv(p, q1.current_levels, q2.current_levels);

q.modes = getscalarorvector(p, q1.modes, q2.modes, q1, q2);
q.phase_widths = getscalarorvector(p, q1.phase_widths, q2.phase_widths, q1, q2);
q.phase_gaps = getscalarorvector(p, q1.phase_gaps, q2.phase_gaps, q1, q2);

function y = getscalarorvector (p, x1, x2, q1, q2)
if isscalar(x1) & isscalar(x2) & x1==x2
    y = x1;
else
    if isscalar(x1), x1 = x1 * ones(size(q1.electrodes)); end
    if isscalar(x2), x2 = x2 * ones(size(q2.electrodes)); end
    y = interlv(p, x1, x2);
end

function y = interlv(p, x1, x2)
nPeriods = length(x1) / p.num_selected;
if length(x2) / p.p.num_selected ~= nPeriods
    error('The length of sequences not matched to be interleaved. Check the codes.');
end
 
q1 = reshape(x1, p.num_selected, nPeriods);
q2 = reshape(x2, p.p.num_selected, nPeriods);
q = [q1; q2];
y=q(:);