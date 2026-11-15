/** Replay view blocks live canvas input except a current TH14 practice modal.
 * Bind to the actual iframe document; navigation and disposal retire ownership. */
export function bindReplayPracticePointer(frame: HTMLIFrameElement) {
  let unbind: (() => void) | null = null;
  const clear = () => frame.removeAttribute('data-th14-practice-panel');
  function bind() {
    unbind?.(); unbind = null; clear();
    const doc = frame.contentDocument, target = doc?.defaultView;
    if (!doc || !target || !/^\/runtime\/th14\//.test(new URL(doc.URL, frame.ownerDocument.baseURI).pathname)) return;
    const handler: EventListener = event => {
      if (frame.contentDocument !== doc || doc.defaultView !== target) return;
      const detail = (event as CustomEvent<{open?: unknown}>).detail;
      if (typeof detail?.open !== 'boolean') return;
      if (detail.open) frame.setAttribute('data-th14-practice-panel', 'open');
      else clear();
    };
    target.addEventListener('eagler-thprac-menu', handler);
    unbind = () => target.removeEventListener('eagler-thprac-menu', handler);
  }
  frame.addEventListener('load', bind); bind();
  return () => {frame.removeEventListener('load', bind); unbind?.(); clear();};
}
