// Test-only instrumentation and fault injection for an owned loopback relay.
// No payload bytes, request URLs, SDP, addresses or membership credentials are retained.
import {performance} from 'node:perf_hooks';
import {createRelayStartupObserver} from './relay-startup-probe.mjs';

export const RELAY_PACKET_COLUMNS=['id','sessionLo','sessionHi','sender','players','sequence','ackSequence','latestFrame','ackFrame','firstInputFrame','senderFrame','frameAdvantage','inputCount','reserved','payloadBytes','fnv1a32','destination'];
export const RELAY_EVENT_COLUMNS=['id','phase','packetId','socketId','receiveEventId','scheduledId','enteredAt','exitedAt','plannedDelayMs','dueAt','burst','bufferedBefore','bufferedAfter','pendingMessages','pendingBytes','threw','reason'];
const DEFAULT_LIMITS={packets:32768,events:131072,connections:64,timing:16384,errors:64,pendingMessages:1024,pendingBytes:262144};
const PHASES=['receive','scheduled','send','cancelled'];
const REASONS=['none','closed','budget','async-send-error'];

export function parseRelayInput(data,destinationHint=-1){
 let bytes;
 if(data instanceof ArrayBuffer)bytes=new Uint8Array(data);
 else if(ArrayBuffer.isView(data))bytes=new Uint8Array(data.buffer,data.byteOffset,data.byteLength);
 else return {kind:typeof data==='string'?'text':'unsupported'};
 let target=-1;
 if(bytes.length>=2&&bytes[0]===0xe7){target=bytes[1];bytes=bytes.subarray(2);}
 if(bytes.length<6||bytes[0]!==69||bytes[1]!==57||bytes[2]!==78||bytes[3]!==80||bytes[5]!==1)return {kind:'other-binary'};
 if(bytes[4]!==4||bytes.length<44||bytes[42]>32||bytes.length!==44+bytes[42]*12)return {kind:'malformed'};
 const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength),sender=bytes[6],players=bytes[7];
 let hash=2166136261;
 for(const byte of bytes)hash=Math.imul(hash^byte,16777619)>>>0;
 const destination=destinationHint>=0?destinationHint:target>=0?target:players===2&&sender<2?1-sender:-1;
 return {kind:'input',target,fields:[view.getUint32(8,true),view.getUint32(12,true),sender,players,
  view.getUint32(16,true),view.getUint32(20,true),view.getUint32(24,true),view.getUint32(28,true),
  view.getUint32(32,true),view.getUint32(36,true),view.getInt16(40,true),bytes[42],bytes[43],bytes.length,hash,destination]};
}

export function installRelayTransportProbe(WebSocket,configuration={},dependencies={}){
 if(typeof WebSocket?.prototype?.send!=='function'||typeof WebSocket?.Server?.prototype?.emit!=='function')throw Error('Expected the relay ws implementation');
 const config={inject:false,delay:0,jitter:0,burstAt:0,burstFor:0,burstDelay:0,...configuration};
 for(const key of ['delay','jitter','burstAt','burstFor','burstDelay'])if(!Number.isFinite(config[key])||config[key]<0)throw Error('Invalid relay injection '+key);
 if(typeof config.inject!=='boolean')throw Error('Invalid relay injection switch');
 const limits={...DEFAULT_LIMITS,...dependencies.limits};
 for(const [key,value] of Object.entries(limits))if(!Number.isInteger(value)||value<1)throw Error('Invalid limit '+key);
 const now=dependencies.now||(()=>performance.now()),epoch=dependencies.epoch||(()=>Date.now());
 const timeOrigin=dependencies.timeOrigin??performance.timeOrigin;
 const startup=config.startupTrace?createRelayStartupObserver({rtcTimeoutMs:config.rtcTimeoutMs??4500},
  {now,epoch,timeOrigin,startupLimit:dependencies.startupLimit}):null;
 const schedule=dependencies.schedule||setTimeout,cancel=dependencies.cancel||clearTimeout;
 const repeat=dependencies.repeat||setInterval,cancelRepeat=dependencies.cancelRepeat||clearInterval;
 const pair=()=>{const before=now(),epochMs=epoch(),after=now();return {timeOrigin,before,epochMs,after};};
 const started=pair(),packets=[],events=[],connections=[],timing=[],errors=[];
 const packetIds=new Map(),sockets=new WeakMap(),live=new Set(),jobs=new Map(),seeds=new Map();
 const overflow={packets:0,events:0,connections:0,timing:0,errors:0,firstAt:null};
 const counts={receive:0,scheduled:0,sent:0,cancelled:0,burstScheduled:0,otherBinaryReceive:0,otherBinarySend:0,
  unsupported:0,malformed:0,syncThrows:0,asyncThrows:0,budgetFailures:0,forwardWithoutReceive:0,timingTicks:0};
 const originalServerEmit=WebSocket.Server.prototype.emit,originalEmit=WebSocket.prototype.emit,originalSend=WebSocket.prototype.send;
 let running=true,stopped=null,frozen=null,currentReceive=null,eventSerial=0,jobSerial=0,anchor=null;
 const excess=key=>{++overflow[key];overflow.firstAt??=now();};
 function note(code){if(!running)return;if(errors.length>=limits.errors){excess('errors');return;}errors.push({at:now(),code});}
 function safeBuffer(socket){try{return Number.isFinite(socket.bufferedAmount)?socket.bufferedAmount:-1;}catch{return -1;}}
 function parsed(data,destination=-1){
  let p;try{p=parseRelayInput(data,destination);}catch{if(running){++counts.unsupported;note('packet-parser-error');}return null;}
  if(p.kind==='malformed'){if(running){++counts.malformed;note('unsupported-or-malformed-input');}return null;}
  if(p.kind==='unsupported'){if(running)++counts.unsupported;return null;}
  if(p.kind!=='input')return null;
  if(p.fields[3]!==2||p.fields[2]>=p.fields[3]||p.fields[15]<0||p.fields[15]>=p.fields[3])note('unexpected-input-identity');
  return p;
 }
 function packetId(p){
  if(!running)return -1;
  const key=p.fields.join(':'),prior=packetIds.get(key);if(prior!==undefined)return prior;
  if(packets.length>=limits.packets){excess('packets');return -1;}
  const id=packets.length;packets.push([id,...p.fields]);packetIds.set(key,id);return id;
 }
 function record(phase,p,slot,fields={}){
  if(!running)return null;
  const id=eventSerial++,pid=packetId(p);
  if(events.length>=limits.events){excess('events');return null;}if(pid<0)return null;
  const at=fields.at??now();
  const row=[id,phase,pid,slot.id,fields.receiveId??-1,fields.jobId??-1,at,fields.exit??at,
   fields.delay??0,fields.due??at,+!!fields.burst,fields.bufferedBefore??safeBuffer(slot.socket),
   fields.bufferedAfter??safeBuffer(slot.socket),slot.pending.size,slot.bytes,+!!fields.threw,fields.reason??0];
  events.push(row);return row;
 }
 function closePending(slot){
  for(const jobId of [...slot.pending]){
   const job=jobs.get(jobId);if(!job)continue;
   cancel(job.timer);jobs.delete(jobId);slot.pending.delete(jobId);slot.bytes-=job.size;
   if(running){++counts.cancelled;record(3,job.parsed,slot,{receiveId:job.receiveId,jobId,delay:job.delay,due:job.due,burst:job.burst,reason:1});}
   job.args=null;
  }
  live.delete(slot);if(running)slot.info.closedAt=now();
 }
 function attach(socket,request){
  if(sockets.has(socket))return;
  if(connections.length>=limits.connections){excess('connections');return;}
  let url;try{url=new URL(request?.url||'','http://127.0.0.1');}catch{note('connection-url-invalid');return;}
  const seat=Number(url.searchParams.get('player')),hasSeat=url.searchParams.has('player');
  const kind=url.searchParams.has('lobby')?'lobby':url.searchParams.has('signal')?'signal':url.searchParams.has('spectator')?'spectator':hasSeat?'player':'other';
  const info={id:connections.length,kind,player:hasSeat&&Number.isInteger(seat)&&seat>=0&&seat<3?seat:null,openedAt:now(),closedAt:null};
  connections.push(info);
  const slot={id:info.id,socket,info,pending:new Set(),bytes:0};sockets.set(socket,slot);live.add(slot);
  startup?.connection(info);
  socket.once('close',()=>closePending(slot));
 }
 function serverEmit(event,...args){
  if(event==='connection')try{attach(args[0],args[1]);}catch{note('connection-hook-error');}
  return Reflect.apply(originalServerEmit,this,[event,...args]);
 }
 function emit(event,...args){
  const slot=sockets.get(this);
  if(startup&&slot&&(event==='close'||event==='error'||event==='message'&&!args[1]))
   return startup.emit(slot.info,event,args,()=>Reflect.apply(originalEmit,this,[event,...args]));
  if(!slot||slot.info.kind!=='player'||event!=='message'||!args[1])return Reflect.apply(originalEmit,this,[event,...args]);
  const p=parsed(args[0]);if(!p){if(running)++counts.otherBinaryReceive;return Reflect.apply(originalEmit,this,[event,...args]);}
  if(!anchor&&p.fields[11]>0)anchor={at:now(),sender:p.fields[2],sessionLo:p.fields[0],sessionHi:p.fields[1],sequence:p.fields[4],firstInputFrame:p.fields[8]};
  if(slot.info.player!==p.fields[2])note('receive-sender-does-not-match-socket');
  if(running)++counts.receive;
  const previous=currentReceive,row=record(0,p,slot,{at:now()});
  currentReceive={id:row?.[0]??-1,parsed:p};
  try{return Reflect.apply(originalEmit,this,[event,...args]);}
  catch(error){if(row)row[15]=1;throw error;}
  finally{if(row)row[7]=now();currentReceive=previous;}
 }
 function callSend(slot,p,args,job){
  if(running)++counts.sent;
  const before=safeBuffer(slot.socket),row=record(2,p,slot,{at:now(),receiveId:job.receiveId,jobId:job.id,delay:job.delay,due:job.due,burst:job.burst,bufferedBefore:before});
  try{return Reflect.apply(originalSend,slot.socket,args);}
  catch(error){if(running)++counts.syncThrows;if(row)row[15]=1;throw error;}
  finally{if(row){row[7]=now();row[12]=safeBuffer(slot.socket);}}
 }
 function send(...args){
  const slot=sockets.get(this);
  if(startup&&slot&&typeof args[0]==='string')return startup.send(slot.info,args,()=>Reflect.apply(originalSend,this,args));
  if(!slot||slot.info.kind!=='player')return Reflect.apply(originalSend,this,args);
  const p=parsed(args[0],slot.info.player??-1);
  if(!p){if(running&&typeof args[0]!=='string')++counts.otherBinarySend;return Reflect.apply(originalSend,this,args);}
  const at=now(),sender=p.fields[2],receiveId=currentReceive?.id??-1;
  if(running&&receiveId<0)++counts.forwardWithoutReceive;
  let delay=0,burst=false;
  if(config.inject){
   const seed=(Math.imul(seeds.get(sender)??(0x915ab+sender),1664525)+1013904223)>>>0;seeds.set(sender,seed);
   const age=anchor?at-anchor.at:-1;
   burst=!!anchor&&config.burstFor>0&&age>=config.burstAt&&age<config.burstAt+config.burstFor;
   delay=Math.max(0,config.delay+(seed/0x100000000*2-1)*config.jitter)+(burst?config.burstDelay:0);
  }
  const job={id:jobSerial++,receiveId,parsed:p,delay,due:at+delay,burst,args:null,timer:null,size:0};
  if(running){++counts.scheduled;if(burst)++counts.burstScheduled;}
  record(1,p,slot,{at,receiveId,jobId:job.id,delay,due:job.due,burst});
  if(delay===0)return callSend(slot,p,args,job);
  const bytes=ArrayBuffer.isView(args[0])?new Uint8Array(args[0].buffer,args[0].byteOffset,args[0].byteLength):new Uint8Array(args[0]);
  job.size=bytes.byteLength+16;
  if(slot.pending.size>=limits.pendingMessages||Math.max(0,safeBuffer(this))+slot.bytes+job.size>limits.pendingBytes){
   if(running){++counts.budgetFailures;++counts.cancelled;note('relay-injection-budget');}
   record(3,p,slot,{receiveId,jobId:job.id,delay,due:job.due,burst,reason:2});
   // Same bounded fault-injection policy as the source relay; owned test socket only.
   this.terminate();return;
  }
  job.args=[Buffer.from(bytes),...args.slice(1)];slot.bytes+=job.size;slot.pending.add(job.id);jobs.set(job.id,job);
  job.timer=schedule(()=>{
   if(!jobs.delete(job.id))return;
   slot.pending.delete(job.id);slot.bytes-=job.size;
   const retained=job.args;job.args=null;
   if(this.readyState!==WebSocket.OPEN){if(running)++counts.cancelled;record(3,p,slot,{receiveId,jobId:job.id,delay,due:job.due,burst,reason:1});return;}
   try{callSend(slot,p,retained,job);}
   catch{if(running){++counts.asyncThrows;note('async-original-send-threw');}record(3,p,slot,{receiveId,jobId:job.id,delay,due:job.due,burst,reason:3,threw:true});}
  },delay);
  return;
 }
 WebSocket.Server.prototype.emit=serverEmit;WebSocket.prototype.emit=emit;WebSocket.prototype.send=send;
 let previous=now(),due=previous+100;
 const timingTimer=repeat(()=>{
  if(!running)return;
  const at=now();++counts.timingTicks;
  if(timing.length<limits.timing)timing.push([at,due,at-due,at-previous]);else excess('timing');
  previous=at;due=at+100;
 },100);timingTimer?.unref?.();
 function snapshot({stop=false}={}){
  if(frozen)return frozen;
  if(stop){stopped=pair();running=false;cancelRepeat(timingTimer);}
  const snapshotClock=pair(),hookIdentity={serverEmitCurrent:WebSocket.Server.prototype.emit===serverEmit,emitCurrent:WebSocket.prototype.emit===emit,sendCurrent:WebSocket.prototype.send===send};
  const startupSnapshot=startup?.snapshot({stop});
  const value={version:1,scope:'Owned loopback shared relay; JS receive/forward/send API boundaries, not OS or peer delivery.',
   injection:{...config,packetScope:'E9NP version 4 kind 1; ACK-only and redundant input included; non-input control/hash/signaling untouched; relay has no separate RTC repair lane.',
    jitterAlgorithm:'Per-sender LCG 1664525*x+1013904223, seed 0x915ab+sender; same formula as legacy client impairment. Relay forwarding order can differ.',
    burstClock:'First non-empty E9NP input receive in this relay process; distinct from browser active time.',anchor:anchor?{...anchor}:null},
   clock:{unit:'milliseconds',started,stopped,snapshot:snapshotClock},
   limits:{...limits},columns:{packet:RELAY_PACKET_COLUMNS,event:RELAY_EVENT_COLUMNS,phases:PHASES,reasons:REASONS,timing:['at','dueAt','lateMs','gapMs']},
   valid:errors.length===0&&Object.entries(overflow).every(([key,v])=>key==='firstAt'||v===0)&&Object.values(hookIdentity).every(Boolean)&&startupSnapshot?.valid!==false,
   coverage:{exercised:counts.receive>0&&counts.sent>0,connectionHookExercised:connections.length>0,exactNativeAcceptance:false,osArrivalIncluded:false,relayTraceIncluded:true},
   identity:hookIdentity,counts:{...counts},overflow:{...overflow},errors:errors.map(x=>({...x})),connections:connections.map(x=>({...x})),
   packets:packets.map(x=>x.slice()),events:events.map(x=>x.slice()),timing:timing.map(x=>x.slice()),
   pending:[...jobs.values()].map(j=>({scheduledId:j.id,packetFields:j.parsed.fields.slice(),dueAt:j.due,plannedDelayMs:j.delay,burst:j.burst})),
   ...(startupSnapshot?{startup:startupSnapshot}:{})};
  if(stop)frozen=value;
  return value;
 }
 function dispose(){
  snapshot({stop:true});for(const slot of [...live])closePending(slot);
  if(WebSocket.Server.prototype.emit===serverEmit)WebSocket.Server.prototype.emit=originalServerEmit;
  if(WebSocket.prototype.emit===emit)WebSocket.prototype.emit=originalEmit;
  if(WebSocket.prototype.send===send)WebSocket.prototype.send=originalSend;
 }
 return {snapshot,dispose};
}

