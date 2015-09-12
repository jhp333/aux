function processing_script

wavname = 'tidbit-st';
p.electrodes=(22:-1:1)';
p.channel_stim_rate=900;
p.reference_SPL_dB = 90;
p.num_selected=8;
p.Q=20;
p.phase_width = 25*ones(size(p.electrodes));
p.phase_gap = 8;
p.modes = 103;
p.threshold_levels=zeros(size(p.electrodes)); %electrode selection is not done here
p.comfort_levels=ones(size(p.electrodes))*100;
p.gains_dB=zeros(22,1);
p.addfieldstr = '';



% p.p=p;
% p.p.electrodes=(21:-2:1)';
% p.p.threshold_levels=ones(size(p.p.electrodes))+100;
% p.p.comfort_levels=ones(size(p.p.electrodes))*200;

p.addfieldstr = 'phase_width=30';
p.p.addfieldstr = 'phase_width=11';

[q,p]=wav2seq(wavname, p, '');

Plot_sequence_(q)
k=100;