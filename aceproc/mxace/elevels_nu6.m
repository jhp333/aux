function elevels_nu6(acousticDB, arg, outname)
% This calculates elevels of all NU6 files.
% if arg is 0, displays all elevels in the directory (unsorted)
% if arg is 1, displays all elevels sorted from low to high
% if outname is specified, the results are written into the file.

p.electrodes = (22:-1:1)';
p.channel_stim_rate = 900;
p.num_selected = 8;
p.Q = 20;
p.phase_width = 25*ones(size(p.electrodes));
p.phase_gap = 8;
p.modes = 103;
p.threshold_levels = zeros(size(p.electrodes));
p.comfort_levels = ones(size(p.electrodes))*100;
p.gains_dB = zeros(22,1);
p.addfieldstr = '';

nu6dir = {'C:\SoundFiles\words\nu6\LIST1\';...
    'C:\SoundFiles\words\nu6\LIST2\';...
    'C:\SoundFiles\words\nu6\LIST4\'};
nu6files = cell(50,3);
for ii = 1:length(nu6dir)
    dirresult = dir([nu6dir{ii} '*.wav']);
    curfiles = {dirresult(:).name};
    curfullfiles = cell(50,1);
    count = 1;
    for jj =1:length(curfiles)
        if ~any(strcmpi(curfiles{jj},{'SHIP.WAV','RUG.WAV','CHOOSE.WAV'}))
            curfullfiles{count}= fullfile(nu6dir{ii},curfiles{jj});
            count = count+1;
        end
    end
    nu6files(:,ii) = curfullfiles;
end
nu6files = nu6files(:);

el = NaN(length(nu6files),1);
fixedspl = acousticDB;
for ii = 1: length(nu6files)
        wavname = nu6files{ii};
        auxstr = sprintf('rms(wave("%s"))',wavname);
        wavrms = auxmex(auxstr);
        p.reference_SPL_dB = fixedspl - wavrms;
        q = wav2seq(wavname, p, '');
        [el(ii) details] = ehist(q);
end

if nargin==3
    fd=fopen(outname,'w');
else
    fd=1;
end

switch arg
    case 0
        for n = 1: length(nu6files)
            fprintf(fd, '%d:   %s   %5.1f\n', n, nu6files{n}, el(n));
        end
    case 1
        [xx yy] = sort(el);
         for n = 1: length(nu6files)
            fprintf(fd, '%d:   %s   %5.1f\n', n, nu6files{yy(n)}, xx(n));
        end
end
if fd>1
    fclose (fd);
end
            


