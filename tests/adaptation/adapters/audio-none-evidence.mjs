import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {deriveSfxLatencyContract,resolveSfxLatencyContract,resolveSfxLatencyScenes} from './audio.mjs';
import {resolveTitleFlow} from './title-flow.mjs';
import {mapDesktopSfxStartJournal} from '../../../tools/ports/runtime-desktop-audio.mjs';
import {correlateInputSfxWindow} from '../../../tools/ports/runtime-desktop-readiness.mjs';
import {classifySfxLatencyEvidence,assertQueueTailMutationIsRejected} from '../../../tools/ports/sfx-latency-contract.mjs';

export function assertGameplaySfxEvidence(e,{game}={}){
  assert.equal(e.mode,'none');assert.equal(e.firing.actionWindow.game,game);
  const sfx=e.firing.nativeSfx;
  const actual=mapDesktopSfxStartJournal(e.firing.actionWindow.inputCapture,{activeObservations:sfx.sampledActiveObservations});
  assert.equal(actual.available,true,actual.reason);
  for(const name of ['activationEvents','activeObservations','sourceIntervals'])assert.deepEqual(sfx[name],actual[name],`SFX ${name} differs from the actual start journal`);
  const verdict=correlateInputSfxWindow(e.idle,e.firing);assert.equal(verdict.status,'passed',verdict.reason);
  return verdict;
}

export function assessAudioNoneEvidence({driver,entry,product}={}){
  if(entry?.id!=='audio-none-contract')return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'native-no-bgm-sfx',checks:['AUDIO-02']};
  try{
    assert.equal(driver.schema,'eagler-touhou/runtime-desktop-probes/1');
    const targets=driver.targets.filter(row=>row.game===product.game&&row.variant===product.variant);assert.equal(targets.length,1);
    const target=targets[0];assert.ok(target.checks.includes(entry)&&target.checks.filter(row=>row.id===entry.id).length===1);assert.equal(entry.status,'passed');
    const root=resolve(import.meta.dirname,'../../..'),sources={};
    for(const [name,path] of Object.entries(resolveSfxLatencyContract(product.game,product.variant).sourcePaths)){
      const bytes=readFileSync(resolve(root,path)),receipt=target.sfxProductionSources.filter(row=>row.path===path);assert.equal(receipt.length,1);
      assert.equal(receipt[0].bytes,bytes.length);assert.equal(receipt[0].sha256,createHash('sha256').update(bytes).digest('hex'));sources[name]=bytes.toString('utf8');
    }
    const contract=deriveSfxLatencyContract(product.game,product.variant,sources);assert.deepEqual(target.sfxContract,contract);
    const scenes=resolveSfxLatencyScenes(contract);
    for(const scene of [scenes.positive,scenes.negative]){
      const matches=target.checks.filter(row=>row.id===scene.checkId);assert.equal(matches.length,1);assert.equal(matches[0].status,'passed');
      const e=matches[0].observation;assert.equal(e.role,scene.role);assert.equal(e.scene,scene.name);assert.deepEqual(e.targetVoice,scene.targetVoice);
      for(const [name,value] of Object.entries(scene.before))assert.equal(e.before[name],value,`${scene.name}: initial native ${name}`);
      for(const [name,value] of Object.entries(scene.after))assert.equal(e.after[name],value,`${scene.name}: final native ${name}`);
      const snapshot=e.snapshot,events=snapshot.nativeTrace.events.filter(row=>row.logicalIndex===contract.logicalIndex&&row.handle===contract.handle);assert.equal(events.length,1);
      const pcm=snapshot.observations.filter(row=>row.eventSerial===events[0].serial&&row.sessionId===snapshot.sessionId);assert.equal(pcm.length,1);
      const input={contract,trace:snapshot.nativeTrace,session:snapshot.session,observation:pcm[0],sourceLead:e.sourceLead};
      if(scene.role==='positive')assert.equal(classifySfxLatencyEvidence(input).status,'passed');
      else assertQueueTailMutationIsRejected({...input,mutation:snapshot.mutation});
    }
    const fire=target.checks.filter(row=>row.id==='audio-output-with-music-disabled');assert.equal(fire.length,1);assert.equal(fire[0].status,'passed');
    const e=fire[0].observation;assertGameplaySfxEvidence(e,{game:product.game});
    assert.deepEqual(entry.observation.nativeInputWindow,e.nativeInputWindow);
    const flow=resolveTitleFlow(product);
    for(const state of [e.nativeInputWindow.before,e.nativeInputWindow.held,e.nativeInputWindow.after])assert.equal(flow.isGameplayReady(state,'desktop'),true);
    for(const output of [e.idle,e.firing]){
      const bytes=readFileSync(output.samplePath);assert.equal(bytes.length,output.bytes);assert.ok(bytes.length>0&&bytes.length%4===0);
      assert.equal(createHash('sha256').update(bytes).digest('hex'),output.sha256);assert.equal(output.contextState,'running');
    }
    assert.equal(e.idle.nonZeroSamples,0,'The no-input gameplay baseline contains output');
    const idle=readFileSync(e.idle.samplePath);for(let i=0;i<idle.length;i+=4)assert.equal(idle.readFloatLE(i),0,'BGM/idle PCM is not silent');
    assert.ok(e.firing.nonZeroSamples>0&&e.firing.tapCount>0);
    result.valid=true;
  }catch(error){result.reason=error.message;}
  return result;
}
