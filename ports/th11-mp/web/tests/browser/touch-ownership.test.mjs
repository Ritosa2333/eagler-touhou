import assert from 'node:assert/strict';
import {bindOutsideTouches} from '../../sdl-runtime/eagler-host.mjs';

const handlers = new Map();
const canvas = {getBoundingClientRect: () => ({left: 100, top: 50, width: 640, height: 480})};
const letterbox = {setPointerCapture: () => {}};
const doc = {
  documentElement: {style: {}},
  addEventListener: (name, handler) => handlers.set(name, handler),
};
const calls = [];
const core = {
  sdl_touch: (...args) => calls.push(['touch', ...args]),
  sdl_touch_cancel: () => calls.push(['cancel']),
};
const cancel = bindOutsideTouches(doc, canvas, () => core, () => true);
const emit = (name, target, pointerId, x, y) => {
  let consumed = 0;
  handlers.get(name)({
    pointerType: 'touch', target, pointerId, clientX: x, clientY: y,
    preventDefault: () => consumed++, stopPropagation: () => consumed++,
    type: name,
  });
  return consumed;
};

// Canvas-origin gestures belong solely to SDL, never to the outside bridge.
assert.equal(emit('pointerdown', canvas, 1, 300, 200), 0);
assert.deepEqual(calls, []);
assert.equal(emit('pointermove', canvas, 1, 330, 200), 0);
assert.deepEqual(calls, []);

// A gesture that starts in the letterbox remains bridge-owned when crossing
// onto the canvas, and must never create another down event in this bridge.
assert.equal(emit('pointerdown', letterbox, 2, 100, 50), 2);
assert.equal(emit('pointermove', canvas, 2, 160, 98), 2);
assert.equal(emit('pointerup', canvas, 2, 160, 98), 2);
assert.equal(calls.length, 3);
assert.deepEqual(calls.map(c => c[1]), [0, 1, 2]);
assert.equal(calls[1][3], 60 / 640);
assert.equal(calls[1][4], 48 / 480);

// A canceled outside gesture resets transient controls rather than becoming
// an artificial release (which could otherwise trigger a tap action).
assert.equal(emit('pointerdown', letterbox, 3, 120, 75), 2);
assert.equal(emit('pointercancel', canvas, 3, 130, 80), 2);
assert.deepEqual(calls.at(-1), ['cancel']);
cancel();
assert.deepEqual(calls.at(-1), ['cancel']);
console.log('TH11 outside touch ownership: PASS');
