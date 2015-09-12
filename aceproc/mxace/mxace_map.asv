function p = mxace_map(p)

% mxace_map: Calculate ACE map parameters.
% To perform processing, use:
%   q = Process(p, audio)
%
% p_out = mxace_map(p_in)
%
% p_in:  A struct containing the clinical parameters.
%          Any fields omitted will be set to default values.
% p_out: A struct containing the clinical and derived parameters.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%    Copyright: Cochlear Ltd
%      $Change: 46997 $
%    $Revision: #1 $
%    $DateTime: 2006/03/27 15:10:22 $
%      Authors: Brett Swanson
%
% Modified by BJ Kwon, Jan 24, 2012. 
% In order to set input sound level properly including SPrint setting,
% use this instead of original ACE_map.

if nargin == 0
	p = [];
end

p = Ensure_field(p, 'map_name', 'mxace');

% This is necesary to put additional fields defined in Aceplayer.

if isfield(p, 'addfieldstr') & ischar(p.addfieldstr) & length(p.addfieldstr)>3
    p = addmlines(p, p.addfieldstr);
end

% This line is necesary to ensure to append front_end and SPrint processes.
p.source = 'HS8'; % For my experiments, we don't use anything else. 

p = Ensure_rate_params_(p);
p = Ensure_field(p, 'special_idle', 0); % For NIC2 and L34, this must be 0. Channel_mapping_proc was based on SPrint

p = Append_front_end_processes(p);
p = Append_process(p, 'FFT_filterbank_proc');
if isfield(p, 'crossover_freqs') 
    p = Append_process(p, 'Vector_sum_proc_');
    p = Append_process(p, 'Abs_proc');
else
    p = Append_process(p, 'Power_sum_envelope_proc_');
end
p = Append_process(p, 'Gain_proc');
p = Append_process(p, 'Reject_smallest_proc');
p = Append_process(p, 'LGF_proc_');

if (p.channel_stim_rate ~= p.analysis_rate)
	p = Append_process(p, 'Resample_FTM_proc');
end

p = Append_process(p, 'Collate_into_sequence_proc');
p = Append_process(p, 'Channel_mapping_proc_');

