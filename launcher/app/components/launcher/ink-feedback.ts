import {playIconFeedback} from '../../ui/micro-motion';

export function impressFavorite(button: HTMLElement) {
  playIconFeedback(button, 'favorite');
}

/** Animate the new theme icon after React updates it. */
export function impressTheme(button: HTMLElement) {
  requestAnimationFrame(() => {
    if (button.isConnected) playIconFeedback(button, 'theme');
  });
}
