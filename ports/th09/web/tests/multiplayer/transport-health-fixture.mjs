// Browser-only transport health fixture. No WASM, game, audio or drawing.
// Also importable in Node for deterministic packet/schedule checks.
export const packetColumns = ['id','sessionLo','sessionHi','sender','players','sequence','ackSequence',
  'latestFrame','ackFrame','firstInputFrame','senderFrame','frameAdvantage','inputCount','reserved',
  'payloadBytes','fnv1a32','destination'];
export const eventColumns = ['id','phase','packetId','planIndex','channelId','enteredAt','exitedAt',
  'bufferedBefore','bufferedAfter','result','queueBefore','queueAfter','threw'];
export const phaseNames = ['api','send','message','poll-copy'];
export const laneNames = ['input','control'];
export function healthSchedule(frames, hz = 60) {
  if (!Number.isInteger(frames) || frames < 1 || frames > 3600 || hz !== 60) throw Error('Invalid fixed health schedule');
  return Array.from({length:frames * 2}, (_, index) => ({
    index, frame:Math.floor(index / 2), lane:index % 2, sequence:index + 1, dueMs:Math.floor(index / 2) * 1000 / hz
  }));
}
export function healthPacket(plan, sender, seed = 12345) {
  const bytes = new Uint8Array(56), view = new DataView(bytes.buffer);
  bytes.set([69,57,78,80,4,1,sender,2]);
  view.setUint32(8,0x09c0ffee,true);view.setUint32(12,seed >>> 0,true);
  view.setUint32(16,plan.sequence,true);view.setUint32(20,0,true);
  view.setUint32(24,plan.frame,true);view.setUint32(28,0xffffffff,true);
  view.setUint32(32,plan.frame,true);view.setUint32(36,plan.frame,true);
  view.setInt16(40,0,true);bytes[42]=1;
  // Deliberately synthetic bytes: they exercise transport, never a title input decoder.
  for(let i=44;i<56;++i) bytes[i]=(seed + plan.frame * 31 + sender * 67 + plan.lane * 101 + i * 17) & 255;
  return bytes;
}
export function healthBytes(data) {
  if(data instanceof ArrayBuffer)return new Uint8Array(data);
  if(ArrayBuffer.isView(data))return new Uint8Array(data.buffer,data.byteOffset,data.byteLength);
  return null;
}
export function parseHealthPacket(data) {
  let bytes=healthBytes(data),target=-1;
  if(!bytes)return null;
  if(bytes.length>=2&&bytes[0]===0xe7){target=bytes[1];bytes=bytes.subarray(2);}
  if(bytes.length!==56||bytes[0]!==69||bytes[1]!==57||bytes[2]!==78||bytes[3]!==80||
      bytes[4]!==4||bytes[5]!==1||bytes[6]>1||bytes[7]!==2||bytes[42]!==1)return null;
  const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
  let fnv=2166136261;for(const byte of bytes)fnv=Math.imul(fnv^byte,16777619)>>>0;
  const sender=bytes[6],sequence=view.getUint32(16,true);
  return {bytes,sequence,sender,lane:(sequence-1)%2,frame:Math.floor((sequence-1)/2),
    fields:[view.getUint32(8,true),view.getUint32(12,true),sender,2,sequence,view.getUint32(20,true),
      view.getUint32(24,true),view.getUint32(28,true),view.getUint32(32,true),view.getUint32(36,true),
      view.getInt16(40,true),bytes[42],bytes[43],bytes.length,fnv,target>=0?target:1-sender]};
}
export function assertHealthPacket(data, plans, expectedSender, seed) {
  const parsed=parseHealthPacket(data);
  if(!parsed||parsed.sender!==expectedSender||!plans[parsed.sequence-1])throw Error('Unexpected health packet identity');
  const expected=healthPacket(plans[parsed.sequence-1],expectedSender,seed);
  if(parsed.bytes.length!==expected.length||parsed.bytes.some((byte,i)=>byte!==expected[i]))
    throw Error('Health packet differs from complete deterministic bytes');
  return parsed;
}
// Small optional observer shared with the Node constructor contract test.
// It never reads constructor arguments, SDP, candidate data or error messages.
export function createHealthConstructorTrace(now,limit=96) {
  if(!Number.isInteger(limit)||limit<1)throw Error('Invalid constructor trace limit');
  const events=[],counts={entered:0,returned:0,threw:0,observerErrors:0};let overflow=0;
  const names=['Error','TypeError','RangeError','ReferenceError','SyntaxError','AbortError','InvalidAccessError',
    'InvalidStateError','NotAllowedError','NotFoundError','NotReadableError','NotSupportedError','OperationError',
    'RTCError','SecurityError','TimeoutError','UnknownError'];
  function record(call,event,cause){
    try{
      const at=now();if(!Number.isFinite(at))throw Error('clock');
      let errorName=null;
      if(event==='threw'){let name;try{name=cause?.name;}catch{}errorName=names.includes(name)?name:'UnknownError';}
      if(events.length<limit)events.push({id:events.length,call,event,at,errorName});else ++overflow;
    }catch{++counts.observerErrors;}
  }
  return {
    enter(){const call=counts.entered++;record(call,'enter');return call;},
    returned(call){++counts.returned;record(call,'returned');},
    threw(call,cause){++counts.threw;record(call,'threw',cause);},
    snapshot(constructorCurrent){return {version:1,enabled:true,limit,overflow,counts:{...counts},
      valid:constructorCurrent&&!overflow&&!counts.observerErrors&&counts.entered===counts.returned+counts.threw,
      coverage:{exercised:counts.entered>0},identity:{constructorCurrent},events:events.map(row=>({...row})),
      scope:'performance.now() immediately around existing super(...args), before local channel observer attachment. The captured NativePC may include preinstalled browser/page wrappers. No additional connection, timer, proxy change, constructor argument or error message capture.'};}
  };
}
export function healthPeerConnectionObserver(NativePC,observeChannel,trace=null) {
  return class ObservedPeerConnection extends NativePC {
    constructor(...args){
      const call=trace?.enter();
      try{super(...args);}catch(cause){trace?.threw(call,cause);throw cause;}
      trace?.returned(call);
      this.addEventListener('datachannel',event=>observeChannel(event.channel,'rtc'));
    }
    createDataChannel(...args){const channel=super.createDataChannel(...args);observeChannel(channel,'rtc');return channel;}
  };
}
export async function installTransportHealthPage(options) {
  if(globalThis.__transportHealth)throw Error('Health fixture already installed');
  const {side,frames,seed,route,sources,relayUrl,pollMs,blockAtMs,blockMs,blockSide}=options;
  const rtcConstructorTrace=options.rtcConstructorTrace===true;
  const plans=healthSchedule(frames),now=()=>performance.now();
  const clock=()=>{const before=now(),epochMs=Date.now(),after=now();return {timeOrigin:performance.timeOrigin,before,epochMs,after};};
  const started=clock(),packets=[],events=[],channels=[],heartbeats=[],rafs=[],blocks=[],states=[],rtc=[],errors=[];
  const constructorTrace=rtcConstructorTrace?createHealthConstructorTrace(now):null;
  const limits={packets:frames*4+64,events:frames*20+512,heartbeats:18000,rafs:24000,states:512,rtc:256,errors:128,channels:32};
  const overflow=Object.fromEntries(Object.keys(limits).map(key=>[key,0]));
  const counts={api:0,send:0,message:0,pollCopy:0,emptyPolls:0,unrecognizedBinary:0,nonBinarySend:0,
    nonBinaryMessage:0,invalidPayloads:0,schedulerCalls:0,statsSkipped:0};
  const packetIds=new Map(),channelIds=new WeakMap(),listeners=[];
  let running=true,armed=false,activePlan=-1,startAt=null,startEpochMs=null,stopAt=null,done=false;
  let nextPlan=0,sendTimer=null,pollTimer=null,heartbeatTimer=null,statsTimer=null,blockTimer=null,rafHandle=null,statsInFlight=false;
  let lastHeartbeat=now(),lastState=null;
  function add(kind,value){if(!running)return;const list={packets,events,channels,heartbeats,rafs,blocks,states,rtc,errors}[kind];
    const limit=kind==='blocks'?4:limits[kind];if(list.length<limit)list.push(value);else if(kind!=='blocks')++overflow[kind];}
  function error(code,detail=''){add('errors',{at:now(),code,detail:String(detail).slice(0,500)});}
  function queue(){const state=globalThis.__eaglerPeerTransport;return state?state.received.length-(state.receivedHead||0):0;}
  function packetId(parsed){const key=parsed.fields.join(':');if(packetIds.has(key))return packetIds.get(key);
    if(packets.length>=limits.packets){++overflow.packets;return -1;}const id=packets.length;packets.push([id,...parsed.fields]);packetIds.set(key,id);return id;}
  function record(phase,data,meta={}) {
    const parsed=parseHealthPacket(data);
    if(!parsed){if(typeof data==='string')++counts[phase===1?'nonBinarySend':'nonBinaryMessage'];else ++counts.unrecognizedBinary;return;}
    const id=packetId(parsed);if(id<0)return;
    ++counts[['api','send','message','pollCopy'][phase]];
    if(phase===2||phase===3){try{assertHealthPacket(data,plans,1-side,seed);}catch(cause){++counts.invalidPayloads;error('received-packet-integrity',cause.message);}}
    add('events',[events.length,phase,id,meta.planIndex??-1,meta.channelId??-1,meta.enteredAt??now(),meta.exitedAt??now(),
      meta.bufferedBefore??-1,meta.bufferedAfter??-1,meta.result??-1,meta.queueBefore??-1,meta.queueAfter??-1,+!!meta.threw]);
  }
  const NativeWS=globalThis.WebSocket,NativePC=globalThis.RTCPeerConnection;
  let observedPeerConnection=null;
  const originalWsSend=NativeWS.prototype.send,originalDcSend=globalThis.RTCDataChannel?.prototype.send;
  function buffered(channel){return typeof channel?.bufferedAmount==='number'?channel.bufferedAmount:-1;}
  function channelInfo(channel,kind,id) {
    if(!channel)return null;
    return {id,kind,at:now(),url:kind==='websocket'?String(channel.url):null,label:channel.label??null,
      channelId:channel.id??null,ordered:channel.ordered??null,maxRetransmits:channel.maxRetransmits??null,
      maxPacketLifeTime:channel.maxPacketLifeTime??null,protocol:channel.protocol??null,negotiated:channel.negotiated??null,
      priority:channel.priority??null,readyState:channel.readyState,binaryType:channel.binaryType,bufferedAmount:buffered(channel)};
  }
  function observeChannel(channel,kind) {
    if(channelIds.has(channel))return channelIds.get(channel);
    if(channels.length>=limits.channels){++overflow.channels;return -1;}
    const id=channels.length;channelIds.set(channel,id);add('channels',channelInfo(channel,kind,id));
    const onMessage=event=>{if(!running)return;const enteredAt=now();record(2,event.data,{channelId:id,enteredAt,exitedAt:now(),queueBefore:queue(),queueAfter:queue()});};
    channel.addEventListener('message',onMessage);listeners.push([channel,'message',onMessage]);
    for(const name of ['open','close','error']){
      const listener=()=>add('states',{at:now(),kind:'channel-'+name,channel:channelInfo(channel,kind,id)});
      channel.addEventListener(name,listener);listeners.push([channel,name,listener]);
    }
    return id;
  }
  class ObservedWebSocket extends NativeWS {
    constructor(...args){super(...args);observeChannel(this,'websocket');}
  }
  globalThis.WebSocket=ObservedWebSocket;
  function sendHook(original) {return function(data){
    if(!running)return Reflect.apply(original,this,arguments);
    const enteredAt=now(),before=buffered(this);let threw=false;
    try{return Reflect.apply(original,this,arguments);}catch(cause){threw=true;throw cause;}
    finally{record(1,data,{planIndex:activePlan,channelId:channelIds.get(this)??-1,enteredAt,exitedAt:now(),
      bufferedBefore:before,bufferedAfter:buffered(this),threw});}
  };}
  NativeWS.prototype.send=sendHook(originalWsSend);
  if(route==='rtc'){
    if(typeof NativePC!=='function'||typeof originalDcSend!=='function')throw Error('RTC unavailable before health setup');
    observedPeerConnection=healthPeerConnectionObserver(NativePC,observeChannel,constructorTrace);
    globalThis.RTCPeerConnection=observedPeerConnection;
    globalThis.RTCDataChannel.prototype.send=sendHook(originalDcSend);
  }else globalThis.RTCPeerConnection=undefined;
  const heap=new Uint8Array(65536);globalThis.HEAPU8=heap;
  globalThis.Module={eaglerOptions:{netplaySpectatorCount:0,netplayIceServers:[]}};
  const abi=Object.fromEntries(Object.entries(sources).map(([name,{args,body}])=>[name,new Function(...args,body)]));
  const encoder=new TextEncoder();heap.set(encoder.encode(relayUrl),0);heap.set(encoder.encode('th09'),2048);
  function channelSnapshot(){
    const state=globalThis.__eaglerPeerTransport;
    return {at:now(),route:state?.route??null,failed:!!state?.failed,disconnected:!!state?.disconnected,closed:!!state?.closed,
      error:state?.error??'',queue:queue(),relayReadyState:state?.relay?.readyState??null,relayBuffered:buffered(state?.relay),
      iceServerCount:state?.iceServers?.length??null,
      peers:[...(state?.peers??[])].map(([peerId,peer])=>({peerId,connectionState:peer.pc.connectionState,iceConnectionState:peer.pc.iceConnectionState,
        input:channelInfo(peer.inputDc,'rtc',channelIds.get(peer.inputDc)??-1),
        control:channelInfo(peer.controlDc,'rtc',channelIds.get(peer.controlDc)??-1)}))};
  }
  function observeState(){
    try{const value=channelSnapshot(),signature=JSON.stringify({...value,at:0,queue:0,relayBuffered:0,
      peers:value.peers.map(peer=>({...peer,input:{...peer.input,at:0,bufferedAmount:0},control:{...peer.control,at:0,bufferedAmount:0}}))});
      if(signature!==lastState){lastState=signature;add('states',{at:value.at,kind:'transport',state:value});}
      if(value.failed||value.disconnected)error('transport-ended',value.error);
    }catch(cause){error('state-observation',cause.message);}
  }
  async function observeRtc(reason='periodic') {
    if(statsInFlight){++counts.statsSkipped;return;}statsInFlight=true;
    try{
      const state=globalThis.__eaglerPeerTransport,peers=[];
      for(const [peerId,peer] of state?.peers??[]){
        const reports=await peer.pc.getStats();let pair=null;
        for(const report of reports.values())if(report.type==='transport'&&report.selectedCandidatePairId)pair=reports.get(report.selectedCandidatePairId);
        if(!pair)for(const report of reports.values())if(report.type==='candidate-pair'&&report.state==='succeeded'&&report.nominated){pair=report;break;}
        const select=(value,fields)=>value?Object.fromEntries(fields.filter(key=>value[key]!==undefined).map(key=>[key,value[key]])):null;
        const candidateFields=['id','candidateType','protocol','address','port','networkType','relayProtocol','tcpType','foundation','priority'];
        peers.push({peerId,connectionState:peer.pc.connectionState,iceConnectionState:peer.pc.iceConnectionState,
          pair:select(pair,['id','state','nominated','localCandidateId','remoteCandidateId','bytesSent','bytesReceived','currentRoundTripTime','availableOutgoingBitrate']),
          local:select(pair?reports.get(pair.localCandidateId):null,candidateFields),
          remote:select(pair?reports.get(pair.remoteCandidateId):null,candidateFields),
          channels:channelSnapshot().peers.find(value=>value.peerId===peerId)});
      }
      add('rtc',{at:now(),reason,route:state?.route??null,peers});
    }catch(cause){error('get-stats',cause.message);}finally{statsInFlight=false;}
  }
  function drain(){
    if(!running)return;const before=queue();
    try{for(let i=0;i<512;++i){const enteredAt=now(),q=queue(),result=abi.eagler_peer_poll_copy(8192,1024);
      if(!result){++counts.emptyPolls;break;}
      record(3,heap.slice(8192,8192+result),{enteredAt,exitedAt:now(),result,queueBefore:q,queueAfter:queue()});
    }}catch(cause){error('poll-copy',cause.message);}
    if(before>0||queue()>0)observeState();
  }
  function heartbeat(){
    const enteredAt=now();add('heartbeats',{at:enteredAt,gapMs:enteredAt-lastHeartbeat,queue:queue(),
      nextPlan,route:globalThis.__eaglerPeerTransport?.route??null});lastHeartbeat=enteredAt;observeState();
  }
  function requestRaf(){
    if(!running)return;const row={id:rafs.length,requestedAt:now(),enteredAt:null,exitedAt:null,rafTimestamp:null,cancelledAt:null};
    if(rafs.length>=limits.rafs){++overflow.rafs;return;}rafs.push(row);
    rafHandle=requestAnimationFrame(timestamp=>{row.enteredAt=now();row.rafTimestamp=timestamp;try{requestRaf();}finally{row.exitedAt=now();}});
  }
  function sendDue(){
    if(!running||!armed)return;++counts.schedulerCalls;
    let sent=0;
    while(nextPlan<plans.length&&plans[nextPlan].dueMs<=now()-startAt&&sent<128){
      const plan=plans[nextPlan++],payload=healthPacket(plan,side,seed),enteredAt=now(),q=queue();
      heap.set(payload,4096);activePlan=plan.index;let result=-1,threw=false;
      try{result=plan.lane===0?abi.eagler_peer_send_to(1-side,4096,payload.length):abi.eagler_peer_send(4096,payload.length,1);}
      catch(cause){threw=true;error('send-api',cause.message);}
      finally{record(0,payload,{planIndex:plan.index,enteredAt,exitedAt:now(),result,queueBefore:q,queueAfter:queue(),threw});activePlan=-1;}
      if(result!==1)error('send-api-not-accepted','plan '+plan.index+' returned '+result);
      ++sent;
    }
    if(nextPlan===plans.length){done=true;observeState();return;}
    sendTimer=setTimeout(sendDue,Math.max(0,startAt+plans[nextPlan].dueMs-now()));
  }
  const snapshot=()=>({schema:'th09/transport-health-page/1',side,options:{frames,seed,route,pollMs,blockAtMs,blockMs,blockSide,rtcConstructorTrace},
    clock:{started,armed:armedClock,stopped:stoppedClock},running,armed,done,startAt,startEpochMs,stopAt,nextPlan,
    columns:{packets:packetColumns,events:eventColumns,phases:phaseNames,lanes:laneNames},
    limits,overflow:{...overflow},counts:{...counts},packets:packets.slice(),events:events.slice(),channels:channels.slice(),
    heartbeats:heartbeats.slice(),rafs:rafs.slice(),blocks:blocks.slice(),states:states.slice(),rtc:rtc.slice(),errors:errors.slice(),
    rtcConstructorTrace:constructorTrace?{...constructorTrace.snapshot(globalThis.RTCPeerConnection===observedPeerConnection),timeOrigin:started.timeOrigin}:null,
    current:channelSnapshot(),scope:'Actual shared browser transport functions, JS Uint8Array ABI storage; no WASM, C++ SessionChannel, native input repair policy, game, audio or Draw. Message hooks run before the shared handler; poll-copy uses the real JS transport body.'});
  let armedClock=null,stoppedClock=null;
  globalThis.__transportHealth={
    ready:()=>abi.eagler_peer_is_open()===1,
    progress:()=>({done,nextPlan,route:globalThis.__eaglerPeerTransport?.route??null}),
    snapshot,observeRtc,
    arm(epochMs){
      if(armed||!Number.isFinite(epochMs)||epochMs-performance.timeOrigin-now()<100)throw Error('Health start must be a future common epoch');
      startEpochMs=epochMs;startAt=epochMs-performance.timeOrigin;armed=true;armedClock=clock();
      add('states',{at:now(),kind:'armed',startAt,startEpochMs});
      if(blockMs>0&&side===blockSide)blockTimer=setTimeout(()=>{
        const enteredAt=now(),until=enteredAt+blockMs;let iterations=0;
        while(now()<until)++iterations;
        add('blocks',{plannedAt:startAt+blockAtMs,enteredAt,exitedAt:now(),requestedMs:blockMs,iterations});
      },Math.max(0,startAt+blockAtMs-now()));
      sendTimer=setTimeout(sendDue,Math.max(0,startAt-now()));return {startAt,startEpochMs};
    },
    async stop(){
      if(!running)return snapshot();clearTimeout(sendTimer);clearTimeout(blockTimer);
      clearInterval(pollTimer);clearInterval(heartbeatTimer);clearInterval(statsTimer);
      drain();await observeRtc('stop');observeState();stopAt=now();stoppedClock=clock();
      cancelAnimationFrame(rafHandle);const pending=rafs.at(-1);if(pending&&pending.enteredAt===null)pending.cancelledAt=stopAt;
      const final=snapshot();running=false;final.running=false;return final;
    },
    close(){
      if(running){clearTimeout(sendTimer);clearTimeout(blockTimer);clearInterval(pollTimer);clearInterval(heartbeatTimer);clearInterval(statsTimer);cancelAnimationFrame(rafHandle);running=false;}
      for(const [target,name,listener] of listeners)target.removeEventListener(name,listener);
      abi.eagler_peer_close();NativeWS.prototype.send=originalWsSend;
      if(originalDcSend)globalThis.RTCDataChannel.prototype.send=originalDcSend;
      globalThis.WebSocket=NativeWS;globalThis.RTCPeerConnection=NativePC;
    }
  };
  if(abi.eagler_peer_connect(0,2048,side,2)!==1)throw Error('Shared browser connect rejected');
  // The concrete WebSocket/RTC hooks were registered before the shared handlers.
  pollTimer=setInterval(drain,pollMs);heartbeatTimer=setInterval(heartbeat,10);
  statsTimer=setInterval(()=>observeRtc(),1000);requestRaf();observeState();
  return {installed:true,clock:started,plannedPackets:plans.length};
}
