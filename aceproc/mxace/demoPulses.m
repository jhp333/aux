function demoPulses(wavname, wavname2)


p.threshold_levels=ones(22,1); %electrode selection is not done here
p.comfort_levels=ones(22,1)*255;
p.reference_SPL_dB = 70;
p.Q=20;
p.phase_width = 25;
p.phase_gap = 8;
p.wav_sample_rate_tolerance = 1.05;
p.audio_sample_rate = 16000;
p.electrodes=(22:-1:1)';  %electrode selection is DONE HERE
p.analysis_rate=900;
p.num_selected = 8;
p.modes = 103;

q1=genaceseq2(wavname, p, '');
% q2=genaceseq2(wavname2, p, '');

cl1=getCL(q1);
% cl2=getCL(q2);
figure,bar(cl1) 
set(gcf,'pos',[164   542   560   420])
set(gca,'ylim',[0 256])
title(sprintf('%s--ACE', wavname))
% figure,bar(cl2)
% set(gcf,'pos',[164   42   560   420])
% set(gca,'ylim',[0 256])
% title(sprintf('%s--ACE', wavname2))

return
% WACE
wacep = []; 
wacep.threshold_levels = ones(22,1);
wacep.comfort_levels = ones(22,1)*255;
wacep.Q=20; % leave this alone
wacep.phase_width = 25; % leave this alone
wacep.phase_gap = 8; % leave this alone
wacep.wav_sample_rate_tolerance = 1.05; % leave this alone
wacep.reference_SPL_dB=70;
qw1=wavletace(wavname,wacep);
qw2=wavletace(wavname2,wacep);
cl3=getCL(qw1);
cl4=getCL(qw2);

figure,bar(cl3) 
set(gcf,'pos',[738   542   560   420])
set(gca,'ylim',[0 256])
title(sprintf('%s--WACE', wavname))
figure,bar(cl4)
set(gcf,'pos',[738   42   560   420])
set(gca,'ylim',[0 256])
title(sprintf('%s--WACE', wavname2))


function cl=getCL(q)
cl=zeros(22,1);
for i=1:22
    ids = find(q.electrodes==23-i);
    cl(i) = mean(q.current_levels(ids));
end