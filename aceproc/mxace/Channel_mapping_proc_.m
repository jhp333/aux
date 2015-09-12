function q = Channel_mapping_proc_(p, cseq)

% Modified by BJ to implement non-constant phasewidth on April 25, 2009
% Bug---dissappearing idle pulses--fixed on Jan 24, 2012
% Channel_mapping_proc: Map a channel-magnitude sequence to a pulse sequence.
%
% q = Channel_mapping_proc(p, cseq)
%
% p:        client map (parameter struct).
% cseq:     channel-magnitude sequence.
%
% q:		pulse sequence.

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

	q = feval(mfilename, []);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
case 1	% Parameter calculations
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	MODE = Implant_modes;

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	% Defaults:
			
	p = Ensure_implant_params(p);
	p = Ensure_field(p,'electrodes',		(22:-1:1)');	
	p = Ensure_field(p,'modes',				MODE.MP1+2);	

	switch (p.modes)

		case MODE.MP1+2
			p = Ensure_field(p,'special_idle',	1);
            
        otherwise
			p = Ensure_field(p,'special_idle',	0);
	end;

	p = Ensure_field(p,'num_bands',			length(p.electrodes));	

	p = Ensure_field(p,'threshold_levels',	repmat(  1, p.num_bands, 1));
	p = Ensure_field(p,'comfort_levels',	repmat(100, p.num_bands, 1));
	
	if ~all(p.comfort_levels >= p.threshold_levels)
		error('Threshold level exceeds comfort level');
	end
		
	p = Ensure_field(p,'phase_width',		p.implant.default_phase_width);	% microseconds
	p = Ensure_field(p,'phase_gap',			p.implant.default_phase_gap);	% microseconds
	
	p = Ensure_field(p,'full_scale',  		   1.0);
	p = Ensure_field(p,'volume',			   100);
	p = Ensure_field(p,'volume_type',		'standard');
	
	p = Ensure_rate_params(p);

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	q = p;	% Return parameters.
	
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
case 2	% Processing
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
	if ~all(cseq.channels >= 0) | ~all(cseq.channels <= p.num_bands)
		error('Channel number out of range');
	end

	% Append idle info:
	
	electrodes			= [p.electrodes;		0];
	threshold_levels	= [p.threshold_levels;	0];
	comfort_levels		= [p.comfort_levels;	0];

	idle_pulses = (cseq.channels == 0);
	cseq.channels(idle_pulses) = length(electrodes);
    
    % Create the fields in the order we like to show them in Disp_sequence:
	q.electrodes	= electrodes(cseq.channels);
    
    MODE = Implant_modes;
    switch (p.modes)
        case {MODE.MP1, MODE.MP2, MODE.MP1+2}
            q.modes	= p.modes;% * ones(size(q.electrodes));		
        otherwise % all the other modes
            if (p.modes-1)*(p.modes-22) > 0 % if it is outside of this range
                error('Invalid stimulation mode');
            end
            q.modes = q.electrodes+p.modes;
            if max(q.modes)>22
                error('Invalid reference electrode. (e.g., E21 for BP1 not possible)');
            end
    end

    % Current level:
	
	volume			= p.volume/100;
	ranges			= comfort_levels - threshold_levels;
	
	q_magnitudes	= cseq.magnitudes / p.full_scale;
	q_magnitudes	= min(q_magnitudes, 1.0);

	q_t = threshold_levels(cseq.channels);
	q_r = ranges(cseq.channels);
		
	switch (p.volume_type)

		case 'standard'	
			q.current_levels = round(q_t + q_r .* volume .* q_magnitudes);

		case 'constant range'
			q.current_levels = round(q_t + q_r .* (q_magnitudes + volume - 1));

		otherwise
			error('Bad volume_type');
	end
	
	% Idle pulses are marked by magnitudes less than zero.
	q_is_idle = (q_magnitudes < 0);		% logical vector
	% The current levels calculated above do not apply for idle pulses,
	% set idle pulses current level to zero:
	q.current_levels(q_is_idle)	= 0;
	
	% If special_idle, then also set the idle pulses electrode to zero:
	if (p.special_idle) & any(q_is_idle)
		if length(q.electrodes) == 1	% replicate constant electrode
			q.electrodes = repmat(q.electrodes, length(q_is_idle), 1);
		end
		q.electrodes(q_is_idle) = 0;
	end
    
 	q.phase_gaps		= p.phase_gap;			% Constant phase gap.
   if length(p.phase_width)==1
       q.phase_widths		= p.phase_width;		% Copy phase width.
       q.periods			= cseq.periods;			% Copy periods.
   elseif  ~any(diff(p.phase_width)) % if all elements of p.phase_width are the same
       q.phase_widths		= p.phase_width(1);
       q.periods			= cseq.periods;
   else
       q.phase_widths = p.phase_width(cseq.channels);
       nUpdatedAnals = length(cseq.channels)/p.num_selected;
       q.periods = zeros(size(cseq.channels));
       for n=1:nUpdatedAnals
           % If the pulse with the largest PW can fit in cseq.periods
           % (which is equal to even-number grid / round-off'd value at 2nd digit of  ( 1.e6/p.channel_stim_rate/p.num_selected  )
           % don't bother and just fill in the periods with cseq.periods
           if 2*max(q.phase_widths(p.num_selected*(n-1)+1:p.num_selected*n))+p.phase_gap+11.4 <= cseq.periods
               q.periods(p.num_selected*(n-1)+1:p.num_selected*n) = cseq.periods .* ones(p.num_selected,1);
           else
           % If not, shorting gap (of which minimum is 11.4) of each pulse
           % is equally distributed across num_selected
               sum_pws = sum(q.phase_widths(p.num_selected*(n-1)+1:p.num_selected*n));
               sum_pgs = p.num_selected * p.phase_gap;
               marginLeft = p.num_selected * cseq.periods - 2*sum_pws - sum_pgs;
               %after shorting gaps
               marginLeft2 = marginLeft - p.num_selected * 11.4;
               if marginLeft<0 
                   str = sprintf('Pulses cannot fit in the specified chan period (%.1f). time id=%d, sum_pws =%.1f, sum_pgs=%.1f, SGs=%.1f', p.num_selected*cseq.periods, n, sum_pws, sum_pgs, p.num_selected*11.4);
                   error(str); 
               end
               margin2add = pseudoequaldistribute(marginLeft2,p);  %marginLeft2 semi-equally distributed across p.num_selected, in RF cycles
               for m=1:p.num_selected
                   q.periods(p.num_selected*(n-1)+m) = 2*q.phase_widths(p.num_selected*(n-1)+m) + q.phase_gaps + 11.4 + margin2add(m);
               end
           end
       end
       if max(q.periods)-min(q.periods) < 1, q.periods = mean(q.periods); end
   end
    
    if (p.modes>=0)
     	Check_sequence_(q);
    end
	
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
end

function out = pseudoequaldistribute(timeleft,p)
% out is timeleft semi-equally distributed across p.num_selected elements
% timeleft is in µs
% First convert in terms of RF cycles
rfcyc = round(timeleft/0.2);
out = ones(p.num_selected,1) * fix(rfcyc/p.num_selected);
for i=1:rfcyc-fix(rfcyc/p.num_selected)*p.num_selected
    out(i) = out(i)+1;
end
% convert it back to time scale of µs
out = 0.2 * out;