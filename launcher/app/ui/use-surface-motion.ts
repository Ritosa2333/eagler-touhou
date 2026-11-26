import {useLayoutEffect, useRef, type RefObject} from 'react';
import {animateSurface, type SurfaceAnimation, type SurfaceKind, type SurfaceMotionOptions, type SurfacePhase} from './surface-motion';

/** React has committed the new content. Keep the destination interactive during its entrance. */
export function useContentMotion<T extends HTMLElement>(view: string | null) {
  const ref = useRef<T>(null);
  const previous = useRef(view);
  useLayoutEffect(() => {
    const before = previous.current;
    previous.current = view;
    if (before === null || view === null || before === view || !ref.current) return;
    const animation = animateSurface(ref.current, 'enter', {kind:'content'});
    return () => animation?.cancel();
  }, [view]);
  return ref;
}

/** Retained surfaces animate state changes; unrelated renders do not restart them. */
export function useSurfaceMotion(ref: RefObject<HTMLElement | null>, phase: SurfacePhase | null, {
  kind = 'panel', reduced = false, direction, onComplete,
}: {kind?: SurfaceKind; reduced?: boolean; direction?: SurfaceMotionOptions['direction']; onComplete?(): void} = {}) {
  const state = useRef<{node: HTMLElement; phase: SurfacePhase; animation: SurfaceAnimation | null} | null>(null);
  const completed = useRef(onComplete); completed.current = onComplete;
  useLayoutEffect(() => () => {state.current?.animation?.cancel(); state.current = null;}, []);
  useLayoutEffect(() => {
    const previous = state.current, node = ref.current;
    if (!node || !phase) {
      previous?.animation?.cancel(); state.current = null;
      if (phase === 'exit') completed.current?.();
      return;
    }
    if (previous?.node === node && previous.phase === phase) {
      if (reduced) previous.animation?.complete();
      return;
    }
    if (previous?.node !== node) previous?.animation?.cancel();
    const animation = animateSurface(node, phase, {kind, reduced, direction, fromCurrent: true});
    const current = {node, phase, animation}; state.current = current;
    const finish = () => {if (state.current === current) completed.current?.();};
    if (animation) void animation.finished.then(finish); else finish();
  });
}
