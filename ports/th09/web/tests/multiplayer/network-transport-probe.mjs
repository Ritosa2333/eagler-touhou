// Synthetic relay diagnostics only. This function is self-contained for addInitScript.
// No timers, network traffic, key changes, native pump calls, or payload retention.
export function installNetworkTransportProbe(options) {
 'use strict';
 options=options??{};
 if(options.enabled!==true)return;
 const root=globalThis;
 if(root.networkTransportProbe?.schema==='th09/network-transport-probe/1')return;
 const now=()=>performance.now();
 const clock=()=>{const before=now(),epochMs=Date.now(),after=now();return {timeOrigin:performance.timeOrigin,before,epochMs,after};};
 const bounded=(name,value,max)=>Number.isInteger(options[name])&&options[name]>0?Math.min(options[name],max):value;
 const limits={packets:bounded('maxPackets',16384,65536),events:bounded('maxEvents',65536,262144),
  pacing:bounded('maxPacing',8192,32768),states:16,sockets:32,imports:16,errors:64};
 const packets=[],events=[],pacing=[],states=[],sockets=[],errors=[];
 const overflow={packets:0,events:0,pacing:0,states:0,sockets:0,imports:0,errors:0,firstAt:null};
 const counts={intentCalls:0,wsSendCalls:0,messageCalls:0,pollCopyCalls:0,pollCopyPositive:0,
  inputIntent:0,inputWsSend:0,inputMessage:0,inputPollCopy:0,inputDequeues:0,
  nonInputBinary:0,nonBinary:0,unsupportedBytes:0,malformedInput:0,
  connectCalls:0,importTables:0,pumpCalls:0,pumpThrows:0,pacingReads:0,unsafePacingRequests:0};
 const packetIds=new Map(),stateIds=new WeakMap(),socketIds=new WeakMap(),tableSeen=new WeakSet();
 const importHooks=[],messageHooks=[];
 const started=clock();
 let stopped=null,stopReason=null,running=true,eventSerial=0,dequeueSerial=0,importDepth=0;
 let attachedCore=null,pumpHook=null,expectedSend=null,wsHook=null,instantiateHook=null,streamingHook=null;
 let lastPacingAt=-Infinity,lastPacingDequeue=-1,pacingAvailable=null;
 const phaseNames=['intent','ws-send','message','poll-copy'];
 const eventColumns=['id','phase','packetId','socketId','stateId','enteredAt','exitedAt',
  'queueBefore','queueAfter','resultBytes','bufferedBefore','bufferedAfter','threw',
  'envelopeTarget','routeBefore','routeAfter','closedBefore','closedAfter'];
 const packetColumns=['id','sessionLo','sessionHi','sender','players','sequence','ackSequence',
  'latestFrame','ackFrame','firstInputFrame','senderFrame','frameAdvantage','inputCount',
  'reserved','payloadBytes','fnv1a32','destination'];
 const pacingColumns=['at','source','sampleIndex','dequeueSerial','version','outstandingFrames',
  'networkIntervalScaleMilli','effectiveIntervalScaleMilli','predictionFrontier','actualContiguousExclusive'];
 function excess(kind){
  ++overflow[kind];if(overflow.firstAt===null)overflow.firstAt=now();
 }
 function note(code){
  if(!running)return;
  if(errors.length>=limits.errors){excess('errors');return;}
  errors.push({at:now(),code});
 }
 function observe(fn){
  if(!running)return;
  try{return fn();}catch{note('observer-exception');}
 }
 function route(state){return !state?.route?0:state.route==='relay'?1:state.route==='rtc'?2:3;}
 function depth(state){
  return Array.isArray(state?.received)?Math.max(0,state.received.length-(state.receivedHead||0)):-1;
 }
 function buffered(socket){
  try{return typeof socket?.bufferedAmount==='number'?socket.bufferedAmount:-1;}catch{return -1;}
 }
 function currentState(){return root.__eaglerPeerTransport;}
 function stateId(state){return state&&typeof state==='object'?(stateIds.get(state)??-1):-1;}
 function socketId(socket){return socket&&typeof socket==='object'?(socketIds.get(socket)??-1):-1;}
 function parse(data){
  if(typeof data==='string'){++counts.nonBinary;return null;}
  let bytes;
  if(data instanceof ArrayBuffer)bytes=new Uint8Array(data);
  else if(ArrayBuffer.isView(data))bytes=new Uint8Array(data.buffer,data.byteOffset,data.byteLength);
  else{++counts.unsupportedBytes;note('unsupported-binary-object');return null;}
  let target=-1;
  if(bytes.length>=2&&bytes[0]===0xe7){target=bytes[1];bytes=bytes.subarray(2);}
  if(bytes.length<6||bytes[0]!==69||bytes[1]!==57||bytes[2]!==78||bytes[3]!==80||bytes[5]!==1){
   ++counts.nonInputBinary;return null;
  }
  if(bytes[4]!==4||bytes.length<44||bytes[42]>32||bytes.length!==44+bytes[42]*12){
   ++counts.malformedInput;note('unsupported-or-malformed-input-schema');return null;
  }
  const v=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength),sender=bytes[6],players=bytes[7];
  if(players!==2||sender>=players)note('unexpected-player-identity');
  let hash=2166136261;
  for(let i=0;i<bytes.length;++i)hash=Math.imul(hash^bytes[i],16777619)>>>0;
  const destination=target>=0?target:players===2&&sender<2?1-sender:-1;
  const fields=[v.getUint32(8,true),v.getUint32(12,true),sender,players,
   v.getUint32(16,true),v.getUint32(20,true),v.getUint32(24,true),v.getUint32(28,true),
   v.getUint32(32,true),v.getUint32(36,true),v.getInt16(40,true),bytes[42],bytes[43],bytes.length,hash,destination];
  return {fields,target};
 }
 function packetId(parsed){
  const key=parsed.fields.join(':');
  const existing=packetIds.get(key);if(existing!==undefined)return existing;
  if(packets.length>=limits.packets){excess('packets');return -1;}
  const id=packets.length;packets.push([id,...parsed.fields]);packetIds.set(key,id);return id;
 }
 function record(phase,data,meta){
  const parsed=parse(data);if(!parsed)return;
  ++counts[['inputIntent','inputWsSend','inputMessage','inputPollCopy'][phase]];
  if(phase===3&&meta.result>0){++counts.inputDequeues;++dequeueSerial;}
  const id=eventSerial++,packet=packetId(parsed);
  if(events.length>=limits.events){excess('events');return;}
  if(packet<0)return;
  events.push([id,phase,packet,meta.socketId??-1,meta.stateId??-1,meta.enteredAt,meta.exitedAt,
   meta.queueBefore??-1,meta.queueAfter??-1,meta.result??-1,meta.bufferedBefore??-1,meta.bufferedAfter??-1,
   +!!meta.threw,parsed.target,meta.routeBefore??-1,meta.routeAfter??-1,meta.closedBefore??-1,meta.closedAfter??-1]);
 }
 function attachRelay(state){
  if(!state||typeof state!=='object'||!state.relay)return;
  if(stateIds.has(state))return;
  if(states.length>=limits.states){excess('states');return;}
  const socket=state.relay,original=socket.onmessage;
  if(typeof original!=='function'){note('relay-message-handler-missing');return;}
  if(sockets.length>=limits.sockets){excess('sockets');return;}
  const sid=states.length,wid=sockets.length;
  stateIds.set(state,sid);socketIds.set(socket,wid);
  states.push({id:sid,attachedAt:now(),localPlayer:state.localPlayer,playerCount:state.playerCount,socketId:wid});
  sockets.push({id:wid,stateId:sid,attachedAt:now()});
  function messageHook(...args){
   if(!running)return Reflect.apply(original,this,args);
   ++counts.messageCalls;
   const enteredAt=now(),queueBefore=depth(state),routeBefore=route(state),closedBefore=+!!state.closed;
   let threw=false;
   try{return Reflect.apply(original,this,args);}catch(error){threw=true;throw error;}
   finally{
    const exitedAt=now();
    observe(()=>record(2,args[0]?.data,{socketId:wid,stateId:sid,enteredAt,exitedAt,
     queueBefore,queueAfter:depth(state),threw,routeBefore,routeAfter:route(state),
     closedBefore,closedAfter:+!!state.closed}));
   }
  }
  socket.onmessage=messageHook;
  messageHooks.push({state,socket,hook:messageHook});
 }
 function wrapImports(imports){
  for(const namespace of ['env','wasi_snapshot_preview1']){
   const table=imports?.[namespace];if(!table||typeof table!=='object'||tableSeen.has(table))continue;
   if(typeof table.eagler_peer_poll_copy!=='function'&&typeof table.eagler_peer_connect!=='function')continue;
   tableSeen.add(table);
   if(importHooks.length>=limits.imports){excess('imports');continue;}
   const entry={table,namespace,connect:null,copy:null,at:now()};
   if(typeof table.eagler_peer_connect==='function'){
    const original=table.eagler_peer_connect;
    entry.connect=function(...args){
     if(!running)return Reflect.apply(original,this,args);
     ++counts.connectCalls;++importDepth;
     try{
      const result=Reflect.apply(original,this,args);
      if(result>0)observe(()=>attachRelay(currentState()));
      return result;
     }finally{--importDepth;}
    };
    table.eagler_peer_connect=entry.connect;
   }else note('connect-import-missing');
   if(typeof table.eagler_peer_poll_copy==='function'){
    const original=table.eagler_peer_poll_copy;
    entry.copy=function(...args){
     if(!running)return Reflect.apply(original,this,args);
     ++counts.pollCopyCalls;++importDepth;
     const enteredAt=now();
     let state,packet,queueBefore=-1,routeBefore=-1,closedBefore=-1,result=-1,threw=false;
     observe(()=>{state=currentState();queueBefore=depth(state);routeBefore=route(state);
      closedBefore=+!!state?.closed;packet=state?.received?.[state.receivedHead||0];});
     try{result=Reflect.apply(original,this,args);return result;}catch(error){threw=true;throw error;}
     finally{
      --importDepth;const exitedAt=now();if(result>0)++counts.pollCopyPositive;
      if(packet)observe(()=>record(3,packet,{stateId:stateId(state),socketId:socketId(state?.relay),
       enteredAt,exitedAt,queueBefore,queueAfter:depth(state),result,threw,
       routeBefore,routeAfter:route(state),closedBefore,closedAfter:+!!state?.closed}));
     }
    };
    table.eagler_peer_poll_copy=entry.copy;
   }else note('poll-copy-import-missing');
   importHooks.push(entry);++counts.importTables;
  }
 }
 function observePacing(core,source='frame',sampleIndex=-1){
  if(!running)return;
  if(importDepth){++counts.unsafePacingRequests;note('pacing-request-inside-import');return;}
  observe(()=>{
   const at=now();
   if(dequeueSerial===lastPacingDequeue&&at-lastPacingAt<100)return;
   if(typeof core?._th09_network_pacing_info!=='function'){pacingAvailable=false;return;}
   pacingAvailable=true;
   const pointer=core._th09_network_pacing_info(),heap=core.HEAPU32;
   if(!Number.isInteger(pointer)||pointer<0||pointer%4||!heap||pointer/4+6>heap.length){
    note('pacing-buffer-invalid');return;
   }
   const values=Array.from(heap.subarray(pointer/4,pointer/4+6));++counts.pacingReads;
   if(values[0]!==1){note('pacing-version-unsupported');return;}
   lastPacingAt=at;lastPacingDequeue=dequeueSerial;
   if(pacing.length>=limits.pacing){excess('pacing');return;}
   pacing.push([at,source==='pump'?0:source==='frame'?1:2,sampleIndex,dequeueSerial,...values]);
  });
 }
 function attachCore(core){
  if(!running||attachedCore===core)return;
  if(attachedCore){note('multiple-core-objects');return;}
  attachedCore=core;
  observe(()=>{
   const original=core?._th09_rollback_pump;
   if(typeof original!=='function'){note('rollback-pump-export-missing');return;}
   pumpHook=function(...args){
    if(!running)return Reflect.apply(original,this,args);
    ++counts.pumpCalls;
    try{
     const result=Reflect.apply(original,this,args);
     observePacing(core,'pump',-1);
     return result;
    }catch(error){++counts.pumpThrows;throw error;}
   };
   core._th09_rollback_pump=pumpHook;
  });
 }
 const wasm=root.WebAssembly,ws=root.WebSocket;
 observe(()=>{
  if(typeof ws?.prototype?.send!=='function'){note('websocket-send-missing');return;}
  const original=ws.prototype.send;
  wsHook=function(...args){
   if(!running)return Reflect.apply(original,this,args);
   ++counts.wsSendCalls;const enteredAt=now(),before=buffered(this);let threw=false;
   try{return Reflect.apply(original,this,args);}catch(error){threw=true;throw error;}
   finally{
    const exitedAt=now();
    observe(()=>record(1,args[0],{socketId:socketId(this),stateId:stateId(currentState()),
     enteredAt,exitedAt,bufferedBefore:before,bufferedAfter:buffered(this),threw}));
   }
  };
  ws.prototype.send=wsHook;expectedSend=wsHook;
 });
 observe(()=>{
  if(typeof wasm?.instantiate!=='function'){note('wasm-instantiate-missing');return;}
  const original=wasm.instantiate;
  instantiateHook=function(...args){
   if(running)observe(()=>wrapImports(args[1]));
   return Reflect.apply(original,this,args);
  };
  wasm.instantiate=instantiateHook;
  if(typeof wasm.instantiateStreaming==='function'){
   const originalStreaming=wasm.instantiateStreaming;
   streamingHook=function(...args){
    if(running)observe(()=>wrapImports(args[1]));
    return Reflect.apply(originalStreaming,this,args);
   };
   wasm.instantiateStreaming=streamingHook;
  }
 });
 function stop(reason='snapshot'){
  if(running){stopped=clock();stopReason=String(reason).slice(0,64);running=false;}
 }
 root.networkTransportProbe={
  schema:'th09/network-transport-probe/1',attachCore,observePacing,stop,
  expectSendWrapper(wrapper){if(running&&typeof wrapper==='function')expectedSend=wrapper;},
  onSendIntent(data,socket){
   if(!running)return;++counts.intentCalls;const at=now();
   observe(()=>record(0,data,{socketId:socketId(socket),stateId:stateId(currentState()),enteredAt:at,exitedAt:at}));
  },
  snapshot(){
   const identity={wsSend:!!wsHook&&ws?.prototype?.send===expectedSend,
    instantiate:!!instantiateHook&&wasm?.instantiate===instantiateHook,
    instantiateStreaming:streamingHook?wasm?.instantiateStreaming===streamingHook:null,
    pump:attachedCore?!!pumpHook&&attachedCore._th09_rollback_pump===pumpHook:null,
    imports:importHooks.map(h=>({namespace:h.namespace,at:h.at,
     connect:!!h.connect&&h.table.eagler_peer_connect===h.connect,
     pollCopy:!!h.copy&&h.table.eagler_peer_poll_copy===h.copy})),
    messages:messageHooks.map(h=>({stateId:stateId(h.state),socketId:socketId(h.socket),
     handler:h.socket.onmessage===h.hook,currentRelay:h.state.relay===h.socket,route:route(h.state)}))};
   const hooksValid=identity.wsSend&&identity.instantiate&&identity.instantiateStreaming!==false&&identity.pump!==false&&
    identity.imports.every(h=>h.connect&&h.pollCopy)&&identity.messages.every(h=>h.handler&&h.currentRelay&&h.route!==2);
   const overflowed=Object.entries(overflow).some(([key,value])=>key!=='firstAt'&&value>0);
   return {schema:this.schema,scope:'Synthetic two-player TH09 relay packet metadata; API send, JS message and native buffer copy. Not OS arrival, native packet acceptance, physical input or scanout.',
    clock:{unit:'milliseconds',started,stopped,snapshot:clock()},running,stopReason,side:options.side??null,
    limits,overflow,counts,valid:!errors.length&&!overflowed&&hooksValid,
    coverage:{exercised:counts.inputWsSend>0&&counts.inputMessage>0&&counts.inputDequeues>0&&counts.connectCalls>0,
     pacingAvailable,packetEventsAttempted:eventSerial,dequeueSerial,importsAreNativeConsumption:counts.inputDequeues>0,
     exactNativeAcceptance:false,relayTraceIncluded:false,blobDecodeIncluded:false},
    identity,errors,columns:{packet:packetColumns,event:eventColumns,pacing:pacingColumns,
     phases:phaseNames,pacingSources:['pump-return','onGameFrame-end','other-safe-boundary'],
     routeCodes:['unknown','relay','rtc','other']},states,sockets,packets,events,pacing};
  }
 };
}

