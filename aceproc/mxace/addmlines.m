function p=addmlines(p,str)
delim=sprintf(',;\r\n');
[obj remain]=strtok(str,delim);
remain = trimchars(remain, delim);
try
    eval(['p.' obj ';']);
catch ME
    str0 = sprintf('%s What is %s?\n',ME.message,str);
    for i=1:length(ME.stack)
        str0 = sprintf('%s in %s.m line %d\n',str0, ME.stack(i).name,ME.stack(i).line);
    end
    error(str0) 
end
while ~isempty(remain)
    [obj remain]=strtok(remain,delim);
     eval(['p.' obj ';']);
end
