function printout(fname,x,q)
fd=fopen(fname,'at');
fprintf(fd,'Out=============================\n');
% fprintf(fd,'%d %d\n',p.channel_stim_rate, p.reference_SPL_dB);
for i=1:length(x)
    fprintf(fd,'%6.3f ',x(i));
end
fprintf(fd,'\n');
for i=1:length(q.electrodes)
    fprintf(fd,'(%d:%d) ',q.electrodes(i),q.current_levels(i));
end
fprintf(fd,'\n');
fclose(fd);
