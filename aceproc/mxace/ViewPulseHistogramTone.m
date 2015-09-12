function ViewPulseHistogramTone(f)
if nargin<1, f=500; end
fs=16000;
t=1/fs:1/fs:1;
in=sin(2*pi*f*t')*.99;
rampdur=0.02;
in=ramp(in,fs,rampdur);

p.pre_emphasis=0;
p.ScaleByOutLevel=1;
p.reference_SPL_dB = 56.6;
% p.Q=20;
p.analysis_rate=900;
p.num_selected = 8;
p.modes = 103;
p.crossover_freqs=[187.5 312.5 437.5 562.5 687.5 812.5 937.5 1062.5 1187.5 1312.5 1562.5 1812.5 2062.5 2312.5 2687.5 3062.5 3562.5 4062.5 4687.5 5312.5 6062.5 6937.5 7937.5]';
p.block_length=128;

[q,dummy]=aceblocks(in, p, 'til_Channel_mapping_proc_'); % regular ACE (128-pt FFT)

figure('pos',[777   348   560   420])
for k=1:2
    ax(k)=axes('pos',[.05 .07+.44*(k-1) .9 .42]);
end
plothistogram(p,q,ax(1));
xt = 1:1:22;
for i=1:length(xt), str_xt{i}=sprintf('%d',23-xt(i)); end
set(ax(1),'xtick',xt,'xticklabel',str_xt)
text(-1.7,-.15,'Elec#','tag','info')

p.block_length=256;

[q2,dummy,p]=aceblocks(in, p, 'til_Channel_mapping_proc_'); % 256-pt FFT

plothistogram(p,q2,ax(2));
set(ax(2),'xtick',-10:100:1000)

if  isfield(p,'ScaleByOutLevel') & p.ScaleByOutLevel, level=p.reference_SPL_dB;
else level=0; 
end
str=sprintf('ACE Pulse Histograms:%.1f Hz, scale=%.1f, PreEmph=%d, Q=%d, level=%.1fdB',...
    f, max(in), p.pre_emphasis, p.Q, level);
text(10.5,1.45,str,'HorizontalAlignment','center','tag','info')


function plothistogram(p,seq,ax)
axes(ax)
bar(getChanMag(seq))
set(ax,'ylim',[-.05 1.4],'xlim',[-2 22.5])
str=sprintf('%d-pt FFT',p.block_length);
text(10.5,1.2,str,'horizontalalignment','center')


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
