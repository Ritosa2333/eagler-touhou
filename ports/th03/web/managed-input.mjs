import {createBrowserKeyboard} from './directory-keyboard.mjs';
const buttons = {ArrowUp:1,ArrowDown:2,ArrowLeft:4,ArrowRight:8,KeyX:16,KeyZ:512,ShiftLeft:32,ShiftRight:32,Space:64,Escape:128,Enter:256};
export function managedInput({game, epoch, active, options}) {
  const held = new Set(), secondary = new Set(), localKeys = new Set(); let touch = {}, pulse = 0, menuPulse = 0, menuUntil = 0, keyPulse = 0, bomb = 0, escape = 0, point = null, motion = {x:0,y:0,active:false};
  const local2={KeyW:1,KeyS:2,KeyA:4,KeyD:8,KeyL:16,KeyK:32,ControlLeft:64,ControlRight:64,KeyJ:512};
  let notificationPending=false;
  function changed(){if(notificationPending)return;notificationPending=true;queueMicrotask(()=>{notificationPending=false;input.onChange?.();});}
  const keyboard = createBrowserKeyboard({send:(code, down) => {if(down){held.add(code);keyPulse|=buttons[code]??0;}else held.delete(code);changed();},
    hostContext:{target:globalThis,protocol:'eagler-touhou/1',game,epoch,origin:location.origin,ready:active}});
  const input = {
    key(message) { if(message.down)localKeys.add(message.code);else localKeys.delete(message.code);keyboard.event(message, message.down, 'hosted');changed(); },
    clear() { secondary.clear(); localKeys.clear();keyboard.clear(); touch = {}; pulse = keyPulse = menuPulse = menuUntil = 0; point = null; motion = {x:0,y:0,active:false}; changed(); },
    clearKeyboard() { localKeys.clear(); keyboard.clear(); held.clear(); keyPulse = 0; changed(); },
    cancelTouch() { secondary.clear(); touch = {}; pulse = menuPulse = menuUntil = 0; point = null; motion = {x:0,y:0,active:false}; changed(); },
    localSecond() {let bits=0;for(const code of localKeys)bits|=local2[code]??0;return bits;},
    touch(message) {
      if (typeof message.fireEnabled !== 'boolean' || typeof message.focusEnabled !== 'boolean' || !['bombSerial','escapeSerial'].every(k => Number.isSafeInteger(message[k]) && message[k] >= 0) || !['joystickX','joystickY'].every(k => Number.isFinite(message[k]) && Math.abs(message[k]) <= 32767)) throw Error('触控输入无效');
      if (message.bombSerial !== bomb) pulse |= 16;
      if (message.escapeSerial !== escape) pulse |= 128;
      bomb = message.bombSerial; escape = message.escapeSerial; touch = message; changed();
    },
    pointer(message) {
      if (!['down','move','up'].includes(message.type) || !Number.isSafeInteger(message.id) || !Number.isFinite(message.x) || !Number.isFinite(message.y)) throw Error('触控坐标无效');
      try {
      if (input.menuPointer?.(message)) return;
      if (message.type === 'down' && point) { secondary.add(message.id); point.menuMoved=true; return; }
      if (message.type === 'up' && secondary.delete(message.id)) { menuPulse = 128; menuUntil = performance.now()+120; return; }
      if (message.type === 'down') { point = {...message, startX:message.x, startY:message.y, menuMoved:false}; motion = {x:0,y:0,active:true,begin:true}; }
      else if (point?.id === message.id && message.type === 'move') {
        const scale = 16 * (options().touchSensitivity ?? 150) / 100;
        motion.x += (message.x - point.x) * 640 * scale; motion.y += (message.y - point.y) * 400 * scale;
        const dx=message.x-point.startX, dy=message.y-point.startY;
        if (!point.menuMoved && Math.max(Math.abs(dx),Math.abs(dy)) >= 0.05) {menuPulse = Math.abs(dx)>Math.abs(dy) ? (dx<0?4:8) : (dy<0?1:2);menuUntil=performance.now()+120;point.menuMoved=true;}
        point = {...point, x:message.x, y:message.y};
      } else if (point?.id === message.id) { if (!point.menuMoved) {menuPulse=512;menuUntil=performance.now()+120;} point = null; motion = {x:0,y:0,active:false}; }
      } finally { changed(); }
    },
    sample({play = true} = {}) {
      if (!active()) return 0;
      if (play || performance.now() >= menuUntil) menuPulse = 0;
      let bits = pulse | keyPulse | menuPulse; pulse = keyPulse = 0;
      for (const code of held) bits |= buttons[code] ?? 0;
      if (play && touch.fireEnabled) bits |= 512;
      if (touch.focusEnabled || play && options().touchFocusMode === 'two-finger' && secondary.size) bits |= 64;
      if (touch.joystickX < 0) bits |= 4; if (touch.joystickX > 0) bits |= 8;
      if (touch.joystickY < 0) bits |= 1; if (touch.joystickY > 0) bits |= 2;
      const pad = globalThis.__eaglerGamepadInputBridge?.sample(options().netplayPlayer ?? 0);
      if (pad >= 0) for (const [mask, out] of [[1,512],[2,16],[4,32],[8,128],[16,1],[32,2],[64,4],[128,8],[512,256]]) if (pad & mask) bits |= out;
      return bits;
    },
    motion() { const value = {...motion, unlimited:options().touchMovementMode === 'touch-unlimited'}; motion.x = motion.y = 0; motion.begin = false; return value; },
  };
  for (const type of ['keydown','keyup']) addEventListener(type, event => {
    if (event.__eaglerPC98Synthetic) return;
    if(type==='keydown')localKeys.add(event.code);else localKeys.delete(event.code);
    const down=type==='keydown',handled=keyboard.mapped()?keyboard.mapNative(event,down):keyboard.event(event,down);
    changed();if (handled) { event.preventDefault(); event.stopImmediatePropagation(); }
  }, true);
  // The host touch surface is outside the iframe; focus changes release keys only.
  addEventListener('blur', () => input.clearKeyboard());
  addEventListener('pagehide', () => input.clear());
  document.addEventListener('visibilitychange', () => { if(document.hidden)input.clear(); });
  return input;
}
