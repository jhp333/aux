function ViewPulseHistogramNoise0
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

fprintf(1,'white noise\n');
for k=1:6
    level = -6*(k-2);
    in=(rand(fs*2,1)-.5) * (2.2-.2*k);
    auxstr=sprintf('in=noise(2000)@%d',level);
    in = auxmex(auxstr);
    a.continue=true;
    RMS = auxmex(a,'rms(in)');
    q=aceblocks(in, p, 'til_Channel_mapping_proc_');
    out=getChanMag(q);
    Elevel = mean(out);
    fprintf(1,'rms=%4.2f dB, Elevel = %5.2f\n',RMS,Elevel);
end
fprintf(1,'consonant samples\n');
for k=1:7
    level = -6*(k-2);
    b.fs=16000;
    auxstr=sprintf('in2=turnerconso_makelongwav(1)@%d',level);
    in = auxmex(b,auxstr);
    b.continue=true;
    RMS2 = auxmex(b,'rms(in2)');
    q=aceblocks(in, p, 'til_Channel_mapping_proc_');
    out=getChanMag(q);
    Elevel = mean(out);
    fprintf(1,'rms=%4.2f dB, Elevel = %5.2f\n',RMS2,Elevel);
end

fprintf(1,'vowel samples\n');
for k=1:7
    level = -6*(k-2);
    b.fs=16000;
    auxstr=sprintf('in2=hilenvow_makelongwav(1)@%d',level);
    in = auxmex(b,auxstr);
    b.continue=true;
    RMS2 = auxmex(b,'rms(in2)');
    q=aceblocks(in, p, 'til_Channel_mapping_proc_');
    out=getChanMag(q);
    Elevel = mean(out);
    fprintf(1,'rms=%4.2f dB, Elevel = %5.2f\n',RMS2,Elevel);
end


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
