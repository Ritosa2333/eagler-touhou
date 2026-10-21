/** Main1661–1700/4991–5022 behavior with synthetic MIDI/focus/event ports.
 * No real AudioContext, permission dialog, MIDI hardware or audible output PASS. */
import test, {before, after} from 'node:test';
import assert from 'node:assert/strict';
import {mkdir, mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
const root = fileURLToPath(new URL('../../', import.meta.url)); let work, createMidiBridge;
before(async () => {
  await mkdir(resolve(root, '.cache'), {recursive: true}); work = await mkdtemp(resolve(root, '.cache/midi-main-'));
  const outfile = resolve(work, 'bridge.mjs');
  await build({entryPoints: [resolve(root, 'app/services/midi-bridge.ts')], bundle: true, platform: 'node', format: 'esm', outfile, logLevel: 'silent'});
  ({createMidiBridge} = await import(pathToFileURL(outfile).href));
});
after(async () => {if (work) await rm(work, {recursive: true, force: true});});
function fixture({available = true, missing = false, factoryGate = null} = {}) {
  const frame = new EventTarget(), window = new EventTarget(), document = new EventTarget();
  Object.assign(document, {hidden: false, focused: true, activeElement: frame, hasFocus() {return this.focused;}});
  let context = {epoch: 1, game: 'th06', document: {}, target: new EventTarget(), music: 'midi'};
  let launched = true, open = true, external = false, externalWasActive = false;
  let synthInstance = null;
  const events = [], streams = [], hardware = [], listeners = new Set();
  const audio = {state: 'suspended',currentTime:0, async resume() {events.push('resume'); this.state = 'running';}, async suspend() {events.push('suspend'); this.state = 'suspended';}};
  const reverbBuffer = {length: 24000};
  const audioNodes = [];
  class AudioNode {
    constructor(name) {this.name = name; this.connections = []; audioNodes.push(this);}
    connect(destination) {this.connections.push({destination}); return destination;}
    disconnect(destination) {this.connections = destination ? this.connections.filter(connection => connection.destination !== destination) : [];}
  }
  class ScheduledSource extends AudioNode {
    constructor(name) {super(name);this.startCalls=[];this.stopCalls=[];this.endedListeners=[];this.scheduledStopTime=null;}
    start(...args) {this.startCalls.push(args);}
    stop(...args) {const time=args.length?args[0]:audio.currentTime;this.stopCalls.push(time);this.scheduledStopTime=time;}
    addEventListener(type,listener) {if(type==='ended')this.endedListeners.push(listener);}
    finish() {for(const listener of this.endedListeners.splice(0))listener({type:'ended'});}
  }
  audio.createOscillator=()=>new ScheduledSource('oscillator');
  audio.createBufferSource=()=>new ScheduledSource('buffer-source');
  audio.createConvolver = () => new AudioNode('replacement-convolver');
  audio.createGain = () => {const node = new AudioNode('replacement-reverb-gain'); node.gain = {value: 1}; return node;};
  class Synth {
    constructor(options) {
      events.push(['construct', options]); this.out = new AudioNode('out'); this.comp = new AudioNode('compressor');
      this.rev = new AudioNode('reverb-gain'); this.rev.gain = {value: 2.4}; this.dest = new AudioNode('destination');
      this.actx=audio;this.notetab=[];this.rhythm=Array(16).fill(false);this.releaseRatio=3.5;
      this.conv = new AudioNode('convolver'); this.conv.buffer = reverbBuffer; this.conv.normalize = true;
      this.initialConvolver = this.conv; this.initialReverb = this.rev;
      this.out.connect(this.conv); this.conv.connect(this.rev); this.rev.connect(this.comp);
      this.out.connect(this.comp); this.comp.connect(this.dest); synthInstance = this;
    }
    getAudioContext() {return audio;}
    reset() {events.push('reset');for(const voice of this.notetab)for(const source of voice.o)source.stop();this.notetab=[];}
    _note(time,channel,note,velocity,partials) {
      const source=this.rhythm[channel]?this.actx.createBufferSource():this.actx.createOscillator();
      source.connect(this.out);source.start(time);
      if(this.rhythm[channel])source.stop(time+partials[0].d*this.releaseRatio);
      else this.notetab.push({t:time,e:99999,ch:channel,n:note,o:[source],g:[{gain:{value:velocity}}],f:0});
      return source;
    }
    send(bytes) {streams.push(bytes);}
  }
  const bridge = createMidiBridge({runtime: {
    getMidiEventContext: () => context, getSnapshot: () => ({launched}),
    subscribe(listener) {listeners.add(listener); return () => listeners.delete(listener);},
  }, external: {
    async prepare() {events.push('external-prepare');}, panic() {events.push('panic'); externalWasActive = false;},
    deliver(bytes) {if (external && !externalWasActive) bridge.resetSynth(); externalWasActive = external; if (external) hardware.push(bytes); return external;},
  }, async loadSynth() {events.push('load'); if (factoryGate) await factoryGate.promise; return missing ? undefined : Synth;},
  webAudioAvailable: available, translate: key => key, frame, window, document, playerOpen: () => open});
  return {bridge, frame, window, document, audio, audioNodes, reverbBuffer, synth: () => synthInstance, events, streams, hardware,
    context: () => context,
    change(value, notify = true) {context = value; if (notify) for (const listener of listeners) listener();},
    launched(value) {launched = value;}, open(value) {open = value;}, external(value) {external = value;},
    midi(bytes, target = context.target) {target.dispatchEvent(new CustomEvent('touhou-midi', {detail: {bytes}}));},
  };
}
test('prepares exactly one retained TinySynth with original options and supports OGG fallback stream', async () => {
  const f = fixture(); await Promise.all([f.bridge.prepare('midi'), f.bridge.prepare('ogg-stream')]);
  assert.equal(f.events.filter(x => x === 'load').length, 1);
  assert.deepEqual(f.events.filter(Array.isArray), [['construct', {quality: 1, useReverb: 1, voices: 64}]]);
  f.change({...f.context(), music: 'ogg'}); f.midi([144, 60, 127, 'x']);
  assert.deepEqual(f.streams, [[144, 60, 127]]); f.bridge.dispose();
});
test('one audible owner: hardware takeover resets browser held notes before exclusive delivery', async () => {
  const f = fixture(); await f.bridge.prepare('midi'); f.midi([144, 60, 100]); f.external(true); f.midi([128, 60, 0]);
  assert.deepEqual(f.streams, [[144, 60, 100]]); assert.deepEqual(f.hardware, [[128, 60, 0]]);
  assert.equal(f.events.filter(x => x === 'reset').length, 1); f.bridge.dispose();
});
test('none mode and unauthenticated old document events cannot reach either output', async () => {
  const f = fixture(); await f.bridge.prepare('midi'); const old = f.context();
  f.change({...old, document: {}}, false); f.midi([144, 60, 1], old.target);
  assert.deepEqual(f.streams, []);
  f.bridge.rebind(); f.change({...f.context(), music: 'none'}); f.midi([144, 60, 1]);
  assert.deepEqual(f.streams, []); f.bridge.dispose();
});
test('rebind removes old stream and close listeners, authentic new close resets and panics', async () => {
  const f = fixture(); await f.bridge.prepare('midi'); const old = f.context();
  const next = {...old, epoch: 2, document: {}, target: new EventTarget()}; f.change(next); f.events.length = 0;
  f.midi([144, 60, 1], old.target); old.target.dispatchEvent(new Event('touhou-midi-close'));
  assert.deepEqual(f.events, []); assert.deepEqual(f.streams, []);
  next.target.dispatchEvent(new Event('touhou-midi-close')); assert.deepEqual(f.events, ['reset', 'panic']);
  const firstConvolver = f.synth().conv, firstReverb = f.synth().rev;
  assert.notEqual(firstConvolver, f.synth().initialConvolver, 'close replaces the convolver that held the old tail');
  assert.notEqual(firstReverb, f.synth().initialReverb, 'close replaces the complete wet branch');
  assert.equal(f.synth().initialConvolver.connections.length, 0, 'the old convolver is detached from the output graph');
  assert.equal(f.synth().initialReverb.connections.length, 0, 'the old reverb bus is detached from the compressor');
  assert.equal(firstConvolver.buffer, f.reverbBuffer, 'the clean convolver retains the original impulse response');
  assert.equal(firstConvolver.normalize, true);
  assert.equal(firstConvolver.connections.some(connection => connection.destination === firstReverb), true);
  assert.equal(firstReverb.connections.some(connection => connection.destination === f.synth().comp), true);
  assert.equal(firstReverb.gain.value, 2.4, 'the clean wet branch retains the configured reverb gain');
  assert.equal(f.synth().out.connections.some(connection => connection.destination === f.synth().comp), true, 'the direct output path remains connected');
  assert.equal(f.synth().out.connections.some(connection => connection.destination === firstConvolver), true);
  f.midi([144, 60, 1]);
  assert.deepEqual(f.streams, [[144, 60, 1]]);
  assert.equal(f.synth().conv, firstConvolver, 'the next MIDI event reuses the clean effect path');
  next.target.dispatchEvent(new Event('touhou-midi-close'));
  assert.notEqual(f.synth().conv, firstConvolver, 'a later close replaces the wet path created by the prior close');
  assert.equal(firstConvolver.connections.length, 0);
  assert.equal(firstReverb.connections.length, 0);
  f.bridge.dispose();
});
test('TinySynth reset stops every owned scheduled source and restores the per-note factories',async()=>{
  const f=fixture();await f.bridge.prepare('midi');
  const synth=f.synth(),context=f.audio,createOscillator=context.createOscillator,createBufferSource=context.createBufferSource;
  synth.rhythm[9]=true;
  context.currentTime=1;
  const rhythm=synth._note(1.1,9,42,96,[{d:.2}]);
  const melodic=synth._note(1.1,0,60,88,[{d:.5}]);
  const external=context.createBufferSource();external.start(1.1);external.stop(10);
  assert.equal(context.createOscillator,createOscillator,'the oscillator factory is restored after the synchronous _note call');
  assert.equal(context.createBufferSource,createBufferSource,'the buffer-source factory is restored after the synchronous _note call');
  assert.deepEqual(rhythm.startCalls,[[1.1]]);
  assert.deepEqual(rhythm.stopCalls,[1.1+.2*3.5],'the TinySynth rhythm source has a future one-shot stop before close');

  context.currentTime=1.4;f.bridge.resetSynth();
  assert.deepEqual(rhythm.stopCalls,[1.1+.2*3.5,1.4],'reset replaces the future rhythm stop with an immediate stop');
  assert.deepEqual(melodic.stopCalls,[1.4],'the vendor reset already stopped its notetab source; the owner cleanup does not stop it twice');
  assert.deepEqual(external.stopCalls,[10],'a same-context source created outside this TinySynth _note is not owned');
  let snapshot=f.bridge.getOwnedAudioSourceSnapshot();
  assert.equal(snapshot.trackingInstalled,true);
  assert.equal(snapshot.activeCount,2);
  assert.deepEqual(snapshot.activeSources.map(({channel,note,nodeType,stopCount,stopTime,resetStopCount})=>
    ({channel,note,nodeType,stopCount,stopTime,resetStopCount})),[
    {channel:9,note:42,nodeType:'ScheduledSource',stopCount:2,stopTime:1.4,resetStopCount:1},
    {channel:0,note:60,nodeType:'ScheduledSource',stopCount:1,stopTime:1.4,resetStopCount:1},
  ]);

  f.bridge.resetSynth();
  assert.deepEqual(rhythm.stopCalls,[1.1+.2*3.5,1.4],'repeated reset does not add a redundant stop call');
  assert.deepEqual(melodic.stopCalls,[1.4]);
  rhythm.finish();melodic.finish();snapshot=f.bridge.getOwnedAudioSourceSnapshot();
  assert.equal(snapshot.activeCount,0);
  assert.equal(snapshot.recentSources.find(source=>source.channel===9&&source.note===42).ended,true);
  assert.equal(snapshot.recentSources.find(source=>source.channel===0&&source.note===60).ended,true);
  f.bridge.dispose();
});
test('blur suspends without reset/panic; resume requires every original focus condition', async () => {
  const f = fixture(); await f.bridge.prepare('midi'); f.events.length = 0;
  f.window.dispatchEvent(new Event('blur')); assert.deepEqual(f.events, ['suspend']);
  f.document.hidden = true; f.frame.dispatchEvent(new Event('focus')); assert.equal(f.audio.state, 'suspended');
  f.document.hidden = false; f.document.focused = false; f.frame.dispatchEvent(new Event('focus')); assert.equal(f.audio.state, 'suspended');
  f.document.focused = true; f.document.activeElement = {}; f.frame.dispatchEvent(new Event('focus')); assert.equal(f.audio.state, 'suspended');
  f.document.activeElement = f.frame; f.open(false); f.frame.dispatchEvent(new Event('focus')); assert.equal(f.audio.state, 'suspended');
  f.open(true); f.launched(false); f.frame.dispatchEvent(new Event('focus')); assert.equal(f.audio.state, 'suspended');
  f.launched(true); f.frame.dispatchEvent(new Event('focus')); assert.equal(f.audio.state, 'running');
  assert.deepEqual(f.events, ['suspend', 'resume']); f.bridge.dispose();
});
test('pagehide panics external hardware without inventing a synth reset', async () => {
  const f = fixture(); await f.bridge.prepare('midi'); f.events.length = 0;
  f.window.dispatchEvent(new Event('pagehide')); assert.deepEqual(f.events, ['panic']); f.bridge.dispose();
});
test('unsupported WebAudio and missing synth retain original error keys; none needs neither', async () => {
  const f = fixture({available: false}); await f.bridge.prepare('none'); assert.deepEqual(f.events, []);
  await assert.rejects(f.bridge.prepare('midi'), /music.webAudioUnsupported/); f.bridge.dispose();
  const g = fixture({missing: true}); await assert.rejects(g.bridge.prepare('midi'), /music.synthMissing/); g.bridge.dispose();
});
test('dispose fences a pending synth load and removes all focus/native listeners', async () => {
  let resolve; const gate = {promise: new Promise(r => {resolve = r;})};
  const f = fixture({factoryGate: gate}); const preparing = f.bridge.prepare('midi'); f.bridge.dispose(); resolve(); await preparing;
  assert.equal(f.events.some(Array.isArray), false); f.events.length = 0;
  f.window.dispatchEvent(new Event('pagehide')); f.frame.dispatchEvent(new Event('focus')); f.midi([144, 60, 1]);
  assert.deepEqual(f.events, []); assert.deepEqual(f.streams, []);
});
