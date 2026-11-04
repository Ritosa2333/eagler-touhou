/** A brief impression on the icon, without delaying the actual selection. */
export function impressFavorite(button: HTMLElement) {
  if (window.matchMedia('(prefers-reduced-motion: reduce)').matches || button.closest('.less-motion')) return;
  const icon = button.querySelector('svg');
  if (!icon) return;
  icon.getAnimations().forEach(animation => {animation.cancel();});
  icon.animate([
    {transform: 'scale(1) rotate(0deg)', offset: 0},
    {transform: 'scale(.92) rotate(-5deg)', offset: .2},
    {transform: 'scale(1.17) rotate(5deg)', offset: .55},
    {transform: 'scale(.99) rotate(-1deg)', offset: .82},
    {transform: 'scale(1) rotate(0deg)', offset: 1},
  ], {duration: 680, easing: 'ease-in-out'});
}

/** Animate the new theme icon after React updates it. */
export function impressTheme(button: HTMLElement) {
  requestAnimationFrame(() => {
    if (!button.isConnected || window.matchMedia('(prefers-reduced-motion: reduce)').matches || button.closest('.less-motion')) return;
    const icon = button.querySelector('svg');
    if (!icon) return;
    icon.getAnimations().forEach(animation => {animation.cancel();});
    icon.animate([
      {opacity: .35, transform: 'rotate(-40deg) scale(.8)', offset: 0},
      {opacity: 1, transform: 'rotate(5deg) scale(1.08)', offset: .72},
      {opacity: 1, transform: 'rotate(0deg) scale(1)', offset: 1},
    ], {duration: 600, easing: 'ease-in-out'});
  });
}
