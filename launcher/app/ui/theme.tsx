import {useLayoutEffect} from 'react';

/** One appearance source for Radix colors, portals, and the document shell. */
export function UiTheme() {
  useLayoutEffect(() => {
    const root = document.documentElement;
    const sync = () => {
      const dark = root.dataset.bookTheme !== 'light';
      root.classList.toggle('dark', dark);
      root.classList.toggle('light', !dark);
    };
    sync();
    const observer = new MutationObserver(sync);
    observer.observe(root, {attributes: true, attributeFilter: ['data-book-theme']});
    return () => observer.disconnect();
  }, []);
  return null;
}
