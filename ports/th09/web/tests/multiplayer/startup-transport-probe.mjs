// Opt-in startup observation only. No SDP, candidate text, addresses, URLs or
// error messages are retained. This function is self-contained for addInitScript.
export function installStartupTransportProbe(options={}){
 if(options.enabled===false)return;
 const root=globalThis;
 if(root.startupTransportProbe)throw Error('Startup transport probe already installed');
 const limits={events:4096,sockets:32,pcs:16,channels:64,errors:32,...options.limits};
 for(const value of Object.values(limits))if(!Number.isInteger(value)||value<1)throw Error('Invalid startup trace limit');
 const clockPair=()=>{const before=performance.now(),epochMs=Date.now(),after=performance.now();return {timeOrigin:performance.timeOrigin,before,epochMs,after};};
 const started=clockPair(),events=[],sockets=[],pcs=[],channels=[],errors=[];
 const socketMap=new WeakMap(),pcMap=new WeakMap(),channelMap=new WeakMap(),methodHooks=[];
 const overflow={events:0,sockets:0,pcs:0,channels:0,errors:0};
 const counts={wsSend:0,wsMessage:0,pcCalls:0,pcSettled:0,channelEvents:0,ignoredSockets:0};
 const wsOriginal=root.WebSocket,pcOriginal=root.RTCPeerConnection,promiseThen=Promise.prototype.then;
 let running=true,frozen=null,serial=0,callSerial=0,wsProxy=null,pcProxy=null,sendHook=null,expectedSend=null;
 const errorNames=['Error','TypeError','RangeError','ReferenceError','SyntaxError','AbortError','DataError','InvalidAccessError','InvalidModificationError','InvalidStateError','NetworkError','NotAllowedError','NotFoundError','NotReadableError','NotSupportedError','OperationError','QuotaExceededError','RTCError','SecurityError','TimeoutError','UnknownError'];
 const errorName=error=>{let name;try{name=typeof error==='string'?error.split(':',1)[0]:error?.name;}catch{}return errorNames.includes(name)?name:'UnknownError';};
 const selected=(value,allowed)=>allowed.includes(value)?value:null;
 const peer=value=>Number.isInteger(value)&&value>=0&&value<3?value:null;
 function note(code){if(!running)return;if(errors.length<limits.errors)errors.push({at:performance.now(),code});else ++overflow.errors;}
 function safe(operation){try{return operation();}catch{note('observer-operation-failed');return null;}}
 function record(target,entity,event,fields={}){
  if(!running)return;
  safe(()=>{if(events.length>=limits.events){++overflow.events;return;}
   events.push({id:serial++,at:performance.now(),target,entity,event,route:selected(root.__eaglerNetplayTransport,['rtc','relay']),...fields});});
 }
 function metadata(data){
  if(typeof data!=='string'||data.length>65536)return null;
  let m;try{m=JSON.parse(data);}catch{return null;}
  if(!m||!['signal','rtc-ready','rtc-failed','route','peers','peer-join','peer-disconnected','ice-restart-request','error'].includes(m.type))return null;
  return {type:m.type,descriptionType:selected(m.description?.type,['offer','answer','pranswer','rollback']),
   candidatePresent:!!m.candidate,from:peer(m.from),to:peer(m.to),player:peer(m.player),
   peers:Array.isArray(m.peers)?m.peers.slice(0,3).map(peer):null,mode:selected(m.mode??m.route,['rtc','relay']),
   errorName:m.error==null?null:errorName(m.error)};
 }
 function pcState(pc){return safe(()=>({connection:selected(pc.connectionState,['new','connecting','connected','disconnected','failed','closed']),
  ice:selected(pc.iceConnectionState,['new','checking','connected','completed','disconnected','failed','closed']),
  gathering:selected(pc.iceGatheringState,['new','gathering','complete']),signaling:selected(pc.signalingState,['stable','have-local-offer','have-remote-offer','have-local-pranswer','have-remote-pranswer','closed'])}))||{};}
 const dcState=dc=>selected(dc.readyState,['connecting','open','closing','closed']);
 function attachChannel(dc,pcId,origin){
  if(!dc||channelMap.has(dc)||!running)return;
  if(channels.length>=limits.channels){++overflow.channels;return;}
  const label=typeof dc.label==='string'?dc.label:'';
  const row={id:channels.length,pc:pcId,kind:label.endsWith('-input')?'input':label.endsWith('-control')?'control':'other',
   origin,ordered:typeof dc.ordered==='boolean'?dc.ordered:null,maxRetransmits:Number.isInteger(dc.maxRetransmits)?dc.maxRetransmits:null};
  channels.push(row);channelMap.set(dc,row);
  record('dc',row.id,'created',{state:dcState(dc)});
  for(const event of ['open','close','error'])dc.addEventListener(event,e=>safe(()=>{
   if(running)++counts.channelEvents;
   record('dc',row.id,event,{state:dcState(dc),errorName:event==='error'?errorName(e.error):null});
  }));
 }
 function attachPc(pc,constructorCall){
  if(pcMap.has(pc)||!running)return;
  if(pcs.length>=limits.pcs){++overflow.pcs;return;}
  const row={id:pcs.length,createdAt:performance.now(),constructorCall};pcs.push(row);pcMap.set(pc,row);
  record('pc',row.id,'created',{constructorCall,...pcState(pc)});
  for(const event of ['connectionstatechange','iceconnectionstatechange','icegatheringstatechange','signalingstatechange'])
   pc.addEventListener(event,()=>safe(()=>record('pc',row.id,event,pcState(pc))));
  pc.addEventListener('icecandidate',e=>safe(()=>record('pc',row.id,'icecandidate',{candidatePresent:!!e.candidate})));
  pc.addEventListener('icecandidateerror',e=>safe(()=>record('pc',row.id,'icecandidateerror',{errorName:errorName(e.error)})));
  pc.addEventListener('datachannel',e=>safe(()=>attachChannel(e.channel,row.id,'remote-event')));
 }
 function attachSocket(socket,args){
  if(socketMap.has(socket)||!running)return;
  if(typeof args[0]!=='string'||args[0].length>16384){++counts.ignoredSockets;return;}
  const url=new URL(args[0],root.location?.href);
  if(url.hostname!=='127.0.0.1'||url.port!==String(options.relayPort)){++counts.ignoredSockets;return;}
  if(sockets.length>=limits.sockets){++overflow.sockets;return;}
  const kind=url.searchParams.has('lobby')?'lobby':url.searchParams.has('signal')?'signal':url.searchParams.has('player')?'player':'other';
  const row={id:sockets.length,kind,player:url.searchParams.has('player')?peer(Number(url.searchParams.get('player'))):null,createdAt:performance.now()};
  sockets.push(row);socketMap.set(socket,row);record('ws',row.id,'created',{readyState:socket.readyState});
  for(const event of ['open','close','error'])socket.addEventListener(event,e=>safe(()=>record('ws',row.id,event,{
   readyState:socket.readyState,code:event==='close'&&Number.isInteger(e.code)?e.code:null,errorName:event==='error'?errorName(e.error):null})));
  socket.addEventListener('message',e=>safe(()=>{
   if(!running||!['signal','player'].includes(kind))return;
   const data=metadata(e.data);if(!data)return;
   ++counts.wsMessage;record('ws',row.id,'message',{...data,readyState:socket.readyState});
  }));
 }
 function replace(proto,name,make){
  const descriptor=Object.getOwnPropertyDescriptor(proto,name);
  if(!descriptor||typeof descriptor.value!=='function'||!descriptor.writable){note('unwrappable-'+name);return null;}
  const hook=make(descriptor.value);Object.defineProperty(proto,name,{...descriptor,value:hook});
  methodHooks.push({proto,name,hook});return hook;
 }
 if(typeof wsOriginal==='function')safe(()=>{
  sendHook=replace(wsOriginal.prototype,'send',original=>function(...args){
   if(!running)return Reflect.apply(original,this,args);
   const row=socketMap.get(this),data=safe(()=>row&&['signal','player'].includes(row.kind)?metadata(args[0]):null),call=callSerial++;
   if(data){++counts.wsSend;record('ws',row.id,'send-enter',{call,...data,readyState:this.readyState});}
   try{const result=Reflect.apply(original,this,args);if(data)record('ws',row.id,'send-exit',{call,threw:false});return result;}
   catch(error){if(data)record('ws',row.id,'send-exit',{call,threw:true,errorName:errorName(error)});throw error;}
  });
  expectedSend=sendHook;
  wsProxy=new Proxy(wsOriginal,{construct(target,args,newTarget){
   const result=Reflect.construct(target,args,newTarget===wsProxy?target:newTarget);safe(()=>attachSocket(result,args));return result;
  }});
  root.WebSocket=wsProxy;
 });
 if(typeof pcOriginal==='function'&&options.rtcEnabled!==false)safe(()=>{
  for(const name of ['createOffer','createAnswer','setLocalDescription','setRemoteDescription','addIceCandidate','createDataChannel','close']){
   replace(pcOriginal.prototype,name,original=>function(...args){
    if(!running)return Reflect.apply(original,this,args);
    const row=pcMap.get(this),call=callSerial++,asyncCall=!['createDataChannel','close'].includes(name);
    if(row&&running){++counts.pcCalls;record('pc',row.id,'call',{call,method:name,
     descriptionType:['setLocalDescription','setRemoteDescription'].includes(name)?selected(args[0]?.type,['offer','answer','pranswer','rollback']):null,
     candidatePresent:name==='addIceCandidate'?!!args[0]:null,...pcState(this)});}
    let result;
    try{result=Reflect.apply(original,this,args);}
    catch(error){if(row)record('pc',row.id,'throw',{call,method:name,errorName:errorName(error),...pcState(this)});throw error;}
    if(row&&name==='createDataChannel')safe(()=>attachChannel(result,row.id,'local-api'));
    if(row&&asyncCall&&running){
     // All audited common startup callers await these native promises. Return
     // the identical promise; the sidecar observes settlement, never its SDP.
     safe(()=>Reflect.apply(promiseThen,result,[
      ()=>safe(()=>{++counts.pcSettled;record('pc',row.id,'resolved',{call,method:name,...pcState(this)});}),
      error=>safe(()=>{++counts.pcSettled;record('pc',row.id,'rejected',{call,method:name,errorName:errorName(error),...pcState(this)});})
     ]));
    }else if(row)record('pc',row.id,'returned',{call,method:name,...pcState(this)});
    return result;
   });
  }
  pcProxy=new Proxy(pcOriginal,{construct(target,args,newTarget){
   const call=callSerial++;
   record('pc',null,'construct-enter',{call});
   let result;try{result=Reflect.construct(target,args,newTarget===pcProxy?target:newTarget);}
   catch(error){record('pc',null,'construct-threw',{call,errorName:errorName(error)});throw error;}
   record('pc',null,'construct-returned',{call});
   safe(()=>attachPc(result,call));return result;
  }});
  root.RTCPeerConnection=pcProxy;
 });
 const initialAvailability={webSocket:typeof wsOriginal==='function',rtcPeerConnection:typeof pcOriginal==='function',rtcEnabled:options.rtcEnabled!==false};
 root.startupTransportProbe={
  expectSendWrapper(wrapper){if(typeof wrapper==='function')expectedSend=wrapper;},
  mark(label){if(['before-connect','after-connect','first-active'].includes(label))record('harness',null,label);},
  snapshot({stop=false}={}){
   if(frozen)return frozen;
   const identity={webSocket:!!wsProxy&&root.WebSocket===wsProxy,wsSend:!!sendHook&&wsOriginal.prototype.send===expectedSend,
    rtcConstructor:pcProxy?root.RTCPeerConnection===pcProxy:null,
    methods:methodHooks.filter(h=>h.name!=='send').map(h=>({name:h.name,current:h.proto[h.name]===h.hook}))};
   const value={version:2,scope:'JS startup API and event dispatch times; promise settlement includes async and dispatch waiting. Constructor enter/return surround the original Reflect.construct before observer attachment. No SDP, IP, URLs, credentials or error messages. No additional heartbeat, connectivity test or network call.',
    clock:{unit:'milliseconds',started,snapshot:clockPair()},side:options.side??null,initialAvailability,limits:{...limits},
    valid:!errors.length&&!Object.values(overflow).some(Boolean)&&identity.webSocket&&identity.wsSend&&identity.rtcConstructor!==false&&identity.methods.every(h=>h.current),
    coverage:{signalingExercised:counts.wsSend+counts.wsMessage>0,peerConnectionExercised:pcs.length>0,constructorBoundaries:!!pcProxy,routeObserved:events.some(e=>e.type==='route'||e.type==='peers'&&e.mode)},
    identity,counts:{...counts},overflow:{...overflow},errors:errors.map(e=>({...e})),sockets:sockets.map(e=>({...e})),pcs:pcs.map(e=>({...e})),channels:channels.map(e=>({...e})),events:events.map(e=>({...e}))};
   if(stop){running=false;value.clock.stopped=clockPair();frozen=value;}
   return value;
  }
 };
}
