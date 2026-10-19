// Opt-in, owned-Chrome scheduling evidence. Raw CDP events never leave this helper.
import { readFileSync } from 'node:fs';
import { createHash } from 'node:crypto';
import { performance } from 'node:perf_hooks';
import { fileURLToPath } from 'node:url';

export const CHROME_SCHEDULER_TRACE_PROFILES=Object.freeze({
  pipeline:Object.freeze(['toplevel','renderer.scheduler','cc','viz','gpu','blink.user_timing']),
  tasks:Object.freeze(['toplevel','renderer.scheduler','blink.user_timing']),
  graphics:Object.freeze(['gpu','viz','blink.user_timing'])
});
export const CHROME_SCHEDULER_TRACE_CATEGORIES=CHROME_SCHEDULER_TRACE_PROFILES.pipeline;
export const CHROME_SCHEDULER_TRACE_BUFFER_HIGH_WATER=0.80;
export const CHROME_SCHEDULER_TRACE_LIMITS=Object.freeze({events:350000,sanitizedBytes:96*1024*1024,bufferUsage:128,marks:8,errors:32,
  names:2048,threads:1024,durationMs:90000,commandTimeoutMs:5000,completionTimeoutMs:30000,pageTimeoutMs:5000});
const STAGES=Object.freeze(['armed','first-active','capture-stop']);
const MARKS=Object.freeze(Object.fromEntries(STAGES.map(x=>[x,'th09-sched-'+x])));
const PHASES=new Set(['B','E','X','I','i','C','S','T','F','b','n','e','s','t','f','M','R','N','O','D']);
const NAMES=new Set([...Object.values(MARKS),'process_name','thread_name',
  'ThreadControllerImpl::RunTask','ThreadControllerImpl::DoWork','ThreadControllerImpl::RunTaskImpl',
  'ThreadControllerWithMessagePumpImpl::DoWork','SequenceManagerImpl::SelectNextTask','TaskQueueManager::ProcessTaskFromWorkQueue',
  'RendererSchedulerImpl::RunIdleTask','RendererSchedulerImpl::UpdatePolicy','RendererSchedulerImpl::OnTaskCompleted',
  'MainThreadSchedulerImpl::UpdatePolicy','MainThreadSchedulerImpl::OnTaskCompleted','RunTask','DoWork','ThreadPool_RunTask',
  'Scheduler::BeginFrame','Scheduler::BeginImplFrame','Scheduler::OnBeginImplFrameDeadline',
  'BeginFrame','BeginMainFrame','ReceiveBeginFrame','BeginFrameSource::OnBeginFrame','ExternalBeginFrameSource::OnBeginFrame',
  'ProxyMain::BeginMainFrame','ProxyImpl::ScheduledActionSendBeginMainFrame','ProxyImpl::ScheduledActionDrawIfPossible',
  'LayerTreeHost::DoUpdateLayers','LayerTreeHostImpl::DrawLayers','LayerTreeHostImpl::PrepareToDraw',
  'WebViewImpl::BeginFrame','WebFrameWidgetImpl::BeginMainFrame','LocalFrameView::UpdateLifecyclePhases',
  'Display::DrawAndSwap','Display::DidReceiveSwapBuffersAck','Display::OnBeginFrame','DisplayScheduler::OnBeginFrame',
  'DisplayScheduler::DrawAndSwap','DisplayScheduler::OnBeginFrameDeadline','SkiaRenderer::DrawFrame','DirectRenderer::DrawFrame',
  'SkiaOutputSurfaceImplOnGpu::SwapBuffers','GpuCommandBufferStub::OnAsyncFlush','GpuCommandBufferStub::OnWaitSyncToken',
  'CommandBufferHelper::WaitForTokenInRange','GLES2DecoderImpl::DoCommands','RasterDecoderImpl::DoCommands',
  'GpuScheduler::RunNextTask','GpuChannel::OnMessageReceived','GpuChannelMessageFilter::OnMessageReceived',
  'SwapBuffers','SwapBuffersComplete','WaitForSwap','WaitForGPU','DrawFrame']);
const PROCESS_NAMES=new Set(['Browser','Renderer','GPU Process','Gpu','Utility','Service: audio.mojom.AudioService']);
const THREAD_NAMES=new Set(['CrBrowserMain','CrRendererMain','CrGpuMain','Chrome_IOThread','Chrome_ChildIOThread',
  'Compositor','VizCompositorThread','CompositorGpuThread','GpuMemoryThread','GpuWatchdog','AudioThread',
  'ThreadPoolForegroundWorker','ThreadPoolBackgroundWorker','ThreadPoolServiceThread','VideoCaptureThread',
  'CompositorTileWorker1','CompositorTileWorker2','CompositorTileWorker3','CompositorTileWorker4']);
const STOP_REASONS=new Set(['capture-stop','failure','deadline','local-overflow','chrome-buffer-full','buffer-high-water','start-failed']);
const ERROR_NAMES=new Set(['Error','TypeError','RangeError','TimeoutError','ProtocolError']);
const RESERVED_SNAPSHOT_BYTES=64*1024;
const hash=value=>createHash('sha256').update(value).digest('hex');
const finite=x=>typeof x==='number'&&Number.isFinite(x);
const integer=x=>Number.isSafeInteger(x)&&x>=0;
const idValue=x=>integer(x)||(typeof x==='string'&&/^(?:0x)?[0-9a-fA-F]{1,16}$/.test(x));

// Serialized by Playwright. No imports, game calls, heartbeat, or arbitrary mark names.
export function chromeSchedulerTracePageOperation(action){
  const key='chromeSchedulerTraceProbe';
  if(action==='install'){
    if(window[key])return {version:1,installConflict:true};
    const marks=[],names={armed:'th09-sched-armed','first-active':'th09-sched-first-active','capture-stop':'th09-sched-capture-stop'};
    let invalidCalls=0,markErrors=0;
    const api=Object.freeze({
      mark(stage){
        if(!Object.hasOwn(names,stage)){++invalidCalls;return null;}
        const old=marks.find(x=>x.stage===stage);if(old)return {...old};
        try{const before=performance.now();performance.mark(names[stage]);const after=performance.now();
          const row={stage,before,after,timeOrigin:performance.timeOrigin};marks.push(row);return {...row};}
        catch{++markErrors;return null;}
      },
      snapshot(){return {version:1,marks:marks.map(x=>({...x})),invalidCalls,markErrors};}
    });
    Object.defineProperty(window,key,{value:api,configurable:true,writable:false});api.mark('armed');
  }else if(action==='capture-stop')window[key]?.mark('capture-stop');
  return window[key]?.snapshot()??null;
}

function makeCollector(limits,now,onLimit,selectedCategories){
  const events=[],bufferUsage=[],marks=[],errors=[],unknownNames=new Map(),threads=new Map();
  const categorySet=new Set(selectedCategories);
  const categoryCounts=Object.fromEntries(selectedCategories.map(x=>[x,0]));
  const counts={batches:0,rawEvents:0,retainedEvents:0,droppedEvents:0,droppedFields:0,malformedEvents:0,
    retainedDataJsonBytes:0,unknownEventNames:0,errors:0};
  const overflow={events:0,bytes:0,bufferUsage:0,marks:0,errors:0,names:0,threads:0,firstAt:null,firstTraceTs:null};
  let limited=false;
  const overflowAt=(kind,ts,n=1)=>{overflow[kind]+=n;if(overflow.firstAt===null){overflow.firstAt=now();overflow.firstTraceTs=finite(ts)?ts:null;}
    if(!limited){limited=true;queueMicrotask(()=>onLimit('local-overflow'));}};
  function retain(row,kind,ts){
    const bytes=Buffer.byteLength(JSON.stringify(row))+128; // Counter digit growth and collection separators.
    if(RESERVED_SNAPSHOT_BYTES+counts.retainedDataJsonBytes+bytes>limits.sanitizedBytes){overflowAt('bytes',ts);return false;}
    counts.retainedDataJsonBytes+=bytes;return true;
  }
  function named(kind,name){
    const digest=hash(name),key=kind+':'+digest,old=unknownNames.get(key);
    if(old)++old.count;
    else if(unknownNames.size>=limits.names)overflowAt('names');
    else{const row={kind,sha256:digest,count:1};if(retain(row,'names'))unknownNames.set(key,row);}
    return digest;
  }
  function error(code,error){
    ++counts.errors;
    const row={code,at:now()};
    if(ERROR_NAMES.has(error?.name))row.name=error.name;
    if(errors.length>=limits.errors){overflowAt('errors');return;}
    if(retain(row,'errors'))errors.push(row);
  }
  function addThread(pid,tid,cats){
    const key=pid+':'+tid;let row=threads.get(key);
    if(!row){if(threads.size>=limits.threads){overflowAt('threads');return;}
      row={pid,tid,count:0,categories:Object.fromEntries(selectedCategories.map(c=>[c,0]))};
      if(!retain(row,'threads'))return;threads.set(key,row);}
    ++row.count;for(const cat of cats)++row.categories[cat];
  }
  function event(raw){
    if(!raw||typeof raw!=='object'||typeof raw.name!=='string'||typeof raw.cat!=='string'||
      !integer(raw.pid)||!integer(raw.tid)||!finite(raw.ts)||raw.ts<0||!PHASES.has(raw.ph)){
      ++counts.malformedEvents;++counts.droppedEvents;return;
    }
    const parts=raw.cat.split(','),cats=selectedCategories.filter(c=>parts.includes(c));
    const metadata=raw.ph==='M'&&['process_name','thread_name'].includes(raw.name);
    for(const c of parts)if(!categorySet.has(c)&&c!=='__metadata')named('category',c);
    if(!cats.length&&!metadata){++counts.droppedEvents;return;}
    const row={cat:metadata?'__metadata':cats.join(','),ph:raw.ph,ts:raw.ts,pid:raw.pid,tid:raw.tid};
    if(NAMES.has(raw.name))row.name=raw.name;
    else{row.nameHash=named('event',raw.name);++counts.unknownEventNames;}
    for(const key of ['dur','tts','tdur'])if(finite(raw[key])&&raw[key]>=0)row[key]=raw[key];
    for(const key of ['id','bind_id'])if(raw[key]!==undefined){if(idValue(raw[key]))row[key]=raw[key];else ++counts.droppedFields;}
    if(raw.id2!==undefined){
      if(raw.id2&&typeof raw.id2==='object'){
        for(const key of ['local','global'])if(raw.id2[key]!==undefined){
          if(idValue(raw.id2[key]))(row.id2??={})[key]=raw.id2[key];else ++counts.droppedFields;
        }
      }else ++counts.droppedFields;
    }
    for(const key of ['bp','scope','sname','cname','url','stack','args'])if(raw[key]!==undefined)++counts.droppedFields;
    if(metadata){
      const name=raw.args?.name,known=raw.name==='process_name'?PROCESS_NAMES:THREAD_NAMES;
      if(typeof name==='string')row.args=known.has(name)?{name}:{nameHash:named(raw.name==='process_name'?'process':'thread',name)};
    }
    if(events.length>=limits.events){overflowAt('events',raw.ts);++counts.droppedEvents;return;}
    if(!retain(row,'events',raw.ts)){++counts.droppedEvents;return;}
    events.push(row);++counts.retainedEvents;
    for(const c of cats)++categoryCounts[c];addThread(raw.pid,raw.tid,cats);
  }
  function data(message){
    ++counts.batches;
    if(!Array.isArray(message?.value)){error('malformed-data-collected');return;}
    counts.rawEvents+=message.value.length;
    for(let i=0;i<message.value.length;++i){
      if(limited){counts.droppedEvents+=message.value.length-i;overflowAt('events',message.value[i]?.ts,message.value.length-i);break;}
      event(message.value[i]);
    }
  }
  function buffer(message){
    const row={at:now()};for(const key of ['percentFull','eventCount','value'])if(finite(message?.[key]))row[key]=message[key];
    if(bufferUsage.length>=limits.bufferUsage)overflowAt('bufferUsage');else if(retain(row,'bufferUsage'))bufferUsage.push(row);
    // A rounded value below 1 can already be a saturated recordUntilFull plateau.
    const fraction=Math.max(finite(row.percentFull)?row.percentFull:0,finite(row.value)?row.value:0);
    if(fraction>=1)onLimit('chrome-buffer-full',row);
    else if(fraction>=CHROME_SCHEDULER_TRACE_BUFFER_HIGH_WATER)onLimit('buffer-high-water',row);
  }
  function page(state){
    if(!state||state.version!==1||state.installConflict||!Array.isArray(state.marks)){error('page-marker-state-unavailable');return;}
    if(state.invalidCalls||state.markErrors)error('page-marker-call-error');
    for(const m of state.marks){
      if(!STAGES.includes(m?.stage)||!finite(m.before)||!finite(m.after)||m.after<m.before||!finite(m.timeOrigin)){
        error('invalid-page-marker');continue;
      }
      const row={stage:m.stage,name:MARKS[m.stage],before:m.before,after:m.after,timeOrigin:m.timeOrigin};
      const prior=marks.find(x=>x.stage===row.stage);
      if(prior){if(JSON.stringify(prior)!==JSON.stringify(row))error('changed-page-marker');continue;}
      if(marks.length>=limits.marks)overflowAt('marks');else if(retain(row,'marks'))marks.push(row);
    }
  }
  return {events,bufferUsage,marks,errors,unknownNames,threads,categoryCounts,counts,overflow,data,buffer,page,error,
    hasOverflow:()=>Object.entries(overflow).some(([k,v])=>!k.startsWith('first')&&v>0)};
}

function mapClock(marks,events){
  const quantumMs=.1; // Conservatively expose the normal Chrome page-clock quantization allowance.
  const matches=marks.map(m=>{
    const candidates=events.filter(e=>e.name===m.name&&e.cat.split(',').includes('blink.user_timing'));
    const unique=[...new Map(candidates.map(e=>[[e.pid,e.tid,e.ts].join(':'),e])).values()];
    const t=unique.length===1?unique[0]:null;
    return {stage:m.stage,name:m.name,before:m.before,after:m.after,timeOrigin:m.timeOrigin,
      candidateCount:candidates.length,uniqueCandidateCount:unique.length,
      trace:t?{pid:t.pid,tid:t.tid,ts:t.ts}:null,
      offsetMs:t?[t.ts/1000-m.after-quantumMs,t.ts/1000-m.before+quantumMs]:null};
  });
  const paired=matches.filter(m=>m.trace),sameThread=paired.length>0&&paired.every(m=>m.trace.pid===paired[0].trace.pid&&m.trace.tid===paired[0].trace.tid);
  const sameOrigin=paired.length>0&&paired.every(m=>m.timeOrigin===paired[0].timeOrigin);
  const lo=paired.length?Math.max(...paired.map(m=>m.offsetMs[0])):null,hi=paired.length?Math.min(...paired.map(m=>m.offsetMs[1])):null;
  const span=paired.length>1?Math.max(...paired.map(m=>m.before))-Math.min(...paired.map(m=>m.after)):0;
  const mapped=paired.length>=2&&sameThread&&sameOrigin&&lo<=hi&&span>1&&matches.every(m=>m.uniqueCandidateCount===1);
  return {traceTimestampUnit:'microseconds',pageTimestampUnit:'milliseconds',mapping:'traceTs/1000 = pagePerformanceNow + offsetMs',
    timeOriginIsNotTraceOrigin:true,quantizationAllowanceMs:quantumMs,matches,matchedMarks:paired.length,
    sameThread,samePageTimeOrigin:sameOrigin,clockMapped:mapped,offsetIntersectionMs:lo===null?null:[lo,hi],
    maximumBracketWidthMs:marks.length?Math.max(...marks.map(m=>m.after-m.before)):null,
    distinctMarkSpanMs:span,renderer:mapped?{pid:paired[0].trace.pid,tid:paired[0].trace.tid}:null};
}

// The second argument injects clocks/files for pure mocks; limits may only be reduced.
export async function startChromeSchedulerTrace(options={},runtime={}){
  const enabled=options.enabled===true;
  const requestedProfile=options.profile??'pipeline';
  const profile=typeof requestedProfile==='string'&&Object.hasOwn(CHROME_SCHEDULER_TRACE_PROFILES,requestedProfile)?requestedProfile:null;
  const selectedCategories=profile?CHROME_SCHEDULER_TRACE_PROFILES[profile]:[];
  const limits={...CHROME_SCHEDULER_TRACE_LIMITS};
  for(const [key,value] of Object.entries(runtime.limits??{})){
    if(!Object.hasOwn(limits,key)||!integer(value)||value<1||value>limits[key])throw new Error('Invalid scheduler trace test limit');
    limits[key]=value;
  }
  const now=runtime.now??(()=>performance.now()),schedule=runtime.setTimer??setTimeout,cancel=runtime.clearTimer??clearTimeout;
  const read=runtime.readFile??readFileSync;
  const config={transferMode:'ReportEvents',bufferUsageReportingInterval:1000,tracingBackend:'chrome',
    traceConfig:{recordMode:'recordUntilFull',traceBufferSizeInKb:32768,enableSampling:false,enableSystrace:false,
      enableArgumentFilter:true,includedCategories:[...selectedCategories]}};
  const availability={available:false,reason:enabled?'not-started':'disabled',missingCategories:[],product:null};
  const sourceIdentity={before:null,after:null,stable:false};
  const capture={nodeStartedAt:null,nodeStopRequestedAt:null,nodeEndRequestedAt:null,nodeCompletedAt:null,nodeStoppedAt:null,
    reason:null,deadlineFired:false,chromeBufferFull:false,bufferHighWater:false,
    bufferHighWaterThreshold:CHROME_SCHEDULER_TRACE_BUFFER_HIGH_WATER,firstBufferHighWater:null,censored:false,
    tracingCompleteReceived:false,dataLossOccurred:null,endRequests:0};
  let status=enabled?'initializing':'disabled',session,started=false,pageInstalled=false,deadlineTimer,finalSnapshot,stopPromise,endPromise;
  let resolveComplete;const completePromise=new Promise(resolve=>{resolveComplete=resolve;});
  const c=makeCollector(limits,now,(reason,bufferRow)=>{
    if(reason==='chrome-buffer-full')capture.chromeBufferFull=true;
    if(reason==='chrome-buffer-full'||reason==='buffer-high-water'){
      capture.bufferHighWater=true;capture.censored=true;
      capture.firstBufferHighWater??={...bufferRow};
    }
    if(started)void stop(reason);
  },selectedCategories);
  function identities(){
    const result={};
    for(const [name,path] of [['helper',fileURLToPath(import.meta.url)],['harness',options.sourceFiles?.harness]]){
      try{const b=read(path);result[name]={bytes:b.length,sha256:hash(b)};}
      catch(error){result[name]=null;c.error('source-read-'+name,error);}
    }
    return result;
  }
  function sourceAfter(){
    if(!sourceIdentity.before)return;
    sourceIdentity.after=identities();sourceIdentity.stable=['helper','harness'].every(name=>
      sourceIdentity.before[name]&&sourceIdentity.after[name]&&sourceIdentity.before[name].sha256===sourceIdentity.after[name].sha256);
    if(!sourceIdentity.stable)c.error('source-identity-changed');
  }
  function snapshot(){
    if(finalSnapshot)return finalSnapshot;
    const clock=mapClock(c.marks,c.events);
    const collectionValid=started&&capture.tracingCompleteReceived&&capture.dataLossOccurred===false&&
      !capture.chromeBufferFull&&!capture.bufferHighWater&&!c.hasOverflow()&&c.counts.errors===0&&c.counts.malformedEvents===0&&sourceIdentity.stable&&status==='stopped';
    const ordered=STAGES.every((stage,i)=>{
      const m=c.marks.find(x=>x.stage===stage),prev=i?c.marks.find(x=>x.stage===STAGES[i-1]):null;
      return m&&(i===0||prev&&m.before>=prev.after);
    });
    const allThree=STAGES.every(stage=>clock.matches.some(m=>m.stage===stage&&m.uniqueCandidateCount===1));
    return {version:1,enabled,status,profile,side:[0,1].includes(options.side)?options.side:null,
      scope:profile==='tasks'
        ?'Owned Chrome only; fixed task/scheduler/user-timing categories, without compositor or GPU categories. Task wall spans are not CPU self time; missing events do not prove idle. No V8, system, network, payload, screenshot, or raw trace storage.'
        :profile==='graphics'
        ?'Owned Chrome only; fixed GPU/viz/user-timing categories, without toplevel, renderer.scheduler or cc coverage. GPU/viz wall spans are not CPU self time, hardware GPU execution time, or physical scanout; missing events do not prove idle. No V8, system, network, payload, screenshot, or raw trace storage.'
        :'Owned Chrome only; post-open/post-CPU setup through active gameplay. Scheduler/GPU wall spans are not CPU self time, hardware GPU execution time, or physical scanout. No V8, system, network, payload, screenshot, or raw trace storage.',
      availability:{...availability},collectionValid,config,limits:{...limits},counts:{...c.counts},overflow:{...c.overflow},errors:c.errors,
      sourceIdentity,clock,marks:c.marks,bufferUsage:c.bufferUsage,capture:{...capture},events:c.events,
      coverage:{clockMapped:clock.clockMapped,allThreeMarks:allThree,marksOrdered:ordered,requestedCategories:[...selectedCategories],
        activeWindowCovered:collectionValid&&clock.clockMapped&&allThree&&ordered&&!capture.deadlineFired&&!capture.censored,
        categories:{...c.categoryCounts},schedulerObserved:selectedCategories.includes('toplevel')&&selectedCategories.includes('renderer.scheduler')
          ?c.categoryCounts['toplevel']>0&&c.categoryCounts['renderer.scheduler']>0:null,
        compositorObserved:selectedCategories.includes('cc')&&selectedCategories.includes('viz')?c.categoryCounts.cc>0&&c.categoryCounts.viz>0:null,
        gpuObserved:selectedCategories.includes('gpu')?c.categoryCounts.gpu>0:null,
        renderer:clock.renderer,threads:[...c.threads.values()],unknownNames:[...c.unknownNames.values()],
        limitation:'No events is not evidence of idle. Match the retained complete time window to existing rAF requests; a non-reproduced stall is not a repair.'},
      storage:{rawTraceWritten:false,reservedMetadataBytes:RESERVED_SNAPSHOT_BYTES,
        byteAccounting:'Compact JSON UTF-8 for retained data rows plus 128 bytes per row and 64 KiB metadata reservation; not pretty-printed file bytes or JavaScript heap usage.',
        unknownStrings:'Only SHA-256 and counts; all dynamic event arguments discarded except fixed allowlisted Chrome process/thread metadata.'}};
  }
  async function bounded(action,code,timeoutMs){
    let timer;
    try{
      const timeout=new Promise((_,reject)=>{timer=schedule(()=>{const e=new Error();e.name='TimeoutError';reject(e);},timeoutMs);});
      return {ok:true,value:await Promise.race([Promise.resolve().then(action),timeout])};
    }catch(error){c.error(code,error);return {ok:false};}
    finally{if(timer!==undefined)cancel(timer);}
  }
  const command=(method,params={})=>bounded(()=>session.send(method,params),'cdp-'+method.replaceAll('.','-'),limits.commandTimeoutMs);
  const onComplete=message=>{
    capture.tracingCompleteReceived=true;capture.dataLossOccurred=typeof message?.dataLossOccurred==='boolean'?message.dataLossOccurred:null;
    capture.nodeCompletedAt=now();resolveComplete();
  };
  async function cleanup(){
    if(deadlineTimer!==undefined){cancel(deadlineTimer);deadlineTimer=undefined;}
    if(session){
      for(const [name,fn] of [['Tracing.dataCollected',c.data],['Tracing.bufferUsage',c.buffer],['Tracing.tracingComplete',onComplete]])session.off(name,fn);
      await bounded(()=>session.detach(),'cdp-detach',limits.commandTimeoutMs);session=null;
    }
  }
  function requestEnd(){
    if(endPromise)return endPromise;
    if(!started)return Promise.resolve({ok:false});
    capture.nodeEndRequestedAt=now();++capture.endRequests;endPromise=command('Tracing.end');return endPromise;
  }
  function stop(reason='capture-stop'){
    if(finalSnapshot)return Promise.resolve(finalSnapshot);
    if(stopPromise)return stopPromise;
    const why=STOP_REASONS.has(reason)?reason:'failure';
    if(!started){status=enabled?'unavailable':'disabled';finalSnapshot=snapshot();return Promise.resolve(finalSnapshot);}
    capture.nodeStopRequestedAt=now();capture.reason=why;capture.censored||=why!=='capture-stop';status='stopping';
    stopPromise=(async()=>{
      try{
      // Deadline/end never waits for a busy page. Normal final marking is bounded by the deadline too.
      const pageRead=pageInstalled?bounded(()=>options.page.evaluate(chromeSchedulerTracePageOperation,
        ['capture-stop','failure'].includes(why)?'capture-stop':'snapshot'),'page-stop-snapshot',limits.pageTimeoutMs):null;
      if(pageRead&&['capture-stop','failure'].includes(why)){const p=await pageRead;if(p.ok)c.page(p.value);}
      const ended=await requestEnd();
      if(ended.ok&&!capture.tracingCompleteReceived)await bounded(()=>completePromise,'tracing-complete-timeout',limits.completionTimeoutMs);
      if(pageRead&&!['capture-stop','failure'].includes(why)){const p=await pageRead;if(p.ok)c.page(p.value);}
        sourceAfter();
      }catch(error){c.error('stop-internal-error',error);}
      finally{
        try{await cleanup();}catch(error){c.error('cleanup-internal-error',error);}
        capture.nodeStoppedAt=now();status='stopped';
      }
      finalSnapshot=snapshot();return finalSnapshot;
    })();
    return stopPromise;
  }
  const controller=Object.freeze({stop,snapshot});
  async function unavailable(reason){
    availability.reason=reason;sourceAfter();
    try{await cleanup();}catch(error){c.error('cleanup-internal-error',error);}
    status='unavailable';finalSnapshot=snapshot();return controller;
  }
  if(!enabled)return controller;
  if(profile===null)return unavailable('unsupported-profile');
  if(!Array.isArray(options.ownedBrowsers)||!options.ownedBrowsers.includes(options.browser))return unavailable('browser-not-owned');
  try{
    if(options.browser?.browserType()?.name()!=='chromium')return unavailable('browser-not-chromium');
    if(options.page?.context()?.browser()!==options.browser)return unavailable('page-not-owned');
  }catch(error){c.error('ownership-check-error',error);return unavailable('ownership-unavailable');}
  if(typeof options.sourceFiles?.harness!=='string'||!options.sourceFiles.harness)return unavailable('source-harness-required');
  sourceIdentity.before=identities();
  if(!sourceIdentity.before.helper||!sourceIdentity.before.harness)return unavailable('source-identity-unavailable');
  const made=await bounded(()=>options.browser.newBrowserCDPSession(),'browser-cdp-session',limits.commandTimeoutMs);
  if(!made.ok)return unavailable('browser-cdp-unavailable');session=made.value;
  const version=await command('Browser.getVersion');
  if(!version.ok||typeof version.value?.product!=='string'||!/^(?:HeadlessChrome|Chrome)\/\d+(?:\.\d+){0,3}$/.test(version.value.product))return unavailable('browser-not-chrome');
  availability.product=version.value.product;
  const categories=await command('Tracing.getCategories');
  if(!categories.ok||!Array.isArray(categories.value?.categories))return unavailable('categories-unavailable');
  availability.missingCategories=selectedCategories.filter(c=>!categories.value.categories.includes(c));
  if(availability.missingCategories.length)return unavailable('required-categories-missing');
  session.on('Tracing.dataCollected',c.data);session.on('Tracing.bufferUsage',c.buffer);session.on('Tracing.tracingComplete',onComplete);
  const began=await command('Tracing.start',config);
  if(!began.ok)return unavailable('tracing-start-failed');
  started=true;capture.nodeStartedAt=now();
  deadlineTimer=schedule(()=>{
    capture.deadlineFired=true;capture.censored=true;void requestEnd();void stop('deadline');
  },limits.durationMs);deadlineTimer?.unref?.();
  const installed=await bounded(()=>options.page.evaluate(chromeSchedulerTracePageOperation,'install'),'page-marker-install',limits.pageTimeoutMs);
  if(installed.ok){c.page(installed.value);pageInstalled=installed.value?.version===1&&!installed.value.installConflict;}
  if(stopPromise||capture.nodeEndRequestedAt!==null){availability.reason='stopped-during-initialization';await stop('start-failed');return controller;}
  if(!installed.ok||!pageInstalled||c.counts.errors){availability.reason='page-markers-unavailable';await stop('start-failed');return controller;}
  availability.available=true;availability.reason='ready';status='recording';return controller;
}
