import {useEffect} from 'react';

/** Native scrolling stays in the browser; only the thumb visibility changes. */
export function UiScrollbars() {
  useEffect(() => {
    const active = new Map<Element, ReturnType<typeof setTimeout>>();
    const onScroll = (event: Event) => {
      const element = event.target === document ? document.documentElement : event.target;
      if (!(element instanceof Element)) return;
      const previous = active.get(element);
      if (previous !== undefined) clearTimeout(previous);
      else element.setAttribute('data-ui-scrolling', '');
      active.set(element, setTimeout(() => {
        element.removeAttribute('data-ui-scrolling');
        active.delete(element);
      }, 800));
    };
    document.addEventListener('scroll', onScroll, {capture: true, passive: true});
    return () => {
      document.removeEventListener('scroll', onScroll, true);
      for (const [element, timer] of active) {
        clearTimeout(timer);
        element.removeAttribute('data-ui-scrolling');
      }
    };
  }, []);
  return null;
}
