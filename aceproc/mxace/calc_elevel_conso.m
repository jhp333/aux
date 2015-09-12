function out=calc_elevel_conso(dB_SPL)
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

pathh='c:\soundfiles\consonants\turner\';
cons={'b','d','dh','f','g','k','m','n','p','s','sh','t','th','v','z','zh'};
spker = {'ah','lf','ct','sy'};
for j=1:length(cons)
    fprintf(1,'%s:\t',cons{j});
    for i=1:length(spker)
        wavname = [pathh spker{i} '-a' cons{j} 'a.wav'];
        auxstr = sprintf('rms(wave("%s"))',wavname);
        wavrms = auxmex(auxstr);
        p.reference_SPL_dB = dB_SPL - wavrms;
        q = wav2seq(wavname, p, '');
        out(i,j) = ehist(q);
        fprintf(1,'%5.2f ',out(i,j));
    end
    fprintf(1,'\n');
end
        