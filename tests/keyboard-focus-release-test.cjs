const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const html = fs.readFileSync(path.join(__dirname, '../resources/shell.html'), 'utf8');
const start = html.indexOf('const browserKeyboardDown = new Map();');
const end = html.indexOf('// BEGIN IMMEDIATE INPUT BRIDGE', start);
assert.ok(start >= 0 && end > start);
function harness() {
  const handlers = {}, controls = {keyboardBits: 0, keyboardPulseBits: 0};
  const context = vm.createContext({launched: true, Module: {eaglerControls: controls},
    window: {addEventListener: (kind, callback) => {handlers[kind] = callback;}}});
  vm.runInContext(html.slice(start, end), context);
  const event = (code, fields = {}) => ({code, key: 'Shift', keyCode: 16, location: 0,
    altKey: false, metaKey: false, repeat: false, preventDefault() {}, ...fields});
  return {controls, context, down: (code, fields) => handlers.keydown(event(code, fields)),
    up: (code, fields) => handlers.keyup(event(code, fields)),
    clear: () => vm.runInContext('clearBrowserKeyboard()', context)};
}
let h = harness();
h.down('ShiftLeft', {location: 1}); h.up('Unidentified');
assert.equal(h.controls.keyboardBits & 4, 0);
assert.equal(h.controls.keyboardPulseBits & 4, 0);
for (const modifier of ['altKey', 'metaKey']) {
  h = harness(); h.down('ShiftLeft'); h.up('ShiftLeft', {[modifier]: true});
  assert.equal(h.controls.keyboardBits & 4, 0, 'modified UP must release');
}
for (const first of ['ShiftLeft', 'ShiftRight']) {
  h = harness(); h.down('ShiftLeft', {location: 1}); h.down('ShiftRight', {location: 2});
  h.up(first, {location: 0});
  assert.equal(h.controls.keyboardBits & 4, 4, 'one released Shift preserves the other physical owner');
  h.up(first === 'ShiftLeft' ? 'ShiftRight' : 'ShiftLeft', {location: 0});
  assert.equal(h.controls.keyboardBits & 4, 0);
}
h = harness(); h.down('ShiftLeft'); h.down('ShiftRight'); h.up('Unidentified');
assert.equal(h.controls.keyboardBits & 4, 0, 'ambiguous UP cannot leave an arbitrary owner latched');
h = harness(); h.down('Unidentified'); h.up('ShiftLeft', {location: 1});
assert.equal(h.controls.keyboardBits & 4, 0, 'UP with improved physical identity releases unknown DOWN');
h = harness(); h.down('ArrowUp', {key: 'ArrowUp', keyCode: 38});
h.down('Numpad8', {key: '8', keyCode: 104});
h.up('ArrowUp', {key: 'ArrowUp', keyCode: 38});
assert.equal(h.controls.keyboardBits & 16, 16, 'Arrow/Numpad ownership is independent');
h.up('Numpad8', {key: '8', keyCode: 104}); assert.equal(h.controls.keyboardBits & 16, 0);
h = harness(); h.up('KeyZ', {key: 'z', keyCode: 90});
assert.equal(h.controls.keyboardPulseBits & 1, 1, 'initial vendor release-only taps remain supported');
h.controls.keyboardPulseBits = 0; h.up('KeyZ', {key: 'z', keyCode: 90});
assert.equal(h.controls.keyboardPulseBits, 1, 'successive vendor release-only taps remain supported');
h = harness(); h.clear(); h.up('KeyZ', {key: 'z', keyCode: 90});
assert.equal(h.controls.keyboardPulseBits, 1, 'empty startup reset preserves vendor release-only taps');
h = harness(); h.down('KeyZ', {key: 'z', keyCode: 90}); h.up('KeyZ', {key: 'z', keyCode: 90});
h.up('KeyZ', {key: 'z', keyCode: 90});
assert.equal(h.controls.keyboardPulseBits, 0, 'duplicate observed release cannot synthesize another press');
h = harness(); h.down('KeyZ', {key: 'z', keyCode: 90}); h.down('F1', {key: 'F1', keyCode: 112}); h.clear();
vm.runInContext('publishBrowserKeyboard()', h.context);
assert.equal(h.controls.keyboardBits, 0, 'reset must clear owners, not only published output');
assert.equal(h.controls.thpracKeyboardBits, 0);
h.down('KeyZ', {key: 'z', keyCode: 90, repeat: true}); h.up('KeyZ', {key: 'z', keyCode: 90});
assert.equal(h.controls.keyboardBits | h.controls.keyboardPulseBits, 0, 'cancelled repeat/orphan UP cannot re-arm input');
h.down('KeyZ', {key: 'z', keyCode: 90}); assert.equal(h.controls.keyboardBits & 1, 1);
h.up('KeyZ', {key: 'z', keyCode: 90}); assert.equal(h.controls.keyboardBits, 0);
h = harness(); h.down('F1', {key: 'F1', keyCode: 112});
h.up('Unidentified', {key: 'F1', keyCode: 0});
assert.equal(h.controls.thpracKeyboardBits, 0, 'trainer keys follow the same release ownership');
console.log('PASS Runtime keyboard ownership: modifiers, aliases, native/host identity, cancellation and trainer');
