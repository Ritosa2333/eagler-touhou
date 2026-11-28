import {useLayoutEffect, useRef, useSyncExternalStore} from 'react';
import {LESS_MOTION_STORAGE_KEY} from '../models/site-preferences';
import {animateSurface, animateUiFrames} from '../ui/surface-motion';
import {clearDocumentEntrance, pageEntranceOpacity, pageEntranceTiming} from './page-entrance';

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

function savedLessMotion(view: Window) {
  try {return view.localStorage.getItem(LESS_MOTION_STORAGE_KEY) === '1';} catch {return false;}
}

/** Reveal live content after a route commits. Never snapshot or delay a route. */
export function usePageEntrance<T extends HTMLElement = HTMLDivElement>(page: string, {
  reduced, contentSelector,
}: PageEntranceOptions = {}) {
  const surface = useRef<T>(null), previousPage = useRef(page);
  useLayoutEffect(() => {
    const changed = previousPage.current !== page;
    previousPage.current = page;
    const container = surface.current;
    if (!container) return;
    const doc = container.ownerDocument;
    const view = doc.defaultView!;
    let entrance: ReturnType<typeof animateUiFrames>;
    let ready: MutationObserver | undefined;
    const start = () => {
      const content = contentSelector ? container.querySelector<HTMLElement>(contentSelector) : container;
      if (!content) return false;
      entrance = animateUiFrames(content, {opacity: [pageEntranceOpacity, '1']}, {reduced}, pageEntranceTiming);
      return true;
    };
    const reveal = () => {
      if (reduced) return;
      ready?.disconnect();
      if (start()) return;
      // A lazy route can commit its shell before mounting the content.
      ready = new MutationObserver(() => {if (start()) ready?.disconnect();});
      ready.observe(container, {childList: true, subtree: true});
    };
    if (changed) {
      clearDocumentEntrance(doc);
      reveal();
    }
    const restored = (event: PageTransitionEvent) => {
      if (!event.persisted) return;
      clearDocumentEntrance(doc);
      if (!savedLessMotion(view)) reveal();
    };
    view.addEventListener('pageshow', restored);
    return () => {
      view.removeEventListener('pageshow', restored);
      ready?.disconnect();
      entrance?.cancel();
    };
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
