import test from 'node:test';
import assert from 'node:assert/strict';
import vm from 'node:vm';
import {deriveSfxLatencyContract,readSfxLatencySceneState,resolveSfxLatencyContract,resolveSfxLatencyScenes,sfxLatencySceneRequest} from './audio.mjs';

test('audio title readiness reads native status without advancing or preparing the game',()=>{
  const calls=[],fields=Array(29).fill(0);Object.assign(fields,{0:1,1:1,11:1,25:1,26:10});
  const sandbox={__th08Runtime:{app:42,core:{status(app,index){calls.push({app,index});return fields[index];}},
    Module:{SDL3:{audioContext:{state:'running'}}}},__eaglerKeyboardInputBridge:{active:()=>true}};
  sandbox.request=sfxLatencySceneRequest({game:'th08',variant:'normal'});
  const result=vm.runInNewContext(`(${readSfxLatencySceneState.toString()})(request)`,sandbox);
  assert.deepEqual(JSON.parse(JSON.stringify(result)),{available:true,activeApp:'Title',appStatus:1,invalid:0,loading:0,
    page:0,cursor:0,phase:1,inputReady:1,phaseTimer:10,inputCurrent:0,inputPrevious:0,bridgeActive:true,audioState:'running'});
  assert.ok(calls.every(row=>row.app===42));
  assert.ok(calls.some(row=>row.index===11),'title phase comes from the new native readiness field');
  fields[25]=0;
  assert.equal(vm.runInNewContext(`(${readSfxLatencySceneState.toString()})(request)`,sandbox).inputReady,0,
    'a visibly Ready menu can still reject confirmation');
  fields[25]=2;
  assert.throws(()=>vm.runInNewContext(`(${readSfxLatencySceneState.toString()})(request)`,sandbox),/must be 0 or 1/);
});

test('audio navigation cannot inherit a normal adapter for MP or infer absent native readiness',()=>{
  const sandbox={__th08Runtime:{app:42,core:{status(){throw Error('must not read an unsupported variant');}}}};
  sandbox.request=sfxLatencySceneRequest({game:'th08',variant:'multiplayer'});
  const mp=vm.runInNewContext(`(${readSfxLatencySceneState.toString()})(request)`,sandbox);
  assert.equal(mp.available,false);
  const missing=vm.runInNewContext(`(${readSfxLatencySceneState.toString()})(request)`,{request:sfxLatencySceneRequest({game:'th08',variant:'normal'})});
  assert.equal(missing.available,false);
  assert.throws(()=>deriveSfxLatencyContract('th08','multiplayer',{}),/No SFX latency adapter/);
});

test('audio scene execution resolves explicit roles and one production target identity, not array positions',()=>{
  const contract=resolveSfxLatencyContract('th08','normal');
  const scenes=resolveSfxLatencyScenes({...contract,scenes:[...contract.scenes].reverse()});
  assert.equal(scenes.positive.name,'title-menu-to-difficulty');
  assert.equal(scenes.negative.name,'difficulty-to-character-live-queue-tail');
  assert.equal(scenes.positive.targetVoice.handle,contract.handle);
  assert.deepEqual(scenes.positive.targetVoice,scenes.negative.targetVoice);
  assert.equal(scenes.negative.checkId,'negative-control-sfx-delayed-to-mixed-queue-tail');
  assert.throws(()=>resolveSfxLatencyScenes({...contract,scenes:contract.scenes.map(scene=>({...scene,role:'positive'}))}),/scene roles/);
  assert.throws(()=>resolveSfxLatencyScenes({...contract,scenes:contract.scenes.map(scene=>({...scene,sameTargetVoice:false}))}),/same production target voice/);
  assert.throws(()=>resolveSfxLatencyScenes({...contract,scenes:contract.scenes.map(scene=>scene.role==='negative-control'?{...scene,mutation:'delay-by-one-frame'}:scene)}),/measured-queue-tail/);
});
