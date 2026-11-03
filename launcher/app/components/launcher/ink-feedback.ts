/** A brief impression on the icon, without delaying the actual selection. */
export function impressFavorite(button: HTMLButtonElement) {
  if (window.matchMedia('(prefers-reduced-motion: reduce)').matches || document.body.classList.contains('less-motion')) return;
  const icon = button.querySelector('svg');
  if (!icon) return;
  icon.getAnimations().forEach(animation => {animation.cancel();});
  icon.animate([{opacity: .45, transform: 'scale(.94)'}, {opacity: .8, transform: 'scale(1.06)', offset: .5}, {opacity: 1, transform: 'scale(1)'}], {duration: parseFloat(getComputedStyle(button).getPropertyValue('--ui-feedback-duration')) || 140, easing: 'cubic-bezier(.2, 0, 0, 1)'});
}
