// The lab owns the browser panel, practice configuration and native engine.
// This boundary only restores its entry and routes the Launcher's keyboard.
export function mountManagedPractice(ui, input, {release, readState}) {
  const panel = document.querySelector('#practice-panel');
  const screen = panel.parentElement;
  const button = document.querySelector('[data-practice-toolbar]');
  button.closest('.th02-player,.th04-player').classList.add('managed-browser-game');
  const modifiers = new Set();
  let enabled = false, mobileVisible = false;
  button.classList.add('managed-practice-open');
  button.textContent = '练习 / 辅助 · F12';
  button.hidden = true;
  button.onclick = () => { release(); ui.show(true); };
  screen.append(button);
  const style = document.createElement('style');
  style.textContent = `html,body{overflow:hidden}
    .managed-browser-game{position:fixed!important;inset:0!important;width:100%!important;height:100dvh!important;min-height:0!important;margin:0!important;padding:0!important;gap:0!important;grid-template-columns:minmax(0,1fr)!important;grid-template-rows:minmax(0,1fr)!important;grid-template-areas:"stage"!important;border:0!important;border-radius:0!important;z-index:10000}
    .managed-browser-game .player-stage{width:100%;height:100%}.managed-browser-game canvas{touch-action:none}
    .managed-practice-open{position:absolute;right:1%;top:1%;z-index:16;pointer-events:auto;min-height:32px;padding:4px 10px;color:#fff;background:#000c;border:1px solid #777;border-radius:4px;font:13px/1.5 system-ui;cursor:pointer}
    .managed-practice-open[hidden],.practice-screen:has(.practice-panel:not([hidden]))>.managed-practice-open{display:none!important}`;
  document.head.append(style);
  // Browser controls need real pointer events (select menus, text fields and
  // scrolling). Gameplay contacts go straight from this canvas to the same
  // managed touch mailbox used by the host HUD.
  const contacts = new Set();
  const pointer = event => {
    if (event.pointerType === 'mouse' || !enabled || !globalThis.__eaglerPC98Options().touchEnabled) return;
    if (event.type === 'pointerdown') {
      if (!event.target.matches('canvas') || blocked()) return;
      contacts.add(event.pointerId); event.target.setPointerCapture(event.pointerId);
    } else if (!contacts.has(event.pointerId)) return;
    event.preventDefault();
    const r = document.querySelector('canvas').getBoundingClientRect();
    input.pointer({type:event.type === 'pointerdown' ? 'down' : event.type === 'pointermove' ? 'move' : 'up',
      id:event.pointerId, x:(event.clientX-r.left)/r.width, y:(event.clientY-r.top)/r.height});
    if (event.type !== 'pointerdown' && event.type !== 'pointermove') contacts.delete(event.pointerId);
  };
  for (const type of ['pointerdown','pointermove','pointerup','pointercancel','lostpointercapture']) screen.addEventListener(type,pointer);
  const editing = () => !!document.activeElement?.closest?.('input,select,textarea,[contenteditable="true"]');
  const blocked = () => enabled && (document.querySelector('.practice-result')?.open || !panel.hidden && panel.classList.contains('practice-full') || editing());
  // The host HUD follows the browser controller's visibility. Only canvas
  // contacts are gameplay; panel taps and scrolling retain their native target.
  const result = document.querySelector('.practice-result');
  const syncControls = () => {
    const open = enabled && (!panel.hidden || result?.open);
    if (!panel.hidden && !panel.classList.contains('practice-full')) panel.scrollTop = 0;
    if (open && document.documentElement.dataset.browserPracticeOpen !== 'true') {
      release(); contacts.clear();
    }
    document.documentElement.dataset.browserPracticeOpen = String(!!open);
  };
  const observer = new MutationObserver(syncControls);
  observer.observe(panel,{attributes:true,attributeFilter:['hidden','class']});
  if(result)observer.observe(result,{attributes:true,attributeFilter:['open']});
  const boundary = {
    blocked,
    ownsFocus: () => enabled && (blocked() || panel.contains(document.activeElement)),
    nativeKey(event) { if (enabled && event.type === 'keydown') ui.handleKey(event); },
    key(code, down) {
      if (down) modifiers.add(code); else modifiers.delete(code);
      if (!enabled || !down) return false;
      let consumed = false;
      ui.handleKey({code, target:document.activeElement, repeat:false,
        shiftKey:modifiers.has('ShiftLeft') || modifiers.has('ShiftRight'),
        ctrlKey:modifiers.has('ControlLeft') || modifiers.has('ControlRight'),
        altKey:modifiers.has('AltLeft') || modifiers.has('AltRight'),
        metaKey:modifiers.has('MetaLeft') || modifiers.has('MetaRight'),
        preventDefault(){consumed = true;}, stopImmediatePropagation(){consumed = true;}});
      return consumed || blocked();
    },
    clear() { modifiers.clear(); contacts.clear(); },
    touch(message) {
      const visible = message.thpracMobilePanelsEnabled === true && message.thpracMobilePanelsVisible === true;
      if (!enabled || visible === mobileVisible) return;
      mobileVisible = visible;
      if (visible) { if (panel.hidden || !panel.classList.contains('practice-full')) { release(); ui.show(true); } }
      else if (!panel.hidden) panel.querySelector('[data-close-practice]').click();
    },
  };
  input.setPractice(boundary);
  return {
    start({readOnly = false} = {}) { enabled = !readOnly; button.hidden = !enabled; syncControls(); },
    stop() { enabled = false; button.hidden = true; modifiers.clear(); contacts.clear(); syncControls(); },
    state: () => ({configuration:ui.configuration(), options:ui.options, native:readState()}),
  };
}
