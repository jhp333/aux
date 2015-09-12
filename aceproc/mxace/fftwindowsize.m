function fftwindowsize
f=1125;
fs=16000;
t=1/fs:1/fs:.1;
in=sin(2*pi*f*t');
modcycle=6;
fmod = 1000/modcycle;
in = in .* (1+sin(2*pi*fmod*t'-pi/2))/2;
rampdur=0.02;
% in=ramp(in,fs,rampdur);

p.pre_emphasis=0;
p.ScaleByOutLevel=1;
p.reference_SPL_dB = 50;
p.Q=100;
p.analysis_rate=500;
p.num_selected = 8;
p.modes = 103;
p.crossover_freqs=[187.5 312.5 437.5 562.5 687.5 812.5 937.5 1062.5 1187.5 1312.5 1562.5 1812.5 2062.5 2312.5 2687.5 3062.5 3562.5 4062.5 4687.5 5312.5 6062.5 6937.5 7937.5]';
p.block_length=128;

[q1,dummy]=wav2seq(in, p, 'til_Channel_mapping_proc_'); % 128-pt FFT, 900Hz
p.analysis_rate=900;
[q2,dummy]=wav2seq(in, p, 'til_Channel_mapping_proc_'); % 128-pt FFT, 1800Hz
p.block_length=64;
p.crossover_freqs=[100 200 330 450 570 690 700 937.5 1312.5 1500 1770 1900 2100 2312.5 2687.5 3062.5 3562.5 4062.5 4687.5 5312.5 6062.5 6937.5 7937.5]';
[q3,dummy]=wav2seq(in, p, 'til_Channel_mapping_proc_'); % 64-pt FFT, 1800Hz

yy(1)=Plot_sequence_(q1);
yy(2)=Plot_sequence_(q2);
yy(3)=Plot_sequence_(q3);
figure('pos',[548   433   556   318],'color',[1 1 1])
yh=[.23 .23 .23 .18];
yadd=.03;
cap='abc';
for k=1:3
    lastyh = yadd+(yh(k)+yadd/2)*(k-1);
    ax(k)=axes('pos',[.04 lastyh+.07 .93 yh(k)]);
    cop(k)=copyobj(yy(4-k).h_lines,gca);
    ww=get(cop(k),'ydata');
    ids=find(ww>=9 | ww< 8);
    ww(ids)=NaN*ones(size(ids));
    set(cop(k),'ydata',ww)
    delete(yy(4-k).h_figure)
    set(gca,'xlim',[0 100],'ylim',[8 9],'box','on','ytick',-10:100:100)
    text(1.5,8.8,['(' cap(4-k) ')'])
end
text(-2,6.5,'Relative Channel Output','rotation',90)
k=k+1;
ax(k)=axes('pos',[.04 lastyh+yh(k-1)+yadd+.03 .93 yh(k)]);
line(t,in,'color',[0 0 0])
for k=2:4
    set(ax(k),'xticklabel','')
end
axes(ax(1))
yl=7.7;
text(5,yl,'Time (ms)')
arrow([20 yl],[30 yl],'baseangle',20,'length',8)
arrow([-2 10.5],[-2 11],'baseangle',20,'length',8)
axes(ax(4))
xx=0.0148;
patch(xx+[0 .008 .008 0],[-1.1 -1.1 1.1 1.1],[0 0 0],'FaceAlpha',.1)
text(xx+.004,-1.5,'8 ms','horizontalalignment','center')
xx=0.0367;
patch(xx+[0 .004 .004 0],[-1.1 -1.1 1.1 1.1],[0 0 0],'FaceAlpha',.1)
text(xx+.002,-1.5,'4 ms','horizontalalignment','center')
set(ax(4),'ylim',[-2 1.2],'visible','off')


% ary=-1.3;
% line(.024+[0 0.008],[ary ary])
% arrow([.024 ary],[.022 ary],'baseangle',20,'length',10), 
% arrow([.016 ary],[.018 ary],'baseangle',20,'length',10)



function y=ramp(in,fs,rampdur)
ramplength=fix(rampdur*fs);
t=(1:ramplength)'/fs;
rampin=sin(2*pi*t/(rampdur*4));
rampout=rampin(end:-1:1);
y=[rampin.*in(1:ramplength); in(ramplength+1:end-ramplength); in(end-ramplength+1:end).*rampout];
