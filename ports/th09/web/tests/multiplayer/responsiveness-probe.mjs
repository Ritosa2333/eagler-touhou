// Test-only, wall-clock hosted-key stimulus and input observation.
// Inject the function itself with page.addInitScript(installResponsivenessProbe).
// It uses public Runtime exports and never draws, advances or rewinds a world.
export function installResponsivenessProbe() {
 const limits={intents:8192,captures:65536,errors:32};
 const keys=[[29,256],[44,1],[42,4],[203,64],[205,128]];
 const intents=[],captures=[],errors=[],byFrame=new Map(),pendingPresent=[];
 let core=null,side=0,timer=null,running=false,startedAt=null,stoppedAt=null;
 let nextScheduled=0,currentSubmitted=null,inputDelay=0,firstFreshFrame=0,nextFreshFrame=0;
 let presentHead=0,forwardFrontier=0,lastPresentation=0,lastPhase=null;
 const counters={packets:0,duplicateFrames:0,preStartFrames:0,captureGaps:0,
  inputMismatches:0,changedHistory:0,observationErrors:0,overflow:false};

 const age=now=>Math.max(0,now-startedAt);
 function note(message) {
  ++counters.observationErrors;
  if(errors.length<limits.errors)errors.push(message);
 }
 function fail(message) {
  counters.overflow=true;note(message);running=false;
  stoppedAt=performance.now();clearTimeout(timer);timer=null;
  throw Error(message);
 }
 function boundedPush(list,value,limit) {
  if(list.length>=limit)fail('TH09 responsiveness probe record limit exceeded');
  list.push(value);return value;
 }
 function maskAt(ms) {
  const cycle=Math.floor(ms/360)+side;
  return (ms<2200?256:0)|(ms%2100<1500?1:0)|(ms%3000<750?4:0)|
   (cycle%4===0?64:0)|(cycle%4===2?128:0);
 }
 function nextEdge(ms) {
  const shot=Math.floor(ms/2100)*2100,focus=Math.floor(ms/3000)*3000;
  return Math.min((Math.floor(ms/360)+1)*360,
   ms<shot+1500?shot+1500:shot+2100,
   ms<focus+750?focus+750:focus+3000,ms<2200?2200:Infinity);
 }
 function addIntent(scheduledAtMs) {
  const mask=maskAt(scheduledAtMs),prior=intents.at(-1);
  if(prior&&prior.mask===mask)return null;
  return boundedPush(intents,{id:intents.length,scheduledAtMs,mask,
   submissionStartedAtMs:null,submittedAtMs:null,previousSubmittedMask:null,
   changedSubmittedMask:null,supersededAtMs:null,captureFrame:null,
   firstWireAtMs:null,presentationObservedAtMs:null,presentationFrontier:null,
   presentationCount:null,phase:null},limits.intents);
 }
 function collectDue(now) {
  const elapsed=age(now);let latest=null;
  // If a long task swallowed several transitions, retain every planned intent.
  // Only the state still current when the timer actually runs is submitted.
  while(nextScheduled<=elapsed) {
   latest=addIntent(nextScheduled)||latest;
   nextScheduled=nextEdge(nextScheduled);
  }
  return latest;
 }
 function submit(intent) {
  const began=performance.now(),prior=currentSubmitted;
  intent.submissionStartedAtMs=age(began);
  intent.previousSubmittedMask=prior?.mask??null;
  intent.changedSubmittedMask=prior?prior.mask!==intent.mask:null;
  if(prior)prior.supersededAtMs=age(began);
  for(const [scan,bit] of keys)core._th09_key(scan,+!!(intent.mask&bit));
  intent.submittedAtMs=age(performance.now());currentSubmitted=intent;
 }
 function wake() {
  if(!running)return;
  const latest=collectDue(performance.now());if(latest)submit(latest);
  timer=setTimeout(wake,Math.max(1,startedAt+nextScheduled-performance.now()));
 }
 function distribution(values) {
  const sorted=values.slice().sort((a,b)=>a-b);
  if(!sorted.length)return {count:0,p50:null,p95:null,p99:null,max:null};
  const percentile=q=>sorted[Math.min(sorted.length-1,Math.floor(sorted.length*q))];
  return {count:sorted.length,p50:percentile(.5),p95:percentile(.95),
   p99:percentile(.99),max:sorted.at(-1)};
 }
 function outcome(intent) {
  if(intent.submittedAtMs===null)return 'not-submitted';
  if(intent.captureFrame===null)return 'submitted-not-captured';
  return intent.presentationObservedAtMs===null?'captured-not-presented':'presented';
 }
 const probe={
  start(runtime,player) {
   if(startedAt!==null)throw Error('TH09 responsiveness probe is single-session');
   if(!runtime||![runtime._th09_key,runtime._th09_rollback_info,
    runtime._th09_adonis_info,runtime._th09_game_metrics].every(f=>typeof f==='function')||
    !runtime.HEAPU32||!Number.isInteger(player)||player<0||player>1)
    throw Error('TH09 responsiveness probe requires the public Runtime ABI and seat 0/1');
   core=runtime;side=player;
   const at=core._th09_rollback_info()/4,ad=core._th09_adonis_info()/4;
   inputDelay=core.HEAPU32[ad+1];forwardFrontier=core.HEAPU32[at];
   // Captures are monotonic physical-input slots; wire frames include D.
   // Retransmitted input captured before start must not satisfy a new intent.
   firstFreshFrame=nextFreshFrame=core.HEAPU32[at+4]+inputDelay;
   lastPresentation=core.HEAPU32[core._th09_game_metrics()/4+9];
   startedAt=performance.now();running=true;wake();
   return {startedAt,side,inputDelay,firstFreshFrame};
  },
  onLocalInput(data,now=performance.now()) {
   if(!running)return;
   let bytes;
   if(ArrayBuffer.isView(data))bytes=new Uint8Array(data.buffer,data.byteOffset,data.byteLength);
   else if(data instanceof ArrayBuffer)bytes=new Uint8Array(data);
   else {note('Input observation was not a byte buffer');return;}
   if(bytes[0]===0xe7)bytes=bytes.subarray(2);
   if(bytes.length<6||bytes[0]!==69||bytes[2]!==78||bytes[3]!==80||bytes[5]!==1)return;
   if(bytes.length<44||bytes[6]!==side||bytes.length!==44+bytes[42]*12) {
    note('Invalid local input packet schema/seat');return;
   }
   ++counters.packets;
   const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);
   const first=view.getUint32(32,true),count=bytes[42];
   for(let i=0;i<count;++i) {
    const frame=first+i,offset=44+i*12,buttons=view.getUint16(offset,true);
    if(frame<firstFreshFrame){++counters.preStartFrames;continue;}
    const prior=byFrame.get(frame);
    if(prior) {
     ++counters.duplicateFrames;
     if(prior.buttons!==buttons){++counters.changedHistory;note('Local input history changed at '+frame);}
     continue;
    }
    const contiguous=frame===nextFreshFrame;
    if(!contiguous){++counters.captureGaps;note('First-wire capture gap/order at '+frame+' expected '+nextFreshFrame);}
    nextFreshFrame=Math.max(nextFreshFrame,frame+1);
    const analogNeutral=view.getUint16(offset+2,true)===0&&
     view.getFloat32(offset+4,true)===0&&view.getFloat32(offset+8,true)===0;
    const matches=!!currentSubmitted&&currentSubmitted.mask===buttons&&analogNeutral;
    if(!matches){++counters.inputMismatches;note('Wire input differs from submitted hosted keys at '+frame);}
    const record=boundedPush(captures,{frame,buttons,firstWireAtMs:age(now),
     intentId:matches?currentSubmitted.id:null,contiguous,analogNeutral},limits.captures);
    byFrame.set(frame,record);
    if(matches&&contiguous&&currentSubmitted.captureFrame===null) {
     currentSubmitted.captureFrame=frame;currentSubmitted.firstWireAtMs=age(now);
     pendingPresent.push(currentSubmitted);
    }
   }
  },
  onFrame(stats,presentationCount,phase,now=performance.now()) {
   if(!running)return;
   if(!stats||stats.length<8||!Number.isInteger(stats[0])||
    !Number.isInteger(presentationCount)||!Number.isFinite(now)) {
    note('Invalid frame observation');return;
   }
   const next=stats[0],newForward=next>forwardFrontier;
   if(presentationCount<lastPresentation)note('Presentation counter regressed during one probe session');
   if(newForward&&presentationCount>lastPresentation) {
    // Frame() points AFTER the last simulated frame. A capture for F can first
    // participate when next>F. Repeated Draw and historical-only work do not
    // become additional input responses. Multiple Presents in one callback
    // have only this callback-end observation time, not individual GL times.
    while(presentHead<pendingPresent.length&&pendingPresent[presentHead].captureFrame<next) {
     const intent=pendingPresent[presentHead++];
     intent.presentationObservedAtMs=age(now);intent.presentationFrontier=next;
     intent.presentationCount=presentationCount;intent.phase=phase;
    }
   }
   forwardFrontier=Math.max(forwardFrontier,next);lastPresentation=presentationCount;lastPhase=phase;
  },
  stop(now=performance.now()) {
   if(!running)return;
   clearTimeout(timer);timer=null;
   // Do not erase transitions whose timer never ran before measurement ended.
   collectDue(now);running=false;stoppedAt=now;core=null;
  },
  snapshot() {
   const end=stoppedAt??(startedAt===null?null:performance.now());
   const counts={planned:intents.length,submitted:0,captured:0,presented:0,
    notSubmitted:0,submittedNotCaptured:0,capturedNotPresented:0,sameMaskResubmissions:0};
   const rows=intents.map(intent=>{
    const status=outcome(intent);
    if(intent.submittedAtMs!==null)++counts.submitted;
    if(intent.captureFrame!==null)++counts.captured;
    if(intent.presentationObservedAtMs!==null)++counts.presented;
    if(status==='not-submitted')++counts.notSubmitted;
    if(status==='submitted-not-captured')++counts.submittedNotCaptured;
    if(status==='captured-not-presented')++counts.capturedNotPresented;
    if(intent.changedSubmittedMask===false)++counts.sameMaskResubmissions;
    return {...intent,status,pendingAgeAtEndMs:end===null||status==='presented'?null:age(end)-intent.scheduledAtMs,
     scheduleToSubmitMs:intent.submittedAtMs===null?null:intent.submittedAtMs-intent.scheduledAtMs,
     submitToFirstWireMs:intent.firstWireAtMs===null?null:intent.firstWireAtMs-intent.submittedAtMs,
     firstWireToPresentationObservedMs:intent.presentationObservedAtMs===null?null:
      intent.presentationObservedAtMs-intent.firstWireAtMs,
     scheduleToPresentationObservedMs:intent.presentationObservedAtMs===null?null:
      intent.presentationObservedAtMs-intent.scheduledAtMs};
   });
   // Distributions are conditional. Counts and null-valued rows are mandatory:
   // a swallowed intent is not a zero-latency response and is never discarded.
   const changed=rows.filter(r=>r.changedSubmittedMask!==false);
   const metric=name=>distribution(changed.filter(r=>r[name]!==null).map(r=>r[name]));
   return {schema:'th09/responsiveness-probe/1',startedAt,stoppedAt,running,
    durationMs:end===null?0:age(end),side,inputDelay,firstFreshFrame,
    forwardFrontier,lastPresentation,lastPhase,limits:{...limits},counts,
    observations:{...counters},errors:errors.slice(),
    valid:startedAt!==null&&!counters.overflow&&!counters.observationErrors,
    scope:'Wall-clock synthetic intent -> hosted-key submission -> first observed local wire capture -> observed callback completing a new-frontier renderer submission. Not hardware input, exact pixel response, physical scanout, or remote acknowledgement.',
    measurementNotes:[
     'Timer lateness is included. Planned intents superseded before submission/capture remain as null-valued rows.',
     'First wire observation follows native capture/serialization; it is not an instrumented sampling timestamp.',
     'Presentation time is the supplied onFrame callback observation; multiple logical Presents in one callback cannot be individually timed.',
     'Matching input and a new frontier establish input participation only; collisions, menus or game state may cause no visible motion.',
     'Conditional latency distributions exclude same-mask resubmissions; coverage counts and all raw intents must be reported beside them.',
     'The initial intent is included; stop does not inject a key release or advance the simulation.'
    ],
    conditionalLatencyMs:{scheduleToSubmit:metric('scheduleToSubmitMs'),
     submitToFirstWire:metric('submitToFirstWireMs'),
     firstWireToPresentationObserved:metric('firstWireToPresentationObservedMs'),
     scheduleToPresentationObserved:metric('scheduleToPresentationObservedMs')},
    intents:rows,captures:captures.map(row=>({...row}))};
  }
 };
 globalThis.responsivenessProbe=probe;
}
