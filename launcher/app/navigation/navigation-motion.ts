import {useLayoutEffect, useRef, useSyncExternalStore} from 'react';
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

interface PageEntranceOptions {
  reduced?: boolean;
  contentSelector?: string;
}

const pageEntranceTiming = {duration: 140, ease: [.2, 0, 0, 1] as [number, number, number, number]};

/** Reveal live content after a route commits. Never snapshot or delay a route. */
export function usePageEntrance<T extends HTMLElement = HTMLDivElement>(page: string, {
  reduced, contentSelector,
}: PageEntranceOptions = {}) {
  const surface = useRef<T>(null), previousPage = useRef(page);
  useLayoutEffect(() => {
    const changed = previousPage.current !== page;
    previousPage.current = page;
    const container = surface.current;
    if (!changed || !container || reduced) return;
    let entrance: ReturnType<typeof animateUiFrames>;
    const start = () => {
      const content = contentSelector ? container.querySelector<HTMLElement>(contentSelector) : container;
      if (!content) return false;
      entrance = animateUiFrames(content, {opacity: ['.85', '1']}, {reduced}, pageEntranceTiming);
      return true;
    };
    if (start()) return () => entrance?.cancel();
    // A lazy route can commit its shell before mounting the content.
    const ready = new MutationObserver(() => {if (start()) ready.disconnect();});
    ready.observe(container, {childList: true, subtree: true});
    return () => {ready.disconnect(); entrance?.cancel();};
  }, [contentSelector, page, reduced]);
  return surface;
}

interface NavigationMotionOptions {
  reduced?: boolean;
  contentSelector?: string;
  direction?: 'forward' | 'back';
}

/** Local view changes within a page, such as details or tabs. */
export function useNavigationPageMotion(page: string | null, {
  reduced = false, contentSelector, direction,
}: NavigationMotionOptions = {}) {
  const surface = useRef<HTMLDivElement>(null), previousPage = useRef(page);
  useLayoutEffect(() => {
    const changed = previousPage.current && page && previousPage.current !== page;
    previousPage.current = page;
    const container = surface.current;
    if (!container || !changed || reduced) return;
    const content = contentSelector ? container.querySelector<HTMLElement>(contentSelector) : container;
    const entrance = content && (direction
      ? animateSurface(content, 'enter', {kind: 'inline', direction})
      : animateUiFrames(content, {opacity: [contentSelector ? '.6' : '.78', '1']}, {kind: 'content'}));
    return () => entrance?.cancel();
  }, [contentSelector, direction, page, reduced]);
  return surface;
}
