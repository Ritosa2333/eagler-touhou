import {useLayoutEffect, useRef, type RefObject} from 'react';
import {animateSurface, type SurfaceAnimation, type SurfaceKind, type SurfacePhase} from './surface-motion';

/** Retained surfaces animate state changes; unrelated renders do not restart them. */
export function useSurfaceMotion(ref: RefObject<HTMLElement | null>, phase: SurfacePhase | null, {
  kind = 'panel', reduced = false, onComplete,
}: {kind?: SurfaceKind; reduced?: boolean; onComplete?(): void} = {}) {
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
    const animation = animateSurface(node, phase, {kind, reduced, fromCurrent: true});
    const current = {node, phase, animation}; state.current = current;
    const finish = () => {if (state.current === current) completed.current?.();};
    if (animation) void animation.finished.then(finish); else finish();
  });
}
