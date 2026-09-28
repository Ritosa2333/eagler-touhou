const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');

const html = fs.readFileSync(path.join(__dirname, '../resources/shell.html'), 'utf8');
const start = html.indexOf('const browserKeyboardDown = new Map();');
const end = html.indexOf('// BEGIN IMMEDIATE INPUT BRIDGE', start);
assert.ok(start >= 0 && end > start);
const handlers = {};
const controls = {keyboardBits: 0, keyboardPulseBits: 0};
const context = vm.createContext({
  launched: true,
  Module: {eaglerControls: controls},
  window: {addEventListener: (kind, callback) => { handlers[kind] = callback; }},
});
vm.runInContext(html.slice(start, end), context);
const key = (code, location, altKey = false) => ({code, key: 'Shift', keyCode: 16, location, altKey,
  preventDefault() {}});

handlers.keydown(key('ShiftLeft', 1));
assert.equal(controls.keyboardBits & 4, 4);
handlers.keyup(key('Unidentified', 0));
assert.equal(controls.keyboardBits & 4, 0, 'release with a changed location clears Focus');
assert.equal(controls.keyboardPulseBits & 4, 0, 'orphan release does not pulse Focus');

handlers.keydown(key('ShiftLeft', 1));
handlers.keyup(key('ShiftLeft', 1, true));
assert.equal(controls.keyboardBits & 4, 0, 'Alt-modified release still clears Focus');
console.log('PASS keyboard Focus release survives changed key identity and modifiers');
