function [bins, cumBins, freqBound]=freqwidth(nBands, freqs)
% function [bins, cumBins]=freqwidth(nBands, freqs)
% This function generates the array of bin count from fft output (62
% elements) for a given nBands and freqs (lower and upper freq boundaries).
% If freqs is not specified, the default is [187.5 7937.5]
% bins: array of bin count (length of nBands)
% cumBins: boundaries of bin indices for each band (length of nBands + 1 ).
%          By default it should begin with 0 and end with 62, and could be 
% different if lower or upper boundaries are given.  
% 
% Developed by BJ Kwon Jul-29-07

global nFullLin

nFullLin = 7;

if nargin<2
    freqs = [187.5 7937.5];
end
freqs = freqs + 62.5;
freqs = round(freqs/125)*125;

[req_nBins pro firstz1]= getnbins(freqs);
ratio = pro(1)/pro(2);
lowport =  ( exp(.6*ratio)-1 ) / (exp(.6*ratio)+2*exp(.6)-3);

dx=1:lowport*nBands;

if ~isempty(dx)
    z1 = nFullLin * pro(1) /nBands/lowport*dx;
    firstz2 = z1(length(z1)) + nFullLin*pro(1)/nBands/lowport;
    z2 = logspace(log(firstz2)/log(10),log(req_nBins)/log(10),nBands-length(dx));
    z=[z1 z2]';
    cumBins=firstz1 + [0; round(z)];
else
    if firstz1==0
        zp=1;
        z = logspace(log(zp)/log(10),log(req_nBins+zp)/log(10),nBands+1)';
        z = z - zp;
    else
        z = logspace(log(firstz1)/log(10),log(req_nBins+firstz1)/log(10),nBands+1)';
    end
    cumBins=round(z);
end
y=diff(cumBins);
bins=sort(y);

freqBound = 187.5 + cumBins * 125;


function [nbin, proportions, firstz1] = getnbins(freqs)
global nFullLin
freqs(1)=max(125,freqs(1));
freqs(2)=min(8000,freqs(2));
% This sequence 125 1125 8000 is going to change to 0 1 2
% The second number is always 1
% the first and last values are determined

proportions=zeros(1,2);
proportions(1) = 1 - (freqs(1)-250)/(1125-250);
proportions(2) = (log(freqs(2))-log(1125))/1.9617; %log(8000)-log(1125)

% if proportions are equal proprt will be 1/3
% if proportion1 is negative, proprt will be 0
% if proportion2 is negative, proprt will be 1
if proportions(1)<0 % freqs(1) is greater than 1125, making proprtions2 less than 1
    proprt = 0;
    proportions(1) = 0;
    proportion2Lin = diff(freqs)/6875;
    firstz1 = round(freqs(1)/125)-2;
elseif proportions(2)<=0
    proprt = 1;
    proportions(2) = 0;
    proportion2Lin = 0;
    firstz1 = round(nFullLin*(1-proportions(1)));
else
    proportion2Lin = (freqs(2)-1125)/6875;
    firstz1 = round(nFullLin*(1-proportions(1)));
end
nbin = min(62,fix(nFullLin * proportions(1) + 55 * proportion2Lin));


        
