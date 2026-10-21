import assert from 'node:assert/strict';
import test from 'node:test';
import {runInNewContext} from 'node:vm';
import {createAdapterRegistry} from '../../tests/adaptation/adapters/registry.mjs';
import {readRuntimeObservation,runtimeObservationRequest} from './runtime-observations.mjs';
import {
  assertTouchMotionSamples,
  assertTouchReleaseSamples,
  decodeTouchMotionTrace,
  decodeTouchMotionTraceResult,
  readTouchMotionBoundary,
  readTouchMotionTrace,
  mapFloat32TouchDragTarget,
  predictTouchMotionStep,
  isTouchMotionTargetReached,
} from './touch-motion-contract.mjs';

// Independent fixture values follow the vector normalization in the verified
// th06 Player::HandlePlayerInputs touch path (ports/th06/src/Player.cpp): a requested
// (6, 8) delta has Euclidean length 10, so speed 4 consumes (2.4, 3.2), then
// (2.4, 3.2), then the remaining (1.2, 1.6). th08 applies the same rule in
// portable/input/MotionTrack.hpp::limit_vector before PlayerMovement.cpp adds
// the delta to the authoritative world position. These observations are fixed
// literals; the oracle under test does not generate them.
const sample = ({sequence,frame,beforeX,beforeY,targetX,targetY,afterX,afterY,
  mode=1,flags=1,speed=4,scaleX=1,scaleY=1,sensitivity=1,abiVersion=2,localSeat=0,executionKind=0}={}) => ({
  sequence,frame,mode,flags,abiVersion,localSeat,executionKind,beforeX,beforeY,targetX,targetY,afterX,afterY,
  speed,scaleX,scaleY,sensitivity,
});

const diagonal = () => [
  sample({sequence:1,frame:0,beforeX:0,beforeY:0,targetX:6,targetY:8,afterX:2.4,afterY:3.2}),
  sample({sequence:2,frame:1,beforeX:2.4,beforeY:3.2,targetX:6,targetY:8,afterX:4.8,afterY:6.4}),
  sample({sequence:3,frame:2,beforeX:4.8,beforeY:6.4,targetX:6,targetY:8,afterX:6,afterY:8}),
];

const micro = () => [
  sample({sequence:1,frame:0,beforeX:0,beforeY:0,targetX:.3,targetY:.4,afterX:.3,afterY:.4}),
  sample({sequence:2,frame:1,beforeX:.3,beforeY:.4,targetX:.3,targetY:.4,afterX:.3,afterY:.4}),
  sample({sequence:3,frame:2,beforeX:.3,beforeY:.4,targetX:.3,targetY:.4,afterX:.3,afterY:.4}),
];

test('standalone motion and release assertions require advancing ABI v2 forward rows and a valid independent target',()=>{
  const valid=diagonal();
  assert.equal(assertTouchMotionSamples(valid,{mode:1,target:{x:6,y:8},speed:4}).arrived,true);
  for(const [label,mutate,pattern] of [
    ['missing ABI',rows=>{delete rows[0].abiVersion;},/requires ABI v2/],
    ['legacy ABI',rows=>{rows[0].abiVersion=1;},/requires ABI v2/],
    ['other seat',rows=>{rows[1].localSeat=1;},/changed local-seat stream/],
    ['replay stream',rows=>{rows[1].executionKind=1;},/changed execution stream/],
    ['sequence stall',rows=>{rows[1].sequence=rows[0].sequence;},/Missing native trace sequence/],
    ['frame stall',rows=>{rows[1].frame=rows[0].frame;},/Missing native movement tick/],
  ]){
    const rows=valid.map(row=>({...row}));mutate(rows);
    assert.throws(()=>assertTouchMotionSamples(rows,{mode:1,target:{x:6,y:8},speed:4}),pattern,label);
  }
  assert.throws(()=>assertTouchMotionSamples(valid,{mode:1,speed:4}),/expected target X/);
  assert.throws(()=>assertTouchMotionSamples(valid,{mode:1,target:{x:6,y:8},targets:valid.map(()=>({x:6,y:8})),speed:4}),
    /one independent target source/);

  const released=[
    sample({sequence:21,frame:50,beforeX:2.4,beforeY:3.2,targetX:2.4,targetY:3.2,afterX:2.4,afterY:3.2,mode:0}),
    sample({sequence:22,frame:51,beforeX:2.4,beforeY:3.2,targetX:2.4,targetY:3.2,afterX:2.4,afterY:3.2,mode:0}),
    sample({sequence:23,frame:52,beforeX:2.4,beforeY:3.2,targetX:2.4,targetY:3.2,afterX:2.4,afterY:3.2,mode:0}),
    sample({sequence:24,frame:53,beforeX:2.4,beforeY:3.2,targetX:2.4,targetY:3.2,afterX:2.4,afterY:3.2,mode:0}),
  ];
  assert.deepEqual(assertTouchReleaseSamples(released.slice(0,3),{position:{x:2.4,y:3.2},afterSequence:20}),{ticks:3,stationary:true});
  const badTarget=released.map(row=>({...row}));badTarget[1].targetX=2.5;
  assert.throws(()=>assertTouchReleaseSamples(badTarget,{position:{x:2.4,y:3.2},afterSequence:20}),/Position drift after release \(targetX\)/);
  assert.throws(()=>assertTouchReleaseSamples(released.slice(1),{position:{x:2.4,y:3.2},afterSequence:20}),
    /omitted the first native tick/);
});

test('th10 expected world target models the normalized float32 command before 1/100 movement rounding',()=>{
  const request=.145000001;
  const command=mapFloat32TouchDragTarget({x:0,y:0},{x:request,y:0},{
    inputSpace:{width:640,height:480},pixelsPerUnit:{x:640,y:480},sensitivity:1,focusFactor:1,
  });
  assert.equal(command.x,0.14499664306640625);
  const precision={unitsPerPixel:100,rounding:'nearest',limitedQuantization:'nearest-within-speed',scalePlacement:'before-limit'};
  assert.deepEqual(predictTouchMotionStep({x:0,y:0},command,{mode:1,speed:4,precision}),{x:.14,y:0});
  assert.deepEqual(predictTouchMotionStep({x:0,y:0},{x:request,y:0},{mode:1,speed:4,precision}),{x:.15,y:0},
    'the former double-only expected target crosses th10’s hundredths rounding boundary');
});

test('fixed native arithmetic preserves its own resolution without widening comparison tolerance',()=>{
  const integer128={unitsPerPixel:128,rounding:'toward-zero',scalePlacement:'after-limit'};
  const integer100={unitsPerPixel:100,rounding:'nearest',scalePlacement:'before-limit'};
  assert.deepEqual(predictTouchMotionStep({x:0,y:0},{x:.3,y:.4},{mode:1,speed:4,precision:integer128}),
    {x:38/128,y:51/128});
  assert.deepEqual(predictTouchMotionStep({x:0,y:0},{x:-.3,y:-.4},{mode:1,speed:4,precision:integer128}),
    {x:-38/128,y:-51/128});
  assert.deepEqual(predictTouchMotionStep({x:0,y:0},{x:.305,y:-.405},{mode:1,speed:4,precision:integer100}),
    {x:31/100,y:-41/100});
  assert.equal(isTouchMotionTargetReached({x:38/128,y:51/128},{x:.3,y:.4},{precision:integer128}),true);
  assert.equal(isTouchMotionTargetReached({x:37/128,y:51/128},{x:.3,y:.4},{precision:integer128}),false);
  const rows=[sample({sequence:1,frame:0,beforeX:0,beforeY:0,targetX:.3,targetY:.4,afterX:38/128,afterY:51/128}),
    sample({sequence:2,frame:1,beforeX:38/128,beforeY:51/128,targetX:.3,targetY:.4,afterX:38/128,afterY:51/128}),
    sample({sequence:3,frame:2,beforeX:38/128,beforeY:51/128,targetX:.3,targetY:.4,afterX:38/128,afterY:51/128})];
  assert.equal(assertTouchMotionSamples(rows,{mode:1,target:{x:.3,y:.4},speed:4,precision:integer128}).arrived,true);
  rows[0].afterX=37/128;
  assert.throws(()=>assertTouchMotionSamples(rows,{mode:1,target:{x:.3,y:.4},speed:4,precision:integer128}),/Per-tick touch position X/);
});

test('native rounding is never permission to exceed the independent speed cap',()=>{
  const precision={unitsPerPixel:100,rounding:'nearest',scalePlacement:'before-limit'};
  // A 43-degree capped vector rounds to (293,273) native hundredths, whose
  // length exceeds 400. Matching native arithmetic must still reject that cap violation.
  const target={x:7.313537016191705,y:6.819983600624985};
  const first=predictTouchMotionStep({x:0,y:0},target,{mode:1,speed:4,precision});
  assert.deepEqual(first,{x:2.93,y:2.73});
  const rows=[sample({sequence:1,frame:0,beforeX:0,beforeY:0,targetX:target.x,targetY:target.y,afterX:first.x,afterY:first.y}),
    sample({sequence:2,frame:1,beforeX:first.x,beforeY:first.y,targetX:target.x,targetY:target.y,afterX:first.x,afterY:first.y}),
    sample({sequence:3,frame:2,beforeX:first.x,beforeY:first.y,targetX:target.x,targetY:target.y,afterX:first.x,afterY:first.y})];
  assert.throws(()=>assertTouchMotionSamples(rows,{mode:1,target,speed:4,precision}),/exceeded original Euclidean speed cap/);
});

test('limited fixed-point movement selects the nearest legal point while micro and unlimited movement retain resolution',()=>{
  const precision={unitsPerPixel:100,rounding:'nearest',limitedQuantization:'nearest-within-speed',scalePlacement:'before-limit'};
  const target={x:2.93,y:2.73};
  assert.deepEqual(predictTouchMotionStep({x:0,y:0},target,{mode:1,speed:4,precision}),{x:2.92,y:2.73});
  assert.deepEqual(predictTouchMotionStep({x:0,y:0},{x:-2.93,y:-2.73},{mode:1,speed:4,precision}),{x:-2.92,y:-2.73});
  assert.deepEqual(predictTouchMotionStep({x:0,y:0},{x:.3,y:.4},{mode:1,speed:4,precision}),{x:.3,y:.4});
  assert.deepEqual(predictTouchMotionStep({x:0,y:0},target,{mode:2,speed:4,precision}),target);
});

test('limited drag follows a sub-speed 0.3,0.4 target without dropping the micro-move', () => {
  const result=assertTouchMotionSamples(micro(),{mode:1,target:{x:.3,y:.4},speed:4});
  assert.equal(result.arrived,true);
  assert.equal(result.saturated,0);
});

test('a native adapter may return to digital neutral only after independently reaching the requested target',()=>{
  const rows=micro();
  for(const row of rows.slice(1)){row.mode=0;row.targetX=row.beforeX;row.targetY=row.beforeY;}
  assert.equal(assertTouchMotionSamples(rows,{mode:1,target:{x:.3,y:.4},speed:4}).arrived,true);
  rows[0].mode=0;rows[0].targetX=rows[0].beforeX;rows[0].targetY=rows[0].beforeY;
  assert.throws(()=>assertTouchMotionSamples(rows,{mode:1,target:{x:.3,y:.4},speed:4}),/mode was not consumed/);
});

test('limited 6,8 diagonal consumes exactly 2.4,3.2 per tick and arrives on tick three', () => {
  const result=assertTouchMotionSamples(diagonal(),{mode:1,target:{x:6,y:8},speed:4});
  assert.deepEqual({ticks:result.ticks,saturated:result.saturated,arrived:result.arrived},
    {ticks:3,saturated:2,arrived:true});
});

test('unlimited direct movement reaches the same diagonal target on its first tick', () => {
  const samples=[
    sample({sequence:1,frame:0,beforeX:0,beforeY:0,targetX:6,targetY:8,afterX:6,afterY:8,mode:2}),
    sample({sequence:2,frame:1,beforeX:6,beforeY:8,targetX:6,targetY:8,afterX:6,afterY:8,mode:2}),
    sample({sequence:3,frame:2,beforeX:6,beforeY:8,targetX:6,targetY:8,afterX:6,afterY:8,mode:2}),
  ];
  assert.equal(assertTouchMotionSamples(samples,{mode:2,target:{x:6,y:8},speed:4}).arrived,true);
});

test('free joystick validates independently specified targets on every tick and rejects quantization',()=>{
  const samples=[
    sample({sequence:1,frame:0,beforeX:0,beforeY:0,targetX:1.2,targetY:1.6,afterX:1.2,afterY:1.6}),
    sample({sequence:2,frame:1,beforeX:1.2,beforeY:1.6,targetX:2.4,targetY:3.2,afterX:2.4,afterY:3.2}),
    sample({sequence:3,frame:2,beforeX:2.4,beforeY:3.2,targetX:3.6,targetY:4.8,afterX:3.6,afterY:4.8}),
  ],targets=[{x:1.2,y:1.6},{x:2.4,y:3.2},{x:3.6,y:4.8}];
  assert.equal(assertTouchMotionSamples(samples,{mode:1,targets,speed:4}).subLimit,3);
  samples[1].afterX=2;samples[1].afterY=4;
  assert.throws(()=>assertTouchMotionSamples(samples,{mode:1,targets,speed:4}),/Per-tick touch position/);
  assert.throws(()=>assertTouchMotionSamples(samples,{mode:1,targets:targets.slice(1),speed:4}),/Each native tick/);
});

test('100, 200, and 300 percent sensitivity use independently known mapped targets', () => {
  const cases=[
    {percent:100,factor:1,target:{x:.3,y:.4}},
    {percent:200,factor:2,target:{x:.6,y:.8}},
    {percent:300,factor:3,target:{x:.9,y:1.2}},
  ];
  for(const {percent,factor,target} of cases){
    const samples=[
      sample({sequence:1,frame:0,beforeX:0,beforeY:0,targetX:target.x,targetY:target.y,
        afterX:target.x,afterY:target.y,sensitivity:factor}),
      sample({sequence:2,frame:1,beforeX:target.x,beforeY:target.y,targetX:target.x,targetY:target.y,
        afterX:target.x,afterY:target.y,sensitivity:factor}),
      sample({sequence:3,frame:2,beforeX:target.x,beforeY:target.y,targetX:target.x,targetY:target.y,
        afterX:target.x,afterY:target.y,sensitivity:factor}),
    ];
    assert.equal(assertTouchMotionSamples(samples,{mode:1,target,speed:4,sensitivity:factor}).arrived,true,
      `${percent}% sensitivity fixture`);
  }
});

test('oracle rejects same-mode substitution, eight-way quantization, teleport, per-axis clipping, micro-drop and decimal truncation', () => {
  const cases=[
    ['limited mode reported as unlimited',samples=>{samples[0].mode=2;}],
    ['free angle quantized to an octant',samples=>{samples[0].afterX=2.828427;samples[0].afterY=2.828427;}],
    ['limited target teleported in one tick',samples=>{samples[0].afterX=6;samples[0].afterY=8;}],
    ['Euclidean speed incorrectly clipped per axis',samples=>{samples[0].afterX=4;samples[0].afterY=4;}],
    ['micro target discarded',samples=>{samples[0].afterX=0;samples[0].afterY=0;}],
    ['fractional coordinates truncated',samples=>{samples[0].afterX=2;samples[0].afterY=3;}],
    ['old target reused after requested target changed',samples=>{samples[1].targetX=5;samples[1].targetY=7;}],
  ];
  for(const [label,mutate] of cases){
    const samples=diagonal();mutate(samples);
    assert.throws(()=>assertTouchMotionSamples(samples,{mode:1,target:{x:6,y:8},speed:4}),undefined,label);
  }
});

test('oracle rejects a missing or duplicate native movement tick', () => {
  const fourTicks=[
    sample({sequence:10,frame:20,beforeX:0,beforeY:0,targetX:10,targetY:0,afterX:4,afterY:0}),
    sample({sequence:11,frame:21,beforeX:4,beforeY:0,targetX:10,targetY:0,afterX:8,afterY:0}),
    sample({sequence:12,frame:22,beforeX:8,beforeY:0,targetX:10,targetY:0,afterX:10,afterY:0}),
    sample({sequence:13,frame:23,beforeX:10,beforeY:0,targetX:10,targetY:0,afterX:10,afterY:0}),
  ];
  const missing=fourTicks.filter((_,index)=>index!==1);
  assert.throws(()=>assertTouchMotionSamples(missing,{mode:1,target:{x:10,y:0},speed:4}),/Missing native trace sequence/);
  const repeated=[fourTicks[0],fourTicks[1],{...fourTicks[1]},fourTicks[3]];
  assert.throws(()=>assertTouchMotionSamples(repeated,{mode:1,target:{x:10,y:0},speed:4}),/Missing native trace sequence/);
  const frameGap=fourTicks.map(item=>({...item}));frameGap[2].frame++;
  assert.throws(()=>assertTouchMotionSamples(frameGap,{mode:1,target:{x:10,y:0},speed:4}),/Missing native movement tick/);
});

test('oracle rejects non-finite samples and release drift', () => {
  const notFinite=diagonal();notFinite[1].afterY=Number.NaN;
  assert.throws(()=>assertTouchMotionSamples(notFinite,{mode:1,target:{x:6,y:8},speed:4}),/afterY must be finite/);

  const released=[
    sample({sequence:21,frame:50,beforeX:2.4,beforeY:3.2,targetX:2.4,targetY:3.2,afterX:2.4,afterY:3.2,mode:0}),
    sample({sequence:22,frame:51,beforeX:2.4,beforeY:3.2,targetX:2.4,targetY:3.2,afterX:2.4,afterY:3.2,mode:0}),
    sample({sequence:23,frame:52,beforeX:2.4,beforeY:3.2,targetX:2.4,targetY:3.2,afterX:2.4,afterY:3.2,mode:0}),
  ];
  assert.deepEqual(assertTouchReleaseSamples(released,{position:{x:2.4,y:3.2},afterSequence:20}),
    {ticks:3,stationary:true});
  released[2].afterY=3.201;
  assert.throws(()=>assertTouchReleaseSamples(released,{position:{x:2.4,y:3.2},afterSequence:20}),/Position drift after release/);
});

function f32bits(value){
  const storage=new ArrayBuffer(4);new DataView(storage).setFloat32(0,value,true);
  return new DataView(storage).getUint32(0,true);
}
function wasmImage({pointer=64,header=[1,8,16,512,1,1,0,0],rows=[],bytes=1024,exportVersion=1}={}){
  const buffer=new ArrayBuffer(bytes),words=new Uint32Array(buffer);
  if(pointer+header.length*4<=bytes)words.set(header,pointer/4);
  if(pointer+header.length*4+rows.length*64<=bytes)
    rows.forEach((row,index)=>words.set(row,pointer/4+8+index*16));
  const calls=[],Module={HEAPU8:new Uint8Array(buffer),[`_sdl_touch_trace_v${exportVersion}`]:(...args)=>{calls.push(args);return pointer;}};
  const context={Module};
  const read=runInNewContext(`(${readTouchMotionTrace.toString()})`,context);
  return{buffer,Module,context,read,calls};
}
function rawRow({sequence=1,frame=0,mode=1,flags=1,beforeX=0,beforeY=0,targetX=.3,targetY=.4,
  afterX=.3,afterY=.4,speed=4,scaleX=1,scaleY=1,sensitivity=1,localSeat=0,executionKind=0}={}){
  return[sequence,frame,mode,flags,...[beforeX,beforeY,targetX,targetY,afterX,afterY,speed,scaleX,scaleY,sensitivity].map(f32bits),localSeat,executionKind];
}

test('a declared touch trace follows the selected mount and never borrows another realm heap',()=>{
  const f=wasmImage({exportVersion:2,header:[2,8,16,512,1,1,0,0],rows:[rawRow()]});
  const request={game:'next',adapter:{runtime:{global:'__nextRuntime',core:'api',module:'engine',app:'instance'}}};
  const context={Module:f.Module,core:f.Module,HEAPU8:f.Module.HEAPU8};
  const read=runInNewContext(`(${readTouchMotionTrace.toString()})`,context);
  assert.equal(read(request).available,false,'another global Module is not the selected Runtime');
  context.__nextRuntime={engine:f.Module};
  assert.equal(read(request).available,true,'non-Module mounting names use the same ABI reader');
  context.__nextRuntime={api:{_sdl_touch_trace_v2:f.Module._sdl_touch_trace_v2}};
  assert.throws(()=>read(request),/Invalid touch trace pointer/,'a foreign global heap cannot complete the read');
});

test('version 2 reads the explicitly selected seat and execution stream without combining their sequences',()=>{
  const fixture=wasmImage({exportVersion:2,header:[2,8,16,512,1,1,2,0],rows:[rawRow({localSeat:2})]});
  const captured=fixture.read({game:'th09',localSeat:2,executionKind:0});
  assert.deepEqual(fixture.calls,[[2,0]]);
  const decoded=decodeTouchMotionTrace(Array.from(captured.raw));
  assert.equal(decoded.abiVersion,2);assert.equal(decoded.localSeat,2);assert.equal(decoded.executionKind,0);
  assert.equal(decoded.samples[0].localSeat,2);assert.equal(decoded.samples[0].sequence,1);
  assert.throws(()=>fixture.read({game:'th09',localSeat:1}),/Invalid touch trace ABI header/);
  const mixed=Array.from(captured.raw);mixed[8+15]=2;
  assert.throws(()=>decodeTouchMotionTrace(mixed),/another execution stream/);
});

test('decoded reader results state availability and retain non-empty unavailable reasons',()=>{
  const fixture=wasmImage({exportVersion:2,header:[2,8,16,512,1,1,0,0],rows:[rawRow()]});
  const decoded=decodeTouchMotionTraceResult(fixture.read({game:'th08'}));
  assert.equal(decoded.available,true);
  assert.equal(decoded.abiVersion,2);
  assert.equal(decoded.total,1);
  assert.equal(decoded.samples.length,1);
  assert.deepEqual(decodeTouchMotionTraceResult({available:false,reason:'No native trace export'}),
    {available:false,reason:'No native trace export'});
  assert.deepEqual(decodeTouchMotionTraceResult({available:false,reason:''}),
    {available:false,reason:'Native touch trace is unavailable'});
  assert.throws(()=>decodeTouchMotionTraceResult({raw:[]}),/must explicitly declare availability/);
  assert.throws(()=>decodeTouchMotionTraceResult({available:true,raw:[]}),/Touch trace header is missing/);
});

function serializedRuntimeAndTouchReaders(fixture,request,runtime){
  const context={Module:fixture.Module,[request.adapter.runtime.global]:runtime};context.window=context;
  return{
    readObservation:()=>structuredClone(runInNewContext(`(${readRuntimeObservation.toString()})`,context)(request)),
    readTrace:()=>runInNewContext(`(${readTouchMotionTrace.toString()})`,context)(request),
  };
}

function th08RuntimeFixture(fixture,inputValue){
  const words=new Uint32Array(fixture.buffer);
  words.set(Array(68).fill(0),256/4);new Float32Array(fixture.buffer,256+7*4,2).set([40,80]);
  const diagnostics=Array(16).fill(0);diagnostics[11]=inputValue;words.set(diagnostics.map(value=>value>>>0),768/4);
  const status=[2,1,0,1,0,0,17,0,0,0,0];
  return{core:{memory:fixture.buffer,status:(_app,index)=>status[index],trace:()=>256,diagnostics:()=>768},Module:fixture.Module,app:8};
}

function nativeV1FixtureAdapter({inputSemantics='queued-for-next-movement',source='ports/th09/web/cpp/sdl/Runtime.cpp | observe_input',
  sampleBoundary='after-player-input-update'}={}){
  const expectedSource='ports/th09/web/cpp/sdl/Runtime.cpp | observe_input';
  const expectedBoundary='after-player-input-update';
  return createAdapterRegistry([{
    schema:'eagler-touhou/adaptation-adapter/1',game:'th09',variant:'normal',
    runtime:{global:'__th09NativeFixture',core:'exports',module:'Module',app:'app'},
    observation:{kind:'native-v1',export:'th09_input_observation_v1',source,sampleBoundary,inputSemantics},
    movement:{source:'ports/th09/web/cpp/game/PlayerMotion.cpp | update',inputBoundary:'queued-for-next-movement',
      inputBoundarySource:expectedSource,precision:{unitsPerPixel:1,rounding:'float',scalePlacement:'before-limit'},
      keyboardReleasePolicy:{semantics:'queued-for-next-movement',observationSource:expectedSource,
        observationPhase:expectedBoundary,maxMovementTicks:1}},
  }]);
}

function nativeV1RuntimeFixture(fixture,inputValue=132){
  const missing=0xffffffff,words=[1,3,1,inputValue,missing,missing,missing,missing,0x7fc00000,0x7fc00000,missing,missing];
  new Uint32Array(fixture.buffer).set(words,512/4);
  const core={memory:fixture.buffer,th09_input_observation_v1:()=>512};
  return{exports:core,Module:fixture.Module,app:9};
}

test('actual observation registries feed one strict queued boundary for th08 legacy and native-v1 adapters',async()=>{
  const th08Fixture=wasmImage({pointer:64,bytes:4096,exportVersion:2,
    header:[2,8,16,512,1,1,0,0],rows:[rawRow()]});
  const th08Request=runtimeObservationRequest({game:'th08',variant:'normal'});
  const th08Policy=th08Request.adapter.observation;
  assert.deepEqual(th08Request.adapter.observation,{kind:'legacy-th08-status-trace',
    sampleBoundary:'after-application-update',inputSource:'diagnostics[11] | player_state.input.buttons',
    inputSemantics:'queued-for-next-movement'});
  const th08Readers=serializedRuntimeAndTouchReaders(th08Fixture,th08Request,th08RuntimeFixture(th08Fixture,132));
  const th08Observation=th08Readers.readObservation();
  assert.deepEqual(th08Observation.input,{kind:'native-queued-for-next-movement-buttons',value:132,
    inputSemantics:'queued-for-next-movement',source:th08Policy.inputSource,sampleBoundary:th08Policy.sampleBoundary});
  const th08Boundary=await readTouchMotionBoundary({...th08Readers,policy:{semantics:th08Policy.inputSemantics,
    observationSource:th08Policy.inputSource,observationPhase:th08Policy.sampleBoundary}});
  assert.equal(th08Boundary.trace.available,true);assert.equal(th08Boundary.stableTraceBoundary,true);
  assert.deepEqual(th08Boundary.nextMovementInputBoundary,{available:true,semantics:'queued-for-next-movement',
    observationSource:th08Policy.inputSource,observationPhase:th08Policy.sampleBoundary,
    inputSemantics:'queued-for-next-movement',source:th08Policy.inputSource,
    sampleBoundary:th08Policy.sampleBoundary,value:132,traceTotal:1});

  const nativeFixture=wasmImage({pointer:64,bytes:4096,exportVersion:2,
    header:[2,8,16,512,1,1,0,0],rows:[rawRow()]});
  const nativeRegistry=nativeV1FixtureAdapter();
  const nativeRequest=runtimeObservationRequest({game:'th09',variant:'normal',adapterRegistry:nativeRegistry});
  const nativeDeclaration=nativeRegistry.resolve('th09','normal');
  const nativeReaders=serializedRuntimeAndTouchReaders(nativeFixture,nativeRequest,nativeV1RuntimeFixture(nativeFixture));
  const nativeObservation=nativeReaders.readObservation();
  assert.deepEqual(nativeObservation.input,{kind:'native-queued-for-next-movement-buttons',value:132,
    inputSemantics:'queued-for-next-movement',source:nativeDeclaration.observation.source,
    sampleBoundary:nativeDeclaration.observation.sampleBoundary});
  const nativePolicy=nativeDeclaration.movement.keyboardReleasePolicy;
  const nativeBoundary=await readTouchMotionBoundary({...nativeReaders,policy:nativePolicy});
  assert.equal(nativeBoundary.trace.available,true);assert.equal(nativeBoundary.stableTraceBoundary,true);
  assert.equal(nativeBoundary.nextMovementInputBoundary.available,true);
  assert.equal(nativeBoundary.nextMovementInputBoundary.value,132);
  assert.equal(nativeBoundary.nextMovementInputBoundary.source,nativeDeclaration.observation.source);
  assert.equal(nativeBoundary.nextMovementInputBoundary.sampleBoundary,nativeDeclaration.observation.sampleBoundary);
});

test('queued boundary rejects consumed semantics and source or sampling-phase mismatches from real adapters',async()=>{
  const expectedSource='ports/th09/web/cpp/sdl/Runtime.cpp | observe_input';
  const expectedBoundary='after-player-input-update';
  for(const [name,declaration,pattern] of [
    ['consumed input',nativeV1FixtureAdapter({inputSemantics:'consumed'}),/declared kind, semantics, source, or sample boundary/],
    ['wrong source',nativeV1FixtureAdapter({source:'ports/th09/web/cpp/platform/World.cpp | unrelated state'}),/declared kind, semantics, source, or sample boundary/],
    ['wrong sample phase',nativeV1FixtureAdapter({sampleBoundary:'before-player-input-update'}),/declared kind, semantics, source, or sample boundary/],
  ]){
    const fixture=wasmImage({pointer:64,bytes:4096,exportVersion:2,
      header:[2,8,16,512,1,1,0,0],rows:[rawRow()]});
    const request=runtimeObservationRequest({game:'th09',variant:'normal',adapterRegistry:declaration});
    const adapter=declaration.resolve('th09','normal');
    const readers=serializedRuntimeAndTouchReaders(fixture,request,nativeV1RuntimeFixture(fixture));
    const policy=adapter.movement.keyboardReleasePolicy;
    assert.equal(policy.observationSource,expectedSource);assert.equal(policy.observationPhase,expectedBoundary);
    const observation=readers.readObservation();
    const boundary=await readTouchMotionBoundary({...readers,policy});
    assert.equal(boundary.nextMovementInputBoundary.available,false,name);
    assert.match(boundary.nextMovementInputBoundary.reason,pattern,name);
    assert.equal(boundary.nextMovementInputBoundary.observedInput.kind,observation.input.kind,name);
    assert.equal(boundary.nextMovementInputBoundary.observedInput.inputSemantics,observation.input.inputSemantics,name);
    assert.equal(boundary.nextMovementInputBoundary.observedInput.source,observation.input.source,name);
    assert.equal(boundary.nextMovementInputBoundary.observedInput.sampleBoundary,observation.input.sampleBoundary,name);
  }

  const th08Request=runtimeObservationRequest({game:'th08',variant:'normal'});
  for(const [field,value] of [['inputSource','wrong TH08 source'],['sampleBoundary','before-application-update'],
    ['inputSemantics','consumed']]){
    const fixture=wasmImage({pointer:64,bytes:4096,exportVersion:2,
      header:[2,8,16,512,1,1,0,0],rows:[rawRow()]});
    const observation={...th08Request.adapter.observation,[field]:value};
    const request={...th08Request,adapter:{...th08Request.adapter,observation}};
    const readers=serializedRuntimeAndTouchReaders(fixture,request,th08RuntimeFixture(fixture,132));
    await assert.rejects(readTouchMotionBoundary({...readers,policy:{semantics:'queued-for-next-movement',
      observationSource:'diagnostics[11] | player_state.input.buttons',observationPhase:'after-application-update'}}),
    /invalid legacy TH08 status-trace declaration/);
  }
});

test('native boundary distinguishes unavailable capability from broken readers and malformed ABI',async()=>{
  let observationCalled=false;
  const unavailable=await readTouchMotionBoundary({readTrace:async()=>({available:false,reason:''}),
    readObservation:async()=>{observationCalled=true;},policy:{}});
  assert.equal(unavailable.trace.available,false);
  assert.ok(unavailable.trace.reason.length>0);
  assert.equal(observationCalled,false);

  await assert.rejects(readTouchMotionBoundary({readTrace:async()=>({available:true,raw:[]})}),/Touch trace header is missing/);
  await assert.rejects(readTouchMotionBoundary({readTrace:async()=>{throw Error('browser eval failed');}}),/browser eval failed/);
});

test('legacy traces cannot silently observe another seat or replay stream',()=>{
  const fixture=wasmImage({rows:[rawRow()]});
  assert.equal(fixture.read({game:'th08',localSeat:1}).available,false);
  assert.equal(fixture.read({game:'th08',executionKind:1}).available,false);
  assert.deepEqual(fixture.calls,[],'an unidentifiable stream is rejected before reading native state');
  for(const selection of [{localSeat:-1},{localSeat:4},{executionKind:4},{localSeat:.5}])
    assert.throws(()=>fixture.read({game:'th08',...selection}),/Invalid touch trace stream selection/);
});

test('serialized browser reader returns unavailable when the Runtime has no trace export', () => {
  const context={Module:{HEAPU8:new Uint8Array(new ArrayBuffer(128))}};
  const read=runInNewContext(`(${readTouchMotionTrace.toString()})`,context);
  const result=read({game:'th08'});
  assert.equal(result.available,false);
  assert.equal(result.reason,'The package has no completed-native-tick touch trace export');
});

test('serialized browser reader rejects an out-of-bounds pointer and malformed ABI headers', () => {
  assert.throws(()=>wasmImage({pointer:1024}).read({game:'th08'}),/Invalid touch trace pointer/);
  assert.throws(()=>wasmImage({header:[2,8,16,512,1,1,0,0]}).read({game:'th08'}),/Invalid touch trace ABI header/);

  // A ring buffer must expose min(total, capacity) rows; reserved header words
  // stay zero so schema changes cannot silently masquerade as version 1.
  assert.throws(()=>wasmImage({header:[1,8,16,512,4,3,0,0]}).read({game:'th08'}),/Invalid touch trace ABI header/);
  assert.throws(()=>wasmImage({header:[1,8,16,512,1,1,1,0]}).read({game:'th08'}),/Invalid touch trace ABI header/);
  assert.throws(()=>wasmImage({header:[1,8,16,512,2,2,0,0],bytes:128}).read({game:'th08'}),/Touch trace exceeds native memory/);
});

test('serialized browser reader and decoder preserve the WASM float fixed bits', () => {
  const row=rawRow({sequence:1,frame:0,beforeX:0,beforeY:0,targetX:.3,targetY:.4,
    afterX:.3,afterY:.4,speed:4,scaleX:1,scaleY:1,sensitivity:2});
  const fixture=wasmImage({header:[1,8,16,512,1,1,0,0],rows:[row]});
  const captured=fixture.read({game:'th08'});
  assert.equal(captured.available,true);
  assert.equal(captured.raw[16],f32bits(.3),'afterX float bits must survive the browser memory copy');
  assert.equal(captured.raw[17],f32bits(.4),'afterY float bits must survive the browser memory copy');
  const decoded=decodeTouchMotionTrace(Array.from(captured.raw));
  assert.equal(decoded.samples[0].targetX,Math.fround(.3));
  assert.equal(decoded.samples[0].targetY,Math.fround(.4));
  assert.equal(decoded.samples[0].afterX,Math.fround(.3));
  assert.equal(decoded.samples[0].afterY,Math.fround(.4));
  assert.equal(decoded.samples[0].sensitivity,2);
});
