function q = Collate_into_sequence_proc_(p, env)

% Collate_into_sequence_proc: creates a chan-mag sequence from an FTM.
%
% function q = Collate_into_sequence_proc(p, env)
%
% Inputs:
% env:          	Envelope FTM.
% p:                Parameter struct, with the following fields:
% p.num_bands:        Number of bands (channels).
% p.channel_order:    Column vector specifying frequency (channel) order.
%
% Outputs:
% q:                Channel-magnitude sequence struct.
%                     Note that Disp_sequence displays the fields
%                     in the order they were created.

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
	
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	% Defaults:

	p = Ensure_rate_params(p);
	p = Ensure_field(p, 'channel_order_type', 'base-to-apex');

        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % Derived parameters:
        switch p.channel_order_type
            case 'base-to-apex'		% High to low frequencies
                p.channel_order	= (p.num_bands:-1:1)';
            case 'apex-to-base'		% Low to high frequencies
                p.channel_order	= (1:p.num_bands)';
            otherwise
                error('Only base-to-apex or apex-to-base implemented');
	end
	
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	q = p;	% Return parameters.	
	
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
case 2	% Processing
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	[num_bands num_time_slots] = size(env);

	q.channels = repmat(p.channel_order, num_time_slots, 1);

	reord_env = env(p.channel_order, :);			% Re-order the channels (rows).
	q.magnitudes = reord_env(:);					% Concatenate columns.

	skip = isnan(q.magnitudes);
	q.channels  (skip) = [];
	q.magnitudes(skip) = [];

	q.periods = p.period;
	
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
end
