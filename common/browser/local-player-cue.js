// Loaded before the multiplayer runtime. The native presentation hooks use
// the same state, so Ctrl also works when focus stays on the Launcher.
(() => {
  if (globalThis.__eaglerLocalPlayerCue) return;
  const cue = globalThis.__eaglerLocalPlayerCue = { held: false, until: 0 };
  const lifetime = 1200;
  const editing = 'input,textarea,select,[contenteditable]:not([contenteditable="false"]),[role="textbox"],dialog,[role="menu"]';
  const chat = '.mp-quick-chat[data-composer-open="true"]:not([hidden])';
  const pressed = new Set();
  const documents = [document];
  const frames = [];
  const windows = [window];
  // Managed games and their Launcher are same-origin. Standalone runtimes
  // and cross-origin embeddings still get native iframe keyboard events.
  for (let child = window; child !== child.parent;) {
    try {
      const parent = child.parent;
      const frame = child.frameElement;
      if (!frame || !parent.document) break;
      frames.push(frame);
      documents.push(parent.document);
      windows.push(parent);
      child = parent;
    } catch { break; }
  }
  const topDocument = documents.at(-1);
  let track = null, renderRequest = 0;
  const hide = () => { if (cue.label) cue.label.hidden = true; };
  const active = () => !document.hidden && (cue.held || performance.now() < cue.until);
  function reset() {
    pressed.clear();
    cue.held = false;
    cue.until = 0;
    hide();
    if (renderRequest) cancelAnimationFrame(renderRequest);
    renderRequest = 0;
  }
  function render() {
    renderRequest = 0;
    if (!track || !active()) { hide(); return; }
    const target = track();
    if (target) {
      const { canvas, seat, x, y, width, height } = target;
      const box = canvas.getBoundingClientRect();
      const scale = Math.min(box.width / width, box.height / height);
      if (scale > 0 && Number.isFinite(x) && Number.isFinite(y)) {
        if (!cue.label) {
          cue.label = document.createElement('span');
          cue.label.setAttribute('aria-hidden', 'true');
          cue.label.style.cssText = 'position:fixed;z-index:100;pointer-events:none;transform:translate(-50%,-100%);font:700 14px/1.2 system-ui,sans-serif;color:#fff4ba;background:#16131de8;padding:2px 5px;border-radius:3px;white-space:nowrap';
          document.body.append(cue.label);
        }
        const label = `Player ${seat + 1}`;
        if (cue.label.textContent !== label) cue.label.textContent = label;
        cue.label.style.left = `${box.left + (box.width - width * scale) / 2 + x * scale}px`;
        cue.label.style.top = `${box.top + (box.height - height * scale) / 2 + y * scale}px`;
        cue.label.hidden = false;
      } else hide();
    } else hide();
    schedule();
  }
  function schedule() {
    if (track && active() && !renderRequest) renderRequest = requestAnimationFrame(render);
  }
  cue.track = read => {
    track = read;
    schedule();
    return () => { if (track === read) { track = null; reset(); } };
  };
  function side(event) {
    if (event.code === 'ControlLeft' || event.code === 'ControlRight') return event.code;
    if (event.key !== 'Control' && event.keyCode !== 17) return '';
    return event.location === 2 ? 'ControlRight' : event.location === 1 ? 'ControlLeft' : 'Control';
  }
  function frameAvailable(frame) {
    const box = frame.getBoundingClientRect();
    const style = frame.ownerDocument.defaultView?.getComputedStyle(frame);
    return frame.isConnected && box.width > 0 && box.height > 0 && style?.visibility !== 'hidden';
  }
  function blocked(event) {
    return event.isComposing || event.target?.closest?.(editing) || documents.some(doc => doc.querySelector(chat));
  }
  function down(event) {
    const key = side(event);
    if (!key || event.repeat || event.altKey || event.metaKey || blocked(event) || !frames.every(frameAvailable)) return;
    pressed.add(key);
    cue.held = true;
    cue.until = performance.now() + lifetime;
    schedule();
  }
  function up(event) {
    const key = side(event);
    if (!key) return;
    if (!event.ctrlKey || key === 'Control') pressed.clear();
    else { pressed.delete(key); pressed.delete('Control'); }
    cue.held = pressed.size !== 0;
  }
  const visibility = () => { if (documents.some(doc => doc.hidden)) reset(); };
  // Moving between the Launcher and game iframe does not leave the tab.
  const blur = () => setTimeout(() => { if (!topDocument.hasFocus()) reset(); }, 0);
  function bind() {
    for (const target of windows) {
      target.addEventListener('keydown', down, true);
      target.addEventListener('keyup', up, true);
      target.addEventListener('blur', blur);
    }
    for (const doc of documents) doc.addEventListener('visibilitychange', visibility);
  }
  addEventListener('pagehide', () => {
    for (const target of windows) {
      target.removeEventListener('keydown', down, true);
      target.removeEventListener('keyup', up, true);
      target.removeEventListener('blur', blur);
    }
    for (const doc of documents) doc.removeEventListener('visibilitychange', visibility);
    reset();
  });
  addEventListener('pageshow', event => { if (event.persisted) bind(); });
  bind();
})();
