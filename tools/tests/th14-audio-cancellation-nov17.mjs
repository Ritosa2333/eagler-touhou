import assert from 'node:assert/strict';
import {readFile, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {createHash} from 'node:crypto';

// The production helper and the source-extracted shell callbacks execute
// unchanged. Web Audio, visibility and native-loop delivery are explicit
// deterministic ports: this is not a real-browser audio acceptance test.
const [hostPath, shellPath, outputPath] = process.argv.slice(2).map(value => resolve(value));
assert(hostPath && shellPath && outputPath, 'host, shell and output paths required');
const hostBytes = await readFile(hostPath), shellBytes = await readFile(shellPath);
const {suspendRuntimeAudio: suspend, resumeRuntimeAudio: resume, createReplayViewerHostTh14} = await import(pathToFileURL(hostPath));
const shell = shellBytes.toString('utf8');
const pauseStart = shell.indexOf('  pause:async on=>{');
const pauseEnd = shell.indexOf('\n  },\n  close:', pauseStart);
assert(pauseStart >= 0 && pauseEnd > pauseStart);
const pauseBody = shell.slice(pauseStart + '  pause:async on=>{'.length, pauseEnd);
const stopStart = shell.indexOf('async function stop(){');
const stopEnd = shell.indexOf('\nfunction path(value)', stopStart);
assert(stopStart >= 0 && stopEnd > stopStart);
const stopFunction = shell.slice(stopStart, stopEnd);
function fixture(state = 'suspended') {
  const calls = [], audio = [], pending = [];
  const context = {state, destination: {}, resume() {
    return new Promise((resolve, reject) => pending.push({
      resolve() {context.state = 'running'; resolve();}, reject,
    }));
  }};
  const node = {disconnect() {audio.push('disconnect');}, connect() {audio.push('connect');}};
  const Module = {SDL3: {audioContext: context, audio_playback: {scriptProcessorNode: node}}};
  const core = {sdl_loop_pause(value) {calls.push(value);}, th11_loop_stop() {calls.push('stop');}};
  return {calls, audio, pending, context, node, Module, core};
}
function shellCallbacks(f, hidden = false) {
  const document = {hidden};
  const build = new Function('Module', 'core', 'document', 'suspendRuntimeAudio', 'resumeRuntimeAudio', `
    let launched=true,stopping=false,replayViewer=null;
    const roomOwned=false,roomRuntime=false,options={replayViewer:true};
    const clearKeyboard=()=>{},save=async()=>{},closeAudio=async()=>{},emit=()=>{};
    const callCore=async(name,value)=>{if(name!=='th11_loop_pause')throw Error(name);core.sdl_loop_pause(value);};
    ${stopFunction}
    return {pause:async on=>{${pauseBody}\n},stop};
  `);
  return {...build(f.Module, f.core, document, suspend, resume), document};
}
const cases = [];
async function check(name, run) {
  try {await run(); cases.push({name, passed: true});}
  catch (error) {cases.push({name, passed: false, error: error.message});}
}
await check('running context ordinary resume', async () => {
  const f = fixture('running'); assert.equal(await resume(f.Module, f.core), true);
  assert.deepEqual(f.calls, [0]); assert.deepEqual(f.audio, []);
});
await check('suspend then successful reconnect', async () => {
  const f = fixture('running'); suspend(f.Module, f.core);
  assert.equal(await resume(f.Module, f.core), true);
  assert.deepEqual(f.calls, [1, 0]); assert.deepEqual(f.audio, ['disconnect', 'connect']);
});
await check('new pause cancels pending resume', async () => {
  const f = fixture(); suspend(f.Module, f.core);
  const p = resume(f.Module, f.core); suspend(f.Module, f.core); f.pending[0].resolve();
  assert.equal(await p, false); assert.deepEqual(f.calls, [1, 1]);
  assert.deepEqual(f.audio, ['disconnect']);
});
await check('repeated pause still cancels pending resume', async () => {
  const f = fixture(); const p = resume(f.Module, f.core);
  suspend(f.Module, f.core); suspend(f.Module, f.core); f.pending[0].resolve();
  assert.equal(await p, false); assert.deepEqual(f.calls, [1, 1]);
});
await check('foreground lost while awaiting context', async () => {
  const f = fixture(); let foreground = true;
  const p = resume(f.Module, f.core, () => foreground); foreground = false;
  f.pending[0].resolve(); assert.equal(await p, false); assert.deepEqual(f.calls, []);
});
await check('resume rejection preserves suspended loop', async () => {
  const f = fixture(); suspend(f.Module, f.core); const p = resume(f.Module, f.core);
  f.pending[0].reject(Error('autoplay denied')); assert.equal(await p, false);
  assert.deepEqual(f.calls, [1]); assert.deepEqual(f.audio, ['disconnect']);
});
await check('reconnect failure preserves suspended loop', async () => {
  const f = fixture('running'); suspend(f.Module, f.core);
  f.node.connect = () => {throw Error('disconnected context');};
  assert.equal(await resume(f.Module, f.core), false); assert.deepEqual(f.calls, [1]);
});
await check('silent no-context control remains resumable', async () => {
  const calls = [], core = {sdl_loop_pause: value => calls.push(value)};
  suspend({}, core); assert.equal(await resume({}, core), true); assert.deepEqual(calls, [1, 0]);
});
await check('detached node without context remains paused', async () => {
  const f = fixture(); suspend(f.Module, f.core); delete f.Module.SDL3.audioContext;
  assert.equal(await resume(f.Module, f.core), false); assert.deepEqual(f.calls, [1]);
});
await check('only newest concurrent resume commits', async () => {
  const f = fixture(); const old = resume(f.Module, f.core), latest = resume(f.Module, f.core);
  f.pending[0].resolve(); assert.equal(await old, false);
  f.pending[1].resolve(); assert.equal(await latest, true); assert.deepEqual(f.calls, [0]);
});
await check('independent runtime cancellation stays independent', async () => {
  const a = fixture(), b = fixture(); const pa = resume(a.Module, a.core), pb = resume(b.Module, b.core);
  suspend(a.Module, a.core); a.pending[0].resolve(); b.pending[0].resolve();
  assert.equal(await pa, false); assert.equal(await pb, true); assert.deepEqual(b.calls, [0]);
});
await check('actual shell newer viewer pause wins', async () => {
  const f = fixture(), s = shellCallbacks(f); const p = s.pause(false);
  await s.pause(true); f.pending[0].resolve(); await p;
  assert(f.calls.every(value => value === 1), JSON.stringify(f.calls));
  assert(!f.audio.includes('connect'), JSON.stringify(f.audio));
});
await check('actual shell denied resume does not unpause', async () => {
  const f = fixture(), s = shellCallbacks(f); await s.pause(true); const p = s.pause(false);
  f.pending[0].reject(Error('autoplay denied')); await p;
  assert.equal(f.calls.at(-1), 1, JSON.stringify(f.calls));
});
await check('actual shell hidden document stays paused', async () => {
  const f = fixture(), s = shellCallbacks(f, true); await s.pause(false);
  assert.equal(f.pending.length, 0); assert(f.calls.every(value => value === 1));
});
await check('actual shell normal viewer resume works', async () => {
  const f = fixture('running'), s = shellCallbacks(f); await s.pause(true); await s.pause(false);
  assert.equal(f.calls.at(-1), 0);
});
await check('actual shell stop cancels pending viewer resume', async () => {
  const f = fixture(), s = shellCallbacks(f); const p = s.pause(false);
  await s.stop(); f.pending[0].resolve(); await p;
  const stopped = f.calls.indexOf('stop'); assert(stopped >= 0);
  assert(!f.calls.slice(stopped + 1).includes(0), JSON.stringify(f.calls));
});
await check('actual viewer foreground resume cannot override newer pause command', async () => {
  const f = fixture('running'), s = shellCallbacks(f);
  const viewer = createReplayViewerHostTh14({
    read: () => ({path: 'replay/test.rpy', bytes: new Uint8Array([1])}),
    describe: () => ({character: 0, shot: 0, difficulty: 1, stages: [{id: 1, frames: 1000}]}),
    open: async () => {}, rate: async () => {}, step: async () => 1,
    status: async () => ({stage: 1, frame: 1, frames: 1000, ended: false, phase: 1, active: true}),
    pause: s.pause, close: async () => {}, emit: () => {},
    schedule: () => 1, cancel: () => {}, visible: () => true,
  });
  await viewer.command({action: 'open', path: 'replay/test.rpy'});
  f.context.state = 'suspended'; const pending = viewer.foreground();
  await viewer.command({action: 'pause'}); f.pending[0].resolve(); await pending;
  assert.equal(viewer.state.paused, true); assert.equal(f.calls.at(-1), 1, JSON.stringify(f.calls));
  await viewer.close();
});
const digest = bytes => createHash('sha256').update(bytes).digest('hex');
const result = {schema: 'th14/audio-resume-race-check/1', hostSha256: digest(hostBytes),
  shellSha256: digest(shellBytes), cases, passed: cases.filter(c => c.passed).length,
  failed: cases.filter(c => !c.passed).length, realBrowserAudioAccepted: false,
  audibleOutput: false, completeGame: false};
await writeFile(outputPath, JSON.stringify(result, null, 2) + '\n');
console.log(JSON.stringify(result));
if (result.failed) process.exitCode = 1;
