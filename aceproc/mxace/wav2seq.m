function [q,p]=wav2seq(wavname, p, seqfilename)

% Jan, 2012
% block_interleave field is added to interleave to seqs in blocks.
% 

dummy = takecareofdummy;

if isfield(p, 'processes')
    p = rmfield(p, 'processes'); % necessary to avoid accumulating processes in subsequent calls.
end

p = mxace_map(p); 
p = prepare_gate_field(p);

p = Wav_proc(p); in=Wav_proc(p,wavname);

% If stereo input but path2 p is not ready, convert the input to mono.
if size(in,2)>1 && ~isfield(p, 'p') 
    in = sum(in,2);
end

q = Process(p,in(:,1)); %the left channel

p = Ensure_field(p, 'block_interleave', 0);
if size(in,2)>1 && isfield(p, 'p')
    if p.num_selected>0 
        q1=q;
        p.p = mxace_map(p.p);
        q2 = Process(p.p,in(:,2));  %the right channel
        if p.gate.rate>0
            [q,p] = interleave_gate (q1, q2, p); % interleave & gate
        elseif p.block_interleave == 1
            q = intleaveseqsblock (q1, q2, p);
        else
            q = interleaveseqs (q1, q2, p);
        end
    end
elseif p.gate.rate>0
    [q,p] = interleave_gate (q, [], p); % gate only
end

if nargin>2
    [pathstr, name, ext] = fileparts(seqfilename);
    switch ext
        case '.quf'
            Save_sequence_(q, seqfilename);
        case '.xml'
            newq=Sequence(q);
            write(newq, seqfilename);
        otherwise
             if ~isempty(ext)
                error('seqfilename must be either quf or xml.')
             end
    end
end


function y = prepare_gate_field(p)
gate.rate = 0;
gate.phase = 0;
p = Ensure_field(p, 'gate', gate);
p.gate = Ensure_field(p.gate, 'rate', 0);
p.gate = Ensure_field(p.gate, 'phase', 0);
y = p;

function y = takecareofdummy
y = Ensure_CIC3_params();
y = Ensure_CIC4_params();
y = Ensure_CIC4_params_();
y = HS8_microphone_proc();
y = SPrint_front_end_proc;
y = Pre_emphasis_proc;
y = Input_scaling_proc;
p = FFT_filterbank_proc;
y = Vector_sum_proc_(p);
y = Abs_proc;
y = Power_sum_envelope_proc_(p);
y = Gain_proc;
y = Reject_smallest_proc;
y = LGF_proc_;
y = Resample_FTM_proc;
y = Collate_into_sequence_proc;
y = Channel_mapping_proc_;
