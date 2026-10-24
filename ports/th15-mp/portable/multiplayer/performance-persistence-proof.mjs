// Post-window persistence evidence only. A newer real writer can invalidate an
// observation; it cannot excuse native errors, changed input history, or an
// inconsistent byte-comparison record. Production save behavior is unchanged.
import {assertProofState,assertBeforeDeadline,awaitBeforeDeadline} from './performance-input-proof.mjs';

const epsilon=.001;
const require=(value,message)=>{if(!value)throw Error(message);};
const finite=value=>Number.isFinite(value);
const integer=value=>Number.isSafeInteger(value)&&value>=0;
const token=value=>Number.isInteger(value)&&value>0&&value<=0xffffffff;
const before=(a,b)=>a<=b+epsilon;
function bytes(value,label){
  require(value instanceof Uint8Array||Array.isArray(value),'Missing bytes: '+label);
  require(value.length>0&&Array.from(value).every(byte=>Number.isInteger(byte)&&byte>=0&&byte<=255),'Invalid bytes: '+label);
  return value;
}
function equal(a,b){return a.length===b.length&&a.every((byte,index)=>byte===b[index]);}
function replay(value,label){
  value=bytes(value,label);require(value.length>=40,'Missing Replay header: '+label);
  const data=value instanceof Uint8Array?value:new Uint8Array(value),view=new DataView(data.buffer,data.byteOffset,data.byteLength);
  const word=at=>view.getUint32(at,true),players=data[20],recordedSeat=data[21],frames=word(32),prefix=40+word(24)+8*word(28);
  require(new TextDecoder().decode(data.subarray(0,8))==='EAGLRPY1'&&word(8)===1&&word(12)===15,
    'Invalid Replay format: '+label);
  require(word(16)>0&&recordedSeat<players&&!data[22]&&!data[23],'Invalid Replay session header: '+label);
  require([2,3].includes(players)&&prefix+frames*players*12===data.length,'Invalid Replay extent: '+label);
  return {data,players,recordedSeat,frames,prefix};
}
function fileEvidence(file,name,kind){
  require(file?.name===name&&file.kind===kind,'Missing persistence file: '+name);
  const memory=bytes(file.memory,name+' MEMFS'),indexedDb=bytes(file.indexedDb,name+' IDB');
  require(file.nativeValid===true,'Native IDB validation failed: '+name);
  if(file.memoryNativeValid!==undefined)require(file.memoryNativeValid===true,'Native MEMFS validation failed: '+name);
  const identical=equal(memory,indexedDb);
  require(file.equal===identical,'Recorded equality flag disagrees with bytes: '+name);
  // Old observations validated only IDB. Identical MEMFS bytes share that
  // validation; distinct MEMFS bytes need their own observed native result.
  if(!identical)require(file.memoryNativeValid===true,'Changed MEMFS bytes lack native validation: '+name);
  return {name,memory,indexedDb,identical,nativeValid:true,
    memoryNativeValid:file.memoryNativeValid===true||identical};
}

export function classifyPersistenceAttempt({sync,copy,persistence,saved,reference,proofEnd,baselineReplay}){
  require(reference&&integer(proofEnd)&&proofEnd>0&&proofEnd<0xffffffff,'Invalid fixed persistence proof horizon');
  require(sync&&finite(sync.timeOrigin)&&finite(sync.start)&&finite(sync.ackAt)&&before(sync.start,sync.ackAt)
    &&typeof sync.request==='string'&&sync.request.length>0,'Missing completed Host persistence sync');
  require(!sync.errors?.length,'Host persistence sync failed');
  require(copy?.metadata?.nativeValid===true,'Native evidence-copy validation failed');
  require(saved?.saveApi===true&&finite(saved.timeOrigin)&&Array.isArray(saved.events),'Missing native save observer');
  require(persistence&&!persistence.error,'Persistence read failed: '+(persistence?.error||'missing observation'));
  require(copy.metadata.timeOrigin===saved.timeOrigin,'Replay copy belongs to a different Runtime clock');
  if(persistence.timeOrigin!==undefined)require(persistence.timeOrigin===saved.timeOrigin,'Persistence belongs to a different Runtime clock');
  if(sync.label!==undefined&&copy.metadata.label!==undefined)require(sync.label===copy.metadata.label,'Host sync and Replay copy labels disagree');
  const originDifference=sync.timeOrigin-saved.timeOrigin;
  const hostStart=sync.start+originDifference,hostAck=sync.ackAt+originDifference;
  const copiedAt=copy.metadata.copiedAt,capturedAt=persistence.capturedAt,readAt=persistence.readAt;
  require([hostStart,hostAck,copiedAt,capturedAt,readAt].every(finite)
    &&before(hostAck,copiedAt)&&before(copiedAt,capturedAt)&&before(capturedAt,readAt),'Persistence observation timestamps regressed');
  let prior={...reference};
  for(const [label,snapshot] of [['Host sync',sync.snapshot],['Replay copy',copy.metadata.snapshot],['persistence end',saved.status]]){
    const state=assertProofState(snapshot,prior);
    require(integer(state.committed)&&state.committed<0xffffffff&&state.committed>=proofEnd-1,
      label+' does not cover the fixed committed proof horizon');
    if(prior.committed!==undefined&&prior.committed!==0xffffffff)require(state.committed>=prior.committed,label+' committed frontier regressed');
    prior={...reference,capture:state.capture,committed:state.committed};
  }
  require(Array.isArray(persistence.files)&&persistence.files.length===2,'Missing score/config persistence files');
  const files=[fileEvidence(persistence.files.find(file=>file.name==='scoreth15.dat'),'scoreth15.dat',0),
    fileEvidence(persistence.files.find(file=>file.name==='th15.cfg'),'th15.cfg',2),
    fileEvidence(persistence.replay,'replay/th15mp_01.rpy',1)];
  const copied=bytes(copy.buffer,'evidence Replay copy'),stored=files[2];
  const parsed=[replay(copied,'evidence copy'),replay(stored.memory,'Replay MEMFS'),replay(stored.indexedDb,'Replay IDB')];
  for(const value of parsed)require(value.players===reference.players&&value.frames>=proofEnd,
    'Replay does not cover the fixed all-seat proof horizon');
  for(const value of parsed)require(value.recordedSeat===reference.seat
    &&equal(value.data.subarray(0,24),parsed[0].data.subarray(0,24)),'Replay session header or recorded seat changed');
  const commonFrames=Math.min(...parsed.map(value=>value.frames)),commonBytes=commonFrames*reference.players*12;
  const commonInput=parsed[0].data.subarray(parsed[0].prefix,parsed[0].prefix+commonBytes);
  for(const value of parsed.slice(1))require(equal(value.data.subarray(value.prefix,value.prefix+commonBytes),commonInput),
    'Replay common input prefix changed');
  let baseline=null;
  if(baselineReplay!==undefined){
    baseline=replay(baselineReplay,'original neutral baseline');
    require(baseline.players===reference.players&&baseline.recordedSeat===reference.seat
      &&equal(baseline.data.subarray(0,24),parsed[0].data.subarray(0,24)),'Original baseline session header changed');
    const length=baseline.frames*baseline.players*12,expected=baseline.data.subarray(baseline.prefix,baseline.prefix+length);
    for(const value of parsed)require(value.frames>=baseline.frames
      &&equal(value.data.subarray(value.prefix,value.prefix+length),expected),'Original baseline input prefix changed');
  }
  const copyCount=copy.metadata.saveEventCount,begin=persistence.saveEventCountBegin,end=persistence.saveEventCountEnd;
  require([copyCount,begin,end].every(integer)&&copyCount<=begin&&begin<=end&&end<=saved.events.length,'Persistence event counts regressed or exceed the observer');
  // Everything after this captured event boundary belongs to a future
  // observation. In particular, a later autosave cannot invalidate this one.
  const events=saved.events.slice(0,end).map((event,index)=>({...event,eventIndex:index}));
  const requests=new Map(),filesystem=new Map(),waiting=[],transactions=[];
  let lastAt=-Infinity,openOutgoing=null;
  for(const event of events){
    const {kind,at,eventIndex:index}=event;
    require(finite(at)&&before(lastAt,at)&&before(at,readAt),'Save event timestamps regressed');lastAt=at;
    require(index<copyCount?before(at,copiedAt):before(copiedAt,at),'Replay-copy event count contradicts timestamps');
    require(index<begin?before(at,capturedAt):before(capturedAt,at),'MEMFS event count contradicts timestamps');
    require(!event.error&&!event.nativeError,'Observed native or filesystem save error');
    if(kind.startsWith('native-')){
      require(finite(event.entered)&&before(event.entered,at),'Missing native call entry timestamp');
      require(token(event.token),'Invalid native save token');
    }
    if(kind==='native-request'){
      require(!requests.has(event.token)&&event.result===event.token,'Duplicate or inconsistent native save request');
      requests.set(event.token,{request:event,complete:null});
    }else if(kind==='native-poll'){
      const request=requests.get(event.token);
      require(request&&(event.result===0||event.result===1),'Native save poll failed or has no request');
      require(!request.complete,'Native save polled after recorded completion');
      if(event.result===1){request.complete=event;waiting.push(request);}
    }else if(kind==='native-cancel'){
      throw Error('Observed native save cancellation');
    }else if(kind==='fs-sync-start'){
      require(token(event.id)&&typeof event.populate==='boolean'&&!filesystem.has(event.id),'Invalid filesystem transaction start');
      const fs={start:event,request:null,complete:null};filesystem.set(event.id,fs);
      if(!event.populate){
        require(openOutgoing===null,'Overlapping outgoing filesystem transactions');openOutgoing=event.id;
        require(waiting.length<=1,'Native completion cannot be correlated uniquely with filesystem start');
        fs.request=waiting.shift()||null;
      }
    }else if(kind==='fs-sync-complete'){
      const fs=filesystem.get(event.id);
      require(fs&&!fs.complete&&event.populate===fs.start.populate,'Filesystem completion has no matching start');fs.complete=event;
      if(!event.populate){
        require(openOutgoing===event.id,'Outgoing filesystem completion order changed');openOutgoing=null;
        if(fs.request){const {request,complete}=fs.request;
          transactions.push({token:request.token,fsId:event.id,requestEnteredAt:request.entered,requestObservedAt:request.at,
            completionEnteredAt:complete.entered,completionObservedAt:complete.at,filesystemStartedAt:fs.start.at,filesystemCompletedAt:event.at,
            requestEventIndex:request.eventIndex,completionEventIndex:complete.eventIndex,
            filesystemStartEventIndex:fs.start.eventIndex,filesystemCompletionEventIndex:event.eventIndex});
        }
      }
    }else throw Error('Unknown save event kind: '+kind);
  }
  const candidates=transactions.filter(transaction=>before(transaction.requestEnteredAt,hostAck)
    &&before(hostStart,transaction.filesystemCompletedAt)&&before(transaction.filesystemCompletedAt,hostAck))
    .map(transaction=>({...transaction,hostRequest:sync.request,hostStartAt:hostStart,hostAckAt:hostAck,
      joinedInFlight:transaction.requestEnteredAt<hostStart-epsilon}));
  require(candidates.length>0,'No completed native request and filesystem transaction matches the Host sync interval');
  const first=candidates[0],transaction=candidates.length===1?first:null;
  require(candidates.every(value=>before(value.filesystemCompletedAt,copiedAt)),'Replay copy precedes the correlated filesystem completion');
  let lastCapture=null,lastObserved=-Infinity;
  for(const event of events.slice(first.requestEventIndex)){
    if(event.net){
      const net=event.net;
      require(net.length>=24&&net[1]&&net[2]&&!net[13]&&!net[23],'Save event left the active session');
      for(const [index,field] of [[5,'players'],[6,'seat'],[7,'delay'],[12,'generation']])
        require(net[index]===reference[field],'Save event changed '+field);
    }
    if(event.rollback&&finite(event.rollbackObservedAt)){
      require(before(lastObserved,event.rollbackObservedAt),'Observed rollback snapshot timestamp regressed');lastObserved=event.rollbackObservedAt;
      const capture=event.rollback[3];require(integer(capture)&&capture<0xffffffff&&(lastCapture===null||capture>=lastCapture),'Observed capture frontier regressed');lastCapture=capture;
    }
    require(!(event.kind==='fs-sync-start'&&event.populate&&event.eventIndex>first.filesystemCompletionEventIndex),
      'Incoming filesystem synchronization during persistence evidence');
  }
  const supersedingEvents=events.filter(event=>event.eventIndex>first.filesystemCompletionEventIndex
    &&(event.kind==='native-request'||event.kind==='fs-sync-start'&&!event.populate));
  const comparisons=files.map(file=>({name:file.name,memoryBytes:file.memory.length,indexedDbBytes:file.indexedDb.length,
    memoryEqualsIndexedDb:file.identical,nativeValid:file.nativeValid,memoryNativeValid:file.memoryNativeValid}));
  const copyEqualsMemory=equal(copied,stored.memory),copyEqualsIndexedDb=equal(copied,stored.indexedDb);
  if(!supersedingEvents.length){
    require(candidates.length===1,'Ambiguous Host sync without an additional observed writer');
    require(files.every(file=>file.identical)&&copyEqualsMemory&&copyEqualsIndexedDb,
      'Persistence bytes differ without a superseding writer');
  }
  return {state:supersedingEvents.length?'superseded':'stable',transaction,candidateTransactions:candidates,
    reason:candidates.length>1?'multiple completed native transactions overlap the Host interval':
      supersedingEvents.length?'a new native request or outgoing filesystem write superseded the observation':'one completed native transaction and a stable byte-identical snapshot',
    supersedingEvents,comparisons,copyEqualsMemory,copyEqualsIndexedDb,proofEnd,
    commonInputFrames:commonFrames,baselineInputFrames:baseline?.frames??null,eventCount:end,ignoredEventsAfterRead:saved.events.length-end,
    observationWindow:{filesystemCompletedAt:first.filesystemCompletedAt,copiedAt,capturedAt,readAt}};
}

export async function collectStablePersistence({deadline,maxAttempts=3,attempt,retain,
  clock=()=>performance.now(),wait=awaitBeforeDeadline}){
  require(finite(deadline)&&Number.isInteger(maxAttempts)&&maxAttempts>=1&&maxAttempts<=3,'Invalid bounded persistence collection');
  require(typeof attempt==='function'&&typeof retain==='function'&&typeof clock==='function'&&typeof wait==='function','Missing persistence collection callbacks');
  const attempts=[];let fixed=null;
  for(let index=1;index<=maxAttempts;++index){
    const observation={};let retained=false;
    try{
      assertBeforeDeadline(clock(),deadline,'persistence attempt '+index);
      await wait(attempt(index,deadline,observation),deadline,'persistence attempt '+index);
      assertBeforeDeadline(clock(),deadline,'persistence attempt '+index);
      const contract=[observation.proofEnd,...['players','seat','delay','generation','capture','committed'].map(field=>observation.reference?.[field])];
      if(fixed){
        require(JSON.stringify(contract)===fixed.contract,'Persistence retry changed the original proof contract');
        require((observation.baselineReplay===undefined)===(fixed.baseline===null)
          &&(fixed.baseline===null||equal(bytes(observation.baselineReplay,'retry baseline'),fixed.baseline)),
          'Persistence retry changed the original baseline');
      }else fixed={contract:JSON.stringify(contract),baseline:observation.baselineReplay===undefined?null:new Uint8Array(bytes(observation.baselineReplay,'baseline'))};
      const classification=classifyPersistenceAttempt(observation);
      assertBeforeDeadline(clock(),deadline,'persistence classification '+index);
      const record={attempt:index,...classification};attempts.push(record);retained=true;
      // Retention also runs after failures/deadlines: partial raw evidence must
      // not disappear merely because the observation budget has expired.
      await retain(record,observation);
      if(classification.state==='stable')return {observation,classification,selectedAttempt:index,attempts};
    }catch(error){
      if(!retained){const record={attempt:index,state:'error',error:String(error)};attempts.push(record);retained=true;await retain(record,observation);}
      throw error;
    }
  }
  throw Error('No stable persistence observation after '+maxAttempts+' attempts');
}
