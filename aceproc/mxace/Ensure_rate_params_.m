function p = Ensure_rate_params_(p)
%
% Modified by BK, to set p.num_bands from p.electrodes if available.
% If channel_stim_rate was specified, it is not adjusted by quantized analysis_rate.
% Ensure_rate_params: Ensure rate parameters are valid.
%
% p_out = Ensure_rate_params(p_in)
%
% p_in:  A struct containing the clinical parameters.
%          Any fields omitted will be set to default values.
% p_out: A struct containing the clinical and derived parameters.
%
% Fundamental parameters:
%   audio_sample_rate:  The sample rate for the audio input signal.
%   analysis_rate:      The number of input blocks analysed per second.
%   channel_stim_rate:  The peak number of pulses per second on a channel.
% Derived parameters:
%   block_shift:        The number of new samples in each block.
%
% The analysis_rate is quantised to a sub-multiple of the audio_sample_rate.
% The channel_stim_rate may also be quantised, as follows:
% If the analysis_rate is specified, but not channel_stim_rate,
% then the channel_stim_rate is set equal to the quantised analysis_rate.
% If the channel_stim_rate is specified, but not analysis_rate,
% then the analysis_rate is set equal to the quantised channel_stim_rate.
% If both the channel_stim_rate and analysis_rate are specified,
% then the channel_stim_rate is not adjusted, and Resample_FTM_proc will be required.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%    Copyright: Cochlear Ltd
%      $Change: 46997 $
%    $Revision: #1 $
%    $DateTime: 2006/03/27 15:10:22 $
%      Authors: Brett Swanson
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

if nargin == 0
	p = [];
end

p = Ensure_field(p,'audio_sample_rate', 16000);	% Hz

if ~isfield(p,'analysis_rate')
	p = Ensure_field(p,'channel_stim_rate', 500);	% Hz
	p.analysis_rate = p.channel_stim_rate;
	p = Quantise_analysis_rate(p);
%	p.channel_stim_rate = p.analysis_rate; % commented out by BJ. Jan 24,
%	2012. If channel_stim_rate was specified, it should not be adjusted to
%	quantized analysis_rate.
else
	p = Quantise_analysis_rate(p);
	p = Ensure_field(p,'channel_stim_rate', p.analysis_rate);
end

if (isfield(p,'electrodes'))
    p = Ensure_field(p, 'num_bands',          length(p.electrodes));
else
    p = Ensure_field(p, 'num_bands',          22);
end
p = Ensure_field(p, 'num_selected',		  min(p.num_bands, 12));

p.interval_length	= round(p.analysis_rate / p.channel_stim_rate);
p.implant_stim_rate = p.channel_stim_rate * p.num_selected;

p = Ensure_implant_params(p);

period_cycles		= round(p.implant.rf_freq / p.implant_stim_rate);
p.period			= 1e6 * period_cycles / p.implant.rf_freq;	% microseconds

if (p.period < p.implant.MIN_PERIOD_us)
	implant_stim_rate = round(1e6 / p.period);
	error(sprintf('%d Hz implant stimulation rate exceeds maximum.', implant_stim_rate));
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Derived parameters:

p.rf_frame_width = 2 * p.phase_width + p.phase_gap + 4.0;	% microseconds

if (p.period < p.rf_frame_width)
	error('Frame width exceeds stim period');
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function p = Quantise_analysis_rate(p)

p.block_shift	= ceil(p.audio_sample_rate / p.analysis_rate);
p.analysis_rate	= p.audio_sample_rate / p.block_shift;
