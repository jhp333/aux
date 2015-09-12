function [out,p]=wavletace0(in, p)

% The following fields in the p structure is overriden in this code:

% num_selected
% analysis_rate
% channel_stim_rate
% num_bands : this is only for the cases where all 22 electrodes are used.

% Ths following fields should have been prepared prior to this call
% reference_SPL_dB

% Release 2: Stim rates of 225,450,900,and 1800 for elec's
% 22-20,19-17,16-14,13-1
% FFT size 256 (225), 128 (450), 64 (900), 64 (1800) and 32 (1800)
% Apr 11, 2011. Authors: BJ Kwon, TT Perry

p = Wav_proc(p);
in = Wav_proc(p, in);

if size(in,2) > 1
    in = in(:,1);
end
    
p = Pre_emphasis_proc(p);  in = Pre_emphasis_proc(p, in);

[wavelet,p] = getwaveletinfo(p);

p = Ensure_field(p,'base_level',   4/256);
p = Ensure_field(p,'sat_level',   150/256);
p = Ensure_field(p,'sub_mag',    -1e-10);
p.analysis_rate=max(wavelet.analysis_rate);

% Quantize analysis rate
blockshift = ceil(p.audio_sample_rate / p.analysis_rate);
p.analysis_rate = p.audio_sample_rate / blockshift;
p.channel_stim_rate=p.analysis_rate;
p.num_selected=8;
p.num_bands = length(p.electrodes);
p.wavelet = wavelet;


%Time-Freq MTX for several different analysis_windows..padded with sub_mag
w=power_sum_env(in,wavelet,p.sub_mag);

% Now, apical channels are time-shifted to spread stimulation across a
% longer time period
amount_stagger = [6 3 0 1 2 0 1]; 
w = [w zeros(size(w,1),max(amount_stagger))];
for i=1:length(amount_stagger)
    w(i,:) = circshift(w(i,:),[1 amount_stagger(i)]);
end
w=w(:,1:end-max(amount_stagger));

v = Reject_smallest_proc(p,w);

[v,p]=ScaleDesiredLevel(p,v);

p.lgf_alpha= calculate_alpha (p); %BJ's code
[w, sub, sat] = LGF_proc(p, v);

p = Collate_into_sequence_proc(p);
cseq = Collate_into_sequence_proc(p, w);
p = Channel_mapping_proc(p);
out = Channel_mapping_proc(p, cseq);

function ab=interleaveTFM(a,b,coef)
% Assuming that the columns of a and b are integer-related.
as=size(a,2); bs=size(b,2);
ratio = bs/as;
if fix(ratio*10)/10 ~= ratio, 
    ratio = ceil(ratio);
    nPads = ratio*as - bs;
    b = [b zeros(size(b,1),nPads)];
end
as=size(a,2); bs=size(b,2);
if as<bs
    temp3d(:,:,1) = a';
    for k=2:ratio
        temp3d(:,:,k) = coef*ones(size(a'));
    end
    shifted=shiftdim(temp3d,2);
    sizes=size(shifted);
    nan_a = reshape(shifted,sizes(1)*sizes(2),size(shifted,3))';
    ab = [nan_a; b];
elseif as==bs
    ab = [a; b];
else
    error('I don''t want this');
end

function w=power_sum_env(in,wavlet,coef)

[max_analysis_rate maxID] = max(wavlet.analysis_rate);

for n=1:length(wavlet.block_length)
    p=[];
    p.block_length = wavlet.block_length(n);
    p.analysis_rate = wavlet.analysis_rate(n);
    p.bins2take=wavlet.bins2take{n};
    if isempty(wavlet.band_bins{n})
        p.band_bins = ones (length(p.bins2take),1);
    else
        p.band_bins = wavlet.band_bins{n};
    end
    p.num_bands=length(p.band_bins);
    p.num_selected=4;
    p=subProc(p);
    v=FFT_filterbank_proc(p,in);
    u{n}=Power_sum_envelope_proc_wace(p,v);
end

for k=1:length(wavlet.block_length)-1
    u{1}=interleaveTFM(u{1},u{k+1},coef);
end
w=u{1};



function p=subProc(p)
p = Ensure_rate_params(p);
p = Append_front_end_processes(p);
p = Append_process(p, 'FFT_filterbank_proc');
p = Append_process(p, 'Power_sum_envelope_proc_wace');


function [wvl, p] = getwaveletinfo(p) 
chosen = find(p.threshold_levels>0);
ct = length(chosen);
ct2 = length( find(p.comfort_levels>0));
allElectrodes = (22:-1:1)'; 
p.electrodes = allElectrodes(chosen);
p.threshold_levels = p.threshold_levels(chosen);
p.comfort_levels = p.comfort_levels(chosen);

if (ct~=ct2)
    error('The number of negative values in T and C levels (the ones that are turned off) should be the same.');
end

wvl.block_length = [256 128 64 64 32];
wvl.analysis_rate = [250 500 1000 2000 2000]*.9;
switch ct
    case 22
        wvl.band_bins={[],[],[], [], [1 1 1 1 2 2 2]'}; % if not specified, it will be ones with the same length of bins2take.
        wvl.bins2take={[3 4 5]', [3 4 5]', [3 4 5]', [6:11]', [6:15]'};
    case 21
        wvl.band_bins={[],[],[], [], [1 1 1 1 1 2 2 2]'}; % if not specified, it will be ones with the same length of bins2take.
        wvl.bins2take={[3 4 5]', [3 4 5]',  [3 4 5]', [6:9]', [5:15]'};
    case 20
        wvl.band_bins={[],[],[], [1 1 2 1 2 2 2]'}; % if not specified, it will be ones with the same length of bins2take.
        wvl.bins2take={[3 4 5]', [3 4 5]', [3 4 5]', [6:9]', [5:15]'};
    case 19
        wvl.band_bins={[],[],[], [1 1 1 1 2 2 2 2]'}; 
        wvl.bins2take={[3 4 5]', [3 4 5]', [3 4 5]', [6 7]', [4:15]'};
%   case 18
    % couldn't do this....the table was a little weird.
end
       


