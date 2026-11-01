// Standalone page touch controls. The native controller owns gesture timing and
// game context; this view only forwards contacts and the user's control state.
export function bindTouchControls(mod, canvas) {
  const coarse = window.matchMedia?.('(pointer: coarse)');
  if (!(coarse?.matches || navigator.maxTouchPoints > 0)) return () => {};

  const style = document.createElement('style');
  style.textContent = `
    .th13-touch { position:fixed; inset:0; pointer-events:none; z-index:10; }
    .th13-touch-group { position:absolute; display:flex; gap:6px; pointer-events:auto; }
    .th13-touch-top { top:env(safe-area-inset-top, 0px); right:env(safe-area-inset-right, 0px); }
    .th13-touch-bottom { bottom:env(safe-area-inset-bottom, 0px); right:env(safe-area-inset-right, 0px); }
    .th13-touch button { min-width:48px; min-height:44px; padding:6px; color:#fff;
      background:#222a; border:1px solid #fff9; border-radius:8px; font:14px system-ui;
      touch-action:none; user-select:none; -webkit-user-select:none; }
    .th13-touch button[aria-pressed="true"] { background:#664b82dd; }
  `;
  document.head.append(style);
  const overlay = document.createElement('div');
  overlay.className = 'th13-touch';
  const top = document.createElement('div'); top.className = 'th13-touch-group th13-touch-top';
  const bottom = document.createElement('div'); bottom.className = 'th13-touch-group th13-touch-bottom';
  overlay.append(top, bottom); document.body.append(overlay);
  let fire = true, focus = false, bomb = 0, escape = 0, disposed = false;
  const pressedKeys = new Set();
  const listeners = [];
  const on = (node, type, handler, options) => {
    node.addEventListener(type, handler, options);
    listeners.push(() => node.removeEventListener(type, handler, options));
  };
  const sync = () => mod._th13_touch_controls(1, +fire, +focus, bomb, escape, 0, 0);
  const button = (group, id, label) => {
    const node = document.createElement('button');
    node.type = 'button'; node.dataset.touch = id; node.textContent = label;
    group.append(node); return node;
  };
  const toggle = button(bottom, 'fire', '自动射击');
  toggle.setAttribute('aria-pressed', 'true');
  const focusButton = button(bottom, 'focus', '低速');
  focusButton.setAttribute('aria-pressed', 'false');
  const bombButton = button(bottom, 'bomb', '符卡');
  const tranceButton = button(bottom, 'trance', '灵界');
  const pauseButton = button(top, 'pause', '暂停');
  const retryButton = button(top, 'retry', '重开');
  const activate = (node, fn) => on(node, 'pointerdown', event => {
    if (event.pointerType === 'mouse') return;
    event.preventDefault(); fn();
  });
  activate(toggle, () => {
    fire = !fire; toggle.setAttribute('aria-pressed', String(fire)); sync();
  });
  activate(bombButton, () => { bomb = (bomb + 1) >>> 0; sync(); });
  activate(pauseButton, () => { escape = (escape + 1) >>> 0; sync(); });
  const focusPointers = new Set();
  on(focusButton, 'pointerdown', event => {
    if (event.pointerType === 'mouse') return;
    event.preventDefault();
    focusPointers.add(event.pointerId); focus = true;
    focusButton.setAttribute('aria-pressed', 'true');
    try { focusButton.setPointerCapture(event.pointerId); } catch {}
    sync();
  });
  for (const type of ['pointerup', 'pointercancel', 'lostpointercapture']) on(focusButton, type, event => {
    if (!focusPointers.delete(event.pointerId)) return;
    focus = focusPointers.size > 0;
    focusButton.setAttribute('aria-pressed', String(focus)); sync();
  });
  const keyButton = (node, scan) => {
    const owners = new Set();
    on(node, 'pointerdown', event => {
      if (event.pointerType === 'mouse') return;
      event.preventDefault(); owners.add(event.pointerId);
      if (!pressedKeys.has(scan)) { pressedKeys.add(scan); mod._th13_key(scan, 1); }
      try { node.setPointerCapture(event.pointerId); } catch {}
    });
    for (const type of ['pointerup', 'pointercancel', 'lostpointercapture']) on(node, type, event => {
      if (!owners.delete(event.pointerId) || owners.size) return;
      pressedKeys.delete(scan); mod._th13_key(scan, 0);
    });
    return () => owners.clear();
  };
  const clearTrance = keyButton(tranceButton, 46); // C
  const clearRetry = keyButton(retryButton, 19); // R
  const contacts = new Set();
  const forward = (event, type) => {
    const box = canvas.getBoundingClientRect();
    if (box.width && box.height)
      mod._th13_touch(type, event.pointerId, (event.clientX - box.left) / box.width,
                      (event.clientY - box.top) / box.height);
  };
  on(canvas, 'pointerdown', event => {
    if (event.pointerType === 'mouse') return;
    event.preventDefault(); contacts.add(event.pointerId);
    try { canvas.setPointerCapture(event.pointerId); } catch {}
    forward(event, 0);
  });
  on(canvas, 'pointermove', event => {
    if (!contacts.has(event.pointerId)) return;
    event.preventDefault(); forward(event, 1);
  });
  on(canvas, 'pointerup', event => {
    if (!contacts.delete(event.pointerId)) return;
    event.preventDefault(); forward(event, 2);
  });
  for (const type of ['pointercancel', 'lostpointercapture']) on(canvas, type, event => {
    if (!contacts.has(event.pointerId)) return;
    event.preventDefault(); cancel();
  });
  const cancel = () => {
    contacts.clear(); focusPointers.clear(); focus = false;
    focusButton.setAttribute('aria-pressed', 'false');
    clearTrance(); clearRetry();
    for (const scan of pressedKeys) mod._th13_key(scan, 0);
    pressedKeys.clear(); mod._th13_touch_cancel(); sync();
  };
  on(window, 'blur', cancel);
  on(document, 'visibilitychange', () => { if (document.hidden) cancel(); });
  mod._th13_touch_options(1, 0, 1.5, 0, 0);
  sync();
  return () => {
    if (disposed) return; disposed = true; cancel();
    for (const remove of listeners) remove();
    mod._th13_touch_options(0, 0, 1.5, 0, 0);
    overlay.remove(); style.remove();
  };
}
