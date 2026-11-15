import {useLayoutEffect, useState} from 'react';

const sides = ['top', 'right', 'bottom', 'left'] as const;
const base = {top: 12, right: 12, bottom: 12, left: 12};
/** Radix collision boundaries include display cutouts and the system gesture area. */
export function usePopupPadding(open: boolean) {
  const [padding, setPadding] = useState(base);
  useLayoutEffect(() => {
    if (!open) return;
    const measure = () => {
      const style = getComputedStyle(document.documentElement);
      const next = Object.fromEntries(sides.map(side => [side, 12 + Math.max(0, parseFloat(style.getPropertyValue(`--ui-safe-${side}`)) || 0)])) as typeof base;
      setPadding(previous => sides.every(side => previous[side] === next[side]) ? previous : next);
    };
    measure(); window.addEventListener('resize', measure); window.visualViewport?.addEventListener('resize', measure);
    return () => {window.removeEventListener('resize', measure); window.visualViewport?.removeEventListener('resize', measure);};
  }, [open]);
  return padding;
}
