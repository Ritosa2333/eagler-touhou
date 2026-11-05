// Two separate Chromium processes, verified RTC or explicit relay transport.
// No artificial outage by default: isolate sustained correction from outages.
import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {createHash,randomUUID} from 'node:crypto';
import {mkdirSync,readFileSync,realpathSync,statSync,writeFileSync} from 'node:fs';
import net from 'node:net';
import {basename,resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {chromium} from 'playwright';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../../../tools/browser/launch.mjs';
import {installAudioOutputProbe} from './audio-output-probe.mjs';
import {installResponsivenessProbe} from './responsiveness-probe.mjs';
import {startHostLoadProbe} from './host-load-probe.mjs';
import {installNetworkTransportProbe} from './network-transport-probe.mjs';
import {startRelayDiagnosticProcess} from './relay-transport-controller.mjs';
import {installLiveCallbackCostProbe} from './callback-cost-probe.mjs';
import {installStartupTransportProbe} from './startup-transport-probe.mjs';
import {SMOOTHNESS_WINDOW_SCHEMA,summarizeSmoothnessWindow} from './smoothness-window.mjs';
import {parseGpuVsyncControl,gpuVsyncLaunchArgs,inspectGpuVsyncLaunch,inspectGpuVsyncRun} from './gpu-vsync-control.mjs';

const root=fileURLToPath(new URL('../../',import.meta.url));
const label=process.env.RUN_LABEL||'smoothness';
assert.match(label,/^[\w-]+$/);
const members=[randomUUID(),randomUUID()];
const integer=(key,fallback,min,max)=>{const n=Number(process.env[key]??fallback);assert.ok(Number.isInteger(n)&&n>=min&&n<=max,key);return n;};
const limit=integer('FRAMES',1800,300,18000),delay=integer('DELAY_MS',39,0,200),jitter=integer('JITTER_MS',5,0,100);
const slow=integer('CPU_RATE',1,1,8),dropEvery=integer('DROP_EVERY',0,0,1000);
const firstSlow=integer('CPU_P1_RATE',1,1,8),cpuRatesBySide=[firstSlow,slow];
const release=process.argv.includes('--release'),profile=process.env.PROFILE_CPU==='1',fallback=process.argv.includes('--relay');
const liveCost=process.env.LIVE_COST==='1';assert.ok(!liveCost||!release,'LIVE_COST requires a DEV build; release is unsupported');
const renderWork=integer('RENDER_WORK',0,0,1)===1;
const startupTrace=process.env.STARTUP_TRACE==='1';
const headedControl=process.env.HEADED_CONTROL==='1';
const gpuVsyncControl=parseGpuVsyncControl(process.env.DISABLE_GPU_VSYNC,headedControl);
const schedulerTrace=process.env.CHROME_SCHED_TRACE==='1';
const schedulerTraceProfile=process.env.CHROME_SCHED_TRACE_PROFILE||'pipeline';
assert.ok(['pipeline','tasks','graphics'].includes(schedulerTraceProfile),'CHROME_SCHED_TRACE_PROFILE');
const rafAnomaly=process.env.RAF_ANOMALY==='1';
const installRafAnomalyProbe=rafAnomaly?(await import('./raf-anomaly-probe.mjs')).installRafAnomalyProbe:null;
const browserArgs=gpuVsyncLaunchArgs(['--enable-gpu','--use-gl=angle','--use-angle=d3d11','--autoplay-policy=no-user-gesture-required'],gpuVsyncControl);
// Keep Playwright's other headless defaults while changing only the window mode.
const headlessAuxiliaryArgs=['--hide-scrollbars','--mute-audio','--blink-settings=primaryHoverType=2,availableHoverTypes=2,primaryPointerType=4,availablePointerTypes=4'];
const browserLaunchOptions={args:browserArgs};
if(headedControl){
 const executablePath=realpathSync(chromium.executablePath());
 assert.equal(process.platform,'win32','Headed control currently pins the verified Windows normal Chrome');
 assert.equal(basename(executablePath).toLowerCase(),'chrome.exe','Headed control requires normal Chrome, not headless shell');
 if(process.env.PLAYWRIGHT_CHROMIUM_EXECUTABLE)assert.equal(realpathSync(process.env.PLAYWRIGHT_CHROMIUM_EXECUTABLE).toLowerCase(),executablePath.toLowerCase(),'Headed control rejects a different executable override');
 Object.assign(browserLaunchOptions,{headless:false,executablePath,args:[...browserArgs,...headlessAuxiliaryArgs]});
}
const impairmentSite=process.env.IMPAIRMENT_SITE||'client';assert.ok(['client','relay','none'].includes(impairmentSite),'IMPAIRMENT_SITE');
const relayInputDiagnostics=process.env.RELAY_TRACE==='1'||impairmentSite==='relay'||(fallback&&impairmentSite==='none');
const relayDiagnostics=relayInputDiagnostics||startupTrace;
assert.ok(!relayInputDiagnostics||fallback,'Relay gameplay trace requires --relay; RTC uses signaling only');
assert.ok(impairmentSite!=='relay'||fallback,'Relay impairment cannot delay RTC data channels');
const expectedRtcPath=process.env.EXPECT_RTC_PATH||'any';assert.ok(['any','direct','turn'].includes(expectedRtcPath),'EXPECT_RTC_PATH');
assert.ok(!fallback||expectedRtcPath==='any','RTC candidate assertion requires RTC transport');
const inputMode=process.env.INPUT_MODE||'wall-clock';assert.ok(['wall-clock','tape'].includes(inputMode));
const audioOutput=process.env.AUDIO_OUTPUT==='1';
const rafDiagnostics=process.env.RAF_DIAGNOSTICS==='1';
const hostLoad=process.env.HOST_LOAD==='1';
const networkTrace=process.env.NETWORK_TRACE==='1';
assert.ok(!schedulerTrace||(release&&rafDiagnostics&&!profile&&!liveCost&&!networkTrace),'Scheduler trace requires release/RAF diagnostics and excludes CPU/live/browser-packet profiling');
assert.ok(!rafAnomaly||(release&&rafDiagnostics&&!schedulerTrace&&!profile&&!liveCost&&!networkTrace),'RAF anomaly observations require release/RAF diagnostics and exclude heavy tracing');
assert.ok(!networkTrace||fallback,'NETWORK_TRACE currently requires --relay');
const responsiveness=process.env.RESPONSIVENESS==='1';
assert.ok(!responsiveness||inputMode==='wall-clock','Responsiveness requires independent wall-clock input');
const touchProfile=process.env.TOUCH_PROFILE||'off',touchInput=touchProfile!=='off';
assert.ok(['off','direct','joystick'].includes(touchProfile),'TOUCH_PROFILE');
const touchPattern=process.env.TOUCH_PATTERN||'lifecycle';
assert.ok(['lifecycle','continuous'].includes(touchPattern),'TOUCH_PATTERN');
assert.ok(!touchInput||inputMode==='wall-clock'&&!responsiveness,'Touch uses its own independent source/probe and excludes keyboard tape/RESPONSIVENESS');
const touchSourceHz=integer('TOUCH_SOURCE_HZ',60,60,120);assert.ok([60,120].includes(touchSourceHz),'TOUCH_SOURCE_HZ is 60 or 120');
const touchDurationMs=integer('TOUCH_DURATION_MS',120000,6000,120000);
const touchModule=touchInput?await import('./touch-input-probe.mjs'):null;
const touchSchedules=touchInput?[0,1].map(side=>touchModule.buildTouchInputSchedule({profile:touchProfile,pattern:touchPattern,sourceHz:touchSourceHz,durationMs:touchDurationMs,side})):null;
const burstAt=integer('BURST_AT_MS',0,0,300000),burstFor=integer('BURST_FOR_MS',0,0,30000),burstDelay=integer('BURST_DELAY_MS',0,0,2000);
if(impairmentSite==='none')assert.ok(delay===0&&jitter===0&&dropEvery===0&&burstFor===0&&burstDelay===0,'No injection requires explicit zero DELAY_MS/JITTER_MS/DROP_EVERY/BURST_FOR_MS/BURST_DELAY_MS');
if(impairmentSite==='relay')assert.equal(dropEvery,0,'Relay DROP_EVERY semantics are not implemented; use a separately declared drop condition');
const artifactDirectory=resolve(process.env.PC_BUILD||resolve(root,release?'artifacts/sdl-release':'artifacts/sdl3'));
const out=resolve(root,'artifacts/multiplayer-tests');mkdirSync(out,{recursive:true});
const sha=path=>createHash('sha256').update(readFileSync(path)).digest('hex');
const wasmPath=resolve(artifactDirectory,release?'th09.wasm':'th09-presentation.wasm');
const identity={wasm:sha(wasmPath),harness:sha(fileURLToPath(import.meta.url)),loader:sha(resolve(artifactDirectory,release?'th09.mjs':'th09-presentation.mjs'))};
identity.smoothnessWindow=sha(fileURLToPath(new URL('./smoothness-window.mjs',import.meta.url)));
identity.gpuVsyncControl=sha(fileURLToPath(new URL('./gpu-vsync-control.mjs',import.meta.url)));
if(headedControl)identity.headedBrowserExecutable={selection:'playwright.chromium.executablePath; existing normal Windows chrome.exe',basename:basename(browserLaunchOptions.executablePath),bytes:statSync(browserLaunchOptions.executablePath).size,sha256:sha(browserLaunchOptions.executablePath),launcherSha256:sha(fileURLToPath(new URL('../../../../../tools/browser/launch.mjs',import.meta.url)))};
const relayPath=resolve(process.env.EAGLER_RELAY_SOURCE||'D:/workspace/eagler/eagler-touhou/server/netplay-relay.mjs');
identity.relay=sha(relayPath);identity.shell=sha(resolve(root,'sdl-runtime/shared-netplay.mjs'));identity.presentationHarness=sha(resolve(root,'tests/browser/presentation.html'));
if(audioOutput)identity.audioProbe=sha(fileURLToPath(new URL('./audio-output-probe.mjs',import.meta.url)));
if(responsiveness)identity.responsivenessProbe=sha(fileURLToPath(new URL('./responsiveness-probe.mjs',import.meta.url)));
if(touchInput){identity.touchInputProbe=sha(fileURLToPath(new URL('./touch-input-probe.mjs',import.meta.url)));identity.touchInputSchedules=touchSchedules.map(p=>createHash('sha256').update(JSON.stringify(p)).digest('hex'));}
if(hostLoad)identity.hostLoadProbe=sha(fileURLToPath(new URL('./host-load-probe.mjs',import.meta.url)));
if(networkTrace)identity.networkTransportProbe=sha(fileURLToPath(new URL('./network-transport-probe.mjs',import.meta.url)));
if(liveCost)identity.callbackCostProbe=sha(fileURLToPath(new URL('./callback-cost-probe.mjs',import.meta.url)));
if(startupTrace)identity.startupTransportProbe=sha(fileURLToPath(new URL('./startup-transport-probe.mjs',import.meta.url)));
if(schedulerTrace)identity.chromeSchedulerTrace=sha(fileURLToPath(new URL('./chrome-scheduler-trace.mjs',import.meta.url)));
if(rafAnomaly)identity.rafAnomalyProbe=sha(fileURLToPath(new URL('./raf-anomaly-probe.mjs',import.meta.url)));
if(relayDiagnostics)for(const [key,file] of [['relayTransportProbe','relay-transport-probe.mjs'],['relayTransportFixture','relay-transport-fixture.mjs'],['relayTransportController','relay-transport-controller.mjs']])identity[key]=sha(fileURLToPath(new URL('./'+file,import.meta.url)));
if(relayDiagnostics)identity.relayStartupProbe=sha(fileURLToPath(new URL('./relay-startup-probe.mjs',import.meta.url)));
const port=await new Promise(r=>{const s=net.createServer();s.listen(0,'127.0.0.1',()=>{const p=s.address().port;s.close(()=>r(p));});});
const hostProbe=hostLoad?startHostLoadProbe():null;hostProbe?.mark('relay-setup-start');
const diagnosticRelay=relayDiagnostics?startRelayDiagnosticProcess({relayPath,port,config:{inject:impairmentSite==='relay',delay,jitter,burstAt,burstFor,burstDelay,startupTrace}}):null;
const relay=diagnosticRelay?.process||spawn(process.execPath,[relayPath],{windowsHide:true,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:String(port),EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe']});
if(Number.isInteger(relay.pid))hostProbe?.registerKnownProcess(relay.pid,'netplay-relay');
hostProbe?.mark('relay-spawned');
let relayLog='',host,retainedResults,relayTraceSnapshot,relayTraceError;const browsers=[],pages=[],sessions=[],errors=[];
const graphicsIdentity=[0,1].map(side=>({side,status:'not-attempted',clock:'Node performance.now()',timeOrigin:performance.timeOrigin,requestedAt:null,returnedAt:null,graphics:null}));
const schedulerTraces=[];let schedulerTraceEvidence,schedulerTraceFinish;
const endpointSnapshots=[];let endpointTraceError,measurementEndpointQualification,teardownClean=null;
const rafAnomalyNodePolls=rafAnomaly?{clock:'Node performance.now()',timeOrigin:performance.timeOrigin,
 limit:2048,attempted:0,overflow:0,rows:[],inFlight:null,last:null,
 scope:'Existing 100 ms status-poll CDP request/return brackets. Page clocks are separate; a missing poll is not proof of a pending RAF.'}:undefined;
const parameters={limit,delay,jitter,slow,dropEvery,release,profile,fallback,inputMode,audioOutput,responsiveness,burstAt,burstFor,burstDelay,rafDiagnostics,hostLoad,networkTrace,impairmentSite,relayDiagnostics,expectedRtcPath,liveCost,clientSendMode:impairmentSite==='client'?'timer':'synchronous',burstClock:impairmentSite==='relay'?'relay-first-nonempty-input-receive':'browser-active',cpuThrottle:{ratesBySide:cpuRatesBySide,configuredAfter:'openProbe',scope:'openProbe and after-open counters precede CPU throttling; subsequent netplay setup and first game callback use the configured per-side rates.'}};
const distribution=values=>{const a=values.slice().sort((a,b)=>a-b);return {count:a.length,total:a.reduce((s,n)=>s+n,0),p50:a[Math.floor(a.length*.5)]??null,p95:a[Math.floor(a.length*.95)]??null,p99:a[Math.floor(a.length*.99)]??null,max:a.at(-1)??null,over25ms:a.filter(n=>n>25).length,over50ms:a.filter(n=>n>50).length,over100ms:a.filter(n=>n>100).length,over250ms:a.filter(n=>n>250).length};};
parameters.startupTrace=startupTrace;parameters.relayInputDiagnostics=relayInputDiagnostics;
parameters.nativeFrontierObservation='Release pacing ABI sampled on every retained game callback, independently of optional RAF diagnostics';
parameters.graphicsIdentity={scope:'Actual captured core/canvas/existing context after openProbe and before CPU throttling; same-owner read repeated inside the existing failure snapshot. Missing references are unavailable, never inferred from requested browser flags.'};
parameters.renderWork={enabled:renderWork,columns:['sampleIndex','batches','uploadBytes','readBytes','vertexUploadBytes','programCompiles','bufferReplacements','bufferSubUpdates','rendererFrames','resamples','presentations'],
 source:'Same onGameFrame th09_game_metrics uint32[10] pointer as samples[11] presentations',limit:50000,cumulative:true,observerOverheadCalibrated:false,
 scope:'Optional numeric copies per retained callback, not per simulated frame; uint32 wrap or regression requires separate intervals. No extra draw/tick or per-frame GPU query/readback; these counters are not GPU timings.'};
parameters.touchInput={enabled:touchInput,profile:touchProfile,pattern:touchPattern,sourceHz:touchSourceHz,durationMs:touchDurationMs,
 mode:touchInput?0:null,inputDelay:touchInput?0:null,scope:'Synthetic native ABI events on an independent Page clock; not a Launcher/iOS bridge or physical phone test'};
parameters.headedControl=headedControl;
parameters.gpuVsyncControl={...gpuVsyncControl,scope:'Optional single-flag browser diagnostic; addedArgs are launch intent only. Explicit 0/1 requires two verified normal Chrome processes with distinct PIDs and qualified actual argv; unset retains default launch and qualification behavior.'};
parameters.chromeSchedulerTrace=schedulerTrace;
parameters.chromeSchedulerTraceProfile=schedulerTrace?schedulerTraceProfile:null;
parameters.rafAnomaly=rafAnomaly;
parameters.measurementEndpoint={scope:'stats/hash/audited/sdl/audio/memory/route aliases are frozen after workload completion, before trace drain. RTC query has separate request/completion clocks and identities. postDrain is a later live observation, not a recertification of the frozen selected route. teardownClean covers that observation before browser disposal, not disposal itself.',rtcTimeoutMs:1500};
// Only called after both workloads report ended. No periodic endpoint observer.
function beginCompletedEndpoint({release,schedulerTrace}){
 const c=window.core,m=window.measure,p=window.play,doc=document;
 const clock={name:'Page performance.now()',timeOrigin:performance.timeOrigin};
 const endpoint={version:1,status:'failed',clock,phase:schedulerTrace?'post-workload-pre-drain':'post-workload-pre-export',
  requestedAt:performance.now(),capturedAt:null,error:null,ownerCurrent:false,body:null,stats:null,hash:null,audited:null,sdl:null,audio:null,memoryBytes:null,route:null,declaredPath:null};
 const state={begin:endpoint,completed:null,completion:null};
 window.completedEndpointCapture=state;
 const current=()=>window.completedEndpointCapture===state&&window.core===c&&window.measure===m&&window.play===p&&document===doc;
 state.postDrain=()=>{
  const row={clock,observedAt:performance.now(),ownerCurrent:current(),stats:null,route:window.__eaglerNetplayTransport??null,
   peerCount:globalThis.__eaglerPeerTransport?.peers?.size??null,connected:p?.connected===true,disconnected:p?.disconnected===true,
   closed:Array.isArray(m?.closed)?m.closed.slice():null,lateClosed:null,closePrefixMatches:false,error:null,teardownClean:false};
  try{if(!row.ownerCurrent)throw Error('Endpoint owner changed before export');row.stats=window.readStats();}
  catch(error){row.error=String(error.message||error).slice(0,200);}
  const before=endpoint.body?.closed;
  row.closePrefixMatches=Array.isArray(before)&&Array.isArray(row.closed)&&row.closed.length>=before.length&&before.every((value,index)=>row.closed[index]===value);
  if(row.closePrefixMatches)row.lateClosed=row.closed.slice(before.length);
  row.teardownClean=!!(row.ownerCurrent&&!row.error&&row.connected&&!row.disconnected&&row.stats?.[6]&&row.closePrefixMatches&&row.closed.length===0);
  return row;
 };
 try{
  if(!c||!m?.ended||!p)throw Error('Endpoint capture before workload completion');
  c._th09_loop_pause(1);
  endpoint.body={started:m.started,ended:m.ended,samples:m.samples.length,confirmedHashes:window.confirmedHashTrace?.length??0,
   closed:m.closed.slice(),connected:p.connected===true,disconnected:p.disconnected===true};
  endpoint.stats=window.readStats();endpoint.hash=c._th09_network_hash()>>>0;
  if(!release){const at=c._th09_probe_world()/4;endpoint.audited=Array.from(c.HEAP32.subarray(at,at+48));}
  const at=c._sdl_stats()/4;endpoint.sdl=Array.from(c.HEAPU32.subarray(at,at+17));
  endpoint.audio={state:c.SDL3?.audioContext?.state,advancedSeconds:(c.SDL3?.audioContext?.currentTime||0)-m.audioStart};
  endpoint.memoryBytes=c.HEAPU8.length;endpoint.route=window.__eaglerNetplayTransport??null;endpoint.declaredPath=globalThis.__eaglerNetplayPath??null;
  endpoint.ownerCurrent=current();endpoint.capturedAt=performance.now();
  if(!endpoint.ownerCurrent)throw Error('Endpoint owner changed during capture');
  endpoint.status='captured';
  const transport=globalThis.__eaglerPeerTransport,peerCount=transport?.peers?.size??0,peers=[];
  // TH09 is two-player. Retain at most two entries to diagnose an invalid count.
  if(endpoint.route==='rtc')for(const [id,peer] of transport?.peers||[]){peers.push({id,peer,pc:peer.pc});if(peers.length===2)break;}
  const sameRoute=()=>current()&&globalThis.__eaglerPeerTransport===transport&&window.__eaglerNetplayTransport===endpoint.route&&(globalThis.__eaglerNetplayPath??null)===endpoint.declaredPath;
  state.completion=(async()=>{
   const rtcPeers=await Promise.all(peers.map(async ({id,peer,pc})=>{
    const identity=()=>({ownerCurrent:current(),routeCurrent:sameRoute(),peerCurrent:transport?.peers?.get(id)===peer,pcCurrent:peer.pc===pc,
     connected:p.connected===true&&!p.disconnected&&pc?.connectionState==='connected',connectionState:pc?.connectionState??null,iceConnectionState:pc?.iceConnectionState??null,
     inputState:peer.inputDc?.readyState??null,controlState:peer.controlDc?.readyState??null});
    const row={peer:id,requestedAt:performance.now(),completedAt:null,requestIdentity:identity(),completionIdentity:null,
     connectionState:pc?.connectionState,iceConnectionState:pc?.iceConnectionState,selected:null,error:null,
     input:{state:peer.inputDc?.readyState,ordered:peer.inputDc?.ordered,maxRetransmits:peer.inputDc?.maxRetransmits,bufferedAmount:peer.inputDc?.bufferedAmount},
     control:{state:peer.controlDc?.readyState,ordered:peer.controlDc?.ordered,maxRetransmits:peer.controlDc?.maxRetransmits,bufferedAmount:peer.controlDc?.bufferedAmount}};
    let timer;
    try{
     const stats=await Promise.race([pc.getStats(),new Promise((_,reject)=>{timer=setTimeout(()=>reject(Error('RTC stats timeout')),1500);})]);
     let pair=null,selectedVia=null;
     for(const report of stats.values())if(report.type==='transport'&&report.selectedCandidatePairId){pair=stats.get(report.selectedCandidatePairId);if(pair){selectedVia='transport';break;}}
     if(!pair)for(const report of stats.values())if(report.type==='candidate-pair'&&report.state==='succeeded'&&report.nominated){pair=report;selectedVia='nominated';break;}
     if(pair){
      const local=stats.get(pair.localCandidateId),remote=stats.get(pair.remoteCandidateId);
      row.selected={selectedVia,state:pair.state,nominated:!!pair.nominated,localType:local?.candidateType??null,remoteType:remote?.candidateType??null,
       localProtocol:local?.protocol??null,remoteProtocol:remote?.protocol??null,bytesSent:pair.bytesSent??null,bytesReceived:pair.bytesReceived??null,currentRoundTripTime:pair.currentRoundTripTime??null};
     }
    }catch(error){row.error=String(error.message||error).slice(0,200);}
    finally{clearTimeout(timer);row.completedAt=performance.now();row.completionIdentity=identity();}
    return row;
   }));
   const selectedPaths=rtcPeers.map(row=>!row.selected?.localType||!row.selected?.remoteType?'unverified':row.selected.localType==='relay'||row.selected.remoteType==='relay'?'turn':'direct');
   return {...endpoint,routeEvidence:{route:endpoint.route,declaredPath:endpoint.declaredPath,
    selectedPath:endpoint.route==='rtc'?(selectedPaths.length&&selectedPaths.every(path=>path===selectedPaths[0])?selectedPaths[0]:'mixed'):endpoint.route,
    peerCount,peers:rtcPeers,completedAt:performance.now(),ownerCurrent:current(),routeCurrent:sameRoute(),
    scope:'Frozen post-workload endpoint. Native capture and asynchronous RTC query are not atomic; request/completion identities and Page clock brackets are retained. No IP/SDP. This is not the current route after trace drain.'}};
  })().catch(error=>({...endpoint,error:String(error.message||error).slice(0,200),routeEvidence:null}));
 }catch(error){endpoint.error=String(error.message||error).slice(0,200);endpoint.status='failed';endpoint.capturedAt??=performance.now();state.completion=Promise.resolve({...endpoint,routeEvidence:null});}
 state.completion=state.completion.then(value=>{state.completed=value;return value;});
 return endpoint;
}
async function collectCompletedEndpoints(){
 const begun=await Promise.allSettled(pages.map(page=>page.evaluate(beginCompletedEndpoint,{release,schedulerTrace})));
 for(let side=0;side<begun.length;++side){const row=begun[side];endpointSnapshots[side]=row.status==='fulfilled'?{side,begin:row.value,completed:null,error:null}:{side,begin:null,completed:null,error:String(row.reason?.message||row.reason).slice(0,200)};}
 // Begin both stops before awaiting RTC. The small promises can settle while
 // the trace drains; the existing finalizer delays all large artifact work.
 const trace=finishSchedulerTrace('capture-stop');
 const completed=Promise.allSettled(pages.map(async(page,side)=>{
  try{endpointSnapshots[side].completed=await page.evaluate(()=>{const state=window.completedEndpointCapture;if(!state?.completion)throw Error('Endpoint capture unavailable');return state.completion;});}
  catch(error){endpointSnapshots[side].error=String(error.message||error).slice(0,200);}
 }));
 const [traceResult]=await Promise.allSettled([trace,completed]);
 if(traceResult.status==='rejected')endpointTraceError=String(traceResult.reason?.message||traceResult.reason).slice(0,200);
}
function qualifyCompletedEndpoint(endpoint,{limit,fallback,expectedRtcPath}){
 const identityOkay=value=>!!value&&['ownerCurrent','routeCurrent','peerCurrent','pcCurrent','connected'].every(key=>value[key]===true)&&value.inputState==='open'&&value.controlState==='open';
 const native=endpoint?.status==='captured'&&!endpoint.error&&endpoint.ownerCurrent===true&&Array.isArray(endpoint.stats)&&endpoint.stats.length===8&&endpoint.stats.every(Number.isFinite)&&
  Number.isInteger(endpoint.hash)&&Array.isArray(endpoint.sdl)&&endpoint.sdl.length===17&&!!endpoint.audio;
 const workload=!!(native&&endpoint.body?.ended>0&&endpoint.body.samples>0&&endpoint.stats[0]>=limit&&endpoint.stats[1]>=limit&&endpoint.stats[6]);
 const open=!!(endpoint?.body?.closed?.length===0&&endpoint.body.connected&&!endpoint.body.disconnected);
 const route=endpoint?.routeEvidence,expected=fallback?'relay':'rtc';
 const routeOkay=!!(endpoint?.route===expected&&route?.route===expected&&route.ownerCurrent===true&&route.routeCurrent===true);
 const rtc=fallback||!!(route?.peerCount===1&&route.peers?.length===1&&route.peers.every(row=>!row.error&&identityOkay(row.requestIdentity)&&identityOkay(row.completionIdentity)&&row.selected?.state==='succeeded'&&row.selected.localType&&row.selected.remoteType)&&
  (expectedRtcPath==='any'||route.selectedPath===expectedRtcPath));
 return {qualified:!!(native&&workload&&open&&routeOkay&&rtc),native:!!native,workload,open,route:routeOkay,rtc:!!rtc};
}
async function finishSchedulerTrace(reason){
 if(!schedulerTrace)return undefined;
 if(!schedulerTraceFinish)schedulerTraceFinish=(async()=>{
  // Drain both CDP streams before any large synchronous serialization or write.
  // A fast side must not consume the other side's tracingComplete deadline.
  const stopped=await Promise.allSettled(browsers.map(async(_,side)=>{
   const controller=schedulerTraces[side];
   return controller?await controller.stop(reason):{version:1,enabled:true,status:'not-started',collectionValid:false,events:[],errors:[{kind:'trace-controller-unavailable'}]};
  }));
  schedulerTraceEvidence=stopped.map((result,side)=>{
   try{
    if(result.status!=='fulfilled')throw Error('Trace stop failed');
    const snapshot=result.value,path=resolve(out,`${label}-P${side+1}-scheduler-trace.json`);
    writeFileSync(path,JSON.stringify(snapshot),{flag:'wx'});
    const {events,...metadata}=snapshot;
    return {...metadata,side,artifact:{path,bytes:statSync(path).size,sha256:sha(path)}};
   }catch{return {version:1,enabled:true,side,status:'finalization-failed',collectionValid:false,errors:[{kind:'stop-or-artifact-write-failed'}]};}
  });
  return schedulerTraceEvidence;
 })();
 return schedulerTraceFinish;
}
if(headedControl)parameters.headedLaunch={headless:false,explicitArgs:browserLaunchOptions.args,preservedHeadlessAuxiliaryArgs:headlessAuxiliaryArgs,scope:'Window-mode environment control with the optional declared GPU-vsync diagnostic. Proxy, input, audio/context, CPU setup and GPU backend selection are unchanged; see gpuVsyncControl for added args and actual argv qualification.'};
const headedControlEvidence=headedControl?{launches:[],windows:[],windowObservationLimit:6,scope:'Only the two browsers created by this harness. CDP supplies the unique browser PID; CIM reads only that PID. Save only command-line SHA256 and allowlisted flags, never raw command lines, executable/profile paths, or process lists. Missing identity observation is explicit and cannot establish a matched environment control. Window bounds, visibility/focus and existing lifecycle events do not prove physical scanout, continuous foreground visibility, or non-overlap. No window is moved or brought to front.'}:undefined;
function splitWindowsCommandLine(line){
 assert.ok(typeof line==='string'&&line.length>0&&line.length<=32768,'Browser command line unavailable or oversized');
 const args=[];let at=0;
 while(at<line.length){
  while(/[ \t]/.test(line[at]||'\n'))++at;
  if(at===line.length)break;
  let word='',quoted=false;
  while(at<line.length&&(quoted||!/[ \t]/.test(line[at]))){
   let slashes=0;while(line[at]==='\\'){++slashes;++at;}
   if(line[at]==='"'){
    word+='\\'.repeat(Math.floor(slashes/2));
    if(slashes%2){word+='"';++at;}
    else if(quoted&&line[at+1]==='"'){word+='"';at+=2;}
    else{quoted=!quoted;++at;}
   }else{word+='\\'.repeat(slashes);if(at<line.length&&(quoted||!/[ \t]/.test(line[at])))word+=line[at++];}
  }
  assert.ok(!quoted,'Browser command line has unmatched quoting');args.push(word);
 }
 return args;
}
async function readOwnedBrowserProcess(browserPid){
 assert.ok(Number.isSafeInteger(browserPid)&&browserPid>0,'Invalid owned browser PID');
 const script=`[Console]::OutputEncoding = [System.Text.UTF8Encoding]::new($false)
try {
 $th09Process = @(Get-CimInstance -ClassName Win32_Process -Filter 'ProcessId = ${browserPid}' -Property ProcessId,ExecutablePath,CommandLine -ErrorAction Stop)
 if ($th09Process.Count -ne 1) { throw 'process-unavailable' }
 $th09Process[0] | Select-Object ProcessId,ExecutablePath,CommandLine | ConvertTo-Json -Compress -ErrorAction Stop
} catch { [Console]::Out.WriteLine('{"unavailable":"cim-query-failed"}') }`;
 return await new Promise((accept,reject)=>{
  const helper=spawn('powershell.exe',['-NoLogo','-NoProfile','-NonInteractive','-EncodedCommand',Buffer.from(script,'utf16le').toString('base64')],{windowsHide:true,stdio:['ignore','pipe','ignore']});
  if(Number.isInteger(helper.pid))hostProbe?.registerKnownProcess(helper.pid,'headed-identity-query');
  let text='',done=false;
  const finish=(error,value)=>{if(done)return;done=true;clearTimeout(timer);if(error)reject(error);else accept(value);};
  const timer=setTimeout(()=>{helper.kill();finish(Error('owned-browser-cim-timeout'));},5000);
  helper.stdout.setEncoding('utf8');helper.stdout.on('data',chunk=>{if(done)return;text+=chunk;if(text.length>131072){helper.kill();finish(Error('owned-browser-cim-output-limit'));}});
  helper.on('error',()=>finish(Error('owned-browser-cim-start-failed')));
  helper.on('close',code=>{
   if(done)return;
   if(code!==0)return finish(Error('owned-browser-cim-exit'));
   try{const info=JSON.parse(text.replace(/^\uFEFF/,''));if(info.unavailable||info.ProcessId!==browserPid||typeof info.ExecutablePath!=='string'||typeof info.CommandLine!=='string')return finish(Error('owned-browser-cim-identity-unavailable'));finish(null,info);}
   catch{finish(Error('owned-browser-cim-parse-failed'));}
  });
 });
}
async function recordHeadedLaunch(browser,side){
 const row={side,observedAt:Date.now(),explicitArgs:browserLaunchOptions.args,method:'CDP SystemInfo.getProcessInfo browser PID + single-PID Win32_Process CIM',allowedArgs:[],observationAvailable:false,verified:false};headedControlEvidence.launches.push(row);
 if(gpuVsyncControl.explicit)row.gpuVsync=inspectGpuVsyncLaunch(gpuVsyncControl,row);
 let cdp,stage='cdp-session';
 try{
  cdp=await browser.newBrowserCDPSession();stage='cdp-process-info';
  const {processInfo}=await cdp.send('SystemInfo.getProcessInfo');
  const matches=processInfo.filter(info=>info.type==='browser');row.browserProcessMatches=matches.length;
  assert.equal(matches.length,1,'Owned browser PID is not unique');row.browserPid=matches[0].id;
  stage='single-pid-cim';const info=await readOwnedBrowserProcess(row.browserPid);
  stage='command-line-parse';const args=splitWindowsCommandLine(info.CommandLine);
  row.commandLineSha256=createHash('sha256').update(info.CommandLine,'utf8').digest('hex');row.commandLineHashEncoding='UTF-8 of the exact Win32_Process.CommandLine string';
  const allowed=new Set([...browserLaunchOptions.args,'--headless','--headless=new','--headless=old','--enable-automation','--disable-background-timer-throttling','--disable-backgrounding-occluded-windows','--disable-renderer-backgrounding','--disable-frame-rate-limit','--disable-gpu-vsync','--enable-unsafe-swiftshader','--no-sandbox']);
  row.allowedArgs=args.filter(arg=>allowed.has(arg));row.omittedArgumentCount=args.length-row.allowedArgs.length;
  row.executableMatches=realpathSync(info.ExecutablePath).toLowerCase()===browserLaunchOptions.executablePath.toLowerCase();
  row.headlessSwitch=args.some(arg=>/^--headless(?:=|$)/.test(arg));
  row.preservedAuxiliaryArgs=headlessAuxiliaryArgs.every(arg=>args.includes(arg));
  row.requiredArgsPresent=browserLaunchOptions.args.every(arg=>args.includes(arg));row.observationAvailable=true;
  row.verified=row.executableMatches&&!row.headlessSwitch&&row.preservedAuxiliaryArgs&&row.requiredArgsPresent;
  if(gpuVsyncControl.explicit){stage='gpu-vsync-qualification';row.gpuVsync=inspectGpuVsyncLaunch(gpuVsyncControl,{...row,args});row.verified=row.verified&&row.gpuVsync.qualified;assert.ok(row.gpuVsync.qualified,'GPU vsync diagnostic actual argv unqualified: '+row.gpuVsync.reasons.join(','));}
  assert.ok(row.verified,'Headed control launch did not retain the requested executable and auxiliary defaults');
 }catch(error){
  row.errorName=String(error?.name||'Error');
  if(!row.observationAvailable)row.unavailable={stage,errorName:row.errorName,code:/^owned-browser-cim-[a-z-]+$/.test(error?.message)?error.message:'observation-unavailable'};
  if(gpuVsyncControl.explicit){if(!row.observationAvailable)row.gpuVsync=inspectGpuVsyncLaunch(gpuVsyncControl,row);throw Error('GPU vsync diagnostic launch unqualified: '+stage);}
  if(row.observationAvailable)throw error;
 }
 finally{row.observationMs=Date.now()-row.observedAt;if(cdp)await cdp.detach().catch(()=>{});}
}
async function recordHeadedWindow(page,cdp,side,stage){
 const row={side,stage,observedAt:Date.now(),window:null,page:null,errors:[]};
 if(headedControlEvidence.windows.length>=headedControlEvidence.windowObservationLimit)throw Error('Headed window observation overflow');
 headedControlEvidence.windows.push(row);
 try{row.window=await cdp.send('Browser.getWindowForTarget');}catch(error){row.errors.push({source:'window',name:String(error?.name||'Error')});}
 try{row.page=await page.evaluate(()=>({at:performance.now(),timeOrigin:performance.timeOrigin,visibility:document.visibilityState,focus:document.hasFocus(),screenX,screenY,outerWidth,outerHeight,innerWidth,innerHeight,devicePixelRatio,screen:{width:screen.width,height:screen.height,availWidth:screen.availWidth,availHeight:screen.availHeight,availLeft:screen.availLeft,availTop:screen.availTop}}));}catch(error){row.errors.push({source:'page',name:String(error?.name||'Error')});}
}
try {
 await new Promise((accept,reject)=>{const timer=setTimeout(()=>reject(Error('relay startup timeout')),10000);relay.stdout.on('data',b=>{relayLog=(relayLog+b).slice(-32768);if(relayLog.includes('netplay relay listening')){clearTimeout(timer);accept();}});relay.stderr.on('data',b=>relayLog=(relayLog+b).slice(-32768));relay.on('error',reject);relay.on('exit',code=>reject(Error('relay exit '+code)));});
 hostProbe?.mark('relay-listening');
 host=await presentationServer(0,{release,artifactDirectory});hostProbe?.mark('presentation-server-ready');
 for(let side=0;side<2;++side){
  hostProbe?.mark(`browser-${side+1}-setup-start`);
  const browser=await launchBrowser(browserLaunchOptions);browsers.push(browser);
  if(headedControl)await recordHeadedLaunch(browser,side);
  if(gpuVsyncControl.explicit&&side===1){headedControlEvidence.gpuVsync=inspectGpuVsyncRun(gpuVsyncControl,headedControlEvidence.launches);assert.ok(headedControlEvidence.gpuVsync.qualified,'GPU vsync diagnostic requires two qualified launches with distinct owned PIDs');}
  const page=await browser.newPage();pages.push(page);page.on('pageerror',e=>errors.push({side,error:e.stack}));
  if(audioOutput)await page.addInitScript(installAudioOutputProbe);
  if(responsiveness)await page.addInitScript(installResponsivenessProbe);
  if(touchInput)await page.addInitScript(touchModule.installTouchInputProbe,{enabled:true,plan:touchSchedules[side]});
  if(liveCost)await page.addInitScript(installLiveCallbackCostProbe);
  if(rafAnomaly)await page.addInitScript(installRafAnomalyProbe,{enabled:true,side});
  const installSmoothnessHooks=({delay,jitter,dropEvery,side,fallback,burstAt,burstFor,burstDelay,rafDiagnostics,networkTrace,impairmentSite,headedControl})=>{
   if(fallback)window.RTCPeerConnection=undefined;
   const lifecycle=window.gameLifecycle=[];
   const record=event=>lifecycle.push([performance.now(),event,document.visibilityState,document.hasFocus()]);
   record('init');for(const event of ['visibilitychange','freeze','resume'])document.addEventListener(event,()=>record(event));
   for(const event of ['pageshow','pagehide',...(headedControl?['focus','blur']:[])])window.addEventListener(event,()=>record(event));
   const raf=window.requestAnimationFrame.bind(window);
   if(!rafDiagnostics){
    window.requestAnimationFrame=callback=>raf(timestamp=>{
     const previous=window.currentGameRafTimestamp;window.currentGameRafTimestamp=timestamp;
     try{return callback(timestamp);}finally{window.currentGameRafTimestamp=previous;}
    });
   }else{
    // Observe the existing RAF chain; do not create a heartbeat or reschedule work.
    // RAF timestamps and actual callback entry use separate fields deliberately.
    const limits={requests:65536,frames:65536,controls:1024,callbacks:1024,hookErrors:32};
    const requests=[],frames=[],controls=[],callbacks=[],hookErrors=[],pending=new Map(),known=new WeakMap();
    const counts={requested:0,entered:0,exited:0,cancelled:0,gameFrames:0,controlCalls:0};
    let overflow=false,currentRaf=null,currentFrame=null,starting=0,lastPauseRequested=null,hookedCore=null,frameHook=null;
    const keep=(list,item,max)=>{if(list.length>=max){overflow=true;return null;}list.push(item);return item;};
    const problem=(target,reason)=>keep(hookErrors,{target,reason},limits.hookErrors);
    const callbackInfo=callback=>{
     let info=known.get(callback);
     if(!info){let name='';try{name=String(callback.name||'').slice(0,120);}catch{}
      info={id:callbacks.length,name,game:!!starting};known.set(callback,info);
      if(!keep(callbacks,info,limits.callbacks))info.id=null;
     }else if(starting)info.game=true;
     return info;
    };
    const rafHook=callback=>{
     const requestedAt=performance.now(),info=callbackInfo(callback);
     ++counts.requested;
     const row=keep(requests,{index:requests.length,nativeId:null,parentIndex:currentRaf?.index??null,
      callbackId:info.id,requestedAt,enteredAt:null,exitedAt:null,rafTimestamp:null,cancelledAt:null,
      frameCalls:0,firstFrameIndex:null,lastFrameIndex:null,frameHookAtEntry:null,frameHookAtExit:null,
      pauseAtEntry:null,pauseAtExit:null,requestThrew:false,callbackThrew:false},limits.requests);
     let nativeId;
     try{nativeId=raf(timestamp=>{
      const enteredAt=performance.now(),previous=currentRaf,previousTimestamp=window.currentGameRafTimestamp;
      ++counts.entered;pending.delete(nativeId);currentRaf=row;window.currentGameRafTimestamp=timestamp;
      if(row){row.enteredAt=enteredAt;row.rafTimestamp=timestamp;row.pauseAtEntry=lastPauseRequested;
       row.frameHookAtEntry=hookedCore?hookedCore.onGameFrame===frameHook:null;}
      try{return callback(timestamp);}catch(error){if(row)row.callbackThrew=true;throw error;}
      finally{
       const exitedAt=performance.now();++counts.exited;
       if(row){row.exitedAt=exitedAt;row.pauseAtExit=lastPauseRequested;
        row.frameHookAtExit=hookedCore?hookedCore.onGameFrame===frameHook:null;}
       currentRaf=previous;window.currentGameRafTimestamp=previousTimestamp;
      }
     });}catch(error){if(row)row.requestThrew=true;throw error;}
     if(row){row.nativeId=nativeId;pending.set(nativeId,row);}
     return nativeId;
    };
    const cancel=window.cancelAnimationFrame.bind(window);
    const cancelHook=id=>{
     const row=pending.get(id);++counts.cancelled;
     const result=cancel(id);
     if(row){row.cancelledAt=performance.now();pending.delete(id);}return result;
    };
    window.requestAnimationFrame=rafHook;window.cancelAnimationFrame=cancelHook;
    const replace=(object,key,factory)=>{
     const descriptor=Object.getOwnPropertyDescriptor(object,key);
     if(!descriptor||!('value' in descriptor)||!descriptor.writable||typeof descriptor.value!=='function'){
      problem(key,'Expected an own writable function; hook skipped');return null;
     }
     const wrapped=factory(descriptor.value);object[key]=wrapped;return wrapped;
    };
    window.rafDiagnosticProbe={
     attach(core,play,m){
      if(hookedCore){problem('attach','Already attached');return;}hookedCore=core;
      const originalPlayFrame=play.frame;
      if(typeof originalPlayFrame==='function'&&Object.isExtensible(play)){
       play.frame=function(...args){
        const frame=currentFrame;if(frame)frame.playEnteredAt=performance.now();
        try{return Reflect.apply(originalPlayFrame,this,args);}
        finally{if(frame)frame.playExitedAt=performance.now();}
       };
      }else problem('play.frame','Hook skipped');
      frameHook=replace(core,'onGameFrame',original=>function(...args){
       const enteredAt=performance.now(),previous=currentFrame;++counts.gameFrames;
       const frame=keep(frames,{index:frames.length,rafIndex:currentRaf?.index??null,enteredAt,exitedAt:null,
        cppWorkMs:args[1],ok:args[0],playEnteredAt:null,playExitedAt:null,sampleAt:null,afterSampleAt:null,
        sampleBefore:m.samples.length,sampleAfter:null,activeBefore:play.active,activeAfter:null,
        endedBefore:!!m.ended,endedAfter:null,threw:false},limits.frames);
       currentFrame=frame;
       if(currentRaf){++currentRaf.frameCalls;
        if(frame){currentRaf.firstFrameIndex??=frame.index;currentRaf.lastFrameIndex=frame.index;}
        const info=callbacks[currentRaf.callbackId];if(info)info.game=true;}
       try{return Reflect.apply(original,this,args);}catch(error){if(frame)frame.threw=true;throw error;}
       finally{
        if(frame){frame.exitedAt=performance.now();frame.sampleAfter=m.samples.length;
         frame.activeAfter=play.active;frame.endedAfter=!!m.ended;}currentFrame=previous;
       }
      });
      for(const name of ['_th09_loop_start','_th09_loop_pause','_th09_loop_stop']){
       replace(core,name,original=>function(...args){
        const enteredAt=performance.now();++counts.controlCalls;
        const row=keep(controls,{name,enteredAt,exitedAt:null,argument:typeof args[0]==='number'?args[0]:null,
         rafIndex:currentRaf?.index??null,active:play.active,finished:play.finished,connected:play.connected,
         recovering:!!play.recovering,visibility:document.visibilityState,threw:false},limits.controls);
        if(name==='_th09_loop_start')++starting;
        if(name==='_th09_loop_pause')lastPauseRequested=typeof args[0]==='number'?args[0]:null;
        try{return Reflect.apply(original,this,args);}catch(error){if(row)row.threw=true;throw error;}
        finally{if(row)row.exitedAt=performance.now();if(name==='_th09_loop_start')--starting;}
       });
      }
     },
     onSample(sample,started){
      if(currentFrame){currentFrame.sampleAt=started+sample[0];currentFrame.afterSampleAt=performance.now();}
     },
     peek(){
      // Page-local references only. The anomaly helper returns compact fields,
      // never these arrays; at most four pending Map entries are inspected.
      const pendingIndices=[],iterator=pending.values();
      for(let i=0;i<4;++i){const item=iterator.next();if(item.done)break;pendingIndices.push(item.value.index);}
      return {version:1,snapshotAt:performance.now(),valid:!overflow&&!hookErrors.length&&window.requestAnimationFrame===rafHook&&window.cancelAnimationFrame===cancelHook&&(!hookedCore||hookedCore.onGameFrame===frameHook),
       counts,lastPauseRequested,pendingCount:pending.size,pendingTruncated:pending.size>pendingIndices.length,
       pendingIndices,callbacks,requests,frames,controls};
     },
     snapshot(){
      return {version:1,clock:'Page performance.now(); rafTimestamp is the browser supplied frame timestamp, not entry time.',
       scope:'Existing RAF dispatch and JS/WASM callback boundaries; not physical scanout. pause fields record requested state, not a native state read.',
       snapshotAt:performance.now(),limits,overflow,valid:!overflow&&!hookErrors.length&&window.requestAnimationFrame===rafHook&&window.cancelAnimationFrame===cancelHook&&(!hookedCore||hookedCore.onGameFrame===frameHook),counts,
       rafHookCurrent:window.requestAnimationFrame===rafHook,cancelHookCurrent:window.cancelAnimationFrame===cancelHook,
       frameHookCurrent:hookedCore?hookedCore.onGameFrame===frameHook:null,lastPauseRequested,
       pendingIndices:Array.from(pending.values(),row=>row.index),callbacks,requests,frames,controls,hookErrors};
     }
    };
   }
   const stats=window.impairment={matched:0,sent:0,dropped:0,pending:0,peakPending:0,burstPackets:0,bursts:[],planned:[],delivered:[],lanes:{}};
   let seed=0x915ab+side;
   const prototype=fallback?WebSocket.prototype:RTCDataChannel.prototype,send=prototype.send;
   prototype.send=function(bytes){
    if(typeof bytes==='string')return send.call(this,bytes);
    const packet=ArrayBuffer.isView(bytes)?new Uint8Array(bytes.buffer,bytes.byteOffset,bytes.byteLength):new Uint8Array(bytes);
    const data=packet[0]===0xe7?packet.subarray(2):packet;
    // Already-confirmed production checks, also available without dev exports.
    if(data.length===20&&data[0]===84&&data[1]===57&&data[2]===72&&data[3]===67){
     const view=new DataView(data.buffer,data.byteOffset,data.byteLength);
     (window.confirmedHashTrace??=[]).push([view.getUint32(12,true),view.getUint32(16,true)]);
    }
    if(data[0]!==69||data[5]!==1)return send.call(this,bytes);
    window.observeLocalInput?.(data);
    if(networkTrace)window.networkTransportProbe.onSendIntent(packet,this);
    const lane=fallback?'relay':this.label,n=++stats.matched;stats.lanes[lane]=(stats.lanes[lane]||0)+1;
    if(impairmentSite!=='client'){
     // The no-timer path calls the original transport in this task with the
     // original bytes, receiver, return value and synchronous exception.
     if(stats.planned.length>=200000)throw Error('synchronous send sample overflow');
     const began=performance.now(),result=send.call(this,bytes);
     ++stats.sent;stats.syncCalls=(stats.syncCalls||0)+1;stats.planned.push(0);stats.delivered.push(performance.now()-began);
     return result;
    }
    if(lane.includes('input')&&dropEvery&&n%dropEvery===0){++stats.dropped;return;}
    seed=(Math.imul(seed,1664525)+1013904223)>>>0;
    const began=performance.now(),age=window.measure?.started?began-window.measure.started:-1;
    const burst=burstFor>0&&age>=burstAt&&age<burstAt+burstFor;
    if(burst){++stats.burstPackets;stats.bursts.push([age,window.readStats?.()[0]??null,!!window.measure?.ended,lane]);}
    const planned=Math.max(0,delay+(seed/0x100000000*2-1)*jitter)+(burst?burstDelay:0),copy=packet.slice();
    stats.planned.push(planned);++stats.pending;stats.peakPending=Math.max(stats.peakPending,stats.pending);
    if(stats.pending>1024||stats.planned.length>200000)throw Error('impairment sample/queue overflow');
    setTimeout(()=>{--stats.pending;if(this.readyState==='open'||this.readyState===WebSocket.OPEN){send.call(this,copy);++stats.sent;stats.delivered.push(performance.now()-began);}},planned);
   };
   if(networkTrace)window.networkTransportProbe.expectSendWrapper(WebSocket.prototype.send);
  };
  const hookOptions={delay,jitter,dropEvery,side,fallback,burstAt,burstFor,burstDelay,rafDiagnostics,networkTrace,impairmentSite,headedControl};
  // A single init script makes the native WS observer the send captured by impairment.
  if(networkTrace||startupTrace){
   const scripts=[];
   if(startupTrace)scripts.push('('+installStartupTransportProbe.toString()+')('+JSON.stringify({enabled:true,side,relayPort:port,rtcEnabled:!fallback})+');');
   if(networkTrace)scripts.push('('+installNetworkTransportProbe.toString()+')('+JSON.stringify({enabled:true,side})+');');
   scripts.push('('+installSmoothnessHooks.toString()+')('+JSON.stringify(hookOptions)+');');
   if(startupTrace)scripts.push('window.startupTransportProbe.expectSendWrapper(WebSocket.prototype.send);');
   await page.addInitScript({content:scripts.join('\n')});
  }else await page.addInitScript(installSmoothnessHooks,hookOptions);
  await page.goto(host.url);
  if(liveCost)await page.evaluate(()=>{window.onProbeModuleReady=module=>window.liveCallbackCostProbe.attach(module);});
  const graphicsRow=graphicsIdentity[side];graphicsRow.status='open-and-graphics-pending';graphicsRow.requestedAt=performance.now();
  try{
   graphicsRow.graphics=await page.evaluate(async()=>{
    await openProbe(0,1,2,3,true);
    const ownerDocument=document,ownerCore=window.core;
    let ownerCanvas,ownerContext,source,contextCaptureError=null;
    try{
     ownerCanvas=ownerCore?.canvas;
     const fromCore=ownerCore?.ctx,fromCanvas=ownerCanvas?.GLctxObject?.GLctx;
     if(fromCore&&fromCanvas&&fromCore!==fromCanvas)contextCaptureError='Existing core and canvas context references disagree';
     else{ownerContext=fromCore||fromCanvas;source=fromCore?(fromCanvas?'core.ctx+core.canvas.GLctxObject.GLctx':'core.ctx'):(fromCanvas?'core.canvas.GLctxObject.GLctx':null);}
    }catch(error){contextCaptureError=String(error?.message||error);}
    // Use only already-bound references. Never create a canvas or call getContext.
    window.readSmoothnessGraphicsIdentity=()=>{
     const row={clock:'Page performance.now()',timeOrigin:performance.timeOrigin,sampledAt:performance.now(),returnedAt:null,
      available:false,reason:null,source:source||null,renderer:null,vendor:null,rendererSource:null};
     try{
      const currentCore=window.core,currentCanvas=currentCore?.canvas;
      const fromCore=currentCore?.ctx,fromCanvas=currentCanvas?.GLctxObject?.GLctx;
      row.readiness={pageReady:window.ready===true,corePresent:!!currentCore,gameMetricsAvailable:typeof currentCore?._th09_game_metrics==='function'};
      row.identity={documentSame:document===ownerDocument,coreSame:currentCore===ownerCore,canvasSame:currentCanvas===ownerCanvas,
       canvasInDocument:!!ownerCanvas&&ownerCanvas.ownerDocument===ownerDocument,canvasConnected:!!ownerCanvas?.isConnected,
       expectedCanvasSame:document.querySelector('canvas')===ownerCanvas,contextCanvasSame:!!ownerContext&&ownerContext.canvas===ownerCanvas,
       currentContextReferencePresent:!!(fromCore||fromCanvas),
       currentContextReferencesSame:!!ownerContext&&!!(fromCore||fromCanvas)&&(!fromCore||fromCore===ownerContext)&&(!fromCanvas||fromCanvas===ownerContext)};
      row.canvas=ownerCanvas?{id:ownerCanvas.id,width:ownerCanvas.width,height:ownerCanvas.height}:null;
      if(contextCaptureError){row.reason=contextCaptureError;return row;}
      if(!ownerContext){row.reason='No existing core.ctx or canvas.GLctxObject.GLctx reference';return row;}
      if(!Object.values(row.identity).every(Boolean)){row.reason='Captured core/canvas/context owner no longer matches';return row;}
      if(typeof ownerContext.getParameter!=='function'||typeof ownerContext.getExtension!=='function'){row.reason='Existing context is not a readable WebGL context';return row;}
      row.contextType=Object.prototype.toString.call(ownerContext);
      row.contextLost=typeof ownerContext.isContextLost==='function'?ownerContext.isContextLost():null;
      if(row.contextLost){row.reason='Existing WebGL context is lost';return row;}
      const ext=ownerContext.getExtension('WEBGL_debug_renderer_info');
      row.rendererSource=ext?'WEBGL_debug_renderer_info':'WebGL masked RENDERER/VENDOR';
      row.renderer=ownerContext.getParameter(ext?ext.UNMASKED_RENDERER_WEBGL:ownerContext.RENDERER);
      row.vendor=ownerContext.getParameter(ext?ext.UNMASKED_VENDOR_WEBGL:ownerContext.VENDOR);
      row.available=typeof row.renderer==='string'&&row.renderer.length>0&&typeof row.vendor==='string'&&row.vendor.length>0;
      if(!row.available)row.reason='Existing context returned unavailable renderer or vendor';
      return row;
     }catch(error){row.reason='Existing context observation failed';row.error=String(error?.message||error);return row;}
     finally{row.returnedAt=performance.now();}
    };
    return window.readSmoothnessGraphicsIdentity();
   });
   graphicsRow.status=graphicsRow.graphics.available?'captured':'unavailable';
  }catch(error){graphicsRow.status='open-or-identity-failed';graphicsRow.error=String(error?.stack||error);throw error;}
  finally{graphicsRow.returnedAt=performance.now();}
  if(liveCost)await page.evaluate(()=>window.liveCallbackCostProbe.mark('after-open'));
  const cdp=await page.context().newCDPSession(page);sessions.push(cdp);
  await cdp.send('Emulation.setCPUThrottlingRate',{rate:cpuRatesBySide[side]});
  if(liveCost)await page.evaluate(()=>window.liveCallbackCostProbe.mark('cpu-throttle-configured'));
  if(headedControl)await recordHeadedWindow(page,cdp,side,'setup-complete');
  hostProbe?.mark(`browser-${side+1}-setup-complete`);
 }
 if(schedulerTrace){
  const {startChromeSchedulerTrace}=await import('./chrome-scheduler-trace.mjs');
  const started=await Promise.allSettled(browsers.map((browser,side)=>startChromeSchedulerTrace({enabled:true,profile:schedulerTraceProfile,browser,page:pages[side],ownedBrowsers:browsers,side,sourceFiles:{harness:fileURLToPath(import.meta.url)}})));
  for(let side=0;side<started.length;++side)if(started[side].status==='fulfilled')schedulerTraces[side]=started[side].value;
  assert.ok(started.every(row=>row.status==='fulfilled'),'Scheduler trace controller initialization failed');
  assert.ok(schedulerTraces.every(controller=>{const state=controller.snapshot();return state.availability.available===true&&state.status==='recording';}),'Scheduler trace unavailable before gameplay');
 }
 hostProbe?.mark('netplay-setup-start');
 await pages[0].evaluate(async ({port,members})=>{
  const wait=async test=>{const start=performance.now();while(!test()){const closed=window.lobbies?.find(ws=>ws.readyState===WebSocket.CLOSED);if(closed)throw Error('lobby closed '+JSON.stringify(closed.failure));if(performance.now()-start>10000)throw Error('lobby timeout');await new Promise(r=>setTimeout(r,10));}};
  window.lobbies=[];
  for(const [side,id] of ['smoothnessleft','smoothnessright'].entries()){const ws=new WebSocket(`ws://127.0.0.1:${port}/?room=th09mp-0998&lobby=${id}&member=${members[side]}&intent=${side?'join':'create'}&players=2&difficulty=3`);ws.onclose=e=>{ws.failure={code:e.code,reason:e.reason};};ws.onmessage=e=>{const m=JSON.parse(e.data);if(m.type==='error')throw Error(m.error);ws.latest=m;};lobbies.push(ws);await wait(()=>ws.readyState===WebSocket.OPEN&&ws.latest);}
  lobbies[0].send(JSON.stringify({type:'take-seat',seat:0,loadout:0}));await wait(()=>lobbies[0].latest.room?.seats[0]);
  lobbies[1].send(JSON.stringify({type:'take-seat',seat:1,loadout:1}));await wait(()=>lobbies[0].latest.room?.seats[1]);
  for(const ws of lobbies)ws.send(JSON.stringify({type:'set-ready',ready:true}));await wait(()=>lobbies[0].latest.room?.seats.slice(0,2).every(s=>s?.ready));
  lobbies[0].send(JSON.stringify({type:'start'}));await wait(()=>lobbies[0].latest.type==='start');
 },{port,members});
 for(let side=0;side<2;++side)await pages[side].evaluate(async ({side,port,limit,inputMode,member,responsiveness,touchInput,rafDiagnostics,networkTrace,liveCost,renderWork,startupTrace,schedulerTrace})=>{
  const {SharedNetplay}=await import('/app/shared-netplay.mjs');
  const options={netplayUrl:`ws://127.0.0.1:${port}/?room=th09mp-0998&run=1&players=2&player=${side}&member=${member}`,netplayPlayer:side,netplayPlayerCount:2,netplaySeed:12345,netplayDifficulty:3,netplayLoadouts:[{character:0},{character:1}],netplayIceServers:[],netplaySpectatorCount:0};
  if(touchInput)Object.assign(options,{netplayAdonisMode:0,netplayInputDelay:0,netplayInputDelayAuto:false});
  const m=window.measure={samples:[],frameDetails:[],networkPacing:[],loads:[],notices:[],closed:[],inputTape:[],started:0,ended:0,limit,audioStart:0,finished:false};
  if(renderWork){m.graphicsMetrics=[];m.graphicsMetricsOverflow=false;}
  core.eaglerOptions=options;window.play=new SharedNetplay(core,{onStatus:s=>m.notices.push(s),onClose:s=>{m.closed.push(s);window.responsivenessProbe?.stop();window.touchInputProbe?.stop();},onResult:()=>{m.finished=true;window.responsivenessProbe?.stop();window.touchInputProbe?.stop();}});
  window.readStats=()=>Array.from(core.HEAPU32.subarray(core._th09_rollback_info()/4,core._th09_rollback_info()/4+8));
  const updateInput=now=>{
   if(inputMode==='tape'||responsiveness||touchInput)return;
   const age=now-m.started,cycle=Math.floor(age/360)+side;
   // Physical intent continues while simulation is late; no frame-driven drag.
   for(const [scan,down] of [[29,age<2200],[44,age%2100<1500],[42,age%3000<750],[203,cycle%4===0],[205,cycle%4===2]])core._th09_key(scan,+down);
  };
  if(responsiveness)window.observeLocalInput=data=>window.responsivenessProbe.onLocalInput(data);
  if(touchInput)window.observeLocalInput=data=>window.touchInputProbe.onLocalInput(data);
  if(inputMode==='tape'){
   // Keep the production keyboard sampler and protocol untouched. A new local
   // packet is sent synchronously by LocalCaptured, so set the NEXT frame's
   // hosted keys there, even if the callback will simulate several fresh ticks.
   // Validate every actual wire sample, not merely our intended key sequence.
   const keys=frame=>{const cycle=Math.floor(frame/22)+side;return frame===0?0:
    (frame<132?256:0)|(frame%126<90?1:0)|(frame%180<45?4:0)|(cycle%4===0?64:0)|(cycle%4===2?128:0);};
   window.observeLocalInput=data=>{
    const view=new DataView(data.buffer,data.byteOffset,data.byteLength),count=data[42];
    if(data.length<44||data[2]!==78||data[3]!==80||data[6]!==side||data.length!==44+count*12)throw Error('Unexpected input packet schema');
    const first=view.getUint32(32,true);
    for(let i=0;i<count;++i){const frame=first+i,at=44+i*12,buttons=view.getUint16(at,true);
     if(frame>limit+120||buttons!==keys(frame)||view.getUint16(at+2,true)!==0||view.getFloat32(at+4,true)!==0||view.getFloat32(at+8,true)!==0)
      throw Error(`Input tape mismatch P${side+1} frame ${frame}: ${buttons} != ${keys(frame)}`);
     if(frame===m.inputTape.length)m.inputTape.push(buttons);
     else if(frame>m.inputTape.length||m.inputTape[frame]!==buttons)throw Error('Input tape gap/rewrite');
    }
    const next=keys(m.inputTape.length);
    for(const [scan,mask] of [[29,256],[44,1],[42,4],[203,64],[205,128]])core._th09_key(scan,+!!(next&mask));
   };
  }
  core.onNetworkSpectatorFrame=(...args)=>play.publish(...args);core.onNetworkResult=()=>play.result();
  core.onGameFrame=(ok,ms)=>{
   window.audioOutputProbe?.observe(core.SDL3,play.active&&!m.ended);
   play.frame();if(!play.active||m.ended)return;const now=performance.now();
   if(!m.started){m.started=now;m.audioStart=core.SDL3?.audioContext?.currentTime||0;if(responsiveness)window.responsivenessProbe.start(core,side);if(touchInput)window.touchInputProbe.start(core,side);if(startupTrace)window.startupTransportProbe.mark('first-active');if(schedulerTrace)window.chromeSchedulerTraceProbe?.mark('first-active');}
   updateInput(now);const s=readStats(),at=core._th09_game_metrics()/4,n=core.HEAPU32[at+9],phase=sessionStatus()[0];
   if(responsiveness)window.responsivenessProbe.onFrame(s,n,phase,now);
   if(touchInput)window.touchInputProbe.onFrame(s,n,phase,now);
   if(s[0]>=180&&!m.sdlStart){const p=core._sdl_stats()/4;m.sdlStart=Array.from(core.HEAPU32.subarray(p,p+17));}
   if(!Number.isFinite(window.currentGameRafTimestamp))throw Error('Game callback outside measured requestAnimationFrame');
   m.samples.push([now-m.started,ok,ms,...s,n,phase,window.currentGameRafTimestamp]);
   if(renderWork){
    if(m.graphicsMetrics.length>=50000){m.graphicsMetricsOverflow=true;throw Error('render work sample overflow');}
    m.graphicsMetrics.push([m.samples.length-1,...core.HEAPU32.subarray(at,at+10)]);
   }
   if(liveCost)window.liveCallbackCostProbe.onFrame({sampleIndex:m.samples.length-1,frame:s[0],confirmed:s[1],present:n,phase,callbackWorkMs:ms,measuredAt:now});
   // Window completeness needs the native frontier even when the heavier
   // RAF instrumentation is disabled. Keep these six ABI words independent.
   if(core._th09_network_pacing_info){const at=core._th09_network_pacing_info()/4;
    m.networkPacing.push([m.samples.length-1,...core.HEAPU32.subarray(at,at+6)]);}
   if(rafDiagnostics)window.rafDiagnosticProbe.onSample(m.samples.at(-1),m.started);
   const adonis=core._th09_adonis_info()/4;
   const tickCost=core._th09_probe_tick_cost?.();
   // These are WorldState saves, unlike samples[7] (local input captures).
   // Development tick costs describe the LAST tick, not a callback total.
   m.frameDetails.push([s[0],core.HEAPU32[adonis+2],...(tickCost?new Float64Array(core.HEAPU8.buffer,tickCost,2):[])]);
   if(core._th09_probe_load&&m.samples.length%30===0){const at=core._th09_probe_load()/4;m.loads.push([s[0],...core.HEAPU32.subarray(at,at+20)]);}
   if(m.samples.length>50000)throw Error('performance sample overflow');
   if(s[0]>=limit&&s[1]>=limit){m.ended=now;window.responsivenessProbe?.stop(now);window.touchInputProbe?.stop(now);}
   if(networkTrace)window.networkTransportProbe.observePacing(core,'frame',m.samples.length-1);
  };
  if(networkTrace)window.networkTransportProbe.attachCore(core);
  if(rafDiagnostics)window.rafDiagnosticProbe.attach(core,play,m);
  core._th09_probe_frame_limit?.(limit);core._th09_loop_start();core._th09_loop_pause(1);
  if(startupTrace)window.startupTransportProbe.mark('before-connect');
  await play.connect(options);
  if(startupTrace)window.startupTransportProbe.mark('after-connect');
 },{side,port,limit,inputMode,member:members[side],responsiveness,touchInput,rafDiagnostics,networkTrace,liveCost,renderWork,startupTrace,schedulerTrace});
 if(profile)for(const cdp of sessions){await cdp.send('Profiler.enable');await cdp.send('Profiler.setSamplingInterval',{interval:500});await cdp.send('Profiler.start');}
 hostProbe?.mark('formal-poll-start');
 const began=Date.now(),timeout=limit/60*1000*3+20000;
 let status;
 while(Date.now()-began<timeout){
  const poll=rafAnomaly?{index:rafAnomalyNodePolls.attempted++,requestedAt:performance.now(),returnedAt:null,sides:[]}:null;
  if(poll)rafAnomalyNodePolls.inFlight=poll;
  status=await Promise.all(pages.map(async(p,side)=>{
   const bracket=poll?{side,requestedAt:performance.now(),returnedAt:null,pagePoll:null,failed:false}:null;
   if(bracket)poll.sides[side]=bracket;
   try{
    const row=await p.evaluate(enabled=>{const stats=readStats();return {stats,ended:measure.ended,closed:measure.closed,finished:measure.finished,
     rafAnomaly:enabled?window.rafAnomalyProbe?.observe({stats}):undefined};},rafAnomaly);
    if(bracket)bracket.pagePoll=row.rafAnomaly?.poll??null;return row;
   }catch(error){if(bracket)bracket.failed=true;throw error;}
   finally{if(bracket)bracket.returnedAt=performance.now();}
  }));
  if(poll){poll.returnedAt=performance.now();rafAnomalyNodePolls.last=poll;rafAnomalyNodePolls.inFlight=null;
   if(rafAnomalyNodePolls.rows.length<rafAnomalyNodePolls.limit)rafAnomalyNodePolls.rows.push(poll);else ++rafAnomalyNodePolls.overflow;}
  if(errors.length||status.some(s=>s.closed.length))throw Error('runtime failure '+JSON.stringify({errors,status}));
  if(status.every(s=>s.ended))break;
  if(status.some(s=>s.finished))throw Error('match ended before requested workload '+JSON.stringify(status));
  await new Promise(r=>setTimeout(r,100));
 }
 assert.ok(status.every(s=>s.ended),'incomplete workload '+JSON.stringify(status));
 hostProbe?.mark('formal-poll-complete');
 await collectCompletedEndpoints();
 if(profile)for(let side=0;side<2;++side){const result=await sessions[side].send('Profiler.stop');writeFileSync(resolve(out,`${label}-P${side+1}.cpuprofile`),JSON.stringify(result.profile));}
 const results=await Promise.all(pages.map((p,side)=>p.evaluate(({endpoint,rafAnomaly})=>{
  const postDrain=window.completedEndpointCapture?.postDrain()??null;
  const gl=document.querySelector('canvas').getContext('webgl2'),ext=gl.getExtension('WEBGL_debug_renderer_info');
  const renderer=ext?gl.getParameter(ext.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER);
  window.networkTransportProbe?.stop('complete');
  const startupTransport=window.startupTransportProbe?.snapshot({stop:true});
  return {measurementEndpoint:endpoint,postDrain,stats:endpoint?.stats??null,hash:endpoint?.hash??null,audited:endpoint?.audited??null,
   route:endpoint?.route??null,routeEvidence:endpoint?.routeEvidence??null,renderer,audio:endpoint?.audio??null,measure,impairment,memoryBytes:endpoint?.memoryBytes??null,startupTransport,
   audioOutput:window.audioOutputProbe?.snapshot(),responsiveness:window.responsivenessProbe?.snapshot(),touchInput:window.touchInputProbe?.snapshot(),rafDiagnostics:window.rafDiagnosticProbe?.snapshot(),rafAnomaly:rafAnomaly?window.rafAnomalyProbe?.snapshot({final:true,stats:postDrain?.stats}):undefined,networkTransport:window.networkTransportProbe?.snapshot(),liveCallbackCosts:window.liveCallbackCostProbe?.snapshot(),lifecycle:window.gameLifecycle,sdl:endpoint?.sdl??null,confirmedHashes:window.confirmedHashTrace||[]};
 },{endpoint:endpointSnapshots[side]?.completed??endpointSnapshots[side]?.begin??null,rafAnomaly})));
 retainedResults=results;hostProbe?.mark('browser-sample-capture-complete');
 measurementEndpointQualification=results.map((result,side)=>{const row=qualifyCompletedEndpoint(result.measurementEndpoint,{limit,fallback,expectedRtcPath}),captureError=endpointSnapshots[side]?.error??null;return {...row,side,captureError,qualified:row.qualified&&!captureError};});
 teardownClean=results.length===2&&results.every(result=>result.postDrain?.teardownClean===true);
 if(renderWork)for(const r of results){
  assert.equal(r.measure.graphicsMetricsOverflow,false,'Render work sample overflow');
  assert.equal(r.measure.graphicsMetrics.length,r.measure.samples.length,'Render work sample count mismatch');
  assert.ok(r.measure.graphicsMetrics.every((row,index)=>row.length===11&&row[0]===index&&row[10]===r.measure.samples[index][11]),'Render work sample identity/presentation mismatch');
 }
 if(headedControl)await Promise.all(pages.map((page,side)=>recordHeadedWindow(page,sessions[side],side,'measurement-complete')));
 if(diagnosticRelay)relayTraceSnapshot=await diagnosticRelay.snapshot({stop:true});
 if(relayDiagnostics){
  assert.ok(relayTraceSnapshot?.valid,JSON.stringify({errors:relayTraceSnapshot?.errors,overflow:relayTraceSnapshot?.overflow}));
  if(relayInputDiagnostics)assert.ok(relayTraceSnapshot.coverage.exercised,'Relay input trace not exercised');
  assert.ok(relayTraceSnapshot.coverage.connectionHookExercised,'Relay connection hook not exercised');
  assert.equal(relayTraceSnapshot.sourceIdentity.relaySha256,identity.relay);
  assert.equal(relayTraceSnapshot.sourceIdentity.probeSha256,identity.relayTransportProbe);
  assert.equal(relayTraceSnapshot.sourceIdentity.fixtureSha256,identity.relayTransportFixture);
  assert.equal(relayTraceSnapshot.sourceIdentity.startupProbeSha256,identity.relayStartupProbe);
  if(startupTrace)assert.ok(relayTraceSnapshot.startup?.valid&&relayTraceSnapshot.startup.coverage.exercised,'Relay startup trace invalid or unexercised');
 }
 if(startupTrace)for(const r of results){assert.ok(r.startupTransport?.valid,JSON.stringify({errors:r.startupTransport?.errors,overflow:r.startupTransport?.overflow,identity:r.startupTransport?.identity}));assert.ok(r.startupTransport.coverage.signalingExercised,'Browser startup signaling not exercised');}
 assert.equal(errors.length,0);
 assert.ok(!endpointTraceError,'Endpoint trace finalization failed: '+endpointTraceError);
 assert.ok(measurementEndpointQualification.length===2&&measurementEndpointQualification.every(row=>row.qualified),'Measurement endpoint unavailable or ineligible '+JSON.stringify(measurementEndpointQualification));
 assert.ok(results.every(r=>r.route===(fallback?'relay':'rtc')));
 if(!fallback)for(const r of results){
  assert.ok(r.routeEvidence.peers.length===1&&r.routeEvidence.peers.every(p=>p.selected?.state==='succeeded'&&p.selected.localType&&p.selected.remoteType&&!p.error),'RTC selected candidate evidence unavailable');
  if(expectedRtcPath!=='any')assert.equal(r.routeEvidence.selectedPath,expectedRtcPath,'Unexpected actual RTC candidate path');
 }
 for(const r of results){assert.doesNotMatch(r.renderer,/swiftshader|llvmpipe|microsoft basic render/i);assert.ok(r.impairment.matched>100&&r.impairment.sent>100,'Input impairment not exercised on requested route');assert.ok(r.audio.advancedSeconds>1);}
 const sameFrame=results[0].stats[0]===results[1].stats[0];
 // Equal speculative frame numbers do not make two predictions comparable.
 // The release ABI deliberately has no frame-limit hook: accept its complete
 // confirmed trace, and compare the frozen endpoint only when fully published.
 const finalComparable=sameFrame&&results.every(r=>r.stats[1]>=r.stats[0]&&r.stats[7]>=r.stats[0]);
 const confirmedChecks=Math.floor(limit/120);
 for(const r of results){
  assert.ok(r.confirmedHashes.length>=confirmedChecks,'Missing production confirmed checks');
  for(let i=0;i<confirmedChecks;++i)assert.equal(r.confirmedHashes[i][0],(i+1)*120,'Missing/reordered confirmed frame');
 }
 assert.deepEqual(results[0].confirmedHashes.slice(0,confirmedChecks),results[1].confirmedHashes.slice(0,confirmedChecks),'Confirmed hash trace diverged');
 if(audioOutput)for(const r of results){assert.ok(r.audioOutput?.blocks.length>100&&r.audioOutput.nonzeroBlocks>100,'Actual audio output not exercised');assert.ok(!r.audioOutput.overflow);}
 if(responsiveness)for(const r of results){assert.ok(r.responsiveness?.valid,JSON.stringify(r.responsiveness?.errors));assert.ok(r.responsiveness.counts.planned>10&&r.responsiveness.counts.captured>10,'Independent input response was not exercised');}
 if(touchInput)for(const r of results){
  const p=r.touchInput;assert.ok(p?.valid,JSON.stringify({errors:p?.errors,overflow:p?.overflow}));
  assert.equal(p.configuration.profile,touchProfile);assert.equal(p.configuration.sourceHz,touchSourceHz);
  assert.equal(p.configuration.pattern??'lifecycle',touchPattern);
  assert.ok(p.clock.startedAt!==null&&p.counts.analogActiveFrames>=100&&p.counts.targetChanges>=50&&p.counts.positionChangedObservations>=20,'Continuous native touch load was not exercised');
  assert.ok(p.captures.length>=limit,'Missing actual local touch input tape');
  for(let i=0;i<limit;++i)assert.equal(p.captures[i].frame,i,'Incomplete touch tape frame prefix');
  r.touchInputTapeSha256=createHash('sha256').update(JSON.stringify(p.captures.slice(0,limit).map(c=>[c.frame,c.rawHex]))).digest('hex');
 }
 if(rafDiagnostics)for(const r of results){assert.ok(r.rafDiagnostics?.valid,JSON.stringify(r.rafDiagnostics?.hookErrors));assert.ok(r.rafDiagnostics.counts.gameFrames>100&&r.rafDiagnostics.callbacks.some(c=>c.game),'Game RAF diagnostics were not exercised');}
 if(rafAnomaly)for(const r of results)assert.ok(r.rafAnomaly?.valid,JSON.stringify(r.rafAnomaly?.errors));
 if(networkTrace)for(const r of results){assert.ok(r.networkTransport?.valid,JSON.stringify({errors:r.networkTransport?.errors,overflow:r.networkTransport?.overflow,identity:r.networkTransport?.identity}));assert.ok(r.networkTransport.coverage.exercised,'Network transport diagnostics were not exercised');}
 if(liveCost)for(const r of results){assert.ok(r.liveCallbackCosts?.valid,JSON.stringify(r.liveCallbackCosts?.errors));assert.ok(r.liveCallbackCosts.frames.length>100,'Live callback costs not exercised');}
 if(burstFor&&burstDelay){
  if(impairmentSite==='client')for(const r of results)assert.ok(r.impairment.bursts.some(b=>b[1]>=180&&b[1]<limit&&!b[2]),'Network burst did not overlap the measured workload');
  else if(impairmentSite==='relay'){
   const packets=new Map(relayTraceSnapshot.packets.map(row=>[row[0],row]));
   for(let side=0;side<2;++side)assert.ok(relayTraceSnapshot.events.some(e=>{const p=packets.get(e[2]);return e[1]===1&&e[10]&&p?.[3]===side&&p[10]>=180&&p[10]<limit;}),'Relay burst did not overlap each sender measured frame range');
  }
 }
 if(inputMode==='tape')for(const r of results){assert.ok(r.measure.inputTape.length>=limit);r.inputTapeSha256=createHash('sha256').update(JSON.stringify(r.measure.inputTape.slice(0,limit))).digest('hex');}
 if(!release)assert.ok(finalComparable,'Diagnostic frame limit did not reach a fully confirmed common endpoint');
 if(finalComparable){assert.equal(results[0].hash,results[1].hash);if(!release)assert.deepEqual(results[0].audited,results[1].audited);}
 // Preserve the historical frame-filtered calculation for report consumers and
 // exact comparison with frozen reports; it is not the corrected window.
 const legacySummary=results.map(r=>{
  const samples=r.measure.samples.filter(s=>s[3]>=180&&s[3]<limit),gaps=[],completionGaps=[],rafGaps=[],effectiveGaps=[];let prior,effectivePrior;
  for(let i=1;i<samples.length;++i)if(samples[i][13]!==samples[i-1][13])rafGaps.push(samples[i][13]-samples[i-1][13]);
  for(const s of samples)if(!prior||s[11]!==prior[11]){if(prior){gaps.push(s[13]-prior[13]);completionGaps.push(s[0]-prior[0]);}prior=s;}
  for(const s of samples)if((!effectivePrior||s[11]!==effectivePrior[11])&&(!effectivePrior||s[3]>effectivePrior[3])){if(effectivePrior)effectiveGaps.push(s[13]-effectivePrior[13]);effectivePrior=s;}
  const first=samples[0],last=samples.at(-1),elapsed=last[0]-first[0];
  const fields=['calls','batches','uploadBytes','readBytes','frames','presentations','bufferReplacements','bufferSubUpdates','vertexUploadBytes','directBytes','copiedBytes','layoutSetups','textureBinds','framebufferBinds','programCompiles','genericBatches','resamples'];
  r.sdlMeasured=Object.fromEntries(fields.map((name,i)=>[name,r.sdl[i]-r.measure.sdlStart[i]]));
  return {logicHz:(last[3]-first[3])*1000/elapsed,elapsedMs:elapsed,firstFrame:first[3],lastFrame:last[3],callbacks:samples.length,waitCallbacks:samples.filter(s=>s[1]===2).length,corrections:last[5]-first[5],resimulated:last[6]-first[6],captures:last[7]-first[7],callbackWorkMs:distribution(samples.map(s=>s[2])),presentationGapMs:distribution(gaps),effectivePresentationGapMs:distribution(effectiveGaps),callbackCompletionGapMs:distribution(completionGaps),rafGapMs:distribution(rafGaps),plannedDelayMs:distribution(r.impairment.planned),deliveredDelayMs:distribution(r.impairment.delivered),scenes:[...new Set(samples.map(s=>s[12]))]};
 });
 const summary=results.map(r=>summarizeSmoothnessWindow(r,{startFrame:180,endFrame:limit}));
 const summaryMethod={schema:SMOOTHNESS_WINDOW_SCHEMA,primary:'summary: continuous first-cross frontier window; see each window for completeness, terminal closure and all startup/tail costs',legacy:'legacySummary: original next/cursor frame-filtered calculation, retained verbatim; may splice non-adjacent samples after rollback'};
 assert.equal(sha(wasmPath),identity.wasm,'WASM changed during run');assert.equal(sha(relayPath),identity.relay);
 assert.equal(sha(fileURLToPath(new URL('./smoothness-window.mjs',import.meta.url))),identity.smoothnessWindow,'Smoothness window helper changed during run');
 assert.equal(sha(fileURLToPath(new URL('./gpu-vsync-control.mjs',import.meta.url))),identity.gpuVsyncControl,'GPU vsync control helper changed during run');
 if(rafAnomaly){identity.rafAnomalyProbeAfter=sha(fileURLToPath(new URL('./raf-anomaly-probe.mjs',import.meta.url)));assert.equal(identity.rafAnomalyProbeAfter,identity.rafAnomalyProbe,'RAF anomaly helper changed during run');}
 assert.equal(sha(resolve(root,'tests/browser/presentation.html')),identity.presentationHarness,'Presentation fixture changed during run');
 if(touchInput)assert.equal(sha(fileURLToPath(new URL('./touch-input-probe.mjs',import.meta.url))),identity.touchInputProbe,'Touch helper changed during run');
 hostProbe?.mark('success-report-ready');const hostLoadSnapshot=await hostProbe?.stop();
 const report={passed:true,passScope:'Measured workload and frozen endpoint gates; clean post-drain state is reported separately by teardownClean.',measurementEndpointQualification,teardownClean,endpointSnapshots,scope:'Two separate desktop Chromium processes; native GPU; real '+(fallback?'WebSocket relay':'RTC')+' with '+(impairmentSite==='client'?'seeded application-send impairment on input and repair lanes':impairmentSite==='relay'?'synchronous client send and seeded relay-process input impairment; burst anchored to relay first input receive':'synchronous client send; no injected send timers')+'; '+(inputMode==='tape'?'wire-verified frame-indexed hosted-key tape':'wall-clock synthetic keyboard')+(audioOutput?'; sampled SDL output blocks, not speaker/acoustic acceptance':'; audio clock only, not acoustic acceptance'),presentationMeasurement:'RAF timestamps of callbacks with a new renderer submission; NOT physical scanout. Completion gaps are reported separately because varying callback work is not itself a missed vsync.',identity,parameters,graphicsIdentity,browsers:browsers.map(b=>b.version()),sameFrame,finalComparable,confirmedChecks,summaryMethod,summary,legacySummary,results,errors,relayLog,relayTransport:relayTraceSnapshot,hostLoad:hostLoadSnapshot};
 if(headedControl){headedControlEvidence.identityVerified=headedControlEvidence.launches.length===2&&headedControlEvidence.launches.every(row=>row.verified);if(gpuVsyncControl.explicit){headedControlEvidence.gpuVsync=inspectGpuVsyncRun(gpuVsyncControl,headedControlEvidence.launches);headedControlEvidence.identityVerified=headedControlEvidence.identityVerified&&headedControlEvidence.gpuVsync.qualified;}report.headedControl=headedControlEvidence;}
 if(touchInput)report.scope=report.scope.replace('wall-clock synthetic keyboard','independent Page-clock synthetic native ABI '+touchProfile+' touch; conditional wire-to-render progress, not physical-phone or Launcher bridge evidence');
 if(schedulerTrace)report.chromeSchedulerTrace=schedulerTraceEvidence;
 if(rafAnomaly)report.rafAnomalyNodePolls=rafAnomalyNodePolls;
 writeFileSync(resolve(out,label+'-report.json'),JSON.stringify(report,null,2));console.log(JSON.stringify({identity,parameters,measurementEndpointQualification,teardownClean,sameFrame,summary,hashes:results.map(r=>r.hash)}));
}catch(error){
 hostProbe?.mark('failure-observed');
 if(schedulerTrace)try{await finishSchedulerTrace('failure');}catch{schedulerTraceEvidence??=[{status:'finalization-failed',collectionValid:false}];}
 const hostLoadSnapshot=await hostProbe?.stop();
 if(diagnosticRelay)try{relayTraceSnapshot=await diagnosticRelay.snapshot({stop:true});}catch(error){relayTraceError=String(error.stack||error);}
 const partial=await Promise.all(pages.map(async p=>{try{return await p.evaluate(enabled=>{window.responsivenessProbe?.stop();window.touchInputProbe?.stop();window.networkTransportProbe?.stop('failure');const stats=window.readStats?.();return {measurementEndpoint:window.completedEndpointCapture?.completed??window.completedEndpointCapture?.begin??null,postDrain:window.completedEndpointCapture?.postDrain()??null,graphics:window.readSmoothnessGraphicsIdentity?.()??{available:false,reason:'After-open graphics reader unavailable',ready:window.ready===true,corePresent:!!window.core},startupTransport:window.startupTransportProbe?.snapshot({stop:true}),measure:window.measure,impairment:window.impairment,stats,audioOutput:window.audioOutputProbe?.snapshot(),responsiveness:window.responsivenessProbe?.snapshot(),touchInput:window.touchInputProbe?.snapshot(),rafDiagnostics:window.rafDiagnosticProbe?.snapshot(),rafAnomaly:enabled?window.rafAnomalyProbe?.snapshot({final:true,stats}):undefined,networkTransport:window.networkTransportProbe?.snapshot(),liveCallbackCosts:window.liveCallbackCostProbe?.snapshot(),lifecycle:window.gameLifecycle,confirmedHashes:window.confirmedHashTrace};},rafAnomaly);}catch{return null;}}));
 if(headedControl)await Promise.all(pages.map((page,side)=>recordHeadedWindow(page,sessions[side],side,'failure-observed')));
 if(headedControl){headedControlEvidence.identityVerified=headedControlEvidence.launches.length===2&&headedControlEvidence.launches.every(row=>row.verified);if(gpuVsyncControl.explicit){headedControlEvidence.gpuVsync=inspectGpuVsyncRun(gpuVsyncControl,headedControlEvidence.launches);headedControlEvidence.identityVerified=headedControlEvidence.identityVerified&&headedControlEvidence.gpuVsync.qualified;}}
 if(rafAnomaly)try{identity.rafAnomalyProbeAfter=sha(fileURLToPath(new URL('./raf-anomaly-probe.mjs',import.meta.url)));}catch{identity.rafAnomalyProbeAfter=null;}
 writeFileSync(resolve(out,label+'-failure.json'),JSON.stringify({error:error.stack,identity,parameters,graphicsIdentity,endpointSnapshots,endpointTraceError,measurementEndpointQualification,teardownClean,partial,results:retainedResults,errors,relayLog,relayTransport:relayTraceSnapshot,relayTraceError,hostLoad:hostLoadSnapshot,headedControl:headedControlEvidence,chromeSchedulerTrace:schedulerTraceEvidence,rafAnomalyNodePolls},null,2));throw error;
}finally{
 if(schedulerTrace)await finishSchedulerTrace('finally').catch(()=>{});
 await hostProbe?.stop();
 if(diagnosticRelay)await diagnosticRelay.snapshot({stop:true}).catch(()=>{});
 for(const browser of browsers)await browser.close();if(host){host.netplay.close();await new Promise(r=>host.server.close(r));}relay.kill();
}
