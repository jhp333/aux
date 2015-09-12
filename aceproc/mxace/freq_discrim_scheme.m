function freq_discrim_scheme
ViewPulseHistogramTone(500)
fig=gcf;
ax2copy = get(gcf,'children');
ax2copy = findobj('type','axes');

figure('pos',[563   422   589   113],'color',[1 1 1])
xw = .47;
pt=[128 256];
ms=[8 16];
ha=8;
for k=1:2
    bars = findobj(ax2copy(3-k),'type','hggroup');
    ax2(k)=axes('pos',[.03+(xw+.02)*(k-1) .3 xw .65],'box','on');
    copyobj(bars,ax2(k));
    set(ax2(k),'xlim',[-2 22.5],'xtick',1:5:21)
    set(ax2(k),'ylim',[-0.05 1.1],'ytick',-10:100:100,'fontsize',13)
    text(ha,.8,[num2str(pt(k)) '-pt FFT'],'horizontalalignment','left','fontsize',13)
    text(ha+.5,.4,['(' num2str(ms(k)) '-ms)'],'horizontalalignment','left','fontsize',13)
    text(-1,-.45,'Channel #:','fontsize',12)
    arrow([11 -.45],[15 -.45],'baseangle',20,'length',10)
end
delete(fig)
