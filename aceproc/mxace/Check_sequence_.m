function Check_sequence_(q)

% Check_sequence: Checks the validity of a pulse sequence.
% An error occurs if the sequence is invalid.
% The comparisons are written so that NaNs are detected,
% as any numerical comparison with a NaN gives false.
%
% Check_sequence(q)
%
% q:        pulse sequence.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%    Copyright: Cochlear Ltd
%      $Change: 46997 $
%    $Revision: #1 $
%    $DateTime: 2006/03/27 15:10:22 $
%      Authors: Brett Swanson
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    % Check for out of range electrodes.
    if ~all(q.electrodes <= 22)
        error('Electrode out of range');
    end
    if ~all(q.electrodes >= 0)
        error('Electrode out of range');
    end

% Check for out of range current levels.
if ~all(q.current_levels <= 255)
    error('Current level too large');
end
if ~all(q.current_levels >= 0)
    error('Current level negative');
end 

% Check for out of range phase widths.
%commented out by BK. Jan-16, 2008
%if ~all(q.phase_widths <= p.implant.maximum_phase_width)
%    error('Phase width too large');
%end
%if ~all(q.phase_widths >= p.implant.minimum_phase_width)
%    error('Phase width too small');
%end 
