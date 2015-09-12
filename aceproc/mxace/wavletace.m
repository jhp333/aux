function [out,p]=wavletace(in, p, outputfilename)

[out,p]=wavletace0(in, p);

if nargin>2
    [pathstr, name, ext] = fileparts(outputfilename);
    switch ext
        case '.quf'
            Save_sequence_(out, outputfilename);
        case '.xml'
            newq=Sequence(out);
            write(newq, seq_xml);
        otherwise
            error('Unknown output sequence file format.')
    end
end