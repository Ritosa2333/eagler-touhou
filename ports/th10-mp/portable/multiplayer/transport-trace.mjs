// Test-only transport metadata. The caller supplies callback opportunities;
// this module never schedules timers, polls packets, or installs message hooks.
export const transportTraceNativeSchema=Object.freeze([
 'schema','networkEnabled','open','error','sent','received','ignored','repairs',
 'latestCapture','acknowledgedLocalThroughAllRemotes','confirmedThroughAllRemotes',
 'retiring','generation','mode','bufferedBytes']);
export const transportTracePeerSchema=Object.freeze([
 'player','lastReceivedAtDateEpochMs','lastReceivedDateMinusTimeOriginMs','lastReceivedAgeDateMs',
 'inputReadyState','inputBufferedBytes','controlReadyState','controlBufferedBytes',
 'connectionState','iceConnectionState','recoveryPending','restartInFlight']);
export const transportTraceRowSchema=Object.freeze([
 'atMs','samplePerformanceMs','sampleDateEpochMs','datePerformanceSkewMs',
 'net.canStart','net.nextFrame','net.lastSimulatedFrame','net.confirmedThroughAllRemotes',
 'smooth.correcting','smooth.predictionFrontier','smooth.missingInputs',
 'smooth.intervalScaleMilli','smooth.localCaptureAtNext',
 ...transportTraceNativeSchema.map(name=>'native.'+name),
 'stateAvailable','receivedQueuePackets','route','recovering','disconnected',
 'peerCount','omittedPeerRows','peers']);

const relayHopShimFields=Object.freeze(['matched','scheduled','sent','pendingJobs','queuedBytes',
 'maxTimerOverrunMs','maxDeliveredDelayMs','canceled','dropped','overflow','sendErrors','scheduleErrors']);
export const transportTraceRelayHopRowSchema=Object.freeze([
 'atMs','samplePerformanceMs','relayAvailable','socketId','player','route','readyState',
 'browser.binaryMessagesReceived','browser.lastBinaryReceivePerformanceMs',
 ...relayHopShimFields.map(name=>'shim.'+name)]);

const HARD_LIMIT=2048,MAX_PEERS=2;
const readyStates=['connecting','open','closing','closed'];
const connectionStates=['new','connecting','connected','disconnected','failed','closed'];
const iceStates=['new','checking','connected','completed','disconnected','failed','closed'];
const finite=value=>Number.isFinite(value)?value:null;
const integer=value=>Number.isSafeInteger(value)?value:null;
const choice=(value,allowed)=>allowed.includes(value)?value:null;
const requireValue=(value,message)=>{if(!value)throw Error('Transport trace: '+message);};
const words=(value,count,unsigned)=>{
 requireValue(value&&value.length===count,'status length');
 const result=[];
 for(let i=0;i<count;++i){
  const word=value[i];
  requireValue(Number.isInteger(word)&&word>=(unsigned?0:-2147483648)&&
   word<=(unsigned?4294967295:2147483647),'status word');
  result.push(word);
 }
 return result;
};

// Pass the driver's fixed performance.now() start as startedAtPerformanceMs.
// atMs remains the caller's value; each row also records the actual read clock.
// Disabled mode performs no API, clock or global transport reads.
export function createTransportTrace(api,options={}){
 if(options.enabled===undefined||options.enabled===false)
  return {enabled:false,sample:()=>false,finish:()=>null};
 requireValue(options.enabled===true,'enabled must be boolean');
 const relayHopTrace=options.relayHopTrace===undefined?false:options.relayHopTrace;
 requireValue(typeof relayHopTrace==='boolean','relayHopTrace must be boolean');
 const intervalMs=options.intervalMs??20,maxSamples=options.maxSamples??HARD_LIMIT;
 const startedAtPerformanceMs=options.startedAtPerformanceMs??null;
 requireValue(Number.isFinite(intervalMs)&&intervalMs>=20&&intervalMs<=1000,'interval must be 20..1000 ms');
 requireValue(Number.isInteger(maxSamples)&&maxSamples>=1&&maxSamples<=HARD_LIMIT,'sample limit must be 1..2048');
 requireValue(startedAtPerformanceMs===null||Number.isFinite(startedAtPerformanceMs)&&
  startedAtPerformanceMs>=0,'invalid caller start');
 requireValue(typeof api?.netStatus==='function'&&typeof api?.transportStatus==='function','status readers required');
 const perf=globalThis.performance;
 requireValue(typeof perf?.now==='function'&&Number.isFinite(perf.timeOrigin),'same-realm performance clock required');
 const timeOrigin=perf.timeOrigin,createdPerformanceMs=perf.now(),createdDateEpochMs=Date.now();
 const report={schema:'th10-transport-trace/1',schemaVersion:1,enabled:true,
  scope:'diagnostic callback metadata; instrumented run is not a clean performance sample',
  sampling:{intervalMs,maxSamples,hardLimit:HARD_LIMIT,maxPeers:MAX_PEERS,
   scope:'caller-driven, minimum interval; no timer and no catch-up burst; no reads after the attempt limit',
   droppedScope:'eligible sample calls beyond maxSamples; throttled calls and elapsed intervals without a callback are not dropped'},
  clock:{scope:'same JS realm; performance.timeOrigin fixed at creation; Date.now wall clock kept separately',
   performanceTimeOriginEpochMs:timeOrigin,createdPerformanceMs,createdDateEpochMs,startedAtPerformanceMs,
   atMs:'caller-supplied milliseconds; with startedAtPerformanceMs, performance.now() minus that fixed start',
   lastReceivedAt:'BrowserPeerTransport peer.lastReceivedAt is Date.now epoch milliseconds; initialized at peer creation, then updated by either RTC lane onmessage',
   projection:'lastReceivedDateMinusTimeOriginMs subtracts the fixed realm timeOrigin from the raw wall clock; it is not an observed monotonic receive timestamp',
   skew:'datePerformanceSkewMs = sampleDateEpochMs - (performanceTimeOriginEpochMs + samplePerformanceMs); negative ages and wall-clock changes are retained'},
  rowSchema:transportTraceRowSchema,peerSchema:transportTracePeerSchema,
  nativeTransportWordSchema:transportTraceNativeSchema,
  fieldSources:{native:'multiplayer_transport_status: exactly 15 u32 words, schema 1, preserved in native order',
   net:'multiplayer_netplay_status: selected i32 indices 1,2,3,4',
   smooth:'multiplayer_smoothness_status: selected i32 indices 2,4,5,6,9; null when not supplied',
   queue:'__eaglerPeerTransport.received.length - receivedHead; only lengths are read',
   recovering:'__eaglerPeerTransport.isRecovering(); read-only status method',
   frames:'native u32 INVALID_FRAME = 4294967295; net/smooth signed INVALID_FRAME = -1',
   counters:'native sent/repairs count successful transport send calls, not remote delivery; received counts SessionChannel Poll results',
   nativeMode:{0:'connecting or unknown',1:'rtc',2:'relay',3:'spectator'},
   stateValues:'unknown or unavailable JS fields are null; queue and buffer sizes are counts and bytes',
   peers:'nested rows follow peerSchema; at most two peers per sample; omissions explicit'},
  valid:true,sampleCalls:0,attempts:0,throttled:0,dropped:0,readErrors:0,invalidTimes:0,
  firstReadError:null,rows:[],complete:false,
  relayHop:relayHopTrace?{schema:'th10-relay-hop/1',enabled:true,valid:true,complete:false,
   rowSchema:transportTraceRelayHopRowSchema,rows:[],
   scope:'same sampling opportunities, clocks and bound as parent trace; no extra timer or payload reads',
   fieldSources:{
    browser:'readRelayHop: current observed gameplay relay socket; WebSocket binary message events before the transport handler, not decoded Input packets or TCP frames',
    browserLifetime:'socketId is observer-local and bounded; receive counts accrue only while that socket is the current relay; a new socket starts at zero',
    absent:'relayAvailable=false uses null identity, state and receive values; missing or failed observer/shim reads invalidate both reports',
    shim:'perfImpairment.stats: page-lifetime counters from the existing WebSocket Input impairment, not per-socket counters',
    sent:'shim.sent counts preserved underlying WebSocket.send calls that returned without throwing; neither instance-send acceptance nor remote delivery',
    queue:'shim.pendingJobs/queuedBytes are scalar queue accounting; queued payloads are not visited',
    clocks:'samplePerformanceMs is exactly the parent row clock; lastBinaryReceivePerformanceMs is performance.now at the observed message event'}
  }:null};
 let closed=false,nextAt=-Infinity,lastAt=-Infinity;
 function sample(atMs,net,smooth){
  if(closed)return false;
  ++report.sampleCalls;
  if(!Number.isFinite(atMs)||atMs<0||atMs<lastAt){
   ++report.invalidTimes;report.valid=false;
   if(report.relayHop)report.relayHop.valid=false;
   return false;
  }
  lastAt=atMs;
  if(atMs<nextAt){++report.throttled;return false;}
  nextAt=atMs+intervalMs;
  if(report.attempts>=maxSamples){++report.dropped;return false;}
  ++report.attempts;
  let stage='clock';
  try{
   const performanceMs=perf.now(),dateEpochMs=Date.now();
   requireValue(Number.isFinite(performanceMs)&&Number.isFinite(dateEpochMs),'invalid read clock');
   stage='netStatus';const n=words(net??api.netStatus(),11,false);
   stage='transportStatus';const t=words(api.transportStatus(),15,true);
   requireValue(t[0]===1,'unsupported transport schema');
   stage='smoothnessStatus';const s=smooth==null?null:words(smooth,10,false);
   requireValue(!s||s[0]===1,'unsupported smoothness schema');
   stage='jsTransport';const state=globalThis.__eaglerPeerTransport;
   let queued=null;
   if(Array.isArray(state?.received)){
    const head=state.receivedHead??0;
    requireValue(Number.isSafeInteger(head)&&head>=0&&head<=state.received.length,'invalid receive queue head');
    queued=state.received.length-head;
   }
   const map=state?.peers instanceof Map?state.peers:null,peers=[];
   if(map)for(const [id,peer] of map){
    if(peers.length>=MAX_PEERS)break;
    const received=finite(peer?.lastReceivedAt);
    peers.push([integer(id),received,received===null?null:received-timeOrigin,
     received===null?null:dateEpochMs-received,
     choice(peer?.inputDc?.readyState,readyStates),finite(peer?.inputDc?.bufferedAmount),
     choice(peer?.controlDc?.readyState,readyStates),finite(peer?.controlDc?.bufferedAmount),
     choice(peer?.pc?.connectionState,connectionStates),choice(peer?.pc?.iceConnectionState,iceStates),
     typeof peer?.recoveryPending==='boolean'?peer.recoveryPending:null,
     typeof peer?.restartInFlight==='boolean'?peer.restartInFlight:null]);
   }
   const recovering=typeof state?.isRecovering==='function'?!!state.isRecovering():null;
   const route=choice(state?.route,['rtc','relay','spectator']);
   let hopRow=null;
   if(report.relayHop){
    stage='relayHopBrowser';
    const evidence=globalThis.__th10BrowserEvidence;
    requireValue(typeof evidence?.readRelayHop==='function','relay hop observer required');
    const hop=evidence.readRelayHop();
    requireValue(hop?.schema==='th10-browser-relay-hop/1'&&hop.enabled===true&&hop.valid===true,
     'invalid relay hop observer');
    requireValue(typeof hop.available==='boolean'&&hop.available===!!state?.relay&&hop.route===route,
     'relay hop transport identity changed');
    if(hop.available){
     requireValue(Number.isSafeInteger(hop.socketId)&&hop.socketId>0&&hop.socketId<=16&&
      Number.isInteger(hop.player)&&hop.player>=0&&hop.player<=2&&
      Number.isInteger(hop.readyState)&&hop.readyState>=0&&hop.readyState<=3&&
      Number.isSafeInteger(hop.binaryMessagesReceived)&&hop.binaryMessagesReceived>=0,
      'invalid relay hop socket values');
     requireValue(hop.binaryMessagesReceived===0?hop.lastBinaryReceivePerformanceMs===null:
      Number.isFinite(hop.lastBinaryReceivePerformanceMs)&&hop.lastBinaryReceivePerformanceMs>=0&&
      hop.lastBinaryReceivePerformanceMs<=performanceMs,'invalid relay receive clock');
    }else{
     requireValue(hop.socketId===null&&hop.player===null&&hop.readyState===null&&
      hop.binaryMessagesReceived===null&&hop.lastBinaryReceivePerformanceMs===null,
      'absent relay must not invent receive values');
    }
    stage='relayHopShim';
    const stats=globalThis.perfImpairment?.stats;
    requireValue(stats?.schema==='th10-websocket-input-impairment/1'&&stats.transportMode==='relay',
     'relay hop WebSocket impairment required');
    const values=relayHopShimFields.map(name=>{
     const value=stats[name],duration=name==='maxTimerOverrunMs'||name==='maxDeliveredDelayMs';
     requireValue((duration?Number.isFinite(value):Number.isSafeInteger(value))&&value>=0,
      'invalid relay hop shim value');
     return value;
    });
    hopRow=[atMs,performanceMs,hop.available,hop.socketId,hop.player,hop.route,hop.readyState,
     hop.binaryMessagesReceived,hop.lastBinaryReceivePerformanceMs,...values];
   }
   report.rows.push([atMs,performanceMs,dateEpochMs,dateEpochMs-(timeOrigin+performanceMs),
    n[1],n[2],n[3],n[4],s?.[2]??null,s?.[4]??null,s?.[5]??null,s?.[6]??null,s?.[9]??null,
    ...t,!!state,queued,route,recovering,
    typeof state?.disconnected==='boolean'?state.disconnected:null,
    map?map.size:null,map?Math.max(0,map.size-peers.length):0,peers]);
   if(report.relayHop)report.relayHop.rows.push(hopRow);
   return true;
  }catch{
   // A diagnostic read cannot replace a native callback result or stop play.
   // The checker must reject invalid evidence instead of treating it as clean.
   report.valid=false;++report.readErrors;
   if(report.relayHop)report.relayHop.valid=false;
   if(!report.firstReadError)report.firstReadError={atMs,stage};
   return false;
  }
 }
 function finish(){
  if(!closed){
   closed=true;report.complete=report.valid&&report.dropped===0&&report.rows.length>0;
   if(report.relayHop)report.relayHop.complete=report.complete&&report.relayHop.valid&&
    report.relayHop.rows.length===report.rows.length;
  }
  return report;
 }
 return {enabled:true,sample,finish};
}
