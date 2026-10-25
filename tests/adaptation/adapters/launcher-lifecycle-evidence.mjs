import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {resolveAdaptationAdapter} from './registry.mjs';
import {resolveTitleFlow} from './title-flow.mjs';
import {assertLauncherInputLifecycle} from '../../../tools/ports/launcher-input-lifecycle.mjs';
import {assertPracticeControlScene,practiceControlSamples} from '../../../tools/ports/practice-ui-scene.mjs';
import {assertViewOptionsScene,assertViewOptionsRestart} from '../../../tools/ports/view-options-scene.mjs';

function assertControlReport(driver,entry,product,{machineOnly=false}={}){
  assert.equal(product.variant,'normal');assert.equal(driver.scope.game,product.game);assert.equal(driver.scope.variant,product.variant);
  assert.equal(driver.checks.filter(row=>row.id===entry.id).length,1);assert.ok(driver.checks.includes(entry));
  assert.equal(entry.status,'passed');if(machineOnly){assert.equal(entry.machineComplete,true);assert.equal(entry.complete,false);}else assert.equal(entry.complete,true);
  assert.equal(driver.finalErrorAudit.status,'passed');assert.deepEqual(driver.teardownErrors,[]);
  assert.deepEqual(driver.inputLifecycleSettings,{touchEnabled:true,touchDisclosureOpen:true});
  assert.deepEqual(driver.harnessSourcesPostflight,driver.harnessSources);assert.ok(Object.keys(driver.harnessSources).length>10);
  const original=driver.runtimePackages.normal,after=driver.postflight.normalRuntimePackage;
  for(const key of ['packageSha256','wasmSha256']){assert.match(original[key],/^[a-f0-9]{64}$/);assert.equal(after[key],original[key]);}
  assert.deepEqual(driver.postflight.privateData,driver.runtimePackages.data);
  const installed=driver.uiImport.observation.generation,online=driver.onlineRuntime.observation,url=new URL(online.frameUrl);
  for(const owner of [installed,online.dataRead])for(const field of ['bytes','sha256'])assert.equal(owner[field],driver.runtimePackages.data[field]);
  assert.equal(online.dataRead.game,product.game);assert.equal(online.dataRead.epoch,online.first.epoch);
  assert.equal(online.dataRead.generation,installed.generationId);assert.equal(url.searchParams.get('gameGeneration'),installed.generationId);
  assert.equal(url.searchParams.get('runtimeVariant'),product.variant);assert.equal(Number(url.searchParams.get('runtimeEpoch')),online.first.epoch);
}

export function assessViewOptionsEvidence({driver,entry,product}={}){
  if(driver?.schema!=='eagler-touhou/offline-runtime-browser/1'||entry?.id!=='RUNTIME-06'||!driver.viewOptions)return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'native-view-options-machine',checks:['RUNTIME-06'],humanRequired:true};
  try{
    assertControlReport(driver,entry,product,{machineOnly:true});
    const envelope=driver.viewOptions;assert.equal(envelope.game,product.game);assert.equal(envelope.variant,product.variant);assert.deepEqual(envelope.adaptationIds,['RUNTIME-06']);
    const e=envelope.observation;assertViewOptionsScene(e,resolveAdaptationAdapter(product.game,product.variant).viewOptions);assertViewOptionsRestart(e);
    assert.equal(e.before.identity.url,driver.onlineRuntime.observation.frameUrl);assert.ok(e.before.identity.app>0);
    const flow=resolveTitleFlow(product);for(const row of [e.restart.before,e.restart.reset,e.restart.advanced])assert.equal(flow.isGameplayReady(row,'desktop'),true);
    for(const row of e.screenshots){
      const path=resolve(driver.outputDirectory,`view-${row.name}.png`);assert.equal(resolve(row.path),path);
      const bytes=readFileSync(path);assert.equal(bytes.length,row.bytes);assert.equal(createHash('sha256').update(bytes).digest('hex'),row.sha256);
      assert.equal(bytes.subarray(0,8).toString('hex'),'89504e470d0a1a0a');assert.equal(bytes.subarray(12,16).toString(),'IHDR');
      assert.ok(bytes.readUInt32BE(16)>0&&bytes.readUInt32BE(20)>0);assert.deepEqual(row.identity,row.name==='enabled'?e.enabled.identity:e.before.identity);
    }
    for(const [index,row] of e.optionChanges.entries()){const runtime=row.runtime,identity=[e.before,e.enabled,e.final][index].identity;
      assert.equal(runtime.frameUrl,identity.url);
      assert.equal(runtime.query.runtimeEpoch,String(row.epoch));assert.equal(runtime.query.gameGeneration,driver.uiImport.observation.generation.generationId);
      for(const field of ['bytes','sha256'])assert.equal(runtime.dataRead[field],driver.runtimePackages.data[field]);
      assert.equal(runtime.first.epoch,row.epoch);assert.equal(runtime.ready.epoch,row.epoch);
    }
    result.valid=true;
  }catch(error){result.reason=error.message;}
  return result;
}

export function assessPracticeControlEvidence({driver,entry,product}={}){
  if(driver?.schema!=='eagler-touhou/offline-runtime-browser/1'||entry?.id!=='PRACTICE-06')return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'native-practice-widget-controls',checks:['PRACTICE-06']};
  try{
    assertControlReport(driver,entry,product);assert.equal(driver.browser.emulatedTouch,true);
    assert.equal(driver.hostPracticeFeatures.request[product.game],true);assert.equal(driver.hostPracticeFeatures.published.thprac,true);
    assert.deepEqual(driver.practiceControlsSetup,{nativeEnabled:true,touchControlsEnabled:true,editorClosed:true});
    const envelope=driver.practiceControls;assert.equal(envelope.game,product.game);assert.equal(envelope.variant,product.variant);assert.deepEqual(envelope.adaptationIds,['PRACTICE-06']);
    const adapter=resolveAdaptationAdapter(product.game,product.variant),e=envelope.observation;
    for(const row of practiceControlSamples(e)){
      assert.deepEqual(row.ui,adapter.practice.uiSnapshot.decode(row.uiRaw));assert.deepEqual(row.practice,adapter.practice.snapshot.decode(row.nativeRaw));
      assert.equal(row.practice.state.enabled,true);assert.equal(row.practice.state.active,true);assert.equal(row.practice.state.inGame,true);assert.equal(row.practice.state.replay,false);
      assert.ok(Number(new URL(row.identity.url).searchParams.get('runtimeEpoch'))>0);
      assert.equal(row.identity.url,driver.onlineRuntime.observation.frameUrl);
    }
    assertPracticeControlScene(e,adapter.practice.controlKeys);result.valid=true;
  }catch(error){result.reason=error.message;}
  return result;
}

export function assessLauncherLifecycleEvidence({driver,entry,product}={}){
  if(driver?.schema!=='eagler-touhou/offline-runtime-browser/1'||entry?.id!=='RUNTIME-04')return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'launcher-input-lifecycle',checks:['RUNTIME-04']};
  try{
    assertControlReport(driver,entry,product);assert.equal(driver.browser.headless,false);
    assert.equal(driver.browser.lifecycle.scope,'same-window-tab');assert.equal(driver.browser.lifecycle.osOcclusion,'disabled-for-tab-isolation');
    assert.deepEqual(driver.lifecycleCleanup.errors,[]);
    const envelope=driver.inputLifecycle;assert.equal(envelope.game,product.game);assert.equal(envelope.variant,product.variant);
    assert.deepEqual(envelope.adaptationIds,['RUNTIME-04']);
    const e=envelope.observation,adapter=resolveAdaptationAdapter(product.game,product.variant),flow=resolveTitleFlow(product);
    assert.equal(e.background.audio.role,adapter.inputLifecycle.audioRole);
    for(const row of [e.initial,e.beforeBackground,e.afterBackground,e.beforeExit,e.reopened,e.singleInput.held,e.singleInput.released])
      assert.equal(flow.isGameplayReady(row.observation,'desktop'),true);
    assertLauncherInputLifecycle(e,{directionMask:adapter.movement.directionMasks.ArrowRight,focusMask:adapter.actions.inputMasks.focus});
    result.valid=true;
  }catch(error){result.reason=error.message;}
  return result;
}
