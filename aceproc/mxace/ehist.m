function [elevel,detail]=ehist(q)
[detail,elevel] = getStat(q);

function [out, elevel] = getStat(q)
out = [];
nominalI = 1760;
cumul_current_ss_mAmp=0;
for i=22:-1:1
    ids = find(q.electrodes==i);
    if ~isempty(ids)
        counts(i) = length(ids);
        current_mAmp = cl2i(q.current_levels(ids),nominalI);
        current_sum_square = current_mAmp'*current_mAmp;
        cumul_current_ss_mAmp = cumul_current_ss_mAmp + current_sum_square;
        chanRMS = sqrt(current_sum_square/counts(i));
        chanRMSinCL(i) = i2cl(chanRMS,nominalI);
        mini(i) = min(q.current_levels(ids));
        maxi(i) = max(q.current_levels(ids));
        out = [out; i counts(i) mini(i) maxi(i) chanRMSinCL(i)];
    end
end

elevel_mAmp = sqrt(cumul_current_ss_mAmp / length(q.current_levels));
elevel = i2cl(elevel_mAmp,nominalI);

function cl=i2cl(i,max)
cl = 255 * log(i/10) ./ log(max/10);

function i=cl2i(cl,max)
i=10 * (max/10) .^ (cl/255);