import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {
  armTh08PracticeIdbfsObserverRaw,assessTh08PracticeReplayRoundTrip,finalizeTh08PracticeReplayRaw,
  parseTh08PracticeReplayMetadata,restoreTh08PracticeIdbfsObserverRaw,TH08_PRACTICE_FRESH_CONFIG,
  waitTh08PracticeIdbfsObserverRaw,th08PracticeRuntimeStage,th08PracticeReplayPlaybackReady,
} from './titles/th08-practice.mjs';
import {practiceAdaptationIds,practiceAdaptationChecks} from '../../../tools/ports/practice-observations.mjs';
import {TH08_PRACTICE_SELECTION_CASE} from './titles/th08-practice.mjs';

const payload=Object.freeze({version:'2.6.1.0',game:'th08',mode:1,stage:0,life:4,bomb:3,power:96,gauge:10000,
  score:1234560,graze:23,point_total:234,point_stage:345,time:321,value:43210,night:4,familiar:17,rank:10,rankLock:true});

test('Practice Replay readiness waits beyond the immutable world frame even after input is consumed',()=>{
  const live={state:{inGame:true,active:true,replay:true},applied:{enemyFrame:0}},stream={loaded:true,frame:1,inputCursor:2};
  const initial={applied:{enemyFrame:0}};
  assert.equal(th08PracticeReplayPlaybackReady(live,stream,initial),false);
  live.applied.enemyFrame=1;assert.equal(th08PracticeReplayPlaybackReady(live,stream,initial),true);
  initial.applied.enemyFrame=100;live.applied.enemyFrame=100;
  assert.equal(th08PracticeReplayPlaybackReady(live,stream,initial),false,'warped Practice uses its actual initialization frame');
  live.applied.enemyFrame=101;assert.equal(th08PracticeReplayPlaybackReady(live,stream,initial),true);
  assert.equal(th08PracticeReplayPlaybackReady(live,{...stream,ended:true},initial),false);
  assert.equal(th08PracticeReplayPlaybackReady(live,{...stream,inputCursor:0},initial),false);
});
function automaticPersistence(){
  const afterFinalizer={startSequence:0,at:10},call={startSequence:1,startedAt:11,populate:false,completed:true,completedAt:12,succeeded:true};
  return{finalizer:{available:true,finalized:true,afterFinalizer},observer:{available:true,...call,afterFinalizer},restored:{available:true,restored:true,calls:[call]}};
}

function rawReplay(value=payload){
  const json=Buffer.from(JSON.stringify(value)),length=(12+json.length+3)&~3,bytes=Buffer.alloc(0x68+length);
  bytes.writeUInt32LE(0x50523854,0);bytes.writeUInt16LE(6,4);bytes.writeUInt32LE(0x68,12);
  bytes.write('USER',0x68,'ascii');bytes.writeUInt32LE(length,0x6c);bytes.write('PRAC',0x70,'ascii');json.copy(bytes,0x74);
  return bytes;
}
function cleanConfig(){return{...TH08_PRACTICE_FRESH_CONFIG};}
function parsedConfig(){return parseTh08PracticeReplayMetadata(rawReplay()).config;}
function goodEvidence(bytes=rawReplay()){
  const sha256=createHash('sha256').update(bytes).digest('hex'),config=parseTh08PracticeReplayMetadata(bytes).config;
  const runtimeStage=th08PracticeRuntimeStage(config);
  const applied={score:123456,displayScore:123456,lives:4,bombs:3,power:96,gauge:10000,gaugeCopy:10000,graze:23,grazeStage:23,
    points:234,pointsStage:345,timeOrbs:321,totalTimeOrbs:321,pointValue:43210,clockTime:4,rank:10,rankMinimum:10,
    rankMaximum:10,familiarCount:17,familiarPending:true,stage:runtimeStage,difficulty:1,character:2};
  return{
    recording:{epoch:1,frameBefore:7,frameAfter:11,saveSucceeded:true,idbfsSyncSucceeded:true,persistence:automaticPersistence(),path:'replay/th8_01.rpy',
      rawBytes:bytes,sha256,inputCapacityFrames:4},
    persisted:{rawBytes:Buffer.from(bytes),freshEpochSha256:sha256},
    import:{beforeLaunch:true,writeSucceeded:true,listed:true,path:'replay/th8_udp05a.rpy',rawBytes:Buffer.from(bytes),sha256,readSha256:sha256},
    fresh:{epoch:5,configured:cleanConfig()},
    menu:{candidate:{...config}},
    playback:{selectedGame:{difficulty:1,character:2},initialSnapshot:{schema:'eagler-touhou/th08-practice-initial-snapshot/1',validMask:3,
      state:{stage:runtimeStage,gameFlags:8,difficulty:1,character:2,replay:true},run:{...config},applied:{...applied},
      cursor:{stageMask:1<<runtimeStage,frame:0,inputCursor:0,timingCursor:0,inputBytes:6,timingBytes:2,inputEnd:6,timingEnd:2},
      source:'GameplayScene::load after PracticeRuntime::apply_practice and before bind_jobs'},
      liveSnapshot:{state:{active:true,replay:true,inGame:true},run:{...config},applied:{...applied,score:123466,displayScore:123466}},
      stream:{available:true,loaded:true,stage:runtimeStage,gameFlags:8,frame:2,inputCursor:4,timingCursor:1,inputBytes:6,timingBytes:2}},
  };
}

test('th08 declares PRACTICE-05 only where the native roundtrip adapter exists',()=>{
  assert.deepEqual(practiceAdaptationIds('th08','normal'),[
    'PRACTICE-01','PRACTICE-02','PRACTICE-03','PRACTICE-04','PRACTICE-05','PRACTICE-06','PRACTICE-07','PRACTICE-08']);
  assert.deepEqual(practiceAdaptationIds('th06','normal'),[
    'PRACTICE-01','PRACTICE-02','PRACTICE-03','PRACTICE-04','PRACTICE-06','PRACTICE-07','PRACTICE-08']);
  assert.deepEqual(practiceAdaptationIds('th08','multiplayer'),[
    'PRACTICE-01','PRACTICE-02','PRACTICE-03','PRACTICE-04','PRACTICE-06','PRACTICE-07','PRACTICE-08']);
  const checks=practiceAdaptationChecks({game:'th08',productSupported:true,feature:true,practiceReplayRoundTrip:{status:'observed',complete:true}});
  assert.equal(checks.find(row=>row.id==='PRACTICE-05').complete,true);
  const partial=practiceAdaptationChecks({game:'th08',productSupported:true,feature:true,practiceReplayRoundTrip:{status:'observed',complete:false}});
  assert.equal(partial.find(row=>row.id==='PRACTICE-05').status,'incomplete');
  const missing=practiceAdaptationChecks({game:'th08',productSupported:true,feature:true});
  assert.equal(missing.find(row=>row.id==='PRACTICE-05').status,'incomplete');
});

test('independent PRAC tail parser maps serialized fields and format defaults',()=>{
  const parsed=parseTh08PracticeReplayMetadata(rawReplay());
  assert.equal(parsed.schema,'eagler-touhou/th08-practice-replay-metadata/1');
  assert.equal(parsed.payload.game,'th08');
  assert.deepEqual(parsed.config,{mode:1,stage:0,warp:0,section:0,phase:0,frame:0,dlg:false,score:1234560,
    life:4,bomb:3,power:96,gauge:10000,graze:23,point:0,point_total:234,point_stage:345,time:321,value:43210,
    night:4,familiar:17,rank:10,rankLock:true,sentinel:1,sp1_pts:false,bsX:0,bsY:0});
  assert.throws(()=>parseTh08PracticeReplayMetadata(rawReplay({...payload,score:undefined})),/missing required field score/);
  assert.throws(()=>parseTh08PracticeReplayMetadata(rawReplay({...payload,game:'th09'})),/th08 versioned object/);
  const corrupt=rawReplay();corrupt.writeUInt32LE(0xffffffff,0x6c);
  assert.throws(()=>parseTh08PracticeReplayMetadata(corrupt),/USER block length is invalid/);
});

test('roundtrip assessor requires all real save, persistence, import, native activation and cursor evidence',()=>{
  const passed=assessTh08PracticeReplayRoundTrip(goodEvidence());
  assert.equal(passed.status,'observed');assert.equal(passed.complete,true);
  assert.equal(passed.evidence.playback.frame,2);
  assert.equal(passed.evidence.livePracticeState.scoreDelta,10,'Replay score may legitimately grow after the initialization boundary');
  assert.equal(passed.evidence.initialPracticeApplication.applied.score,123456);
  const noTick=goodEvidence();noTick.recording.frameAfter=noTick.recording.frameBefore;
  assert.equal(assessTh08PracticeReplayRoundTrip(noTick).status,'incomplete');
  const noSync=goodEvidence();noSync.recording.idbfsSyncSucceeded=false;
  assert.equal(assessTh08PracticeReplayRoundTrip(noSync).status,'incomplete');
  const notImportedBeforeLaunch=goodEvidence();notImportedBeforeLaunch.import.beforeLaunch=false;
  assert.equal(assessTh08PracticeReplayRoundTrip(notImportedBeforeLaunch).status,'incomplete');
  const noPlayback=goodEvidence();noPlayback.playback.stream.inputCursor=0;
  assert.equal(assessTh08PracticeReplayRoundTrip(noPlayback).status,'failed');
  const noInitial=goodEvidence();delete noInitial.playback.initialSnapshot;
  assert.match(assessTh08PracticeReplayRoundTrip(noInitial).missing.join(' '),/immutable practice-application snapshot/);
  const malformedInitial=goodEvidence();delete malformedInitial.playback.initialSnapshot.state;
  assert.equal(assessTh08PracticeReplayRoundTrip(malformedInitial).status,'incomplete');
  const cursorRegressed=goodEvidence();cursorRegressed.playback.initialSnapshot.cursor.inputCursor=4;
  assert.match(assessTh08PracticeReplayRoundTrip(cursorRegressed).problems.join(' '),/initial Replay cursor was not captured/);
});

test('PRACTICE-05 binds Last Word metadata to its mapped native stage',()=>{
  const bytes=rawReplay({...payload,stage:9,section:108});
  const accepted=assessTh08PracticeReplayRoundTrip(goodEvidence(bytes));
  assert.equal(accepted.status,'observed');
  assert.equal(accepted.evidence.initialPracticeApplication.expectedRuntimeStage,5);
  assert.equal(accepted.evidence.initialPracticeApplication.stage,5);
  const wrongStage=goodEvidence(bytes);wrongStage.playback.initialSnapshot.state.stage=0;
  assert.match(assessTh08PracticeReplayRoundTrip(wrongStage).problems.join(' '),/another stage/);
});

test('roundtrip assessor rejects stale or substituted Replay bytes and wrong native PRAC consumer',()=>{
  const badPersistence=goodEvidence();badPersistence.persisted.freshEpochSha256='0'.repeat(64);
  assert.match(assessTh08PracticeReplayRoundTrip(badPersistence).problems.join(' '),/persisted Replay differs/);
  const wrongImport=goodEvidence();wrongImport.import.readSha256='0'.repeat(64);
  assert.match(assessTh08PracticeReplayRoundTrip(wrongImport).problems.join(' '),/imported Replay bytes differ/);
  const wrongCandidate=goodEvidence();wrongCandidate.menu.candidate.score++;
  assert.match(assessTh08PracticeReplayRoundTrip(wrongCandidate).problems.join(' '),/candidate differs/);
  const leaked=goodEvidence();leaked.fresh.configured.score=payload.score;
  assert.match(assessTh08PracticeReplayRoundTrip(leaked).problems.join(' '),/fresh Runtime did not start/);
  const wrongApplied=goodEvidence();wrongApplied.playback.initialSnapshot.applied.gauge=9999;
  assert.match(assessTh08PracticeReplayRoundTrip(wrongApplied).problems.join(' '),/initial\.applied\.gauge/);
  const wrongDifficulty=goodEvidence();wrongDifficulty.playback.initialSnapshot.applied.difficulty=0;
  assert.match(assessTh08PracticeReplayRoundTrip(wrongDifficulty,{selectionCase:TH08_PRACTICE_SELECTION_CASE}).problems.join(' '),/initial\.applied\.difficulty/);
  const wrongCharacter=goodEvidence();wrongCharacter.playback.initialSnapshot.state.character=1;
  assert.match(assessTh08PracticeReplayRoundTrip(wrongCharacter,{selectionCase:TH08_PRACTICE_SELECTION_CASE}).problems.join(' '),/another title selection/);
  const onlyLiveWrong=goodEvidence();onlyLiveWrong.playback.liveSnapshot.applied.gauge=9999;
  assert.equal(assessTh08PracticeReplayRoundTrip(onlyLiveWrong).status,'observed',
    'Post-tick live values are evidence of evolution, not the initialization parameter oracle');
  const substitutedSaved=goodEvidence();substitutedSaved.recording.rawBytes=rawReplay({...payload,score:payload.score+10});
  assert.match(assessTh08PracticeReplayRoundTrip(substitutedSaved).problems.join(' '),/saved Replay SHA-256 does not match/);
  const substitutedFresh=goodEvidence();substitutedFresh.persisted.rawBytes=rawReplay({...payload,score:payload.score+10});
  assert.match(assessTh08PracticeReplayRoundTrip(substitutedFresh).problems.join(' '),/fresh-epoch Replay SHA-256 does not match/);
  const substitutedImport=goodEvidence();substitutedImport.import.rawBytes=rawReplay({...payload,score:payload.score+10});
  assert.match(assessTh08PracticeReplayRoundTrip(substitutedImport).problems.join(' '),/import readback SHA-256 does not match/);
  const noCapacity=goodEvidence();delete noCapacity.recording.inputCapacityFrames;
  assert.equal(assessTh08PracticeReplayRoundTrip(noCapacity).status,'incomplete');
  const emptyCapacity=goodEvidence();emptyCapacity.recording.inputCapacityFrames=0;
  assert.match(assessTh08PracticeReplayRoundTrip(emptyCapacity).problems.join(' '),/no independently parsed input\/timing capacity/);
});

test('native save finalizer bridge uses a bounded C string and releases wasm memory',()=>{
  const prior=globalThis.window,buffer=new ArrayBuffer(128),calls=[];
  const core={memory:{buffer},allocate:size=>{calls.push(['allocate',size]);return 16;},deallocate:pointer=>calls.push(['free',pointer]),
    save_replay:(app,slot,pointer)=>{calls.push(['save',app,slot,new TextDecoder().decode(new Uint8Array(buffer,pointer,8))]);return 1;}};
  globalThis.window={__th08Runtime:{core,Module:core,app:3}};
  try{
    assert.deepEqual(finalizeTh08PracticeReplayRaw({game:'th08',slot:1,name:'P05TEST'}),
      {available:true,finalized:true,slot:1,name:'P05TEST',afterFinalizer:null});
    assert.deepEqual(calls,[['allocate',8],['save',3,1,'P05TEST\0'],['free',16]]);
    assert.equal(finalizeTh08PracticeReplayRaw({game:'th06'}).available,false);
    globalThis.window={Module:core,core,__th08Runtime:{app:3}};
    assert.equal(finalizeTh08PracticeReplayRaw({game:'th08'}).available,false,'the finalizer cannot use a foreign Module');
    assert.equal(calls.length,3,'no save call reaches a different instance');
  }finally{if(prior===undefined)delete globalThis.window;else globalThis.window=prior;}
});

test('IDBFS observer preserves production sync and only credits starts after the finalizer boundary',async()=>{
  const prior=globalThis.window,pending=[],calls=[];
  const fs={syncfs(populate,callback){calls.push(populate);pending.push(callback);}};
  const buffer=new ArrayBuffer(128),core={memory:{buffer},allocate:()=>16,deallocate(){},save_replay:()=>1};
  globalThis.window={__th08Runtime:{core,Module:{FS:fs},app:3}};
  try{
    armTh08PracticeIdbfsObserverRaw();fs.syncfs(false,()=>{});
    const finalized=finalizeTh08PracticeReplayRaw({game:'th08'});
    assert.equal(finalized.afterFinalizer.startSequence,1);
    const wait=waitTh08PracticeIdbfsObserverRaw({timeoutMs:100,afterFinalizer:finalized.afterFinalizer});let settled=false;wait.then(()=>{settled=true;});
    pending[0](null);await Promise.resolve();assert.equal(settled,false,'old in-flight completion cannot satisfy the new native save');
    fs.syncfs(true,()=>{});pending[1](null);await Promise.resolve();assert.equal(settled,false,'populate cannot prove save persistence');
    fs.syncfs(false,()=>{});pending[2](Error('failed'));await Promise.resolve();assert.equal(settled,false,'failed post-boundary sync cannot satisfy persistence');
    fs.syncfs(false,()=>{});pending[3](null);const receipt=await wait;
    assert.equal(receipt.succeeded,true);assert.equal(receipt.startSequence,4);assert.ok(receipt.startedAt>=finalized.afterFinalizer.at);assert.ok(receipt.completedAt>=receipt.startedAt);
    const restored=restoreTh08PracticeIdbfsObserverRaw();assert.equal(restored.restored,true);assert.equal(restored.calls.length,4);assert.deepEqual(calls,[false,true,false,false]);
  }finally{if(prior===undefined)delete globalThis.window;else globalThis.window=prior;}
});
test('IDBFS observer refuses missing boundaries and absence of a completed automatic sync',async()=>{
  const prior=globalThis.window;const fs={syncfs(){}};globalThis.window={__th08Runtime:{Module:{FS:fs}}};
  try{armTh08PracticeIdbfsObserverRaw();assert.equal((await waitTh08PracticeIdbfsObserverRaw({timeoutMs:5})).available,false);
    const afterFinalizer={startSequence:0,at:performance.now()};fs.syncfs(false,()=>{});
    const result=await waitTh08PracticeIdbfsObserverRaw({timeoutMs:5,afterFinalizer});assert.equal(result.succeeded,false);assert.match(result.reason,/timed out/);
    restoreTh08PracticeIdbfsObserverRaw();
  }finally{if(prior===undefined)delete globalThis.window;else globalThis.window=prior;}
});

test('retained P05 persistence is independently recomputed from its actual start and completion receipts',()=>{
  const missing=goodEvidence();delete missing.recording.persistence;assert.equal(assessTh08PracticeReplayRoundTrip(missing).status,'incomplete');
  for(const mutate of [p=>p.finalizer.finalized=false,p=>p.observer.startSequence=0,p=>p.observer.startedAt=9,p=>p.observer.completed=false,p=>p.observer.succeeded=false,p=>p.observer.populate=true,p=>p.restored.restored=false,p=>p.restored.calls=[],p=>p.observer.afterFinalizer={startSequence:2,at:10}]){
    const e=goodEvidence();mutate(e.recording.persistence);assert.equal(assessTh08PracticeReplayRoundTrip(e).status,'failed');
  }
});
