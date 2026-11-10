import {useLayoutEffect, type RefObject} from 'react';
import {isCompactNavigation} from '../../navigation/navigation-motion';
const viewportProperties = ['--options-view-width', '--options-view-height', '--options-view-top', '--options-view-left'];
/** Main app.mts8506–8530, scoped to this options node with listener/RAF teardown.
 * Only the options layer follows visualViewport; pinch zoom is left untouched.
 */
export function useOptionsViewport(ref: RefObject<HTMLElement | null>, active: boolean) {
  useLayoutEffect(() => {
    const node = ref.current;
    if (!active || !node) return;
    const update = () => {
      const viewport = window.visualViewport;
      if (viewport && viewport.scale !== 1) return;
      const viewportWidth = viewport?.width || window.innerWidth;
      // The desktop modal reserves the body gutter after locking the root scrollbar.
      const width = isCompactNavigation() ? viewportWidth
        : Math.min(viewportWidth, document.body.getBoundingClientRect().width || document.documentElement.clientWidth);
      const height = viewport?.height || window.innerHeight;
      const values = {'--options-view-width': `${width}px`, '--options-view-height': `${height}px`,
        '--options-view-top': `${viewport?.offsetTop || 0}px`, '--options-view-left': `${viewport?.offsetLeft || 0}px`};
      for (const [name, value] of Object.entries(values)) {
        if (node.style.getPropertyValue(name) !== value) node.style.setProperty(name, value);
      }
    };
    let frame = 0;
    const schedule = () => {if (!frame) frame = requestAnimationFrame(() => {frame = 0; update();});};
    // On phones the CSS viewport units place the first frame. Reading
    // visualViewport dimensions here would force layout before the fade starts.
    if (!isCompactNavigation()) update();
    window.addEventListener('resize', schedule, {passive: true});
    window.visualViewport?.addEventListener('resize', schedule, {passive: true});
    window.visualViewport?.addEventListener('scroll', schedule, {passive: true});
    return () => {
      cancelAnimationFrame(frame);
      window.removeEventListener('resize', schedule);
      window.visualViewport?.removeEventListener('resize', schedule);
      window.visualViewport?.removeEventListener('scroll', schedule);
      for (const property of viewportProperties) node.style.removeProperty(property);
    };
  }, [ref, active]);
}
