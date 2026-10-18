// Correctness-only observer of the REAL GameHost / native rAF input path.
// Keep this mutually exclusive with the wall-clock performance touch producer.
// No world writes, input capture/commit/submit exports, artificial tick loop,
// canonical hashing, or packet transmission are performed here.
const INVALID_FRAME=0xffffffff;
const PACKET_BYTES=44+32*12;
const CASE_KINDS=['new-down-move','cancel-redown-move'];
const same=(left,right)=>!!left&&!!right&&left.length===right.length&&left.every((value,index)=>value===right[index]);
const clamp=(value,low,high)=>Math.max(low,Math.min(high,value));
const f32=Math.fround;

// Driver contract:
//   1. Call prepare() inside runtimePrepare, before native input sampling.
//   2. While ownsMovement OR holdMovement is true, suppress the driver's
//      Arrow/Shift stimulus before that sample; the oracle owns the drag.
//   3. Call afterCallback(result) from runtimeFinish. It queues its observation
//      and any pointer events until the current native callback has returned.
//   4. Once done (or the runner's bound expires), stop native FIRST, then call
//      finish({nativeStopped:true,reason:null}). cleanup() is idempotent after
//      that stop assertion. Never destroy app before finish/cleanup.
// options.inputDelay is a runner assertion: only the 0f configuration is
// supported, and the selected packet must end at the actual capture frame.
export function createTouchFrontierOracle(api,options={}){
 const {core,app,netStatus,transportStatus,status}=api;
 const enabled=options.enabled===true,seat=options.seat??0,inputDelay=options.inputDelay??0;
 const minHistoryDistancePx=options.minHistoryDistancePx??2;
 const maxCallbacks=options.maxCallbacks??6000,maxCaptureCallbacks=options.maxCaptureCallbacks??120;
 const clock=()=>globalThis.performance.now();
 const report={schemaVersion:1,enabled,seat,inputDelay,status:'not-covered',passed:false,done:false,
  scope:'correctness-only / real native rAF / GameHost application_touch_state / existing local captures',
  stimulus:{newPointerPixels:16,secondCaseWarmupPixelsPerNewPrepare:3,mode:0,unlimited:true,sensitivity:1},
  constraints:{inputDelayMustBeZero:true,minHistoryDistancePx,maxCallbacks,maxCaptureCallbacks,
   maximumCases:2,maximumCaseSamples:20,maximumSkippedWindows:16,packetBufferBytes:PACKET_BYTES},
  coverage:{newDownMove:false,cancelRedownMove:false,distinctRecoveryWindows:false,
   completeEightWords:false,actualFirstNewCapture:false,physicalPhone:false,
   historicalReadyOrContextTransition:false},
  callbacks:0,prepareCalls:0,recoveryWindows:0,skippedWindows:0,skipped:[],
  warmup:{events:0,begins:0,moves:0,samples:[]},cases:[],failures:[],notes:[],
  cleanup:{nativeStopped:false,canceled:false,freedTouchBuffer:false,freedPacketBuffer:false,complete:false}};
 let touchPointer=0,packetPointer=0,closed=false,hostStopped=false,cleaned=false,terminal=false;
 let callbackSequence=0,microtaskEpoch=0,wasCorrecting=false,window=null,lastStable=null,pending=null;
 let successes=0,owned=false,activePointer=610+seat*10,lastFingerX=f32(.5),lastTargetX=0;
 let warmupDirection=1,warmupFrame=INVALID_FRAME;
 const decodingBytes=new ArrayBuffer(32),decodingWords=new Uint32Array(decodingBytes),decodingFloats=new Float32Array(decodingBytes);
 const now=()=>clock()-(report.startedAtPerformanceMs??clock());
 const check=(condition,message)=>{if(!condition)throw Error('Touch frontier oracle: '+message);};
 const failure=(reason,detail=null)=>{
  if(report.failures.length<8)report.failures.push({reason,detail,callback:callbackSequence,atMs:now()});
  if(pending&&pending.record.status==='pending'){pending.record.status='fail';pending.record.reason=reason;}
  terminal=true;
 };
 const uncovered=(reason,detail=null)=>{
  if(report.notes.length<12)report.notes.push({reason,detail,callback:callbackSequence,atMs:now()});
  if(pending){pending.record.status='not-covered';pending.record.reason=reason;pending=null;}
  terminal=true;
 };
 const wordsAt=(pointer,count,Type=Uint32Array)=>{
  const address=pointer>>>0,buffer=core.memory.buffer;
  check(address>0&&address%4===0&&address+count*4<=buffer.byteLength,'invalid native observation range');
  return Array.from(new Type(buffer,address,count));
 };
 const touch=()=>{
  core.application_touch_state(app,touchPointer);
  return wordsAt(touchPointer,8);
 };
 const decode=words=>{
  decodingWords.set(words);
  return {context:words[0],instance:words[1],ready:words[2]!==0,x:decodingFloats[3],y:decodingFloats[4],
   fast:decodingFloats[5],slow:decodingFloats[6],rate:decodingFloats[7]};
 };
 const movable=words=>{
  const state=decode(words);
  return state.instance!==0&&state.ready&&(state.context===1||state.context===2)&&
   Number.isFinite(state.x)&&Number.isFinite(state.y)&&state.x>=-184&&state.x<=184&&state.y>=32&&state.y<=432;
 };
 const snapshot=()=>{
  const net=netStatus(),transport=transportStatus();
  const pointer=core.multiplayer_smoothness_status(app);
  const smooth=wordsAt(pointer,10,Int32Array);
  check(net.length>=11&&transport.length>=15&&transport[0]===1&&smooth[0]===1,'unsupported status schema');
  return {net,transport,smooth,identity:[net[7]>>>0,net[8]>>>0,transport[12]>>>0],
   correcting:smooth[2]===1,atMs:now(),callback:callbackSequence};
 };
 const brief=sample=>({atMs:sample.atMs,callback:sample.callback,next:sample.net[2],last:sample.net[3],
  recoveryYields:sample.smooth[1],correcting:sample.correcting,correctionEnd:sample.smooth[3],
  predictionFrontier:sample.smooth[4],forwardAdvanced:sample.smooth[7]===1,
  localCaptureAtNext:sample.smooth[9]===1,latestCapture:sample.transport[8]>>>0,
  sessionWords:sample.identity.slice(0,2),generation:sample.identity[2],
  network:{enabled:sample.transport[1]===1,open:sample.transport[2]===1,route:sample.transport[13],
   packetsSent:sample.transport[4],packetsReceived:sample.transport[5],repairsSent:sample.transport[7]}});
 const controls=()=>wordsAt(core.sdl_game_status(),10,Int32Array);
 const history=()=>{
  const players=status();
  check(players.length>=44&&players[0]===1&&players[2]===seat,'local seat/status changed');
  const at=8+seat*12;
  return {source:'multiplayer_status fixed_position / 100; historical negative expectation is approximate',
   stage:players[6],stageFrames:players[7],playerState:players[at+4],fixedX:players[at+5],fixedY:players[at+6],
   x:f32(players[at+5]/100),y:f32(players[at+6]/100),inputKeys:players[at+11]};
 };
 function syncWindow(sample){
  if(sample.correcting&&!wasCorrecting){
   window={id:++report.recoveryWindows,reference:lastStable,start:brief(sample),noted:false};
  }else if(!sample.correcting&&wasCorrecting){
   if(pending&&window?.id===pending.record.windowId&&!pending.record.recovered)
    pending.record.recovered=brief(sample);
   window=null;
  }
  wasCorrecting=sample.correcting;
 }
 function skip(reason,detail=null){
  if(!window||window.noted)return;
  window.noted=true;++report.skippedWindows;
  if(report.skipped.length<16)report.skipped.push({windowId:window.id,reason,detail,start:window.start});
 }
 function addSample(record,sample,words,phase){
  ++record.observedSamples;
  if(record.samples.length<20)record.samples.push({...brief(sample),phase,touchWords:words.slice()});
  else ++record.droppedSamples;
 }
 function expectedTarget(x,y,fingerStart,fingerEnd){
  const dx=f32(f32(f32(fingerEnd)-f32(fingerStart))*640);
  const targetX=f32(clamp(f32(f32(x)+dx),-184,184));
  const targetY=f32(clamp(f32(y),32,432));
  return {x:f32(targetX*100),y:f32(targetY*100),targetX,targetY,physicalDx:dx};
 }
 function packetFor(captureFrame,peer){
  const size=core.multiplayer_packet_build(app,peer,captureFrame,0,0,packetPointer,PACKET_BYTES)>>>0;
  if(!size)return {covered:false,reason:'packet-build-returned-no-sample'};
  check(size>=44&&size<=PACKET_BYTES,'invalid packet byte count');
  const buffer=core.memory.buffer;
  check(packetPointer+size<=buffer.byteLength,'packet observation range');
  const bytes=new Uint8Array(buffer,packetPointer,size),view=new DataView(buffer,packetPointer,size);
  check(bytes[0]===69&&bytes[2]===78&&bytes[3]===80&&bytes[4]===4&&bytes[5]===1,'unsupported input wire format');
  const count=bytes[42],first=view.getUint32(32,true),latest=view.getUint32(24,true);
  check(count<=32&&size===44+12*count,'invalid input packet length');
  const header={magic:Array.from(bytes.subarray(0,4)),version:bytes[4],sender:bytes[6],players:bytes[7],
   sessionWords:[view.getUint32(8,true),view.getUint32(12,true)],first,latest,count,bytes:size};
  if(!count||first===INVALID_FRAME||latest!==first+count-1||captureFrame<first||captureFrame>latest)
   return {covered:false,reason:'first-new-capture-not-in-unacknowledged-packet-window',header};
  // Under the runner's independently enforced 0f setup, require this exact
  // capture tail. Never relabel a sample from an older 32-frame send window.
  if(latest!==captureFrame)return {covered:false,reason:'packet-tail-is-not-zero-delay-capture-frame',header};
  const at=44+(captureFrame-first)*12;
  return {covered:true,header,sample:{frame:captureFrame,buttons:view.getUint16(at,true),
   analogMode:bytes[at+2],flags:bytes[at+3],x:view.getFloat32(at+4,true),y:view.getFloat32(at+8,true),
   xBits:view.getUint32(at+4,true),yBits:view.getUint32(at+8,true)}};
 }
 function verifyCapture(sample){
  if(!pending)return;
  const record=pending.record,latest=sample.transport[8]>>>0;
  if(!same(sample.identity,pending.identity)){
   uncovered('lifecycle-changed-before-first-new-capture',{before:pending.identity,after:sample.identity});return;
  }
  if(latest===pending.captureBefore){
   if(callbackSequence-record.injected.callback>maxCaptureCallbacks)
    uncovered('first-new-capture-not-observed-within-bound');
   return;
  }
  if(latest===INVALID_FRAME||latest<pending.captureBefore){
   uncovered('local-capture-frontier-retired',{before:pending.captureBefore,after:latest});return;
  }
  const captureFrame=pending.captureBefore+1;
  const packet=packetFor(captureFrame,pending.peer);
  record.capture={...brief(sample),captureFrame,packet,nativeControls:controls()};
  if(!packet.covered){uncovered(packet.reason,packet.header??null);return;}
  check(packet.header.sender===seat&&packet.header.players===pending.players&&
   same(packet.header.sessionWords,pending.identity.slice(0,2)),'packet owner differs from injected gesture');
  const value=packet.sample,expected=record.expected,negative=record.historicalNegativeExpected;
  const matches=value.analogMode===2&&(value.flags&3)===3&&
   Math.abs(value.x-expected.x)<=.01&&Math.abs(value.y-expected.y)<=.01;
  const negativeDistance=Math.hypot(value.x-negative.x,value.y-negative.y);
  record.capture.matchesExpected=matches;
  record.capture.distanceFromHistoricalTarget=negativeDistance;
  if(!matches||negativeDistance<minHistoryDistancePx*100-.02){
   record.status='fail';record.reason='first-new-device-capture-did-not-use-complete-frontier';
   failure(record.reason,{expected,negative,actual:value});pending=null;return;
  }
  record.status='pass';record.finishedAtMs=now();++successes;
  report.coverage.actualFirstNewCapture=true;pending=null;
  warmupFrame=INVALID_FRAME;
  if(successes===2)terminal=true;
 }
 function inject(sample,actualWords,old){
  const reference=window.reference,front=decode(reference.words);
  const kind=CASE_KINDS[successes],fingerStart=f32(.5);
  const fingerEnd=f32(front.x<=152 ? .525 : .475);
  const expected=expectedTarget(front.x,front.y,fingerStart,fingerEnd);
  const negative=expectedTarget(old.x,old.y,fingerStart,fingerEnd);
  const separation=Math.hypot(expected.x-negative.x,expected.y-negative.y);
  if(separation<minHistoryDistancePx*100){skip('historical-target-not-distinguishable',{separation});return;}
  if(!same(actualWords,reference.words)){
   failure('eight-word-cache-read-historical-state',{windowId:window.id,before:reference.words,actual:actualWords,history:old});return;
  }
  const nativeBefore=controls();
  if(successes===0&&nativeBefore[6]){skip('first-case-needs-a-new-inactive-drag');return;}
  if(successes===1&&!nativeBefore[6]){skip('second-case-needs-an-active-old-drag');return;}
  const captureBefore=sample.transport[8]>>>0;
  if(captureBefore===INVALID_FRAME||captureBefore>=INVALID_FRAME-1){skip('no-bounded-existing-local-capture');return;}
  const players=status()[1],peer=(seat+1)%players;
  const record={kind,status:'pending',windowId:window.id,windowStart:window.start,
   completeBefore:reference.observation,completeWords:reference.words.slice(),history:old,
   injected:brief(sample),nativeBefore,captureBefore,expected,historicalNegativeExpected:negative,
   historicalNegativeIsApproximate:true,expectedSeparation:separation,
   pointer:{id:activePointer+1,start:[fingerStart,f32(.5)],end:[fingerEnd,f32(.5)]},
   exactEightWordChecks:0,observedSamples:0,droppedSamples:0,samples:[]};
  report.cases.push(record);
  pending={record,captureBefore,identity:sample.identity.slice(),players,peer};
  owned=true;
  if(kind==='cancel-redown-move'){
   core.sdl_touch_cancel();
   const afterCancel=touch();++record.exactEightWordChecks;
   record.afterCancelWords=afterCancel;
   check(same(afterCancel,reference.words),'cancel invalidated the complete world cache');
   check(controls()[6]===0,'native cancel left the old drag active');
  }
  ++activePointer;
  core.sdl_touch(0,activePointer,fingerStart,f32(.5));
  core.sdl_touch(1,activePointer,fingerEnd,f32(.5));
  lastFingerX=fingerEnd;lastTargetX=expected.targetX;
  const after=touch();++record.exactEightWordChecks;
  check(same(after,reference.words),'pointer event changed the complete eight-word source');
  record.nativeAfter=controls();check(record.nativeAfter[6]===1,'real GameHost did not start the drag');
  addSample(record,sample,after,'injected-after-native-return');
 }
 function observeReturnedCallback(sequence,result){
  if(closed||!enabled||sequence!==callbackSequence)return;
  if(result){failure('native-callback-failed',result);return;}
  if(terminal)return;
  const sample=snapshot();syncWindow(sample);
  if(pending){
   if(window?.id===pending.record.windowId&&sample.correcting){
    const actual=touch();++pending.record.exactEightWordChecks;
    if(!same(actual,pending.record.completeWords)){
     failure('eight-word-cache-changed-inside-correction',{expected:pending.record.completeWords,actual});return;
    }
    addSample(pending.record,sample,actual,'continued-correction');
   }
   verifyCapture(sample);
   if(terminal||pending)return;
  }
  if(!sample.correcting){
   if(sample.net[9]&&sample.net[3]>=0){
    const actual=touch();
    lastStable={words:actual,identity:sample.identity.slice(),observation:brief(sample)};
   }
   return;
  }
  if(!window?.reference){skip('no-observed-complete-frontier');return;}
  if(sample.net[2]>=sample.smooth[3]||sample.smooth[1]<=window.reference.observation.recoveryYields){
   skip('not-a-new-yielded-recovery-gap');return;
  }
  if(sample.transport[1]!==1||sample.transport[2]!==1){skip('not-an-open-real-network-session');return;}
  if(!same(sample.identity,window.reference.identity)){skip('lifecycle-changed-at-recovery');return;}
  if(!movable(window.reference.words)){skip('complete-frontier-does-not-allow-touch-movement');return;}
  if(successes===1&&report.cases[0].windowId===window.id){skip('second-case-requires-a-different-recovery-window');return;}
  const old=history();
  if(old.playerState!==1||!Number.isFinite(old.x)||!Number.isFinite(old.y)){
   skip('historical-player-not-a-clean-position-negative',old);return;
  }
  const actual=touch();
  inject(sample,actual,old);
 }
 function warmupSecondCase(sample,actual){
  if(successes!==1||pending||terminal||sample.correcting||!movable(actual)||warmupFrame===sample.net[2])return;
  warmupFrame=sample.net[2];
  if(!controls()[6]){
   ++activePointer;lastFingerX=f32(.5);lastTargetX=decode(actual).x;
   core.sdl_touch(0,activePointer,lastFingerX,f32(.5));++report.warmup.begins;
  }
  if(lastTargetX>=128)warmupDirection=-1;else if(lastTargetX<=-128)warmupDirection=1;
  const nextFinger=f32(lastFingerX+warmupDirection*3/640);
  const next=expectedTarget(lastTargetX,decode(actual).y,lastFingerX,nextFinger);
  core.sdl_touch(1,activePointer,nextFinger,f32(.5));
  lastFingerX=nextFinger;lastTargetX=next.targetX;
  ++report.warmup.events;++report.warmup.moves;
  if(report.warmup.samples.length<12)report.warmup.samples.push({nextFrame:sample.net[2],pointerX:nextFinger,targetX:lastTargetX});
 }
 function prepare(){
  if(!enabled||closed||terminal)return;
  try{
   ++report.prepareCalls;
   const sample=snapshot();syncWindow(sample);
   if(sample.correcting)return;
   if(sample.net[9]&&sample.net[3]>=0){
    const actual=touch();
    lastStable={words:actual,identity:sample.identity.slice(),observation:brief(sample)};
    warmupSecondCase(sample,actual);
   }
  }catch(error){failure('prepare-observation-failed',String(error));}
 }
 function afterCallback(result=0){
  if(!enabled||closed||terminal)return;
  const sequence=++callbackSequence,epoch=microtaskEpoch;
  report.callbacks=sequence;
  queueMicrotask(()=>{
   if(closed||epoch!==microtaskEpoch)return;
   try{
    if(sequence>maxCallbacks){uncovered('native-callback-bound-exhausted');return;}
    observeReturnedCallback(sequence,result);
   }catch(error){failure('callback-observation-failed',String(error));}
  });
 }
 function cleanup(){
  if(!enabled||cleaned)return report.cleanup;
  check(hostStopped,'cleanup requires finish({nativeStopped:true}) after sdl_loop_stop');
  try{core.sdl_touch_cancel();report.cleanup.canceled=true;}
  catch(error){failure('native-touch-cleanup-failed',String(error));}
  for(const [name,pointer] of [['freedTouchBuffer',touchPointer],['freedPacketBuffer',packetPointer]]){
   if(pointer){
    try{core.files_free(pointer);report.cleanup[name]=true;}
    catch(error){failure('observation-buffer-release-failed',{name,error:String(error)});}
   }else report.cleanup[name]=true;
  }
  touchPointer=packetPointer=0;owned=false;cleaned=true;
  report.cleanup.complete=report.cleanup.canceled&&report.cleanup.freedTouchBuffer&&report.cleanup.freedPacketBuffer;
  return report.cleanup;
 }
 function finish({reason=null,nativeStopped=false}={}){
  if(report.done)return report;
  closed=true;++microtaskEpoch;hostStopped=nativeStopped===true;
  report.cleanup.nativeStopped=hostStopped;
  if(reason)failure('runner-stopped-with-error',String(reason));
  if(enabled&&!hostStopped)failure('finish-called-before-native-stop');
  if(pending){
   if(pending.record.status==='pending'){pending.record.status='not-covered';pending.record.reason='stopped-before-first-new-capture';}
   pending=null;
  }
  if(enabled&&hostStopped)cleanup();
  report.coverage.newDownMove=report.cases.some(value=>value.kind===CASE_KINDS[0]&&value.status==='pass');
  report.coverage.cancelRedownMove=report.cases.some(value=>value.kind===CASE_KINDS[1]&&value.status==='pass');
  report.coverage.distinctRecoveryWindows=report.cases.length===2&&report.cases[0].windowId!==report.cases[1].windowId;
  report.coverage.completeEightWords=successes===2&&report.cases.every(value=>value.exactEightWordChecks>0);
  report.coverage.actualFirstNewCapture=successes===2&&report.cases.every(value=>value.capture?.matchesExpected===true);
  report.status=report.failures.length?'fail':enabled&&successes===2&&report.cleanup.complete?'pass':'not-covered';
  report.passed=report.status==='pass';report.done=true;report.elapsedMs=enabled?now():0;
  if(enabled&&successes<2&&!report.failures.length)report.notes.push({reason:'two-eligible-distinct-recovery-windows-not-completed',completed:successes});
  return report;
 }
 if(enabled){
  check(core&&app&&typeof netStatus==='function'&&typeof transportStatus==='function'&&typeof status==='function','active native API required');
  check(Number.isInteger(seat)&&seat>=0&&seat<3&&inputDelay===0,'seat / zero input delay contract');
  check(Number.isFinite(minHistoryDistancePx)&&minHistoryDistancePx>=1&&minHistoryDistancePx<=64,'history discrimination bound');
  check(Number.isSafeInteger(maxCallbacks)&&maxCallbacks>=1&&maxCallbacks<=72000&&
   Number.isSafeInteger(maxCaptureCallbacks)&&maxCaptureCallbacks>=1&&maxCaptureCallbacks<=1200,'callback bounds');
  for(const name of ['application_touch_state','multiplayer_smoothness_status','multiplayer_packet_build',
   'sdl_game_status','sdl_touch','sdl_touch_cancel','sdl_touch_mode','sdl_touch_options','files_allocate','files_free'])
   check(typeof core[name]==='function',name+' export required');
  const initial=status();check(initial[2]===seat&&initial[1]>=2&&initial[1]<=3,'actual local seat/player count');
  report.startedAtPerformanceMs=clock();
  touchPointer=core.files_allocate(32)>>>0;check(touchPointer!==0,'touch observation allocation');
  packetPointer=core.files_allocate(PACKET_BYTES)>>>0;
  if(!packetPointer){core.files_free(touchPointer);touchPointer=0;check(false,'packet observation allocation');}
  // This correctness fixture uses the same unlimited direct movement mode as
  // the 120 Hz load fixture. Serial baselines are never reset.
  core.sdl_touch_mode(0);core.sdl_touch_options(1,1,1);
 }
 return {prepare,afterCallback,finish,cleanup,
  get ownsMovement(){return enabled&&!closed&&owned;},
  get holdMovement(){return enabled&&!closed&&(!!pending||terminal);},
  get done(){return closed||terminal;},
  get enabled(){return enabled;}};
}
