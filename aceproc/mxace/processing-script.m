wavname='C:\soundfiles\consonants\turner\ah-asha.wav';
wavname='c:\soundfiles\words\Words_diction\friends';
wavname2='c:\soundfiles\consonants\ah-aba-st.wav';
wavname2='c:\SoundFiles\sentences\timit\dr2_fram1_sx190.wav';
% Beginning Jan 25, 2012, T, C gains_dB and pw fields have elements
% aligned with p.electrocdes (i.e., you must provide those values in the same order as p.electrodes).
p.electrodes=(21:-2:1)';

% Just to run wav2seq with default parameters, you can run with empty p.
% Or you can it with only electrode selection.

% In order to run wav2seq with HiACE mode, you need to specify the
% following: 
p.channel_stim_rate = 2400;
p.num_selected=10; 
p.implant.IC='CIC4';
% if p.channel_stim_rate * p.num_selected > 14400, output will be processed in HiACE (i.e., pw 9.6 and pg 4.8) 

% other fields that calling for attention 
p.threshold_levels=ones(size(p.electrodes))+100; 
p.comfort_levels=p.threshold_levels+100;
p.gains_dB=zeros(size(p.electrodes)); % or a scalar.
p.reference_SPL_dB = 90;
p.phase_width = 25;
p.phase_gap = 8;
p.channel_stim_rate=720;
p.num_selected = 8;
p.modes = 103;
p.block_interleave=1;
p.special_idle=0;

% This is the default 22-band frequency boundaries. 
p.crossover_freqs = [188 313 438 563 688 813 938 1063 1188 1313 1438 1688 1938 2188 2563 2938 3313 3813 4438 5188 5938 6813 7938];

wavname='asha-st';
q=wav2seq(wavname, p, '');

p.p=p;
p.p.electrodes=(20:-2:1)';
p.p.threshold_levels=ones(size(p.p.electrodes))+100;
p.p.comfort_levels=ones(size(p.p.electrodes))*200;
p.p.channel_stim_rate=360;
p.p.num_selected = 7;
p.p.modes = 2;
%p.p.phase_width=30*ones(size(p.p.electrodes));
p.p.phase_width=30;

[q2,p]=genaceseq2(wavname, p, 'c:\test.xml');

p.gate.rate=5;
[q3,p]=genaceseq2(wavname, p, 'c:\test.xml');

%WACE
wacep = []; 
wacep.threshold_levels = ones(22,1);
wacep.comfort_levels = ones(22,1)*100;
wacep.Q=20; % leave this alone
wacep.phase_width = 25; % leave this alone
wacep.phase_gap = 8; % leave this alone
wacep.wav_sample_rate_tolerance = 1.05; % leave this alone
wacep.reference_SPL_dB=80;
[qq,newp]=wavletace(wavname,wacep);