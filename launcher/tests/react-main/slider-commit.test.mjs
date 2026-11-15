/** Real Radix event handling under a synthetic DOM; no pixel or Runtime claim. */
import {test, beforeAll as before, afterAll as after, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom} from './mounted-dom-environment.mjs';
const project = fileURLToPath(new URL('../../', import.meta.url));
let env, work, React, createRoot, api, root;
const act = fn => React.act(async () => {fn(); await Promise.resolve();});
before(async () => {
  env = installMountedDom(); React = await import('react'); ({createRoot} = await import('react-dom/client'));
  const captures = new WeakMap();
  env.window.HTMLElement.prototype.setPointerCapture = function (id) {captures.set(this, id);};
  env.window.HTMLElement.prototype.hasPointerCapture = function (id) {return captures.get(this) === id;};
  env.window.HTMLElement.prototype.releasePointerCapture = function () {captures.delete(this);};
  work = await mkdtemp(resolve(project, '.cache/slider-contract-'));
  const outfile = resolve(work, 'fixture.mjs');
  await build({absWorkingDir: project, stdin: {resolveDir: project, loader: 'tsx', contents: `
    import {useState} from 'react';
    import {Slider} from './app/ui/slider';
    export {ReplaySeekBar} from './app/components/player/ReplaySeekBar';
    import {ReplayTransport} from './app/components/player/ReplayTransport';
    export {ReplayTransport};
    export function ReplayRestartFixture({control}) {
      const [playback,setPlayback]=useState({path:'replay/th10_01.rpy',frame:600,total:18000,paused:true,seeking:false,target:null,rate:1,ended:false});
      return <ReplayTransport playback={playback} onToggleFullscreen={()=>{}} onControl={(action,value)=>{control(action,value);if(action==='seek')setPlayback(current=>({...current,frame:value}));}}/>;
    }
    export function OrderedFixture({record}) {
      const [value,setValue]=useState(50);
      return <Slider value={value} min={0} max={100} step={5} aria-label="Ordered" onValueChange={v=>{record('change',v);setValue(v);}} onValueCommit={v=>record('commit',v)}/>;
    }
    export function Fixture({commit, cancel, disabled=false}) {
      const [value,setValue]=useState(50), [saved,setSaved]=useState(50);
      return <Slider min={0} max={100} step={5} value={value} disabled={disabled} aria-label="Sensitivity"
        onValueChange={setValue} onValueCommit={v=>{setSaved(v);commit(v);}}
        onValueCancel={()=>{setValue(saved);cancel();}}/>;
    }
  `}, outfile, bundle: true, packages: 'external', platform: 'node', format: 'esm', jsx: 'automatic', logLevel: 'silent'});
  api = await import(pathToFileURL(outfile).href);
});
afterEach(async () => {if (root) await act(() => root.unmount()); root = null; assert.deepEqual(env.errors.splice(0), []); env.document.body.replaceChildren();});
after(async () => {env.close(); await rm(work, {recursive: true, force: true});});
const n = selector => {const node = env.document.querySelector(selector); assert.ok(node, selector); return node;};
async function mount(component, props) {const host = env.document.createElement('div');env.document.body.append(host);root = createRoot(host);await act(() => root.render(React.createElement(component, props)));}
const pointer = (type, x) => act(() => n('.ui-kit-slider-track').dispatchEvent(new env.window.PointerEvent(type, {bubbles: true, cancelable: true, pointerId: 7, pointerType: 'touch', button: 0, clientX: x, clientY: 10})));
const key = value => act(() => n('[role="slider"]').dispatchEvent(new env.window.KeyboardEvent('keydown', {key: value, bubbles: true, cancelable: true})));

test('drag previews until release, cancellation restores saved value and the next drag can commit', async () => {
  const commits = []; let cancellations = 0;
  await mount(api.Fixture, {commit: value => commits.push(value), cancel: () => cancellations++});
  await pointer('pointerdown', 320); await pointer('pointermove', 960);
  assert.equal(n('[role="slider"]').getAttribute('aria-valuenow'), '75'); assert.deepEqual(commits, []);
  await pointer('pointercancel', 960); await pointer('pointerup', 960);
  assert.equal(cancellations, 1); assert.equal(n('[role="slider"]').getAttribute('aria-valuenow'), '50'); assert.deepEqual(commits, []);
  await pointer('pointerdown', 320); await pointer('pointerup', 320); assert.deepEqual(commits, [25]);
});
test('keyboard changes commit within bounds, while a disabled slider cannot change', async () => {
  const commits = [];
  await mount(api.Fixture, {commit: value => commits.push(value), cancel() {}});
  await key('ArrowRight'); await key('Home'); await key('End'); await key('ArrowRight');
  assert.deepEqual(commits, [55, 0, 100]);
  await act(() => root.render(React.createElement(api.Fixture, {disabled: true, commit: value => commits.push(value), cancel() {}})));
  await key('ArrowLeft'); assert.deepEqual(commits, [55, 0, 100]);
});
test('Replay does not seek while dragging or after cancellation and commits release once', async () => {
  const seeks = [];
  await mount(api.ReplaySeekBar, {frame: 600, total: 18000, disabled: false, onSeek: value => seeks.push(value)});
  await pointer('pointerdown', 640); assert.deepEqual(seeks, []); assert.equal(n('[role="slider"]').getAttribute('aria-valuetext'), '2:30');
  await pointer('pointercancel', 640); await pointer('pointerup', 640); assert.deepEqual(seeks, []);
  assert.equal(n('[role="slider"]').getAttribute('aria-valuetext'), '0:10');
  await pointer('pointerdown', 960); await pointer('pointerup', 960); assert.deepEqual(seeks, [13500]);
});

test('shared Replay transport submits absolute frames, full speed and runtime actions', async () => {
  const actions = [];
  let fullscreen = 0;
  const props = {playback: {path:'replay/th10_01.rpy', frame:1800, total:7200, startFrame:1200, paused:true, seeking:false, target:null, rate:1, ended:false}, onControl:(...args) => actions.push(args), onToggleFullscreen:() => fullscreen++};
  await mount(api.ReplayTransport, props);
  assert.equal(n('[role="slider"]').getAttribute('aria-valuetext'), '0:10');
  await act(() => n('.replay-toggle').click());
  await act(() => n('.replay-restart').click());
  await key('End');
  const select = n('.replay-speed select');
  assert.deepEqual([...select.options].map(option => option.value), ['0.5','1','2','4','8','0']);
  await act(() => {select.value='0'; select.dispatchEvent(new env.window.Event('change', {bubbles:true}));});
  await act(() => n('.replay-fullscreen').click());
  assert.deepEqual(actions, [['play'], ['seek',1200], ['seek',7200], ['rate',0]]);
  assert.equal(fullscreen, 1);
  await act(() => root.render(React.createElement(api.ReplayTransport, {...props, busy:true})));
  await act(() => n('.replay-toggle').click());
  await key('Home');
  assert.equal(actions.length, 4);
});


test('keyboard preview precedes commit exactly once for every adjustment', async () => {
  const events=[];
  await mount(api.OrderedFixture, {record:(...args)=>events.push(args)});
  await key('End');await key('Home');await key('ArrowRight');
  assert.deepEqual(events,[['change',100],['commit',100],['change',0],['commit',0],['change',5],['commit',5]]);
});

test('Replay End then restart follows the runtime frame, with no stale keyboard preview', async () => {
  const actions=[];
  await mount(api.ReplayRestartFixture,{control:(...args)=>actions.push(args)});
  await key('End');
  assert.equal(n('[role="slider"]').getAttribute('aria-valuenow'),'18000');
  await act(()=>n('.replay-restart').click());
  assert.equal(n('[role="slider"]').getAttribute('aria-valuenow'),'0');
  assert.equal(n('[role="slider"]').getAttribute('aria-valuetext'),'0:00');
  await key('ArrowRight');
  assert.equal(n('[role="slider"]').getAttribute('aria-valuenow'),'1');
  await pointer('pointerdown',640);assert.equal(n('[role="slider"]').getAttribute('aria-valuenow'),'9000');
  assert.deepEqual(actions,[['seek',18000],['seek',0],['seek',1]]);
  await pointer('pointercancel',640);await pointer('pointerup',640);
  assert.equal(n('[role="slider"]').getAttribute('aria-valuenow'),'1');
  await pointer('pointerdown',960);await pointer('pointerup',960);
  assert.equal(n('[role="slider"]').getAttribute('aria-valuenow'),'13500');
  assert.deepEqual(actions,[['seek',18000],['seek',0],['seek',1],['seek',13500]]);
  await act(()=>n('.replay-restart').click());
  assert.equal(n('[role="slider"]').getAttribute('aria-valuenow'),'0');
});

test('dragging back to the original Replay frame releases preview without seeking', async () => {
  const seeks=[],props={frame:600,total:18000,disabled:false,onSeek:value=>seeks.push(value)};
  await mount(api.ReplaySeekBar,props);
  await pointer('pointerdown',640);await pointer('pointermove',1280*600/18000);await pointer('pointerup',1280*600/18000);
  assert.deepEqual(seeks,[]);
  await act(()=>root.render(React.createElement(api.ReplaySeekBar,{...props,frame:1200})));
  assert.equal(n('[role="slider"]').getAttribute('aria-valuenow'),'1200');
});
