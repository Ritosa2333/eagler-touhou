import {useLayoutEffect, useRef} from 'react';

const FAVORITE_REORDER_EVENT = 'lobby-favorite-reorder';
const running = new WeakMap<HTMLElement, Animation>();

export function beginFavoriteReorder() {
  window.dispatchEvent(new Event(FAVORITE_REORDER_EVENT));
}

/** Animate favorite-driven moves without disturbing normal room arrivals or exits. */
export function useFavoriteReorder(selector: string, favorites: readonly string[]) {
  const positions = useRef<Map<HTMLElement, DOMRect> | null>(null);
  const signature = JSON.stringify([...favorites].sort());
  useLayoutEffect(() => {
    const capture = () => {
      positions.current = new Map(Array.from(document.querySelectorAll<HTMLElement>(selector))
        .filter(element => element.getClientRects().length > 0)
        .map(element => [element, element.getBoundingClientRect()]));
    };
    window.addEventListener(FAVORITE_REORDER_EVENT, capture);
    return () => window.removeEventListener(FAVORITE_REORDER_EVENT, capture);
  }, [selector]);
  useLayoutEffect(() => {
    const previous = positions.current;
    positions.current = null;
    if (!previous) return;
    for (const [element, before] of previous) {
      if (!element.isConnected || !element.getClientRects().length) continue;
      running.get(element)?.cancel();
      const after = element.getBoundingClientRect();
      const x = before.left - after.left, y = before.top - after.top;
      if ((!x && !y) || element.closest('.less-motion') || window.matchMedia?.('(prefers-reduced-motion: reduce)').matches) continue;
      const animation = element.animate?.([{translate: `${x}px ${y}px`}, {translate: '0px 0px'}],
        {duration: 500, easing: 'cubic-bezier(.22, .7, .2, 1)'});
      if (animation) running.set(element, animation);
    }
  }, [signature]);
}
