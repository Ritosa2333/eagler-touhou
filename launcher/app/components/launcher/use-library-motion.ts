import {useCallback, useEffect, useRef, type RefObject} from 'react';
import {flushSync} from 'react-dom';
import {microArrivalFrames, microLayoutTiming, microReorderFrames, prefersStillMotion} from '../../ui/micro-motion';

/** Animate deliberate changes to the book library and its visible rows. */
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

  const run = (change: () => void, {nextIds, reorder = false}: {
    nextIds?: ReadonlySet<string>; reorder?: boolean;
  } = {}) => {
    const element = grid.current;
    if (!element || typeof element.animate !== 'function' || prefersStillMotion(element)) {
      cancel();
      change();
      return;
    }
    const cards = Array.from(element.querySelectorAll<HTMLElement>(':scope > .book-card'));
    const positions = new Map(cards.map(card => [card.dataset.bookCard!, card.getBoundingClientRect()]));
    const rows = reorder ? element.closest('.book-library')?.querySelectorAll<HTMLElement>(
      '.book-category-fold[data-open="true"] .book-category-games > li',
    ) : undefined;
    const rowPositions = Array.from(rows ?? [], row => ({row, before: row.getBoundingClientRect()}));
    cancel();
    const generation = serial.current;
    const leaving = reorder ? [] : nextIds ? cards.filter(card => !nextIds.has(card.dataset.bookCard!)) : [element];
    if (nextIds && !cards.length) {
      const empty = element.querySelector<HTMLElement>(':scope > .book-empty');
      if (empty) leaving.push(empty);
    }
    const animate = (node: HTMLElement, frames: Keyframe[], duration = microLayoutTiming.duration, hold = false) => {
      const animation = node.animate(frames, {...microLayoutTiming, duration, fill: 'both'});
      active.current.push(animation);
      if (!hold) void animation.finished.then(() => {
        animation.cancel(); active.current = active.current.filter(item => item !== animation);
      }).catch(() => {});
      return animation;
    };
    const move = (node: HTMLElement, before: DOMRect) => {
      const after = node.getBoundingClientRect(), x = before.left - after.left, y = before.top - after.top;
      if (x || y) animate(node, microReorderFrames(x, y));
    };
    const finish = () => {
      if (generation !== serial.current || !grid.current) return;
      flushSync(change);
      active.current.forEach(animation => {animation.cancel();}); active.current = [];
      inert.current.forEach(node => {node.removeAttribute('inert');}); inert.current = [];
      if (!nextIds && !reorder) {
        animate(element, [{opacity: .82}, {opacity: 1}]);
        return;
      }
      const nextCards = Array.from(element.querySelectorAll<HTMLElement>(':scope > .book-card'));
      for (const card of nextCards) {
        const before = positions.get(card.dataset.bookCard!);
        if (!before) {animate(card, microArrivalFrames); continue;}
        move(card, before);
      }
      for (const {row, before} of rowPositions) move(row, before);
      const empty = !reorder && element.querySelector<HTMLElement>(':scope > .book-empty');
      if (empty) animate(empty, microArrivalFrames);
    };
    if (!leaving.length) {finish(); return;}
    inert.current = leaving; leaving.forEach(node => {node.setAttribute('inert', '');});
    const exits = leaving.map(node => animate(node, [{opacity: 1}, {opacity: nextIds ? 0 : .82}], 110, true));
    void Promise.all(exits.map(animation => animation.finished.catch(() => {}))).then(finish);
  };
  return {run, cancel};
}
