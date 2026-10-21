import test from 'node:test';
import assert from 'node:assert/strict';
import {assertRuntimeObservation, readRuntimeObservation,runtimeScenarioResult,REQUIRED_RUNTIME_CHECKS} from './runtime-observations.mjs';
import {evaluateDesktopGameplayReadiness} from './runtime-desktop-readiness.mjs';
import {resolveAdaptationAdapter} from '../../tests/adaptation/adapters/registry.mjs';

test('observed actions and missing required behaviors cannot produce an overall passing Runtime',()=>{
  const checks=REQUIRED_RUNTIME_CHECKS.map(id=>({id,status:'passed'}));
  assert.equal(runtimeScenarioResult({checks}).passed,true);
  const partial=runtimeScenarioResult({checks:checks.filter(check=>check.id!=='native-pause-and-restart')});
  assert.equal(partial.status,'incomplete');assert.equal(partial.passed,false);
  assert.deepEqual(partial.unverifiedChecks,['native-pause-and-restart']);
  assert.equal(runtimeScenarioResult({checks:[]}).passed,false);
  for(const id of ['hosted-keyboard-native-effect','native-gamepad-effect','replay-file-roundtrip-persistence']) {
    const sampled=runtimeScenarioResult({checks:checks.map(check=>check.id===id?{...check,status:'observed'}:check)});
    assert.equal(sampled.status,'incomplete');assert.deepEqual(sampled.unverifiedChecks,[id]);
  }
  assert.equal(runtimeScenarioResult({checks:[...checks,{id:'additional',status:'failed'}]}).status,'failed');
});

function withWindow(value, operation) {
  const previous = globalThis.window;
  globalThis.window = value;
  try { return operation(); }
  finally {
    if (previous === undefined) delete globalThis.window;
    else globalThis.window = previous;
  }
}

function wasmFixture() {
  const buffer = new ArrayBuffer(4096), view = new DataView(buffer);
  return {
    buffer, view, memory: {buffer},
    int32(pointer, values) { values.forEach((value, index) => view.setInt32(pointer + index * 4, value, true)); },
    uint32(pointer, values) { values.forEach((value, index) => view.setUint32(pointer + index * 4, value, true)); },
    float32(pointer, values) { values.forEach((value, index) => view.setFloat32(pointer + index * 4, value, true)); },
  };
}

const nativeTarget=(game,variant='normal',inputSemantics='consumed')=>({game,variant,adapter:{
  runtime:{global:'__nativeRuntime',core:'core',module:'Module',app:'app'},observation:{
  kind:'native-v1',export:`${game}_native_observation_v1`,source:`ports/${game}/runtime.cpp | read_native_state`,
  sampleBoundary:'after-native-update',inputSemantics,
}}});

test('versioned native observation uses validity and consumed local-seat state, not legacy guesses',()=>{
  const wasm=wasmFixture();
  const words=[1,0x1bf,2,0x80,120,120,1,0xffffffff,0,0,777,0];
  const positions=new Float32Array([-12.5,240.25]);
  words[8]=new Uint32Array(positions.buffer)[0];words[9]=new Uint32Array(positions.buffer)[1];
  wasm.uint32(128,words);
  const core={memory:wasm.memory,th11_phase:()=>1,th11_frame:()=>999,th11_native_observation_v1:()=>128};
  withWindow({core,__nativeRuntime:{core}},()=>{
    const normal=readRuntimeObservation(nativeTarget('th11'));
    assert.equal(normal.nativeObservation.sampleBoundary,'after-native-update');
    assert.equal(normal.nativeObservation.inputSemantics,'consumed');
    assert.deepEqual(normal.phase,{value:2,name:'paused'});
    assert.equal(normal.input.value,0x80);assert.deepEqual(normal.position,{x:-12.5,y:240.25});
    assert.equal(normal.frames.simulation,120);assert.equal(normal.frames.application,777);
    assert.equal(normal.frames.multiplayer,undefined);assert.equal(normal.localSeat,0);
    assert.equal(normal.capabilities.restartFrame,'restart');
    wasm.uint32(128,[...words.slice(0,11),2]);
    assert.throws(()=>readRuntimeObservation(nativeTarget('th11')),/invalid local seat/);
    assert.equal(readRuntimeObservation(nativeTarget('th11','multiplayer')).capabilities.restartFrame,null);
    assert.equal(readRuntimeObservation(nativeTarget('th11','multiplayer')).localSeat,2);
    wasm.uint32(128,[1,1,6,...Array(5).fill(0xffffffff),0x7fc00000,0x7fc00000,0xffffffff,0xffffffff]);
    const inactive=readRuntimeObservation(nativeTarget('th11'));
    assert.deepEqual(inactive.phase,{value:6,name:'inactive'});
    assert.equal(inactive.input,null);assert.equal(inactive.position,null);assert.equal(inactive.pause,null);
    assert.equal(inactive.capabilities.pause,false);assert.equal(inactive.capabilities.restartFrame,null);
    wasm.uint32(128,[2,...words.slice(1)]);
    assert.throws(()=>readRuntimeObservation(nativeTarget('th11')),/unsupported.*ABI/);
    wasm.uint32(128,[1,0x201,...words.slice(2)]);
    assert.throws(()=>readRuntimeObservation(nativeTarget('th11')),/unsupported.*ABI/);
    wasm.uint32(128,[...words.slice(0,6),2,...words.slice(7)]);
    assert.throws(()=>readRuntimeObservation(nativeTarget('th11')),/invalid observed pause/);
    wasm.uint32(128,[...words.slice(0,3),0xffffffff,...words.slice(4)]);
    assert.throws(()=>readRuntimeObservation(nativeTarget('th11')),/missing native observation field 3/);
    assert.throws(()=>assertRuntimeObservation({...normal,phase:{value:3,name:'game_over'}},'pause-active'),/not a paused game/);
  });
});

test('native-v1 export discovery requires an explicit ABI and input meaning',()=>{
  const wasm=wasmFixture();wasm.uint32(128,[1,0x13f,1,128,20,20,0,0xffffffff,0,0,0xffffffff,0]);
  const core={memory:wasm.memory,th11_native_observation_v1:()=>128,th11_phase:()=>1,th11_frame:()=>20};
  withWindow({core,__nativeRuntime:{core}},()=>{
    const legacy=readRuntimeObservation({game:'th11',variant:'normal'});
    assert.equal(legacy.nativeObservation,undefined,'an export-shaped symbol alone is not a registered ABI');
    assert.equal(legacy.capabilities.input,false);
    assert.throws(()=>readRuntimeObservation({game:'th11',variant:'normal',adapter:{...nativeTarget('th11').adapter,observation:{
      kind:'native-v1',export:'th11_native_observation_v1',sampleBoundary:'after-update',
      inputSemantics:'consumed',
    }}}),/must declare its source, sampling boundary and input semantics/);
    const unsupported=readRuntimeObservation({game:'th11',variant:'normal',adapter:{observation:{
      kind:'unsupported',reason:'title input has no stable native snapshot',
    }}});
    assert.ok(unsupported.unsupported.includes('native-observation-unsupported:title input has no stable native snapshot'));
  });
});

test('TH08 legacy input mapping names the real queued status trace, not a fabricated native-v1 export',()=>{
  const wasm=wasmFixture(),app=8,adapter=resolveAdaptationAdapter('th08','normal');
  wasm.uint32(128,[0,0,0,1,0,0,0,0,0,0,0]);wasm.int32(512,[...Array(11).fill(0),1,...Array(4).fill(0)]);
  const core={memory:wasm.memory,status:(_app,index)=>[2,1,0,1,0,0,1,0,0,0,0][index],trace:()=>128,
    diagnostics:()=>512,sdl_stats:()=>0,allocate:()=>1536,deallocate:()=>{},
    touch_state:(_app,pointer)=>wasm.float32(pointer,[1,1,1,0,0,0,0,0,0,0,0,0]),
    th08_native_observation_v1(){throw Error('TH08 does not export this ABI');}};
  const result=withWindow({__th08Runtime:{core,app}},()=>readRuntimeObservation({game:'th08',variant:'normal',
    adapter:{runtime:adapter.runtime,observation:adapter.observation}}));
  assert.equal(result.input.kind,'native-queued-for-next-movement-buttons');
  assert.equal(result.input.inputSemantics,'queued-for-next-movement');
  assert.equal(result.input.source,adapter.observation.inputSource);
  assert.equal(result.input.sampleBoundary,adapter.observation.sampleBoundary);
  const unsupported=withWindow({__th08Runtime:{core,app}},()=>readRuntimeObservation({game:'th08',variant:'normal',
    adapter:{runtime:adapter.runtime,observation:{kind:'unsupported',reason:'no supported observation ABI'}}}));
  assert.equal(unsupported.input,null);
  assert.equal(unsupported.capabilities.input,false,'unsupported observation cannot satisfy input readiness');
});

test('TH11 phase can become ready before its native player ABI, then become gameplay-ready in the same runtime epoch',()=>{
  const wasm=wasmFixture();
  const missing=0xffffffff,missingFloat=0x7fc00000;
  const words=[1,0x029,1,missing,24,missing,0,missing,missingFloat,missingFloat,missing,missing];
  wasm.uint32(128,words);
  const core={memory:wasm.memory,th11_phase:()=>1,th11_frame:()=>24,th11_native_observation_v1:()=>128};
  withWindow({core,__nativeRuntime:{core}},()=>{
    const before=readRuntimeObservation(nativeTarget('th11'));
    assert.deepEqual(before.phase,{value:1,name:'stage'});
    assert.equal(before.capabilities.input,false);
    assert.equal(before.capabilities.position,false);
    assert.deepEqual(evaluateDesktopGameplayReadiness(before,'th11'),{
      ready:false,reasons:['native-input-unavailable','native-player-position-unavailable','local-seat-unavailable'],
    });

    const position=new Float32Array([27,310]);
    const positionWords=new Uint32Array(position.buffer);
    wasm.uint32(128,[1,0x13f,1,0,25,25,0,missing,positionWords[0],positionWords[1],missing,0]);
    const after=readRuntimeObservation(nativeTarget('th11'));
    assert.equal(after.capabilities.input,true);
    assert.equal(after.capabilities.position,true);
    assert.equal(after.input.value,0);
    assert.deepEqual(after.position,{x:27,y:310});
    assert.equal(after.localSeat,0);
    assert.equal(after.frames.simulation,25);
    assert.deepEqual(evaluateDesktopGameplayReadiness(after,'th11'),{ready:true,reasons:[]});
  });
});

test('TH08 normal exports expose native control/enemy frames, mapped buttons, and position', () => {
  for (const variant of ['normal']) {
    const wasm = wasmFixture(), app = 8;
    wasm.uint32(128, [0, 1, 0, 17, 0, 0, 0, 0, 0, 0, 0]);
    wasm.float32(128 + 7 * 4, [40, 80]);
    wasm.int32(512, [0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0]);
    wasm.uint32(768, [0, 0, 0, 0, 0, 22]);
    const core = {memory: wasm.memory, status: (_app, field) => [2, 1, 0, 1, 0, 0, 17, 0, 0, 0, 0][field],
      trace: () => 128, diagnostics: () => 512, sdl_stats: () => 768, allocate: () => 1536, deallocate: () => {},
      touch_state: (_app, pointer) => wasm.float32(pointer, [1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0])};
    const observed = withWindow({__th08Runtime: {core, app, status: () => []}}, () => readRuntimeObservation({game: 'th08', variant}));
    assert.equal(observed.frames.enemy, 17);
    assert.equal(observed.frames.control, 17);
    assert.equal(observed.render.presentationFrame, 22);
    assert.deepEqual(observed.position, {x: 40, y: 80});
    assert.equal(observed.input.value, 1);
    assert.equal(observed.input.sampleBoundary,'after-application-update');
    assert.equal(observed.input.source,'diagnostics[11] | player_state.input.buttons');
    assert.equal(observed.pause.active, false);
    assert.deepEqual(observed.worldProgress.counters,{stage:1,enemySceneFrame:17,controlTick:17,
      lastCompletedNetworkFrame:null,confirmedNetworkFrame:null,nextNetworkFrame:null});
    assert.equal(observed.worldProgress.state.touchContext,'gameplay');
    assert.equal(observed.worldProgress.localPlayer.seat,0);
    assert.equal(observed.worldProgress.comparable,true);
    assert.equal(observed.frames.simulation,undefined);
    assertRuntimeObservation(observed, 'phase', {name: 'stage'});
    assert.equal(observed.localSeat,0);
    assertRuntimeObservation(observed, 'input-held', {mask: 1});
    assertRuntimeObservation({...observed, position: {x: 41, y: 80}}, 'position-moved',
      {before: {x: 40, y: 80}, axis: 'x', direction: 'positive'});
  }
});

test('TH08 Replay observation exposes no-playback defaults and the native selected-stage consumption counters',()=>{
  const wasm=wasmFixture(),app=8,slots=Array(10).fill(0);
  wasm.uint32(1024,[0,0,0,0,0]);
  const core={memory:wasm.memory,eagler_replay_state:()=>1024,
    status:(_app,field)=>field>=12?slots[field-12]:[2,1,0,5,0,0,0,0,0,0,0,0][field]};
  const read=()=>withWindow({__th08Runtime:{core,app}},()=>readRuntimeObservation({game:'th08',variant:'normal'}));
  let observed=read();
  assert.deepEqual(observed.replay,{ready:false,state:{frame:0,total:0,ended:false,stage:0,startStage:0},
    stream:{available:false,loaded:false,frame:0,inputCursor:0,inputBytes:0,timingCursor:0,timingBytes:0,stride:0,
      gameFlags:0,inGame:false,stage:0,reportedFrame:0,ended:false,reason:'no Replay stage is loaded'}});

  slots.splice(0,10,1,5,10,10,1,2,2,8,0,5);wasm.uint32(1024,[5,0,1,5,5]);
  observed=read();
  assert.equal(observed.replay.state.ended,true);
  assert.deepEqual(observed.replay.stream,{available:true,loaded:true,frame:5,inputCursor:10,inputBytes:10,
    timingCursor:1,timingBytes:2,stride:2,gameFlags:8,inGame:false,stage:5,reportedFrame:5,ended:true,reason:null});

  slots.splice(0,10,...Array(10).fill(-1));
  observed=read();assert.equal(observed.replay.stream.available,false);
  assert.match(observed.replay.stream.reason,/does not expose/);
});

test('TH08 MP effects select the authoritative local seat and cannot fall back to player zero', () => {
  const wasm=wasmFixture(),app=8;
  wasm.uint32(128,Array(68).fill(0));wasm.float32(128+7*4,[999,888]);
  wasm.int32(512,Array(16).fill(0));wasm.int32(512+11*4,[0x800]);
  const players=Array(44).fill(0);Object.assign(players,{0:1,1:3,2:2,3:1});
  for(let seat=0;seat<3;seat++){
    const offset=8+12*seat;players[offset+5]=40+seat*100;players[offset+6]=80;
    players[offset+9]=1;players[offset+10]=1<<seat;
  }
  wasm.int32(1024,players);wasm.uint32(1280,[3,1,1,71,70,70,0xffffffff,1,0,0,0,0,0,0,0]);
  wasm.uint32(1408,[71,71,0,0,0,0xffffffff]);
  const core={memory:wasm.memory,status:(_app,field)=>[2,1,0,1,0,0,17,0,0,0,0][field],
    trace:()=>128,diagnostics:()=>512,multiplayer_status:()=>1024,multiplayer_netplay_status:()=>1280,
    multiplayer_progress:()=>1408,allocate:()=>1536,deallocate:()=>{},
    touch_state:(_app,pointer)=>wasm.float32(pointer,[1,1,1,0,0,0,0,0,0,0,0,0])};
  const read=()=>withWindow({__th08Runtime:{core,app}},()=>readRuntimeObservation({game:'th08',variant:'multiplayer'}));
  const observed=read();assert.equal(observed.localSeat,2);assert.equal(observed.input.value,4);
  assert.deepEqual(observed.position,{x:240,y:80});assert.equal(observed.frames.multiplayer,70);
  assert.equal(observed.phase.name,'stage');assert.equal(observed.capabilities.restartFrame,null);
  assert.equal(observed.worldProgress.localPlayer.seat,2);
  assert.equal(observed.worldProgress.localPlayer.inputButtons,4);
  assert.equal(observed.worldProgress.netplay.lastCompletedFrame,70);
  assert.equal(observed.worldProgress.netplay.confirmedThrough,70);
  assert.equal(observed.worldProgress.netplay.frontier.predictionFrontier,71);
  assert.equal(observed.worldProgress.netplay.correctionIdle,true);
  assert.equal(observed.worldProgress.netplay.allFramesConfirmed,true);
  assert.equal(observed.worldProgress.netplay.settled,true);
  assert.equal(observed.worldProgress.comparable,true);
  assert.equal(observed.frames.simulation,undefined);
  wasm.uint32(1280,[3,1,1,261,260,259,0xffffffff,1,0,0,0,0,0,0,0]);
  wasm.uint32(1408,[261,261,0,0,0,0xffffffff]);
  const predictedFrame=read();
  assert.equal(predictedFrame.worldProgress.netplay.correctionIdle,true);
  assert.equal(predictedFrame.worldProgress.netplay.allFramesConfirmed,false);
  assert.equal(predictedFrame.worldProgress.netplay.settled,false);
  assert.equal(predictedFrame.worldProgress.comparable,false);
  wasm.uint32(1280,[3,1,1,71,70,70,0xffffffff,1,0,0,0,0,0,0,0]);
  wasm.uint32(1408,[71,71,0,0,0,0xffffffff]);
  const waiting=[...new Uint32Array(wasm.buffer,1280,15)];waiting[9]=1;wasm.uint32(1280,waiting);
  assert.equal(read().worldProgress.netplay.waiting,true);
  assert.equal(read().worldProgress.comparable,false);
  waiting[9]=0;wasm.uint32(1280,waiting);
  const frameOpen=[...new Uint32Array(wasm.buffer,1280,15)];frameOpen[8]=1;wasm.uint32(1280,frameOpen);
  assert.equal(read().worldProgress.netplay.frameOpen,true);
  assert.equal(read().worldProgress.comparable,false);
  frameOpen[8]=0;wasm.uint32(1280,frameOpen);
  const rollbackPending=[...new Uint32Array(wasm.buffer,1280,15)];rollbackPending[6]=69;wasm.uint32(1280,rollbackPending);
  assert.equal(read().worldProgress.netplay.rollbackFrame,69);
  assert.equal(read().worldProgress.comparable,false);
  rollbackPending[6]=0xffffffff;wasm.uint32(1280,rollbackPending);
  const correcting=[...new Uint32Array(wasm.buffer,1408,6)];correcting[2]=1;correcting[5]=72;wasm.uint32(1408,correcting);
  assert.equal(read().worldProgress.netplay.frontier.correcting,true);
  assert.equal(read().worldProgress.comparable,false);
  wasm.uint32(1408,[71,71,0,0,0,0xffffffff]);
  players[2]=-1;wasm.int32(1024,players);
  const missingSeat=read();
  assert.equal(missingSeat.input,null);assert.equal(missingSeat.position,null);
  assert.equal(missingSeat.worldProgress.localPlayer.seat,null);
  assert.equal(missingSeat.worldProgress.localPlayer.inputButtons,null);
  assert.equal(missingSeat.worldProgress.localPlayer.position,null);
  delete core.multiplayer_status;
  const missingExport=read();
  assert.equal(missingExport.capabilities.input,false);assert.equal(missingExport.capabilities.position,false);
  assert.equal(missingExport.worldProgress.comparable,false);
  delete core.multiplayer_progress;
  const missingFrontier=read();
  assert.equal(missingFrontier.worldProgress.netplay.frontier,undefined);
  assert.equal(missingFrontier.worldProgress.netplay.correctionIdle,null);
  assert.equal(missingFrontier.worldProgress.netplay.allFramesConfirmed,true);
  assert.equal(missingFrontier.worldProgress.netplay.settled,false);
  assert.equal(missingFrontier.worldProgress.comparable,false);
});

test('TH08 world progress keeps scene frames, control ticks, dialogue, pause, and loading distinct',()=>{
  const wasm=wasmFixture(),app=8;
  const trace=Array(68).fill(0);trace[2]=20;trace[3]=24;trace[4]=0;trace[9]=0;
  wasm.uint32(128,trace);wasm.float32(128+7*4,[50,60]);
  const diagnostics=Array(16).fill(0);diagnostics[11]=5;wasm.int32(512,diagnostics);
  const status=[2,1,0,3,0,0,20,0,0,0,0];
  wasm.uint32(768,[0,0,0,0,0,40]);
  let touchMode=1,playerAvailable=1;
  const core={memory:wasm.memory,status:(_app,index)=>status[index],trace:()=>128,diagnostics:()=>512,sdl_stats:()=>768,
    allocate:()=>1536,deallocate:()=>{},touch_state:(_app,pointer)=>wasm.float32(pointer,[touchMode,playerAvailable,4,0,0,0,0,0,0,0,0,0])};
  const read=()=>withWindow({__th08Runtime:{core,app}},()=>readRuntimeObservation({game:'th08',variant:'normal'}));
  let observed=read();
  assert.equal(observed.worldProgress.comparable,true);
  assert.equal(observed.worldProgress.counters.enemySceneFrame,20);
  assert.equal(observed.worldProgress.counters.controlTick,24);
  assert.equal(observed.worldProgress.state.dialogue,false);
  assert.equal(observed.worldProgress.state.paused,false);

  diagnostics[6]=2;wasm.int32(512,diagnostics);touchMode=0;
  observed=read();
  assert.equal(observed.worldProgress.state.paused,true);
  assert.equal(observed.worldProgress.state.pauseState,2);
  assert.equal(observed.worldProgress.comparable,true);

  diagnostics[6]=0;wasm.int32(512,diagnostics);touchMode=2;
  trace[2]=20;trace[3]=25;wasm.uint32(128,trace);
  observed=read();
  assert.equal(observed.worldProgress.state.dialogue,true);
  assert.equal(observed.worldProgress.counters.enemySceneFrame,20);
  assert.equal(observed.worldProgress.counters.controlTick,25);
  assert.equal(observed.worldProgress.comparable,true);

  status[7]=1;diagnostics[3]=1;diagnostics[4]=1;wasm.int32(512,diagnostics);touchMode=0;
  observed=read();
  assert.equal(observed.worldProgress.state.loading,true);
  assert.equal(observed.worldProgress.comparable,false);

  status[7]=0;diagnostics[3]=0;diagnostics[4]=0;wasm.int32(512,diagnostics);
  trace[4]=0x20;wasm.uint32(128,trace);
  assert.equal(read().worldProgress.comparable,false);
});

test('TH09 live session phase/frame, pause, player position, and presentation are distinct observations', () => {
  const wasm = wasmFixture();
  wasm.int32(512, [70, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0]);
  wasm.float32(640, [12, 34, 1, -1, 0, 0, 0, 0]);
  wasm.uint32(768, [0, 0, 0, 0, 0, 0, 0, 0, 0, 31]);
  const core = {memory: wasm.memory, _th09_probe_status: () => 512, _th09_touch_probe: () => 640, _th09_game_metrics: () => 768};
  const runtime = {core, status: () => ({title: [70], session: [1, 24, 0, 0, 0, 1, 0, 1], touch: [0, 0, 0, 0]})};
  const observed = withWindow({__th09Runtime: runtime}, () => readRuntimeObservation('th09'));
  assert.deepEqual(observed.phase, {value: 1, name: 'match'});
  assert.equal(observed.frames.session, 24);
  assert.equal(observed.frames.runtime, 70);
  assert.deepEqual(observed.position, {x: 12, y: 34});
  assert.equal(observed.pause.active, true);
  assert.equal(observed.render.presentationFrame, 31);
  assert.deepEqual(observed.touchInput,{context:'inactive',contextValue:0,ready:false,readyValue:0,
    active:false,activeValue:0,inTitle:false,inTitleValue:0});
  assert.throws(() => assertRuntimeObservation(observed, 'input-held'), /no read-only observer/);
  runtime.status=()=>({title:[70],session:[1,25,0,0,0,1,0,1],touch:[1,1,0,0]});
  const gameplay=withWindow({__th09Runtime:runtime},()=>readRuntimeObservation('th09'));
  assert.deepEqual(gameplay.touchInput,{context:'gameplay',contextValue:1,ready:true,readyValue:1,
    active:false,activeValue:0,inTitle:false,inTitleValue:0});
  runtime.status=()=>({title:[70],session:[1,26,0,0,0,1,0,1],touch:[2,1,1,0]});
  const dialogue=withWindow({__th09Runtime:runtime},()=>readRuntimeObservation('th09'));
  assert.deepEqual(dialogue.touchInput,{context:'dialogue-or-ending',contextValue:2,ready:true,readyValue:1,
    active:true,activeValue:1,inTitle:false,inTitleValue:0});
});

test('TH10 production status and world actor expose native input, player movement, and presentation for both variants', () => {
  for (const variant of ['normal', 'multiplayer']) {
    const wasm = wasmFixture(), actor = 1024;
    wasm.view.setFloat32(actor + 0x3c0, 240, true);
    wasm.view.setFloat32(actor + 0x3c4, 300, true);
    wasm.uint32(2048, [0, 0, 0, 0, 0, 91]);
    const core = {memory: wasm.memory, _application_world: () => 8, _world_actor: () => actor,
      _sdl_stats: () => 2048};
    const runtime = {core, app: 4, status: () => [1, 1, 0, 3, 72, 0, 0, 0, 0, 4]};
    const observed = withWindow({__th10Runtime: runtime}, () => readRuntimeObservation({game: 'th10', variant}));
    assert.equal(observed.input.value, 4);
    assert.deepEqual(observed.position, {x: 240, y: 300});
    assert.equal(observed.render.presentationFrame, 91);
    assert.throws(() => assertRuntimeObservation(observed, 'phase', {name: 'stage'}), /no read-only observer/);
    assertRuntimeObservation(observed, 'input-held', {mask: 4});
    assert.throws(() => assertRuntimeObservation(observed, 'restart-frame-reset', {before: 80}), /restart frame is not observable/);
  }
});

test('TH11 reads raw window.core exports from the actual shell, while MP restart remains explicitly unsupported', () => {
  const core = {th11_phase: () => 2, th11_frame: () => 85};
  const module = {HEAPU8: new Uint8Array(4096)};
  const observed = withWindow({Module: module, core}, () => readRuntimeObservation({game: 'th11', variant: 'normal'}));
  assert.deepEqual(observed.phase, {value: 2, name: 'paused'});
  assert.equal(observed.frames.session, 85);
  assertRuntimeObservation(observed, 'pause-active');
  assertRuntimeObservation({...observed, phase: {value: 1, name: 'stage'}}, 'phase', {name: 'stage'});
  const multiplayer = withWindow({Module: module, core}, () => readRuntimeObservation({game: 'th11', variant: 'multiplayer'}));
  assert.equal(multiplayer.capabilities.restartFrame, null);
  assert.throws(() => assertRuntimeObservation(multiplayer, 'restart-frame-reset', {before: 85}), /restart frame is not observable/);
  assert.throws(() => assertRuntimeObservation(observed, 'position-moved', {before: {x: 1, y: 2}, axis: 'x'}), /no read-only observer/);
  const moduleOnly = {_th11_phase: () => 1, _th11_frame: () => 86};
  const emscripten = withWindow({Module: moduleOnly}, () => readRuntimeObservation({game: 'th11', variant: 'normal'}));
  assert.deepEqual(emscripten.phase, {value: 1, name: 'stage'});
  assert.equal(emscripten.frames.session, 86);
});

test('TH15 phase is semantic but its application frame does not advertise restart or pause freeze', () => {
  const module = {_th15_phase: () => 2, _th15_frame: () => 301};
  const observed = withWindow({Module: module}, () => readRuntimeObservation({game: 'th15', variant: 'normal'}));
  assert.deepEqual(observed.phase, {value: 2, name: 'paused'});
  assert.equal(observed.frames.application, 301);
  assertRuntimeObservation(observed, 'pause-active');
  assert.throws(() => assertRuntimeObservation(observed, 'restart-frame-reset', {before: 300}), /restart frame is not observable/);
  assert.throws(() => assertRuntimeObservation(observed, 'presentation-advanced', {after: 300}), /no read-only observer/);
});

test('th15 input context explains unavailable gameplay input without making the action pass',()=>{
  const wasm=wasmFixture();
  wasm.uint32(128,[1,0x1ed,7,0xffffffff,321,0xffffffff,0,320,0,0,321,0]);
  wasm.float32(128+8*4,[-48,400]);
  const context=[1,0x3ff,1,1,1,0,1,1,0,4,0,0];wasm.uint32(512,context);
  const core={memory:wasm.memory,_th15_native_observation_v1:()=>128,_th15_input_context_v1:()=>512};
  const read=()=>withWindow({core,__nativeRuntime:{core}},()=>readRuntimeObservation(nativeTarget('th15','multiplayer')));
  const observed=read();
  assert.equal(observed.phase.name,'transition');assert.equal(observed.input,null);
  assert.equal(observed.capabilities.input,false);
  assert.deepEqual(observed.inputContext,{version:1,validMask:0x3ff,initialized:true,scenePresent:true,
    multiplayer:true,localSeat:0,pilotPresent:true,playerPresent:true,ghost:false,lifeState:4,sceneFlags:0,messagesActive:false});
  assert.throws(()=>assertRuntimeObservation(observed,'input-held'),/no read-only observer/);
  wasm.uint32(512,[...context.slice(0,5),2,...context.slice(6)]);
  assert.throws(read,/different local seat/);
  wasm.uint32(512,[2,...context.slice(1)]);assert.throws(read,/unsupported input context ABI/);
  wasm.uint32(512,[1,0x3fe,...context.slice(2)]);assert.throws(read,/inconsistent input context field 2/);
  wasm.uint32(512,[1,0,...Array(10).fill(0xffffffff)]);
  assert.equal(read().inputContext.lifeState,null);assert.equal(read().inputContext.localSeat,null);
  delete core._th15_input_context_v1;
  assert.equal(read().inputContext,undefined);assert.equal(read().capabilities.input,false);
});

test('TH20 reports native input and presentation without calling its pulse-consuming touch probe', () => {
  const wasm = wasmFixture();
  wasm.uint32(768, [0, 0, 0, 0, 0, 44]);
  let touched = false;
  const core = {memory: wasm.memory, sdl_stats: () => 768, sdl_touch_probe: () => { touched = true; throw Error('must not be called'); }};
  const runtime = {core, status: () => [4, 0, 0, 1, 0, 0, 0, 2, 0, 16]};
  const observed = withWindow({__th20Runtime: runtime}, () => readRuntimeObservation('th20'));
  assert.equal(touched, false);
  assert.equal(observed.status.scene, 4);
  assert.equal(observed.input.value, 16);
  assert.equal(observed.render.presentationFrame, 44);
  assertRuntimeObservation(observed, 'input-held', {mask: 16});
  assert.throws(() => assertRuntimeObservation(observed, 'pause-active'), /no read-only observer/);
  assert.throws(() => assertRuntimeObservation(observed, 'position-moved', {before: {x: 0, y: 0}, axis: 'x'}), /no read-only observer/);
});

test('TH06/07 replay readiness and rendering cannot be mistaken for live gameplay or keyboard behavior', () => {
  const wasm = wasmFixture();
  wasm.uint32(256, [9, 120, 0, 2, 1]);
  const module = {_eagler_replay_ready: () => 1, _eagler_replay_state: () => 256, HEAPU8: new Uint8Array(wasm.buffer)};
  const th06 = withWindow({Module: module}, () => readRuntimeObservation('th06'));
  const th07 = withWindow({Module: module, __eaglerDebugRenderCallbacks: 24}, () => readRuntimeObservation('th07'));
  assert.equal(th06.replay.ready, true);
  assert.deepEqual(th06.replay.state, {frame: 9, total: 120, ended: false, stage: 2, startStage: 1});
  assert.equal(th07.render.renderCallbacks, 24);
  for (const observed of [th06, th07]) {
    assert.throws(() => assertRuntimeObservation(observed, 'phase', {name: 'stage'}), /no read-only observer/);
    assert.throws(() => assertRuntimeObservation(observed, 'input-held'), /no read-only observer/);
    assert.throws(() => assertRuntimeObservation(observed, 'restart-frame-reset', {before: 10}), /restart frame is not observable/);
  }
});

test('classic Emscripten declarations bind the native ABI to the Module-owned shell heap', () => {
  for(const game of ['th06','th07']){
    const wasm=wasmFixture();
    wasm.uint32(256,[1,0x123,1,16,0xffffffff,0xffffffff,0,0xffffffff,0x7fc00000,0x7fc00000,0xffffffff,0]);
    const module={HEAPU8:new Uint8Array(wasm.buffer),[`_${game}_native_observation_v1`]:()=>256};
    const observed=withWindow({HEAPU8:module.HEAPU8,Module:module,__nativeRuntime:{Module:module}},
      ()=>readRuntimeObservation(nativeTarget(game)));
    assert.equal(observed.nativeObservation.version,1);
    assert.equal(observed.phase.name,'stage');assert.equal(observed.input.value,16);assert.equal(observed.localSeat,0);
  }
});

test('TH20 retry readiness and stone navigation come from independent read-only UI state', () => {
  const wasm=wasmFixture();
  const core={memory:wasm.memory,th20_ui_observation_v1:()=>256};
  const read=()=>withWindow({core},()=>readRuntimeObservation({game:'th20',variant:'normal'}));
  wasm.uint32(256,[1,3,1,7,1,0,3,1,0]);
  assert.deepEqual(read().ui.pause,{state:1,substate:7,cursor:1,retryConfirmInputReady:false});
  wasm.uint32(256,[1,3,1,7,0,1,5,3,0]);
  assert.equal(read().ui.pause.retryConfirmInputReady,true);assert.equal(read().ui.stone.visible,3);
  wasm.uint32(256,[1,0,-1,-1,-1,-1,-1,-1,-1]);
  assert.equal(read().ui.pause,null);assert.equal(read().ui.stone,null);
  wasm.uint32(256,[1,0,1,7,0,1,-1,-1,-1]);
  assert.throws(read,/invalid UI observation group/);
  wasm.uint32(256,[1,1,1,7,0,2,-1,-1,-1]);
  assert.throws(read,/invalid retry-confirmation readiness/);
});

test('negative controls reject missing progress, unchanged position, and unobservable actions', () => {
  const base = {game: 'fixture', capabilities: {phase: true, input: true, position: true, pause: false,
    restartFrame: null, presentationFrame: false}, phase: {name: 'stage'}, input: {value: 0}, frames: {session: 5}, position: {x: 4, y: 8}};
  assert.throws(() => assertRuntimeObservation(base, 'input-held'), /no native input is held/);
  assert.throws(() => assertRuntimeObservation({...base, input:{}}, 'input-held'), /native input state is unavailable/);
  assertRuntimeObservation({...base, input:{value:4}}, 'input-released', {mask:1});
  assert.throws(() => assertRuntimeObservation({...base, input:{value:5}}, 'input-released', {mask:1}), /mask remains held/);
  assert.throws(() => assertRuntimeObservation(base, 'frame-advanced', {key: 'session', before: 5}), /did not advance/);
  assert.throws(() => assertRuntimeObservation(base, 'position-moved', {before: {x: 4, y: 8}, axis: 'x'}), /position did not change/);
  assert.throws(() => assertRuntimeObservation(base, 'pause-active'), /no read-only observer/);
});

test('the browser-side reader has no imported closure dependencies', () => {
  const serialized = new Function(`return (${readRuntimeObservation.toString()})`)();
  const core = {th11_phase: () => 1, th11_frame: () => 12};
  const observed = withWindow({Module: {HEAPU8: new Uint8Array(256)}, core}, () => serialized({game: 'th11', variant: 'normal'}));
  assert.deepEqual(observed.phase, {value: 1, name: 'stage'});
  assert.equal(observed.frames.session, 12);
});

test('a new title uses the shared ABI reader with declared mounting, export and input sampling semantics',()=>{
  const wasm=wasmFixture(),words=[1,0x13f,1,128,20,20,0,0xffffffff,0,0,0xffffffff,0];
  const positions=new Float32Array([12.5,27.75]);
  words[8]=new Uint32Array(positions.buffer)[0];words[9]=new Uint32Array(positions.buffer)[1];
  wasm.uint32(128,words);
  const request={game:'next',variant:'normal',adapter:{runtime:{global:'__next',core:'exports',module:'Module',app:'instance'},
    observation:{kind:'native-v1',export:'observe_state',source:'ports/next/runtime.cpp | read_state',
      sampleBoundary:'after-application-update',inputSemantics:'queued-for-next-movement'}}};
  const serialized=new Function(`return (${readRuntimeObservation.toString()})`)();
  const core={memory:wasm.memory,observe_state:()=>128};
  withWindow({__next:{exports:core}},()=>{
    const observed=serialized(request);
    assert.deepEqual(observed.position,{x:12.5,y:27.75});
    assert.deepEqual(observed.phase,{value:1,name:'stage'});
    assert.equal(observed.frames.simulation,20);assert.equal(observed.localSeat,0);
    assert.equal(observed.input.kind,'native-queued-for-next-movement-buttons');
    assert.equal(observed.input.sampleBoundary,'after-application-update');
    assert.ok(!observed.unsupported.includes('unknown-runtime'));
    delete core.observe_state;
    assert.throws(()=>serialized(request),/declared native observation .* is unavailable/);
  });
});

test('a declared Runtime never borrows another mount, exports or wasm memory from the browser realm',()=>{
  const wasm=wasmFixture();wasm.uint32(128,[1,0x13f,1,128,20,20,0,0xffffffff,0,0,0xffffffff,0]);
  const request={game:'next',variant:'normal',adapter:{runtime:{global:'__next',core:'exports',module:'engine',app:'instance'},
    observation:{kind:'native-v1',export:'observe_state',source:'ports/next/runtime.cpp | read_state',
      sampleBoundary:'after-application-update',inputSemantics:'consumed'}}};
  const serialized=new Function(`return (${readRuntimeObservation.toString()})`)();
  const otherCore={memory:wasm.memory,observe_state:()=>128};
  const otherRealm={core:otherCore,Module:{HEAPU8:new Uint8Array(wasm.memory.buffer),...otherCore},
    HEAPU8:new Uint8Array(wasm.memory.buffer)};
  withWindow(otherRealm,()=>assert.throws(()=>serialized({...request,adapter:{observation:request.adapter.observation}}),
    /native-v1 observation requires its declared Runtime mapping/));
  withWindow(otherRealm,()=>assert.throws(()=>serialized(request),/declared Runtime mount __next is unavailable/));
  withWindow({...otherRealm,__next:{}},()=>assert.throws(()=>serialized(request),/declared native observation .* is unavailable/));
  withWindow({...otherRealm,__next:{exports:{observe_state:()=>128}}},()=>
    assert.throws(()=>serialized(request),/declared native observation .* is unavailable/));
  withWindow({...otherRealm,__next:{engine:otherCore}},()=>{
    assert.equal(serialized(request).frames.simulation,20,'declared Module-owned exports remain supported');
  });
});
