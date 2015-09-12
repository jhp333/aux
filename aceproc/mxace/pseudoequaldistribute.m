function out = pseudoequaldistribute(timeleft,p)
% out is timeleft semi-equally distributed across p.num_selected elements
% timeleft is in µs
% First convert in terms of RF cycles
rfcyc = round(timeleft/0.2);
out = ones(p.num_selected,1) * fix(rfcyc/p.num_selected);
for i=1:rfcyc-fix(rfcyc/p.num_selected)*p.num_selected
    out(i) = out(i)+1;
end
% convert it back to time scale of µs
out = 0.2 * out;