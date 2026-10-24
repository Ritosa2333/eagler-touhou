import {practiceAdaptationChecks} from '../../../tools/ports/practice-observations.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {assessPracticeParameters,createPracticeReplayObservationInput,practiceSnapshotRequest,readPracticeMenuRaw,
  readPracticeSnapshotRaw,runPracticeGameplayStage,syncPracticeIdbfsRaw} from './practice.mjs';
import {resolveAdaptationAdapter} from './registry.mjs';
import {assessTh08PracticeConsumption,assessTh08PracticeLifecycle,requireTh08PracticeLifecycleComplete,decodeGameplayInitialSnapshot,decodePracticeInitialSnapshot,decodePracticeSnapshot,
  assessTh08PracticeReplayRoundTrip,parseTh08PracticeReplayMetadata,TH08_PRACTICE_FRESH_CONFIG,
  th08ReplayListConfirmReady,th08PracticeFeatureInventory,th08PracticeRuntimeStage,TH08_PRACTICE_SELECTION_CASE} from './titles/th08-practice.mjs';

function automaticPersistence(){
  const afterFinalizer={startSequence:0,at:10},call={startSequence:1,startedAt:11,populate:false,completed:true,completedAt:12,succeeded:true};
  return{finalizer:{available:true,finalized:true,afterFinalizer},observer:{available:true,...call,afterFinalizer},restored:{available:true,restored:true,calls:[call]}};
}

function validWords(){
  const words=Array(128).fill(0);words[0]=1;words[1]=11;
  words[2]=1;words[3]=0;words[4]=0;words[5]=1;words[6]=0;words[7]=1;words[8]=0;words[9]=1;
  words[10]=11;words[11]=1;words[12]=2;words[13]=0;words[14]=1;words[15]=2;words[16]=0;words[17]=1;
  const config=[1,0,0,0,0,0,0,1234560,4,3,96,10000,23,0,234,345,321,43210,4,17,10,1,1,0,48,96];
  for(let index=0;index<config.length;index++){words[20+index]=config[index];words[46+index]=config[index];}
  const applied=[123456,123456,4,3,96,10000,10000,23,23,234,345,321,321,43210,4,10,10,10,17,0,0,0,1,0,0,0,1,2];
  for(let index=0;index<applied.length;index++)words[100+index]=applied[index];
  words[119]=1;return words;
}

function validInitialWords(){
  const words=Array(80).fill(0);words[0]=1;words[1]=3;words[2]=0;words[3]=8;words[4]=1;words[5]=2;
  words[6]=1;words[7]=0;words[8]=0;words[9]=0;words[10]=6;words[11]=2;words[12]=6;words[13]=2;
  const config=[1,0,0,0,0,0,0,1234560,4,3,96,10000,23,0,234,345,321,43210,4,17,10,1,1,0,48,96];
  words.splice(16,26,...config);
  const applied=[123456,123456,4,3,96,10000,10000,23,23,234,345,321,321,43210,4,10,10,10,17,0,0,0,1,0,0,0,1,2];
  words.splice(42,28,...applied);return words;
}

function consumptionContext(){
  const words=validInitialWords();words[1]=5;words[3]=1;
  const identity={epoch:1,app:0,documentTimeOrigin:123,url:'https://fixture/runtime'};
  return{initialRaw:{available:true,words},identityBefore:identity,identityAfter:{...identity}};
}
const assessConsumption=(snapshot,context=consumptionContext())=>assessTh08PracticeConsumption(snapshot,undefined,undefined,context);

function practiceReplayEvidence(){
  const payload={version:'2.6.1.0',game:'th08',mode:1,stage:0,life:4,bomb:3,power:96,gauge:10000,
    score:1234560,graze:23,point_total:234,point_stage:345,time:321,value:43210,night:4,familiar:17,rank:10,rankLock:true};
  const json=Buffer.from(JSON.stringify(payload)),blockLength=(12+json.length+3)&~3,raw=Buffer.alloc(0x68+blockLength);
  raw.writeUInt32LE(0x50523854,0);raw.writeUInt16LE(6,4);raw.writeUInt32LE(0x68,12);
  raw.write('USER',0x68,'ascii');raw.writeUInt32LE(blockLength,0x6c);raw.write('PRAC',0x70,'ascii');json.copy(raw,0x74);
  const sha256=createHash('sha256').update(raw).digest('hex'),parsed=parseTh08PracticeReplayMetadata(raw),stage=th08PracticeRuntimeStage(parsed.config);
  const initialSnapshot=decodePracticeInitialSnapshot({available:true,words:validInitialWords()});
  initialSnapshot.state.stage=stage;initialSnapshot.cursor.stageMask=1<<stage;
  initialSnapshot.run={...parsed.config};initialSnapshot.applied={...initialSnapshot.applied,stage,difficulty:1,character:2};
  const liveApplied={...initialSnapshot.applied,score:initialSnapshot.applied.score+10,displayScore:initialSnapshot.applied.displayScore+10};
  const copied=()=>Buffer.from(raw);
  return{recording:{epoch:1,frameBefore:7,frameAfter:11,saveSucceeded:true,idbfsSyncSucceeded:true,persistence:automaticPersistence(),path:'replay/th8_01.rpy',
      rawBytes:raw,sha256,inputCapacityFrames:4},
    persisted:{rawBytes:copied(),freshEpochSha256:sha256},
    import:{beforeLaunch:true,writeSucceeded:true,listed:true,path:'replay/th8_ud1234.rpy',rawBytes:copied(),sha256,readSha256:sha256},
    fresh:{epoch:2,configured:{...TH08_PRACTICE_FRESH_CONFIG}},menu:{candidate:{...parsed.config}},
    playback:{selectedGame:{difficulty:1,character:2},initialSnapshot,
      liveSnapshot:{state:{active:true,replay:true,inGame:true},applied:liveApplied},
      stream:{available:true,loaded:true,stage,gameFlags:8,frame:2,inputCursor:4,timingCursor:1,inputBytes:6,timingBytes:2}}};
}

test('th08 PracticeSnapshot decodes independent configured, accepted and consumed owners',()=>{
  const snapshot=decodePracticeSnapshot({available:true,words:validWords()});
  assert.equal(snapshot.schema,'eagler-touhou/th08-practice-snapshot/1');
  assert.equal(snapshot.state.menu,false);
  assert.equal(snapshot.state.active,true);
  assert.equal(snapshot.configured.score,1234560);
  assert.equal(snapshot.configured.gauge,10000);
  assert.equal(snapshot.run.rankLock,true);
  assert.equal(snapshot.applied.score,123456);
  assert.equal(snapshot.applied.familiarCount,17);
  assert.equal(snapshot.applied.familiarPending,true);
  assert.equal(snapshot.units.clockTime,'native half-hour index');
  assert.equal(assessConsumption(snapshot).status,'observed');
  const inventory=th08PracticeFeatureInventory(snapshot,{pointerTested:true,exercisedKeys:['F7']});
  assert.equal(inventory.status,'observed');
  assert.ok(inventory.items.some(item=>item.id==='native-pointer'&&item.exercised));
  assert.ok(inventory.items.some(item=>item.id==='replay-candidate'&&!item.exercised));
  assert.ok(inventory.items.some(item=>item.id==='trainer-f7'&&item.exercised));
  assert.ok(inventory.items.filter(item=>/^trainer-f[1-6]$/.test(item.id)).every(item=>!item.exercised));
  assert.ok(inventory.items.some(item=>item.id==='trainer-u'&&!item.exercised));
});

test('th08 PracticeSnapshot rejects wrong version, unknown validity, malformed flags and invalid applied evidence',()=>{
  const version=validWords();version[0]=2;
  assert.throws(()=>decodePracticeSnapshot({available:true,words:version}),/Unsupported PracticeSnapshot version/);
  const mask=validWords();mask[1]=0x80;
  assert.throws(()=>decodePracticeSnapshot({available:true,words:mask}),/unknown validity bit/);
  const flag=validWords();flag[5]=2;
  assert.throws(()=>decodePracticeSnapshot({available:true,words:flag}),/flag 5 is invalid/);
  const invalidBool=validWords();invalidBool[26]=2;
  assert.throws(()=>decodePracticeSnapshot({available:true,words:invalidBool}),/config boolean at 26 is invalid/);
  const unavailable=validWords();unavailable[1]=3;
  const noApplied=decodePracticeSnapshot({available:true,words:unavailable});
  assert.equal(assessConsumption(noApplied).status,'incomplete');
  const changed=validWords(),context=consumptionContext();context.initialRaw.words[42]=123455;
  const assessment=assessConsumption(decodePracticeSnapshot({available:true,words:changed}),context);
  assert.equal(assessment.status,'failed');
  assert.ok(assessment.mismatches.some(item=>item.field==='applied.score'));
  assert.throws(()=>decodePracticeSnapshot({available:false,reason:'export unavailable'}),/export unavailable/);
});

test('serialized PracticeSnapshot reader binds app zero and validates the wasm range without module closures',()=>{
  const serializedReader=Function(`return (${readPracticeSnapshotRaw.toString()});`)();
  const memory={buffer:new ArrayBuffer(8+128*8)};
  new Float64Array(memory.buffer,8,128).set(validWords());
  const core={memory,practice_snapshot_v1:app=>{assert.equal(app,0);return 8;}};
  const title=resolveAdaptationAdapter('th08','normal'),practice=title.practice;
  const request=structuredClone(practiceSnapshotRequest({runtime:title.runtime,contract:practice}));
  assert.deepEqual(request,{runtime:{...title.runtime},snapshot:{export:'practice_snapshot_v1',wordCount:128,wordType:'f64',app:true}});
  assert.equal(typeof practice.snapshot.decode,'function','Node retains the title decoder outside the serialized frame request');
  const prior=globalThis.window;globalThis.window={Module:core,__th08Runtime:{core,Module:core,app:0}};
  try{
    const raw=serializedReader(request);
    assert.equal(raw.available,true);
    assert.deepEqual(raw.words,validWords());
    assert.equal(practice.snapshot.decode(raw).applied.score,123456);
    core.practice_snapshot_v1=()=>memory.buffer.byteLength-16;
    assert.equal(serializedReader(request).available,false);
  }finally{if(prior===undefined)delete globalThis.window;else globalThis.window=prior;}
});

test('serialized PracticeSnapshot reader supports a different title ABI without code changes',()=>{
  const serializedReader=Function(`return (${readPracticeSnapshotRaw.toString()});`)();
  const memory={buffer:new ArrayBuffer(4*4)};new Uint32Array(memory.buffer).set([2,41,99,123]);
  const core={memory,read_practice_v2:app=>{assert.equal(app,7);return 4;}};
  const runtime={global:'__nextRuntime',core:'exports',module:'exports',app:'instance'};
  const contract={snapshot:{export:'read_practice_v2',wordCount:3,wordType:'u32',app:true,decode:()=>{}}};
  const request=structuredClone(practiceSnapshotRequest({runtime,contract}));
  const fallback={memory:{buffer:new ArrayBuffer(64)},read_practice_v2:()=>0};
  const prior=globalThis.window;globalThis.window={__nextRuntime:{exports:core,instance:7},Module:fallback,exports:fallback};
  try{
    assert.deepEqual(serializedReader(request),{available:true,words:[41,99,123]});
    assert.equal(serializedReader({runtime,snapshot:{...request.snapshot,wordType:'f16'}}).available,false);
    globalThis.window.HEAPU8=new Uint8Array(memory.buffer);delete core.memory;
    assert.equal(serializedReader(request).available,false,'a foreign realm heap cannot supply a missing selected memory');
    delete globalThis.window.__nextRuntime;
    assert.equal(serializedReader(request).available,false,'a missing declared mount cannot fall back to a global module');
  }finally{if(prior===undefined)delete globalThis.window;else globalThis.window=prior;}
});

test('practice menu and IDBFS readers honor non-Module Runtime mounts without cross-instance fallback',async()=>{
  const menuReader=Function(`return (${readPracticeMenuRaw.toString()});`)();
  const syncReader=Function(`return (${syncPracticeIdbfsRaw.toString()});`)();
  const runtime=structuredClone({global:'__nextRuntime',core:'exports',module:'exports',app:'instance'});
  const wrongInstance={eaglerThpracMenuOpen:false,eaglerOptions:{thpracEnabled:false,thpracLocale:'wrong'},
    FS:{syncfs(_populate,callback){this.wrongCalls=(this.wrongCalls??0)+1;callback(null);}}};
  const calls=[];
  const actualInstance={eaglerThpracMenuOpen:true,eaglerOptions:{thpracEnabled:true,thpracLocale:'ja-JP'},
    FS:{syncfs(populate,callback){calls.push(populate);callback(null);}}};
  const prior=globalThis.window;
  globalThis.window={__nextRuntime:{exports:actualInstance,instance:7},Module:wrongInstance,exports:wrongInstance,
    __practiceBrowserEvents:[{open:true,source:'runtime'}]};
  try{
    assert.deepEqual(menuReader({runtime}),{available:true,open:true,
      options:{thpracEnabled:true,thpracLocale:'ja-JP'},events:[{open:true,source:'runtime'}]});
    assert.deepEqual(await syncReader({runtime}),{available:true,succeeded:true,error:null});
    assert.deepEqual(calls,[false]);
    assert.equal(wrongInstance.FS.wrongCalls,undefined,'the globally named Module must not receive the sync');
    delete globalThis.window.__nextRuntime;
    assert.equal(menuReader({runtime}).available,false);
    assert.deepEqual(await syncReader({runtime}),{available:false,reason:'Declared practice Runtime module is unavailable'});
    assert.equal(wrongInstance.FS.wrongCalls,undefined,'a missing declared mount cannot redirect IDBFS work');
  }finally{if(prior===undefined)delete globalThis.window;else globalThis.window=prior;}
});

test('serialized PracticeSnapshot reader honors the declared no-app export signature',()=>{
  const serializedReader=Function(`return (${readPracticeSnapshotRaw.toString()});`)();
  const memory={buffer:new ArrayBuffer(16)};new Uint32Array(memory.buffer,4,3).set([1,8,2]);
  const core={memory,read_practice_no_app:()=>4};
  const runtime={global:'__plainRuntime',core:'exports',module:'Module',app:'missingApp'};
  const contract={snapshot:{export:'read_practice_no_app',wordCount:3,wordType:'u32',app:false,decode:()=>{}}};
  const request=structuredClone(practiceSnapshotRequest({runtime,contract}));
  const prior=globalThis.window;globalThis.window={__plainRuntime:{exports:core,Module:core}};
  try{assert.deepEqual(serializedReader(request),{available:true,words:[1,8,2]});}
  finally{if(prior===undefined)delete globalThis.window;else globalThis.window=prior;}
});

test('serialized PracticeSnapshot reader reports missing Runtime identity and export from declared ABI',()=>{
  const title=resolveAdaptationAdapter('th08','normal');
  const prior=globalThis.window;globalThis.window={__th08Runtime:{core:{memory:{buffer:new ArrayBuffer(2048)}},app:null}};
  const request=structuredClone(practiceSnapshotRequest({runtime:title.runtime,contract:title.practice}));
  try{
    assert.match(readPracticeSnapshotRaw(request).reason,/Runtime iframe identity/);
    globalThis.window.__th08Runtime.app=1;
    assert.match(readPracticeSnapshotRaw(request).reason,/snapshot export or wasm memory/);
  }finally{if(prior===undefined)delete globalThis.window;else globalThis.window=prior;}
});

test('shared practice parameter comparison accepts title-normalized records and detects field drift',()=>{
  const snapshot={state:{active:true,inGame:true},configured:{speed:7,seed:41},run:{speed:7,seed:41},applied:{velocity:14,stage:3}};
  const expected={expectedConfig:{speed:7,seed:41},expectedApplied:{velocity:14,stage:3},
    configFields:['speed','seed'],appliedFields:['velocity','stage']};
  assert.equal(assessPracticeParameters({snapshot,...expected}).status,'observed');
  snapshot.applied.velocity=13;
  const changed=assessPracticeParameters({snapshot,...expected});
  assert.equal(changed.status,'failed');
  assert.deepEqual(changed.mismatches,[{field:'applied.velocity',expected:14,actual:13}]);
});

test('shared practice parameter comparison distinguishes absent evidence from incorrect evidence',()=>{
  const valid={state:{active:true,inGame:true},configured:{enabled:false,level:2},run:{enabled:false,level:2},applied:{level:2}};
  const args={snapshot:valid,expectedConfig:{enabled:false,level:2},expectedApplied:{level:2},
    configFields:['enabled','level'],appliedFields:['level']};
  assert.equal(assessPracticeParameters(args).status,'observed');
  assert.equal(assessPracticeParameters({...args,expectedConfig:{level:2}}).status,'incomplete',
    'a missing expected own field cannot pass through undefined equality');
  assert.equal(assessPracticeParameters({...args,snapshot:{...valid,run:{level:2}}}).status,'incomplete');
  assert.equal(assessPracticeParameters({...args,snapshot:{...valid,applied:{level:3}}}).status,'failed');
  assert.equal(assessPracticeParameters({...args,expectedApplied:{level:{nested:2}}}).status,'incomplete',
    'generic comparisons accept normalized scalar values only');
});

test('th08 difficulty and character expectations come from the declared menu route',()=>{
  const snapshot=decodePracticeSnapshot({available:true,words:validWords()});
  assert.equal(assessConsumption(snapshot).status,'observed');
  const wrongDifficulty={...snapshot,applied:{...snapshot.applied,difficulty:0}};
  assert.equal(assessConsumption(wrongDifficulty).status,'failed');
  const wrongCharacter={...snapshot,applied:{...snapshot.applied,character:1}};
  assert.equal(assessConsumption(wrongCharacter).status,'failed');
  assert.deepEqual(TH08_PRACTICE_SELECTION_CASE,{difficulty:1,character:2,
    source:'GameConfiguration.hpp default difficulty=1 | TitleMenus.cpp PracticeStart copies config.difficulty | CharacterSelect stores the selected cursor'});
});

test('shared gameplay stage recomputes parameter evidence instead of trusting menu callback status',async()=>{
  const record={};
  await assert.rejects(runPracticeGameplayStage({record,epoch:1,stage:async(_name,_details,operation)=>operation(),
    ready:()=>true,perform:async()=>({snapshot:{frames:{simulation:2}},practice:{status:'observed'}}),
    assess:()=>({status:'failed',mismatches:[{field:'applied.level',expected:4,actual:3}]})}),
  /did not match the independent title declaration/);
  assert.equal(record.parameterConsumption.status,'failed');
});

function th08PracticeMenuIo({waitForGameplay}={}){
  const calls={predicates:0,keys:[]};
  let state={schema:'eagler-touhou/th08-practice-snapshot/1',validMask:8,
    state:{titleScreen:0,titlePhase:1,titleCursor:0,menu:false,enabled:true,active:false,replayCandidateValid:false},applied:null};
  const snapshot=()=>({...state,state:{...state.state},applied:state.applied&&{...state.applied}});
  const read=async()=>snapshot();
  const waitUntil=async(label,predicate)=>{
    calls.predicates++;
    const matched=predicate(snapshot());
    if(matched&&typeof matched.then==='function')throw new Error(`${label}: waitUntil accepts synchronous predicates only`);
    if(!matched)throw new Error(`${label}: expected native menu state was not reached`);
    return snapshot();
  };
  const press=async key=>{
    calls.keys.push(key);
    if(key==='ArrowDown'&&state.state.titleScreen===0)
      state={...state,state:{...state.state,titleCursor:state.state.titleCursor+1}};
    else if(key==='KeyZ'&&state.state.titleScreen===0&&state.state.titleCursor===3)
      state={...state,state:{...state.state,titleScreen:8,titlePhase:1,titleCursor:1}};
    else if(key==='KeyZ'&&state.state.titleScreen===8)
      state={...state,state:{...state.state,titleScreen:9,titlePhase:1,titleCursor:0}};
    else if(key==='ArrowRight'&&state.state.titleScreen===9)
      state={...state,state:{...state.state,titleCursor:state.state.titleCursor+1}};
    else if(key==='KeyZ'&&state.state.titleScreen===9&&state.state.titleCursor===TH08_PRACTICE_SELECTION_CASE.character)
      state={...state,state:{...state.state,titleScreen:11,titlePhase:1,menu:true}};
    else if(key==='KeyZ'&&state.state.titleScreen===11)
      state={...state,state:{...state.state,active:true},applied:{score:123456}};
  };
  return{calls,run:()=>resolveAdaptationAdapter('th08','normal').practice.enterMenu({read,waitUntil,press,waitForGameplay})};
}

test('registered th08 menu awaits the async gameplay operation and returns its Runtime observation',async()=>{
  let signalRequested,resolveReady,settled=false;
  const requested=new Promise(resolve=>{signalRequested=resolve;});
  const runtimeObservation={phase:{name:'stage'},localSeat:0,frames:{simulation:37},epoch:1};
  const io=th08PracticeMenuIo({waitForGameplay:()=>{
    signalRequested();return new Promise(resolve=>{resolveReady=resolve;});
  }});
  const operation=io.run();operation.then(()=>{settled=true;},()=>{settled=true;});
  await requested;
  await new Promise(resolve=>setImmediate(resolve));
  try{assert.equal(settled,false,'menu operation must wait for native gameplay readiness');}
  finally{resolveReady(runtimeObservation);}
  const result=await operation;
  assert.deepEqual(result.snapshot,runtimeObservation,'returned snapshot must be the async Runtime observation, not PracticeSnapshot');
  assert.equal(result.practice.state.active,true);
  assert.ok(result.featureInventory.items.some(item=>item.id==='custom-parameter-consumption'&&item.exercised));
  assert.equal(io.calls.predicates,5,'only native title-menu predicates use the synchronous wait helper');
});

test('registered th08 menu propagates a rejected gameplay wait without creating a predicate waiter',async()=>{
  const io=th08PracticeMenuIo({waitForGameplay:async()=>{throw new Error('native gameplay observation failed');}});
  await assert.rejects(io.run(),/native gameplay observation failed/);
  assert.equal(io.calls.predicates,5,'the rejected async operation was not passed to the synchronous predicate helper');
});

test('practice Replay observation input survives JSON and re-runs the original native assessor',()=>{
  const observationInput=createPracticeReplayObservationInput({format:'th08-prac-v1',evidence:practiceReplayEvidence()});
  const saved=JSON.parse(JSON.stringify(observationInput));
  assert.equal(saved.schema,'eagler-touhou/practice-replay-input/1');
  assert.equal(saved.format,'th08-prac-v1');
  for(const key of ['recording','persisted','import'])
    assert.ok(Array.isArray(saved.evidence[key].rawBytes)&&saved.evidence[key].rawBytes.every(byte=>Number.isInteger(byte)&&byte>=0&&byte<=255));
  const rechecked=assessTh08PracticeReplayRoundTrip(saved.evidence,{selectionCase:TH08_PRACTICE_SELECTION_CASE});
  assert.equal(rechecked.status,'observed');assert.equal(rechecked.complete,true);

  const truncated=structuredClone(saved);truncated.evidence.recording.rawBytes.pop();
  assert.equal(assessTh08PracticeReplayRoundTrip(truncated.evidence).status,'failed','raw byte loss must invalidate the saved digest and parser input');
  const wrongDifficulty=structuredClone(saved);wrongDifficulty.evidence.playback.initialSnapshot.applied.difficulty=0;
  assert.equal(assessTh08PracticeReplayRoundTrip(wrongDifficulty.evidence,{selectionCase:TH08_PRACTICE_SELECTION_CASE}).status,'failed');
});

test('PracticeInitialSnapshot decodes the apply boundary and Replay cursor separately from live state',()=>{
  const snapshot=decodePracticeInitialSnapshot({available:true,words:validInitialWords()});
  assert.equal(snapshot.schema,'eagler-touhou/th08-practice-initial-snapshot/1');
  assert.deepEqual(snapshot.state,{stage:0,gameFlags:8,difficulty:1,character:2,replay:true});
  assert.deepEqual(snapshot.cursor,{stageMask:1,frame:0,inputCursor:0,timingCursor:0,
    inputBytes:6,timingBytes:2,inputEnd:6,timingEnd:2});
  assert.equal(snapshot.run.score,1234560);assert.equal(snapshot.applied.score,123456);
  assert.equal(snapshot.source,'GameplayScene::load after PracticeRuntime::apply_practice and before bind_jobs');
  const invalid=validInitialWords();invalid[1]=7;
  assert.throws(()=>decodePracticeInitialSnapshot({available:true,words:invalid}),/unknown validity bit/);
  const wrongBoundary=validInitialWords();wrongBoundary[8]=2;
  assert.throws(()=>decodePracticeInitialSnapshot({available:true,words:wrongBoundary}),/before the first logical playback tick/);
});

test('th08 Last Word config maps to the actual loaded stage by its section',()=>{
  const stages=[0,1,2,5,6,7,8,5,8,3,4,3,3,3,3,3,3];
  stages.forEach((stage,index)=>assert.equal(th08PracticeRuntimeStage({stage:9,section:105+index}),stage));
  assert.equal(th08PracticeRuntimeStage({stage:9,section:104}),-1);
  assert.equal(th08PracticeRuntimeStage({stage:9,section:122}),-1);
  assert.equal(th08PracticeRuntimeStage({stage:10,section:0}),-1);

  const words=validInitialWords();words[2]=5;words[3]=8;words[6]=1<<5;words[67]=5;
  const lastWord=[1,9,0,108,0,0,0,1234560,4,3,96,10000,23,0,234,345,321,43210,4,17,10,1,1,0,48,96];
  // Last Word stage=9 stores LW_4 here; the native mapping resolves its section to gameplay stage 5.
  for(let index=0;index<lastWord.length;index++)words[16+index]=lastWord[index];
  assert.equal(decodePracticeInitialSnapshot({available:true,words}).state.stage,5);

  const mismatched=validInitialWords();mismatched[2]=5;mismatched[3]=8;mismatched[6]=1<<5;mismatched[67]=5;
  for(let index=0;index<26;index++)mismatched[16+index]=lastWord[index];
  mismatched[19]=109;
  assert.throws(()=>decodePracticeInitialSnapshot({available:true,words:mismatched}),/run identity/);
});

test('serialized PracticeInitialSnapshot reader uses its declared ABI and validates its range',()=>{
  const serializedReader=Function(`return (${readPracticeSnapshotRaw.toString()});`)();
  const memory={buffer:new ArrayBuffer(8+80*8)};new Float64Array(memory.buffer,8,80).set(validInitialWords());
  const core={memory,practice_initial_snapshot_v1:app=>{assert.equal(app,0);return 8;}};
  const prior=globalThis.window;globalThis.window={Module:core,__th08Runtime:{core,Module:core,app:0}};
  const title=resolveAdaptationAdapter('th08','normal');
  const request=structuredClone(practiceSnapshotRequest({runtime:title.runtime,contract:title.practice,kind:'initial'}));
  assert.deepEqual(request.snapshot,{export:'practice_initial_snapshot_v1',wordCount:80,wordType:'f64',app:true});
  try{
    const raw=serializedReader(request);
    assert.deepEqual(raw.words,validInitialWords());
    assert.equal(title.practice.initialSnapshot.decode(raw).state.replay,true,
      'Node retains the initial-snapshot decoder outside the serialized frame request');
    core.practice_initial_snapshot_v1=()=>memory.buffer.byteLength-16;
    assert.equal(serializedReader(request).available,false);
  }finally{if(prior===undefined)delete globalThis.window;else globalThis.window=prior;}
});

function validLifecycleEvidence(){
  const before=decodePracticeSnapshot({available:true,words:validWords()});before.applied.enemyFrame=400;
  const retried=structuredClone(before);retried.applied.enemyFrame=6;
  const practiceTitle=structuredClone(before);practiceTitle.state.inGame=false;practiceTitle.state.menu=false;practiceTitle.state.accepted=false;
  // Title leaves these owners alone. Replay menu State(1) resets run and validity only.
  const replayMenu=structuredClone(practiceTitle);replayMenu.run={...replayMenu.run,mode:0};replayMenu.state.replayCandidateValid=false;
  const replay=structuredClone(retried);replay.state.replay=true;replay.state.gameFlags|=8;
  const replayTitle=structuredClone(practiceTitle);replayTitle.state.replay=true;
  const ordinary=structuredClone(retried);ordinary.state.active=false;ordinary.state.replay=false;ordinary.state.gameFlags=4;
  ordinary.applied=null;
  const initials=Object.fromEntries(['before','retried','replay','ordinary','fresh'].map(name=>{
    const words=validInitialWords();words[1]=name==='ordinary'?1:name==='replay'?7:5;words[3]=name==='replay'?8:name==='ordinary'?4:1;
    if(name==='ordinary'){words.fill(0,16,42);words.fill(0,42,70);words[57]=10;words[58]=8;words[59]=16;words[68]=1;words[69]=2;}
    return[name,{available:true,words}];
  }));
  const identities=Object.fromEntries(['before','retried','practiceTitle','replayMenu','replay','replayTitle','ordinary','fresh'].map(name=>[name,{epoch:4,app:0,documentTimeOrigin:123,url:'https://fixture/runtime'}]));
  return{before,retried,practiceTitle,replayMenu,replay,replayTitle,ordinary,fresh:structuredClone(retried),initials,identities,
    ordinaryProgress:{counters:{enemySceneFrame:5}},retryBeforeFrame:400,replayConfig:structuredClone(replay.run),replayStream:{loaded:true,frame:10,inputCursor:20},activeReplayExit:true};
}

test('same Runtime Practice lifecycle preserves configured/sticky owners while rebuilding actual game resources',()=>{
  assert.equal(assessTh08PracticeLifecycle(validLifecycleEvidence()).status,'observed');
  for(const mutate of [
    e=>e.retried.applied.enemyFrame=401,e=>e.initials.retried.words[46]=0,
    e=>e.practiceTitle.state.accepted=true,e=>e.replayMenu.run.mode=1,e=>e.replayMenu.state.replayCandidateValid=true,
    e=>e.replay.run.score=0,e=>e.initials.replay.words[45]=8,e=>e.replayStream.inputCursor=0,
    e=>e.activeReplayExit=false,e=>e.ordinary.state.active=true,e=>e.initials.ordinary.words[46]=96,
    e=>e.initials.ordinary.words[2]=1,e=>e.initials.fresh.words[47]=0,e=>e.fresh.configured.score=0,
  ]){const evidence=validLifecycleEvidence();mutate(evidence);const result=assessTh08PracticeLifecycle(evidence);assert.equal(result.status,'failed',JSON.stringify(result));}
  const missing=validLifecycleEvidence();delete missing.ordinary;assert.equal(assessTh08PracticeLifecycle(missing).status,'incomplete');
});

test('immutable initialization accepts legitimate live resource drift and requires every actual boundary',()=>{
  const evidence=validLifecycleEvidence();
  for(const name of ['before','retried','replay','fresh'])Object.assign(evidence[name].applied,{score:999999,gauge:1,familiarCount:0,power:128,bombs:0});
  assert.equal(assessTh08PracticeLifecycle(evidence).status,'observed');
  for(const name of Object.keys(evidence.initials)){const absent=validLifecycleEvidence();delete absent.initials[name];assert.equal(assessTh08PracticeLifecycle(absent).status,'incomplete');}
  const changed=validLifecycleEvidence();changed.identities.fresh.app=8;assert.equal(assessTh08PracticeLifecycle(changed).status,'failed');
});
test('generic gameplay initial decoder distinguishes ordinary and custom/replay schemas',()=>{
  const e=validLifecycleEvidence();
  assert.equal(decodeGameplayInitialSnapshot(e.initials.ordinary).run,null);
  assert.equal(decodeGameplayInitialSnapshot(e.initials.ordinary).state.customPractice,false);
  assert.equal(decodeGameplayInitialSnapshot(e.initials.replay).schema,'eagler-touhou/th08-gameplay-initial-snapshot/1');
  for(const mask of [2,4,8]){const raw=structuredClone(e.initials.replay);raw.words[1]=mask;assert.throws(()=>decodeGameplayInitialSnapshot(raw));}
  const polluted=structuredClone(e.initials.ordinary);polluted.words[16]=1;assert.throws(()=>decodeGameplayInitialSnapshot(polluted),/ordinary run payload/);
});

test('generic gameplay snapshot request selects the independent export',()=>{const title=resolveAdaptationAdapter('th08','normal');assert.deepEqual(practiceSnapshotRequest({runtime:title.runtime,contract:title.practice,kind:'gameplayInitial'}).snapshot,{export:'gameplay_initial_snapshot_v1',wordCount:80,wordType:'f64',app:true});});


test('lifecycle completion gate rejects missing immutable raw before the outer journey continues',()=>{
  const missing=validLifecycleEvidence();delete missing.initials.retried;
  const assessment=assessTh08PracticeLifecycle(missing);assert.equal(assessment.status,'incomplete');
  assert.throws(()=>requireTh08PracticeLifecycleComplete(assessment),/not complete/);
  for(const result of [undefined,{status:'incomplete',complete:false},{status:'observed',complete:false},{status:'failed',complete:false}])assert.throws(()=>requireTh08PracticeLifecycleComplete(result));
  assert.equal(requireTh08PracticeLifecycleComplete(assessTh08PracticeLifecycle(validLifecycleEvidence())).complete,true);
  for(const mutate of [e=>e.initials.ordinary.words[3]=1,e=>e.initials.before.words[3]=4,e=>e.fresh.state.gameFlags=4]){const e=validLifecycleEvidence();mutate(e);assert.equal(assessTh08PracticeLifecycle(e).status,'failed');}
});

test('first Practice resource assessment uses immutable values and preserves live evidence',()=>{
  const live=decodePracticeSnapshot({available:true,words:validWords()});Object.assign(live.applied,{score:999999,gauge:4,familiarCount:0});
  const result=assessConsumption(live);assert.equal(result.status,'observed');assert.equal(result.liveApplied.score,999999);assert.equal(result.applied.score,123456);assert.ok(result.initialRaw.words);
  for(const mutate of [c=>delete c.initialRaw,c=>delete c.identityBefore]){const context=consumptionContext();mutate(context);assert.equal(assessConsumption(live,context).status,'incomplete');}
  for(const mutate of [c=>c.initialRaw.words[47]=0,c=>c.identityAfter.app=8,c=>c.initialRaw.words[3]=8]){const context=consumptionContext();mutate(context);assert.equal(assessConsumption(live,context).status,'failed');}
  const wrongRun=structuredClone(live);wrongRun.run.score=0;assert.equal(assessConsumption(wrongRun).status,'failed');
});

test('Replay list confirmation waits for the actual native input guard',()=>{
  for(const timer of [undefined,0,8,9])assert.equal(th08ReplayListConfirmReady({state:{titleScreen:7,titlePhase:1,titleTimer:timer}}),false);
  for(const timer of [10,11])assert.equal(th08ReplayListConfirmReady({state:{titleScreen:7,titlePhase:1,titleTimer:timer}}),true);
  assert.equal(th08ReplayListConfirmReady({state:{titleScreen:7,titlePhase:0,titleTimer:20}}),false);
});

test('P04 numbered observations recompute retained immutable initialization and live owner evidence',()=>{
  const live=decodePracticeSnapshot({available:true,words:validWords()}),consumption=assessConsumption(live);
  const row=(snapshot,result)=>practiceAdaptationChecks({game:'th08',productSupported:true,feature:true,locales:[{epoch:1,practiceSnapshot:snapshot,parameterConsumption:result}]}).find(item=>item.id==='PRACTICE-04');
  assert.equal(row(live,consumption).status,'observed');
  const missing=structuredClone(consumption);delete missing.initialRaw;assert.equal(row(live,missing).status,'incomplete');
  const wrong=structuredClone(consumption);wrong.initialRaw.words[47]=0;assert.equal(row(live,wrong).status,'failed');
  const owner=structuredClone(live);owner.state.active=false;assert.equal(row(owner,consumption).status,'failed');
});
