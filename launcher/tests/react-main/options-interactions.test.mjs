/** Synthetic mounted DOM interaction contracts only, not browser/layout evidence. */
import {test, beforeAll as before, afterAll as after, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom} from './mounted-dom-environment.mjs';
const project = fileURLToPath(new URL('../../', import.meta.url));
let env, work, React, createRoot, owners, root;
before(async () => {
  env = installMountedDom(); React = await import('react'); ({createRoot} = await import('react-dom/client'));
  // Explicit visible geometry fixture, not rendered browser geometry.
  env.window.HTMLElement.prototype.getClientRects = function () {return this.closest('[hidden]') ? [] : [this.getBoundingClientRect()];};
  work = await mkdtemp(resolve(project, '.cache/options-interactions-'));
  const output = resolve(work, 'owners.mjs');
  await build({absWorkingDir: project, stdin: {resolveDir: project, loader: 'ts', contents: `
    export {OptionsBackdrop} from './app/components/launcher/OptionsBackdrop.tsx';
    export {useOptionsInteractions} from './app/components/launcher/use-options-interactions.ts';
    export {useOptionsViewport} from './app/components/launcher/use-options-viewport.ts';
    export {OptionsOwnership, useOptionsTarget} from './app/components/launcher/options-ownership.ts';
    export {RoomSettingsDrawer} from './app/components/room/RoomSettingsDrawer.tsx';
    export {LocaleProvider} from './app/i18n.tsx';
  `}, bundle: true, platform: 'node', format: 'esm', jsx: 'automatic', outfile: output, packages: 'external', logLevel: 'silent'});
  owners = await import(pathToFileURL(output).href);
});
afterEach(async () => {if (root) await React.act(async () => root.unmount()); root = null; assert.deepEqual(env.errors, []); env.document.body.replaceChildren();});
after(async () => {env?.close(); if (work) await rm(work, {recursive: true, force: true});});
async function render(children) {
  if (!root) {const node = env.document.createElement('div'); env.document.body.append(node); root = createRoot(node);}
  await React.act(async () => root.render(React.createElement(owners.LocaleProvider, {locale: 'en'}, children)));
}
function key(node, key, shiftKey = false) {const event = new env.window.KeyboardEvent('keydown', {key, shiftKey, bubbles: true, cancelable: true}); node.dispatchEvent(event); return event;}

test('independent shared backdrops suppress wheel and route dismissal to their own host after rerender', async () => {
  const calls = [];
  await render([React.createElement(owners.OptionsBackdrop, {key: 'a', onBack: () => calls.push('a')}), React.createElement(owners.OptionsBackdrop, {key: 'b', onBack: () => calls.push('b')})]);
  const [a, b] = env.document.querySelectorAll('.library-backdrop');
  for (const node of [a, b]) {const wheel = new env.window.WheelEvent('wheel', {cancelable: true}); node.dispatchEvent(wheel); assert.equal(wheel.defaultPrevented, true);}
  a.click(); b.click(); assert.deepEqual(calls, ['a', 'b']);
  await render(React.createElement(owners.OptionsBackdrop, {key: 'b', onBack: () => calls.push('reopened')}));
  env.document.querySelector('.library-backdrop').click(); assert.deepEqual(calls, ['a', 'b', 'reopened']);
  const wheel = new env.window.WheelEvent('wheel', {cancelable: true}); a.dispatchEvent(wheel); assert.equal(wheel.defaultPrevented, false, 'unmounted listener released');
});

for (const carrier of ['options', 'room']) test(`${carrier} shares boundary focus cycling, disabled filtering and native relocated-key ownership`, async () => {
  let closes = 0;
  function Panel() {const ref = React.useRef(null); owners.useOptionsInteractions(ref, true, () => closes++); return React.createElement('aside', {ref, id: 'fixture-panel'});}
  await render(carrier === 'room' ? React.createElement(owners.RoomSettingsDrawer, {open: true, roomOpen: true, onCloseRequest: () => closes++}) : React.createElement(Panel));
  const node = env.document.querySelector(carrier === 'room' ? '#mpSettingsRoomDrawer' : '#fixture-panel');
  // Native DOM relocation has no React event ancestry under the destination.
  const controls = env.document.createElement('div');
  controls.innerHTML = '<button id="first">First</button><button disabled>Disabled</button><div hidden><button>Hidden</button></div><button style="visibility:hidden">Invisible</button><textarea id="middle"></textarea><div tabindex="2" id="last">Last</div>';
  node.append(controls);
  const first = controls.querySelector('#first'), middle = controls.querySelector('#middle'), last = controls.querySelector('#last');
  first.focus(); assert.equal(key(first, 'Tab', true).defaultPrevented, true); assert.equal(env.document.activeElement, last);
  last.focus(); assert.equal(key(last, 'Tab').defaultPrevented, true); assert.equal(env.document.activeElement, first);
  middle.focus(); assert.equal(key(middle, 'Tab').defaultPrevented, false);
  assert.equal(key(middle, 'Escape').defaultPrevented, true); assert.equal(closes, 1);
});

test('scoped registration isolates multiple owners and cleans local refs without clearing newer nodes', async () => {
  const a = {panel: null, selectedCard: null}, b = {panel: null, selectedCard: null};
  const locals = [], callbacks = [];
  function Port({index}) {const ref = React.useRef(null); locals[index] = ref; const register = owners.useOptionsTarget('panel', ref); callbacks[index] = register; return React.createElement('aside', {ref: register, 'data-port': index});}
  await render([React.createElement(owners.OptionsOwnership, {key: 'a', value: a}, React.createElement(Port, {index: 0})), React.createElement(owners.OptionsOwnership, {key: 'b', value: b}, React.createElement(Port, {index: 1}))]);
  assert.notEqual(a.panel, b.panel); assert.equal(a.panel.dataset.port, '0'); assert.equal(b.panel.dataset.port, '1');
  const older = env.document.createElement('aside'), newer = env.document.createElement('aside');
  const retireOlder = callbacks[0](older), retireNewer = callbacks[0](newer);
  retireOlder(); assert.equal(a.panel, newer); assert.equal(locals[0].current, newer);
  retireNewer(); assert.equal(a.panel, null); assert.equal(locals[0].current, null);
  await render(null); assert.equal(b.panel, null); assert.equal(locals[1].current, null);
});

test('options keep content-gutter width through modal resize and close, while respecting pinch zoom', async () => {
  const viewportDescriptor=Object.getOwnPropertyDescriptor(env.window,'visualViewport');
  const bodyDescriptor=Object.getOwnPropertyDescriptor(env.document.body,'getBoundingClientRect');
  const viewport=Object.assign(new env.window.EventTarget(),{width:390.25,height:844,scale:1,offsetTop:0,offsetLeft:0});
  let contentWidth=380.25;
  Object.defineProperty(env.window,'visualViewport',{configurable:true,value:viewport});
  env.document.body.getBoundingClientRect=()=>new env.window.DOMRect(0,0,contentWidth,viewport.height);
  function Fixture(){const ref=React.useRef(null);owners.useOptionsViewport(ref);return React.createElement('aside',{ref,id:'viewport-fixture'});}
  const resize=()=>React.act(async()=>{viewport.dispatchEvent(new env.window.Event('resize'));await new Promise(resolve=>env.window.requestAnimationFrame(()=>env.window.requestAnimationFrame(resolve)));});
  try {
    await render(React.createElement(Fixture));const panel=env.document.querySelector('#viewport-fixture');
    assert.equal(panel.style.getPropertyValue('--options-view-width'),'380.25px');
    contentWidth=834.25;viewport.width=844.25;viewport.height=390;
    await resize();assert.equal(panel.style.getPropertyValue('--options-view-width'),'834.25px');
    assert.equal(panel.style.getPropertyValue('--options-view-height'),'390px');
    viewport.width=834.25;await resize();
    assert.equal(panel.style.getPropertyValue('--options-view-width'),'834.25px','closing the modal does not shift the options');
    viewport.scale=2;viewport.width=417;await resize();
    assert.equal(panel.style.getPropertyValue('--options-view-width'),'834.25px','pinch zoom is left to the browser');
  } finally {
    await render(null);
    if(viewportDescriptor)Object.defineProperty(env.window,'visualViewport',viewportDescriptor);else delete env.window.visualViewport;
    if(bodyDescriptor)Object.defineProperty(env.document.body,'getBoundingClientRect',bodyDescriptor);else delete env.document.body.getBoundingClientRect;
  }
});
