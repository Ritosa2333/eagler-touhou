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

const root=resolve(import.meta.dirname,'../..'),repo=resolve(root,'../..');
function option(name,fallback){const combined=process.argv.find(x=>x.startsWith(name+'='));if(combined)return combined.slice(name.length+1);
  const i=process.argv.indexOf(name);if(i<0)return fallback;assert.ok(process.argv[i+1]&&!process.argv[i+1].startsWith('--'),'Missing '+name);return process.argv[i+1];}
const mode=Number(option('--mode','0')),players=Number(option('--players','2')),duration=Number(option('--duration','15'));
assert.ok([0,1,2].includes(mode)&&[2,3].includes(players));assert.ok(duration>=5&&duration<=60);
const runtime=resolve(option('--runtime',process.env.TH15_MP_PACKAGE||resolve(root,'build-eagler-multiplayer')));
const launcher=resolve(option('--launcher-root',process.env.TH15_LAUNCHER_ROOT||resolve(repo,'launcher')));
const relayEntry=resolve(process.env.EAGLER_RELAY_ROOT||resolve(repo,'services/relay'),'src/netplay-relay.mjs');
const out=resolve(option('--output',resolve(root,'web/artifacts/sdl-multiplayer/performance-'+mode+'-'+players+'p-'+new Date().toISOString().replaceAll(':','-'))));
assert.ok(!existsSync(resolve(out,'receipt.json')),'Never overwrite an earlier receipt');mkdirSync(out,{recursive:true});
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const fileIdentity=path=>{const b=readFileSync(path);return {path,bytes:b.length,sha256:hash(b)};};
const saveJson=(name,value)=>writeFileSync(resolve(out,name),JSON.stringify(value,null,2)+'\n');
const pause=ms=>new Promise(r=>setTimeout(r,ms));
function gitIdentity(){const run=args=>{const r=spawnSync('git',['-C',repo,...args],{encoding:'utf8',maxBuffer:8*1024*1024});assert.equal(r.status,0,r.stderr);return r.stdout.trim();};
  return {head:run(['rev-parse','HEAD']),tree:run(['rev-parse','HEAD^{tree}']),status:run(['status','--porcelain=v1']),diffSha256:hash(run(['diff','--binary','HEAD']))};}
const report={schema:'eagler-touhou/th15-production-performance/1',startedAt:new Date().toISOString(),completedAt:null,
  status:'running',performanceMeasured:false,observationPassed:false,settings:{mode,players,durationSeconds:duration,inputDelayFrames:mode===0?0:2,
    predictionFrames:mode===1?0:12,renderScale:1,cpuThrottle:1,route:'relay',oneWaySendDelayMs:50,jitterPlusMinusMs:5,
    nominalRoundTripAddedMs:100,profiler:false,music:false,warmupMinimumFrames:180,tailThresholdMs:100},
  scope:{productionPackage:true,independentBrowserProcesses:players,realRaf:true,managedHostInput:true,renderer:'SwiftShader',
    actualGpuScanout:false,performanceObserver:true,fullCampaign:false,phone:false},
  stages:[],errors:[],consoleErrors:[],cleanup:[],results:[]};
const save=()=>saveJson('receipt.json',report),mark=name=>{report.stages.push({name,at:new Date().toISOString()});save();};
save();
let server,relay,relayLog='',firstFailure,browserTemp;const browsers=[],contexts=[],pages=[],frames=[],lobbies=[];
const originalTempEnv=Object.fromEntries(['TMPDIR','TMP','TEMP'].map(key=>[key,process.env[key]]));
let shuttingDown=false,options=[];
const observeError=(seat,kind,error)=>{(shuttingDown?report.cleanup:report.errors).push({seat,kind,error:String(error)});};
const watchdog=setTimeout(()=>{report.errors.push({kind:'overall-timeout',error:'Production performance observer exceeded its bounded deadline'});save();
  relay?.kill();void Promise.allSettled(browsers.map(b=>b.close()));},(240+duration)*1000);

// Self-contained browser observer. The sender queue is an exact copy of the
// independently tested TH11 FIFO helper from commit c32c922f64263a07ec602041297c7a17325a8dec.
function installProbe(config,createSenderDelayQueue){
  const state={timeOrigin:performance.timeOrigin,phase:'warmup',start:0,end:0,stoppedAt:null,rows:[],presentations:[],
    presentationsOutsideNativeCallback:0,transport:[],errors:[],overflow:[],queued:0,queuedBytes:0,maxQueued:0,maxQueuedBytes:0,
    nativeTokens:[],current:null,serial:0,wireSerial:0,presentCount:0,attached:false,warmupHighFrame:0};
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
  function snapshot(){return {at:performance.now(),net:words('_th15_mp_status',24),rollback:words('_th15_mp_rollback_status',20),
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
  window.rpc=(command,fields={})=>new Promise((yes,no)=>{const child=document.querySelector('iframe').contentWindow,request=crypto.randomUUID();
    const timer=setTimeout(()=>{removeEventListener('message',listen);no(Error('RPC '+command+' timeout'));},60000);
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
  const iframe=document.createElement('iframe');iframe.src='/runtime/th15.html?managedData=1&hosted=1&runtimeEpoch=1';document.body.append(iframe);
}

function nextJson(socket,predicate=()=>true){return new Promise((yes,no)=>{const timeout=setTimeout(()=>{cleanup();no(Error('Lobby timeout'));},10000);
  const receive=e=>{let message;try{message=JSON.parse(String(e.data));}catch(error){cleanup();no(error);return;}
    if(message.type==='error'){cleanup();no(Error(message.error));}else if(predicate(message)){cleanup();yes(message);}};
  const closed=()=>{cleanup();no(Error('Lobby closed'));};function cleanup(){clearTimeout(timeout);socket.removeEventListener('message',receive);socket.removeEventListener('close',closed);}
  socket.addEventListener('message',receive);socket.addEventListener('close',closed);});}
async function lobbySend(socket,message,predicate){const promise=nextJson(socket,predicate);socket.send(JSON.stringify(message));return promise;}

try{
  report.sourceBefore=gitIdentity();report.driverFiles=Object.fromEntries(['performance-browser.mjs','performance-browser-metrics.mjs','performance-sender-delay.mjs']
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
  report.browserProcesses=[];
  for(let seat=0;seat<players;++seat){
    const browser=await chromium.launch({executablePath:process.env.EAGLER_CHROME_PATH,headless:true,args:[
      '--no-sandbox','--disable-extensions','--no-first-run','--autoplay-policy=no-user-gesture-required','--use-angle=swiftshader','--enable-unsafe-swiftshader',
      '--disable-background-timer-throttling','--disable-renderer-backgrounding','--disable-backgrounding-occluded-windows'],
      env:{...process.env,TMPDIR:browserTemp}});browsers.push(browser);
    const browserCdp=await browser.newBrowserCDPSession();const processes=await browserCdp.send('SystemInfo.getProcessInfo');await browserCdp.detach();
    const gpuSession=await browser.newBrowserCDPSession();const systemInfo=await gpuSession.send('SystemInfo.getInfo');await gpuSession.detach();
    report.browserProcesses.push({seat,version:browser.version(),processes:processes.processInfo,systemInfo});
    const context=await browser.newContext({viewport:{width:640,height:480},deviceScaleFactor:1});contexts.push(context);
    await context.addInitScript({content:'('+installProbe.toString()+')('+JSON.stringify({seat,duration})+',('+createSenderDelayQueue.toString()+'))'});
    const page=await context.newPage();pages.push(page);page.on('pageerror',error=>observeError(seat,'pageerror',error));
    page.on('crash',()=>observeError(seat,'page-crash','Chromium page crashed'));
    page.on('console',message=>{if(message.type()==='error')report.consoleErrors.push({seat,at:new Date().toISOString(),text:message.text(),cleanup:shuttingDown});});
    await page.goto(base+'/peer/'+seat);await page.waitForFunction(()=>window.ready,null,{timeout:60000});await page.evaluate(()=>launch());
    const frame=page.frames().find(f=>f.url().includes('/runtime/th15.html'));assert.ok(frame);frames.push(frame);
  }
  const pids=report.browserProcesses.map(b=>b.processes.find(p=>p.type==='browser')?.id);assert.ok(pids.every(Number.isFinite));assert.equal(new Set(pids).size,players,'Each peer must have its own browser process');
  mark('warmup');
  await Promise.all(frames.map(frame=>frame.waitForFunction(()=>{const p=Module._th15_mp_status()>>>2;return Module.HEAPU32[p+3]>=180||document.querySelector('#error').textContent;},null,{timeout:90000,polling:250})));
  report.warmup=await Promise.all(frames.map(frame=>frame.evaluate(()=>__th15Performance.snapshot())));
  for(const warm of report.warmup){assert.equal(warm.error,'');assert.equal(warm.nativePhase,1);assert.equal(warm.route,'relay');assert.equal(warm.rollback[0],1);}
  mark('measurement-start');
  await Promise.all(pages.map(page=>page.evaluate(duration=>measure(duration),duration)));
  // Do no polling, screenshots, profiling, serialization or test work in the window.
  await pause(duration*1000+600);
  await Promise.all(frames.map(frame=>frame.waitForFunction(()=>__th15Performance.state.phase==='complete',null,{timeout:10000,polling:250})));
  mark('measurement-complete');report.performanceMeasured=true;
  await pause(800); // Let the final managed-input release and FIFO packets settle.
  for(let seat=0;seat<players;++seat){
    const raw=await frames[seat].evaluate(()=>__th15Performance.result()),input=await pages[seat].evaluate(()=>inputEvidence);
    const events=await pages[seat].evaluate(()=>events);saveJson('peer-'+seat+'.json',{probe:raw,input,events});
    const metrics=summarize(raw),sent=raw.transport.filter(e=>Number.isFinite(e.actual)&&e.phase==='measure');
    metrics.actualSendDelay=distribution(sent.map(e=>e.actual-e.scheduled));metrics.sendTimerLateness=distribution(sent.map(e=>e.lateness));
    report.results.push({seat,metrics,input:{submitted:input.submitted,acknowledged:input.acknowledged,errors:input.errors},
      raw:fileIdentity(resolve(out,'peer-'+seat+'.json')),errors:raw.errors,overflow:raw.overflow});save();
  }
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
    result.finalStatus=replay.status;save();
  }
  const parsed=replays.map(bytes=>replayInputs(bytes)),commonFrames=Math.min(...parsed.map(p=>p.frames));
  const prefixBytes=commonFrames*players*12,reference=replays[0].subarray(parsed[0].prefix,parsed[0].prefix+prefixBytes);
  report.confirmedReplayComparison={commonFrames,allSeatPrefixBytes:prefixBytes,equal:replays.every((bytes,i)=>Buffer.from(bytes.subarray(parsed[i].prefix,parsed[i].prefix+prefixBytes)).equals(reference))};
  assert.equal(report.confirmedReplayComparison.equal,true,'Native-confirmed Replay input prefixes disagree');
  report.sourceAfter=gitIdentity();report.sourceStable=JSON.stringify(report.sourceBefore)===JSON.stringify(report.sourceAfter);
  report.runtimeStable=Object.entries(report.runtime.files).every(([name,identity])=>fileIdentity(resolve(runtime,name)).sha256===identity.sha256);
  const failures=[];
  if(!report.sourceStable||!report.runtimeStable)failures.push('Source or Runtime identity changed');
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
  report.observationFailures=failures;report.observationPassed=failures.length===0;
  report.thresholds={maximumMs:100,meaning:'The 100 ms comparison used for TH11, applied to complete native callback wall, presentation and forward-frontier window holds. This does not claim 60 fps.',
    peers:report.results.map(({seat,metrics:m})=>({seat,callbackMax:m.callbackWall.max,presentationMax:m.presentWindowHolds.max,frontierMax:m.frontierWindowHolds.max,
      met:[m.callbackWall.max,m.presentWindowHolds.max,m.frontierWindowHolds.max].every(v=>Number.isFinite(v)&&v<=100)}))};
  report.thresholdsMet=report.observationPassed&&report.thresholds.peers.every(p=>p.met);
  report.status=report.observationPassed?'measured':'observation-failed';if(!report.observationPassed)process.exitCode=1;
}catch(error){firstFailure=error;report.status=report.performanceMeasured?'observation-failed':'startup-or-execution-failed';
  report.errors.push({kind:'runner',error:String(error),stack:error.stack});process.exitCode=1;
  // Preserve partial observations without turning startup failure into a measurement.
  for(let seat=0;seat<frames.length;++seat)try{saveJson('peer-'+seat+'-partial.json',await frames[seat].evaluate(()=>({probe:globalThis.__th15Performance?.result(),error:document.querySelector('#error')?.textContent})));}catch{}
}finally{
  mark('cleanup');shuttingDown=true;
  await Promise.allSettled(pages.map(page=>page.evaluate(()=>window.stopInput?.())));
  await Promise.allSettled(frames.map(frame=>frame.evaluate(()=>{globalThis.__th15Performance?.close();globalThis.Module?._th15_loop_stop?.();globalThis.Module?._th15_mp_close?.();})));
  await Promise.allSettled(browsers.map(browser=>browser.close()));for(const socket of lobbies)socket.close();relay?.kill();
  if(server){server.closeAllConnections();await new Promise(yes=>server.close(yes));}if(browserTemp)rmSync(browserTemp,{recursive:true,force:true});
  for(const [key,value] of Object.entries(originalTempEnv))if(value===undefined)delete process.env[key];else process.env[key]=value;
  clearTimeout(watchdog);writeFileSync(resolve(out,'relay.log'),relayLog);report.completedAt=new Date().toISOString();save();
}
console.log(JSON.stringify({out,status:report.status,performanceMeasured:report.performanceMeasured,observationPassed:report.observationPassed,
  thresholdsMet:report.thresholdsMet,results:report.results.map(({seat,metrics:m})=>({seat,logicHz:m.logicHz,presentationHz:m.presentationHz,
    callbackP95:m.callbackWall.p95,callbackMax:m.callbackWall.max,presentationMax:m.presentWindowHolds.max,frontierMax:m.frontierWindowHolds.max,rollback:m.rollbackDelta}))},null,2));
if(firstFailure)console.error(firstFailure.stack);
