import {useLayoutEffect, useState, type ReactNode} from 'react';
import {createPortal} from 'react-dom';
import './fullscreen-transient.css';
/** Main's transient-overlay host rule, with a stable React portal target so
 * native dialog state/focus survives fullscreen changes without remounting. */
export function FullscreenTransient({children, anchor}: {children: ReactNode; anchor?: HTMLElement | null}) {
  const [host] = useState(() => {
    if (typeof document === 'undefined') return null;
    const element = document.createElement('div'); element.dataset.launcherTransientHost = ''; element.dataset.launcherDocument = ''; return element;
  });
  useLayoutEffect(() => {
    if (!host) return;
    const sync = () => {
      const fullscreen = document.fullscreenElement || (document as Document & {webkitFullscreenElement?: Element}).webkitFullscreenElement;
      const player = document.getElementById('player');
      const parent = anchor?.closest<HTMLDialogElement>('dialog[open]') ??
        (player && fullscreen instanceof HTMLElement && player.contains(fullscreen) ? fullscreen : document.body);
      if (host.parentNode !== parent) parent.append(host);
    };
    sync(); document.addEventListener('fullscreenchange', sync); document.addEventListener('webkitfullscreenchange', sync);
    return () => {document.removeEventListener('fullscreenchange', sync); document.removeEventListener('webkitfullscreenchange', sync); host.remove();};
  }, [host, anchor]);
  return host ? createPortal(children, host) : null;
}
