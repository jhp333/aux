function q=interleaveseqs(q1, q2, p)

% Note on Jan 25, 2012. Just be aware that the length of periods field in the output q may different from other fields.... should be OK as long as aceproc handles is gracefully.
%
% Release 9, Jan 26, 2012, small bug fixed---Error on shgap if phasewidths
% is an array with different length.

% Release 8, Apr 26, 2009
% Two sequences with different pw and period can be interleaved...
% BUT only if the chan stim rate is the same for them.
% general case is too difficult to do at this point.
% Chan stim rate is checked in genaceseq0.m so just assume that they are
% the same.

% Release 7, Mar 6, 2008

% Release 6. Sep 21, 2007
% Allows interleaving different pw/pg

% Release 5. May 11,2005
% staggered 
% default=0, (not staggered, interleaved with the same alignment)

p = Ensure_field(p, 'staggered', 0);

staggered = p.staggered;
nCh1 = p.num_selected;
nCh2 = p.p.num_selected;
len1 = length(q1.electrodes);
len2 = length(q2.electrodes);
if length(q1.periods)*length(q2.periods) == 1
    if q1.periods==q2.periods
        chPeriod1=1.e6/p.analysis_rate;
        chPeriod2=chPeriod1;
    else 
    % This works for interleaving 1200 Hz and 600 Hz with 8 max each.... but
    % not working in general, (e.g., 720 Hz and 720 *3/2 Hz
    % Take care of this later...
        chPeriod1=1.e6/p.channel_stim_rate;
        chPeriod2=1.e6/p.p.channel_stim_rate;
    end
else
    chPeriod1=1.e6/p.channel_stim_rate;
    chPeriod2=chPeriod1;
end

if chPeriod1>chPeriod2
    temp = p;
    newp = p.p;
    temp = rmfield (temp, 'p');
    newp.p = temp;
    newp.staggered=p.staggered;
    q = interleaveseqs(q2, q1, newp); 
    return; 
end

lm = lcm(p.channel_stim_rate, p.p.channel_stim_rate);
rep4LCM1 = lm / p.p.channel_stim_rate;
rep4LCM2 = lm / p.channel_stim_rate;
longPeriod = chPeriod1 * rep4LCM1;
longPeriod2 = chPeriod2 * rep4LCM2;


if rep4LCM1>20 && rep4LCM2>20
   error('Too high LCM of periods');
end 
% avgPeriods = longPeriod/(rep4LCM1*nCh1+rep4LCM2*nCh2);
avgPeriods = 1/(1/q1.periods+1/q2.periods);
if avgPeriods < p.implant.MIN_PERIOD_us
   str = sprintf('Total stimulation rate %7.1f exceeds the limit for %s.', 1.e6/avgPeriods, p.implant.IC);
   error(str);
end 
% Shorting gap must be equally distributed
shgap = (longPeriod - rep4LCM1*nCh1*(2*mean(q1.phase_widths)+mean(q1.phase_gaps)) - rep4LCM2*nCh2*(2*mean(q2.phase_widths)+mean(q2.phase_gaps)))...
        /(rep4LCM1*nCh1+rep4LCM2*nCh2);
if mean(shgap)<11.4
    str=sprintf('Interleaving requires minimum shorting interval=%f',mean(shgap));
    error(str);
end
%Now, interleaving is done by the long period, (not the period).
q=q1;
q.electrodes=interleave(q1.electrodes,rep4LCM1*nCh1, q2.electrodes,rep4LCM2*nCh2, 1, staggered);
if (length(q1.modes) * length(q2.modes)==1) && (q1.modes==q2.modes)
    q.modes = q1.modes;
else
    if length(q1.modes)==1, q1.modes = q1.modes*ones(len1,1); end
    if length(q2.modes)==1, q2.modes = q2.modes*ones(len2,1); end
    q.modes=interleave(q1.modes, rep4LCM1*nCh1, q2.modes, rep4LCM2*nCh2, 102, staggered);
end
% for the left over padded frames, electrodes 1, 102 are chosen--if this
% creates a problem, adjust them to other electrodes--but don't use 0, 100, 101 or 102 for AE.
q.current_levels=interleave(q1.current_levels, rep4LCM1*nCh1, q2.current_levels,rep4LCM2*nCh2, 0, staggered);

%size of one long period packet is max1 * LCM/T1 + max2 * LCM/T2
%These pulses occur in LCM
LCM = lcm(round(chPeriod1), round(chPeriod2));
onepulse1 = 2*q1.phase_widths + q1.phase_gaps + 11.8;
onepulse2 = 2*q2.phase_widths + q2.phase_gaps + 11.8;
n1PulsesPerPacket = p.num_selected * LCM /round(chPeriod1);
n2PulsesPerPacket = p.p.num_selected * LCM /round(chPeriod2);
nPulsesPerPacket = n1PulsesPerPacket + n2PulsesPerPacket;
nPackets = fix(length(q.current_levels) / nPulsesPerPacket);
if length(q1.phase_widths)*length(q2.phase_widths) == 1
    if q1.phase_widths==q2.phase_widths
        q.periods = avgPeriods;
    else % constant but different phasewidth
        % calculate the minimum time that accommodates all the pulses for
        % one long period and check if that is below LCM
        durpulses = onepulse1 * n1PulsesPerPacket + onepulse2 * n2PulsesPerPacket;
        durationmargin = LCM - durpulses;
        if durationmargin >= 0 
            durationmarginClocks = fix (durationmargin/0.2); % quantized by the clock rate 0.2 us.
            q1.periods = onepulse1 + fix(durationmarginClocks/nPulsesPerPacket)*.2;
            q2.periods = onepulse2 + fix(durationmarginClocks/nPulsesPerPacket)*.2;
            % A total of nPulsesPerPacket pulses are filled up in durpulses. periods becomes the minimum biphasic pulse duration (including shorting)
            % plus allowable margin calculated by all the pulses of other phasewidth and the total number of pulses. 
            % Now, need to distribute the remainder of the margin (after divided by nPulsesPerPacket) to each pulse possible to use it up
            % otherwise, the inaccuracy in timing will accumulate (the remainder per each long period)
            per = [q1.periods * ones(n1PulsesPerPacket,1); q2.periods * ones(n2PulsesPerPacket,1)];
            remainder = zeros(nPulsesPerPacket, 1);
            for j=1:durationmarginClocks - fix(durationmarginClocks/nPulsesPerPacket) * nPulsesPerPacket,  remainder(j) = 1; end
            per = per + remainder;
            per = repmat(per, 1, nPackets);
            q1.periods = per(1:n1PulsesPerPacket,:); q1.periods=q1.periods(:);
            q2.periods = per(n1PulsesPerPacket+1:nPulsesPerPacket,:); q2.periods=q2.periods(:);
            q.periods=interleave(q1.periods, rep4LCM1*nCh1, q2.periods, rep4LCM2*nCh2, 70., staggered);
        else
            error('Too many maxima or too wide phasewidth to interleave'); 
        end
    end
else % one of the phasewidths is not constant--then inspect all pulses for phasewidths and numbers and see if all can fit in.
    q1.periods = zeros(length(q1.current_levels),1);
    q2.periods = zeros(length(q2.current_levels),1);
%     fd=fopen('record.txt','w');
%     fprintf(fd, 'nPulsesPerPacket=%d(%d+%d)\n',nPulsesPerPacket, n1PulsesPerPacket, n2PulsesPerPacket);
%     fprintf(fd, 'nPackets=%d\n',nPackets);
%     fclose(fd);

    for k=1:nPackets-1
        if length(q1.phase_widths) == 1, q1.phase_widths = q1.phase_widths * ones (length(q1.current_levels),1); end
        if length(q2.phase_widths) == 1, q2.phase_widths = q2.phase_widths * ones (length(q2.current_levels),1); end
        begin1ID = n1PulsesPerPacket*(k-1)+1; end1ID = n1PulsesPerPacket*k;
        begin2ID = n2PulsesPerPacket*(k-1)+1; end2ID = n2PulsesPerPacket*k;
%        fd=fopen('record.txt','a');    
%        fprintf(fd,'k=%d, beginID:endID=%d:%d, %d:%d\n', k, begin1ID, end1ID, begin2ID, end2ID);
%        fclose(fd);
       durpulses1 = 2*sum(q1.phase_widths(begin1ID:end1ID)) + (q1.phase_gaps + 11.8) * n1PulsesPerPacket;
        durpulses2 = 2*sum(q2.phase_widths(begin2ID:end2ID)) + (q2.phase_gaps + 11.8) * n2PulsesPerPacket;
        durationmargin = LCM - durpulses1 - durpulses2;
        if durationmargin >= 0 
            durationmarginClocks = fix (durationmargin/0.2); % quantized by the clock rate 0.2 us.
            for j=0:n1PulsesPerPacket-1
                q1.periods(begin1ID+j) = 2 * q1.phase_widths(begin1ID+j) + q1.phase_gaps + 11.8 + fix(durationmarginClocks/nPulsesPerPacket)*.2;
            end
            for j=0:n2PulsesPerPacket-1
                q2.periods(begin2ID+j) = 2 * q2.phase_widths(begin2ID+j) + q2.phase_gaps + 11.8 + fix(durationmarginClocks/nPulsesPerPacket)*.2;
            end
        else
             error('Too many maxima or too wide phasewidth to interleave'); 
        end
    end
    for j=n1PulsesPerPacket*(nPackets-1)+1:length(q1.current_levels)
        q1.periods(j) = 2 * q1.phase_widths(j) + q1.phase_gaps + 11.8 + fix(durationmarginClocks/nPulsesPerPacket)*.2;
    end
    for j=n2PulsesPerPacket*(nPackets-1)+1:length(q2.current_levels)
        q2.periods(j) = 2 * q2.phase_widths(j) + q2.phase_gaps + 11.8 + fix(durationmarginClocks/nPulsesPerPacket)*.2;
    end
    q.periods=interleave(q1.periods, rep4LCM1*nCh1, q2.periods, rep4LCM2*nCh2, 70., staggered);
end

if length(q1.phase_widths) * length(q2.phase_widths)==1 && q1.phase_widths==q2.phase_widths
    q.phase_widths = q1.phase_widths;
else
    if length(q1.phase_widths)==1, q1.phase_widths = q1.phase_widths*ones(len1,1); end
    if length(q2.phase_widths)==1, q2.phase_widths = q2.phase_widths*ones(len2,1); end
    q.phase_widths=interleave(q1.phase_widths, rep4LCM1*nCh1, q2.phase_widths, rep4LCM2*nCh2, 25, staggered);
end
if length(q1.phase_gaps) * length(q2.phase_gaps)==1 && q1.phase_gaps==q2.phase_gaps
    q.phase_gaps = q1.phase_gaps;
else
    if length(q1.phase_gaps)==1, q1.phase_gaps = q1.phase_gaps*ones(len1,1); end
    if length(q2.phase_gaps)==1, q2.phase_gaps = q2.phase_gaps*ones(len2,1); end
    q.phase_gaps=interleave(q1.phase_gaps, rep4LCM1*nCh1, q2.phase_gaps, rep4LCM2*nCh2, 8, staggered);
end



   
function ab=interleave(a,nCh1,b,nCh2, trailFillValue, staggered)
totalLen = length(a)+length(b);
a=appendTrailValue(a,nCh1,trailFillValue);
b=appendTrailValue(b,nCh2,trailFillValue);
numSlots1=length(a)/nCh1;
numSlots2=length(b)/nCh2;
numSlots = min(numSlots1, numSlots2);
if  numSlots1 > numSlots2
    rem = a(numSlots2*nCh1+1:end);
    a=a(1:numSlots2*nCh1);
else
    rem = b(numSlots1*nCh2+1:end);
    b=b(1:numSlots1*nCh2);
end
a=reshape(a,nCh1, numSlots);
b=reshape(b,nCh2, numSlots);
if staggered==1
    stgrows=fix(size(b,1)/2)+1;
    b=[b(stgrows:size(b,1),:); b(1:stgrows-1,:)];
end

ab=interpose(a,b);
ab=ab(:);
% rem = removeTrail(rem,trailFillValue);
% The above line used to work when ghost trailvalues were filled--don't
% remember why I needed them.
ab=[ab;rem];
ab=ab(1:totalLen);

function result=interpose(a,b)
mata=a; matb=b;
a=size(a,1); b=size(b,1);
if a>b
    result = -1*ones(a+b,size(mata,2));
    j=1;k=1;
    for i=1:a+b
        if j/a < k/b
            result(i,:) = mata(j,:); j=j+1;
        else
            result(i,:) = matb(k,:); k=k+1;
        end
    end
elseif b>a
    result = interpose(matb,mata);
else % a=b
    result=zeros(2*a,size(mata,2));
    for i=1:a
        result(2*i-1,:)=mata(i,:);
        result(2*i,:)=matb(i,:);
    end
end

function y=appendTrailValue(x, nCh, value)
numSlots=length(x)/nCh;
if fix(numSlots) ~= numSlots
    desiredLength = ceil(numSlots)*nCh;
    if size(x,1)==1, y = [x ones(1,desiredLength-length(x))];
    else             y = [x; value*ones(desiredLength-length(x),1)];
    end
else
    y=x;
end

function y=removeTrail(x,value)
ids=find(x~=value);
y=x(ids);
