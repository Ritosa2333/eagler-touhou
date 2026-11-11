import {createBrowserKeyboard} from './directory-keyboard.mjs';
const buttons = {ArrowUp:1,ArrowDown:2,ArrowLeft:4,ArrowRight:8,KeyX:16,KeyZ:32,ShiftLeft:64,ShiftRight:64,Escape:128,Enter:256};
export function managedInput({game, epoch, active, options}) {
  const held = new Set(); let touch = {}, pulse = 0, localPulse = 0, bomb = 0, escape = 0, point = null, motion = {x:0,y:0,active:false};
  const keyboard = createBrowserKeyboard({send:(code, down) => down ? held.add(code) : held.delete(code),
    hostContext:{target:globalThis,protocol:'eagler-touhou/1',game,epoch,origin:location.origin,ready:active}});
  const input = {
    key(message) { keyboard.event(message, message.down, 'hosted'); },
    clear() { keyboard.clear(); touch = {}; pulse = localPulse = 0; point = null; motion = {x:0,y:0,active:false}; },
    touch(message) {
      if (typeof message.fireEnabled !== 'boolean' || typeof message.focusEnabled !== 'boolean' || !['bombSerial','escapeSerial'].every(k => Number.isSafeInteger(message[k]) && message[k] >= 0) || !['joystickX','joystickY'].every(k => Number.isFinite(message[k]) && Math.abs(message[k]) <= 32767)) throw Error('触控输入无效');
      if (message.bombSerial !== bomb) {pulse |= 16; localPulse |= 16;}
      if (message.escapeSerial !== escape) pulse |= 128;
      bomb = message.bombSerial; escape = message.escapeSerial; touch = message;
    },
    pointer(message) {
      if (!['down','move','up'].includes(message.type) || !Number.isSafeInteger(message.id) || !Number.isFinite(message.x) || !Number.isFinite(message.y)) throw Error('触控坐标无效');
      if (message.type === 'down') { point = message; motion = {x:0,y:0,active:true,begin:true}; }
      else if (point?.id === message.id && message.type === 'move') {
        const scale = 16 * (options().touchSensitivity ?? 150) / 100;
        motion.x += (message.x - point.x) * 640 * scale; motion.y += (message.y - point.y) * 400 * scale; point = message;
      } else if (point?.id === message.id) { point = null; motion = {x:0,y:0,active:false}; }
    },
    nativeSample() {
      if (!active()) return 0;
      let bits=0;
      const net={ArrowLeft:1,ArrowRight:2,KeyZ:4,KeyX:8,KeyC:4096,ArrowUp:16,ArrowDown:32,Enter:64,Escape:128};
      for(const code of held)bits|=net[code]??0;
      const directions=touch.joystickX<0?1:touch.joystickX>0?2:0;
      if(directions&1)bits|=1;if(directions&2)bits|=2;
      if(touch.joystickY<0)bits|=16;if(touch.joystickY>0)bits|=32;
      if(touch.fireEnabled)bits|=4;if(pulse&16)bits|=4096;if(pulse&128)bits|=128; pulse = 0;
      const pad=globalThis.__eaglerGamepadInputBridge?.sample(options().netplayPlayer??0);
      if(pad>=0)for(const [mask,out] of [[1,4],[2,8],[4,4096],[16,16],[32,32],[64,1],[128,2],[512,64]])if(pad&mask)bits|=out;
      return bits;
    },
    localSample() {
      if (!active()) return 0;
      let bits=0;for(const code of held)bits|=({ArrowLeft:1,ArrowRight:2,KeyZ:4,KeyX:8,KeyC:16}[code]??0);
      if(touch.joystickX<0)bits|=1;if(touch.joystickX>0)bits|=2;
      if(touch.fireEnabled)bits|=4;if(localPulse&16)bits|=16; localPulse = 0;
      const pad=globalThis.__eaglerGamepadInputBridge?.sample(options().netplayPlayer??0);
      if(pad>=0)for(const [mask,out] of [[1,4],[2,8],[4,16],[64,1],[128,2]])if(pad&mask)bits|=out;
      return bits;
    },
    sample() {
      if (!active()) return 0;
      let bits = pulse; pulse = 0;
      for (const code of held) bits |= buttons[code] ?? 0;
      if (touch.fireEnabled) bits |= 32;
      if (touch.focusEnabled) bits |= 64;
      if (touch.joystickX < 0) bits |= 4; if (touch.joystickX > 0) bits |= 8;
      if (touch.joystickY < 0) bits |= 1; if (touch.joystickY > 0) bits |= 2;
      const pad = globalThis.__eaglerGamepadInputBridge?.sample(options().netplayPlayer ?? 0);
      if (pad >= 0) for (const [mask, out] of [[1,32],[2,16],[4,64],[8,128],[16,1],[32,2],[64,4],[128,8],[512,256]]) if (pad & mask) bits |= out;
      return bits;
    },
    motion() { const value = {...motion, unlimited:options().touchMovementMode === 'touch-unlimited'}; motion.x = motion.y = 0; motion.begin = false; return value; },
  };
  for (const type of ['keydown','keyup']) addEventListener(type, event => { if (event.__eaglerPC98Synthetic) return; if (keyboard.mapNative(event, type === 'keydown')) { event.preventDefault(); event.stopImmediatePropagation(); } }, true);
  addEventListener('blur', () => input.clear());
  return input;
}
