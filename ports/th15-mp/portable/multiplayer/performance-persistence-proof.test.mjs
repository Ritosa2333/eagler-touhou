import test from 'node:test';
import assert from 'node:assert/strict';
import {proofState} from './performance-input-proof.mjs';
import {classifyPersistenceAttempt,collectStablePersistence} from './performance-persistence-proof.mjs';

// These fixtures exercise evidence rules, not the native validator or game.
function snapshot({seat=0,players=2,delay=2,generation=4,capture=40,committed=capture+delay-1}={}){
  const net=Array(24).fill(0),rollback=Array(20).fill(0);
  Object.assign(net,{1:1,2:1,5:players,6:seat,7:delay,12:generation});
  Object.assign(rollback,{0:1,2:capture,3:capture,4:committed});
  return {net,rollback,nativePhase:1,error:''};
}
function replay(frames,players=2,recordedSeat=0){
  const data=new Uint8Array(180+frames*players*12),view=new DataView(data.buffer);
  data.set(new TextEncoder().encode('EAGLRPY1'));
  view.setUint32(8,1,true);view.setUint32(12,15,true);view.setUint32(16,1,true);
  data[20]=players;data[21]=recordedSeat;
  view.setUint32(24,132,true);view.setUint32(28,1,true);view.setUint32(32,frames,true);
  for(let frame=14;frame<frames;++frame)for(let seat=0;seat<players;++seat)
    view.setUint16(180+(frame*players+seat)*12,1,true);
  return data;
}
function cycle({token=1,id=token,entered=100,requested=entered,completed=110,fsStart=111,fsEnd=120}={}){
  return [{kind:'native-request',entered,at:requested,token,result:token},
    {kind:'native-poll',entered:completed,at:completed,token,result:1},
    {kind:'fs-sync-start',at:fsStart,id,populate:false},
    {kind:'fs-sync-complete',at:fsEnd,id,populate:false,error:null}];
}
function file(name,kind,memory,indexedDb=memory){return {name,kind,memory:Array.from(memory),indexedDb:Array.from(indexedDb),
  equal:memory.length===indexedDb.length&&memory.every((byte,i)=>byte===indexedDb[i]),nativeValid:true,memoryNativeValid:true};}
function fixture(){
  const buffer=replay(40),reference=proofState(snapshot({capture:10}));
  return {reference,proofEnd:20,baselineReplay:replay(12),
    sync:{label:'evidence-sync',timeOrigin:1000,start:95,ackAt:125,request:'host-request-1',snapshot:snapshot({capture:30})},
    copy:{buffer,metadata:{label:'evidence-sync',timeOrigin:1000,copiedAt:130,saveEventCount:4,nativeValid:true,snapshot:snapshot({capture:40})}},
    persistence:{database:'/savesth15mp',store:'FILE_DATA',capturedAt:140,readAt:150,saveEventCountBegin:4,saveEventCountEnd:4,
      files:[file('scoreth15.dat',0,[1,2,3]),file('th15.cfg',2,[4,5,6])],replay:file('replay/th15mp_01.rpy',1,buffer)},
    saved:{timeOrigin:1000,saveApi:true,events:cycle(),status:snapshot({capture:50})}};
}
function runtimeWindow(f,{startAt=95,ackAt=125,startCount=0,ackCount=4}={}){
  const {request,label}=f.sync,timeOrigin=f.saved.timeOrigin;
  const start={markerIndex:0,request,label,edge:'start',timeOrigin,at:startAt,saveEventCount:startCount};
  const ack={markerIndex:1,request,label,edge:'ack',timeOrigin,at:ackAt,saveEventCount:ackCount};
  f.saved.persistenceMarkers=[start,ack];
  f.sync.runtimeWindow={request,label,start:{...start},ack:{...ack}};return f;
}
function changeMarker(f,edge,fields){
  Object.assign(f.sync.runtimeWindow[edge],fields);
  Object.assign(f.saved.persistenceMarkers[edge==='start'?0:1],fields);
}
function superseded({duringRead=false,sameBytes=false}={}){
  const f=fixture(),next=sameBytes?f.copy.buffer:replay(42);
  f.saved.events.push(...cycle({token:2,entered:duringRead?142:135,completed:duringRead?143:136,fsStart:duringRead?144:137,fsEnd:duringRead?146:138}));
  f.persistence.saveEventCountBegin=duringRead?4:8;f.persistence.saveEventCountEnd=8;
  f.persistence.replay=file('replay/th15mp_01.rpy',1,duringRead?f.copy.buffer:next,next);return f;
}
function laterStable(){
  const f=superseded();
  f.saved.events.push(...cycle({token:3,entered:160,completed:170,fsStart:171,fsEnd:180}));
  Object.assign(f.sync,{start:155,ackAt:185,request:'host-request-3'});
  f.copy.buffer=replay(44);Object.assign(f.copy.metadata,{copiedAt:190,saveEventCount:12});
  Object.assign(f.persistence,{capturedAt:200,readAt:210,saveEventCountBegin:12,saveEventCountEnd:12,
    replay:file('replay/th15mp_01.rpy',1,f.copy.buffer)});
  return f;
}
function raceFixture(){
  const f=fixture(),base={seat:2,players:3,delay:2,generation:0};
  f.reference=proofState(snapshot({...base,capture:450,committed:449}));f.proofEnd=500;f.baselineReplay=replay(480,3,2);
  f.sync={label:'evidence-sync',timeOrigin:1792844153958,start:59305.3,ackAt:59353.4,request:'actual-shape-token-4',
    snapshot:snapshot({...base,capture:502,committed:500})};
  f.copy={buffer:replay(501,3,2),metadata:{label:'evidence-sync',timeOrigin:1792844154020.8,copiedAt:59452.8,
    saveEventCount:16,nativeValid:true,snapshot:snapshot({...base,capture:505,committed:503})}};
  f.saved={timeOrigin:1792844154020.8,saveApi:true,status:snapshot({...base,capture:520,committed:518}),events:[
    ...cycle({token:1,entered:30000,completed:30050,fsStart:30051,fsEnd:30060}),
    ...cycle({token:2,entered:57679.3,requested:57846.6,completed:57846.9,fsStart:57848.5,fsEnd:58055.1}),
    ...cycle({token:3,entered:58880.8,requested:58898.9,completed:58898.9,fsStart:58905,fsEnd:59055.1}),
    ...cycle({token:4,entered:59246.6,requested:59253.5,completed:59253.6,fsStart:59259.9,fsEnd:59290}),
    ...cycle({token:5,entered:60132.2,requested:60132.9,completed:60133,fsStart:60133.2,fsEnd:60193.4})]};
  Object.assign(f.persistence,{capturedAt:60272.9,readAt:60385.9,saveEventCountBegin:20,saveEventCountEnd:20,
    replay:file('replay/th15mp_01.rpy',1,replay(513,3,2))});return f;
}
const fakeWait=async promise=>await promise;

test('a stable observation binds Host ACK to native Completed and its matching FS callback',()=>{
  const result=classifyPersistenceAttempt(fixture());assert.equal(result.state,'stable');
  assert.deepEqual([result.transaction.token,result.transaction.fsId,result.transaction.hostRequest],[1,1,'host-request-1']);
  assert.equal(result.transaction.joinedInFlight,false);assert.equal(result.baselineInputFrames,12);
  assert.equal(result.copyEqualsMemory,true);assert.equal(result.copyEqualsIndexedDb,true);
});
test('Host sync can join the one already pending real native and filesystem save',()=>{
  const f=fixture();f.sync.start=105;
  const result=classifyPersistenceAttempt(f);assert.equal(result.state,'stable');assert.equal(result.transaction.joinedInFlight,true);
});
test('same-Runtime markers resolve a 0.1ms Host conversion offset without relaxing the legacy rule',()=>{
  const f=fixture();f.sync.ackAt=119.9;
  assert.throws(()=>classifyPersistenceAttempt(f),/No completed native request/);
  runtimeWindow(f,{ackAt:120.1});
  const result=classifyPersistenceAttempt(f);
  assert.equal(result.state,'stable');assert.equal(result.transaction.token,1);
  assert.equal(result.transaction.hostAckAt,120.1);assert.equal(result.transaction.associationClock,'runtime-markers');
  assert.equal(f.sync.ackAt,119.9);assert.equal(result.observationWindow.runtimeSyncAck.saveEventCount,4);
});
test('new windows require matching request, label, origin and independently retained markers',()=>{
  const changes=[f=>{f.sync.runtimeWindow.request='wrong-request';},f=>{f.sync.runtimeWindow.label='wrong-label';},
    f=>{f.sync.runtimeWindow.start.request='wrong-request';},f=>{f.sync.runtimeWindow.ack.label='wrong-label';},
    f=>changeMarker(f,'start',{timeOrigin:1001}),f=>changeMarker(f,'ack',{timeOrigin:1001}),
    f=>{f.sync.runtimeWindow.start.at=94;},f=>{delete f.saved.persistenceMarkers;},
    f=>{delete f.sync.runtimeWindow;},f=>{f.sync.runtimeWindow=null;},f=>changeMarker(f,'ack',{markerIndex:0})];
  for(const change of changes){const f=runtimeWindow(fixture());change(f);assert.throws(()=>classifyPersistenceAttempt(f),/Runtime persistence/);}
});
test('completion membership uses the marker event-count half-open interval even at equal timestamps',()=>{
  for(const bounds of [{startAt:120,startCount:4,ackAt:125,ackCount:4},
                       {startAt:95,startCount:0,ackAt:120,ackCount:3}]){
    const f=runtimeWindow(fixture(),bounds);
    assert.throws(()=>classifyPersistenceAttempt(f),/No completed native request/);
  }
  const joined=runtimeWindow(fixture(),{startAt:119,startCount:3,ackAt:125,ackCount:4});
  const result=classifyPersistenceAttempt(joined);
  assert.equal(result.state,'stable');assert.equal(result.transaction.joinedInFlight,true);
  assert.equal(result.transaction.filesystemCompletionEventIndex,joined.sync.runtimeWindow.start.saveEventCount);
});
test('new marker clocks and event counts must agree with each other and actual save events',()=>{
  for(const [edge,fields] of [['start',{saveEventCount:1}],['ack',{saveEventCount:2}],
                            ['ack',{saveEventCount:5}],['ack',{at:94}],['ack',{saveEventCount:3.5}]]){
    const f=runtimeWindow(fixture());changeMarker(f,edge,fields);
    assert.throws(()=>classifyPersistenceAttempt(f),/Runtime persistence/);
  }
});
test('Runtime ACK must precede the retained copy, MEMFS and IDB snapshot boundaries',()=>{
  for(const change of [f=>changeMarker(f,'ack',{at:131}),f=>{f.copy.metadata.saveEventCount=3;},
    f=>{f.persistence.capturedAt=129;},f=>{f.persistence.readAt=139;}]){
    const f=runtimeWindow(fixture());change(f);assert.throws(()=>classifyPersistenceAttempt(f),/timestamps|event count/);
  }
});
test('a same-clock window can join an old pending save but cannot make its short Replay a fresh proof',()=>{
  const f=runtimeWindow(fixture(),{startAt:105,startCount:1});
  assert.equal(classifyPersistenceAttempt(f).transaction.joinedInFlight,true);
  f.copy.buffer=replay(19);f.persistence.replay=file('replay/th15mp_01.rpy',1,f.copy.buffer);
  assert.throws(()=>classifyPersistenceAttempt(f),/fixed all-seat proof horizon/);
});
test('same-clock correlation retains supersession, ambiguous transactions and hard input failures',()=>{
  const supersededResult=classifyPersistenceAttempt(runtimeWindow(superseded()));
  assert.equal(supersededResult.state,'superseded');assert.equal(supersededResult.transaction.token,1);
  const ambiguous=fixture();ambiguous.saved.events.push(...cycle({token:2,entered:121,completed:122,fsStart:123,fsEnd:124}));
  ambiguous.copy.metadata.saveEventCount=ambiguous.persistence.saveEventCountBegin=ambiguous.persistence.saveEventCountEnd=8;
  const result=classifyPersistenceAttempt(runtimeWindow(ambiguous,{ackCount:8}));
  assert.equal(result.state,'superseded');assert.equal(result.transaction,null);assert.equal(result.candidateTransactions.length,2);
  const changed=runtimeWindow(superseded());changed.persistence.replay.indexedDb[180+18*2*12]^=1;changed.persistence.replay.equal=false;
  assert.throws(()=>classifyPersistenceAttempt(changed),/common input prefix changed/);
});
test('new windows reject a reused retained request marker instead of fabricating another ACK pair',()=>{
  const f=runtimeWindow(fixture()),first=f.saved.persistenceMarkers.map(marker=>({...marker}));
  const second=first.map(marker=>({...marker,markerIndex:marker.markerIndex+2,at:125,saveEventCount:4}));
  f.saved.persistenceMarkers.push(...second);
  f.sync.runtimeWindow.start={...second[0]};f.sync.runtimeWindow.ack={...second[1]};
  assert.throws(()=>classifyPersistenceAttempt(f),/request order changed/);
});
test('observed copy 59452.8 to autosave 60132.2 then stable MEMFS 60272.9/IDB 60385.9 is superseded even when Begin equals End 20',()=>{
  const f=raceFixture(),result=classifyPersistenceAttempt(f);
  assert.equal(result.state,'superseded');assert.equal(result.transaction.token,4);assert.equal(result.transaction.fsId,4);
  assert.equal(result.transaction.joinedInFlight,false);assert.equal(result.commonInputFrames,501);
  assert.deepEqual(result.supersedingEvents.map(event=>[event.kind,event.token??event.id]),[['native-request',5],['fs-sync-start',5]]);
  assert.equal(f.persistence.saveEventCountBegin,20);assert.equal(f.persistence.saveEventCountEnd,20);
  assert.equal(result.comparisons[2].memoryEqualsIndexedDb,true);assert.equal(result.copyEqualsMemory,false);
});
test('an outgoing writer between MEMFS and IDB reads requires a new full observation',()=>{
  const result=classifyPersistenceAttempt(superseded({duringRead:true}));
  assert.equal(result.state,'superseded');assert.equal(result.copyEqualsMemory,true);assert.equal(result.copyEqualsIndexedDb,false);
});
test('a real writer still supersedes an observation when the resulting bytes happen to match',()=>{
  const result=classifyPersistenceAttempt(superseded({sameBytes:true}));
  assert.equal(result.state,'superseded');assert.equal(result.copyEqualsMemory,true);assert.equal(result.copyEqualsIndexedDb,true);
});
test('two complete native save chains inside the Host interval are explicit ambiguity, not a guessed latest transaction',()=>{
  const f=fixture();f.saved.events.push(...cycle({token:2,entered:121,completed:122,fsStart:123,fsEnd:124}));
  f.copy.metadata.saveEventCount=f.persistence.saveEventCountBegin=f.persistence.saveEventCountEnd=8;
  const result=classifyPersistenceAttempt(f);assert.equal(result.state,'superseded');assert.equal(result.transaction,null);
  assert.deepEqual(result.candidateTransactions.map(value=>value.token),[1,2]);assert.match(result.reason,/multiple/);
});
test('new save events after the captured read boundary do not invalidate the completed snapshot',()=>{
  const f=fixture();f.saved.events.push(...cycle({token:2,entered:160,completed:170,fsStart:171,fsEnd:180}));
  f.saved.events[7].error='future error';
  const result=classifyPersistenceAttempt(f);assert.equal(result.state,'stable');assert.equal(result.ignoredEventsAfterRead,4);
});
test('matching JS ACK alone, an unrelated FS completion, and copy before ACK are not native transaction proof',()=>{
  for(const change of [f=>{f.saved.events=[];f.copy.metadata.saveEventCount=f.persistence.saveEventCountBegin=f.persistence.saveEventCountEnd=0;},
    f=>{f.saved.events[3].id=2;},f=>{f.copy.metadata.copiedAt=124;}]){
    const f=fixture();change(f);assert.throws(()=>classifyPersistenceAttempt(f));
  }
});
test('without a new writer, any file byte mismatch is a hard failure',()=>{
  for(const index of [0,1,2]){const f=fixture(),value=index===2?f.persistence.replay:f.persistence.files[index];
    const at=index===2?36:0;value.memory[at]^=1;value.equal=false;
    assert.throws(()=>classifyPersistenceAttempt(f),/bytes differ without a superseding writer/);
  }
});
test('an equality flag cannot conceal different bytes or falsely reject equal bytes, even with a writer',()=>{
  for(const mismatch of [false,true]){const f=superseded();
    if(mismatch)f.persistence.files[0].memory[0]^=1;else f.persistence.files[0].equal=false;
    assert.throws(()=>classifyPersistenceAttempt(f),/equality flag/);
  }
});
test('native validation failures remain hard failures despite a superseding writer',()=>{
  for(const change of [f=>{f.copy.metadata.nativeValid=false;},
    ...[0,1,2].flatMap(index=>['nativeValid','memoryNativeValid'].map(field=>f=>{(index===2?f.persistence.replay:f.persistence.files[index])[field]=false;}))]){
    const f=superseded();change(f);assert.throws(()=>classifyPersistenceAttempt(f),/validation failed/);
  }
});
test('legacy identical MEMFS bytes share the IDB native check, while distinct unvalidated MEMFS cannot be retried',()=>{
  const f=fixture();for(const value of [...f.persistence.files,f.persistence.replay])delete value.memoryNativeValid;
  assert.equal(classifyPersistenceAttempt(f).state,'stable');
  const g=superseded();g.persistence.files[0].memory[0]^=1;g.persistence.files[0].equal=false;delete g.persistence.files[0].memoryNativeValid;
  assert.throws(()=>classifyPersistenceAttempt(g),/lack native validation/);
});
test('session, seat, delay, capture, phase and committed-horizon failures cannot be hidden by a writer',()=>{
  const changes=[f=>{f.saved.status.net[12]++;},f=>{f.saved.status.net[6]=1;},f=>{f.saved.status.net[7]++;},
    f=>{f.saved.status.rollback[3]=1;},f=>{f.saved.status.nativePhase=2;},f=>{f.saved.status.rollback[4]=18;},
    f=>{f.saved.status.rollback[4]=35;},f=>{f.saved.status.error='native failure';},f=>{f.saved.events[4].net=snapshot({generation:5}).net;}];
  for(const change of changes){const f=superseded();change(f);assert.throws(()=>classifyPersistenceAttempt(f));}
});
test('original baseline input bytes remain immutable across all three Replay observations',()=>{
  const f=superseded();const at=180+2*12;
  f.copy.buffer[at]=7;f.persistence.replay.memory[at]=7;f.persistence.replay.indexedDb[at]=7;
  assert.throws(()=>classifyPersistenceAttempt(f),/baseline input prefix changed/);
});
test('a writer cannot excuse changes to fire inputs or any later common confirmed input frame',()=>{
  for(const frame of [18,35]){const f=superseded(),at=180+frame*2*12;
    f.persistence.replay.memory[at]^=1;f.persistence.replay.indexedDb[at]^=1;
    assert.throws(()=>classifyPersistenceAttempt(f),/common input prefix changed/);
  }
});
test('each Replay keeps the recorded seat and original session header despite a writer',()=>{
  for(const change of [f=>{f.copy.buffer[21]=1;},f=>{f.persistence.replay.memory[21]=f.persistence.replay.indexedDb[21]=1;},
    f=>{f.persistence.replay.memory[16]=f.persistence.replay.indexedDb[16]=2;},f=>{f.baselineReplay[16]=2;}]){
    const f=superseded();change(f);assert.throws(()=>classifyPersistenceAttempt(f),/session header|recorded seat/);
  }
});
test('a short or wrong-player Replay is rejected even if a writer and nativeValid flags exist',()=>{
  for(const buffer of [replay(19),replay(42,3)]){const f=superseded();f.persistence.replay=file('replay/th15mp_01.rpy',1,buffer);
    assert.throws(()=>classifyPersistenceAttempt(f),/proof horizon/);
  }
});
test('native errors, cancellation and filesystem errors are fatal, not supersession',()=>{
  for(const change of [f=>{f.saved.events[1].result=-1;},f=>{f.saved.events[1].nativeError='failed';},
    f=>{f.saved.events[1].kind='native-cancel';},f=>{f.saved.events[3].error='IDB failed';},
    f=>{f.saved.events[4].token=0;},f=>{f.saved.events[4].result=99;}]){
    const f=superseded();change(f);assert.throws(()=>classifyPersistenceAttempt(f));
  }
});
test('event counters and timestamps cannot regress or contradict their snapshot boundaries',()=>{
  for(const change of [f=>{f.copy.metadata.saveEventCount=9;},f=>{f.persistence.saveEventCountEnd=99;},
    f=>{f.persistence.saveEventCountBegin=9;},f=>{f.persistence.capturedAt=120;},
    f=>{f.saved.events[5].at=100;},f=>{f.saved.events[4].entered=1000;},
    f=>{f.copy.metadata.saveEventCount=8;},f=>{f.persistence.saveEventCountBegin=4;},
    f=>{f.saved.events[4].rollback=snapshot().rollback;f.saved.events[4].rollbackObservedAt=130;
      f.saved.events[5].rollback=snapshot().rollback;f.saved.events[5].rollbackObservedAt=129;}]){
    const f=superseded();change(f);assert.throws(()=>classifyPersistenceAttempt(f));
  }
});
test('collector retains each observation once and selects the first later stable full transaction',async()=>{
  const retained=[],called=[],result=await collectStablePersistence({deadline:100,clock:()=>0,wait:fakeWait,
    attempt:async(index,deadline,observation)=>{called.push([index,deadline]);Object.assign(observation,index===1?superseded():laterStable());},
    retain:async(record,observation)=>{retained.push({record,observation});}});
  assert.equal(result.selectedAttempt,2);assert.equal(result.classification.transaction.token,3);
  assert.deepEqual(called,[[1,100],[2,100]]);assert.deepEqual(retained.map(value=>value.record.state),['superseded','stable']);
  assert.equal(result.observation,retained[1].observation);assert.equal(result.attempts.length,2);
});
test('collector stops after three superseded observations and retains all three without a fourth attempt',async()=>{
  let calls=0;const retained=[];
  await assert.rejects(collectStablePersistence({deadline:100,clock:()=>0,wait:fakeWait,
    attempt:async(index,deadline,observation)=>{calls++;Object.assign(observation,superseded());},retain:async record=>{retained.push(record);}}),/after 3 attempts/);
  assert.equal(calls,3);assert.deepEqual(retained.map(value=>[value.attempt,value.state]),[[1,'superseded'],[2,'superseded'],[3,'superseded']]);
});
test('collector preserves the original reference, baseline and proofEnd across retries',async()=>{
  for(const change of [f=>{f.reference.capture++;},f=>{f.proofEnd++;},f=>{f.baselineReplay[180]=1;}]){
    const retained=[];await assert.rejects(collectStablePersistence({deadline:100,clock:()=>0,wait:fakeWait,
      attempt:async(index,deadline,observation)=>{const f=index===1?superseded():laterStable();if(index===2)change(f);Object.assign(observation,f);},
      retain:async record=>{retained.push(record);}}),/changed the original/);
    assert.deepEqual(retained.map(value=>value.state),['superseded','error']);
  }
});
test('collector never retries native or validation errors and retains the partial raw observation once',async()=>{
  for(const failInAttempt of [false,true]){let calls=0;const retained=[];
    await assert.rejects(collectStablePersistence({deadline:100,clock:()=>0,wait:fakeWait,
      attempt:async(index,deadline,observation)=>{calls++;observation.partial='raw evidence';if(failInAttempt)throw Error('attempt failed');
        Object.assign(observation,superseded());observation.copy.metadata.nativeValid=false;},
      retain:async(record,observation)=>{retained.push({record,observation});}}));
    assert.equal(calls,1);assert.equal(retained.length,1);assert.equal(retained[0].record.state,'error');assert.equal(retained[0].observation.partial,'raw evidence');
  }
});
test('collector keeps one total deadline and rejects a late resolved second attempt',async()=>{
  let now=0;const deadlines=[],retained=[];
  await assert.rejects(collectStablePersistence({deadline:100,clock:()=>now,wait:fakeWait,
    attempt:async(index,deadline,observation)=>{deadlines.push(deadline);Object.assign(observation,index===1?superseded():laterStable());now=index===1?50:101;},
    retain:async record=>{retained.push(record);}}),/deadline expired/);
  assert.deepEqual(deadlines,[100,100]);assert.deepEqual(retained.map(value=>value.state),['superseded','error']);
});
test('collector bounds an unresolved real asynchronous attempt and records the timeout without retry',async()=>{
  let calls=0;const retained=[];
  await assert.rejects(collectStablePersistence({deadline:performance.now()+30,
    attempt:async(index,deadline,observation)=>{calls++;observation.partial=true;await new Promise(()=>{});},
    retain:async(record,observation)=>{retained.push({record,observation});}}),/deadline expired/);
  assert.equal(calls,1);assert.equal(retained.length,1);assert.equal(retained[0].record.state,'error');assert.equal(retained[0].observation.partial,true);
});
test('collector does not call retain twice if the evidence writer itself fails',async()=>{
  let retained=0;
  await assert.rejects(collectStablePersistence({deadline:100,clock:()=>0,wait:fakeWait,
    attempt:async(index,deadline,observation)=>{Object.assign(observation,fixture());},
    retain:async()=>{retained++;throw Error('retention failed');}}),/retention failed/);
  assert.equal(retained,1);
});
test('collector rejects an invalid retry budget rather than increasing the three-attempt limit',async()=>{
  for(const maxAttempts of [0,4,1.5,NaN]){
    let called=false;await assert.rejects(collectStablePersistence({deadline:100,maxAttempts,clock:()=>0,wait:fakeWait,
      attempt:async()=>{called=true;},retain:async()=>{called=true;}}),/bounded persistence collection/);assert.equal(called,false);
  }
});
