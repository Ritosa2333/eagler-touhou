const STYLE_ID = 'eagler-pixel-perfect-display';

/** Final browser scaling only. A stylesheet also covers canvases created later. */
export function setPixelPerfectDisplay(document: Document, enabled: boolean) {
  const existing = document.getElementById(STYLE_ID);
  if (!enabled) { existing?.remove(); return; }
  if (existing) return;
  const style = document.createElement('style');
  style.id = STYLE_ID;
  style.textContent = 'canvas { image-rendering: pixelated !important; }';
  (document.head || document.documentElement).append(style);
}

/** Each navigation reads the captured game's preference, so iframe reuse cannot
 * carry an enabled setting into another title. No Runtime protocol change. */
export function bindPixelPerfectDisplay(frame: HTMLIFrameElement, enabled: () => boolean) {
  const refresh = () => {
    const document = frame.contentDocument;
    if (document) setPixelPerfectDisplay(document, enabled());
  };
  frame.addEventListener('load', refresh);
  return () => {
    frame.removeEventListener('load', refresh);
    if (frame.contentDocument) setPixelPerfectDisplay(frame.contentDocument, false);
  };
}
