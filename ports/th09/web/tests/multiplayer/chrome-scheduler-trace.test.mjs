// Pure mocks: no Chrome, network, processes, or trace files.
import test from 'node:test';
import assert from 'node:assert/strict';
import { EventEmitter } from 'node:events';
import { createContext, runInContext } from 'node:vm';
import { startChromeSchedulerTrace,CHROME_SCHEDULER_TRACE_CATEGORIES,CHROME_SCHEDULER_TRACE_PROFILES,CHROME_SCHEDULER_TRACE_BUFFER_HIGH_WATER,CHROME_SCHEDULER_TRACE_LIMITS } from './chrome-scheduler-trace.mjs';

const flush=async()=>{for(let i=0;i<20;++i)await Promise.resolve();};
function fixture(config={}){
  let at=0,pageAt=100,id=0,readCount=0,sessionCount=0,detachCount=0,gameChanges=0,sourceVersion=1;
  let resolveNativeStart,nativeTracing=false,browserClosed=false;
  const timers=new Map(),calls=[],raw=[],fail=new Set(config.fail??[]);
  const setTimer=(fn,ms)=>{const token={id:++id,unref(){}};timers.set(token,{fn,due:at+ms});return token;};
  const context=createContext({window:{},performance:{timeOrigin:1700000000000,
    now(){pageAt+=.025;return pageAt;},mark(name){raw.push({cat:'blink.user_timing',name,ph:'R',pid:10,tid:20,ts:(pageAt+.01+1000)*1000});}}});
  const secretError=()=>new Error('private https://fixture.invalid/?room=secret#token');
  class Session extends EventEmitter{
    async send(method,params){
      calls.push({method,params});if(fail.has(method))throw secretError();
      if(method==='Browser.getVersion')return {product:config.product??'Chrome/153.0.0.0',userAgent:'private-user-agent'};
      if(method==='Tracing.getCategories')return {categories:config.categories??[...CHROME_SCHEDULER_TRACE_CATEGORIES]};
      if(method==='Tracing.start'){
        if(config.lateStart)await new Promise(resolve=>{resolveNativeStart=resolve;});
        if(!browserClosed)nativeTracing=true;
      }
      if(method==='Tracing.end'){
        nativeTracing=false;
        if(config.transformEvents)config.transformEvents(raw);
        if(config.completionDelayMs){
          this.emit('Tracing.dataCollected',{value:raw.slice(0,1)});
          setTimer(()=>{
            this.emit('Tracing.dataCollected',{value:raw.slice(1)});
            if(!config.noComplete)this.emit('Tracing.tracingComplete',{dataLossOccurred:config.dataLoss??false,stream:'private-stream'});
          },config.completionDelayMs);
        }else{
          this.emit('Tracing.dataCollected',{value:raw});
          if(!config.noComplete)this.emit('Tracing.tracingComplete',{dataLossOccurred:config.dataLoss??false,stream:'private-stream'});
        }
      }
      return {};
    }
    async detach(){++detachCount;if(config.detachFail)throw secretError();}
  }
  const session=new Session();
  const browser={browserType:()=>({name:()=>config.browserType??'chromium'}),newBrowserCDPSession:async()=>{++sessionCount;return session;},
    close:()=>{++gameChanges;browserClosed=true;nativeTracing=false;}};
  const page={context:()=>({browser:()=>config.foreignPage?{}:browser}),
    async evaluate(fn,action){
      if(config.pageFail===action)throw secretError();
      if(config.pageHang===action)return new Promise(()=>{});
      context.action=action;return JSON.parse(JSON.stringify(runInContext('('+fn.toString()+')(action)',context)));
    }};
  const runtime={now:()=>at,readFile(path){++readCount;if(config.readFail)throw secretError();return Buffer.from(path==='fixture-harness.mjs'?'harness-'+sourceVersion:'helper-1');},
    setTimer,clearTimer:token=>timers.delete(token),limits:config.limits};
  async function tick(ms){const target=at+ms;await flush();for(;;){const next=[...timers].filter(([,x])=>x.due<=target).sort((a,b)=>a[1].due-b[1].due)[0];
      if(!next)break;timers.delete(next[0]);at=next[1].due;next[1].fn();await flush();}at=target;await flush();}
  return {session,browser,page,calls,raw,timers,fail,tick,
    start:(extra={})=>startChromeSchedulerTrace({enabled:true,browser,page,ownedBrowsers:[browser],side:0,sourceFiles:{harness:'fixture-harness.mjs'},...extra},runtime),
    mark(stage,value=150){pageAt=value;return context.window.chromeSchedulerTraceProbe.mark(stage);},
    pageTime:value=>{pageAt=value;},changeSource:()=>{++sourceVersion;},
    resolveNativeStart:()=>resolveNativeStart?.(),native:()=>({tracing:nativeTracing,closed:browserClosed}),
    stats:()=>({readCount,sessionCount,detachCount,gameChanges}),
    task:(extra={})=>raw.push({cat:'toplevel,renderer.scheduler,cc,viz,gpu',name:'RunTask',ph:'X',ts:1200000,dur:25,pid:10,tid:20,...extra})};
}

test('default off performs no browser, file, timer, or page work',async()=>{
  const h=fixture();const p=await h.start({enabled:false});
  assert.equal(p.snapshot().status,'disabled');assert.equal(p.snapshot().collectionValid,false);
  await p.stop();assert.deepEqual(h.stats(),{readCount:0,sessionCount:0,detachCount:0,gameChanges:0});assert.equal(h.timers.size,0);
});
test('ownership and actual Chrome capability reject without broadening or starting trace',async()=>{
  for(const [config,extra,reason] of [
    [{},{ownedBrowsers:[]},'browser-not-owned'],[{browserType:'firefox'},{},'browser-not-chromium'],
    [{foreignPage:true},{},'page-not-owned'],[{product:'Other/153'},{},'browser-not-chrome'],
    [{categories:['toplevel']},{},'required-categories-missing'],[{}, {sourceFiles:{}},'source-harness-required']]){
    const h=fixture(config),p=await h.start(extra);assert.equal(p.snapshot().availability.reason,reason);
    assert.equal(p.snapshot().status,'unavailable');assert.equal(h.calls.some(x=>x.method==='Tracing.start'),false);
    await p.stop();assert.equal(h.stats().gameChanges,0);assert.equal(h.timers.size,0);
  }
});
test('default pipeline preserves six categories, ordered markers, calibrated clock and idempotent stop',async()=>{
  const h=fixture(),p=await h.start();assert.equal(p.snapshot().status,'recording');
  const start=h.calls.find(x=>x.method==='Tracing.start').params;
  assert.deepEqual(CHROME_SCHEDULER_TRACE_CATEGORIES,['toplevel','renderer.scheduler','cc','viz','gpu','blink.user_timing']);
  assert.strictEqual(CHROME_SCHEDULER_TRACE_CATEGORIES,CHROME_SCHEDULER_TRACE_PROFILES.pipeline);
  assert.equal(p.snapshot().profile,'pipeline');
  assert.deepEqual(start,{transferMode:'ReportEvents',bufferUsageReportingInterval:1000,tracingBackend:'chrome',traceConfig:{
    recordMode:'recordUntilFull',traceBufferSizeInKb:32768,enableSampling:false,enableSystrace:false,enableArgumentFilter:true,
    includedCategories:[...CHROME_SCHEDULER_TRACE_CATEGORIES]}});
  h.mark('first-active');h.mark('first-active',160);h.task();h.pageTime(250);
  const a=p.stop('capture-stop'),b=p.stop('failure');assert.equal(a,b);
  const s=await a;assert.equal(s.collectionValid,true);assert.equal(s.coverage.activeWindowCovered,true);
  assert.equal(s.coverage.clockMapped,true);assert.equal(s.clock.renderer.pid,10);assert.equal(s.clock.renderer.tid,20);
  assert.equal(s.clock.offsetIntersectionMs[0]<1000.01,true);assert.equal(s.clock.offsetIntersectionMs[1]>1000.01,true);
  assert.equal(s.marks.length,3);assert.equal(s.coverage.schedulerObserved,true);assert.equal(s.coverage.compositorObserved,true);assert.equal(s.coverage.gpuObserved,true);
  assert.equal(s.sourceIdentity.stable,true);assert.equal(h.calls.filter(x=>x.method==='Tracing.end').length,1);
  assert.equal(h.stats().detachCount,1);assert.equal(h.stats().gameChanges,0);assert.equal(h.timers.size,0);assert.equal(await p.stop(),s);
});

test('tasks profile requires only its fixed categories and filters other pipeline events',async()=>{
  const wanted=['toplevel','renderer.scheduler','blink.user_timing'];
  assert.deepEqual(CHROME_SCHEDULER_TRACE_PROFILES.tasks,wanted);
  assert.equal(Object.isFrozen(CHROME_SCHEDULER_TRACE_PROFILES),true);
  assert.equal(Object.isFrozen(CHROME_SCHEDULER_TRACE_PROFILES.tasks),true);
  const h=fixture({categories:wanted}),p=await h.start({profile:'tasks'});
  assert.equal(p.snapshot().availability.available,true);
  assert.deepEqual(h.calls.find(x=>x.method==='Tracing.start').params.traceConfig.includedCategories,wanted);
  h.mark('first-active');h.task({cat:'toplevel,renderer.scheduler'});
  for(const cat of ['cc','viz','gpu'])h.task({cat,name:'ExcludedPipelineEvent'});
  h.task({cat:'toplevel,gpu',name:'DoWork'});h.pageTime(250);
  const s=await p.stop();
  assert.equal(s.profile,'tasks');assert.equal(s.collectionValid,true);assert.equal(s.coverage.activeWindowCovered,true);
  assert.deepEqual(s.coverage.requestedCategories,wanted);assert.deepEqual(Object.keys(s.coverage.categories),wanted);
  assert.equal(s.coverage.schedulerObserved,true);assert.equal(s.coverage.compositorObserved,null);assert.equal(s.coverage.gpuObserved,null);
  assert.equal(s.events.some(e=>e.name==='ExcludedPipelineEvent'),false);
  assert.equal(s.events.some(e=>e.cat.split(',').some(c=>['cc','viz','gpu'].includes(c))),false);
  assert.equal(s.events.find(e=>e.name==='DoWork').cat,'toplevel');
  assert.equal(s.counts.droppedEvents,3);
  assert.ok(s.coverage.threads.every(t=>JSON.stringify(Object.keys(t.categories))===JSON.stringify(wanted)));
  assert.equal(h.stats().gameChanges,0);assert.equal(h.calls.filter(x=>x.method==='Tracing.end').length,1);
  const missing=fixture({categories:['toplevel','blink.user_timing']}),q=await missing.start({profile:'tasks'});
  assert.equal(q.snapshot().availability.reason,'required-categories-missing');
  assert.deepEqual(q.snapshot().availability.missingCategories,['renderer.scheduler']);
  assert.equal(missing.calls.some(x=>x.method==='Tracing.start'),false);
});

test('graphics uses exactly GPU/viz/user-timing without claiming task or cc coverage',async()=>{
  const wanted=['gpu','viz','blink.user_timing'];
  assert.deepEqual(CHROME_SCHEDULER_TRACE_PROFILES.graphics,wanted);
  assert.equal(Object.isFrozen(CHROME_SCHEDULER_TRACE_PROFILES.graphics),true);
  const h=fixture({categories:wanted}),p=await h.start({profile:'graphics'});
  assert.equal(p.snapshot().availability.available,true);
  assert.deepEqual(h.calls.find(x=>x.method==='Tracing.start').params.traceConfig.includedCategories,wanted);
  h.mark('first-active');
  h.task({cat:'gpu',name:'GpuCommandBufferStub::OnWaitSyncToken',ph:'B',ts:1200000,dur:undefined,pid:30,tid:40});
  h.task({cat:'gpu',name:'GpuCommandBufferStub::OnWaitSyncToken',ph:'E',ts:1207500,dur:undefined,pid:30,tid:40});
  h.task({cat:'viz',name:'Display::DrawAndSwap',ts:1202000,dur:4000,pid:30,tid:41});
  h.task({cat:'gpu,toplevel',name:'GpuScheduler::RunNextTask'});
  for(const cat of ['toplevel','renderer.scheduler','cc'])h.task({cat});
  h.raw.push({cat:'__metadata',ph:'M',ts:0,pid:30,tid:40,name:'thread_name',args:{name:'CrGpuMain'}});
  h.pageTime(250);const s=await p.stop();
  assert.equal(s.profile,'graphics');assert.equal(s.collectionValid,true);assert.equal(s.coverage.activeWindowCovered,true);
  assert.equal(s.coverage.clockMapped,true);assert.equal(s.coverage.allThreeMarks,true);assert.equal(s.coverage.marksOrdered,true);
  assert.deepEqual(s.coverage.renderer,{pid:10,tid:20});assert.equal(s.sourceIdentity.stable,true);
  assert.deepEqual(s.coverage.requestedCategories,wanted);assert.deepEqual(Object.keys(s.coverage.categories),wanted);
  assert.equal(s.coverage.schedulerObserved,null);assert.equal(s.coverage.compositorObserved,null);assert.equal(s.coverage.gpuObserved,true);
  assert.equal(s.coverage.categories.gpu,3);assert.equal(s.coverage.categories.viz,1);
  assert.equal(s.events.some(e=>e.name==='RunTask'),false);assert.equal(s.counts.droppedEvents,3);
  assert.ok(s.events.every(e=>e.cat==='__metadata'||e.cat.split(',').every(c=>wanted.includes(c))));
  assert.equal(s.events.find(e=>e.name==='GpuScheduler::RunNextTask').cat,'gpu');
  assert.deepEqual(s.events.filter(e=>e.name==='GpuCommandBufferStub::OnWaitSyncToken').map(e=>[e.ph,e.ts,e.pid,e.tid]),
    [['B',1200000,30,40],['E',1207500,30,40]]);
  assert.equal(s.events.find(e=>e.name==='Display::DrawAndSwap').dur,4000);
  assert.equal(s.events.find(e=>e.args?.name==='CrGpuMain').tid,40);
  assert.ok(s.coverage.threads.every(t=>JSON.stringify(Object.keys(t.categories))===JSON.stringify(wanted)));
  assert.equal(h.stats().gameChanges,0);assert.equal(h.timers.size,0);assert.equal(h.calls.filter(x=>x.method==='Tracing.end').length,1);
  for(const category of ['gpu','viz']){
    const missing=fixture({categories:wanted.filter(c=>c!==category)}),q=await missing.start({profile:'graphics'});
    assert.equal(q.snapshot().availability.reason,'required-categories-missing');
    assert.deepEqual(q.snapshot().availability.missingCategories,[category]);
    assert.equal(missing.calls.some(x=>x.method==='Tracing.start'),false);
  }
  const empty=fixture({categories:wanted}),q=await empty.start({profile:'graphics'});empty.mark('first-active');empty.pageTime(250);
  const e=await q.stop();assert.equal(e.collectionValid,true);assert.equal(e.coverage.activeWindowCovered,true);
  assert.equal(e.coverage.gpuObserved,false);assert.equal(e.coverage.categories.viz,0);
  assert.equal(e.coverage.schedulerObserved,null);assert.equal(e.coverage.compositorObserved,null);
});

test('graphics missing stop marker and bounded overflow cannot qualify the active window',async()=>{
  const missing=fixture({transformEvents(rows){rows.splice(rows.findIndex(e=>e.name==='th09-sched-capture-stop'),1);}});
  const p=await missing.start({profile:'graphics'});missing.mark('first-active');const s=await p.stop();
  assert.equal(s.marks.length,3);assert.equal(s.collectionValid,true);
  assert.equal(s.coverage.clockMapped,false);assert.equal(s.coverage.allThreeMarks,false);assert.equal(s.coverage.activeWindowCovered,false);
  const full=fixture({limits:{events:4}}),q=await full.start({profile:'graphics'});full.mark('first-active');
  for(let i=0;i<5;++i)full.task({cat:'gpu',name:'GpuScheduler::RunNextTask'});
  const f=await q.stop();assert.ok(f.overflow.events>0);assert.ok(f.events.length<=4);
  assert.equal(f.collectionValid,false);assert.equal(f.coverage.activeWindowCovered,false);
  assert.equal(full.stats().gameChanges,0);assert.equal(full.timers.size,0);
});

test('profiles cannot silently broaden to arbitrary names or caller category arrays',async()=>{
  for(const profile of ['gpu','constructor',['toplevel']]){
    const h=fixture(),p=await h.start({profile}),s=p.snapshot();
    assert.equal(s.profile,null);assert.equal(s.availability.reason,'unsupported-profile');
    assert.equal(s.collectionValid,false);assert.equal(h.calls.length,0);
    assert.deepEqual(h.stats(),{readCount:0,sessionCount:0,detachCount:0,gameChanges:0});
  }
});

test('80 percent and sub-full saturation stop once and cannot qualify as complete capture',async()=>{
  assert.equal(CHROME_SCHEDULER_TRACE_BUFFER_HIGH_WATER,0.80);
  for(const field of ['percentFull','value']){
    const h=fixture(),p=await h.start();h.mark('first-active');
    h.session.emit('Tracing.bufferUsage',{[field]:0.799,eventCount:40});await flush();
    assert.equal(h.calls.some(x=>x.method==='Tracing.end'),false);assert.equal(p.snapshot().capture.bufferHighWater,false);
    h.session.emit('Tracing.bufferUsage',{[field]:0.80,eventCount:44});
    h.session.emit('Tracing.bufferUsage',{[field]:0.996245,eventCount:55});await flush();
    const s=await p.stop('capture-stop');
    assert.equal(s.capture.reason,'buffer-high-water');assert.equal(s.capture.bufferHighWater,true);
    assert.equal(s.capture.bufferHighWaterThreshold,0.80);assert.equal(s.capture.censored,true);
    assert.equal(s.capture.firstBufferHighWater[field],0.80);assert.equal(s.capture.firstBufferHighWater.eventCount,44);
    assert.equal(s.capture.chromeBufferFull,false);assert.equal(s.capture.tracingCompleteReceived,true);assert.equal(s.capture.dataLossOccurred,false);
    assert.equal(s.collectionValid,false);assert.equal(s.coverage.activeWindowCovered,false);
    assert.equal(s.marks.some(m=>m.stage==='capture-stop'),false);
    assert.equal(h.calls.filter(x=>x.method==='Tracing.end').length,1);assert.strictEqual(await p.stop('finally'),s);
    assert.equal(h.stats().gameChanges,0);assert.equal(h.timers.size,0);
  }
});

test('tasks retains missing stop-marker, data-loss and incomplete-flush qualification failures',async()=>{
  const missing=fixture({transformEvents(rows){rows.splice(rows.findIndex(e=>e.name==='th09-sched-capture-stop'),1);}});
  const p=await missing.start({profile:'tasks'});missing.mark('first-active');const s=await p.stop();
  assert.equal(s.marks.length,3);assert.equal(s.collectionValid,true);
  assert.equal(s.coverage.clockMapped,false);assert.equal(s.coverage.allThreeMarks,false);assert.equal(s.coverage.activeWindowCovered,false);
  const lost=fixture({dataLoss:true}),q=await lost.start({profile:'tasks'});lost.mark('first-active');const l=await q.stop();
  assert.equal(l.capture.dataLossOccurred,true);assert.equal(l.collectionValid,false);assert.equal(l.coverage.activeWindowCovered,false);
  const absent=fixture({noComplete:true,limits:{completionTimeoutMs:40}}),r=await absent.start({profile:'tasks'});
  absent.mark('first-active');const pending=r.stop();await absent.tick(40);const a=await pending;
  assert.equal(a.capture.tracingCompleteReceived,false);assert.equal(a.capture.dataLossOccurred,null);
  assert.equal(a.collectionValid,false);assert.equal(a.coverage.activeWindowCovered,false);
  assert.ok(a.errors.some(e=>e.code==='tracing-complete-timeout'));assert.equal(absent.timers.size,0);
  const unknown=fixture({noComplete:true}),t=await unknown.start({profile:'tasks'});unknown.mark('first-active');
  const incomplete=t.stop();await flush();unknown.session.emit('Tracing.tracingComplete',{});
  const u=await incomplete;assert.equal(u.capture.tracingCompleteReceived,true);assert.equal(u.capture.dataLossOccurred,null);
  assert.equal(u.collectionValid,false);assert.equal(u.coverage.activeWindowCovered,false);assert.equal(unknown.timers.size,0);
});

test('unknown names and dynamic args/IDs never survive sanitization',async()=>{
  const h=fixture(),p=await h.start();h.mark('first-active');
  h.task({name:'https://fixture.invalid/?room=secret',args:{url:'https://private.invalid',data:'secret-payload'},scope:'secret-scope',
    id:'not-a-safe-id-secret',bind_id:12,id2:{local:'0xdeadbeef',global:'private-user'},cat:'toplevel,secret-category'});
  h.raw.push({cat:'__metadata',ph:'M',ts:0,pid:10,tid:20,name:'thread_name',args:{name:'CrRendererMain',url:'private-url'}});
  h.raw.push({cat:'__metadata',ph:'M',ts:0,pid:10,tid:21,name:'thread_name',args:{name:'https://secret-thread.invalid'}});
  const s=await p.stop();const text=JSON.stringify(s);
  for(const secret of ['https://','secret','private','not-a-safe-id'])assert.equal(text.includes(secret),false,secret);
  const event=s.events.find(x=>x.nameHash);assert.match(event.nameHash,/^[a-f0-9]{64}$/);assert.equal(event.id,undefined);
  assert.equal(event.bind_id,12);assert.deepEqual(event.id2,{local:'0xdeadbeef'});assert.equal(event.args,undefined);
  assert.equal(s.events.find(x=>x.args?.name).args.name,'CrRendererMain');
  assert.deepEqual(s.coverage.unknownNames.map(x=>x.kind).sort(),['category','event','thread']);
});
test('collection completeness and event/clock coverage remain independent',async()=>{
  const noActive=fixture(),p=await noActive.start(),s=await p.stop();
  assert.equal(s.collectionValid,true);assert.equal(s.coverage.allThreeMarks,false);assert.equal(s.coverage.activeWindowCovered,false);
  for(const transformEvents of [
    rows=>{rows.find(x=>x.name==='th09-sched-first-active').ts+=1000000;},
    rows=>{rows.find(x=>x.name==='th09-sched-first-active').tid=999;},
    rows=>{rows.splice(rows.findIndex(x=>x.name==='th09-sched-first-active'),1);},
    rows=>{rows.push({...rows.find(x=>x.name==='th09-sched-first-active'),ts:999999999});}
  ]){
    const h=fixture({transformEvents}),q=await h.start();h.mark('first-active');const r=await q.stop();
    assert.equal(r.collectionValid,true);assert.equal(r.coverage.clockMapped,false);assert.equal(r.coverage.activeWindowCovered,false);
  }
});
test('recordUntilFull data loss and reported Chrome full are explicit failures',async()=>{
  const h=fixture({dataLoss:true}),p=await h.start();h.mark('first-active');const s=await p.stop();
  assert.equal(s.capture.dataLossOccurred,true);assert.equal(s.collectionValid,false);
  const q=fixture(),t=await q.start();q.mark('first-active');q.session.emit('Tracing.bufferUsage',{percentFull:1,eventCount:44});
  await flush();const u=await t.stop();assert.equal(u.capture.chromeBufferFull,true);assert.equal(u.capture.reason,'chrome-buffer-full');
  assert.equal(u.collectionValid,false);assert.equal(u.coverage.activeWindowCovered,false);assert.equal(q.stats().gameChanges,0);
});
test('all retained event, byte, name, thread and marker budgets are hard and visible',async()=>{
  for(const [key,limit,add] of [
    ['events',2,h=>{for(let i=0;i<8;++i)h.task();}],
    ['sanitizedBytes',65540,h=>h.task()],
    ['names',1,h=>{h.task({name:'unknown-1'});h.task({name:'unknown-2'});}],
    ['threads',1,h=>h.task({pid:44,tid:55})],
    ['marks',1,h=>h.mark('first-active')]
  ]){
    const h=fixture({limits:{[key]:limit}}),p=await h.start();add(h);const s=await p.stop();
    const field=key==='sanitizedBytes'?'bytes':key;assert.ok(s.overflow[field]>0,field);assert.equal(s.collectionValid,false);
    assert.notEqual(s.overflow.firstAt,null);assert.ok(s.events.length<=s.limits.events);assert.ok(s.marks.length<=s.limits.marks);
    assert.ok(s.coverage.unknownNames.length<=s.limits.names);assert.ok(s.coverage.threads.length<=s.limits.threads);
    assert.equal(h.calls.filter(x=>x.method==='Tracing.end').length,1);assert.equal(h.stats().gameChanges,0);
  }
  await assert.rejects(()=>fixture({limits:{events:CHROME_SCHEDULER_TRACE_LIMITS.events+1}}).start(),/test limit/);
});
test('bufferUsage and error arrays have independent caps and sanitization',async()=>{
  const h=fixture({limits:{bufferUsage:1}}),p=await h.start();
  h.session.emit('Tracing.bufferUsage',{percentFull:.1,eventCount:1,secret:'private'});
  h.session.emit('Tracing.bufferUsage',{percentFull:.2,eventCount:2});await flush();
  const s=await p.stop();assert.equal(s.bufferUsage.length,1);assert.equal(s.overflow.bufferUsage,1);assert.equal(s.collectionValid,false);
  const q=fixture({limits:{errors:1}}),t=await q.start();
  q.session.emit('Tracing.dataCollected',{value:null});q.session.emit('Tracing.dataCollected',{value:'private'});await flush();
  const u=await t.stop();assert.equal(u.errors.length,1);assert.ok(u.overflow.errors>0);assert.equal(u.collectionValid,false);
  assert.equal(JSON.stringify(u).includes('private'),false);
});
test('CDP start errors do not end a trace whose start was never confirmed',async()=>{
  for(const method of ['Browser.getVersion','Tracing.getCategories','Tracing.start']){
    const h=fixture({fail:[method]}),p=await h.start();const s=await p.stop();
    assert.equal(s.status,'unavailable');assert.equal(s.collectionValid,false);assert.equal(h.calls.some(x=>x.method==='Tracing.end'),false);
    assert.equal(h.stats().detachCount,1);assert.equal(h.timers.size,0);assert.equal(JSON.stringify(s).includes('https://'),false);
  }
});
test('stop page, protocol and detach errors return bounded structured snapshots',async()=>{
  for(const config of [{fail:['Tracing.end']},{pageFail:'capture-stop'},{detachFail:true}]){
    const h=fixture(config),p=await h.start();h.mark('first-active');const s=await p.stop('failure');
    assert.equal(s.status,'stopped');assert.equal(s.collectionValid,false);assert.ok(s.errors.length>0);
    assert.equal(JSON.stringify(s).includes('https://'),false);assert.equal(h.stats().gameChanges,0);
    assert.equal(h.stats().detachCount,1);assert.equal(h.timers.size,0);assert.equal(await p.stop(),s);
  }
});
test('source changes, malformed events and invalid marker calls cannot pass collection',async()=>{
  const h=fixture(),p=await h.start();h.changeSource();const s=await p.stop();
  assert.equal(s.sourceIdentity.stable,false);assert.ok(s.errors.some(e=>e.code==='source-identity-changed'));
  const q=fixture(),t=await q.start();q.raw.push({name:'not valid',cat:'toplevel',ts:NaN});q.mark('private-user-value');
  const u=await t.stop();assert.equal(u.counts.malformedEvents,1);assert.equal(u.collectionValid,false);
  assert.ok(u.errors.some(e=>e.code==='page-marker-call-error'));assert.equal(JSON.stringify(u).includes('private-user-value'),false);
});
test('deadline stops only tracing and retains the uncovered tail explicitly',async()=>{
  const h=fixture({limits:{durationMs:30}}),p=await h.start();h.mark('first-active');await h.tick(30);const s=await p.stop();
  assert.equal(s.capture.reason,'deadline');assert.equal(s.capture.deadlineFired,true);assert.equal(s.capture.nodeEndRequestedAt,30);
  assert.equal(s.collectionValid,true);assert.equal(s.coverage.clockMapped,true);assert.equal(s.coverage.allThreeMarks,false);
  assert.equal(s.coverage.activeWindowCovered,false);assert.equal(s.marks.some(m=>m.stage==='capture-stop'),false);
  assert.equal(h.calls.filter(x=>x.method==='Tracing.end').length,1);assert.equal(h.stats().gameChanges,0);assert.equal(h.timers.size,0);
});
test('deadline preempts a hanging final page mark; tracingComplete wait is bounded',async()=>{
  const h=fixture({pageHang:'capture-stop',limits:{durationMs:30,pageTimeoutMs:50}}),p=await h.start();h.mark('first-active');
  const result=p.stop();await h.tick(30);assert.equal(h.calls.filter(x=>x.method==='Tracing.end').length,1);
  await h.tick(20);const s=await result;assert.equal(s.capture.nodeEndRequestedAt,30);assert.equal(s.capture.deadlineFired,true);
  assert.equal(s.collectionValid,false);assert.equal(h.stats().gameChanges,0);assert.equal(h.timers.size,0);
  const q=fixture({noComplete:true,limits:{completionTimeoutMs:40}}),t=await q.start(),pending=t.stop();await q.tick(40);const u=await pending;
  assert.equal(u.collectionValid,false);assert.equal(u.capture.tracingCompleteReceived,false);
  assert.ok(u.errors.some(x=>x.code==='tracing-complete-timeout'));assert.equal(q.timers.size,0);
});
test('a flush completing after 5s but before 30s retains its late events and completion',async()=>{
  const h=fixture({completionDelayMs:12000}),p=await h.start();h.mark('first-active');h.task();h.pageTime(300);
  const pending=p.stop();await h.tick(6000);
  assert.equal(p.snapshot().status,'stopping');assert.equal(p.snapshot().counts.retainedEvents,1);
  assert.equal(p.snapshot().capture.tracingCompleteReceived,false);assert.equal(h.stats().detachCount,0);
  await h.tick(6000);const s=await pending;
  assert.equal(s.limits.completionTimeoutMs,30000);assert.equal(s.capture.nodeCompletedAt,12000);
  assert.equal(s.counts.retainedEvents,4);assert.equal(s.collectionValid,true);assert.equal(s.coverage.activeWindowCovered,true);
  assert.equal(s.errors.length,0);assert.equal(h.stats().detachCount,1);assert.equal(h.timers.size,0);
});
test('late native start after timeout is unconfirmed; only the owner closing Chrome bounds it',async()=>{
  const h=fixture({lateStart:true}),pending=h.start();await h.tick(5000);const p=await pending,s=p.snapshot();
  assert.equal(s.status,'unavailable');assert.equal(s.availability.reason,'tracing-start-failed');
  assert.ok(s.errors.some(e=>e.code==='cdp-Tracing-start'&&e.name==='TimeoutError'));
  assert.equal(s.capture.nodeStartedAt,null);assert.equal(s.capture.deadlineFired,false);assert.equal(h.timers.size,0);
  assert.equal(h.stats().detachCount,1);assert.equal(h.stats().gameChanges,0);assert.equal(h.native().tracing,false);
  h.resolveNativeStart();await flush();assert.equal(h.native().tracing,true);
  await p.stop();assert.equal(h.calls.some(x=>x.method==='Tracing.end'),false);assert.equal(h.native().tracing,true);
  h.browser.close();assert.deepEqual(h.native(),{tracing:false,closed:true});assert.equal(h.stats().gameChanges,1);
});
