function testWACE(f)
%close all
if nargin<1, f=900;
end
fs=16000;
t=1/fs:1/fs:1;
in=sin(2*pi*f*t')*.99;
rampdur=0.2;
% in = 2*(rand(fs/2,1)-.5);
in=ramp(in,fs,rampdur);

tempname = [pwd '\temp_testWAVE'];
wavwrite(in,fs,tempname)

p.pre_emphasis=1;
p.ScaleByOutLevel=0;
p.equalise=1;
p.reference_SPL_dB = 70;
p.Q=20; % overriden below
p.electrodes=(22:-1:1)';  %electrode selection is DONE HERE
p.modes = 103;
%desired frequency boundaries (assuming 22 channels)
crossover_freqs={[187.5 250 312.5 375],...
    [375  500    625    750], ...
    [1000  1150  1300 1500   1750   2000   2250   2500   2750   3000],...   
    [3000 3500   4000   4500   5000   6000   7000   8000]};

% The following is the FFT length, analysis_rate and Num of channels for
% each group.
block_length=[256 128 64 32];
analysis_rate = [250 500 1000 2000]*.9;
channels = [3 3 9 7]; % Still assuming 22 channels
Q=[20 20 30 20];
baselevel=[4/256 4/256 4/256 4/256];
    
processes2add = {'FFT_filterbank_proc', 'Vector_sum_proc', 'Abs_proc', 'LGF_proc_'};
% Cannot call Append_process here because it automatically calls the process
% to compute the parameters.
p.processes={};
for k=1:length(processes2add)
    p.processes{end + 1, 1} = processes2add{k};	
end

FTM=cell(length(processes2add),1); cFTM=cell(length(processes2add),1);
cumChan = 0;
for k=1:length(block_length)
    chans = cumChan+[1:(channels(k)+1)];
    p.block_length = block_length(k);
    p.analysis_rate = analysis_rate(k);
    p.Q=Q(k);
    p.base_level = baselevel(k);
    p.crossover_freqs = crossover_freqs{k}';
    c = Process_chain(p, in);
    FTM{k} = c{end-1}; % FTM at right before LGF_proc
    cFTM{k} = c{end};% FTM at after LGF_proc --"compressed" FTM
    cumChan = cumChan + channels(k);
end

p.block_length=128;
p.analysis_rate = 450;
p.Q=20;

cace = Process_chain(p, in);
aceFTM = cace{end};

for k=1:length(p.processes)
    p = feval(p.processes{k},p);
end

figure('pos', [1533 234   800  590])
cumChan = 0;
cols=[0 .2 .8; 0 .6 .8; .2 .8 .4; .2 .3 .5];
for k=1:4
    ax(k)=axes('pos',[.05 .03+.15*(k-1) .9 .14]);
    obj = bar([mean(FTM{k},2) mean(cFTM{k},2)],'FaceColor',cols(k,:),'edgecolor',[1 1 1]);
    set(obj,'xdata',get(obj(1),'xdata')+cumChan)
    line([cumChan cumChan]+.5,[0 2],'linewidth',2,'color',[1 .4 0])
    for j=1:channels(k)
        text(cumChan+j-.5, .3, sprintfloat(crossover_freqs{k}(j),1),'rotation',90)
    end
    cumChan = cumChan + channels(k);
    text(cumChan+.5, .3, sprintfloat(crossover_freqs{k}(j+1),1),'rotation',90)
    line([cumChan cumChan]+.5,[0 2],'linewidth',2,'color',[1 .4 0])
    text(9,1.25,sprintf('%d-pt FFT, Q=%d', block_length(k), Q(k)))
    set(gca,'xlim',[-.5 22.5])
    xloc = max(find(f>crossover_freqs{k}));
%     if ~isempty(xloc), 
%         xloc = xloc + (f-crossover_freqs{k}(xloc))/(crossover_freqs{k}(xloc+1)-crossover_freqs{k}(xloc))-.5;
%         arrow([xloc 1.5],[xloc 1.1], 6, 30)
%     end
%     ax(k+6) = copyobj(ax(k),gcf); 
%     axes(ax(k+6));
%     set(gca,'pos', [.52 .03+.15*(k-1) .45 .14]);
%     prevBar=findobj(gca,'type','hggroup'); delete(prevBar)
%     bar(,'FaceColor',cols(k,:),'edgecolor',[1 1 1])
%     copyobj(findobj(ax(k),'type','line'),ax(k+6))
end
for k=2:4
    set(ax(k),'xtick',-10:100:1000)
end

xt=1;
for k=1:length(channels)
    if channels(k)>1
        next = xt(end)+channels(k)-1;
    else
    xt=[xt channels(k)];
    end
end

cumChan = 0;
finalFTM = FTM{1}(1:channels(1),:); finalcFTM = cFTM{1}(1:channels(1),:);
%prevalence is the column vector showing how often (ratio) the signal energy
%existed in this channel
prevalence=zeros(sum(channels),1); 
for j=1:size(finalFTM,1)
    prevalence(j) = length(find(finalFTM(j,:)>0.0001))/length(finalFTM(j,:));
end
for k=1:length(block_length)-1
    cumChan = cumChan + channels(k);
    next = FTM{k+1};
    cnext= cFTM{k+1};
    for j=1:channels(k+1)
        prevalence(cumChan+j) = length(find(next(j,:)>0.0001))/length(next(j,:));
    end
    finalFTM = interleaveTFM(finalFTM,next,NaN);
    finalcFTM = interleaveTFM(finalcFTM,cnext,NaN);
end

p.processes = '';
p.num_selected = 8;
p = Append_process(p, 'Uniform_sampler_proc');
p = Append_process(p, 'Reject_smallest_proc');
p = Append_process(p, 'Resample_FTM_proc');
p = Append_process(p, 'Collate_into_sequence_proc');
%p = Append_process(p, 'Channel_mapping_proc_');
d = Process_chain(p, finalFTM);
e = Process_chain(p, finalcFTM);
qace = Process_chain(p, aceFTM);

qw_no_LFG = d{end};
qw_LFG = e{end};
qace = qace{end};

ax(5) = axes('pos',[.05 .05+.15*(5-1) .9 .13]);
bb=nanmean(finalFTM,2);
cumChan = 0;
for k=1:length(channels)
    chans = cumChan+(1:channels(k));
    bar(chans,bb(chans),'FaceColor',cols(k,:))
    set(gca,'nextplot','add')
    cumChan = cumChan + channels(k);
end
set(gca,'xtick',get(ax(1),'xtick'))
text(8,1.25, 'Aligned FTM (before LGF)','fontsize',12)
aa=findobj(ax(4),'Tag','Arrow');
copyobj(aa,gca)

ax(6) = axes('pos',[.05 .03+.16*(6-1) .9 .13]);
bar(getChanMag({qw_LFG qace}))

xt = 1:5:21;
for i=1:length(xt), str_xt{i}=sprintf('%d',23-xt(i)); end
set(gca,'ylim',[0 1.02], 'xtick',xt,'xticklabel',str_xt)
line(1:22,prevalence,'color',[0 .5 .2],'marker','o','linestyle','none')
if p.ScaleByOutLevel, level=p.reference_SPL_dB;
else level=0; 
end
str={ sprintf('scale = %3.1f', max(in)), sprintf('PreEmp %d, Q %d ',p.pre_emphasis, p.Q), ...
    sprintf('equalise %d, level=%4.1f',p.equalise,level)};
str2=sprintf('%5.1f Hz WACE',f);
title(str2,'fontsize',14)
text(16,.95,str)
set(ax(1:6),'ylim',[-.05 1.4], 'xlim',[-.5 22.5])


function y=ramp(in,fs,rampdur)
ramplength=fix(rampdur*fs);
t=(1:ramplength)'/fs;
rampin=sin(2*pi*t/(rampdur*4));
rampout=rampin(end:-1:1);
y=[rampin.*in(1:ramplength); in(ramplength+1:end-ramplength); in(end-ramplength+1:end).*rampout];



function cl=getChanMag(q)
if size(q,2)>1
    first = getChanMag(q{:,1});
    second = getChanMag(q{:,2:end});
    cl=[first second];
else
    cl=zeros(22,1);
    for i=1:22
        ids = find(q.channels==i);
        cl(i) = mean(q.magnitudes(ids));
    end
end
    

function [v,p]=getPS_env(x,p)

p = Ensure_rate_params_(p); 

if p.pre_emphasis
    p = Pre_emphasis_proc(p);  x = Pre_emphasis_proc(p, x);
end

p = FFT_filterbank_proc (p); u = FFT_filterbank_proc (p, x);
p = Vector_sum_proc(p); v = Vector_sum_proc(p, u);
v=abs(v);

function ab=interleaveTFM(a,b,coef)
% Assuming that the columns of a and b are integer-related.
as=size(a,2); bs=size(b,2);
ratio = bs/as;
if fix(ratio*10)/10 ~= ratio, 
    ratio = ceil(ratio);
    nPads = ratio*as - bs;
    b = [b zeros(size(b,1),nPads)];
end
as=size(a,2); bs=size(b,2);
if as<bs
    temp3d(:,:,1) = a';
    for k=2:ratio
        temp3d(:,:,k) = coef*ones(size(a'));
    end
    shifted=shiftdim(temp3d,2);
    sizes=size(shifted);
    nan_a = reshape(shifted,sizes(1)*sizes(2),size(shifted,3))';
    ab = [nan_a; b];
elseif as==bs
    ab = [a; b];
else
    error('I don''t want this');
end

function out=sprintfloat(value,digit)
switch digit
    case 0
        out=sprintf('.0f',value);
    case 1
        if fix(value) == value
            out=sprintf('%.0f',value);
        else
            out=sprintf('%.1f',value);
        end
end

