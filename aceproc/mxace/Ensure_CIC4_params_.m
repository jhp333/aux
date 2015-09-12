function implant = Ensure_CIC4_params_(implant)

% Minimum period, phasewidth and phasegap modified by BJ Kwon 
% to reflect observation with L34 and emulator. Jan 25, 2012
%
% Ensure_CIC4_params: Ensure implant parameters are valid for CIC4.
%
% p_out = Ensure_CIC4_params(p_in)
%
% p_in:  A struct containing the implant parameters.
%          Any fields omitted will be set to default values.
% p_out: A struct containing the derived parameters.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%    Copyright: Cochlear Ltd
%      $Change: 46997 $
%    $Revision: #1 $
%    $DateTime: 2006/03/27 15:10:22 $
%      Authors: Brett Swanson
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

if nargin == 0
    implant = [];
end

implant.IC = 'CIC4';

implant = Ensure_field(implant, 'protocol', 'condensed');
implant = Ensure_field(implant, 'rf_freq',  5e6);	% Hz

% Minimum period in microseconds:
implant.MIN_PERIOD_us = 39;

implant.default_phase_width   =  12;
implant.default_phase_gap     =  6;
