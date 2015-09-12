function [f, idle] = Encode_embedded_protocol_(q)

% BK's Note on Oct 5, 07
% This is used for Save_sequence_ (which is only NICstream), 
% for the revision today, q.modes will have actual "mode" --i.e., reference E
% for BP. Therefore, it will be no longer a scalar for BP. 
% (There is no change for MP)
% For BP, q.modes will be an array and no adjustment is done. In other
% words,
% %     otherwise
% %         if (q.modes-1)*(q.modes-22) > 0 % if it is outside of this range
% %         ...
% --> this part is no longer used and moved to Channel_mapping_proc_

% Note by BK regarding the mixed mode (July 22 2004):
% If q.modes is a scalar: it must be either MODE.MP1, MODE.MP2, MODE.MP3, 
% or an integer between 0 and 21. If it is either MP1 MP2 or MP3, then
% f.ms will be 24, 25, or 30, respectively. If it is between 0 and 21,
% then f.ms will be f.es + q.modes. If it is outside of that range, 
% the function will return error ("invalid mode"). The electrode pair of 
% idle frame will be (23,23)---E_POWER in NIC1 term.
% If max of f.es + q.modes is greater than 22, it will return error ("invalid
% reference electrode")
% 
% If q.modes is not a scalar (possibly in a mixed mode), then f.ms will be
% the vector sum of f.es and q.modes but if an element of q.modes is either
% MP1 MP2 or MP3, then f.ms corresponding to that index will be 24, 25 or 30,
% respectively. If any number between 22 and 100 (inclusively) exists in q.modes, then 
% it will return an error("invalid mode"). If If any number between 23 and 100 (incl)
% exists in f.ms, it will return error ("invalid reference electrode").
% 
% Modified bj BK on Jun-03-2004, power-up frames have the same mode as the choice of the mode
% Modified by BK. Apr-12.2004 to accommodate non-MP modes.
% Encode_embedded_protocol: Encode a sequence into embedded protocol.
%
% [f, idle] = Encode_embedded_protocol(q)
%
% q:    Pulse sequence (fields listed below).
%
% f:    Embedded protocol frame sequence (fields listed below).
% idle: Idle frame inferred from the pulse sequence.
%       In MP1+2, a special idle frame can be used: (E,M,A) = (24,25,0).
%       In MP1 or MP2, there is no special idle mode, instead
%       the most apical electrode is used with CL = 0.
%
%
% Pulse sequence fields             Protocol frame sequence fields
% ---------------------------       ------------------------------
% electrodes                        es
% modes                             ms
% current_levels                    as
% phase_widths (microseconds)       ws (RF cycles)
% phase_gaps   (microseconds)       gs (RF cycles)
% periods      (microseconds)       ts (RF cycles)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%    Copyright: Cochlear Ltd
%     $Archive: /SPrint Research Software/Latest/Matlab/Implant/Encode_embedded_protocol.m $
%    $Revision: 3 $
%        $Date: 2/10/02 4:06p $
%      Authors: Brett Swanson
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

PROTOCOL = Embedded_protocol_params;
MODE     = Implant_modes;

num_pulses = length(q.electrodes);

Check_sequence_(q);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Electrode & Mode:

special_idles = find(q.electrodes == 0);

f.es = q.electrodes;

if (isscalar(q.modes))
    switch (q.modes)
    case MODE.MP1
        f.ms = 24;
		idle.es = max(f.es);    % most apical electrode used.
		idle.ms = 25;
    case MODE.MP2
        f.ms = 25;
		idle.es = max(f.es);    % most apical electrode used.
		idle.ms = 25;
    case MODE.MP3
        f.ms = 30;
		idle.es = 24;
		idle.ms = 25;
    otherwise
        if (q.modes-1)*(q.modes-22) > 0 % if it is outside of this range
            error('Invalid stimulation mode');
        end
        f.ms = f.es+q.modes;
        if max(f.ms)>22
            error('Invalid reference electrode. (e.g., E21 for BP1 not possible)');
        end
		idle.es = 23;
        idle.ms = 23;
    end
else
    if length(q.electrodes)~=length(q.modes)
        error('Electrodes and modes vectors must have same length.');
    end
    if ~isempty(find(q.modes.*(q.modes-23).*(q.modes-100).*(q.modes-104)>=0))
        error('Invalid stimulation mode');
    end
    idMP1=find(q.modes==MODE.MP1);
    idMP2=find(q.modes==MODE.MP2);
    idMP3=find(q.modes==MODE.MP3);
    idNonMP=find((q.modes-101).*(q.modes-103)>0);
    f.ms = NaN * ones(size(f.es));
    f.ms(idMP1)=24*ones(length(idMP1),1);
    f.ms(idMP2)=25*ones(length(idMP2),1);
    f.ms(idMP3)=30*ones(length(idMP3),1);
% the next line was commented on apr 22-2009 for version mxace23 BJ Kwon
% because    line 100 in
% Channel_mapping_proc_.m      q.modes = q.electrodes+p.modes;     
%   f.ms(idNonMP)=f.es(idNonMP)+q.modes(idNonMP);
    f.ms(idNonMP)=q.modes(idNonMP);

    if (~isempty(idNonMP))
        modeMaxNonMP=max(f.ms(idNonMP));
        modeMinNonMP=min(f.ms(idNonMP));
        if (modeMaxNonMP-1)*(modeMaxNonMP-22)>0 | (modeMinNonMP-1)*(modeMinNonMP-22)>0 
            error('Invalid reference electrode. (e.g., E21 for BP1 not possible)');
        end
    end
	idle.es = 24;
	idle.ms = 25;
end

   
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Current level

f.as = q.current_levels;
idle.as = 0;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Phase width

phase_width_cycles = round(q.phase_widths * 5);     % 5 MHz
f.ws = phase_width_cycles - PROTOCOL.PHASE_WIDTH_BASE_CYCLES;
idle.ws = min(f.ws);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Phase gap

if ~all(q.phase_gaps >= PROTOCOL.MIN_PHASE_GAP_us)
    error('Phase gap is too small');
end;
if ~all(q.phase_gaps <= PROTOCOL.MAX_PHASE_GAP_us)
    error('Phase gap is too large');
end;

phase_gap_cycles = round(q.phase_gaps * 5);         % 5 MHz.
f.gs = phase_gap_cycles - PROTOCOL.PHASE_GAP_BASE_CYCLES;
idle.gs = min(f.gs);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Period

if ~all(q.periods >= PROTOCOL.MIN_PERIOD_us)
    error('Period is too small');
end;
if ~all(q.periods <= PROTOCOL.MAX_PERIOD_us)
    error('Period is too large');
end;

f.ts = round(q.periods * 5);                        % 5 MHz.
idle.ts = min(f.ts);

