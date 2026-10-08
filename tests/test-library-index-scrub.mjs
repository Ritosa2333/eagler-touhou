// Source-level gesture checks, without a browser or real-device claims.
import assert from 'node:assert/strict';
import {test} from 'node:test';
import {build} from 'esbuild';

const compiled = await build({entryPoints: [new URL('../src/launcher/game-library.mts', import.meta.url).pathname.replace(/^\/(\w:)/, '$1')], bundle: true, format: 'esm', write: false});
const {initializeGameLibrary, libraryIndexWidth} = await import(`data:text/javascript;base64,${Buffer.from(compiled.outputFiles[0].text).toString('base64')}`);

test('overflow leaves half of the next target, while short indexes fit completely', () => {
  for (const available of [274, 304, 360, 1200]) {
    const width = libraryIndexWidth(8, available);
    assert.ok(width < Math.min(available, 320));
    assert.equal((width - 4) % 46, 22, 'the right edge cuts the next 44px target in half');
  }
  assert.equal(libraryIndexWidth(5, 360), 236);
});

function fixture() {
  let now = 0, serial = 0, opens = 0;
  const timers = new Map(), frames = new Map();
  const rect = (left, width, top = 10, height = 44) => ({left, right: left + width, top, bottom: top + height, width, height});
  class Element {
    constructor() {
      this.handlers = new Map(); this.attributes = new Map(); this.dataset = {}; this.hidden = false;
      this.scrollLeft = 0; this.clientWidth = 260; this.scrollWidth = 374;
      const classes = new Set();
      this.classList = {contains: name => classes.has(name), add: (...names) => names.forEach(name => classes.add(name)), remove: (...names) => names.forEach(name => classes.delete(name)),
        toggle: (name, force) => {if (force ?? !classes.has(name)) classes.add(name); else classes.delete(name);}};
    }
    addEventListener(type, handler) {const list = this.handlers.get(type) ?? []; list.push(handler); this.handlers.set(type, list);}
    fire(type, event = {}) {for (const handler of this.handlers.get(type) ?? []) handler({preventDefault() {}, stopPropagation() {}, stopImmediatePropagation() {}, ...event});}
    setAttribute(name, value) {this.attributes.set(name, value);}
    querySelector() {return null;}
    querySelectorAll() {return [];}
    getClientRects() {return this.hidden ? [] : [this.getBoundingClientRect()];}
    getBoundingClientRect() {return rect(20, this.clientWidth);}
    scrollTo({left}) {this.scrollLeft = Math.max(0, Math.min(this.scrollWidth - this.clientWidth, left)); this.fire('scroll');}
    hasPointerCapture(id) {return this.capture === id;}
    setPointerCapture(id) {this.capture = id;}
    releasePointerCapture() {this.capture = undefined;}
    closest() {return null;}
    contains(node) {return node === this || buttons.includes(node);}
    focus() {document.activeElement = this;}
    click() {opens++;}
  }
  const shelf = new Element(), rail = new Element(), root = new Element(), dock = new Element();
  root.style = {set width(value) {dock.clientWidth = root.clientWidth = parseFloat(value);}};
  const ids = ['th06', 'th07', 'th08', 'th09', 'th10', 'th11', 'th15', 'th20'];
  const cards = ids.map((id, index) => {const item = new Element(); item.dataset.product = id; item.getBoundingClientRect = () => rect(20 + index * 250 - rail.scrollLeft, 240); return item;});
  const buttons = ids.map((id, index) => {const item = new Element(); item.dataset.minimapPreview = id; item.getBoundingClientRect = () => rect(24 + index * 46 - dock.scrollLeft, 44); return item;});
  rail.scrollWidth = 2000;
  shelf.querySelector = selector => selector === '.game-rail' ? rail : root;
  rail.querySelectorAll = selector => selector === '.game' ? cards : [];
  root.querySelector = () => dock;
  root.querySelectorAll = () => buttons;
  const document = new Element(); document.hidden = false; document.body = new Element(); document.querySelectorAll = () => [shelf];
  const window = new Element();
  const set = (callback, delay) => {const id = ++serial; timers.set(id, {at: now + delay, callback}); return id;};
  Object.assign(globalThis, {document, window, Node: Element, matchMedia: () => ({matches: false}),
    getComputedStyle: () => ({columnGap: '2px', paddingLeft: '4px'}),
    ResizeObserver: class {observe() {}}, MutationObserver: class {observe() {}},
    requestAnimationFrame: callback => {const id = ++serial; frames.set(id, callback); return id;}, cancelAnimationFrame: id => frames.delete(id),
    setTimeout: set, clearTimeout: id => timers.delete(id), performance: {now: () => now}});
  window.setTimeout = set;
  initializeGameLibrary();
  function advance(ms) {
    const end = now + ms;
    while (now < end) {
      now = Math.min(end, now + 16);
      for (const [id, timer] of [...timers]) if (timer.at <= now) {timers.delete(id); timer.callback();}
      const pending = [...frames]; frames.clear(); for (const [, callback] of pending) callback(now);
    }
  }
  const pointer = (x, extra = {}) => ({pointerId: 1, clientX: x, clientY: 30, pointerType: 'touch', isPrimary: true, button: 0, buttons: 1, ...extra});
  return {dock, root, buttons, document, advance, frames,
    down: () => buttons[0].fire('pointerdown', pointer(46)),
    move: (x, extra) => document.fire('pointermove', pointer(x, extra)),
    up: x => document.fire('pointerup', pointer(x, {buttons: 0})),
    cancel: () => document.fire('pointercancel', pointer(270)),
    get selected() {return buttons.find(button => button.classList.contains('is-current'))?.dataset.minimapPreview;},
    get opens() {return opens;}};
}

test('long hold selects continuously, reaches offscreen titles at either edge, and stops on release', () => {
  const f = fixture(); f.down(); f.advance(352);
  assert.equal(f.root.classList.contains('is-scrubbing'), true);
  f.move(276); f.advance(1400);
  assert.equal(f.selected, 'th20'); assert.equal(f.dock.scrollLeft, f.dock.scrollWidth - f.dock.clientWidth);
  f.move(21); f.advance(1400);
  assert.equal(f.selected, 'th06'); assert.equal(f.dock.scrollLeft, 0);
  f.move(276); f.advance(160); f.up(276);
  const stopped = f.dock.scrollLeft, selected = f.selected;
  f.advance(400); assert.equal(f.dock.scrollLeft, stopped); assert.equal(f.selected, selected);
  assert.equal(f.frames.size, 0); assert.equal(f.opens, 0);
  f.buttons.find(button => button.dataset.minimapPreview === selected).fire('click', {pointerType: 'touch'});
  assert.equal(f.opens, 0, 'release click is suppressed');
});

test('horizontal intent scrubs immediately; cancellation stops edge motion', () => {
  const f = fixture(); f.down(); f.move(180);
  assert.equal(f.selected, 'th09'); assert.equal(f.root.classList.contains('is-scrubbing'), true);
  f.move(276); f.advance(160); f.cancel();
  const stopped = f.dock.scrollLeft; f.advance(800);
  assert.equal(f.dock.scrollLeft, stopped); assert.equal(f.frames.size, 0); assert.equal(f.opens, 0);
});

test('vertical page intent before the hold does not select or acquire a scrub', () => {
  const f = fixture(); f.down(); f.move(46, {clientY: 90}); f.advance(600);
  assert.equal(f.selected, 'th06'); assert.equal(f.root.classList.contains('is-scrubbing'), false);
  assert.equal(f.dock.scrollLeft, 0); assert.equal(f.frames.size, 0);
});

test('a stopped index aligns its overflow hint instead of ending inside a gap', () => {
  const f = fixture();
  f.dock.scrollTo({left: 60}); f.advance(300);
  assert.equal(f.dock.scrollLeft, 46);
  assert.equal((f.dock.clientWidth - 4 + f.dock.scrollLeft) % 46, 22);
  assert.equal(f.selected, 'th06', 'aligning the hint does not select a game');
  assert.equal(f.opens, 0);
});
