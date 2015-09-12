function out = show_p(p,fname)
out=[];
item = fieldnames(p);
structfieldID=[];
for i=1:length(item)
    if iscell(p.(item{i})) 
%         if length(p.(item{i}))==1
            out = [out sprintf('%s = <cell>\n',item{i})];
%         else iscellstr(p.(item{i}))
%             out = [out sprintf('%s =\n',item{i})];
%             len = length(p.(item{i}));
%             for k=1:len
%                 out = [out sprintf('''%s'' ',p.(item{i}){k})];
%             end
%             out = [out sprintf('\n')];
%         end
    elseif ischar(p.(item{i}))
        out = [out sprintf('%s =\t\t''%s''\n',item{i},p.(item{i}))];
    elseif isstruct(p.(item{i}))
        structfieldID = [structfieldID i];
    elseif isscalar(p.(item{i})) 
        out = [out sprintf('%s =\t\t%7.1f\n',item{i},p.(item{i}))];
    elseif isfloat(p.(item{i}))
        out = [out sprintf('%s =\n',item{i})];
        len = length(p.(item{i}));
        for k=1:min(len,11)
            out = [out sprintf('%7.1f ',p.(item{i})(k))];
        end
        if len>10
            out = [out sprintf('....\n')];
        else
            out = [out sprintf('\n')];
        end
     end
end

for i=1:length(structfieldID)        
    out = [out sprintf('(%s)====================\n',item{structfieldID(i)})];
    str = sprintf('show_p(p.(item{%d}),''temp.out'');',structfieldID(i));
    eval(str)
    fd2=fopen('temp.out','rt');
    out = [out fread(fd2)' sprintf('====================(%s)\n',item{structfieldID(i)})];
    fclose(fd2);
end

        
if nargin>1
    fd=fopen(fname,'wt');
    fprintf(fd,out);
    fclose(fd);
end