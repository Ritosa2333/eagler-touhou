// Production Runtime performance observation: real relay, independent Chrome
// processes, real rAF, managed Host input. No gameplay or native source changes.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {spawn,spawnSync} from 'node:child_process';
import {readFileSync,writeFileSync,mkdirSync,mkdtempSync,rmSync,existsSync} from 'node:fs';
import {resolve,extname,sep,dirname} from 'node:path';
import {createHash,randomUUID} from 'node:crypto';
import {loadPlaywright} from '../../../../tools/browser/playwright.mjs';
import {createSenderDelayQueue} from './performance-sender-delay.mjs';
import {distribution,summarize,replayInputs} from './performance-browser-metrics.mjs';
import {installLongTaskDiagnostic,installGlDiagnostic,startCpuDiagnostic,stopCpuDiagnostic,captureProcessCpuDiagnostic,compareProcessCpuDiagnostics} from './performance-browser-diagnostic.mjs';
import {environmentOptions,browserLaunchSettings,inspectRuntimeWebGL,rendererIdentity,startCpuThrottle,coverRuntimeCpuThrottle,verifyCpuThrottleCoverage,closeCpuThrottle} from './performance-browser-environment.mjs';
import {neutralPlan,committedHorizon,assertProofState,assertBeforeDeadline,awaitBeforeDeadline,proveNeutral,fireHorizon,proveFire} from './performance-input-proof.mjs';
import {collectStablePersistence} from './performance-persistence-proof.mjs';

const root=resolve(import.meta.dirname,'../..'),repo=resolve(root,'../..');
function option(name,fallback){const combined=process.argv.find(x=>x.startsWith(name+'='));if(combined)return combined.slice(name.length+1);
  const i=process.argv.indexOf(name);if(i<0)return fallback;assert.ok(process.argv[i+1]&&!process.argv[i+1].startsWith('--'),'Missing '+name);return process.argv[i+1];}
const mode=Number(option('--mode','0')),players=Number(option('--players','2')),duration=Number(option('--duration','15'));
assert.ok([0,1,2].includes(mode)&&[2,3].includes(players));assert.ok(duration>=5&&duration<=60);
const environment=environmentOptions({cpuThrottle:option('--cpu-throttle','1'),renderer:option('--renderer','swiftshader'),headless:!process.argv.includes('--headed')});
const diagnosticMode=option('--diagnostic','none');assert.ok(['none','gl-cpu','longtasks','all'].includes(diagnosticMode),'Unsupported diagnostic mode');
const diagnostic=diagnosticMode!=='none',cpuDiagnostic=['gl-cpu','all'].includes(diagnosticMode),longTaskDiagnostic=['longtasks','all'].includes(diagnosticMode);
const saveValidationOnly=process.argv.includes('--save-validation-only');
const saveEvidenceSupersessionTest=process.argv.includes('--save-evidence-supersession-test');
assert.ok(!(diagnostic&&saveValidationOnly),'Save validation is separate from profiler diagnostics');
assert.ok(!saveEvidenceSupersessionTest||saveValidationOnly,'--save-evidence-supersession-test requires --save-validation-only');
const runtime=resolve(option('--runtime',process.env.TH15_MP_PACKAGE||resolve(root,'build-eagler-multiplayer')));
const launcher=resolve(option('--launcher-root',process.env.TH15_LAUNCHER_ROOT||resolve(repo,'launcher')));
const relayEntry=resolve(process.env.EAGLER_RELAY_ROOT||resolve(repo,'services/relay'),'src/netplay-relay.mjs');
const out=resolve(option('--output',resolve(root,'web/artifacts/sdl-multiplayer/performance-'+mode+'-'+players+'p-'+new Date().toISOString().replaceAll(':','-'))));
assert.ok(!existsSync(resolve(out,'receipt.json'))&&!existsSync(resolve(out,'receipt.final.json')),'Never overwrite an earlier receipt');mkdirSync(out,{recursive:true});
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const fileIdentity=path=>{const b=readFileSync(path);return {path,bytes:b.length,sha256:hash(b)};};
const saveJson=(name,value)=>writeFileSync(resolve(out,name),JSON.stringify(value,null,2)+'\n');
const saveJsonExclusive=(name,value)=>{const path=resolve(out,name);writeFileSync(path,JSON.stringify(value,null,2)+'\n',{flag:'wx'});return fileIdentity(path);};
const pause=ms=>new Promise(r=>setTimeout(r,ms));
function gitIdentity(){const run=args=>{const r=spawnSync('git',['-C',repo,...args],{encoding:'utf8',maxBuffer:8*1024*1024});assert.equal(r.status,0,r.stderr);return r.stdout.trim();};
  return {head:run(['rev-parse','HEAD']),tree:run(['rev-parse','HEAD^{tree}']),status:run(['status','--porcelain=v1']),diffSha256:hash(run(['diff','--binary','HEAD']))};}
const report={schema:'eagler-touhou/th15-production-performance/1',startedAt:new Date().toISOString(),completedAt:null,
  status:'running',performanceMeasured:false,performanceEligible:!diagnostic&&!saveValidationOnly,diagnostic,saveValidationOnly,observationPassed:false,settings:{mode,players,durationSeconds:duration,inputDelayFrames:mode===0?0:2,
    predictionFrames:mode===1?0:12,renderScale:1,cpuThrottle:environment.cpuThrottle,rendererRequested:environment.renderer,headless:environment.headless,route:'relay',oneWaySendDelayMs:50,jitterPlusMinusMs:5,
    nominalRoundTripAddedMs:100,profiler:cpuDiagnostic,music:false,warmupMinimumFrames:180,tailThresholdMs:100},
  scope:{productionPackage:true,independentBrowserProcesses:players,realRaf:true,managedHostInput:true,renderer:'pending-verification',
    actualGpuScanout:false,performanceObserver:true,fullCampaign:false,phone:false,
    saveObserver:'Sparse native request/poll transitions and real FS.syncfs callbacks; reuse the previous rollback observation without extra journal Stats in the measurement window.',
    persistenceValidation:'After the fixed workload: confirmed all-seat neutral baseline and concurrent Host sync/new fire, then bounded stable Host/native/FS/Replay/MEMFS/IDB evidence with every superseded attempt retained.',
    ...(saveValidationOnly?{performanceEligibility:'Supplemental save/input acceptance only; workload timings cannot satisfy or replace a performance result.'}: {})},
  stages:[],errors:[],consoleErrors:[],cleanup:[],results:[]};
report.saveEvidenceValidation={maxAttempts:3,totalDeadlineMs:35000,naturalSupersessionTest:saveEvidenceSupersessionTest,
  forcedSeat:saveEvidenceSupersessionTest?players-1:null,
  deadlineMeaning:'One fixed total deadline for the original 15s Host sync + 10s Replay read + 10s persistence-read budget, shared by all seats and attempts.'};
if(diagnostic){report.thresholdsMet=null;report.diagnostics={mode:diagnosticMode,gl:cpuDiagnostic,cpuProfiler:cpuDiagnostic,longTasks:longTaskDiagnostic,samplingIntervalUs:cpuDiagnostic?4000:null,profiles:[],
  processCpu:{before:[],after:[],deltas:[],errors:[]},
  performanceEligibility:'Opt-in diagnostic observers, GL wrappers or CPU sampling add overhead. This run cannot satisfy the performance gate; thresholdsMet remains null.'};}
if(saveValidationOnly)report.thresholdsMet=null;
const save=()=>saveJson('receipt.json',report),mark=name=>{report.stages.push({name,at:new Date().toISOString()});save();};
save();
let server,relay,relayLog='',firstFailure,browserTemp;const browsers=[],contexts=[],pages=[],frames=[],lobbies=[],cpuProfiles=[],cpuThrottles=[];
const originalTempEnv=Object.fromEntries(['TMPDIR','TMP','TEMP'].map(key=>[key,process.env[key]]));
let shuttingDown=false,options=[];
const observeError=(seat,kind,error)=>{(shuttingDown?report.cleanup:report.errors).push({seat,kind,error:String(error)});};
const watchdog=setTimeout(()=>{report.errors.push({kind:'overall-timeout',error:'Production performance observer exceeded its bounded deadline'});save();
  relay?.kill();void Promise.allSettled(browsers.map(b=>b.close()));},(240+duration)*1000);
async function finishCpuProfiles(){
  for(const {seat,handle} of cpuProfiles){if(handle.closed)continue;
    try{const {profile,metadata}=await stopCpuDiagnostic(handle);const name='peer-'+seat+'.cpuprofile';saveJson(name,profile);
      report.diagnostics.profiles.push({seat,...metadata,file:fileIdentity(resolve(out,name))});}
    catch(error){observeError(seat,'cpu-profiler',error);report.diagnostics.profiles.push({seat,...handle.metadata,error:String(error)});}
  }
}
async function finishProcessCpuSnapshots(){
  for(const before of report.diagnostics.processCpu.before){const {seat}=before;
    if(report.diagnostics.processCpu.after.some(item=>item.seat===seat)||report.diagnostics.processCpu.errors.some(item=>item.seat===seat))continue;
    try{const after={seat,...await captureProcessCpuDiagnostic(browsers[seat],'after-profiler-stop')};
      report.diagnostics.processCpu.after.push(after);
      report.diagnostics.processCpu.deltas.push({seat,...compareProcessCpuDiagnostics(before,after)});}
    catch(error){observeError(seat,'process-cpu-snapshot',error);report.diagnostics.processCpu.errors.push({seat,error:String(error)});}
  }
}

// Self-contained browser observer. The sender queue is an exact copy of the
// independently tested TH11 FIFO helper from commit c32c922f64263a07ec602041297c7a17325a8dec.
function installProbe(config,createSenderDelayQueue){
  const state={timeOrigin:performance.timeOrigin,phase:'warmup',start:0,end:0,stoppedAt:null,rows:[],presentations:[],
    presentationsOutsideNativeCallback:0,transport:[],errors:[],overflow:[],queued:0,queuedBytes:0,maxQueued:0,maxQueuedBytes:0,
    nativeTokens:[],current:null,serial:0,wireSerial:0,presentCount:0,attached:false,warmupHighFrame:0,saveEvents:[],saveApi:false};
  const push=(name,value,max=20000)=>{if(state[name].length<max)state[name].push(value);else if(!state.overflow.includes(name))state.overflow.push(name);};
  document.addEventListener('webglcontextlost',event=>{if(event.target?.id==='canvas')state.errors.push('Game WebGL context lost');},true);
  const queue=createSenderDelayQueue({state}),nativeCallbacks=new WeakSet(),tokens=new WeakMap();let tokenSerial=0,random=(0x713515+config.seat*1009)>>>0;
  const rng=()=>{random=(Math.imul(random,1664525)+1013904223)>>>0;return random/4294967296;};
  const originalSend=WebSocket.prototype.send;
  WebSocket.prototype.send=function(data){
    const payload=data instanceof ArrayBuffer?new Uint8Array(data.slice(0)):ArrayBuffer.isView(data)?new Uint8Array(data.buffer.slice(data.byteOffset,data.byteOffset+data.byteLength)):null;
    if(!payload)return originalSend.call(this,data);
    const scheduled=performance.now(),delay=50+(rng()*2-1)*5;
    const entry={id:++state.wireSerial,phase:state.phase,scheduled,due:scheduled+delay,requestedDelay:delay,size:payload.length,actual:null,lateness:null};
    push('transport',entry);queue.enqueue({socket:this,payload,entry,send:originalSend,ordered:true});
  };
  Object.defineProperty(globalThis,'RTCPeerConnection',{value:undefined,configurable:true});
  // Renderer::commit is the production path that blits to DRAW framebuffer 0.
  // Track binding without getParameter, synchronization, or framebuffer reads.
  const drawTargets=new WeakMap(),gl=globalThis.WebGL2RenderingContext?.prototype;
  if(!gl)state.errors.push('WebGL2 prototype unavailable');
  else{
    const bind=gl.bindFramebuffer,blit=gl.blitFramebuffer;
    gl.bindFramebuffer=function(target,buffer){const result=bind.call(this,target,buffer);if(target===36160||target===36009)drawTargets.set(this,buffer);return result;};
    gl.blitFramebuffer=function(...args){const result=blit.apply(this,args);
      if(drawTargets.get(this)===null&&(args[8]&16384)&&this.canvas?.id==='canvas'){
        ++state.presentCount;const at=performance.now();
        if(state.phase==='measure'&&at>=state.start&&at<=state.end){push('presentations',at);
          if(!state.current||!nativeCallbacks.has(state.current.fn))++state.presentationsOutsideNativeCallback;}
      }return result;
    };
  }
  const words=(name,count)=>{const core=globalThis.Module;if(!core?.[name])return [];const p=core[name]()>>>2;return Array.from(core.HEAPU32.subarray(p,p+count));};
  let previousSnapshot=null;
  function snapshot(){return previousSnapshot={at:performance.now(),net:words('_th15_mp_status',24),rollback:words('_th15_mp_rollback_status',20),
    nativePhase:globalThis.Module?._th15_phase?.(),wasmBytes:globalThis.Module?.HEAPU8?.buffer.byteLength??0,
    presentCount:state.presentCount,route:globalThis.__eaglerPeerTransport?.route??null,
    error:document.querySelector('#error')?.textContent||'',queued:state.queued};}
  const originalRaf=requestAnimationFrame.bind(globalThis);
  globalThis.requestAnimationFrame=function(callback){
    if(!tokens.has(callback))tokens.set(callback,++tokenSerial);const token=tokens.get(callback),requested=performance.now();
    return originalRaf(stamp=>{
      const row={id:++state.serial,token,requested,stamp,entry:performance.now(),exit:null,nativeMs:null,forwardTicks:null,
        native:nativeCallbacks.has(callback),shellMs:0,observerMs:0};
      const previous=state.current;state.current={row,fn:callback};
      try{return callback(stamp);}finally{
        row.exit=performance.now();row.native=nativeCallbacks.has(callback);state.current=previous;
        if(state.phase==='warmup'&&row.native)state.warmupHighFrame=Math.max(state.warmupHighFrame,words('_th15_mp_status',24)[3]??0);
        if(state.phase==='measure'&&row.entry>=state.start&&row.entry<=state.end){
          const begin=performance.now();if(row.native)Object.assign(row,snapshot());row.observerMs=performance.now()-begin;push('rows',row);
        }
      }
    });
  };
  globalThis.__th15Performance={state,snapshot,
    attach(){if(state.attached)return;state.attached=true;const previous=Module.onGameFrame;
      Module.onGameFrame=(ok,ms,count)=>{const current=state.current;
        if(current){nativeCallbacks.add(current.fn);if(!state.nativeTokens.includes(current.row.token))state.nativeTokens.push(current.row.token);
          current.row.nativeMs=ms;current.row.forwardTicks=count;current.row.ok=ok;}
        else state.errors.push('Native frame notification outside observed rAF');
        const begin=performance.now();try{return previous?.(ok,ms,count);}finally{if(current)current.row.shellMs+=performance.now()-begin;}
      };
      // Observe each request / terminal transition, not every poll. Journal
      // statistics are reused from the ordinary callback observer; no extra
      // mspace_mallinfo traversal is added to a measured save request.
      const saveEvent=(kind,fields={})=>push('saveEvents',{kind,at:performance.now(),phase:state.phase,...fields,
        net:words('_th15_mp_status',24),rollbackObservedAt:previousSnapshot?.at??null,rollback:previousSnapshot?.rollback??null},1000);
      const nativeError=()=>{const address=Module._th15_mp_error?.();if(!address)return '';const end=Module.HEAPU8.indexOf(0,address);
        return new TextDecoder().decode(Module.HEAPU8.subarray(address,end<0?address+256:end));};
      const pollStates=new Map();let fsSerial=0;
      state.saveApi=['request','poll','cancel'].every(name=>typeof Module['_th15_mp_save_'+name]==='function');
      for(const kind of ['request','poll','cancel']){
        const name='_th15_mp_save_'+kind,original=Module[name];if(typeof original!=='function')continue;
        Module[name]=function(...args){const entered=performance.now();let result;
          try{result=original.apply(this,args);}catch(error){saveEvent('native-'+kind,{entered,token:args[0]??null,error:String(error)});throw error;}
          if(kind!=='poll'||pollStates.get(args[0])!==result){
            saveEvent('native-'+kind,{entered,token:kind==='request'?result:args[0],result,
              ...(kind==='request'?{stack:new Error('Score save request origin').stack}: {}),
              ...((kind==='request'&&!result)||(kind==='poll'&&result<0)?{nativeError:nativeError()}: {})});
            if(kind==='poll')pollStates.set(args[0],result);
          }return result;
        };
      }
      if(Module.FS?.syncfs){const original=Module.FS.syncfs;
        Module.FS.syncfs=function(populate,callback){if(typeof populate==='function'){callback=populate;populate=false;}
          const id=++fsSerial;saveEvent('fs-sync-start',{id,populate:!!populate});
          return original.call(this,populate,error=>{saveEvent('fs-sync-complete',{id,populate:!!populate,error:error?String(error):null});callback?.(error);});
        };
      }
    },
    start(){if(!state.attached||!state.nativeTokens.length)throw Error('Native rAF not identified during warmup');
      state.startSnapshot=snapshot();state.startSnapshot.highFrame=Math.max(state.warmupHighFrame,state.startSnapshot.rollback[2]);
      state.start=performance.now();state.end=state.start+config.duration*1000;state.phase='measure';return {start:state.start,end:state.end};},
    stop(){state.stoppedAt=performance.now();state.phase='complete';state.stopSnapshot=snapshot();return {stoppedAt:state.stoppedAt};},
    result(){return {...state,current:null};},close(){queue.close();},
  };
}

function host(seat){
  window.events=[];window.ready=false;window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
  window.addEventListener('message',event=>{if(event.source!==document.querySelector('iframe')?.contentWindow||event.origin!==location.origin)return;
    if(event.data?.protocol==='eagler-touhou/1'){events.push(event.data);if(event.data.event==='ready')ready=true;}});
  window.rpc=(command,fields={},timeoutMs=60000)=>new Promise((yes,no)=>{const child=document.querySelector('iframe').contentWindow,request=crypto.randomUUID();
    const timer=setTimeout(()=>{removeEventListener('message',listen);no(Error('RPC '+command+' timeout'));},timeoutMs);
    function listen(event){if(event.source===child&&event.origin===location.origin&&event.data?.request===request){clearTimeout(timer);removeEventListener('message',listen);event.data.ok?yes(event.data):no(Error(event.data.error));}}
    addEventListener('message',listen);child.postMessage({protocol:'eagler-touhou/1',game:'th15',epoch:1,request,command,...fields},location.origin);});
  window.launch=async()=>{const child=document.querySelector('iframe').contentWindow;child.__th15Performance.attach();
    const options=await(await fetch('/options/'+seat)).json();await rpc('configure',{music:'none',options,
      sharedResources:[{path:'/msgothic.ttc',url:'/shared/msgothic.ttc'},{path:'/unifont.otf',url:'/shared/unifont.otf'}]});await rpc('launch');};
  window.measure=duration=>{
    const child=document.querySelector('iframe').contentWindow,probe=child.__th15Performance;
    const input=window.inputEvidence={submitted:0,acknowledged:0,errors:[],events:[],start:performance.now(),stop:null};
    let active=true,timer=null;
    const send=(x,fire)=>{const at=performance.now();++input.submitted;input.events.push({at,x,fire});
      void rpc('touch-controls',{controls:{fireEnabled:fire,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:x,joystickY:0}})
        .then(()=>++input.acknowledged,error=>input.errors.push(String(error)));};
    probe.start();const start=performance.now(),end=start+duration*1000;
    function pump(){if(!active)return;const now=performance.now();if(now>=end){active=false;probe.stop();input.stop=now;send(0,false);return;}
      const slot=Math.floor((now-start)/240);send(((slot+seat)%2?1:-1)*22000,true);
      timer=setTimeout(pump,Math.max(1,start+(slot+1)*240-performance.now()));}
    pump();window.stopInput=()=>{active=false;clearTimeout(timer);send(0,false);};
  };
  const neutral={fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0};
  const remaining=deadline=>{const ms=deadline-performance.now();if(ms<=0)throw Error('Host input-proof deadline expired');return ms;};
  window.neutralize=async(timeoutMs=10000)=>{
    const deadline=performance.now()+timeoutMs;window.stopInput?.();
    // The measurement producer keeps its original hot path. Once stopped,
    // submitted==acknowledged proves that all its live RPCs have settled.
    while(window.inputEvidence&&inputEvidence.submitted!==inputEvidence.acknowledged){
      if(inputEvidence.errors.length)throw Error('Measurement input producer failed');
      await new Promise(resolve=>setTimeout(resolve,Math.min(100,remaining(deadline))));
    }
    if(window.inputEvidence?.errors.length)throw Error('Measurement input producer failed');
    const start=performance.now(),reply=await rpc('touch-controls',{controls:neutral},remaining(deadline));
    return {timeOrigin:performance.timeOrigin,start,ackAt:performance.now(),request:reply.request,controls:neutral,
      settledProducer:window.inputEvidence?{submitted:inputEvidence.submitted,acknowledged:inputEvidence.acknowledged,errors:inputEvidence.errors}:null,
      snapshot:document.querySelector('iframe').contentWindow.__th15Performance.snapshot()};
  };
  window.proofSync=async(label,timeoutMs=15000)=>{
    const start=performance.now(),reply=await rpc('sync',{},timeoutMs);
    return {label,timeOrigin:performance.timeOrigin,start,ackAt:performance.now(),request:reply.request,
      snapshot:document.querySelector('iframe').contentWindow.__th15Performance.snapshot()};
  };
  window.releaseProofInput=async(timeoutMs=10000)=>{
    const start=performance.now(),reply=await rpc('touch-controls',{controls:neutral},timeoutMs);
    return {timeOrigin:performance.timeOrigin,start,ackAt:performance.now(),request:reply.request,controls:neutral,
      snapshot:document.querySelector('iframe').contentWindow.__th15Performance.snapshot()};
  };
  window.verifySave=async(timeoutMs=15000)=>{
    const child=document.querySelector('iframe').contentWindow,probe=child.__th15Performance;
    const evidence={label:'concurrent-sync',timeOrigin:performance.timeOrigin,start:performance.now(),before:probe.snapshot(),events:[],errors:[],commands:[]};
    const deadline=evidence.start+timeoutMs;
    const send=(command,fields={})=>{const record={command,...fields,requestedAt:performance.now()};evidence.commands.push(record);
      return rpc(command,fields,remaining(deadline)).then(reply=>{record.request=reply.request;record.ackAt=performance.now();return reply;});};
    const note=kind=>evidence.events.push({kind,at:performance.now()});let syncDone=false;
    note('sync-request');const saved=send('sync').then(()=>{syncDone=true;note('sync-ack');evidence.afterSync=probe.snapshot();});
    note('input-request');const input=send('touch-controls',{controls:{...neutral,fireEnabled:true,joystickX:22000}})
      .then(()=>{evidence.inputAckBeforeSync=!syncDone;note('input-ack');evidence.afterInput=probe.snapshot();});
    const outcomes=await Promise.allSettled([saved,input]);
    outcomes.forEach((result,i)=>{if(result.status==='rejected')evidence.errors.push({operation:i?'input':'sync',error:String(result.reason)});});
    // The coordinator holds this new input until every peer has committed
    // post-ACK captures and a separate completed sync has saved the evidence.
    evidence.after=probe.snapshot();return evidence;
  };
  const iframe=document.createElement('iframe');iframe.src='/runtime/th15.html?managedData=1&hosted=1&runtimeEpoch=1';document.body.append(iframe);
}

// Read the actual IDBFS database after Host sync has acknowledged its real
// callback. This observer neither invokes save nor changes the game files.
async function inspectPersistence(includeReplay=false){
  const root='/savesth15mp',files=[{name:'scoreth15.dat',kind:0},{name:'th15.cfg',kind:2}];
  if(includeReplay)files.push({name:'replay/th15mp_01.rpy',kind:1});
  const capturedAt=performance.now(),saveEventCountBegin=globalThis.__th15Performance?.state?.saveEvents?.length??0;
  for(const file of files)file.memory=Array.from(Module.FS.readFile(root+'/'+file.name));
  const databases=await indexedDB.databases();if(!databases.some(db=>db.name===root))throw Error('IDBFS save database is missing');
  const stored=await new Promise((yes,no)=>{
    const request=indexedDB.open(root);let db=null,finished=false;
    const finish=(error,entries)=>{if(finished)return;finished=true;clearTimeout(timer);db?.close();error?no(error):yes(entries);};
    const timer=setTimeout(()=>finish(Error('IDBFS verification read timed out')),10000);
    request.onerror=()=>finish(request.error||Error('IDBFS database open failed'));
    request.onupgradeneeded=()=>{request.transaction.abort();finish(Error('IDBFS database unexpectedly requires creation'));};
    request.onsuccess=()=>{db=request.result;if(finished){db.close();return;}
      try{const transaction=db.transaction('FILE_DATA','readonly'),store=transaction.objectStore('FILE_DATA'),entries={};
        transaction.oncomplete=()=>finish(null,entries);transaction.onabort=()=>finish(transaction.error||Error('IDBFS read aborted'));
        transaction.onerror=()=>finish(transaction.error||Error('IDBFS read failed'));
        for(const file of files){const read=store.get(root+'/'+file.name);read.onsuccess=()=>{entries[file.name]=read.result;};}
      }catch(error){finish(error);}
    };
  });
  for(const file of files){const entry=stored[file.name];if(!entry?.contents)throw Error('IDBFS file is missing: '+file.name);
    // MEMFS may persist an Int8Array. Compare the stored bytes, independent
    // of whether its JS view represents the high bit with a negative number.
    file.indexedDb=Array.from(new Uint8Array(entry.contents));file.equal=file.memory.length===file.indexedDb.length&&file.memory.every((byte,i)=>byte===file.indexedDb[i]);
    for(const [field,contents] of [['memoryNativeValid',file.memory],['nativeValid',file.indexedDb]]){
      const bytes=new Uint8Array(contents),pointer=Module._malloc(bytes.length);if(!pointer)throw Error('Native validation allocation failed');
      try{Module.HEAPU8.set(bytes,pointer);file[field]=(file.kind===1?Module._th15_mp_replay_validate(pointer,bytes.length):Module._th15_validate_file(file.kind,pointer,bytes.length))===1;}finally{Module._free(pointer);}
    }
  }
  return {database:root,store:'FILE_DATA',capturedAt,readAt:performance.now(),saveEventCountBegin,
    saveEventCountEnd:globalThis.__th15Performance?.state?.saveEvents?.length??0,files:files.filter(file=>file.kind!==1),
    ...(includeReplay?{replay:files.find(file=>file.kind===1)}: {})};
}

function summarizeSaveEvidence(saved,host,persistence){
  const explicitAt=host.timeOrigin+(host.periodicCutoff??host.start)-saved.timeOrigin;
  const automatic=saved.events.filter(event=>event.kind==='native-request'&&event.at>=29000&&event.at<explicitAt).map(request=>{
    const complete=saved.events.find(event=>event.kind==='native-poll'&&event.token===request.token&&event.result===1);
    const fsStart=complete&&saved.events.find(event=>event.kind==='fs-sync-start'&&!event.populate&&event.at>=complete.at);
    const persisted=fsStart&&saved.events.find(event=>event.kind==='fs-sync-complete'&&event.id===fsStart.id&&!event.error);
    return {token:request.token,requestedAt:request.at,completedAt:complete?.at??null,fsStartedAt:fsStart?.at??null,
      persistedAt:persisted?.at??null,requestStack:request.stack};
  });
  const failures=(host.errors||[]).map(e=>e.operation+': '+e.error);
  if(!saved.saveApi)failures.push('Production Runtime has no pending score save API');
  if(!saved.events.some(event=>event.kind==='native-poll'&&event.result===1))failures.push('No observed real native score completion');
  if(!saved.events.some(event=>event.kind==='fs-sync-complete'&&!event.populate&&!event.error))failures.push('No observed real IDBFS completion');
  if(!automatic.some(event=>event.completedAt!==null&&event.persistedAt!==null))failures.push('No periodic score request before Host sync was observed to complete and persist');
  if(host.inputAckBeforeSync!==true)failures.push('Live input ACK did not precede Host sync completion; concurrent input delivery was not demonstrated');
  if(persistence.error)failures.push(persistence.error);
  if(persistence.files.length!==2||persistence.files.some(file=>!file.equal||!file.nativeValid||file.memoryNativeValid===false))failures.push('Persisted score/config differ from MEMFS or fail native validation');
  return {periodicSavesRequestedBeforeHostSync:automatic,failures,passed:failures.length===0};
}

// Consume a completed Host save. The native exporter has no path parameter,
// so probes never invoke it or intercept FS: only the validation copy is /tmp.
function copySavedReplay({label,seat}){
  const source='/save/replay/th15mp_01.rpy',temporary='/tmp/th15-input-proof/'+label+'-'+seat+'.rpy';
  Module.FS.mkdirTree('/tmp/th15-input-proof');Module.FS.writeFile(temporary,Module.FS.readFile(source));
  const bytes=Module.FS.readFile(temporary),pointer=Module._malloc(bytes.length);if(!pointer)throw Error('Replay proof validation allocation failed');
  let valid;try{Module.HEAPU8.set(bytes,pointer);valid=Module._th15_mp_replay_validate(pointer,bytes.length);}finally{Module._free(pointer);}
  if(valid!==1)throw Error('Completed Host Replay failed native validation');
  return {label,source,temporary,copiedAt:performance.now(),timeOrigin:performance.timeOrigin,nativeValid:true,
    saveEventCount:__th15Performance.state?.saveEvents?.length??0,bytes:Array.from(bytes),snapshot:__th15Performance.snapshot()};
}
async function waitProofCommit(plan,end,label,timeoutMs=10000){
  const deadline=performance.now()+timeoutMs;
  for(;;){
    assertBeforeDeadline(performance.now(),deadline,label);
    const snapshots=await awaitBeforeDeadline(Promise.all(frames.map(frame=>frame.evaluate(()=>__th15Performance.snapshot()))),deadline,label);
    if(report.inputProof)report.inputProof.lastPoll={label,end,snapshots};
    assertBeforeDeadline(performance.now(),deadline,label);
    if(committedHorizon(snapshots,plan,end))return snapshots;
    await pause(Math.min(100,deadline-performance.now()));
  }
}
function retainProofCopy(observation,seat,label){
  const {bytes,...metadata}=observation,buffer=new Uint8Array(bytes),path=resolve(out,'peer-'+seat+'-'+label+'.rpy');
  writeFileSync(path,buffer,{flag:'wx'});return {buffer,metadata:{...metadata,file:fileIdentity(path)}};
}
async function savedProofCopies(label,deadline=performance.now()+10000){
  const observations=await awaitBeforeDeadline(Promise.all(frames.map((frame,seat)=>frame.evaluate(copySavedReplay,{label,seat}))),deadline,label+' Replay read');
  return observations.map((observation,seat)=>retainProofCopy(observation,seat,label));
}
async function hostProofPhase(label,method,timeoutMs=15000){
  mark(label);
  const deadline=performance.now()+timeoutMs;
  return awaitBeforeDeadline(Promise.all(pages.map(page=>page.evaluate(({label,method,timeoutMs})=>
    method==='proofSync'?window[method](label,timeoutMs):window[method](timeoutMs),{label,method,timeoutMs}))),deadline,label);
}

function readSaveEvidence(){return {timeOrigin:__th15Performance.state.timeOrigin,saveApi:__th15Performance.state.saveApi,
  events:__th15Performance.state.saveEvents,status:__th15Performance.snapshot()};}

// The save-only regression waits for the existing production timer. It never
// invokes a native save/export or installs, advances or disables a timer.
async function waitNaturalSaveAfterCopy(seat,copy,deadline,naturalSaveLines){
  assert.ok(saveValidationOnly&&saveEvidenceSupersessionTest,'Natural-save supersession is restricted to save-only validation');
  assertBeforeDeadline(performance.now(),deadline,'natural save supersession');
  const handle=await awaitBeforeDeadline(frames[seat].waitForFunction(({count,lines})=>{
    const events=__th15Performance.state.saveEvents.slice(count);
    if(document.querySelector('#error').textContent)throw Error(document.querySelector('#error').textContent);
    if(events.some(event=>event.error||event.nativeError||event.kind==='native-cancel'))throw Error('Natural-save regression observed a save failure');
    for(const request of events.filter(event=>event.kind==='native-request')){
      const stackLines=[...(request.stack||'').matchAll(/shell\.mjs:(\d+):\d+/g)].map(match=>Number(match[1]));
      if(!stackLines.some(line=>lines.includes(line)))continue;
      const completed=events.find(event=>event.kind==='native-poll'&&event.token===request.token&&event.result===1);
      const start=completed&&events.find(event=>event.kind==='fs-sync-start'&&!event.populate&&event.at>=completed.at);
      const end=start&&events.find(event=>event.kind==='fs-sync-complete'&&event.id===start.id&&!event.error);
      if(end)return {token:request.token,requestEnteredAt:request.entered,completionObservedAt:completed.at,
        fsId:start.id,filesystemStartedAt:start.at,filesystemCompletedAt:end.at,requestStack:request.stack};
    }
    return false;
  },{count:copy.metadata.saveEventCount,lines:naturalSaveLines},{timeout:Math.max(1,deadline-performance.now()),polling:100}),deadline,'natural save supersession');
  try{return await awaitBeforeDeadline(handle.jsonValue(),deadline,'natural save supersession evidence');}
  finally{await handle.dispose();}
}

async function stableProofEvidence({plan,proofEnd,syncEvidence,baselineCopies,initialSyncs,initialCopies,deadline}){
  const attempts=report.inputProof.persistenceAttempts=Array.from({length:players},()=>[]);
  const naturalSaveLines=saveEvidenceSupersessionTest?readFileSync(resolve(runtime,'shell.mjs'),'utf8').split('\n')
    .flatMap((line,index)=>line.includes('setInterval(')?[index+1]:[]):[];
  if(saveEvidenceSupersessionTest)assert.ok(naturalSaveLines.length,'The frozen production shell has no natural save interval');
  const remaining=phase=>{assertBeforeDeadline(performance.now(),deadline,phase);return deadline-performance.now();};
  const outcomes=await Promise.allSettled(frames.map(async(frame,seat)=>{
    const result=await collectStablePersistence({deadline,maxAttempts:report.saveEvidenceValidation.maxAttempts,
      attempt:async(number,attemptDeadline,observation)=>{
        const label='evidence-attempt-'+number,prefix='peer-'+seat+'-'+label;
        Object.assign(observation,{reference:plan.references[seat],proofEnd,baselineReplay:baselineCopies[seat].buffer});
        if(number===1){observation.sync=initialSyncs[seat];observation.copy=initialCopies[seat];}
        else{
          observation.sync=await awaitBeforeDeadline(pages[seat].evaluate(({label,timeoutMs})=>window.proofSync(label,timeoutMs),
            {label,timeoutMs:Math.min(15000,remaining(label+' Host sync'))}),attemptDeadline,label+' Host sync');
          const copy=await awaitBeforeDeadline(frame.evaluate(copySavedReplay,{label,seat}),
            Math.min(attemptDeadline,performance.now()+10000),label+' Replay read');
          observation.copy=retainProofCopy(copy,seat,label);
        }
        assertProofState(observation.copy.metadata.snapshot,plan.references[seat]);
        const proofCopies=initialCopies.map(value=>value.buffer);proofCopies[seat]=observation.copy.buffer;
        // Keep the original neutral interval, concurrent input/ACK transaction
        // and required committed horizon for every replacement copy.
        observation.inputProof=proveFire(proofCopies,plan,syncEvidence,proofEnd);
        if(saveEvidenceSupersessionTest&&seat===players-1&&number===1){
          observation.forcedSupersession=await waitNaturalSaveAfterCopy(seat,observation.copy,attemptDeadline,naturalSaveLines);
        }
        observation.persistence=await awaitBeforeDeadline(frame.evaluate(inspectPersistence,true),
          Math.min(attemptDeadline,performance.now()+10000),label+' persistence read');
        observation.persistenceFile=saveJsonExclusive(prefix+'-persistence.json',observation.persistence);
        observation.saved=await awaitBeforeDeadline(frame.evaluate(readSaveEvidence),attemptDeadline,label+' native/FS evidence');
        observation.saveFile=saveJsonExclusive(prefix+'-save.json',observation.saved);
      },
      retain:async(record,observation)=>{
        const entry={...record,seat,source:record.attempt===1?'initial-evidence-sync':'fresh-host-sync',
          sync:observation.sync??null,copy:observation.copy?.metadata??null,
          persistence:observation.persistenceFile??null,saveEvidence:observation.saveFile??null,
          inputProof:observation.inputProof??null,forcedSupersession:observation.forcedSupersession??null};
        const file=saveJsonExclusive('peer-'+seat+'-evidence-attempt-'+record.attempt+'.final.json',entry);
        attempts[seat].push({...entry,file});save();
      }});
    if(saveEvidenceSupersessionTest&&seat===players-1){
      assert.equal(attempts[seat][0]?.state,'superseded','The deliberate natural save must supersede the first evidence copy');
      assert.equal(result.selectedAttempt,2,'The deliberate save-only regression requires the second full transaction to be stable');
      assert.ok(attempts[seat][0].forcedSupersession,'The first attempt must retain the real natural native/FS completion');
    }
    return result;
  }));
  const failures=outcomes.filter(value=>value.status==='rejected');
  if(failures.length)throw new AggregateError(failures.map(value=>value.reason),'Stable post-window persistence evidence failed');
  return outcomes.map(value=>value.value);
}

function nextJson(socket,predicate=()=>true){return new Promise((yes,no)=>{const timeout=setTimeout(()=>{cleanup();no(Error('Lobby timeout'));},10000);
  const receive=e=>{let message;try{message=JSON.parse(String(e.data));}catch(error){cleanup();no(error);return;}
    if(message.type==='error'){cleanup();no(Error(message.error));}else if(predicate(message)){cleanup();yes(message);}};
  const closed=()=>{cleanup();no(Error('Lobby closed'));};function cleanup(){clearTimeout(timeout);socket.removeEventListener('message',receive);socket.removeEventListener('close',closed);}
  socket.addEventListener('message',receive);socket.addEventListener('close',closed);});}
async function lobbySend(socket,message,predicate){const promise=nextJson(socket,predicate);socket.send(JSON.stringify(message));return promise;}

try{
  report.sourceBefore=gitIdentity();report.driverFiles=Object.fromEntries(['performance-browser.mjs','performance-browser-metrics.mjs','performance-sender-delay.mjs','performance-input-proof.mjs','performance-persistence-proof.mjs','performance-browser-environment.mjs',
    ...(diagnostic?['performance-browser-diagnostic.mjs']:[])]
    .map(name=>[name,fileIdentity(resolve(import.meta.dirname,name))]));
  report.relaySource=fileIdentity(relayEntry);
  assert.ok(process.env.TH15_TEST_ARCHIVE,'Set TH15_TEST_ARCHIVE');
  const archive=readFileSync(process.env.TH15_TEST_ARCHIVE),fonts=[['msgothic.ttc',process.env.TH15_TEST_FONT||process.env.EAGLER_DEVELOPMENT_VANILLA_FONT],
    ['unifont.otf',process.env.TH15_TEST_UNICODE_FONT||process.env.EAGLER_DEVELOPMENT_UNICODE_FONT]];
  report.inputs={archive:fileIdentity(process.env.TH15_TEST_ARCHIVE),fonts:fonts.map(([name,path])=>({name,...fileIdentity(path)}))};
  const inventory=JSON.parse(readFileSync(resolve(runtime,'runtime-files.json'),'utf8'));
  report.runtime={path:runtime,manifest:JSON.parse(readFileSync(resolve(runtime,'manifest.json'),'utf8')),inventory:inventory.files,files:{}};
  for(const [name,expected] of Object.entries(inventory.files)){const identity=fileIdentity(resolve(runtime,name));assert.equal(identity.sha256,expected.sha256,name);assert.equal(identity.bytes,expected.bytes,name);report.runtime.files[name]=identity;}
  assert.equal(report.runtime.manifest.product,'th15mp');assert.equal(report.runtime.manifest.netplay?.stateBackend,'write-journal-32');
  assert.equal(report.runtime.manifest.execution.sha256,report.runtime.files['th15.wasm'].sha256);
  mark('identities-verified');
  relay=spawn(process.execPath,[relayEntry],{cwd:launcher,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe']});
  relay.stderr.on('data',bytes=>{relayLog+=bytes;});
  const relayPort=await new Promise((yes,no)=>{const timer=setTimeout(()=>no(Error('Relay startup timeout '+relayLog)),10000);
    relay.stdout.on('data',bytes=>{relayLog+=bytes;const match=relayLog.match(/listening ws:\/\/127\.0\.0\.1:(\d+)/);if(match){clearTimeout(timer);yes(Number(match[1]));}});
    relay.once('error',no);relay.once('exit',code=>{clearTimeout(timer);no(Error('Relay exited '+code));});});
  const room='th15mp-perf-'+randomUUID(),members=[];
  for(let seat=0;seat<players;++seat){const client='perf_'+seat+'_'+randomUUID(),member='member_'+randomUUID();members.push({client,member});
    const query=new URLSearchParams({room,lobby:client,member,intent:seat===0?'create':'join',players:String(players),difficulty:'1'});
    const socket=new WebSocket('ws://127.0.0.1:'+relayPort+'/?'+query);lobbies.push(socket);await nextJson(socket);
    await lobbySend(socket,{type:'take-seat',seat,loadout:players===2?seat+2:seat},m=>!!m.room?.seats[seat]);}
  const chapterThreshold=players===2?500000:5000000;
  await lobbySend(lobbies[0],{type:'settings',playerCount:players,difficulty:1,chapterThreshold},m=>m.room?.chapterThreshold===chapterThreshold);
  for(let seat=0;seat<players;++seat)await lobbySend(lobbies[seat],{type:'set-ready',ready:true},m=>m.room?.seats[seat]?.ready);
  report.roomStart=await lobbySend(lobbies[0],{type:'start',adonisMode:mode,inputDelay:report.settings.inputDelayFrames,inputDelayAuto:false,predictionReserve:2},m=>m.type==='start');
  options=members.map(({member},seat)=>({netplayMode:'lan',netplayUrl:'ws://127.0.0.1:'+relayPort+'/?'+new URLSearchParams({room,run:String(report.roomStart.serial),member}),
    netplayPlayer:seat,netplayPlayerCount:players,netplayDifficulty:1,netplaySeed:417,
    netplayLoadouts:Array.from({length:players},(_,s)=>({character:players===2?s+2:s,shot:0})),netplayInputDelay:report.settings.inputDelayFrames,
    netplayInputDelayAuto:false,netplayAdonisMode:mode,netplayPredictionLimit:mode===1?0:12,netplayPredictionReserve:2,
    netplaySpectator:false,netplaySpectatorCount:0,netplayChapterThreshold:chapterThreshold,netplayChallengeMode:false,
    thpracEnabled:false,renderScale:1,touchEnabled:true,touchMovementMode:'joystick'}));
  server=createServer((req,res)=>{try{const path=new URL(req.url,'http://localhost').pathname;
    if(/^\/peer\/[012]$/.test(path)){const seat=Number(path.at(-1));res.setHeader('Content-Type','text/html');res.end('<!doctype html><meta charset="utf-8"><style>html,body{margin:0;background:#111}iframe{width:640px;height:480px;border:0}</style><body><script>('+host.toString()+')('+seat+')</script>');return;}
    const font=fonts.find(([name])=>path==='/shared/'+name);if(font){res.end(readFileSync(font[1]));return;}
    if(path==='/data'){res.end(archive);return;}
    if(/^\/options\/[012]$/.test(path)){res.setHeader('Content-Type','application/json');res.end(JSON.stringify(options[Number(path.at(-1))]));return;}
    if(!path.startsWith('/runtime/')){res.writeHead(404).end();return;}const file=resolve(runtime,path.slice(9));
    if(!file.startsWith(runtime+sep))throw Error('Invalid path');res.setHeader('Content-Type',({'.mjs':'application/javascript','.wasm':'application/wasm','.json':'application/json','.html':'text/html','.css':'text/css'})[extname(file)]||'application/octet-stream');res.end(readFileSync(file));
  }catch(error){res.writeHead(404).end(String(error));}});
  await new Promise(yes=>server.listen(0,'127.0.0.1',yes));const base='http://127.0.0.1:'+server.address().port;
  const {chromium}=await loadPlaywright({preferPython:true});assert.ok(process.env.EAGLER_CHROME_PATH,'Set EAGLER_CHROME_PATH');
  report.browserExecutable=fileIdentity(process.env.EAGLER_CHROME_PATH);
  browserTemp=mkdtempSync(resolve(process.platform==='win32'?out:'/tmp','th15-perf-'));
  // Playwright allocates the profile in its own Node process before launching
  // Chromium. Its tmpdir must also be short, not only the child's environment.
  for(const key of Object.keys(originalTempEnv))process.env[key]=browserTemp;
  report.browserProcesses=[];report.cpuThrottling=[];report.renderers=[];report.browserLaunch=browserLaunchSettings(environment);
  for(let seat=0;seat<players;++seat){
    const browser=await chromium.launch({executablePath:process.env.EAGLER_CHROME_PATH,...report.browserLaunch,
      env:{...process.env,TMPDIR:browserTemp}});browsers.push(browser);
    const browserCdp=await browser.newBrowserCDPSession();const processes=await browserCdp.send('SystemInfo.getProcessInfo');await browserCdp.detach();
    const gpuSession=await browser.newBrowserCDPSession();const systemInfo=await gpuSession.send('SystemInfo.getInfo');await gpuSession.detach();
    report.browserProcesses.push({seat,version:browser.version(),processes:processes.processInfo,systemInfo});
    const context=await browser.newContext({viewport:{width:640,height:480},deviceScaleFactor:1});contexts.push(context);
    await context.addInitScript({content:'('+installProbe.toString()+')('+JSON.stringify({seat,duration})+',('+createSenderDelayQueue.toString()+'));'+
      (cpuDiagnostic?'('+installGlDiagnostic.toString()+')(__th15Performance.state);':'')+
      (longTaskDiagnostic?'('+installLongTaskDiagnostic.toString()+')(__th15Performance.state);':'')});
    const page=await context.newPage();pages.push(page);page.on('pageerror',error=>observeError(seat,'pageerror',error));
    page.on('crash',()=>observeError(seat,'page-crash','Chromium page crashed'));
    page.on('console',message=>{if(message.type()==='error')report.consoleErrors.push({seat,at:new Date().toISOString(),text:message.text(),cleanup:shuttingDown});});
    const throttle=await startCpuThrottle(page,environment.cpuThrottle);cpuThrottles.push({seat,handle:throttle});
    report.cpuThrottling.push({seat,metadata:throttle.metadata});save();
    await page.goto(base+'/peer/'+seat);await page.waitForFunction(()=>window.ready,null,{timeout:60000});
    const frame=page.frames().find(f=>f.url().includes('/runtime/th15.html'));assert.ok(frame);frames.push(frame);
    await coverRuntimeCpuThrottle(throttle,frame);save();await page.evaluate(()=>launch());
  }
  const pids=report.browserProcesses.map(b=>b.processes.find(p=>p.type==='browser')?.id);assert.ok(pids.every(Number.isFinite));assert.equal(new Set(pids).size,players,'Each peer must have its own browser process');
  mark('warmup');
  await Promise.all(frames.map(frame=>frame.waitForFunction(()=>{const p=Module._th15_mp_status()>>>2;return Module.HEAPU32[p+3]>=180||document.querySelector('#error').textContent;},null,{timeout:90000,polling:250})));
  report.warmup=await Promise.all(frames.map(frame=>frame.evaluate(()=>__th15Performance.snapshot())));
  for(const warm of report.warmup){assert.equal(warm.error,'');assert.equal(warm.nativePhase,1);assert.equal(warm.route,'relay');assert.equal(warm.rollback[0],1);}
  for(let seat=0;seat<players;++seat){
    const session=await browsers[seat].newBrowserCDPSession();let systemInfo;
    try{systemInfo=await session.send('SystemInfo.getInfo');}finally{await session.detach();}
    const webgl=await frames[seat].evaluate(inspectRuntimeWebGL),identity=rendererIdentity(environment.renderer,systemInfo,webgl);
    report.renderers.push({seat,webgl,systemInfo,identity});save();
    assert.equal(identity.matched,true,'Requested '+environment.renderer+' renderer was not verified for seat '+seat+' (observed '+identity.classification+')');
    if(longTaskDiagnostic)assert.equal(await frames[seat].evaluate(()=>__th15Performance.state.longTaskDiagnostic?.supported),true,'Long-task diagnostics unavailable');
  }
  report.scope.renderer=environment.renderer==='swiftshader'?'SwiftShader':'hardware-verified';mark('environment-verified');
  if(cpuDiagnostic){mark('diagnostic-process-cpu-before');
    for(let seat=0;seat<browsers.length;++seat)report.diagnostics.processCpu.before.push({seat,
      ...await captureProcessCpuDiagnostic(browsers[seat],'before-profiler-start')});
    mark('diagnostic-profiler-start');
    for(let seat=0;seat<pages.length;++seat)cpuProfiles.push({seat,handle:await startCpuDiagnostic(pages[seat],{samplingIntervalUs:4000})});}
  mark(saveValidationOnly?'validation-workload-start':'measurement-start');
  await Promise.all(pages.map(page=>page.evaluate(duration=>measure(duration),duration)));
  // The default measurement has no profiler or extra GL wrappers. Explicit
  // diagnostic runs keep their sampler active and are never performance eligible.
  // Neither path polls, screenshots, serializes or runs tests in the window.
  await pause(duration*1000+600);
  await Promise.all(frames.map(frame=>frame.waitForFunction(()=>__th15Performance.state.phase==='complete',null,{timeout:10000,polling:250})));
  if(cpuDiagnostic){await finishCpuProfiles();await finishProcessCpuSnapshots();mark('diagnostic-profiler-stop');}
  if(longTaskDiagnostic)await Promise.all(frames.map(frame=>frame.evaluate(()=>globalThis.__th15LongTaskDiagnosticStop?.())));
  for(const {handle} of cpuThrottles)await verifyCpuThrottleCoverage(handle);
  mark(saveValidationOnly?'validation-workload-complete':'measurement-complete');report.performanceMeasured=!saveValidationOnly;
  await pause(800); // Let the final managed-input release and FIFO packets settle.
  for(let seat=0;seat<players;++seat){
    const raw=await frames[seat].evaluate(()=>__th15Performance.result()),input=await pages[seat].evaluate(()=>inputEvidence);
    const events=await pages[seat].evaluate(()=>events);saveJson('peer-'+seat+'.json',{probe:raw,input,events});
    const metrics=summarize(raw),sent=raw.transport.filter(e=>Number.isFinite(e.actual)&&e.phase==='measure');
    metrics.actualSendDelay=distribution(sent.map(e=>e.actual-e.scheduled));metrics.sendTimerLateness=distribution(sent.map(e=>e.lateness));
    report.results.push({seat,metrics,input:{submitted:input.submitted,acknowledged:input.acknowledged,errors:input.errors},
      raw:fileIdentity(resolve(out,'peer-'+seat+'.json')),errors:raw.errors,overflow:raw.overflow,
      ...(cpuDiagnostic?{glDiagnostic:raw.glDiagnostic}: {}),...(longTaskDiagnostic?{longTaskDiagnostic:raw.longTaskDiagnostic}: {})});save();
  }
  // Keep the unmodified 15 s measurement intact. Exercise the real 30 s
  // autosave and then concurrent Host sync/input while all peers still run.
  mark('post-window-save-validation');
  await Promise.all(frames.map(frame=>frame.waitForFunction(()=>performance.now()>=31000,null,{timeout:20000,polling:250})));
  report.inputProof={status:'running',method:'all-seat confirmed neutral baseline, then a new fire command concurrent with Host sync and a later confirmed transition',
    persistenceSource:'During this neutral/fire proof, Replay is written only by normal Host saves. Evidence copies live in /tmp; probes never invoke native export or intercept FS. The runner retains its separate final confirmed-prefix export after stopping producers.'};save();
  const neutralEvidence=report.inputProof.neutralize=await hostProofPhase('neutralize','neutralize',10000);
  const plan=report.inputProof.plan=neutralPlan(neutralEvidence.map(value=>value.snapshot));save();
  report.inputProof.neutralCommitted=await waitProofCommit(plan,plan.end,'confirmed neutral barrier');
  report.inputProof.baselineSync=await hostProofPhase('baseline-sync','proofSync');
  const baselineCopies=await savedProofCopies('baseline-sync');
  baselineCopies.forEach((value,seat)=>assertProofState(value.metadata.snapshot,plan.references[seat]));
  report.inputProof.baselineFiles=baselineCopies.map(value=>value.metadata);
  report.inputProof.neutral=proveNeutral(baselineCopies.map(value=>value.buffer),plan);save();
  const syncEvidence=report.inputProof.concurrentSync=await hostProofPhase('concurrent-sync','verifySave');
  syncEvidence.forEach((value,seat)=>{value.periodicCutoff=neutralEvidence[seat].start;});save();
  for(const value of syncEvidence){assert.deepEqual(value.errors,[]);assert.equal(value.inputAckBeforeSync,true,'Concurrent input ACK must precede the tested sync ACK');}
  const proofEnd=report.inputProof.end=fireHorizon(syncEvidence,plan);
  report.inputProof.fireCommitted=await waitProofCommit(plan,proofEnd,'confirmed new fire barrier',15000);
  const evidenceDeadline=performance.now()+report.saveEvidenceValidation.totalDeadlineMs;
  const initialSyncs=report.inputProof.evidenceSync=await hostProofPhase('evidence-sync','proofSync',Math.min(15000,evidenceDeadline-performance.now()));
  const initialCopies=await savedProofCopies('evidence-sync',Math.min(evidenceDeadline,performance.now()+10000));
  initialCopies.forEach((value,seat)=>assertProofState(value.metadata.snapshot,plan.references[seat]));
  report.inputProof.initialEvidenceSync=initialSyncs;
  report.inputProof.initialEvidenceFiles=initialCopies.map(value=>value.metadata);
  report.inputProof.fire=proveFire(initialCopies.map(value=>value.buffer),plan,syncEvidence,proofEnd);save();
  const stableEvidence=await stableProofEvidence({plan,proofEnd,syncEvidence,baselineCopies,initialSyncs,initialCopies,deadline:evidenceDeadline});
  const fireCopies=stableEvidence.map(value=>value.observation.copy),persistences=stableEvidence.map(value=>value.observation.persistence);
  report.inputProof.evidenceSync=stableEvidence.map(value=>value.observation.sync);
  report.inputProof.evidenceFiles=fireCopies.map(value=>value.metadata);
  report.inputProof.selectedPersistenceAttempts=stableEvidence.map((value,seat)=>({seat,attempt:value.selectedAttempt,
    attemptFinal:report.inputProof.persistenceAttempts[seat].find(attempt=>attempt.attempt===value.selectedAttempt).file,
    transaction:value.classification.transaction}));
  report.inputProof.fire=proveFire(fireCopies.map(value=>value.buffer),plan,syncEvidence,proofEnd);save();
  const released=report.inputProof.release=await hostProofPhase('input-proof-release','releaseProofInput',10000);
  released.forEach((value,seat)=>{assertProofState(value.snapshot,plan.references[seat]);syncEvidence[seat].after=value.snapshot;
    syncEvidence[seat].release=value;syncEvidence[seat].events.push({kind:'input-release-ack',at:value.ackAt});});
  report.saveValidationFailures=[];
  for(let seat=0;seat<players;++seat){
    const persistence=persistences[seat];
    const saved=await frames[seat].evaluate(readSaveEvidence);
    saveJson('peer-'+seat+'-save.json',{...saved,host:syncEvidence[seat]});
    const validation=report.results[seat].saveValidation={host:syncEvidence[seat],saveApi:saved.saveApi,
      ...summarizeSaveEvidence(saved,syncEvidence[seat],persistence),
      evidence:fileIdentity(resolve(out,'peer-'+seat+'-save.json')),persistence:stableEvidence[seat].observation.persistenceFile,
      files:persistence.files.map(file=>({name:file.name,bytes:file.indexedDb.length,sha256:hash(new Uint8Array(file.indexedDb)),equal:file.equal,
        nativeValid:file.nativeValid,memoryNativeValid:file.memoryNativeValid})),
      inputProof:report.inputProof.fire.seats[seat],persistenceSync:report.inputProof.evidenceSync[seat],
      selectedPersistenceAttempt:report.inputProof.selectedPersistenceAttempts[seat],
      saveActivityAfterEvidenceCopy:saved.events.slice(fireCopies[seat].metadata.saveEventCount,persistence.saveEventCountEnd)};
    const persistedReplay=persistence.replay;
    if(persistedReplay){validation.persistedReplay={name:persistedReplay.name,bytes:persistedReplay.indexedDb.length,
      sha256:hash(new Uint8Array(persistedReplay.indexedDb)),equal:persistedReplay.equal,nativeValid:persistedReplay.nativeValid,memoryNativeValid:persistedReplay.memoryNativeValid,
      matchesEvidenceCopy:Buffer.from(persistedReplay.indexedDb).equals(fireCopies[seat].buffer)};}
    if(!validation.persistedReplay?.equal||!validation.persistedReplay.nativeValid||validation.persistedReplay.memoryNativeValid===false||!validation.persistedReplay.matchesEvidenceCopy){
      const superseded=validation.saveActivityAfterEvidenceCopy.some(event=>event.kind==='native-request'||event.kind==='fs-sync-start');
      validation.failures.push(superseded?'Another save started during persistence inspection; stable snapshot byte evidence was not established':
        'Completed evidence-sync Replay differs from MEMFS/IDB or fails native validation');validation.passed=false;}
    report.saveValidationFailures.push(...validation.failures.map(error=>({seat,error})));save();
  }
  report.inputProof.status='PASS';
  report.saveValidationPassed=report.saveValidationFailures.length===0;save();
  // Stop one producer after measurement. Let peers consume its final captured
  // prefix; save only native-confirmed Replay data, then compare all-seat input.
  const producer=players-1;
  report.stoppedProducer=await frames[producer].evaluate(()=>{Module._th15_loop_stop();return __th15Performance.snapshot();});
  const limit=report.stoppedProducer.rollback[3]+report.stoppedProducer.net[7]+(mode===1?0:12);
  await Promise.all(frames.slice(0,producer).map(frame=>frame.waitForFunction(limit=>{const p=Module._th15_mp_rollback_status()>>>2;
    return (Module.HEAPU32[p+2]>=limit&&!Module.HEAPU32[p+1])||document.querySelector('#error').textContent;},limit,{timeout:20000,polling:100})));
  await Promise.all(frames.map(frame=>frame.evaluate(()=>Module._th15_loop_stop())));
  const replays=[];
  for(let seat=0;seat<players;++seat){
    const replay=await frames[seat].evaluate(()=>{if(!Module._th15_mp_replay_save())throw Error('Native Replay save failed');
      const bytes=Module.FS.readFile('/save/replay/th15mp_01.rpy'),p=Module._malloc(bytes.length);let valid;
      try{Module.HEAPU8.set(bytes,p);valid=Module._th15_mp_replay_validate(p,bytes.length);}finally{Module._free(p);}
      return {bytes:Array.from(bytes),valid,status:__th15Performance.snapshot()};});
    assert.equal(replay.valid,1);const bytes=new Uint8Array(replay.bytes);replays.push(bytes);writeFileSync(resolve(out,'peer-'+seat+'.rpy'),bytes);
    const result=report.results[seat],raw=JSON.parse(readFileSync(resolve(out,'peer-'+seat+'.json'),'utf8')).probe;
    const rangeBegin=raw.startSnapshot.rollback[3]+raw.startSnapshot.net[7];
    const rangeEnd=result.metrics.finalCompleteWindowStatus.rollback[3]+raw.startSnapshot.net[7];
    result.replay={...fileIdentity(resolve(out,'peer-'+seat+'.rpy')),nativeValid:true,inputs:replayInputs(bytes,rangeBegin,rangeEnd)};
    const saveInput=result.saveValidation.host;
    assertProofState(replay.status,plan.references[seat]);
    assert.ok(replayInputs(bytes).frames>=proofEnd,'Final confirmed Replay must cover the entire input-proof interval');
    result.saveValidation.confirmedInput=replayInputs(bytes,saveInput.before.rollback[3]+plan.references[seat].delay,proofEnd);
    result.finalStatus=replay.status;save();
  }
  report.inputProof.final=proveFire(replays,plan,syncEvidence,proofEnd);
  report.saveValidationPassed=report.saveValidationFailures.length===0;
  const parsed=replays.map(bytes=>replayInputs(bytes)),commonFrames=Math.min(...parsed.map(p=>p.frames));
  const prefixBytes=commonFrames*players*12,reference=replays[0].subarray(parsed[0].prefix,parsed[0].prefix+prefixBytes);
  report.confirmedReplayComparison={commonFrames,allSeatPrefixBytes:prefixBytes,equal:replays.every((bytes,i)=>Buffer.from(bytes.subarray(parsed[i].prefix,parsed[i].prefix+prefixBytes)).equals(reference))};
  assert.equal(report.confirmedReplayComparison.equal,true,'Native-confirmed Replay input prefixes disagree');
  report.sourceAfter=gitIdentity();report.sourceStable=JSON.stringify(report.sourceBefore)===JSON.stringify(report.sourceAfter);
  report.driverStable=Object.values(report.driverFiles).every(identity=>fileIdentity(identity.path).sha256===identity.sha256);
  report.runtimeStable=Object.entries(report.runtime.files).every(([name,identity])=>fileIdentity(resolve(runtime,name)).sha256===identity.sha256);
  const failures=[];
  if(!report.sourceStable||!report.driverStable||!report.runtimeStable)failures.push('Source, observer or Runtime identity changed');
  for(const result of report.results){const {seat,metrics}=result;
    if(result.errors.length||result.overflow.length||result.input.errors.length||result.input.submitted!==result.input.acknowledged)failures.push('Observer/input errors for seat '+seat);
    if(!metrics.completeNativeCallbacks||metrics.observations.nativeCallbackTokens.length!==1||metrics.observations.presentationOutsideNativeCallback)failures.push('Native callback/presentation coverage for seat '+seat);
    if(metrics.retainedFramesMax>12||metrics.runtimePhases.some(p=>p!==1)||result.finalStatus.error)failures.push('Runtime/state error for seat '+seat);
    if(mode===1&&(metrics.rollbackDelta.restores||metrics.rollbackDelta.historyTicks))failures.push('Exact-input mode unexpectedly rolled back for seat '+seat);
    if(mode!==1&&(!metrics.rollbackDelta.restores||!metrics.rollbackDelta.historyTicks))failures.push('No observed rollback workload for seat '+seat);
    const lane=result.replay.inputs.seats[seat];if(!lane.fire||!lane.left||!lane.right)failures.push('Confirmed movement/shooting workload missing for seat '+seat);
    if(!metrics.presentationSubmissions)failures.push('No submitted game presentation for seat '+seat);
  }
  if(report.errors.length)failures.push('Browser errors');
  if(cpuDiagnostic&&report.diagnostics.profiles.filter(p=>p.stopped&&p.file&&!p.error).length!==players)failures.push('Incomplete diagnostic CPU profiles');
  if(cpuDiagnostic&&report.diagnostics.processCpu.deltas.length!==players)failures.push('Incomplete diagnostic process CPU snapshots');
  if(cpuDiagnostic&&report.diagnostics.processCpu.deltas.some(item=>!item.validMatchedCounters))failures.push('Regressed diagnostic process CPU counters');
  report.observationFailures=failures;report.observationPassed=failures.length===0;
  report.thresholds={maximumMs:100,meaning:'The 100 ms comparison used for TH11, applied to complete native callback wall, presentation and forward-frontier window holds. This does not claim 60 fps.',
    peers:report.results.map(({seat,metrics:m})=>({seat,callbackMax:m.callbackWall.max,presentationMax:m.presentWindowHolds.max,frontierMax:m.frontierWindowHolds.max,
      met:[m.callbackWall.max,m.presentWindowHolds.max,m.frontierWindowHolds.max].every(v=>Number.isFinite(v)&&v<=100)}))};
  report.thresholdsMet=diagnostic||saveValidationOnly?null:report.observationPassed&&report.thresholds.peers.every(p=>p.met);
  report.status=report.observationPassed?(saveValidationOnly?(report.saveValidationPassed?'save-validation-passed':'save-validation-failed'):diagnostic?'diagnostic-observed':'measured'):'observation-failed';if(!report.observationPassed||!report.saveValidationPassed)process.exitCode=1;
}catch(error){firstFailure=error;report.status=saveValidationOnly?'save-validation-failed':report.performanceMeasured?'observation-failed':'startup-or-execution-failed';
  if(report.inputProof){report.inputProof.status='FAIL';report.inputProof.error=String(error);report.saveValidationPassed=false;}
  report.errors.push({kind:'runner',error:String(error),stack:error.stack});process.exitCode=1;
  // Preserve partial observations without turning startup failure into a measurement.
  for(let seat=0;seat<frames.length;++seat)try{saveJson('peer-'+seat+'-partial.json',await frames[seat].evaluate(()=>({probe:globalThis.__th15Performance?.result(),error:document.querySelector('#error')?.textContent})));}catch{}
}finally{
  mark('cleanup');shuttingDown=true;
  if(cpuDiagnostic){await finishCpuProfiles();await finishProcessCpuSnapshots();}
  if(longTaskDiagnostic)await Promise.allSettled(frames.map(frame=>frame.evaluate(()=>globalThis.__th15LongTaskDiagnosticStop?.())));
  await Promise.allSettled(pages.map(page=>page.evaluate(()=>window.stopInput?.())));
  await Promise.allSettled(frames.map(frame=>frame.evaluate(()=>{globalThis.__th15Performance?.close();globalThis.Module?._th15_loop_stop?.();globalThis.Module?._th15_mp_close?.();})));
  for(const {seat,handle} of cpuThrottles)try{await closeCpuThrottle(handle);}catch(error){observeError(seat,'cpu-throttle-cleanup',error);}
  await Promise.allSettled(browsers.map(browser=>browser.close()));for(const socket of lobbies)socket.close();relay?.kill();
  if(server){server.closeAllConnections();await new Promise(yes=>server.close(yes));}if(browserTemp)rmSync(browserTemp,{recursive:true,force:true});
  for(const [key,value] of Object.entries(originalTempEnv))if(value===undefined)delete process.env[key];else process.env[key]=value;
  clearTimeout(watchdog);writeFileSync(resolve(out,'relay.log'),relayLog);report.completedAt=new Date().toISOString();save();
  // Keep a separate, write-once receipt after all cleanup has finished. The
  // progress receipt is rewritten throughout the run; a matrix must hash this
  // completed copy instead of depending on a mutable progress-file snapshot.
  writeFileSync(resolve(out,'receipt.final.json'),JSON.stringify(report,null,2)+'\n',{flag:'wx'});
}
console.log(JSON.stringify({out,status:report.status,performanceMeasured:report.performanceMeasured,observationPassed:report.observationPassed,
  diagnostic,performanceEligible:report.performanceEligible,thresholdsMet:report.thresholdsMet,saveValidationPassed:report.saveValidationPassed,results:report.results.map(({seat,metrics:m})=>({seat,logicHz:m.logicHz,presentationHz:m.presentationHz,
    callbackP95:m.callbackWall.p95,callbackMax:m.callbackWall.max,presentationMax:m.presentWindowHolds.max,frontierMax:m.frontierWindowHolds.max,rollback:m.rollbackDelta}))},null,2));
if(firstFailure)console.error(firstFailure.stack);
