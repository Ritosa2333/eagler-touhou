import test from 'node:test';
import assert from 'node:assert/strict';
import {assertNativeMusicSuspension,midiOutputResumeReady,assertMidiOutputResumeBoundary} from './music-native-lifecycle.mjs';
import {nativeTabFixture} from '../../tests/adaptation/fixtures/native-tab-lifecycle.mjs';
import {summarizeRuntimePcmWindow} from './music-browser-helpers.mjs';
test('native tab proof binds one Chrome window, the same two documents and browser-generated hidden/visible events',()=>assertNativeMusicSuspension(nativeTabFixture()));
for(const [name,mutate] of Object.entries({
  'different window':e=>e.peer.windowId=13,
  'same page as peer':e=>e.peer.targetId='game-page',
  'new Runtime document':e=>e.restored.runtime.timeOrigin=200,
  'hidden getter shadow':e=>e.hidden.runtime.shadowed=['hidden'],
  'synthetic event':e=>e.restored.runtime.events[0].trusted=false,
  'no host event':e=>e.restored.host.events=[],
  'no actual hidden state':e=>e.hidden.runtime.hidden=false,
  'no restored focus':e=>e.restored.runtime.hasFocus=false,
  'cleanup failure':e=>e.cleanupErrors.push('close failed'),
}))test(`native tab proof rejects ${name}`,()=>{const e=nativeTabFixture();mutate(e);assert.throws(()=>assertNativeMusicSuspension(e));});

function resumeFixture(){
  const sourceIdentity={nodeId:0,nodeType:'DynamicsCompressorNode',role:'tinysynth-compressor-output'};
  const state=(time,state)=>({midiHost:{contextState:state,pcmTarget:{role:'launcher-tinysynth',available:true,contextIndex:0,tapIds:[0],sampleRate:48000,
    sourceIdentity:{...sourceIdentity},state,currentTime:time,stateChangeDrops:0,stateChangeCount:state==='running'?3:2,
    stateChanges:[{sequence:1,state:'running',currentTime:0},{sequence:2,state:'suspended',currentTime:1},...(state==='running'?[{sequence:3,state:'running',currentTime:1}]:[])]}}});
  const e={analysisWindowFrames:2048,settled:state(1,'suspended'),samples:[state(1,'suspended'),state(1.01,'running'),state(1.05,'running')]};
  const target=state(1.06,'running').midiHost.pcmTarget;
  const pcm=summarizeRuntimePcmWindow([{target,captures:[{contextIndex:0,tapId:0,contextState:'running',sampleRate:48000,
    sourceIdentity:{...sourceIdentity},currentTime:1.06,nonZero:32,peakAbs:.2,rms:.1,samples:2048}]}],{role:'launcher-tinysynth'});
  assert.equal(pcm.status,'complete');
  return{e,output:{pcm}};
}
test('MIDI recovery retains suspended transition samples and starts output after a real analyser window',()=>{
  const {e,output}=resumeFixture();assert.equal(midiOutputResumeReady(e.samples[0],e.settled),false);
  assert.equal(midiOutputResumeReady(e.samples[1],e.settled),false);assert.equal(midiOutputResumeReady(e.samples[2],e.settled),true);
  assertMidiOutputResumeBoundary(e,output);
});
for(const [name,mutate] of Object.entries({
  'no rendered recovery window':x=>x.e.samples.at(-1).midiHost.pcmTarget.currentTime=1.01,
  'different synth context':x=>x.e.samples.at(-1).midiHost.pcmTarget.contextIndex=1,
  'different production source':x=>x.e.samples.at(-1).midiHost.pcmTarget.sourceIdentity.nodeId=2,
  'resumed state event absent':x=>x.e.samples.at(-1).midiHost.pcmTarget.stateChangeCount=2,
  'output predates readiness':x=>x.output.pcm.windows[0].target.currentTime=1.01,
  'output still suspended':x=>x.output.pcm.windows[0].target.state='suspended',
}))test(`MIDI recovery boundary rejects ${name}`,()=>{const args=resumeFixture();mutate(args);assert.throws(()=>assertMidiOutputResumeBoundary(args.e,args.output));});
