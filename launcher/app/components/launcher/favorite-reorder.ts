import {useLayoutEffect, useRef} from 'react';

const FAVORITE_REORDER_EVENT = 'lobby-favorite-reorder';
const FILTER_REORDER_EVENT = 'lobby-filter-reorder';
const running = new WeakMap<HTMLElement, Animation>();

export function beginFavoriteReorder() {
  window.dispatchEvent(new Event(FAVORITE_REORDER_EVENT));
}

export function beginFilterReorder() {
  window.dispatchEvent(new Event(FILTER_REORDER_EVENT));
}

/** Animate deliberate list changes without disturbing normal room arrivals or exits. */
export function useFavoriteReorder(selector: string, favorites: readonly string[], filterSignature?: string) {
  const positions = useRef<{items: Map<HTMLElement, DOMRect>; filter: boolean} | null>(null);
  const signature = JSON.stringify([[...favorites].sort(), filterSignature]);
  useLayoutEffect(() => {
    const capture = (filter: boolean) => {
      positions.current = {filter, items: new Map(Array.from(document.querySelectorAll<HTMLElement>(selector))
        .filter(element => element.getClientRects().length > 0)
        .map(element => [element, element.getBoundingClientRect()]))};
    };
    const favorite = () => capture(false);
    const filter = () => capture(true);
    window.addEventListener(FAVORITE_REORDER_EVENT, favorite);
    if (filterSignature !== undefined) window.addEventListener(FILTER_REORDER_EVENT, filter);
    return () => {window.removeEventListener(FAVORITE_REORDER_EVENT, favorite); window.removeEventListener(FILTER_REORDER_EVENT, filter);};
  }, [selector, filterSignature !== undefined]);
  useLayoutEffect(() => {
    const previous = positions.current;
    positions.current = null;
    if (!previous) return;
    for (const [element, before] of previous.items) {
      if (!element.isConnected || !element.getClientRects().length) continue;
      running.get(element)?.cancel();
      const after = element.getBoundingClientRect();
      const x = before.left - after.left, y = before.top - after.top;
      if ((!x && !y) || element.closest('.less-motion') || window.matchMedia?.('(prefers-reduced-motion: reduce)').matches) continue;
      const animation = element.animate?.([{translate: `${x}px ${y}px`}, {translate: '0px 0px'}],
        {duration: 500, easing: 'cubic-bezier(.22, .7, .2, 1)'});
      if (animation) running.set(element, animation);
    }
    if (previous.filter) for (const element of document.querySelectorAll<HTMLElement>(selector)) {
      if (previous.items.has(element) || !element.getClientRects().length || element.closest('.less-motion') || window.matchMedia?.('(prefers-reduced-motion: reduce)').matches) continue;
      const animation = element.animate?.([{opacity: 0, translate: '0px 8px'}, {opacity: 1, translate: '0px 0px'}],
        {duration: 500, easing: 'cubic-bezier(.22, .7, .2, 1)'});
      if (animation) running.set(element, animation);
    }
  }, [signature]);
}
