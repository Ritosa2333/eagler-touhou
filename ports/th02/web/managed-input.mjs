import {createBrowserKeyboard} from './directory-keyboard.mjs';
import {packTouch,unpackTouch} from './touch-input.js';
const buttons = {ArrowUp:1,ArrowDown:2,ArrowLeft:4,ArrowRight:8,KeyX:16,KeyZ:32,ShiftLeft:64,ShiftRight:64,Escape:128,Enter:256};
export function managedInput({game, epoch, active, options}) {
  let practice;
  const held = new Set(); let touch = {}, pulse = 0, bomb = 0, escape = 0, point = null, motion = {x:0,y:0,active:false};
  const keyboard = createBrowserKeyboard({send:(code, down) => {
    const consumed = practice?.key(code, down);
    if (down && !consumed) held.add(code); else held.delete(code);
  },
    hostContext:{target:globalThis,protocol:'eagler-touhou/1',game,epoch,origin:location.origin,ready:active}});
  const input = {
    setPractice(value) { practice = value; },
    releaseGameplay() { held.clear(); pulse = 0; point = null; motion = {x:0,y:0,active:false}; },
    key(message) { keyboard.event(message, message.down, 'hosted'); },
    clear() { keyboard.clear(); practice?.clear(); touch = {}; pulse = 0; point = null; motion = {x:0,y:0,active:false}; },
    touch(message) {
      if (typeof message.fireEnabled !== 'boolean' || typeof message.focusEnabled !== 'boolean' || !['bombSerial','escapeSerial'].every(k => Number.isSafeInteger(message[k]) && message[k] >= 0) || !['joystickX','joystickY'].every(k => Number.isFinite(message[k]) && Math.abs(message[k]) <= 32767)) throw Error('触控输入无效');
      if (options().touchEnabled && message.bombSerial !== bomb) pulse |= 16;
      if (options().touchEnabled && message.escapeSerial !== escape) pulse |= 128;
      bomb = message.bombSerial; escape = message.escapeSerial; touch = message; practice?.touch(message);
    },
    pointer(message) {
      if (!['down','move','up'].includes(message.type) || !Number.isSafeInteger(message.id) || !Number.isFinite(message.x) || !Number.isFinite(message.y)) throw Error('触控坐标无效');
      if (message.type === 'down') { point = message; motion = {x:0,y:0,active:true,begin:true}; }
      else if (point?.id === message.id && message.type === 'move') {
        const scale = 16 * (options().touchSensitivity ?? 150) / 100;
        motion.x += (message.x - point.x) * 640 * scale; motion.y += (message.y - point.y) * 400 * scale; point = message;
      } else if (point?.id === message.id) { point = null; motion = {x:0,y:0,active:false}; }
    },
    sample({play = true} = {}) {
      if (!active() || practice?.blocked()) return 0;
      let bits = pulse; pulse = 0;
      for (const code of held) bits |= buttons[code] ?? 0;
      if (options().touchEnabled && play && touch.fireEnabled) bits |= 32;
      if (options().touchEnabled && play && touch.focusEnabled) bits |= 64;
      if (options().touchEnabled && touch.joystickX < 0) bits |= 4; if (options().touchEnabled && touch.joystickX > 0) bits |= 8;
      if (options().touchEnabled && touch.joystickY < 0) bits |= 1; if (options().touchEnabled && touch.joystickY > 0) bits |= 2;
      const pad = globalThis.__eaglerGamepadInputBridge?.sample(options().netplayPlayer ?? 0);
      if (pad >= 0) for (const [mask, out] of [[1,32],[2,16],[4,64],[8,128],[16,1],[32,2],[64,4],[128,8],[512,256]]) if (pad & mask) bits |= out;
      return bits;
    },
    motion() { if (practice?.blocked()) { point = null; motion = {x:0,y:0,active:false}; } const value = unpackTouch(packTouch(0,{...motion, unlimited:options().touchMovementMode === 'touch-unlimited'})); motion.x = motion.y = 0; motion.begin = false; return value; },
  };
  for (const type of ['keydown','keyup']) addEventListener(type, event => {
    if (event.__eaglerPC98Synthetic) return;
    if (practice?.ownsFocus()) {
      practice.nativeKey(event);
      if (type === 'keyup') keyboard.mapNative(event, false) || keyboard.event(event, false);
      if (event.defaultPrevented) event.stopImmediatePropagation();
      return;
    }
    if (event.type === 'keydown' && event.target?.closest?.('input,select,textarea,[contenteditable="true"]')) return;
    if (keyboard.mapNative(event, type === 'keydown') || !globalThis.__eaglerKeyboardInputBridge?.active() && keyboard.event(event, type === 'keydown')) {
      event.preventDefault(); event.stopImmediatePropagation();
    }
  }, true);
  addEventListener('blur', () => input.clear());
  return input;
}
