import {useLayoutEffect} from 'react';

/** Use full width and cap height at the image's natural height. */
export function useWorkspaceViewport(stage: HTMLDivElement | null, enabled: boolean) {
  useLayoutEffect(() => {
    if (!stage || !enabled) return;
    const viewport = stage.querySelector<HTMLElement>('.game-viewport');
    if (!viewport) return;
    let frame = 0;
    const measure = () => {
      const style = getComputedStyle(stage);
      const width = stage.clientWidth - parseFloat(style.paddingLeft) - parseFloat(style.paddingRight);
      // Fullscreen/history changes can temporarily leave the stage without a
      // layout box. Keep the last usable size until the room is laid out again.
      if (!Number.isFinite(width) || width <= 0) return;
      const stacked = stage.parentElement && getComputedStyle(stage.parentElement).display === 'flex';
      viewport.toggleAttribute('data-workspace-stacked', !!stacked);
      viewport.style.setProperty('--workspace-game-height', `${Math.max(0, width) * 3 / 4}px`);
    };
    const schedule = () => {cancelAnimationFrame(frame); frame = requestAnimationFrame(measure);};
    const resize = typeof ResizeObserver === 'undefined' ? null : new ResizeObserver(schedule);
    resize?.observe(stage);
    measure();
    schedule();
    window.addEventListener('resize', schedule);
    window.visualViewport?.addEventListener('resize', schedule);
    document.addEventListener('fullscreenchange', schedule);
    document.addEventListener('webkitfullscreenchange', schedule);
    return () => {
      cancelAnimationFrame(frame); resize?.disconnect();
      window.removeEventListener('resize', schedule);
      window.visualViewport?.removeEventListener('resize', schedule);
      document.removeEventListener('fullscreenchange', schedule);
      document.removeEventListener('webkitfullscreenchange', schedule);
      viewport.removeAttribute('data-workspace-stacked'); viewport.style.removeProperty('--workspace-game-height');
    };
  }, [stage, enabled]);
}
