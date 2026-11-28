import {useContext, useLayoutEffect, useRef, useSyncExternalStore} from 'react';
import {UNSAFE_ViewTransitionContext} from 'react-router';
import {animateSurface, animateUiFrames} from '../ui/surface-motion';

const compactQuery = '(max-width: 780px)';
export function isCompactNavigation() {
  return typeof window !== 'undefined' && (window.matchMedia?.(compactQuery)?.matches ?? false);
}
function subscribeCompactNavigation(change: () => void) {
  const media = window.matchMedia?.(compactQuery);
  media?.addEventListener?.('change', change);
  return () => media?.removeEventListener?.('change', change);
}
export function useCompactNavigation() {
  return useSyncExternalStore(subscribeCompactNavigation, isCompactNavigation, () => false);
}

interface NavigationMotionOptions {
  reduced?: boolean;
  contentSelector?: string;
  railSelector?: string;
  direction?: 'forward' | 'back';
}

/** Animate the changing regions while the shared chrome keeps its layout. */
export function useNavigationPageMotion(page: string | null, {
  reduced = false, contentSelector, railSelector, direction,
}: NavigationMotionOptions = {}) {
  const {isTransitioning} = useContext(UNSAFE_ViewTransitionContext);
  const surface = useRef<HTMLDivElement>(null), previousPage = useRef(page);
  useLayoutEffect(() => {
    const changed = previousPage.current && page && previousPage.current !== page;
    previousPage.current = page;
    const container = surface.current;
    if (!container || !changed || isTransitioning || reduced) return;
    const content = contentSelector ? container.querySelector<HTMLElement>(contentSelector) : container;
    const rail = railSelector ? container.querySelector<HTMLElement>(railSelector) : null;
    const entrance = content && (direction
      ? animateSurface(content, 'enter', {kind: 'inline', direction})
      : animateUiFrames(content, {opacity: [contentSelector ? '.6' : '.78', '1']}, {kind: 'content'}));
    const railEntrance = rail && animateUiFrames(rail, {opacity: ['.55', '1']}, {kind: 'content'});
    return () => {entrance?.cancel(); railEntrance?.cancel();};
  }, [contentSelector, direction, isTransitioning, page, railSelector, reduced]);
  return surface;
}
