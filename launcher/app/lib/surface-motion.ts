import { animate } from 'motion/mini';
import { type RefObject, useLayoutEffect } from 'react';

/** Read the site's tokens at the surface, including inherited theme overrides. */
export function surfaceMotionTokens(element: HTMLElement, kind: 'menu' | 'panel') {
  const style = element.ownerDocument.defaultView!.getComputedStyle(element);
  const duration = style.getPropertyValue('--motion-duration').trim();
  const distance = parseFloat(style.getPropertyValue(`--motion-distance-${kind}`));
  const curve = style
    .getPropertyValue('--motion-ease')
    .match(/[\d.]+/g)
    ?.map(Number);
  return {
    duration: duration ? parseFloat(duration) / (duration.endsWith('ms') ? 1000 : 1) : 0.18,
    distance: Number.isFinite(distance) ? distance : kind === 'menu' ? 8 : 12,
    ease: curve?.length === 4 ? (curve as [number, number, number, number]) : ([0.22, 0.8, 0.22, 1] as const),
  };
}

/** Presentation only: callers keep their native focus, presence and close owner.
 * Mini uses WAAPI, so these effects do not depend on React's render/frame loop.
 */
export function useSurfaceMotion(ref: RefObject<HTMLElement | null>, open: boolean, kind: 'menu' | 'panel') {
  useLayoutEffect(() => {
    const element = ref.current,
      view = element?.ownerDocument.defaultView;
    if (!element || !view || !open || typeof element.animate !== 'function') return;
    const media = view.matchMedia('(prefers-reduced-motion: reduce)');
    if (media.matches || element.closest('.less-motion')) return;
    const tokens = surfaceMotionTokens(element, kind);
    if (tokens.duration <= 0) return;
    const previous = { opacity: element.style.opacity, transform: element.style.transform };
    element.dataset.motionSurface = kind;
    element.dataset.motionEnter = kind;
    // Read after retiring the CSS entrance, preserving centering transforms
    // such as translate(-50%, -50%) on the native fullscreen utility windows.
    const transform = view.getComputedStyle(element).transform || 'none';
    const base = transform === 'none' ? '' : ` ${transform}`;
    const controls = animate(
      element,
      {
        opacity: [0, 1],
        transform: [`translateY(${kind === 'menu' ? -tokens.distance : tokens.distance}px)${base}`, transform],
      },
      { duration: tokens.duration, ease: [...tokens.ease] },
    );
    let disposed = false;
    const restore = () => {
      if (disposed) return;
      disposed = true;
      controls.stop();
      element.style.opacity = previous.opacity;
      element.style.transform = previous.transform;
      delete element.dataset.motionEnter;
      media.removeEventListener('change', reduced);
      observer.disconnect();
    };
    const reduced = () => {
      if (media.matches || element.closest('.less-motion')) restore();
    };
    const observer = new view.MutationObserver(reduced);
    observer.observe(element.ownerDocument.body, { attributes: true, attributeFilter: ['class'] });
    media.addEventListener('change', reduced);
    controls.then(restore, restore);
    return restore;
  }, [ref, open, kind]);
}
