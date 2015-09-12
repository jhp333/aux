function out=calc_elevel_vow(dB_SPL)
%out is elevel in a 4 by 16 matrix
p.electrodes=(22:-1:1)';
p.channel_stim_rate=900;
p.reference_SPL_dB = dB_SPL;
p.num_selected=8;
p.Q=20;
p.phase_width = 25*ones(size(p.electrodes));
p.phase_gap = 8;
p.modes = 103;
p.threshold_levels=zeros(size(p.electrodes)); %electrode selection is not done here
p.comfort_levels=ones(size(p.electrodes))*100;
p.gains_dB=zeros(22,1);
p.addfieldstr = '';

vpath='c:\SoundFiles\vowels\hillenbrand\';
spker ={'w03', 'w04', 'w13', 'm03', 'm04', 'm13'};
vow={'aw', 'eh', 'ei', 'er', 'ih', 'iy', 'oa', 'uh', 'uw'};
for j=1:length(vow)
    fprintf(1,'%s:\t',vow{j});
    for i=1:length(spker)
        filename = [spker{i} vow{j} '.wav'];
        wavname = fullfile(vpath,filename);
        auxstr = sprintf('rms(wave("%s"))',wavname);
        wavrms = auxmex(auxstr);
        p.reference_SPL_dB = dB_SPL - wavrms;
        q = wav2seq(wavname, p, '');
        out(i,j) = ehist(q);
        fprintf(1,'%5.2f ',out(i,j));
    end
    fprintf(1,'\n');
end
        