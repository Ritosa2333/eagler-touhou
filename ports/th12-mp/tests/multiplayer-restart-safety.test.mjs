import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
import {multiplayerHeldInput} from '../src/multiplayer.mjs';
import {inspectMultiplayerReplay} from '../src/multiplayer-replay.mjs';

const source=readFileSync(new URL('../src/multiplayer.mjs',import.meta.url),'utf8');
const reportSource=source.slice(source.indexOf(' function report(now){'),source.indexOf(' function tick(input='));
const tickSource=source.slice(source.indexOf(' function tick(input='),source.indexOf(' function stop(){'));
const flush=async()=>{for(let i=0;i<12;i++)await Promise.resolve();};

test('multiplayer keyboard, mapped gamepad and touch preserve Retry, Enter and movement for native ownership',()=>{
 for(const retry of [0x2000,0x200000]){
  assert.equal(multiplayerHeldInput({held:retry,ui:retry}),0x2000);
  assert.equal(multiplayerHeldInput({},retry),0x2000);
  assert.equal(multiplayerHeldInput({},0,retry),0x2000);
  assert.equal(multiplayerHeldInput({held:retry|64,ui:0x80000}),0x3040);
 }
 assert.equal(multiplayerHeldInput({},0x80000|32),0x1020);
 assert.equal(multiplayerHeldInput({},0,0x80000|16),0x1010);
});

test('replays using the former pause/retry rules require their original runtime',()=>{
 const bytes=new Uint8Array(200),view=new DataView(bytes.buffer),u32=(offset,value)=>view.setUint32(offset,value,true);
 bytes.set(new TextEncoder().encode('EAGLRPY1'));bytes[20]=2;
 for(const [offset,value] of [[8,1],[12,12],[24,128],[28,1],[32,1],[40,0x4d323154],[44,1],[48,7],[80,6],[84,2],[136,1],[168,1]])u32(offset,value);
 assert.equal(inspectMultiplayerReplay(bytes,{validate:()=>true}).playerCount,2);
 u32(48,6);assert.throws(()=>inspectMultiplayerReplay(bytes,{validate:()=>true}),/录像版本/);
});

function harness(world){
 const calls=[],errors=[],events=[],status=Array(24).fill(0);status[3]=5100;status[12]=1;status[13]=1;
 let completeSave,completeLoad;
 const saving=new Promise(resolve=>{completeSave=resolve;}),loading=new Promise(resolve=>{completeLoad=resolve;});
 const context={lifetime:0,current:()=>{},active:true,transitioning:false,exitPending:false,lastReport:0,savedAt:0,readOnly:false,lastRestart:null,rollbackCount:0,
  replayPath:null,words:()=>status,state:()=>world,rollbackDiagnostics:()=>({restores:0,resimulated:0,missingInputs:0}),
  emit:event=>events.push(event),save:()=>{calls.push('save');return saving;},loadStage:stage=>{calls.push(['load',stage]);return loading;},
  getOptions:()=>({netplayDifficulty:3}),onError:error=>errors.push(error),onExit:async()=>calls.push('exit'),
  calibration:{start:()=>calls.push('calibrate')},core:{_th12_mp_pump:()=>{calls.push('pump');return 1;},_th12_mp_restart:()=>{calls.push('restart');status[12]++;return 1;}}};
 runInNewContext(reportSource+tickSource+';globalThis.report=report;globalThis.tick=tick;',context);
 return {context,calls,errors,events,status,completeSave,completeLoad};
}

test('unconfirmed restart cannot load stage 1, even if a terminal flag is set',async()=>{
 for(const world of [
  {restart:1,finished:1,paused:0,menuConfirm:0,menuAction:0,menuSelection:0},
  {restart:1,finished:1,paused:1,menuConfirm:1,menuAction:3,menuSelection:1},
  {restart:1,finished:1,paused:1,menuConfirm:1,menuAction:1,menuSelection:0},
 ]){
  const h=harness({...world,stage:6,players:[]});h.context.report(1);h.completeSave();await flush();
  assert.deepEqual(h.calls,['save']);assert.equal(h.errors.length,1);assert.match(h.errors[0].message,/未确认/);
  h.context.tick();assert.deepEqual(h.calls,['save','pump'],'a retired run only pumps transport while its exit is pending');
 }
});

test('confirmed retry saves and loads once, freezes simulation and retains restart provenance',async()=>{
 const h=harness({restart:1,finished:1,paused:1,menuConfirm:1,menuAction:3,menuSelection:0,stage:6,players:[{seat:0,held:0},{seat:1,held:1}]});
 h.context.report(1);h.context.tick();h.context.report(2);assert.deepEqual(h.calls,['save','pump']);
 h.completeSave();await flush();assert.deepEqual(h.calls,['save','pump',['load',1]]);
 h.context.tick();assert.equal(h.calls.at(-1),'pump');assert(!h.calls.includes('restart'));
 h.completeLoad();await flush();assert.deepEqual(h.calls.slice(-2),['restart','calibrate']);assert.equal(h.context.exitPending,false);
 assert.equal(h.context.lastRestart.stage,6);assert.equal(h.context.lastRestart.fromGeneration,1);assert.equal(h.context.lastRestart.generation,2);
 assert.deepEqual(Array.from(h.context.lastRestart.confirmHeldSeats),[1]);
 h.status[13]=0;h.context.report(1001);assert.equal(h.events.at(-1).multiplayer.lastRestart.frame,5100);
 assert.equal(h.errors.length,0);
});
