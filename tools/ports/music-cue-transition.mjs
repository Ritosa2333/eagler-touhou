import assert from 'node:assert/strict';
import {isDeepStrictEqual as same} from 'node:util';

const ready=sample=>sample?.music?.pending===false&&sample.music.soundReady===true&&sample.music.playing===true&&sample.music.outputPaused===false;
function readState(sample,expected){
  for(const field of ['pending','soundReady','playing','outputPaused'])assert.equal(typeof sample?.music?.[field],'boolean',`Native music ${field} observation is missing`);
  assert.deepEqual(Object.keys(sample?.inputs??{}).sort(),[...expected.releaseInputs].sort(),'Native cue input observation differs from its declaration');
  for(const field of expected.releaseInputs)assert.equal(typeof sample.inputs[field],'boolean',`Native cue input ${field} observation is missing`);
}
function validateReleasePolicy(expected){
  assert.ok(Array.isArray(expected?.releaseInputs)&&expected.releaseInputs.length>0&&expected.releaseInputs.length<=16&&
    new Set(expected.releaseInputs).size===expected.releaseInputs.length&&
    expected.releaseInputs.every(name=>typeof name==='string'&&/^[a-zA-Z][a-zA-Z0-9]*$/.test(name)),
  'Declare the native inputs held by the cue trigger');
  assert.ok(Number.isSafeInteger(expected.maxCleanupPolls)&&expected.maxCleanupPolls>0&&expected.maxCleanupPolls<=600);
}
const neutral=(sample,expected)=>expected.releaseInputs.every(name=>sample?.inputs?.[name]===false);

/** Shared cue observation; a title supplies the trigger and its independently read asset cue. */
export function assessMusicCueTransition(evidence,expected){
  try{
    assert.equal(evidence?.schema,'eagler-touhou/music-cue-transition/2');
    validateReleasePolicy(expected);
    assert.ok(typeof expected?.beforeFile==='string'&&expected.beforeFile&&typeof expected.afterFile==='string'&&expected.afterFile&&expected.beforeFile!==expected.afterFile);
    assert.ok(Number.isSafeInteger(expected.maxPolls)&&expected.maxPolls>0&&expected.maxPolls<=600);
    assert.ok(typeof expected.cueSource?.asset==='string'&&expected.cueSource.asset&&Number.isSafeInteger(expected.cueSource.bytes)&&expected.cueSource.bytes>0);
    assert.ok(typeof expected.cueSource.hash?.algorithm==='string'&&expected.cueSource.hash.algorithm&&
      /^[a-f0-9]{16,128}$/.test(expected.cueSource.hash.value??''),'Cue source needs an independently read byte identity');
    const {source,before,request,samples,cleanup}=evidence;
    assert.equal(source?.stage,expected.stage);
    assert.deepEqual(source.identity,expected.cueSource);
    assert.ok(source.entries?.includes(expected.entry),'Cue entry is not referenced by the loaded source');
    assert.ok(before?.runtime&&Number.isSafeInteger(before.runtime.epoch)&&before.runtime.epoch>0);
    assert.deepEqual(source.runtime,before.runtime);
    assert.ok(Number.isSafeInteger(before.frame)&&before.frame>=0&&Number.isSafeInteger(before.request?.serial)&&before.request.serial>=0);
    assert.equal(before.file,expected.beforeFile);
    readState(before,expected);assert.equal(ready(before),true,'Initial native music source is not ready');
    assert.equal(request?.accepted,true);assert.equal(request.entry,expected.entry);assert.equal(request.statusCode,expected.acceptedStatus);
    assert.ok(Array.isArray(samples)&&samples.length>0&&samples.length<=expected.maxPolls);
    let prior=before;
    for(const [index,sample] of samples.entries()){
      assert.ok(same(sample.runtime,before.runtime),'Cue transition changed Runtime instance');
      assert.ok(Number.isSafeInteger(sample.frame)&&sample.frame>=prior.frame,'Native frame receipt moved backwards');
      readState(sample,expected);
      const opening=sample.file===null&&(sample.music.pending||!sample.music.soundReady&&!sample.music.playing);
      assert.ok([expected.beforeFile,expected.afterFile].includes(sample.file)||opening,`An unexpected native track appeared: ${sample.file}`);
      if(sample.file===expected.afterFile&&ready(sample))assert.equal(index,samples.length-1,'Cue capture continued after source establishment');
      assert.equal(sample.request.serial,before.request.serial+1);
      assert.equal(sample.request.entry,expected.entry);assert.equal(sample.request.statusCode,expected.acceptedStatus);
      prior=sample;
    }
    assert.equal(prior.file,expected.afterFile);assert.equal(ready(prior),true,'Target native music source is not ready');
    assert.ok(prior.frame>before.frame,'No native frame advanced after the cue');
    for(const field of expected.releaseInputs)assert.ok(samples.some(sample=>sample.inputs[field]===true),`Cue trigger input ${field} was never observed held`);
    assert.equal(cleanup?.releaseCompleted,true,'Cue release operation did not complete');
    assert.equal(evidence.cleanupError,undefined,'Cue cleanup contains an operation error');
    assert.equal(evidence.cleanupObservationError,undefined,'Cue cleanup contains an observation error');
    assert.ok(Array.isArray(cleanup.samples)&&cleanup.samples.length>0&&cleanup.samples.length<=expected.maxCleanupPolls,
      'Cue cleanup needs bounded native observations');
    const transitionFrame=prior.frame;
    for(const [index,sample] of cleanup.samples.entries()){
      readState(sample,expected);
      assert.deepEqual(sample.runtime,before.runtime,'Cue cleanup changed Runtime instance');
      assert.ok(Number.isSafeInteger(sample.frame)&&sample.frame>=prior.frame,'Cue cleanup frame receipt moved backwards');
      assert.deepEqual(sample.request,prior.request,'Cue cleanup changed the native request');
      assert.equal(sample.file,expected.afterFile);assert.equal(ready(sample),true,'Cue cleanup lost the established music source');
      if(neutral(sample,expected)&&sample.frame>transitionFrame)assert.equal(index,cleanup.samples.length-1,'Cleanup capture continued after native input release');
      prior=sample;
    }
    assert.ok(prior.frame>transitionFrame,'Cue cleanup has no subsequent native frame');
    assert.equal(neutral(prior,expected),true,'Cue trigger inputs remain held in native state');
    return{passed:true,status:'observed',complete:false};
  }catch(error){return{passed:false,status:'failed',complete:false,reason:error.message};}
}

export async function collectMusicCueTransition(io,expected){
  for(const name of ['readSource','observe','request','wait','release'])assert.equal(typeof io[name],'function',`Missing music cue operation ${name}`);
  assert.ok(Number.isSafeInteger(expected.maxPolls)&&expected.maxPolls>0&&expected.maxPolls<=600);
  validateReleasePolicy(expected);
  const evidence={schema:'eagler-touhou/music-cue-transition/2',source:await io.readSource(),before:await io.observe(),request:null,samples:[],
    cleanup:{releaseCompleted:false,samples:[]}};
  let failure;
  try{
    evidence.request=await io.request(expected.entry);
    assert.equal(evidence.request.accepted,true,'Native cue request was rejected');
    for(let poll=0;poll<expected.maxPolls;poll++){
      await io.wait();const sample=await io.observe();evidence.samples.push(sample);
      if(sample.file===expected.afterFile&&ready(sample))break;
    }
  }catch(error){failure=error;}
  finally{
    try{await io.release();evidence.cleanup.releaseCompleted=true;}
    catch(error){evidence.cleanupError=error.message;failure??=error;}
    try{
      const transitionFrame=(evidence.samples.at(-1)??evidence.before).frame;
      for(let poll=0;poll<expected.maxCleanupPolls;poll++){
        await io.wait();const sample=await io.observe();evidence.cleanup.samples.push(sample);
        readState(sample,expected);
        if(sample.frame>transitionFrame&&neutral(sample,expected))break;
      }
    }catch(error){evidence.cleanupObservationError=error.message;failure??=error;}
  }
  if(failure){failure.evidence=evidence;throw failure;}
  const result=assessMusicCueTransition(evidence,expected);
  if(!result.passed){const error=new Error(`Native music cue failed: ${result.reason}`);error.evidence=evidence;throw error;}
  return evidence;
}
