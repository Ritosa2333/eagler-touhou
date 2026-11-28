import {useCallback, useEffect, useRef, type RefObject} from 'react';
import {flushSync} from 'react-dom';

/** Animate only deliberate view/category changes; the caller still owns selection. */
export function useLibraryMotion(grid: RefObject<HTMLDivElement | null>) {
  const serial = useRef(0);
  const active = useRef<Animation[]>([]);
  const inert = useRef<HTMLElement[]>([]);
  const cancel = useCallback(() => {
    ++serial.current;
    active.current.forEach(animation => {animation.cancel();});
    active.current = [];
    inert.current.forEach(element => {element.removeAttribute('inert');});
    inert.current = [];
  }, []);
  useEffect(() => () => cancel(), [cancel]);

  const run = (change: () => void, nextIds?: ReadonlySet<string>) => {
    cancel();
    const generation = serial.current, element = grid.current;
    if (!element || typeof element.animate !== 'function' || element.closest('.less-motion') || window.matchMedia?.('(prefers-reduced-motion: reduce)').matches) {change(); return;}
    const cards = Array.from(element.querySelectorAll<HTMLElement>(':scope > .book-card'));
    const positions = new Map(cards.map(card => [card.dataset.bookCard!, card.getBoundingClientRect()]));
    const leaving = nextIds ? cards.filter(card => !nextIds.has(card.dataset.bookCard!)) : [element];
    if (nextIds && !cards.length) {
      const empty = element.querySelector<HTMLElement>(':scope > .book-empty');
      if (empty) leaving.push(empty);
    }
    const animate = (node: HTMLElement, frames: Keyframe[], duration: number, hold = false) => {
      const animation = node.animate(frames, {duration, easing: 'cubic-bezier(.22,.7,.2,1)', fill: 'both'});
      active.current.push(animation);
      if (!hold) void animation.finished.then(() => {
        animation.cancel(); active.current = active.current.filter(item => item !== animation);
      }).catch(() => {});
      return animation;
    };
    const finish = () => {
      if (generation !== serial.current || !grid.current) return;
      flushSync(change);
      active.current.forEach(animation => {animation.cancel();}); active.current = [];
      inert.current.forEach(node => {node.removeAttribute('inert');}); inert.current = [];
      if (!nextIds) {
        animate(element, [{opacity: 0}, {opacity: 1}], 240);
        return;
      }
      const nextCards = Array.from(element.querySelectorAll<HTMLElement>(':scope > .book-card'));
      for (const card of nextCards) {
        const before = positions.get(card.dataset.bookCard!);
        if (!before) {animate(card, [{opacity: 0, translate: '0 10px'}, {opacity: 1, translate: '0 0'}], 440); continue;}
        const after = card.getBoundingClientRect(), x = before.left - after.left, y = before.top - after.top;
        if (x || y) animate(card, [{translate: `${x}px ${y}px`}, {translate: '0 0'}], 440);
      }
      const empty = element.querySelector<HTMLElement>(':scope > .book-empty');
      if (empty) animate(empty, [{opacity: 0, translate: '0 8px'}, {opacity: 1, translate: '0 0'}], 440);
    };
    if (!leaving.length) {finish(); return;}
    inert.current = leaving; leaving.forEach(node => {node.setAttribute('inert', '');});
    const exits = leaving.map(node => animate(node, [{opacity: 1}, {opacity: 0}], 210, true));
    void Promise.all(exits.map(animation => animation.finished.catch(() => {}))).then(finish);
  };
  return {run, cancel};
}
