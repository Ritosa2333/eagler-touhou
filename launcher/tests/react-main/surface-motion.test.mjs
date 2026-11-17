import {test, beforeAll as before, afterAll as after, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom} from './mounted-dom-environment.mjs';
import {installNativeTimeline} from './native-animation-environment.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));
let env, api, React, createRoot, root, work, reduced = false;
const native = [], handles = [], listeners = new Set(), observers = new Set();
const act = action => React.act(async () => {action(); await Promise.resolve();});
const flush = async () => {await Promise.resolve(); await Promise.resolve(); await Promise.resolve();};
function surface() {
  const element = env.document.createElement('div'); env.document.body.append(element);
  element.getBoundingClientRect = () => {throw new Error('Surface motion must not measure layout');};
  return element;
}
function animate(element, phase, options) {const result = api.animateSurface(element, phase, options); if (result) handles.push(result); return result;}
function finish(element) {for (const record of native.filter(item => item.element === element && item.playState === 'running')) record.finish();}

before(async () => {
  env = installMountedDom();
  env.window.matchMedia = () => ({get matches() {return reduced;}, addEventListener: (_, fn) => listeners.add(fn), removeEventListener: (_, fn) => listeners.delete(fn)});
  const Observer = env.window.MutationObserver;
  env.window.MutationObserver = class extends Observer {
    constructor(fn) {super(fn); observers.add(this);}
    disconnect() {super.disconnect(); observers.delete(this);}
  };
  const computed = env.window.getComputedStyle.bind(env.window);
  env.window.getComputedStyle = element => new Proxy(computed(element), {get(style, name) {
    const record = [...native].reverse().find(item => item.element === element && item.playState === 'running' && item.values[name] !== undefined);
    if (record) return record.values[name];
    const value = Reflect.get(style, name); return typeof value === 'function' ? value.bind(style) : value;
  }});
  installNativeTimeline(env.window, native);
  work = await mkdtemp(resolve(project, '.cache/surface-motion-'));
  const outfile = resolve(work, 'fixture.mjs');
  await build({absWorkingDir: project, stdin: {resolveDir: project, loader: 'tsx', contents: `
    import {useRef} from 'react';
    import {useSurfaceMotion} from './app/ui/use-surface-motion';
    export * from './app/ui/surface-motion';
    export function Fixture({phase, reduced, complete, label}) {
      const ref=useRef(null);useSurfaceMotion(ref,phase,{kind:'panel',reduced,onComplete:complete});
      return <div ref={ref} data-testid="surface">{label}</div>;
    }
  `}, outfile, bundle: true, platform: 'node', format: 'esm', packages: 'external', jsx: 'automatic', logLevel: 'silent'});
  api = await import(pathToFileURL(outfile).href);
  React = await import('react'); ({createRoot} = await import('react-dom/client'));
});
afterEach(async () => {
  if (root) await act(() => root.unmount()); root = null;
  for (const handle of handles.splice(0)) handle.cancel(); await flush();
  assert.equal(listeners.size, 0); assert.equal(observers.size, 0);
  native.length = 0; reduced = false; env.document.body.replaceChildren(); env.document.body.className = '';
  assert.deepEqual(env.errors.splice(0), []);
});
after(async () => {env.close(); await rm(work, {recursive: true, force: true});});

test('window phases share the scale recipe while preserving the centering transform', async () => {
  const element = surface(); element.style.transform = 'translate(-50%, -50%)';
  element.style.setProperty('--ui-window-enter-duration', '0.24s'); element.style.setProperty('--ui-window-enter-easing', 'cubic-bezier(.1,.2,.3,1)');
  const entered = animate(element, 'enter', {kind: 'dialog'});
  assert.ok(entered); assert.equal(native.length, 3);
  assert.deepEqual(native[0].keyframes, {opacity: ['0', '1']}); assert.deepEqual(native[1].keyframes, {translate: ['0 6px', '0 0']});
  assert.deepEqual(native[2].keyframes, {scale: ['.965', '1']});
  assert.equal(native[0].options.duration, 240); assert.equal(native[0].options.easing, 'cubic-bezier(0.1, 0.2, 0.3, 1)');
  finish(element); await entered.finished;
  assert.equal(element.style.opacity, ''); assert.equal(element.style.translate, ''); assert.equal(element.style.scale, ''); assert.equal(element.style.transform, 'translate(-50%, -50%)');
  const exited = animate(element, 'exit', {kind: 'dialog'});
  assert.deepEqual(native.at(-3).keyframes, {opacity: ['1', '0']});
  assert.deepEqual(native.at(-1).keyframes, {scale: ['1', '.965']});
  assert.equal(native.at(-1).options.duration, 150);
  finish(element); await exited.finished; assert.equal(element.style.opacity, '');
  element.style.setProperty('--ui-window-scrim-exit-easing', 'cubic-bezier(.42,0,.58,1)');
  const scrim = animate(element, 'exit', {kind: 'backdrop'});
  assert.equal(native.at(-1).options.duration, 150);
  assert.equal(native.at(-1).options.easing, 'cubic-bezier(0.42, 0, 0.58, 1)');
  finish(element); await scrim.finished;
});


test('interrupted close and reopen continue from current opacity and retire stale completion', async () => {
  const element = surface(), entrance = animate(element, 'enter'); native.at(-1).values.opacity = '.4';
  const exit = animate(element, 'exit', {fromCurrent: true});
  assert.deepEqual(native.at(-1).keyframes, {opacity: ['.4', '0']}); await entrance.finished;
  native.at(-1).values.opacity = '.2';
  const reopen = animate(element, 'enter', {fromCurrent: true}); await exit.finished;
  assert.deepEqual(native.at(-1).keyframes, {opacity: ['.2', '1']});
  assert.equal(native.filter(item => item.playState === 'running').length, 1);
  entrance.cancel(); exit.cancel(); assert.equal(native.at(-1).playState, 'running');
  finish(element); await reopen.finished;
});

test('a phone options panel fades without moving and reverses from its current opacity', async () => {
  const element = surface();
  const enter = animate(element, 'enter', {kind: 'panel'});
  assert.equal(native.length, 1);
  assert.deepEqual(native[0].keyframes, {opacity: ['0', '1']});
  native[0].values.opacity = '.6';
  const exit = animate(element, 'exit', {kind: 'panel', fromCurrent: true});
  await enter.finished;
  assert.equal(native.length, 2);
  assert.equal(native[0].playState, 'idle');
  assert.deepEqual(native[1].keyframes, {opacity: ['.6', '0']});
  finish(element); await exit.finished;
  assert.equal(element.style.opacity, '');
});

test('cancellation restores owned inline properties and priorities', async () => {
  const element = surface(); element.style.setProperty('opacity', '.8', 'important'); element.style.translate = '1px 2px';
  const animation = animate(element, 'enter', {kind: 'menu'}); animation.cancel(); await animation.finished;
  assert.equal(element.style.opacity, '0.8'); assert.equal(element.style.getPropertyPriority('opacity'), 'important');
  assert.equal(element.style.translate, '1px 2px'); assert.ok(native.every(item => item.playState === 'idle'));
});

test('chat movement and expiry keep independent property owners and clean them on reduced motion', async () => {
  const element=surface();element.style.transform='translate(-50%, -50%)';element.style.translate='2px 3px';
  const fade=api.animateUiFrames(element,{opacity:['1','0']});handles.push(fade);
  const move=api.animateUiFrames(element,{translate:['0 12px','0 0']});handles.push(move);
  assert.ok(fade);assert.ok(move);
  assert.equal(native.filter(item=>item.playState==='running').length,2);
  move.cancel();await move.finished;
  assert.equal(native.filter(item=>item.playState==='running').length,1);
  assert.equal(element.style.translate,'2px 3px');
  assert.equal(element.style.transform,'translate(-50%, -50%)');
  reduced=true;for(const listener of listeners)listener();await fade.finished;
  assert.ok(native.every(item=>item.playState==='idle'));
  assert.equal(element.style.opacity,'');assert.equal(listeners.size,0);
});

test('a new expiry retires overlapping movement while preserving its completed position', async () => {
  const element=surface();
  const movement=api.animateUiFrames(element,{opacity:['0','1'],translate:['0 8px','0 0']});handles.push(movement);
  const expiry=api.animateUiFrames(element,{opacity:['1','0']});handles.push(expiry);
  await movement.finished;
  assert.equal(native.filter(item=>item.playState==='running').length,1);
  assert.equal(element.style.translate,'');
  finish(element);await expiry.finished;
  assert.equal(element.style.opacity,'');
});

test('hidden surfaces cancel without committing styles and cannot apply a stale finish', async () => {
  const element = surface(); element.style.opacity = '.8';
  const animation = animate(element, 'enter', {kind: 'dialog'});
  const records = native.filter(item => item.node === element);
  for (const record of records) record.commitStyles = () => {throw new env.window.DOMException('Target element is not rendered', 'InvalidStateError');};
  element.hidden = true;
  assert.doesNotThrow(() => animation.cancel()); await animation.finished;
  assert.ok(records.every(item => item.cancelled && item.onfinish === null));
  assert.equal(element.style.opacity, '0.8'); assert.equal(element.style.translate, '');
  element.hidden = false;
  const reopened = animate(element, 'enter');
  for (const record of records) record.finish();
  await flush(); assert.equal(native.at(-1).playState, 'running');
  finish(element); await reopened.finished;
});

test('multiple surfaces share one preference listener and observer until the last finishes', async () => {
  const first = surface(), second = surface(); const a = animate(first, 'enter'), b = animate(second, 'enter');
  assert.equal(listeners.size, 1); assert.equal(observers.size, 1);
  finish(first); await a.finished; assert.equal(listeners.size, 1);
  finish(second); await b.finished; assert.equal(listeners.size, 0); assert.equal(observers.size, 0);
});

test('enabling system reduced motion completes all active surfaces immediately', async () => {
  const a = animate(surface(), 'enter'), b = animate(surface(), 'exit');
  reduced = true; for (const changed of [...listeners]) changed();
  await Promise.all([a.finished, b.finished]); assert.ok(native.every(item => item.playState === 'idle'));
});

test('enabling the site less-motion preference also completes active surfaces', async () => {
  const animation = animate(surface(), 'exit'); env.document.body.classList.add('less-motion'); await animation.finished;
  assert.ok(native.every(item => item.playState === 'idle'));
});

test('zero duration, reduced motion and missing WAAPI use immediate presentation', () => {
  const element = surface(); element.style.setProperty('--ui-surface-duration', '0ms'); assert.equal(animate(element, 'exit'), null);
  element.style.removeProperty('--ui-surface-duration'); reduced = true; assert.equal(animate(element, 'enter'), null);
  reduced = false; assert.equal(animate(element, 'enter', {reduced: true}), null);
  element.animate = undefined; assert.equal(animate(element, 'enter'), null); assert.equal(native.length, 0);
});

test('invalid timing tokens use shared defaults', () => {
  const element = surface(); element.style.setProperty('--ui-surface-duration', 'invalid'); element.style.setProperty('--ui-surface-easing', 'cubic-bezier(2,0,3,1)');
  assert.deepEqual(api.surfaceMotionTokens(element), {duration: 180, ease: [.2, 0, 0, 1], distance: 0, scale: undefined});
});

test('a partially failed native start releases created effects and original styles', async () => {
  const element = surface(), original = element.animate;
  element.style.opacity = '.8'; let calls = 0;
  element.animate = function (...args) {if (++calls === 2) throw new Error('partial WAAPI'); return original.apply(this, args);};
  assert.equal(animate(element, 'enter', {kind: 'dialog'}), null);
  await flush();assert.equal(native.length, 1);assert.equal(native[0].cancelled, true);assert.equal(element.style.opacity, '0.8');
});

test('the content recipe keeps data refresh local and opacity-only', async () => {
  const element=surface(), animation=animate(element,'enter',{kind:'content'});
  assert.equal(native.length,1);assert.deepEqual(native[0].keyframes,{opacity:['.25','1']});
  finish(element);await animation.finished;
});

test('hook ignores unrelated renders, replaces interrupted motion and cleans up on unmount', async () => {
  const host = env.document.createElement('div'); env.document.body.append(host); root = createRoot(host);
  const completed = [], complete = () => completed.push('done');
  const render = (phase, label = 'one') => act(() => root.render(React.createElement(api.Fixture, {phase, label, complete})));
  await render('enter'); assert.equal(native.length, 1);
  await render('enter', 'two'); assert.equal(native.length, 1);
  await render('exit'); assert.equal(native.length, 2); assert.deepEqual(completed, []);
  await render('enter'); assert.equal(native.length, 3); assert.deepEqual(completed, []);
  await act(() => finish(host.firstChild)); assert.deepEqual(completed, ['done']);
  await render('exit'); await act(() => root.unmount()); root = null;
  assert.deepEqual(completed, ['done']); assert.ok(native.every(item => item.playState === 'idle'));
});
