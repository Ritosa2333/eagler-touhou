import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import test from 'node:test';
import {evaluateDesktopGameplayReadiness, incompleteGameplayPrecondition,
  incompleteTh09TouchPrecondition, th09TouchFireEligibility,correlateTh09SfxWindow} from './runtime-desktop-readiness.mjs';

const gameplay = () => ({
  capabilities: {input:true,position:true,phase:true,pause:true},
  input:{value:0},position:{x:200,y:300},phase:{name:'match'},pause:{active:false},
  nativeObservation:{version:1},localSeat:0,frames:{simulation:40},
});

test('desktop gameplay readiness requires native input, position, live phase, local seat, and simulation progress',()=>{
  assert.deepEqual(evaluateDesktopGameplayReadiness(gameplay(),'th10'),{ready:true,reasons:[]});
  for(const [mutate,reason] of [
    [state=>{state.input.value=null;},'native-input-unavailable'],
    [state=>{state.position.x=NaN;},'native-player-position-unavailable'],
    [state=>{state.phase.name='game_over';},'native-gameplay-phase-unavailable'],
    [state=>{state.pause.active=true;},'gameplay-not-unpaused'],
    [state=>{state.localSeat=1;},'local-seat-unavailable'],
    [state=>{state.frames.simulation=null;},'native-simulation-frame-unavailable'],
  ]){
    const state=gameplay();mutate(state);
    assert.ok(evaluateDesktopGameplayReadiness(state,'th10').reasons.includes(reason),reason);
    const incomplete=incompleteGameplayPrecondition(state,'th10');
    assert.equal(incomplete.checkStatus,'incomplete');
    assert.equal(incomplete.precondition,state,'the original native observation must remain available');
  }
});

test('TH08 readiness uses its title-specific native supervisor state when no phase ABI exists',()=>{
  const state={capabilities:{input:true,position:true,phase:false},input:{value:0},position:{x:200,y:300},
    status:{supervisorActive:2,active:1,loading:false}};
  assert.deepEqual(evaluateDesktopGameplayReadiness(state,'th08'),{ready:true,reasons:[]});
  state.status.loading=true;
  assert.ok(evaluateDesktopGameplayReadiness(state,'th08').reasons.includes('native-gameplay-phase-unavailable'));
});

test('TH09 touch eligibility is grounded in native context, player readiness, and title state',()=>{
  const eligible={touchInput:{context:'gameplay',ready:true,inTitle:false,active:false}};
  assert.deepEqual(th09TouchFireEligibility(eligible),{ready:true,reasons:[]});
  assert.equal(incompleteTh09TouchPrecondition(eligible),null);
  for(const [touch,reason] of [
    [{context:'inactive',ready:true,inTitle:false},'touch-context-not-gameplay'],
    [{context:'dialogue-or-ending',ready:true,inTitle:false},'touch-context-not-gameplay'],
    [{context:'gameplay',ready:false,inTitle:false},'native-touch-player-not-ready'],
    [{context:'gameplay',ready:true,inTitle:true},'title-context-active'],
    [{context:null,ready:null,inTitle:null},'touch-context-not-gameplay'],
  ]){
    const state={touchInput:touch};
    const result=incompleteTh09TouchPrecondition(state);
    assert.equal(result.checkStatus,'incomplete');
    assert.equal(result.precondition,state);
    assert.ok(result.eligibility.reasons.includes(reason));
  }
});

test('TH09 SFX correlation requires a newly activated native voice and PCM on its own output tap',()=>{
  const voice=(id,overrides={})=>({id,playing:true,node:true,started:2,outputTapIndex:4,...overrides});
  const baseline={nativeSfx:{available:true,snapshots:{end:[voice('steady') ]}}};
  const action={nativeSfx:{available:true,activationEvents:[voice('steady',{started:2}),voice('fresh',{started:3})],
    associatedOutputPcm:[{tapIndex:4,nonZeroSamples:200},{tapIndex:5,nonZeroSamples:100}]}};
  const result=correlateTh09SfxWindow(baseline,action);
  assert.equal(result.newVoiceActivations.length,1);
  assert.equal(result.newVoiceActivations[0].id,'fresh');
  assert.equal(result.correlatedVoice.id,'fresh');
  assert.equal(result.correlatedPcm.tapIndex,4);
  assert.equal(result.correlated,true);
  assert.equal(correlateTh09SfxWindow(baseline,{nativeSfx:{...action.nativeSfx,
    associatedOutputPcm:[{tapIndex:4,nonZeroSamples:0},{tapIndex:5,nonZeroSamples:100}]}}).correlated,false,
  'non-zero PCM on a different voice output tap cannot correlate');
  assert.equal(correlateTh09SfxWindow(baseline,
    {nativeSfx:{available:true,activationEvents:[voice('steady',{started:2})],associatedOutputPcm:[{tapIndex:4,nonZeroSamples:10}]}}).correlated,false,
  'a voice already active at baseline and still using the same start time is not a new action-window activation');
});

test('TH09 desktop firing checks apply same-voice PCM correlation to keyboard and touch windows',async()=>{
  const source=await readFile(new URL('./runtime-desktop-probes.mjs',import.meta.url),'utf8');
  assert.match(source,/correlateTh09SfxWindow\(idle,firing\)/,
    'the KeyZ window must compare against its idle baseline');
  assert.match(source,/correlateTh09SfxWindow\(idle,touchFire\)/,
    'the touch-fire window must compare against its idle baseline');
  assert.equal((source.match(/if\(!sfxCorrelation\.correlated\)/g)??[]).length,2,
    'both TH09 checks must gate success on exact native voice-to-tap PCM correlation');
});
