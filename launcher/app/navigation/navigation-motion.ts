import {useLayoutEffect, useRef, useSyncExternalStore} from 'react';
import {animate} from 'motion/mini';

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

// GSAP power3.out, as used in the navigation motion preview.
const easeOutQuart = (progress: number) => 1 - (1 - progress) ** 4;
function motionAllowed(reduced: boolean) {
  return !reduced && !window.matchMedia?.('(prefers-reduced-motion: reduce)')?.matches;
}

/** Animate live mobile content after navigation commits. */
export function useNavigationPageMotion(pathname: string, reduced: boolean) {
  const compact = useCompactNavigation();
  const surface = useRef<HTMLDivElement>(null), previousPath = useRef(pathname);
  useLayoutEffect(() => {
    const changed = previousPath.current !== pathname;
    previousPath.current = pathname;
    const node = surface.current;
    if (!changed || !compact || !node?.animate || !motionAllowed(reduced)) return;
    const animation = animate(node, {opacity:[0, 1]}, {duration:.42, ease:easeOutQuart});
    return () => {animation.cancel(); node.style.removeProperty('opacity');};
  }, [compact, pathname, reduced]);
  return surface;
}
