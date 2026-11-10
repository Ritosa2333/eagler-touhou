/** Pinned main app.mts4504–4510,6933–6945,7433–7449,9490–9527: native
 * fullscreen menu hosts, workbench dismissal and editor lifecycle. Synthetic DOM only;
 * not evidence of real fullscreen top-layer paint or browser focus behavior. */
import {test, beforeAll as before, afterAll as after, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {mkdir, mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom} from './mounted-dom-environment.mjs';
import {authoredSourcesPlugin} from './authored-sources.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));
let env, work, api, React, createRoot, root, fullscreen, webkitFullscreen;
before(async () => {
  env = installMountedDom();
  React = await import('react'); ({createRoot} = await import('react-dom/client'));
  Object.defineProperty(env.document, 'fullscreenElement', {configurable: true, get: () => fullscreen});
  Object.defineProperty(env.document, 'webkitFullscreenElement', {configurable: true, get: () => webkitFullscreen});
  await mkdir(resolve(project, '.cache'), {recursive: true});
  work = await mkdtemp(resolve(project, '.cache/touch-editor-selects-'));
  const outfile = resolve(work, 'components.mjs');
  await build({absWorkingDir: project, stdin: {resolveDir: project, loader: 'ts', contents: `
    export {MainSelect,closeMainSelectMenus} from './app/components/launcher/MainSelect.tsx';
    export {Select,SelectTrigger,SelectValue,SelectContent,SelectItem,closeSelectMenus} from './app/ui/select.tsx';
    export {TouchLayoutEditor} from './app/components/settings/TouchLayoutEditor.tsx';
    export {createGameSettingsModel} from './app/models/game-settings.ts';
    export {createTouchLayoutModel} from './app/models/touch-layout.ts';
    export {touchLayoutControlNames, touchLayoutControlMeta, normalizeTouchLayoutPriorityOrder} from './src/launcher/touch-layout-model.mts';
  `}, outfile, bundle: true, platform: 'node', format: 'esm', jsx: 'automatic', packages: 'external', logLevel: 'silent', plugins: [authoredSourcesPlugin(project)]});
  api = await import(pathToFileURL(outfile).href);
});
afterEach(async () => {
  if (root) {await React.act(async () => root.unmount()); root = null;}
  assert.equal(env.document.querySelectorAll('.ui-select-content').length, 0);
  assert.equal(env.document.querySelectorAll('.mizuki-select').length, 0);
  assert.deepEqual(env.errors.splice(0), []);
  fullscreen = null; webkitFullscreen = null; env.document.body.replaceChildren();
});
after(async () => {env?.close(); if (work) await rm(work, {recursive: true, force: true});});
const el = (...args) => React.createElement(...args);
function n(selector) {const node = env.document.querySelector(selector); assert.ok(node, selector); return node;}
async function mount(children) {
  const container = env.document.createElement('div'); env.document.body.append(container); root = createRoot(container);
  await React.act(async () => root.render(el(React.StrictMode, null, children)));
}
async function click(target) {await React.act(async () => (typeof target === 'string' ? n(target) : target).click());}
async function touchTap(target, pointerId) {await React.act(async () => {
  const bounds = target.getBoundingClientRect();
  const pointer = {bubbles: true, cancelable: true, pointerId, pointerType: 'touch', button: 0, clientX: bounds.left + bounds.width / 2, clientY: bounds.top + bounds.height / 2};
  target.dispatchEvent(new env.window.PointerEvent('pointerdown', pointer));
  target.dispatchEvent(new env.window.PointerEvent('pointerup', pointer));
  target.dispatchEvent(new env.window.MouseEvent('click', {bubbles: true, cancelable: true}));
});}
async function touchRetargetedTap(downTarget, clickTarget, pointerId) {
  const bounds = clickTarget.getBoundingClientRect();
  const pointer = {bubbles: true, cancelable: true, pointerId, pointerType: 'touch', button: 0, clientX: bounds.left + bounds.width / 2, clientY: bounds.top + bounds.height / 2};
  await React.act(async () => {downTarget.dispatchEvent(new env.window.PointerEvent('pointerdown', pointer));});
  await React.act(async () => {downTarget.dispatchEvent(new env.window.Event('touchstart', {bubbles: true, cancelable: true}));});
  await React.act(async () => {downTarget.dispatchEvent(new env.window.PointerEvent('pointerup', pointer));});
  await React.act(async () => {downTarget.dispatchEvent(new env.window.Event('touchend', {bubbles: true, cancelable: true}));});
  await React.act(async () => {clickTarget.dispatchEvent(new env.window.MouseEvent('click', {bubbles: true, cancelable: true, detail: 1}));});
}
function select() {return el(api.MainSelect, {id: 'hostSelect', className: 'option-select', defaultValue: 'a'}, el('option', {value: 'a'}, 'A'), el('option', {value: 'b'}, 'B'));}
function uiKitSelect() {
  return el(api.Select, {defaultValue: 'number'},
    el(api.SelectTrigger, {id: 'sort-trigger', 'aria-label': 'Sort'}, el(api.SelectValue)),
    el(api.SelectContent, null,
      el(api.SelectItem, {value: 'number'}, 'By number'),
      el(api.SelectItem, {value: 'name'}, 'By name')));
}

for (const target of ['windowed', 'player', 'webkit-player', 'root', 'foreign', 'dialog']) test(`Main select keeps the menu above its ${target} player`, async () => {
  await mount(el('section', {id: 'player'}, target === 'dialog' ? el('dialog', {open: true, id: 'nativeDialog'}, select()) : select()));
  if (target === 'player' || target === 'dialog') fullscreen = n('#player');
  if (target === 'webkit-player') webkitFullscreen = n('#player');
  if (target === 'root') fullscreen = env.document.documentElement;
  if (target === 'foreign') {fullscreen = env.document.createElement('section'); env.document.body.append(fullscreen);}
  await click('.mizuki-select-trigger');
  const menu = n('.ui-select-content');
  assert.equal(menu.closest('[data-radix-popper-content-wrapper]').parentElement.id, target === 'dialog' ? 'nativeDialog' : 'player');
  await click(menu.querySelector('[data-value="b"]'));
  assert.equal(n('#hostSelect').value, 'b'); assert.equal(!menu.isConnected, true);
  assert.equal(env.document.activeElement.classList.contains('mizuki-select-trigger'), true);
});

test('Main select reopens the same menu in the current fullscreen host', async () => {
  await mount(el('section', {id: 'player'}, select()));
  const viewport=env.document.createElement('div');viewport.id='gameViewport';n('#player').append(viewport);
  await click('.mizuki-select-trigger'); const menu = n('.ui-select-content'); assert.equal(menu.closest('[data-radix-popper-content-wrapper]').parentElement.id,'player');
  await React.act(async()=>api.closeMainSelectMenus()); fullscreen = viewport;
  await click('.mizuki-select-trigger'); const reopened = n('.ui-select-content'); assert.equal(reopened.closest('[data-radix-popper-content-wrapper]').parentElement.id,'gameViewport');
  await React.act(async()=>api.closeMainSelectMenus()); fullscreen = null;
  await click('.mizuki-select-trigger'); const again = n('.ui-select-content'); assert.equal(again.closest('[data-radix-popper-content-wrapper]').parentElement.id,'player');
  assert.equal(env.document.querySelectorAll('.ui-select-content').length, 1);
});

async function editor({storage = null} = {}) {
  const player = env.document.createElement('section'); player.id = 'player'; player.innerHTML = '<div id="gameViewport"></div>'; env.document.body.append(player);
  const settings = api.createGameSettingsModel({storage: null});
  settings.hydrate({productId: 'th06', uiLocale: 'en', mobile: false, webMidiAvailable: false, languages: [{id: 'ja'}], musicAvailability: {audio: true, midiAvailable: true, importServer: false, remoteOggAdvertised: false}});
  const model = api.createTouchLayoutModel({storage}); model.hydrate();
  const errors = [], feedback = [];
  const controls = Object.fromEntries(api.touchLayoutControlNames.map((name, index) => [name, {x: .15 + index % 5 * .12, y: .25 + Math.floor(index / 5) * .3, scale: name === 'bomb' ? 1.5 : 1, priority: api.touchLayoutControlMeta[name].priority}]));
  await mount(el(api.TouchLayoutEditor, {playerElement: player, model, settings, actions: {feedback: (...args) => feedback.push(args), reportError: error => errors.push(error), async confirm() {return false;}}, native: {
    async enterFullscreen() {fullscreen = player; return true;}, async exitFullscreen() {fullscreen = null;}, canSwitchOrientation: () => false,
    measureDefaults: () => ({controls: api.normalizeTouchLayoutPriorityOrder(structuredClone(controls)), viewport: {x: 0}}),
  }, onCloseIntent() {throw new Error('Unexpected close');}}));
  for (let i = 0; i < 20 && !model.getSnapshot().isEditing; i++) await React.act(async () => {await new Promise(resolve => setTimeout(resolve, 10));});
  assert.equal(model.getSnapshot().isEditing, true); assert.deepEqual(errors, []);
  return {player, model, settings, errors};
}
async function openMovementMenu() {await click(n('#touchMovementMode').closest('.mizuki-select').querySelector('.mizuki-select-trigger')); return n('.ui-select-content');}

test('Touch select trigger toggles its popup closed on a second activation', async () => {
  const f = await editor(), trigger = n('#touchMovementMode-trigger');
  await click(trigger);
  assert.equal(n('.ui-select-content').isConnected, true);
  assert.equal(trigger.getAttribute('aria-expanded'), 'true');
  await click(trigger);
  assert.equal(env.document.querySelector('.ui-select-content'), null);
  assert.equal(trigger.getAttribute('aria-expanded'), 'false');
  assert.deepEqual(f.errors, []);
});

test('Touch taps close the currently open selector without reopening it', async () => {
  const f = await editor(), trigger = n('#touchMovementMode-trigger');
  await touchTap(trigger, 41);
  assert.equal(n('.ui-select-content').isConnected, true);
  assert.equal(trigger.getAttribute('aria-expanded'), 'true');
  await touchTap(trigger, 42);
  assert.equal(env.document.querySelector('.ui-select-content'), null);
  assert.equal(trigger.getAttribute('aria-expanded'), 'false');
  assert.equal(env.document.activeElement, trigger);
  let outsideClicks = 0;
  const outside = env.document.createElement('button'); outside.id = 'outsideSelectAction'; outside.addEventListener('click', () => outsideClicks++); env.document.body.append(outside);
  await click(outside); assert.equal(outsideClicks, 1, 'consuming the compatibility click must not swallow another control');
  const nextTrigger = n('#touchFocusMode-trigger'); await touchTap(nextTrigger, 53);
  assert.equal(nextTrigger.getAttribute('aria-expanded'), 'true', 'the next selector has its own activation sequence');
  assert.equal(env.document.querySelectorAll('.ui-select-content').length, 1);
  assert.deepEqual(f.errors, []);
});

test('A touch compatibility click cannot reopen a selector dismissed by the earlier pointerdown', async () => {
  const f = await editor(), trigger = n('#touchMovementMode-trigger');
  await touchTap(trigger, 51);
  assert.equal(trigger.getAttribute('aria-expanded'), 'true');
  assert.equal(n('.ui-select-content').isConnected, true);
  await touchRetargetedTap(env.document.body, trigger, 52);
  assert.equal(env.document.querySelector('.ui-select-content'), null);
  assert.equal(trigger.getAttribute('aria-expanded'), 'false');
  assert.equal(env.document.activeElement, trigger);
  assert.deepEqual(f.errors, []);
});

test('UI-kit select uses the same touch toggle, focus return, and popup styling as MainSelect', async () => {
  await mount(uiKitSelect());
  const trigger = n('#sort-trigger');
  assert.equal(trigger.classList.contains('ui-select-trigger'), true);
  assert.equal(trigger.classList.contains('ui-kit-select-trigger'), true);
  await touchTap(trigger, 61);
  assert.equal(n('.ui-select-content').classList.contains('ui-kit-select-content'), true);
  assert.equal(trigger.getAttribute('aria-expanded'), 'true');
  await touchTap(trigger, 62);
  assert.equal(env.document.querySelector('.ui-select-content'), null);
  assert.equal(trigger.getAttribute('aria-expanded'), 'false');
  assert.equal(env.document.activeElement, trigger);
});

test('UI-kit select consumes only the retargeted touch click and leaves other controls active', async () => {
  await mount(uiKitSelect());
  const trigger = n('#sort-trigger');
  await touchTap(trigger, 71);
  await touchRetargetedTap(env.document.body, trigger, 72);
  assert.equal(env.document.querySelector('.ui-select-content'), null);
  assert.equal(env.document.activeElement, trigger);
  let outsideClicks = 0;
  const outside = env.document.createElement('button');
  outside.addEventListener('click', () => outsideClicks++);
  env.document.body.append(outside);
  await click(outside);
  assert.equal(outsideClicks, 1);
});

test('controlled UI-kit select reports close requests while its open prop remains authoritative', async () => {
  const requests = [];
  await mount(el(api.Select, {open: true, defaultValue: 'number', onOpenChange: (next) => requests.push(next)},
    el(api.SelectTrigger, {id: 'controlled-sort-trigger', 'aria-label': 'Sort'}, el(api.SelectValue)),
    el(api.SelectContent, null, el(api.SelectItem, {value: 'number'}, 'By number'))));
  const trigger = n('#controlled-sort-trigger');
  await click(trigger);
  assert.equal(trigger.getAttribute('aria-expanded'), 'true', 'a controlled parent still owns the displayed state');
  await click(trigger);
  assert.deepEqual(requests, [false, false], 'a rejected close request does not corrupt the next interaction');
});

test('UI-kit and native form selects share one open popup and keyboard selection flow', async () => {
  await mount(el('div', null, uiKitSelect(), select()));
  n('#sort-trigger').getBoundingClientRect = () => new env.window.DOMRect(100, 50, 150, 44);
  await click('#sort-trigger');
  assert.equal(env.document.querySelectorAll('.ui-select-content').length, 1);
  await click('.mizuki-select-trigger');
  assert.equal(env.document.querySelectorAll('.ui-select-content').length, 1, 'opening a second adapter closes the first popup');
  await React.act(async () => n('#hostSelect-trigger').dispatchEvent(new env.window.KeyboardEvent('keydown', {key: 'Escape', bubbles: true, cancelable: true})));
  assert.equal(env.document.querySelector('.ui-select-content'), null);
  await React.act(async () => n('#sort-trigger').dispatchEvent(new env.window.KeyboardEvent('keydown', {key: 'ArrowDown', bubbles: true, cancelable: true})));
  assert.ok(n('.ui-select-content'));
  const nameOption = [...env.document.querySelectorAll('[role="option"]')].find((option) => option.textContent.includes('By name'));
  assert.ok(nameOption);
  nameOption.getBoundingClientRect = () => new env.window.DOMRect(300, 120, 120, 40);
  await touchTap(nameOption, 81);
  assert.equal(env.document.querySelector('.ui-select-content'), null);
  assert.match(n('#sort-trigger').textContent, /By name/);
});

test('Changing touch opacity does not clear the editor layout or reset its opacity style', async () => {
  const f = await editor(), control = n('#touchBomb');
  const layoutX = control.style.getPropertyValue('--touch-layout-x');
  const classObserver = new env.window.MutationObserver(() => {}), styleObserver = new env.window.MutationObserver(() => {});
  classObserver.observe(f.player, {attributes: true, attributeFilter: ['class'], attributeOldValue: true});
  styleObserver.observe(f.player, {attributes: true, attributeFilter: ['style'], attributeOldValue: true});
  await React.act(async () => {f.settings.setOption('touchControlOpacity', 75); await Promise.resolve();});
  const classChanges = classObserver.takeRecords(), styleChanges = styleObserver.takeRecords();
  classObserver.disconnect(); styleObserver.disconnect();
  assert.equal(f.player.classList.contains('touch-layout-custom'), true);
  assert.equal(f.player.style.getPropertyValue('--touch-control-opacity'), '0.75');
  assert.equal(control.style.getPropertyValue('--touch-layout-x'), layoutX);
  assert.deepEqual(classChanges, [], 'an option update must not remove and reapply the custom-layout class');
  assert.ok(styleChanges.every(change => change.oldValue?.includes('--touch-control-opacity:')),
    'the opacity style should update directly without briefly falling back to its default');
  assert.deepEqual(f.errors, []);
});

test('Keyboard-style workbench collapse closes the real fullscreen menu and expansion does not resurrect it', async () => {
  const f = await editor(), menu = await openMovementMenu();
  assert.equal(menu.closest('[data-radix-popper-content-wrapper]').parentElement, f.player); assert.equal(!menu.isConnected, false);
  assert.equal(menu.getAttribute('data-ui-surface'), 'application');
  await pointer('pointerdown', menu, 81, 120);
  assert.equal(n('#touchSensitivityPreview').classList.contains('active'), false, 'application portal input is not treated as canvas preview');
  await pointer('pointerup', menu, 81, 120);
  // HTMLElement.click emits no pointerdown: the workbench itself must dismiss
  // the detached menu, rather than relying on the document pointer listener.
  await click('#touchLayoutCollapse');
  assert.equal(n('#touchWorkbenchBody').hidden, true); assert.equal(!menu.isConnected, true);
  assert.equal(n('#touchMovementMode').closest('.mizuki-select').querySelector('.mizuki-select-trigger').getAttribute('aria-expanded'), 'false');
  await click('#touchLayoutCollapse'); assert.equal(n('#touchWorkbenchBody').hidden, false); assert.equal(!menu.isConnected, true);
  const reopened = await openMovementMenu();
  assert.equal(reopened.isConnected, true); assert.equal(env.document.querySelectorAll('.ui-select-content').length, 1); assert.deepEqual(f.errors, []);
});

test('Game-marked surface remains an active touch-layout preview target', async () => {
  const f = await editor(), surface = n('#gameViewport');
  surface.setAttribute('data-ui-surface', 'game');
  await pointer('pointerdown', surface, 420, 240);
  assert.equal(n('#touchSensitivityPreview').classList.contains('active'), true);
  await pointer('pointerup', env.document, 420, 240);
  assert.equal(n('#touchSensitivityPreview').classList.contains('active'), false);
  assert.deepEqual(f.errors, []);
});

test('Restoring the workbench after viewport adjustment closes its detached select', async () => {
  const f = await editor();
  await click('#touchViewportAdjust'); assert.equal(n('#touchLayoutEditor').hidden, true);
  // A retained/programmatically reopened menu must follow main's explicit
  // syncTouchLayoutWorkbench close when viewport editing finishes.
  const menu = await openMovementMenu(); assert.equal(!menu.isConnected, false);
  await click('#touchViewportDone'); assert.equal(n('#touchLayoutEditor').hidden, false); assert.equal(!menu.isConnected, true);
  assert.deepEqual(f.errors, []);
});

async function pointer(type, target, x, y, pointerId = 7) {
  await React.act(async () => target.dispatchEvent(new env.window.PointerEvent(type, {bubbles: true, pointerId, pointerType: 'mouse', button: 0, clientX: x, clientY: y})));
}
for (const reason of ['hidden', 'fullscreen']) test(`Main editor cancels an active control drag on ${reason} transition`, async () => {
  const f = await editor();
  await pointer('pointerdown', n('#touchBomb'), 175, 225);
  await pointer('pointermove', env.document, 207, 241);
  assert.equal(f.player.classList.contains('touch-layout-manipulating'), true);
  const before = structuredClone(f.model.getSnapshot().draft), descriptor = Object.getOwnPropertyDescriptor(env.document, 'hidden');
  try {
    if (reason === 'hidden') Object.defineProperty(env.document, 'hidden', {configurable: true, value: true});
    await React.act(async () => env.document.dispatchEvent(new env.window.Event(reason === 'hidden' ? 'visibilitychange' : 'fullscreenchange')));
    assert.equal(f.player.classList.contains('touch-layout-manipulating'), false);
    await pointer('pointermove', env.document, 300, 320);
    assert.deepEqual(f.model.getSnapshot().draft, before, 'a retired pointer cannot continue moving the draft');
    await pointer('pointerup', env.document, 300, 320);
    assert.deepEqual(f.errors, []);
  } finally {if (descriptor) Object.defineProperty(env.document, 'hidden', descriptor); else delete env.document.hidden;}
});

test('A visibility event while still visible retains main’s active drag', async () => {
  const f = await editor(), descriptor = Object.getOwnPropertyDescriptor(env.document, 'hidden');
  try {
    Object.defineProperty(env.document, 'hidden', {configurable: true, value: false});
    await pointer('pointerdown', n('#touchBomb'), 175, 225);
    const before = structuredClone(f.model.getSnapshot().draft);
    await React.act(async () => env.document.dispatchEvent(new env.window.Event('visibilitychange')));
    await pointer('pointermove', env.document, 207, 241);
    assert.notDeepEqual(f.model.getSnapshot().draft, before);
    await pointer('pointerup', env.document, 207, 241);
    assert.deepEqual(f.errors, []);
  } finally {if (descriptor) Object.defineProperty(env.document, 'hidden', descriptor); else delete env.document.hidden;}
});

test('Same-orientation resize clamps the current workbench position rather than resetting it', async () => {
  const f = await editor(), panel = n('#touchLayoutEditor');
  panel.style.left = '120px'; panel.style.top = '140px';
  panel.getBoundingClientRect = () => new env.window.DOMRect(parseFloat(panel.style.left), parseFloat(panel.style.top), 260, 400);
  await React.act(async () => env.window.dispatchEvent(new env.window.Event('resize')));
  assert.equal(panel.style.left, '120px'); assert.equal(panel.style.top, '140px');
  panel.style.left = '1400px'; panel.style.top = '900px';
  await React.act(async () => env.window.dispatchEvent(new env.window.Event('resize')));
  assert.equal(panel.style.left, '1004px'); assert.equal(panel.style.top, '384px');
  assert.deepEqual(f.errors, []);
});

test('Safe-zone resize and orientation events refresh the real draft; teardown releases both observers', async () => {
  const previousObserver = globalThis.ResizeObserver, orientationDescriptor = Object.getOwnPropertyDescriptor(env.window.screen, 'orientation');
  const width = Object.getOwnPropertyDescriptor(env.document.documentElement, 'clientWidth'), height = Object.getOwnPropertyDescriptor(env.document.documentElement, 'clientHeight');
  const observed = [], orientation = new env.window.EventTarget();
  globalThis.ResizeObserver = class {constructor(callback) {this.callback = callback; observed.push(this);} observe(node) {this.node = node;} unobserve(node) {if(this.node === node) this.disconnected = true;} disconnect() {this.disconnected = true;}};
  Object.defineProperty(env.window.screen, 'orientation', {configurable: true, value: orientation});
  try {
    const f = await editor(), observer = observed.findLast(value => value.node === n('#touchLayoutSafeZone') && !value.disconnected);
    assert.equal(observer.node, n('#touchLayoutSafeZone'));
    let revision = f.model.getSnapshot().revision;
    await React.act(async () => observer.callback([]));
    assert.ok(f.model.getSnapshot().revision > revision);
    Object.defineProperty(env.document.documentElement, 'clientWidth', {configurable: true, value: 800});
    Object.defineProperty(env.document.documentElement, 'clientHeight', {configurable: true, value: 1280});
    await React.act(async () => orientation.dispatchEvent(new env.window.Event('change')));
    assert.equal(f.model.getSnapshot().orientation, 'portrait');
    assert.ok(f.model.getSnapshot().draft.profiles.portrait);
    await React.act(async () => root.unmount()); root = null;
    assert.ok(observed.every(value => value.disconnected), 'including the StrictMode-replayed owner');
    revision = f.model.getSnapshot().revision;
    await React.act(async () => {observer.callback([]); orientation.dispatchEvent(new env.window.Event('change'));});
    assert.equal(f.model.getSnapshot().revision, revision, 'retired callbacks cannot mutate the draft');
    assert.deepEqual(f.errors, []);
  } finally {
    globalThis.ResizeObserver = previousObserver;
    if (orientationDescriptor) Object.defineProperty(env.window.screen, 'orientation', orientationDescriptor); else delete env.window.screen.orientation;
    for (const [name, descriptor] of [['clientWidth', width], ['clientHeight', height]]) {if (descriptor) Object.defineProperty(env.document.documentElement, name, descriptor); else delete env.document.documentElement[name];}
  }
});


test('Viewport Done restores the current orientation’s saved workbench position after a hidden rotation', async () => {
  const values = new Map(), storage = {getItem: key => values.get(key) ?? null, setItem: (key, value) => values.set(key, value)};
  const f = await editor({storage}), panel = n('#touchLayoutEditor');
  const width = Object.getOwnPropertyDescriptor(env.document.documentElement, 'clientWidth'), height = Object.getOwnPropertyDescriptor(env.document.documentElement, 'clientHeight');
  panel.getBoundingClientRect = () => panel.hidden ? new env.window.DOMRect(0, 0, 0, 0) : new env.window.DOMRect(parseFloat(panel.style.left), parseFloat(panel.style.top), 260, 400);
  panel.style.left = '120px'; panel.style.top = '140px';
  f.model.windowPositions.set('portrait', 'editor', {x: .25, y: .5});
  try {
    await click('#touchViewportAdjust');
    Object.defineProperty(env.document.documentElement, 'clientWidth', {configurable: true, value: 800});
    Object.defineProperty(env.document.documentElement, 'clientHeight', {configurable: true, value: 1280});
    await React.act(async () => env.window.dispatchEvent(new env.window.Event('resize')));
    assert.equal(f.model.getSnapshot().orientation, 'portrait'); assert.equal(panel.hidden, true);
    await click('#touchViewportDone');
    assert.equal(panel.hidden, false);
    assert.equal(panel.style.left, '143px'); assert.equal(panel.style.top, '440px');
    assert.deepEqual(f.errors, []);
  } finally {
    for (const [name, descriptor] of [['clientWidth', width], ['clientHeight', height]]) {if (descriptor) Object.defineProperty(env.document.documentElement, name, descriptor); else delete env.document.documentElement[name];}
  }
});

test('TH075 layout is independent from shared shooters and persists across game switches', () => {
  const values=new Map(),storage={getItem:k=>values.get(k)??null,setItem:(k,v)=>values.set(k,v),removeItem:k=>values.delete(k)};
  const defaults={controls:Object.fromEntries(api.touchLayoutControlNames.map(name=>[name,{x:.2,y:.3,scale:1,priority:api.touchLayoutControlMeta[name].priority}])),viewport:{x:0}};
  const model=api.createTouchLayoutModel({storage});model.hydrate();
  model.begin('landscape',defaults,['joystick']);model.updateControl('joystick',{x:.25});model.save();model.discard();
  const shared=values.get('eagler-touhou-touch-layout-v1');
  model.setGame('th075');assert.equal(model.getSnapshot().saved,null);
  model.begin('landscape',defaults,['joystick']);model.updateControl('joystick',{x:.7,y:.6});model.save();model.discard();
  assert.equal(values.get('eagler-touhou-touch-layout-v1'),shared);assert.ok(values.has('eagler-touhou-touch-layout-v1-th075'));
  model.setGame('th06');assert.equal(model.getSnapshot().saved.profiles.landscape.controls.joystick.x,.25);
  model.setGame('th07');assert.equal(model.getSnapshot().saved.profiles.landscape.controls.joystick.x,.25);
  model.setGame('th075');assert.equal(model.getSnapshot().saved.profiles.landscape.controls.joystick.x,.7);
  const reopened=api.createTouchLayoutModel({storage});reopened.hydrate();reopened.setGame('th075');
  assert.equal(reopened.getSnapshot().saved.profiles.landscape.controls.joystick.x,.7);
});

test('responsive defaults resample within an orientation while edited and saved layouts keep their positions', () => {
  const values=new Map(),storage={getItem:k=>values.get(k)??null,setItem:(k,v)=>values.set(k,v),removeItem:k=>values.delete(k)};
  const defaults=y=>({controls:Object.fromEntries(api.touchLayoutControlNames.map(name=>[name,{x:.2,y,scale:1,priority:api.touchLayoutControlMeta[name].priority}])),viewport:{x:0}});
  const model=api.createTouchLayoutModel({storage});model.hydrate();
  model.begin('landscape',defaults(.8),['bomb']);
  assert.equal(model.getSnapshot().dirty,false,'opening an unsaved default is a preview');
  model.ensureOrientation('landscape',defaults(.6));
  model.ensureOrientation('portrait',defaults(.7));
  assert.equal(model.getSnapshot().dirty,false,'resizing and rotating an untouched preview do not request discard');
  model.ensureOrientation('landscape',defaults(.6));
  assert.equal(model.getSnapshot().draft.profiles.landscape.controls.bomb.y,.6);
  assert.equal(values.size,0,'resizing a default preview never writes an override');
  model.updateControl('bomb',{y:.4,scale:1.5});
  assert.equal(model.getSnapshot().dirty,true,'manual placement changes still require save or discard');
  model.ensureOrientation('landscape',defaults(.3));
  assert.equal(model.getSnapshot().draft.profiles.landscape.controls.bomb.y,.4);
  assert.equal(model.getSnapshot().draft.profiles.landscape.controls.bomb.scale,1.5);
  model.ensureOrientation('portrait',defaults(.7));model.ensureOrientation('portrait',defaults(.5));
  assert.equal(model.getSnapshot().draft.profiles.portrait.controls.bomb.y,.5);
  model.save();model.discard();model.begin('landscape',defaults(.2),['bomb']);
  assert.equal(model.getSnapshot().dirty,false,'saved layouts reopen cleanly');
  model.ensureOrientation('landscape',defaults(.1));
  assert.equal(model.getSnapshot().draft.profiles.landscape.controls.bomb.y,.4);
  model.ensureOrientation('portrait',defaults(.1));
  assert.equal(model.getSnapshot().draft.profiles.portrait.controls.bomb.y,.5);
});
