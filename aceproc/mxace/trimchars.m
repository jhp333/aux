function out = trimchars(in, trimmed, recurse)
trimmed = [trimmed ' '];
if nargin<3, recurse=0; end
for i=1:length(in)
    if ~any(in(i)==trimmed)
        break;
    end
    if i==length(in)
        i=i+1;
    end
end
out = in(end:-1:i);
if recurse==0
    out = trimchars(out, trimmed, 1);
end
