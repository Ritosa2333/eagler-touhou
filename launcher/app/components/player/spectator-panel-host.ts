import {retainSpectatorTimelineStyles} from '../../shared/spectator-timeline-view.mjs';

/** Keep the Runtime's real controls/listeners. The Host owns their presentation
 * so an older Runtime's unlayered CSS cannot override the current room layout. */
export function mountSpectatorPanel(frame: HTMLIFrameElement, host: HTMLElement,
  changed: (score: HTMLElement | null) => void) {
  let observer: MutationObserver | null = null;
  let nativeBody: HTMLElement | null = null;
  let panel: HTMLElement | null = null;
  const releaseStyles = retainSpectatorTimelineStyles(host.ownerDocument);
  function find() {
    const next = host.querySelector<HTMLElement>('#spectatorTimeline') ??
      nativeBody?.querySelector<HTMLElement>('#spectatorTimeline') ?? null;
    if (next && next !== panel) {
      panel = next;
      host.append(next);
    }
    changed(next?.querySelector<HTMLElement>('#spectatorScore') ?? null);
  }
  function bind() {
    observer?.disconnect();
    if (panel?.parentElement === host) panel.remove();
    panel = null;
    try { nativeBody = frame.contentDocument?.body ?? null; } catch { nativeBody = null; }
    observer = new MutationObserver(find);
    if (nativeBody) observer.observe(nativeBody, {childList: true});
    observer.observe(host, {childList: true});
    find();
  }
  frame.addEventListener('load', bind);
  bind();
  return () => {
    observer?.disconnect();
    frame.removeEventListener('load', bind);
    // React can retire the Host before the still-live Runtime closes itself.
    if (panel?.parentElement === host && nativeBody?.isConnected) nativeBody.append(panel);
    releaseStyles();
  };
}
