function printin(fname,x)
fd=fopen(fname,'at');
fprintf(fd,'In=============================\n');
fprintf(fd,'%d %d\n',p.channel_stim_rate, p.reference_SPL_dB);
for i=1:length(x)
    fprintf(fd,'%6.3f ',x(i));
end
fprintf(fd,'\n');
fclose(fd)

