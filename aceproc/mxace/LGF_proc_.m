function [v, sub, sat] = LGF_proc_(p, u)

% LGF_proc: Loudness Growth Function
% function [v, sub, sat] = LGF_proc(p, u)
%
% Inputs:
% p:            Parameter struct, with the following fields:
% p.Q:            Percentage decrease of output when input is 10 dB below sat_level.
% p.lgf_alpha:    Curve shape factor.
% p.base_level:   Input value which maps to 0.
% p.sat_level:    Input value which maps to 1.
% p.sub_mag:      Output value used for inputs less than base_level (negative or zero).
% u:            Input magnitude vector or FTM.
%
% Outputs:
% v:            Magnitude in range 0:1 (proportion of dynamic range).
% sub:          Logical FTM indicating the values that were below base_level.
% sat:          Logical FTM indicating the values that were above sat_level.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%    Copyright: Cochlear Ltd
%      $Change: 46997 $
%    $Revision: #1 $
%    $DateTime: 2006/03/27 15:10:22 $
%      Authors: Brett Swanson
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Modified by BJ Kwon to accommodate skipping this proc if p.Q > 70
% Apr 30, 2011.

switch nargin

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
case 0	% Default parameters
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	v = feval(mfilename, []);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
case 1	% Parameter calculations
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	% Defaults:

	p = Ensure_field(p,'base_level',  4/256);
	p = Ensure_field(p,'sat_level', 150/256);
	p = Ensure_field(p,'Q',          20);
	p = Ensure_field(p,'sub_mag',    -1e-10);

	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	% Derived parameters:
if p.Q<=70	
	if (p.base_level > 0)
		% for information, not used in processing.
		p.lgf_dynamic_range = 20*log10(p.sat_level/p.base_level); 
    end
    p.lgf_alpha= calculate_alpha (p); %BJ's code
    % p.lgf_alpha=LGF_alpha(p.Q, p.base_level, p.sat_level);
end	
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	v = p;	% Return parameters.	
	
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
case 2	% Processing
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if p.Q<=70
	% Scale the input between base_level and sat_level:
	r = (u - p.base_level)/(p.sat_level - p.base_level);

	% Find all the inputs that are above sat_level (i.e. r > 1) 
	% and move them down to sat_level:
	sat = r > 1;		% This is a logical matrix, same size as r. 
	r(sat) = 1;

	% Find all the inputs that are below base_level (i.e. r < 0) 
	% and temporarily move them up to base_level:
	sub = r < 0;		% This is a logical matrix, same size as r.
	r(sub) = 0;

	% Logarithmic compression:
	v = log(1 + p.lgf_alpha * r) / log(1 + p.lgf_alpha);

	% Handle values that were below base_level:
	v(sub) = p.sub_mag;
else
    v=u;
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
end

