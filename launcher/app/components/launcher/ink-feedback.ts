/** A brief impression on the icon, without delaying the actual selection. */
export function impressFavorite(button: HTMLElement) {
  if (window.matchMedia('(prefers-reduced-motion: reduce)').matches || button.closest('.less-motion')) return;
  const icon = button.querySelector('svg');
  if (!icon) return;
  icon.getAnimations().forEach(animation => {animation.cancel();});
  icon.animate([
    {transform: 'scale(1) rotate(0deg)', offset: 0},
    {transform: 'scale(.75) rotate(-12deg)', offset: .22},
    {transform: 'scale(1.35) rotate(12deg)', offset: .58},
    {transform: 'scale(.95) rotate(-3deg)', offset: .82},
    {transform: 'scale(1) rotate(0deg)', offset: 1},
  ], {duration: 480, easing: 'cubic-bezier(.22, 1, .36, 1)'});
}

/** Animate the new theme icon after React updates it. */
export function impressTheme(button: HTMLElement) {
  requestAnimationFrame(() => {
    if (!button.isConnected || window.matchMedia('(prefers-reduced-motion: reduce)').matches || button.closest('.less-motion')) return;
    const icon = button.querySelector('svg');
    if (!icon) return;
    icon.getAnimations().forEach(animation => animation.cancel());
    icon.animate([
      {opacity: .35, transform: 'rotate(-75deg) scale(.6)', offset: 0},
      {opacity: 1, transform: 'rotate(12deg) scale(1.18)', offset: .7},
      {opacity: 1, transform: 'rotate(0deg) scale(1)', offset: 1},
    ], {duration: 420, easing: 'cubic-bezier(.22, 1, .36, 1)'});
  });
}
