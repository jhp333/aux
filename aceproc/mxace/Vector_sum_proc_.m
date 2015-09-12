function x = Vector_sum_proc_(p, u)

% Vector_sum_proc: FFT vector-sum filterbank.
% Combines the FFT bins into bands.
%
% x = Vector_sum_proc(p, u)
%
% p: Struct containing the filterbank parameters.
% u: Complex FTM with p.num_bins rows (FFT_filterbank_proc output).
%
% x: Complex FTM with p.num_bands rows.

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

	x = feval(mfilename, []);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
case 1	% Parameter calculations
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	% Fundamental parameters:

	p = Ensure_field(p,'equalise',  1);
	p = Ensure_field(p,'num_freq_response_samples', 2048);

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	% Derived parameters:

	if isfield(p, 'crossover_freqs')
	
		% Frequency boundaries have been specified.
		% Use fractional bin weights.
		
       % Modified by BK. May 5, 2011. 
       [nRows, nCols] = size(p.crossover_freqs);
       if nRows == 1
           p.crossover_freqs = p.crossover_freqs';
           nRows = nCols; nCols = 1;
       end
       % Now p.crossover_freqs is either a column vector (nCols=1) or
       % numbands by 2 matrix
       %================end of BK May 5, 2011
       
		p.weights = FFT_filterbank_weights(p.crossover_freqs, p.block_length, p.audio_sample_rate);
		p.num_bands = size(p.weights, 1);
		
    else
        nCols = 1;
		% Use integer bin weights:
		
		p = Ensure_field(p, 'num_bands', 22);
		
		p.band_bins = FFT_band_bins(p.num_bands)';
		p.start_bin = 3;	% Ignore the first two bins (DC & 125 Hz).
		p.weights	= zeros(p.num_bands, p.num_bins);
		bin = p.start_bin;
		for band = 1:p.num_bands;
			n = p.band_bins(band);	% the number of FFT bins in this band
			weight_indices = bin:(bin + n - 1);
			p.weights(band, weight_indices) = 1;
			bin = bin + n;
		end

		cum_num_bins = (p.start_bin - 1.5) + [0; cumsum(p.band_bins)];
		p.crossover_freqs = cum_num_bins * p.bin_freq;
    end
    % Modified by BK. May 4, 2011. 
    % Original Vector_sum_proc code makes an error here when
    % p.crossover_freqs was defined by a nBands-by-2 matrix.
    
    if nCols>1
        p.band_widths = diff(p.crossover_freqs,1,2);
        p.char_freqs  = p.crossover_freqs(1:p.num_bands) + p.band_widths'/2;
    else
        p.band_widths = diff(p.crossover_freqs);
        p.char_freqs  = p.crossover_freqs(1:p.num_bands) + p.band_widths/2;
    end
    
    % Till this line 
	
	% Weights need to have alternating signs:
	alt = (-1).^(0:p.num_bins-1);	% row vector with alternating signs
	p.weights = p.weights .* repmat(alt, p.num_bands, 1);

	% Calculate frequency responses of each FFT bin.
	% The DC bin's impulse response is p.window.
	impulse_response = [p.window/2; zeros(p.num_freq_response_samples - p.block_length, 1)];
	H0 = fft(impulse_response);
	p.response_freqs = (0:(p.num_freq_response_samples-1))' * p.audio_sample_rate/p.num_freq_response_samples;
	
	% Each bin response is a frequency-shifted copy of the DC bin's response.
	n_bin = p.num_freq_response_samples/p.block_length;
	H = zeros(p.num_freq_response_samples, p.num_bins);
	for b = 1:p.num_bins
		H(:,b) = circshift(H0, (b-1) * n_bin);
	end
	H = H.';	% transpose (without complex conjugation)

	% Each channel frequency response is a weighted sum of FFT bin responses:
	p.freq_response = p.weights * H;

	% Equalise the gains across channels:
	p.vector_sum_gains = max(abs(p.freq_response), [], 2);	% Max of each channel (row)
	if p.equalise
		for band = 1:p.num_bands;
			p.weights(band, :) = p.weights(band, :) / p.vector_sum_gains(band);
		end	
		p.freq_response = p.weights * H;	% Recalculate with equalised weights.
	end

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	x = p;	% Return parameters.	
	
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
case 2	% Processing
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	x = p.weights * u;						% Weighted sum of bin vectors.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
end
