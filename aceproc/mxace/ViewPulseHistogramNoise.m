function ViewPulseHistogramNoise
fs=16000;
%duration is 2 sec
in=2*(rand(fs*2,1)-.5);

p.pre_emphasis=0;
p.ScaleByOutLevel=1;
p.reference_SPL_dB = 56.6;
% p.Q=20;
p.analysis_rate=900;
p.num_selected = 8;
p.modes = 103;
p.crossover_freqs=[187.5 312.5 437.5 562.5 687.5 812.5 937.5 1062.5 1187.5 1312.5 1562.5 1812.5 2062.5 2312.5 2687.5 3062.5 3562.5 4062.5 4687.5 5312.5 6062.5 6937.5 7937.5]';
p.block_length=128;

figure('pos',[696   366   545   627])
for k=1:6
    level = -6*(k-1);
    in=(rand(fs*2,1)-.5) * (2.2-.2*k);
    q=aceblocks(in, p, 'til_Channel_mapping_proc_');
    auxstr=sprintf('in=noise(2000)@%d',level);
    in = auxmex(auxstr);
    a.continue=true;
    RMS = auxmex(a,'rms(in)');
    ax(k)=axes('pos',[.05 .03+.16*(k-1) .9 .15]);
    plothistogram(p,q,ax(k));
    plottedobj = findobj(ax(k),'Type','hggroup');
    Elevel = mean(get(plottedobj,'Ydata'));
    str=sprintf('rms=%4.2f dB, Elevel = %5.2f',RMS,Elevel);
    text(0.5,.95,str);
    set(ax(k),'xtick',-150:100:100)
end

axes(ax(1));
xt = 1:1:22;
for i=1:length(xt), str_xt{i}=sprintf('%d',23-xt(i)); end
set(ax(1),'xtick',xt,'xticklabel',str_xt)
text(-1.7,-.15,'Elec#','tag','info')


function plothistogram(p,seq,ax)
axes(ax)
bar(getChanMag(seq))
set(ax,'ylim',[-.05 1.4],'xlim',[-2 22.5])


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
