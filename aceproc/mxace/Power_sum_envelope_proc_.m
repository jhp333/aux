function v = Power_sum_envelope_proc_(p, u)

% Power_sum_envelope_proc_: Power-sum envelopes for FFT filterbank
% using arbitrary frequency boundaries (lower & upper)
% based on Power_sum_envelope_proc in original NMT
%
% function v = PS_envelope_set_cutoffs_proc(p, u)
%
% Inputs:
% p:                Parameter struct containing the fields:
% p.block_length:     The FFT length.
% p.window:           The FFT window function.
% p.num_bins:         The number of FFT bins to retain.
% p.weights:          The weights used in summing the bins.
% p.num_bands:        The number of bands in the filterbank.
% p.freq_limits:      requested frequency boundaries (lower & upper)
%
% Outputs:
% v:                Frequency-Time-indexed Matrix (FTM) of envelopes.
% p.freq_ends:     quantized frequency boundaries  

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%    $Revision: 4 $
%        $Date: 07/28/07
%      Modifier: Bomjun Kwon
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Original: Power_sum_envelope_proc.m
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%    Copyright: Cochlear Ltd
%      $Change: 46997 $
%    $Revision: #1 $
%    $DateTime: 2006/03/27 15:10:22 $
%      Authors: Brett Swanson
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

switch nargin

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
case 0	% Default parameters
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	v = feval(mfilename, []);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
case 1	% Parameter calculations
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	% Fundamental parameters:

	p = Ensure_field(p, 'equalise',  1);
	p = Ensure_field(p, 'num_bands', 22);
  	p = Ensure_field(p, 'freq_limits', [188 7938]);

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	% Derived parameters:

	[p.band_bins cumBins] = freqwidth(p.num_bands, p.freq_limits);  

	% Weights matrix for combining FFT bins into bands:
	p.weights = zeros(p.num_bands, p.num_bins);
	bin = 3;	% We always ignore bins 0 (DC) & 1.
	for band = 1:p.num_bands;
		width = p.band_bins(band);
		p.weights(band, bin:(bin + width - 1)) = 1;
		bin = bin + width;
	end

	% Optionally incorporate frequency response equalisation:

	freq_response  = freqz(p.window/2, 1, p.block_length);
	power_response = freq_response .* conj(freq_response);

	P1 = power_response(1);
	P2 = 2 * power_response(2);
	P3 = power_response(1) + 2 * power_response(3);

	p.power_gains = zeros(p.num_bands, 1);
	for band = 1:p.num_bands;
		width = p.band_bins(band);
		if (width == 1)
			p.power_gains(band) = P1;
		elseif (width == 2)
			p.power_gains(band) = P2;
		else
			p.power_gains(band) = P3;
		end
	end

	if p.equalise
		for band = 1:p.num_bands;
			p.weights(band, :) = p.weights(band, :) / p.power_gains(band);
		end	
	end

	cum_num_bins = p.freq_limits(1) / p.bin_freq + cumBins;
	p.crossover_freqs = cum_num_bins * p.bin_freq;
	p.band_widths = diff(p.crossover_freqs);
	p.char_freqs = p.crossover_freqs(1:p.num_bands) + p.band_widths/2;
	
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	v = p;	% Return parameters.	
	
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
case 2	% Processing
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	v = u .* conj(u);						% Power (magnitude squared) of each bin.
	u = p.weights * v;						% Weighted sum of bin powers.
	v = sqrt(u);							% Magnitude.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
end
