// Read-only metadata at the owned relay's existing ws boundaries.
// No timer, request, signaling message, route choice or connection is created.
export function relayRtcTimeoutConfiguration(environment){
 const names=['EAGLER_NETPLAY_RTC_TIMEOUT_MS','TH07_RTC_TIMEOUT_MS','TH07_DIRECT_TIMEOUT_MS'];
 const configured=names.filter(name=>environment[name]!=null).map(name=>[name,String(environment[name])]);
 if(new Set(configured.map(([,value])=>value)).size>1)throw Error('Conflicting RTC timeout aliases');
 return {rtcTimeoutMs:Math.max(1000,Number.parseInt(configured[0]?.[1]??'4500',10)||4500),configuredNames:configured.map(([name])=>name)};
}
export function createRelayStartupObserver(configuration,dependencies){
 const {now,epoch,timeOrigin}=dependencies,limit=dependencies.startupLimit??4096;
 if(!Number.isInteger(limit)||limit<1)throw Error('Invalid relay startup event limit');
 const pair=()=>{const before=now(),epochMs=epoch(),after=now();return {timeOrigin,before,epochMs,after};};
 const started=pair(),events=[],counts={connections:0,receive:0,send:0,close:0,error:0};
 let running=true,overflow=0,observerErrors=0,serial=0,currentReceive=null,frozen=null;
 const errorNames=['Error','TypeError','RangeError','ReferenceError','SyntaxError','AbortError','DataError','InvalidAccessError','InvalidModificationError','InvalidStateError','NetworkError','NotAllowedError','NotFoundError','NotReadableError','NotSupportedError','OperationError','QuotaExceededError','RTCError','SecurityError','TimeoutError','UnknownError'];
 const errorName=error=>{let name;try{name=typeof error==='string'?error.split(':',1)[0]:error?.name;}catch{}return errorNames.includes(name)?name:'UnknownError';};
 const selected=(value,allowed)=>allowed.includes(value)?value:null;
 const peer=value=>Number.isInteger(value)&&value>=0&&value<3?value:null;
 function metadata(data){
  if(typeof data!=='string'&&!(data instanceof Uint8Array))return null;
  if(typeof data!=='string'){if(data.byteLength>65536)return null;data=Buffer.from(data.buffer,data.byteOffset,data.byteLength).toString('utf8');}
  if(data.length>65536)return null;
  let m;try{m=JSON.parse(data);}catch{return null;}
  if(!m||!['signal','rtc-ready','rtc-failed','route','peers','peer-join','peer-disconnected','ice-restart-request','error'].includes(m.type))return null;
  return {type:m.type,descriptionType:selected(m.description?.type,['offer','answer','pranswer','rollback']),
   candidatePresent:!!m.candidate,from:peer(m.from),to:peer(m.to),player:peer(m.player),
   peers:Array.isArray(m.peers)?m.peers.slice(0,3).map(peer):null,mode:selected(m.mode??m.route,['rtc','relay']),
   errorName:m.error==null?null:errorName(m.error)};
 }
 function safe(operation){try{return operation();}catch{if(running)++observerErrors;return null;}}
 function record(info,phase,fields={}){
  if(!running)return null;
  if(events.length>=limit){++overflow;return null;}
  const row={id:serial++,phase,socketId:info.id,kind:info.kind,socketPlayer:info.player,
   enteredAt:now(),exitedAt:null,causeReceiveId:currentReceive,...fields};events.push(row);return row;
 }
 function call(info,phase,fields,invoke){
  const row=safe(()=>{if(running)++counts[phase];return record(info,phase,fields);});
  const previous=currentReceive;if(phase==='receive'&&row)currentReceive=row.id;
  try{return invoke();}
  catch(error){if(row)safe(()=>{row.threw=true;row.thrownErrorName=errorName(error);});throw error;}
  finally{if(row)safe(()=>{row.exitedAt=now();});currentReceive=previous;}
 }
 return {
  connection(info){safe(()=>{if(running)++counts.connections;const row=record(info,'connection');if(row)row.exitedAt=now();});},
  emit(info,event,args,invoke){
   if(!running)return invoke();
   if(event==='message'&&!args[1]&&['signal','player'].includes(info.kind)){
    const fields=safe(()=>metadata(args[0]));return fields?call(info,'receive',fields,invoke):invoke();
   }
   if(event==='close')return call(info,'close',{code:Number.isInteger(args[0])?args[0]:null},invoke);
   if(event==='error')return call(info,'error',{errorName:errorName(args[0])},invoke);
   return invoke();
  },
  send(info,args,invoke){
   if(!running||!['signal','player'].includes(info.kind)||typeof args[0]!=='string')return invoke();
   const fields=safe(()=>metadata(args[0]));return fields?call(info,'send',fields,invoke):invoke();
  },
  snapshot({stop=false}={}){
   if(frozen)return frozen;
   const value={version:1,scope:'Existing relay JS text receive/send and connection/close boundaries. Connection observation precedes canonical admission; no SDP, candidate text, URLs, addresses, credentials or error messages retained. Route timer firing is not instrumented; deadline attribution requires explicit inference.',
    configuration:{...configuration},clock:{unit:'milliseconds',started,snapshot:pair()},limit,
    valid:overflow===0&&observerErrors===0,coverage:{exercised:counts.receive+counts.send>0,exactTimerFire:false},
    counts:{...counts},overflow,observerErrors,events:events.map(e=>({...e}))};
   if(stop){running=false;value.clock.stopped=pair();frozen=value;}return value;
  }
 };
}
