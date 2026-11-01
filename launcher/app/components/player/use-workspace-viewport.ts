import { useLayoutEffect } from 'react';

/** Use full width and cap height at the image's natural height. */
export function useWorkspaceViewport(stage: HTMLDivElement | null, enabled: boolean) {
  useLayoutEffect(() => {
    if (!stage || !enabled || typeof ResizeObserver === 'undefined') return;
    const viewport = stage.querySelector<HTMLElement>('.game-viewport');
    if (!viewport) return;
    let frame = 0;
    const fit = () => {
      const style = getComputedStyle(stage);
      const width = stage.clientWidth - parseFloat(style.paddingLeft) - parseFloat(style.paddingRight);
      const stacked = stage.parentElement && getComputedStyle(stage.parentElement).display === 'flex';
      viewport.toggleAttribute('data-workspace-stacked', !!stacked);
      viewport.style.setProperty('--workspace-game-height', `${(Math.max(0, width) * 3) / 4}px`);
    };
    const schedule = () => {
      cancelAnimationFrame(frame);
      frame = requestAnimationFrame(fit);
    };
    const resize = new ResizeObserver(schedule);
    resize.observe(stage);
    schedule();
    window.addEventListener('resize', schedule);
    return () => {
      cancelAnimationFrame(frame);
      resize.disconnect();
      window.removeEventListener('resize', schedule);
      viewport.removeAttribute('data-workspace-stacked');
      viewport.style.removeProperty('--workspace-game-height');
    };
  }, [stage, enabled]);
}
