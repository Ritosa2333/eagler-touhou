import test from 'node:test';
import assert from 'node:assert/strict';
import {createAdapterRegistry,resolveAdaptationAdapter,resolvePracticeAdapter} from './registry.mjs';
import {runInNewContext} from 'node:vm';
import {controlValidationScene} from './scene.mjs';
import {controlTouchMotionClock} from './touch-clock.mjs';

const base={schema:'eagler-touhou/adaptation-adapter/1',game:'next',variant:'normal',
  runtime:{global:'__nextRuntime',core:'exports',module:'Module',app:'instance'},
  observation:{kind:'native-v1',export:'observe_native_v1',source:'ports/next/runtime.cpp | read_native_state',
    sampleBoundary:'after-native-update',inputSemantics:'consumed'},
  flow:{strategy:'confirm',needsControlWarmup:false},
  movement:{inputBoundary:'synchronous-consumption',inputBoundarySource:'ports/next/player.cpp | update',source:'ports/next/player.cpp | update',
    precision:{unitsPerPixel:128,rounding:'toward-zero',scalePlacement:'after-limit'}},
  controlledScene:{schema:'eagler-touhou/controlled-input-scene/3',armExport:'arm_input_world',stateExport:'read_input_world',
    armStateExport:'read_input_world_arm_state',writeStateExport:'read_input_world_write_state_v1',source:'ports/next/validation.cpp',
    writePolicy:{initialization:'none',source:'ports/next/application.cpp | world load has no initialization save'}},
};
const products={next:{multiplayerRuntime:true}};

test('a new title registers its own names and arithmetic without modifying a shared driver',()=>{
  const r=createAdapterRegistry([base,{...base,variant:'multiplayer',controlledScene:undefined}],{products});
  assert.equal(r.resolve('next','normal').runtime.core,'exports');
  assert.equal(r.resolve('next','normal').controlledScene.armExport,'arm_input_world');
  assert.equal(r.resolve('next','multiplayer').controlledScene,undefined,'normal capabilities never fill a missing MP interface');
  assert.equal(r.resolve('unregistered','normal'),null);
  assert.throws(()=>r.resolve('next'),/explicitly/);
  assert.throws(()=>{r.resolve('next','normal').movement.precision.unitsPerPixel=1;},TypeError);
  assert.equal(base.movement.precision.unitsPerPixel,128,'registration does not mutate declarations');
});

test('language diagnostics retain a host-only title classifier and data-only Runtime source declaration',()=>{
  const language={schema:'eagler-touhou/language-scenes/1',enableExport:'language_enable',sizeExport:'language_size',
    copyExport:'language_copy',allocateExport:'allocate',freeExport:'deallocate',table:'/thcrap/next/strings.etl',
    missingKey:'missing-key',source:'ports/next/localization.cpp | observe lookup',
    samples:[{operation:'string-by-id',key:'translated-key',source:'ports/next/title.cpp | translated'},
      {operation:'string-by-id',key:'missing-key',source:'ports/next/title.cpp | missing'}],
    browserDiagnostics:{runtimeAsset:'scripts/title-entry-arbitrary.js',source:'ports/next/runtime.cpp | browser diagnostic producer'}};
  const classifyLanguageDiagnostic=({text})=>text==='next runtime ready'
    ?{classification:'next-runtime-ready',disposition:'expected'}:null;
  const adapter=createAdapterRegistry([{...base,language,classifyLanguageDiagnostic}],{products}).resolve('next','normal');
  assert.equal(adapter.classifyLanguageDiagnostic,classifyLanguageDiagnostic);
  assert.deepEqual(adapter.language.browserDiagnostics,language.browserDiagnostics);
  assert.equal(classifyLanguageDiagnostic({text:'next runtime ready'}).disposition,'expected');
  assert.throws(()=>createAdapterRegistry([{...base,language}],{products}),/host classifier/);
  assert.throws(()=>createAdapterRegistry([{...base,classifyLanguageDiagnostic}],{products}),/require a language capability/);
  for(const browserDiagnostics of [undefined,{runtimeAsset:'../other.js',source:'ports/next/runtime.cpp'},
    {runtimeAsset:'/absolute.js',source:'ports/next/runtime.cpp'},{runtimeAsset:'title.js',source:''}]){
    assert.throws(()=>createAdapterRegistry([{...base,language:{...language,browserDiagnostics},classifyLanguageDiagnostic}],{products}),
      /Runtime source declaration|Runtime-relative path|source anchor/);
  }
});

test('native STG actions declare the actual consumer boundary and independent input bits',()=>{
  const actions={schema:'eagler-touhou/stg-actions/1',stateExport:'read_actions',wordCount:20,stateApp:false,
    sampleBoundary:'after-player-consumer',source:['ports/next/player.cpp | completed consumer'],
    escapePulseTicks:1,escapePulseSource:'ports/next/touch.cpp | one-shot Escape lifetime',
    inputMasks:{fire:16,focus:32,bomb:64,escape:128}};
  const r=createAdapterRegistry([{...base,actions}],{products});
  assert.equal(r.resolve('next','normal').actions.stateExport,'read_actions');
  for(const changed of [{sampleBoundary:'queued-for-next-movement'},{wordCount:19},{source:[]},{stateApp:undefined},{escapePulseTicks:undefined},{escapePulseTicks:0},
    {inputMasks:{...actions.inputMasks,bomb:16}},{inputMasks:{...actions.inputMasks,fire:3}}])
    assert.throws(()=>createAdapterRegistry([{...base,actions:{...actions,...changed}}],{products}));
});

test('invalid or ambiguous title declarations fail before execution',()=>{
  const reject=(value,pattern)=>assert.throws(()=>createAdapterRegistry([value],{products}),pattern);
  reject({...base,variant:'mp'},/variant/);
  reject({...base,game:'missing'},/Register product/);
  reject({...base,runtime:{...base.runtime,core:'window.core'}},/identifier/);
  reject({...base,movement:{...base.movement,precision:{...base.movement.precision,unitsPerPixel:0}}},/units/);
  reject({...base,movement:{...base.movement,precision:{...base.movement.precision,unitsPerPixel:1.5}}},/positive integer/);
  reject({...base,movement:{...base.movement,precision:{...base.movement.precision,limitedQuantization:'unknown'}}},/quantizer/);
  reject({...base,movement:{...base.movement,inputBoundary:'queued-for-next-movement'}},/source-anchored keyboard release policy/);
  reject({...base,movement:{...base.movement,inputBoundarySource:''}},/input boundary needs its production source anchor/);
  reject({...base,observation:undefined},/Declare how native observations/);
  reject({...base,observation:{...base.observation,source:''}},/production source anchor/);
  reject({...base,observation:{...base.observation,inputSemantics:undefined}},/input consumption semantics/);
  reject({...base,observation:{...base.observation,kind:'guess-v1'}},/Observation kind/);
  reject({...base,observation:{kind:'unsupported'}},/Unsupported observation must state/);
  reject({...base,controlledScene:{...base.controlledScene,schema:'invented'}},/scene contract/);
  reject({...base,controlledScene:{...base.controlledScene,armExport:undefined}},/controlledScene.armExport/);
  reject({...base,controlledScene:{...base.controlledScene,armStateExport:undefined}},/controlledScene.armStateExport/);
  reject({...base,controlledScene:{...base.controlledScene,writeStateExport:undefined}},/controlledScene.writeStateExport/);
  reject({...base,controlledScene:{...base.controlledScene,writePolicy:undefined}},/declare its initialization write policy/);
  reject({...base,controlledScene:{...base.controlledScene,writePolicy:{initialization:'guess',source:'source.cpp'}}},/declare its initialization write policy/);
  reject({...base,controlledScene:{...base.controlledScene,writePolicy:{initialization:'none',source:''}}},/write policy needs a production source anchor/);
  reject({...base,flow:{strategy:'made-up',needsControlWarmup:false}},/legacy strategy is unsupported/);
  reject({...base,skipFailedChecks:true},/Unknown adapter capability/);
  assert.throws(()=>createAdapterRegistry([base,base],{products}),/Duplicate/);
  assert.throws(()=>createAdapterRegistry([{...base,variant:'multiplayer'}],{products:{next:{}}}),/no multiplayer/);
});

test('Replay registration keeps independent title parsers and per-variant semantics explicit',()=>{
  const pilot=resolveAdaptationAdapter('th08','normal'),replay={...pilot.replay,game:'next'};
  const next={...base,replay,parseReplayFrames:pilot.parseReplayFrames,parseReplayExtension:pilot.parseReplayExtension};
  const registry=createAdapterRegistry([next],{products});
  assert.equal(registry.resolve('next','normal').parseReplayFrames,pilot.parseReplayFrames);
  assert.equal(registry.resolve('next','multiplayer'),null);
  assert.throws(()=>createAdapterRegistry([{...next,parseReplayFrames:undefined}],{products}),/independent title-owned file parser/);
  assert.throws(()=>createAdapterRegistry([{...next,replay:{...replay,variant:'multiplayer'}}],{products}),/another variant/);
  assert.throws(()=>createAdapterRegistry([{...next,replay:undefined}],{products}),/require explicit title semantics/);
});

test('practice capability accepts a title-owned ABI and preserves callbacks for a new title',()=>{
  const decode=words=>({state:{active:true},configured:{speed:7},run:{speed:7},applied:{velocity:14},words});
  const practice={schema:'eagler-touhou/practice-title/1',source:'ports/next/practice.cpp | apply_options',
    parameterCase:{speed:7},selectionCase:{difficulty:3,character:5,source:'ports/next/title.cpp | selected start fields'},
    snapshot:{export:'read_practice_v2',wordCount:3,wordType:'u32',app:true,decode},
    initialSnapshot:{export:'read_initial_practice_v2',wordCount:2,wordType:'u32',app:true,decode},
    enterMenu:async()=>({}),exerciseControls:async()=>({}),assessConsumption:()=>({status:'observed'}),
    featureInventory:()=>({status:'observed',items:[]}),
    replayRoundTrip:{supported:false,reason:'This title has no native practice Replay importer'}};
  const registry=createAdapterRegistry([{...base,practice}],{products});
  const adapter=registry.resolve('next','normal');
  assert.equal(adapter.practice.snapshot.wordCount,3);
  assert.equal(adapter.practice.snapshot.wordType,'u32');
  assert.equal(adapter.practice.snapshot.app,true);
  assert.deepEqual(adapter.practice.selectionCase,{difficulty:3,character:5,source:'ports/next/title.cpp | selected start fields'});
  assert.equal(adapter.practice.snapshot.decode,decode);
  assert.equal(resolvePracticeAdapter('next','normal'),null,'Unregistered declarations do not leak across registries');
  assert.throws(()=>createAdapterRegistry([{...base,practice:{...practice,replayRoundTrip:undefined}}],{products}),
    /must be declared as supported or explicitly blocked/);
  assert.throws(()=>createAdapterRegistry([{...base,practice:{...practice,snapshot:{...practice.snapshot,wordType:'unknown'}}}],{products}),/ABI word type/);
  assert.throws(()=>createAdapterRegistry([{...base,practice:{...practice,snapshot:{...practice.snapshot,app:undefined}}}],{products}),/declare whether its export takes/);
  const supportedReplay={...resolveAdaptationAdapter('th08','normal').practice.replayRoundTrip,assess:undefined};
  assert.throws(()=>createAdapterRegistry([{...base,practice:{...practice,replayRoundTrip:supportedReplay}}],{products}),/assess title codec/);
});

test('th08 pilot declares source meanings without granting another product its interfaces',()=>{
  const a=resolveAdaptationAdapter('th08','normal');
  assert.equal(a.movement.keyboardReleasePolicy.maxMovementTicks,1);
  assert.equal(a.audioLatency.sample,'se_ok00.wav');
  assert.equal(a.controlledScene.schema,'eagler-touhou/controlled-input-scene/3');
  assert.equal(a.controlledScene.armExport,'validation_scene_arm');
  assert.equal(a.controlledScene.writeStateExport,'validation_scene_write_state_v1');
  assert.equal(resolveAdaptationAdapter('th08','multiplayer'),null);
  assert.equal(resolveAdaptationAdapter('th10','normal').observation.export,'th10_native_observation_v1');
  assert.equal(resolveAdaptationAdapter('th10','normal').replay,undefined);
  assert.equal(resolveAdaptationAdapter('th10','multiplayer'),null);
  assert.equal(a.observation.kind,'legacy-th08-status-trace');
  assert.equal(a.observation.inputSource,'diagnostics[11] | player_state.input.buttons');
  assert.equal(a.movement.inputBoundary,'queued-for-next-movement');
  assert.match(a.movement.inputBoundarySource,/GameplayScene\.cpp.*PlayerMotion\.cpp/);
});

test('audio scene registration requires one positive and one same-voice queue-tail negative control',()=>{
  const declaration={...base,audioLatency:{game:'next',variant:'normal',sourcePaths:{sound:'ports/next/sound.cpp'},
    scenes:[{name:'positive-readiness',role:'positive',sameTargetVoice:true,input:'KeyZ',before:{page:0,inputReady:1},after:{page:1}},
      {name:'negative-queue-tail',role:'negative-control',sameTargetVoice:true,mutation:'measured-queue-tail',input:'KeyZ',before:{page:1,inputReady:1},after:{page:2}}],
    state:{export:'status',inputReadySource:'ports/next/menu.cpp | accepts_confirm',
      fields:{activeApp:0,appStatus:1,invalid:2,loading:3,page:4,cursor:5,phase:6,inputReady:7}}},
    deriveSfxContract:()=>({})};
  assert.equal(createAdapterRegistry([declaration],{products}).resolve('next','normal').audioLatency.scenes.length,2);
  assert.throws(()=>createAdapterRegistry([{...declaration,audioLatency:{...declaration.audioLatency,
    state:{...declaration.audioLatency.state,inputReadySource:''}}}],{products}),/production input gate source/);
  assert.throws(()=>createAdapterRegistry([{...declaration,audioLatency:{...declaration.audioLatency,
    scenes:declaration.audioLatency.scenes.map(scene=>({...scene,before:{page:0}}))}}],{products}),/native input gate/);
  for(const scenes of [
    [{name:'p',role:'positive',sameTargetVoice:true,input:'KeyZ',before:{},after:{}},
      {name:'p2',role:'positive',sameTargetVoice:true,input:'KeyZ',before:{},after:{}}],
    [{name:'p',role:'positive',input:'KeyZ',before:{},after:{}},
      {name:'n',role:'negative-control',sameTargetVoice:true,mutation:'measured-queue-tail',input:'KeyZ',before:{},after:{}}],
    [{name:'p',role:'positive',sameTargetVoice:true,mutation:'measured-queue-tail',input:'KeyZ',before:{},after:{}},
      {name:'n',role:'negative-control',sameTargetVoice:true,mutation:'measured-queue-tail',input:'KeyZ',before:{},after:{}}],
    [{name:'p',role:'positive',sameTargetVoice:true,input:'KeyZ',before:{},after:{}},
      {name:'n',role:'negative-control',sameTargetVoice:false,mutation:'measured-queue-tail',input:'KeyZ',before:{},after:{}}],
    [{name:'p',role:'positive',sameTargetVoice:true,input:'KeyZ',before:{},after:{}},
      {name:'n',role:'negative-control',sameTargetVoice:true,input:'KeyZ',before:{},after:{}}],
  ]) assert.throws(()=>createAdapterRegistry([{...declaration,audioLatency:{...declaration.audioLatency,scenes}}],{products}),/positive role|negative-control role|same production target voice|cannot apply|measured queue-tail/);
});

test('shared browser readers use a new title mounting and export names through serialized declarations',()=>{
  const adapter=createAdapterRegistry([base],{products}).resolve('next','normal');
  const calls=[],memory={buffer:new ArrayBuffer(160)};
  new Uint32Array(memory.buffer,16,16).set([1,1,1,1,0,0,0,0,0,2,70,64,29,1,1,0]);
  new Uint32Array(memory.buffer,88,8).set([1,0,0,0,0,0,0,0]);
  new Uint32Array(memory.buffer,120,8).set([1,0,0,0,0,0,0,0]);
  const core={memory,read_input_world(app){assert.equal(app,0);return 16;},
    arm_input_world(app){calls.push(['arm',app]);new Uint32Array(memory.buffer,88,8).set([1,1,1,1,0,0,0,0]);return 1;},
    read_input_world_arm_state(){return 88;},
    read_input_world_write_state_v1(){return 120;},
    halt(){calls.push('halt');},advance(...args){calls.push(args);return 0;},resume(){calls.push('resume');}};
  const sandbox={__nextRuntime:{instance:0,exports:core}};
  const read=runInNewContext(`(${controlValidationScene.toString()})`,sandbox);
  const receipt=read({game:'next',variant:'normal',action:'read',contract:{...adapter.controlledScene,runtime:adapter.runtime}});
  assert.equal(receipt.stage,2);assert.equal(receipt.enemyFrame,70);assert.equal(calls.length,0);
  assert.equal(read({game:'next',variant:'normal',action:'arm',contract:{...adapter.controlledScene,runtime:adapter.runtime}}).armAccepted,true);
  const clock=runInNewContext(`(${controlTouchMotionClock.toString()})`,sandbox);
  const request={game:'next',variant:'normal',contract:{runtime:adapter.runtime,tickApp:true,startApp:false,
    stopExport:'halt',tickExport:'advance',startExport:'resume',source:'ports/next/loop.cpp'}};
  assert.equal(clock({...request,action:'begin'}).mode,'native-manual');
  assert.equal(clock({...request,action:'tick'}).ticks,1);
  clock({...request,action:'end'});
  assert.deepEqual(calls,[['arm',0],'halt',[0,1/60,0],'resume']);
});
