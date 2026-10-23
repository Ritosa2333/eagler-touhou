// Root-scheduled, bounded desktop integration gate. No phone / production-site writes.
// Real production package, managed Host input, real relay admission and native loop.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {spawn,spawnSync} from 'node:child_process';
import {readFileSync,writeFileSync,mkdirSync,existsSync,createReadStream,statSync} from 'node:fs';
import {pipeline} from 'node:stream';
import {resolve,dirname,extname,relative,isAbsolute,sep} from 'node:path';
import {pathToFileURL} from 'node:url';
import {loadPlaywright} from '../../../../tools/browser/playwright.mjs';
import {expectedDisconnectConsole} from './acceptance-errors.mjs';
import {createHash,randomUUID} from 'node:crypto';
import {performance} from 'node:perf_hooks';

const root=resolve(import.meta.dirname,'../..');
function option(name,fallback){const i=process.argv.indexOf(name);if(i<0)return fallback;assert.ok(process.argv[i+1]&&!process.argv[i+1].startsWith('--'),'Missing '+name);return process.argv[i+1];}
const duration=Number(option('--duration','15')),cpu=Number(option('--cpu','1'));
const headed=process.argv.includes('--headed');
const peerCpu=Number(option('--peer-cpu',String(cpu))),cpuRates=[cpu,peerCpu];
const routeOption=option('--route','rtc'),route=routeOption==='direct'?'rtc':routeOption;
const delayLocation=option('--delay-location','sender');
const profile=option('--profile','stable'),disconnect=process.argv.includes('--disconnect');
const nativeProfile=process.argv.includes('--native-profile');
const audioProfile=process.argv.includes('--audio-profile');
const cpuSamples=process.argv.includes('--cpu-samples');
const referenceOption=option('--replay-reference-runtime','');
const referenceRuntime=referenceOption?resolve(referenceOption):null;
const replayOracle=process.argv.includes('--replay-oracle')||!!referenceRuntime;
const pixelOracle=process.argv.includes('--pixel-oracle');
assert.ok(!pixelOracle||replayOracle,'Pixel oracle requires exact Replay oracle');
const stationary=process.argv.includes('--diagnostic-stationary');
const touchCarrier=option('--touch-carrier','rpc');
assert.ok(['rpc','launcher'].includes(touchCarrier),'touch carrier: rpc or actual launcher helper');
const latency=Number(option('--latency-ms','50')),jitter=Number(option('--jitter-ms','5'));
const prediction=12,maxGap=Number(option('--max-gap-ms','100'));
assert.ok(Number.isFinite(duration)&&duration>=5&&duration<=60,'duration: 5..60 seconds');
assert.ok([1,4].includes(cpu),'CPU must be 1 or 4');
assert.ok([1,4].includes(peerCpu),'Peer CPU must be 1 or 4');
assert.ok(['rtc','relay'].includes(route),'route: rtc/direct or relay');
assert.ok(['stable','spike'].includes(profile),'profile: stable or spike');
assert.ok(['sender','relay'].includes(delayLocation),'delay-location: sender or relay');
assert.ok(delayLocation!=='relay'||(route==='relay'&&profile==='stable'),'Independent relay impairment requires --route relay --profile stable');
assert.ok(latency>=0&&latency<=200&&jitter>=0&&jitter<=25&&maxGap>=30&&maxGap<=1000);
const launcher=resolve(option('--launcher-root',process.env.EAGLER_LAUNCHER_ROOT||resolve(root,'../../launcher')));
const runtime=resolve(option('--runtime',process.env.TH11_MP_PACKAGE||resolve(root,'build-eagler-multiplayer')));
const archive=resolve(process.env.TH11_TEST_ARCHIVE||resolve(root,'../th11/assets-ogg/th11.data'));
const stamp=new Date().toISOString().replaceAll(':','-').replaceAll('.','-');
const out=resolve(option('--output',resolve(root,'artifacts/rollback-smoothness/browser-'+route+'-'+cpu+'x-'+profile+'-'+stamp)));
assert.ok(!existsSync(resolve(out,'receipt.json')),'Never overwrite an earlier good or bad receipt');
mkdirSync(out,{recursive:true});
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const fileIdentity=path=>({path,bytes:readFileSync(path).length,sha256:hash(readFileSync(path))});
const pause=ms=>new Promise(r=>setTimeout(r,ms));
const report={schema:'th11-rollback-browser/1',startedAt:new Date().toISOString(),completedAt:null,
  status:'running',passed:false,root,launcher,runtime,archive,out,scope:{desktop:true,independentChromeProcesses:2,
    managedHostInput:true,realNativeLoop:true,actualGpu:true,pixelMotion:false,fullWorldOracle:false,
    launcherUi:false,phone:false,performanceObserver:true},settings:{duration,cpu,routeRequested:routeOption,profile,
    oneWaySendDelayMs:latency,jitterPlusMinusMs:jitter,prediction,maxGapMs:maxGap,disconnect,nativeProfile,audioProfile,cpuSamples,cpuRates,delayLocation,replayOracle,stationary,touchCarrier},
  stages:[],errors:[],cleanup:[],browser:[],processIds:[],network:[],http:[],results:[]};
const save=()=>writeFileSync(resolve(out,'receipt.json'),JSON.stringify(report,null,2)+'\n');
report.settings.headed=headed;
const mark=(name,extra={})=>{report.stages.push({name,checkerMs:performance.now(),...extra});save();};
const observeError=entry=>{if(report.stages.at(-1)?.name==='cleanup-start')report.cleanup.push({action:'owned-shutdown-event',...entry});else report.errors.push(entry);};
save();
let relay,server;const browsers=[],contexts=[],pages=[],cdps=[],relayHeartbeats=[];
let relayLog='',relayLogOverflow=false,firstFailure=null;
const totalBudgetMs=(300+duration)*1000;
const watchdog=setTimeout(()=>{
  report.watchdog={exceeded:true,totalBudgetMs,checkerMs:performance.now(),error:'Overall bounded browser gate timed out'};
  report.status='failed';report.passed=false;process.exitCode=1;save();
  // Closing only the owned browsers rejects pending evaluate calls in a wedged
  // renderer. Never kill-by-name or touch an unrelated browser/relay process.
  if(relay&&relay.exitCode===null)relay.kill();
  void Promise.allSettled(browsers.map(browser=>browser.close()));
},totalBudgetMs);
report.totalBudgetMs=totalBudgetMs;
function gitIdentity(path){const run=args=>{const r=spawnSync('git',['-C',path,...args],{encoding:'utf8',windowsHide:true,maxBuffer:4*1024*1024});return {exitCode:r.status,stdout:r.stdout?.trim(),stderr:r.stderr?.trim()};};return {path,head:run(['rev-parse','HEAD']),status:run(['status','--porcelain=v1','--untracked-files=normal'])};}
function blocked(message){const e=Error(message);e.blocked=true;throw e;}
function percentile(values,p){if(!values.length)return null;const a=[...values].sort((a,b)=>a-b);return a[Math.min(a.length-1,Math.floor((a.length-1)*p))];}
function distribution(values){return {count:values.length,p50:percentile(values,.5),p95:percentile(values,.95),p99:percentile(values,.99),
  max:values.length?Math.max(...values):null,over33:values.filter(x=>x>33.4).length,over50:values.filter(x=>x>50).length,over100:values.filter(x=>x>100).length};}
const RB={version:0,allows:1,canCapture:2,correcting:3,rollbackFrame:4,predictionFrontier:5,captureFrame:6,committedThrough:7,
  records:8,journalFrames:9,restores:10,historyTicks:11,peakDepth:12,peakBytes:13,peakBlocks:14,networkWait:15,cpuYield:16,
  barrierFrame:17,touchCached:18,touchFrame:19,revision:20,presents:21,publishedExclusive:22,error:23};
report.rollbackStatus=RB;
const PROFILE_NAMES=['version','enabled','callback','restore','capture','update','text','draw','seal','confirmedOutput','exactTick','backendCommit','audioPump','initialNetPump','historyTicks','forwardTicks'];
report.nativeProfileBins={layout:PROFILE_NAMES,units:'wall milliseconds for bins2..13; counts14..15',defaultEnabled:false};
const AUDIO_PROFILE_NAMES=['version','words','enabled','callbackSerial','pumpCalls','mixCalls','putSuccess',
  'mixedFrames','queuedFrames','pumpMs','mixMs','putMs','queueMs','queueReads','queueBeforeFrames','queueAfterFrames','mixFailures','putFailures'];
if(audioProfile){
  report.scope.diagnosticOnly=true;
  report.audioProfileBins={layout:AUDIO_PROFILE_NAMES,defaultEnabled:false,requested:true,
    milliseconds:[9,10,11,12],counts:[4,5,6,13,16,17],pcmFrames:[7,8,14,15],queueUnavailable:-1,
    semantics:'Per native rAF totals including every internal pump; pumpMs includes mix/put/queue time and must not be summed with them.',
    correspondence:'audio callbackSerial and JS rAF id are distinct serials bound by the same recorded onGameFrame; rollback profile[2] is milliseconds, not a serial.',
    observer:'Diagnostic clocks, extra native queue read and JS 18-double copy are enabled; this run is not unobserved performance acceptance.',
    raw:'peer-N.json probe.raf[].audioProfile; paired rollback profile remains independently optional'};
}

// Installed before WASM instantiation, in each real document. This changes only
// transport scheduling / observation, never packet bytes or native input state.
function installProbe(config){
  const state={timeOrigin:performance.timeOrigin,phase:'idle',start:0,end:0,raf:[],transport:[],motion:[],tail:[],
    overflow:[],errors:[],lastMotion:0,current:null,serial:0,wireSerial:0,queued:0,queuedBytes:0,maxQueued:0,
    maxQueuedBytes:0,realDelay:config.latency,profile:config.profile,attached:false,settleStarted:0};
  const timers=new Set(),orderedDue=new WeakMap(),originalRaf=requestAnimationFrame.bind(globalThis);
  let random=(0x713311+config.seat*1009)>>>0;
  const push=(list,value,max=20000)=>{if(list.length<max)list.push(value);else if(!state.overflow.includes(list===state.raf?'raf':list===state.transport?'transport':'motion'))state.overflow.push(list===state.raf?'raf':list===state.transport?'transport':'motion');};
  function rnd(){random=(Math.imul(random,1664525)+1013904223)>>>0;return random/4294967296;}
  function bytes(data){if(data instanceof ArrayBuffer)return new Uint8Array(data.slice(0));if(ArrayBuffer.isView(data))return new Uint8Array(data.buffer.slice(data.byteOffset,data.byteOffset+data.byteLength));return null;}
  function signature(b){let v=2166136261;for(let i=0;i<b.length;++i)v=Math.imul(v^b[i],16777619)>>>0;return v;}
  function send(original,socket,data,kind,ordered){
    const payload=bytes(data);if(!payload)return original.call(socket,data);
    const now=performance.now(),measured=state.phase==='measure',elapsed=now-state.start;
    let extra=0;
    if(measured&&state.profile==='spike'){
      const span=config.duration*1000;
      if(elapsed>=span*.32&&elapsed<span*.32+250)extra=150;
      if(elapsed>=span*.66&&elapsed<span*.66+350)extra=300;
    }
    const delay=state.realDelay>0?Math.max(0,state.realDelay+(rnd()*2-1)*config.jitter+extra):0;
    let due=now+delay;
    if(ordered){due=Math.max(due,orderedDue.get(socket)||0);orderedDue.set(socket,due+.001);}
    const id=++state.wireSerial,entry={id,kind,phase:state.phase,scheduled:now,due,requestedDelay:delay,extra,
      size:payload.length,signature:signature(payload),actual:null,lateness:null,readyState:socket.readyState};
    if(state.phase==='idle'&&payload.length<=64)entry.startupBytes=Array.from(payload);
    push(state.transport,entry);
    if(++state.queued>1024||(state.queuedBytes+=payload.length)>4*1024*1024){state.errors.push('bounded transport injection queue overflow');throw Error('acceptance transport queue overflow');}
    state.maxQueued=Math.max(state.maxQueued,state.queued);state.maxQueuedBytes=Math.max(state.maxQueuedBytes,state.queuedBytes);
    const deliver=()=>{state.queued--;state.queuedBytes-=payload.length;entry.actual=performance.now();entry.lateness=entry.actual-due;
      try{if(socket.readyState!=='open'&&socket.readyState!==1){entry.cancelled='channel closed before due';return;}original.call(socket,payload);}
      catch(e){entry.error=String(e);state.errors.push(String(e));}};
    if(!delay&&due<=now){deliver();return;}
    const timer=setTimeout(()=>{timers.delete(timer);deliver();},Math.max(0,due-now));timers.add(timer);
  }
  if(globalThis.WebSocket){const original=WebSocket.prototype.send;WebSocket.prototype.send=function(data){return send(original,this,data,'ws',true);};}
  if(config.route==='relay')Object.defineProperty(globalThis,'RTCPeerConnection',{value:undefined,configurable:true});
  if(globalThis.RTCDataChannel){const original=RTCDataChannel.prototype.send;RTCDataChannel.prototype.send=function(data){return send(original,this,data,'rtc:'+this.label,this.ordered);};}
  globalThis.requestAnimationFrame=function(callback){
    const requested=performance.now(),id=++state.serial;
    return originalRaf(stamp=>{
      const row={id,requested,stamp,entry:performance.now(),exit:null,nativeMs:null,nativeTicks:null,
        observerMs:0,net:null,rollback:null,phase:state.phase};
      const previous=state.current;state.current=row;
      try{return callback(stamp);}finally{row.exit=performance.now();state.current=previous;
        if(row.nativeMs!==null&&['measure','drain','disconnect'].includes(row.phase))push(state.raf,row);}
    });
  };
  const words=(name,count)=>{const core=globalThis.core;if(!core?.[name])return [];const p=core[name]();return p?Array.from(new Uint32Array(core.memory.buffer,p,count)):[];};
  const nativeText=name=>{const core=globalThis.core;if(!core?.[name])return '';const p=core[name]();if(!p)return '';const bytes=new Uint8Array(core.memory.buffer);let end=p;while(end<bytes.length&&bytes[end])++end;return new TextDecoder().decode(bytes.subarray(p,Math.min(end,p+4096)));};
  function small(){return {timeOrigin:state.timeOrigin,now:performance.now(),phase:state.phase,nativePhase:globalThis.core?.th11_phase?.(),net:words('th11_mp_status',24),
    rollback:words('th11_mp_rollback_status',24),audio:words('th11_audio_statistics',9),raster:words('sdl_raster_stats',9),recovery:words('th11_mp_recovery_statistics',8),
    pacing:globalThis.core?.th11_mp_pacing_statistics?Array.from(new Float64Array(core.memory.buffer,core.th11_mp_pacing_statistics(),8)):[],error:nativeText('th11_mp_error')||nativeText('th11_error'),
    route:globalThis.__eaglerPeerTransport?.route||null,disconnected:!!globalThis.__eaglerPeerTransport?.disconnected,
    shellError:document.querySelector('#error')?.textContent||'',queued:state.queued,overflow:state.overflow,probeErrors:state.errors};}
  function readAudioProfile(mode){
    if(typeof core.th11_audio_pump_profile!=='function')throw Error('--audio-profile requires th11_audio_pump_profile; no fallback');
    const pointer=core.th11_audio_pump_profile(mode),memory=core.memory.buffer;
    if(!Number.isInteger(pointer)||pointer<=0||pointer%8||pointer+18*8>memory.byteLength)
      throw Error('--audio-profile returned an invalid 18-double pointer');
    const value=Array.from(new Float64Array(memory,pointer,18));
    const counts=[3,4,5,6,7,8,13,16,17];
    if(value[0]!==1||value[1]!==18||value[2]!==1||!value.every(Number.isFinite)||
      counts.some(i=>!Number.isInteger(value[i])||value[i]<0)||
      [9,10,11,12].some(i=>value[i]<0)||[14,15].some(i=>!Number.isInteger(value[i])||value[i]<-1)){
      state.audioProfileFailure={at:performance.now(),mode,pointer,value};
      throw Error('--audio-profile ABI/version/enabled/value mismatch: '+JSON.stringify(value));
    }
    return value;
  }
  globalThis.__rollbackAcceptance={
    state,small,
    attach(){
      if(state.attached)return;state.attached=true;
      if(config.nativeProfile){if(!core.th11_mp_rollback_profile)throw Error('Missing native profile ABI');core.th11_mp_rollback_profile(1);}
      if(config.audioProfile){
        const value=readAudioProfile(1);
        state.audioProfileObservation={enabledAt:performance.now(),first:value,lastCallback:value[3],samples:0,unmapped:0};
      }
      const prior=Module.onGameFrame;
      if(config.nativeProfile&&core.th11_mp_runtime_cost)core.th11_mp_runtime_cost(1);
      const recoveryBegin=Module.onRecoveryTaskStart,recoveryEnd=Module.onRecoveryTaskEnd;
      let recoveryPrevious=null;
      Module.onRecoveryTaskStart=requested=>{
        recoveryPrevious=state.current;const now=performance.now();
        state.current={id:++state.serial,kind:'recovery-task',requested,stamp:now,entry:now,exit:null,nativeMs:null,nativeTicks:null,observerMs:0,net:null,rollback:null,phase:state.phase};
        recoveryBegin?.(requested);
      };
      Module.onRecoveryTaskEnd=()=>{
        const row=state.current;try{recoveryEnd?.();}finally{row.exit=performance.now();state.current=recoveryPrevious;
          if(row.nativeMs!==null&&['measure','drain','disconnect'].includes(row.phase))push(state.raf,row);}
      };
      Module.onGameFrame=(ok,ms,count)=>{
        const shellEntry=performance.now(),row=state.current;
        if(row){row.nativeMs=ms;row.nativeTicks=count;row.ok=ok;}
        prior?.(ok,ms,count);
        if(row)row.shellHookMs=performance.now()-shellEntry;
        const entry=performance.now();
        if(config.audioProfile){
          const value=readAudioProfile(-1),watch=state.audioProfileObservation;
          const readMs=performance.now()-entry;
          if(row){row.audioProfile=value;row.audioProfileReadMs=readMs;}
          else{++watch.unmapped;throw Error('--audio-profile onGameFrame had no active rAF record');}
          if(value[3]<=watch.lastCallback)throw Error('--audio-profile callback serial did not advance');
          watch.lastCallback=value[3];++watch.samples;
        }
        if(!['measure','drain','disconnect'].includes(state.phase))return;
        const net=words('th11_mp_status',24),rollback=words('th11_mp_rollback_status',24);
        if(row){row.net=net;row.rollback=rollback;
          if(config.nativeProfile){row.profile=Array.from(new Float64Array(core.memory.buffer,core.th11_mp_rollback_profile(-1),16));row.renderer=words('sdl_stats',17);
            if(core.th11_mp_runtime_cost)row.detail=Array.from(new Float64Array(core.memory.buffer,core.th11_mp_runtime_cost(-1),64));}}
        const now=performance.now();
        if(state.phase==='drain'||now-state.lastMotion>=100){
          const game=words('th11_mp_game_status',56),sample={at:now,callback:row?.id??null,net,rollback,game,audio:words('th11_audio_statistics',9)};
          state.lastMotion=now;push(state.motion,sample,6000);
          // Exact snapshots are observed only after native correction has completed
          // and every input through this post-tick state is confirmed.
          if(state.phase==='drain'&&game.length===56&&game[3]===2&&net[4]!==0xffffffff&&net[4]+1>=game[4]&&
             !rollback[3]&&!rollback[23]&&rollback[0]===1){
            if(globalThis.__framebufferProof)
              sample.pixelSample=globalThis.__framebufferProof.capture(net[12]+':'+game[4]);
            push(state.tail,sample,1200);
          }
        }
        if(row)row.observerMs+=performance.now()-entry;
      };
      const transport=globalThis.__eaglerPeerTransport;
      if(transport&&!transport.__rollbackObserved){
        const receive=transport.receiveBinary;
        transport.receiveBinary=function(data){const b=bytes(data);if(b)push(state.transport,{kind:'receive',phase:state.phase,at:performance.now(),size:b.length,signature:signature(b),...(state.phase==='idle'&&b.length<=64?{startupBytes:Array.from(b)}:{})});return receive.call(this,data);};
        transport.__rollbackObserved=true;
      }
    },
    start(){state.phase='measure';state.start=performance.now();state.end=state.start+config.duration*1000;state.lastMotion=0;},
    drain(){state.phase='drain';state.realDelay=0;state.settleStarted=performance.now();return small();},
    stop(){state.phase='settled';return small();},
    lifecycle(){state.phase='disconnect';return small();},
    result(){return state;},
    close(){for(const timer of timers)clearTimeout(timer);timers.clear();},
  };
}

function canonical(sample){return {frame:sample.game[4],generation:sample.net[12],values:[...sample.game.slice(1,5),...sample.game.slice(8)]};}
function summarizeAudioProfile(rows){
  const valid=rows.filter(r=>r.audioProfile?.length===18&&r.audioProfile[0]===1&&r.audioProfile[1]===18&&r.audioProfile[2]===1);
  const compact=values=>({count:values.length,p50:percentile(values,.5),p95:percentile(values,.95),p99:percentile(values,.99),
    max:values.length?Math.max(...values):null});
  const fields=Object.fromEntries(AUDIO_PROFILE_NAMES.slice(4).map((name,i)=>{
    const index=i+4,values=valid.map(r=>r.audioProfile[index]);
    return [name,compact(index===14||index===15?values.filter(x=>x>=0):values)];
  }));
  const serialErrors=valid.slice(1).filter((r,i)=>r.audioProfile[3]<=valid[i].audioProfile[3]).length;
  const pumpCalls=valid.reduce((n,r)=>n+r.audioProfile[4],0),mixCalls=valid.reduce((n,r)=>n+r.audioProfile[5],0);
  const mixFailures=valid.reduce((n,r)=>n+r.audioProfile[16],0),putFailures=valid.reduce((n,r)=>n+r.audioProfile[17],0);
  return {diagnosticOnly:true,rows:rows.length,samples:valid.length,missing:rows.length-valid.length,serialErrors,
    coverageComplete:rows.length>0&&valid.length===rows.length&&!serialErrors&&pumpCalls>0&&mixCalls>0,
    firstSerial:valid[0]?.audioProfile[3]??null,lastSerial:valid.at(-1)?.audioProfile[3]??null,
    pumpCalls,mixCalls,mixFailures,putFailures,
    queueBeforeUnavailable:valid.filter(r=>r.audioProfile[14]===-1).length,
    queueAfterUnavailable:valid.filter(r=>r.audioProfile[15]===-1).length,
    readWallMs:compact(valid.map(r=>r.audioProfileReadMs)),fields,
    worstPump:valid.toSorted((a,b)=>b.audioProfile[9]-a.audioProfile[9]).slice(0,5).map(r=>({
      id:r.id,entry:r.entry,exit:r.exit,rafTimestamp:r.stamp,nativeMs:r.nativeMs,nativeTicks:r.nativeTicks,
      observerMs:r.observerMs,audioProfileReadMs:r.audioProfileReadMs,audioProfile:r.audioProfile,
      profile:r.profile,net:r.net,rollback:r.rollback}))};
}
function analysis(data){
  const rows=data.raf.filter(r=>r.phase==='measure'&&r.entry>=data.start&&r.entry<=data.end);
  const motion=data.motion.filter(r=>r.at>=data.start&&r.at<=data.end);
  const gaps=values=>values.slice(1).map((v,i)=>v-values[i]);
  let lastPresent=null,highFrame=null;const presents=[],frontiers=[];
  for(const r of rows){const rb=r.rollback;
    if(rb?.length===24){if(lastPresent!==null&&rb[21]>lastPresent)presents.push(r.exit);lastPresent=rb[21];}
    if(r.net?.length===24){if(highFrame!==null&&r.net[3]>highFrame)frontiers.push(r.exit);highFrame=Math.max(highFrame??0,r.net[3]);}
  }
  const elapsed=rows.length>1?rows.at(-1).exit-rows[0].entry:0;
  const logical=rows.length>1?Math.max(0,rows.at(-1).net[3]-rows[0].net[3]):0;
  const first=rows[0]?.rollback||[],last=rows.at(-1)?.rollback||[];
  const deltas={restores:(last[10]??0)-(first[10]??0),historyTicks:(last[11]??0)-(first[11]??0),
    presentations:(last[21]??0)-(first[21]??0)};
  const callbackFlags={networkWait:rows.filter(r=>r.rollback?.[15]===1).length,
    cpuYield:rows.filter(r=>r.rollback?.[16]===1).length};
  const moves=[0,1].map(seat=>{const coords=motion.filter(s=>s.game.length===56).map(s=>[s.game[17+seat*16],s.game[18+seat*16]]);
    return {seat,samples:coords.length,distinct:new Set(coords.map(c=>c.join(','))).size};});
  const audio=motion.filter(r=>r.audio?.length===9).map(r=>r.audio);
  const audioSummary={samples:audio.length,first:audio[0]||[],last:audio.at(-1)||[],errors:audio.filter(a=>a[3]!==0),
    mixedFramesDelta:audio.length?(audio.at(-1)[2]-audio[0][2])>>>0:0,queueFrames:distribution(audio.map(a=>a[4])),musicIncluded:false};
  const measuredWire=data.transport.filter(e=>e.phase==='measure'&&e.actual!==null&&e.actual!==undefined);
  const edgeIntervals=times=>gaps([data.start,...times.filter(t=>t>=data.start&&t<=data.end),data.end]);
  return {callbacks:rows.length,callbackKinds:{raf:rows.filter(r=>r.kind!=='recovery-task').length,recoveryTasks:rows.filter(r=>r.kind==='recovery-task').length},elapsedMs:elapsed,logicalTicks:logical,logicHz:elapsed?logical*1000/elapsed:0,
    rafRequestToEntry:distribution(rows.map(r=>r.entry-r.requested)),rafTimestampToEntry:distribution(rows.map(r=>r.entry-r.stamp)),
    callbackEntryIntervals:distribution(gaps(rows.map(r=>r.entry))),callbackWall:distribution(rows.map(r=>r.exit-r.entry)),
    nativeReportedWall:distribution(rows.map(r=>r.nativeMs)),observerWall:distribution(rows.map(r=>r.observerMs)),
    existingShellHookWall:distribution(rows.map(r=>r.shellHookMs??0)),
    presentCount:presents.length,frontierCount:frontiers.length,
    presentIntervals:distribution(gaps(presents)),newFrontierIntervals:distribution(gaps(frontiers)),
    presentWindowHolds:distribution(edgeIntervals(presents)),frontierWindowHolds:distribution(edgeIntervals(frontiers)),
    windowEdgeMeaning:'Start/end intervals are censored lower bounds; presentation counter changes are observed at callback exit, not GPU scanout.',
    moves,rollbackDelta:deltas,callbackFlags,audio:audioSummary,
    ...(audioProfile?{audioProfile:summarizeAudioProfile(rows)}:{}),
    injectionTimerLateness:distribution(measuredWire.map(e=>e.lateness)),actualSendDelay:distribution(measuredWire.map(e=>e.actual-e.scheduled)),
    sourceClock:{timeOrigin:data.timeOrigin,note:'All page deltas use this document performance clock; never subtract peer clocks.'},
    rollbackFirst:first,rollbackLast:last,overflow:data.overflow,probeErrors:data.errors,
    profile:Object.fromEntries(PROFILE_NAMES.slice(2).map((name,i)=>[name,distribution(rows.filter(r=>r.profile?.[1]===1).map(r=>r.profile[i+2]))])),
    worstNative:rows.toSorted((a,b)=>b.nativeMs-a.nativeMs).slice(0,5).map(r=>({id:r.id,entry:r.entry,nativeMs:r.nativeMs,
      rafWait:r.entry-r.requested,net:r.net,rollback:r.rollback,profile:r.profile,
      ...(audioProfile?{audioProfile:r.audioProfile,audioProfileReadMs:r.audioProfileReadMs}:{})})),
    maxQueued:data.maxQueued,maxQueuedBytes:data.maxQueuedBytes};
}

try{
  mark('preflight');
  const web=launcher;
  const relayFile=resolve(process.env.EAGLER_RELAY_ROOT||resolve(root,'../../services/relay'),'src/netplay-relay.mjs');
  const catalogFile=resolve(web,'lib/contracts/product-catalog.mjs');
  const relayUrlFile=resolve(web,'.cache/build/browser/assets/launcher/multiplayer-relay-url.mjs');
  const optionsFile=resolve(web,'.cache/build/browser/assets/launcher/multiplayer-runtime-options.mjs');
  const touchHelperFile=resolve(web,'.cache/build/browser/assets/launcher/touch-runtime-protocol.mjs');
  for(const file of [relayFile,catalogFile,relayUrlFile,optionsFile])
    if(!existsSync(file))blocked('Missing migrated dependency: '+file);
  const catalog=await import(pathToFileURL(catalogFile)),policy=catalog.PRODUCT_GAMES.th11.multiplayer;
  const timing=catalog.multiplayerInputTimingPolicy(policy);report.policy={inputTiming:policy.inputTiming,resolved:timing};
  // The same actual contract is imported by the unmodified relay service.
  // Never admit a P0 room then secretly substitute a rollback runtime.
  if(!timing.rollback||timing.predictionLimit!==prediction)blocked('Actual TH11 relay product contract must permit Direct mode and P12; current '+JSON.stringify(report.policy));
  const {buildMultiplayerGameplayRelayUrl}=await import(pathToFileURL(relayUrlFile));
  const {buildMultiplayerRuntimeOptions}=await import(pathToFileURL(optionsFile));
  for(const file of [archive,resolve(runtime,'manifest.json'),resolve(runtime,'runtime-files.json'),resolve(runtime,'th11-sdl.wasm')])
    if(!existsSync(file))blocked('Missing built production fixture: '+file);
  const manifest=JSON.parse(readFileSync(resolve(runtime,'manifest.json'))),inventory=JSON.parse(readFileSync(resolve(runtime,'runtime-files.json')));
  assert.equal(manifest.product,'th11mp');assert.equal(manifest.variant,'multiplayer');
  const wasm=fileIdentity(resolve(runtime,'th11-sdl.wasm'));assert.equal(wasm.sha256,manifest.execution.sha256,'Actual WASM agrees with untouched manifest');
  report.identity={game:gitIdentity(root),common:gitIdentity(resolve(root,'../../common')),launcher:gitIdentity(launcher),
    archive:fileIdentity(archive),manifest:fileIdentity(resolve(runtime,'manifest.json')),inventory:fileIdentity(resolve(runtime,'runtime-files.json')),
    wasm,loader:fileIdentity(resolve(runtime,'th11-sdl.mjs')),shell:fileIdentity(resolve(runtime,'shell.mjs')),
    relay:fileIdentity(relayFile),catalog:fileIdentity(catalogFile),runtimeOptions:fileIdentity(optionsFile),relayUrl:fileIdentity(relayUrlFile),
    runner:fileIdentity(import.meta.filename),host:fileIdentity(resolve(import.meta.dirname,'browser-host.html'))};
  const timingObserver=resolve(import.meta.dirname,'relay-timing-observer.mjs');
  if(delayLocation==='relay')report.identity.relayTimingObserver=fileIdentity(timingObserver);
  const pinned=new Map([report.identity.wasm,report.identity.loader,report.identity.shell,report.identity.manifest,report.identity.inventory,
    report.identity.relay,report.identity.catalog,report.identity.runtimeOptions,report.identity.relayUrl,report.identity.runner,report.identity.host].map(f=>[f.path,f.sha256]));
  report.identity.errorClassifier=fileIdentity(resolve(import.meta.dirname,'acceptance-errors.mjs'));
  pinned.set(report.identity.errorClassifier.path,report.identity.errorClassifier.sha256);
  const sharedFiles=new Map();
  const sharedResources=[];
  report.identity.sharedFonts=[];
  for(const [name,input] of [['msgothic.ttc',process.env.TH11_TEST_FONT||process.env.EAGLER_DEVELOPMENT_VANILLA_FONT],
    ['unifont.otf',process.env.TH11_TEST_UNICODE_FONT||process.env.EAGLER_DEVELOPMENT_UNICODE_FONT]]){
    if(!input||!existsSync(input))blocked('Supply shared font through TH11_TEST_FONT and TH11_TEST_UNICODE_FONT: '+name);
    const file=fileIdentity(resolve(input)),url='/shared/'+name;sharedFiles.set(url,file.path);
    sharedResources.push({path:'/'+name,url,bytes:file.bytes});
    report.identity.sharedFonts.push(file);pinned.set(file.path,file.sha256);
  }
  if(replayOracle){const f=fileIdentity(resolve(import.meta.dirname,'replay-prefix-oracle.mjs'));report.identity.replayOracle=f;pinned.set(f.path,f.sha256);}
  if(replayOracle){const f=fileIdentity(resolve(import.meta.dirname,'framebuffer-proof.mjs'));report.identity.framebufferProof=f;pinned.set(f.path,f.sha256);}
  report.pixelOracle={requested:pixelOracle,readbackPhase:'bounded confirmed post-draw pause samples after measurement; exact Replay after seek',performanceWindowReadbacks:0};
  let referenceInventory=null;
  if(referenceRuntime){
    const manifestFile=resolve(referenceRuntime,'manifest.json'),inventoryFile=resolve(referenceRuntime,'runtime-files.json');
    const refManifest=JSON.parse(readFileSync(manifestFile));referenceInventory=JSON.parse(readFileSync(inventoryFile));
    assert.equal(refManifest.product,'th11mp');
    report.identity.replayReference={manifest:fileIdentity(manifestFile),inventory:fileIdentity(inventoryFile),wasm:fileIdentity(resolve(referenceRuntime,'th11-sdl.wasm'))};
    assert.equal(report.identity.replayReference.wasm.sha256,refManifest.execution.sha256);
    for(const [name,expected]of Object.entries(referenceInventory.files)){
      const path=resolve(referenceRuntime,name),rel=relative(referenceRuntime,path);
      assert.ok(!isAbsolute(rel)&&rel!=='..'&&!rel.startsWith('..'+sep));
      const f=fileIdentity(path);assert.equal(f.sha256,expected.sha256);assert.equal(f.bytes,expected.bytes);pinned.set(f.path,f.sha256);
    }
    pinned.set(inventoryFile,hash(readFileSync(inventoryFile)));
  }
  if(touchCarrier==='launcher'){
    const f=fileIdentity(touchHelperFile);report.identity.touchHelper=f;pinned.set(f.path,f.sha256);
    report.touchCarrierScope='Unmodified compiled Launcher input helpers; no per-move RPC request. Fallback postMessage calls counted. Actual native movement and recorded input independently validated.';
  }
  if(delayLocation==='relay')pinned.set(timingObserver,report.identity.relayTimingObserver.sha256);
  const {chromium}=await loadPlaywright();
  const browserTemp=resolve(out,'browser-temp');mkdirSync(browserTemp,{recursive:true});
  // Scope temporary profiles/downloads to this owned test process and its children.
  // The machine's C: temp volume can be full; never clean another user's files.
  process.env.TEMP=browserTemp;process.env.TMP=browserTemp;process.env.TMPDIR=browserTemp;
  report.browserTemp=browserTemp;
  const chrome=option('--chrome',process.env.PLAYWRIGHT_CHROMIUM_EXECUTABLE||chromium.executablePath());
  if(!existsSync(chrome))blocked('Installed desktop Chrome executable missing; pass --chrome (no download/install): '+chrome);
  report.browserExecutable=fileIdentity(chrome);report.playwrightDriver='shared Node/Python Playwright resolver';
  const gpu=option('--gpu','hardware');assert.ok(['hardware','swiftshader'].includes(gpu));report.settings.gpuRequested=gpu;
  // Child owns a separate event loop. Heartbeats are evidence of timer lateness,
  // not proof of per-packet forwarding; no cross-process clock subtraction.
  const relayBootstrap=(delayLocation==='relay'?"(await import("+JSON.stringify(pathToFileURL(timingObserver).href)+")).installRelayTimingObserver();":"")+
    "let prior=performance.now();setInterval(()=>{const now=performance.now();process.send?.({kind:'heartbeat',now,late:now-prior-50});prior=now;},50).unref();await import("+JSON.stringify(pathToFileURL(relayFile).href)+");";
  relay=spawn(process.execPath,['--input-type=module','--eval',relayBootstrap],{cwd:dirname(relayFile),windowsHide:true,
    env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:'',
      EAGLER_NETPLAY_RELAY_DELAY_MS:delayLocation==='relay'?String(latency):'0',EAGLER_NETPLAY_RELAY_JITTER_MS:delayLocation==='relay'?String(jitter):'0'},
    stdio:['ignore','pipe','pipe','ipc']});
  report.relayProcess={pid:relay.pid,entry:relayFile};
  relay.on('message',m=>{if(m?.kind==='heartbeat'&&relayHeartbeats.length<10000)relayHeartbeats.push(m);
    if(m?.kind==='relay-delay-observation')report.relayDelayObservation=m;});
  const relayOutput=b=>{if(relayLog.length+b.length>4*1024*1024){relayLogOverflow=true;return;}relayLog+=b.toString();};
  relay.stdout.on('data',relayOutput);relay.stderr.on('data',relayOutput);
  const deadline=Date.now()+15000;
  while(!/listening ws:\/\/127\.0\.0\.1:\d+/.test(relayLog)){if(relay.exitCode!==null||Date.now()>deadline)throw Error('Relay startup failed: '+relayLog);await pause(25);}
  const relayUrl=relayLog.match(/listening (ws:\/\/127\.0\.0\.1:\d+)/)[1];
  const html=readFileSync(resolve(import.meta.dirname,'browser-host.html'));
  const mime={'.mjs':'text/javascript','.js':'text/javascript','.html':'text/html','.json':'application/json','.wasm':'application/wasm','.css':'text/css'};
  server=createServer((req,res)=>{
    const transfer={url:req.url,started:performance.now(),expectedBytes:null,readBytes:0,finished:false,closed:false};
    report.http.push(transfer);
    res.on('finish',()=>{transfer.finished=true;transfer.finishedAt=performance.now();});
    res.on('close',()=>{transfer.closed=true;transfer.closedAt=performance.now();});
    try{
      const pathname=new URL(req.url,'http://localhost').pathname;res.setHeader('Cache-Control','no-store');
      if(pathname==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}
      if(pathname==='/touch-runtime-protocol.mjs'&&touchCarrier==='launcher'){
        res.setHeader('Content-Type','text/javascript');res.end(readFileSync(touchHelperFile));return;
      }
      if(pathname==='/favicon.ico'){res.writeHead(204);res.end();return;}
      let file=sharedFiles.get(pathname);if(pathname==='/data/th11')file=archive;
      else if(pathname.startsWith('/runtime/')){
        const name=decodeURIComponent(pathname.slice(9)),candidate=resolve(runtime,name),rel=relative(runtime,candidate);
        if(!isAbsolute(rel)&&rel!== '..'&&!rel.startsWith('..'+sep)&&
          (name==='runtime-files.json'||Object.hasOwn(inventory.files,name)))file=candidate;
      }
      else if(referenceRuntime&&pathname.startsWith('/reference-runtime/')){
        const name=decodeURIComponent(pathname.slice('/reference-runtime/'.length)),candidate=resolve(referenceRuntime,name),rel=relative(referenceRuntime,candidate);
        if(!isAbsolute(rel)&&rel!=='..'&&!rel.startsWith('..'+sep)&&
           (name==='runtime-files.json'||Object.hasOwn(referenceInventory.files,name)))file=candidate;
      }
      if(!file||!existsSync(file)){res.writeHead(404);res.end('Unknown fixture');return;}
      transfer.file=file;transfer.expectedBytes=statSync(file).size;
      res.setHeader('Content-Type',mime[extname(file)]||'application/octet-stream');
      res.setHeader('Content-Length',String(transfer.expectedBytes));
      const stream=createReadStream(file);stream.on('data',chunk=>{transfer.readBytes+=chunk.length;});
      pipeline(stream,res,error=>{if(error){transfer.error=String(error);observeError({kind:'server-transfer',url:req.url,error:String(error),checkerMs:performance.now()});}});
    }catch(error){if(res.headersSent)res.destroy(error);else{res.writeHead(500);res.end(String(error));}observeError({kind:'server',url:req.url,error:String(error)});}
  });
  // Large managed archives can remain buffered in a throttled renderer after
  // the Node response finishes. Retain the owned localhost keep-alive socket
  // beyond the default 5s (+1s buffer); record the setting for reproduction.
  server.keepAliveTimeout=65000;server.headersTimeout=70000;
  report.httpTimeouts={keepAliveMs:server.keepAliveTimeout,headersMs:server.headersTimeout};
  await new Promise((yes,no)=>{server.once('error',no);server.listen(0,'127.0.0.1',yes);});
  const base='http://127.0.0.1:'+server.address().port,roomCode=randomUUID().replaceAll('-','').slice(0,12),room='th11mp-'+roomCode;
  mark('launch-independent-browsers');
  for(let seat=0;seat<2;++seat){
    const args=['--disable-background-timer-throttling','--disable-backgrounding-occluded-windows','--disable-renderer-backgrounding',
      '--autoplay-policy=no-user-gesture-required',...(gpu==='swiftshader'?['--enable-unsafe-swiftshader']:
        process.platform==='win32'?['--enable-gpu','--use-gl=angle','--use-angle=d3d11']:['--enable-gpu'])];
    const browser=await chromium.launch({headless:!headed,executablePath:chrome,args});browsers.push(browser);
    const connection=await browser.newBrowserCDPSession();
    const processes=await connection.send('SystemInfo.getProcessInfo'),system=await connection.send('SystemInfo.getInfo');
    const browserPid=processes.processInfo.find(p=>p.type==='browser')?.id;
    assert.ok(browserPid,'Chrome process identity available');report.processIds.push(browserPid);
    report.browser.push({seat,version:browser.version(),pid:browserPid,args,gpu:system.gpu,processes:processes.processInfo});
    const context=await browser.newContext({viewport:{width:660,height:510},serviceWorkers:'block'});contexts.push(context);
    await context.addInitScript(installProbe,{seat,route,latency:delayLocation==='sender'?latency:0,jitter:delayLocation==='sender'?jitter:0,duration,profile,nativeProfile,audioProfile});
    const page=await context.newPage();pages.push(page);
    page.on('pageerror',e=>observeError({seat,kind:'pageerror',error:String(e),checkerMs:performance.now()}));
    page.on('crash',()=>observeError({seat,kind:'page-crash',checkerMs:performance.now()}));
    page.on('requestfailed',request=>observeError({seat,kind:'request-failed',url:request.url(),method:request.method(),failure:request.failure(),checkerMs:performance.now()}));
    page.on('console',m=>{if(m.type()==='error')observeError({seat,kind:'console-error',error:m.text(),checkerMs:performance.now()});});
    const cdp=await context.newCDPSession(page);cdps.push(cdp);
    await cdp.send('Emulation.setCPUThrottlingRate',{rate:cpuRates[seat]});report.browser[seat].cpuThrottle={requested:cpuRates[seat],cdpAcknowledged:true};
    await page.goto(base);await page.evaluate(()=>host.open());
    await page.waitForFunction(()=>host.ready()||host.events.some(m=>m.event==='error'),null,{timeout:120000});
    const startup=await page.evaluate(()=>({ready:host.ready(),errors:host.events.filter(m=>m.event==='error')}));
    assert.ok(startup.ready&&!startup.errors.length,'Managed initialization failed: '+JSON.stringify(startup));
  }
  assert.notEqual(report.processIds[0],report.processIds[1],'Two contexts in one process do not satisfy this fixture');
  const ids=[0,1].map(s=>'th11_rollback_'+s+'_'+randomUUID().slice(0,8)),selections=[0,3];
  for(let seat=0;seat<2;++seat)await pages[seat].evaluate(([url,id,seat,selection])=>host.lobby(url,id,seat,2,selection),[relayUrl+'/?room='+room+'&run=1',ids[seat],seat,selections[seat]]);
  await Promise.all(pages.map(p=>p.evaluate(()=>host.readyLobby())));
  report.startRoom=await pages[0].evaluate(async()=>{
    const s=host.lobbyState;await s.until(()=>s.room.seats.filter(Boolean).every(p=>p.ready));
    s.socket.send(JSON.stringify({type:'start',adonisMode:0,inputDelayAuto:false,inputDelay:0,predictionReserve:2,predictionLimit:12,challengeMode:false}));
    await s.until(()=>s.messages.some(m=>m.type==='start')||s.messages.some(m=>m.type==='error'));
    const start=s.messages.find(m=>m.type==='start');if(!start)throw Error(JSON.stringify(s.messages.filter(m=>m.type==='error')));return start;
  });
  assert.equal(report.startRoom.room.adonisMode,0);assert.equal(report.startRoom.room.inputDelay,0);
  assert.equal(report.startRoom.room.predictionLimit,prediction,'Relay and native prediction policy must agree');
  const approved=report.startRoom.room;
  assert.equal(approved.playerCount,2);assert.equal(approved.difficulty,1);
  assert.equal(approved.inputDelayAuto,false);assert.equal(approved.challengeMode,false);
  const active=approved.seats.slice(0,approved.playerCount);assert.equal(active.length,2);
  active.forEach((s,i)=>{assert.ok(s);assert.equal(s.clientId,ids[i]);assert.equal(s.loadout,selections[i]);
    assert.ok(Number.isInteger(s.loadout)&&s.loadout>=0&&s.loadout<policy.loadouts.length);});
  const loadouts=active.map(s=>{const {character,shot}=policy.loadouts[s.loadout];return {character,shot};});
  report.fixtureSeed={value:1234,source:'fixed deterministic acceptance seed; raw relay snapshot has no seed'};
  report.runtimeOptions=[];
  mark('real-relay-admitted');
  for(let seat=0;seat<2;++seat){
    const url=buildMultiplayerGameplayRelayUrl(relayUrl,{product:'th11mp',roomCode,runId:report.startRoom.serial,role:{player:seat},memberId:'member_'+ids[seat]});
    const options={...buildMultiplayerRuntimeOptions({url,player:seat,playerCount:approved.playerCount,
      seed:report.fixtureSeed.value,difficulty:approved.difficulty,inputDelay:approved.inputDelay,
      inputDelayAuto:approved.inputDelayAuto,predictionReserve:approved.predictionReserve,
      adonisMode:approved.adonisMode,predictionLimit:approved.predictionLimit,challengeMode:approved.challengeMode,
      spectator:false,spectatorId:'',spectatorCount:approved.spectatorCount,iceServers:[],loadouts},policy),
      touchEnabled:true,touchMovementMode:'touch',touchSensitivity:100,doubleTapBombEnabled:false};
    report.runtimeOptions.push(options);
    await pages[seat].evaluate(async({options,sharedResources})=>{await host.configure(options,sharedResources);await host.launch();document.querySelector('iframe').contentWindow.__rollbackAcceptance.attach();},{options,sharedResources});
  }
  async function smalls(){return Promise.all(pages.map(p=>p.evaluate(()=>document.querySelector('iframe').contentWindow.__rollbackAcceptance.small())));}
  const capabilities=await smalls();
  for(const s of capabilities)assert.equal(s.rollback.length,24,'Built package must export real rollback status ABI');
  const warmDeadline=Date.now()+45000;let warm;
  while(true){warm=await smalls();
    for(const s of warm)assert.ok(!s.error&&!s.shellError&&!s.probeErrors.length,JSON.stringify(s));
    if(warm.every(s=>s.net[2]&&s.net[3]>=125&&s.rollback[0]===1&&s.rollback[1]&&s.rollback[2]&&!s.rollback[3]))break;
    if(Date.now()>warmDeadline)throw Error('Stable rollback-capable world did not activate: '+JSON.stringify(warm));await pause(50);
  }
  report.webgl=await Promise.all(pages.map(p=>p.evaluate(()=>{
    const canvas=document.querySelector('iframe').contentDocument.querySelector('canvas'),gl=canvas?.getContext('webgl2');
    if(!gl)return {available:false};const ext=gl.getExtension('WEBGL_debug_renderer_info');return {available:true,
      renderer:gl.getParameter(ext?ext.UNMASKED_RENDERER_WEBGL:gl.RENDERER),vendor:gl.getParameter(ext?ext.UNMASKED_VENDOR_WEBGL:gl.VENDOR)};
  })));
  report.warmup=warm;for(const s of warm){assert.equal(s.route,route);assert.equal(s.net[7],0);}
  async function routes(){return Promise.all(pages.map(p=>p.evaluate(async()=>{
    const r=document.querySelector('iframe').contentWindow,t=r.__eaglerPeerTransport,out={route:t?.route,native:Array.from(new Uint32Array(r.core.memory.buffer,r.core.th11_mp_status(),24))[20],pairs:[]};
    for(const [id,peer] of t?.peers||[]){const stats=await peer.pc.getStats();let selected;
      for(const s of stats.values())if(s.type==='transport'&&s.selectedCandidatePairId)selected=stats.get(s.selectedCandidatePairId);
      if(!selected)for(const s of stats.values())if(s.type==='candidate-pair'&&s.state==='succeeded'&&s.nominated)selected=s;
      if(selected)out.pairs.push({peer:id,pair:selected,local:stats.get(selected.localCandidateId),remote:stats.get(selected.remoteCandidateId)});
    }return out;
  })));}
  report.network=await routes();
  for(const r of report.network){assert.equal(r.route,route);assert.equal(r.native,route==='rtc'?1:2);
    if(route==='rtc'){assert.equal(r.pairs.length,1,'Selected RTC pair must be observed');assert.notEqual(r.pairs[0].local?.candidateType,'relay');assert.notEqual(r.pairs[0].remote?.candidateType,'relay');}}
  if(cpuSamples){
    for(const cdp of cdps){await cdp.send('Profiler.enable');await cdp.send('Profiler.setSamplingInterval',{interval:1000});await cdp.send('Profiler.start');}
    report.cpuProfiles={diagnosticOnly:true,intervalMicroseconds:1000,files:[]};save();
  }
  mark('measurement-start');
  if(delayLocation==='relay')relay.send({kind:'relay-delay-phase',phase:'measure'});
  await Promise.all(pages.map((page,seat)=>page.evaluate(async({duration,seat,stationary,touchCarrier})=>{
    const r=document.querySelector('iframe').contentWindow,probe=r.__rollbackAcceptance;
    const helper=touchCarrier==='launcher'?await import('/touch-runtime-protocol.mjs'):null;
    const t=window.__rollbackTouch={start:performance.now(),end:0,planned:Math.ceil(duration*120),submitted:0,acks:0,errors:[],events:[],lateSlots:0,active:true,serial:0};
    t.carrier=touchCarrier;t.dispatchAccepted=0;t.postMessages=0;
    const target={get __eaglerDirectInputBridge(){return r.__eaglerDirectInputBridge;},postMessage(m,origin){++t.postMessages;r.postMessage(m,origin);}};
    const context={target,targetOrigin:location.origin,protocol:'eagler-touhou/1',game:'th11',epoch:1,launched:true,ready:true,spectator:false};
    const step=1000/120,end=t.start+duration*1000;
    const send=(command,fields={},planned=performance.now())=>{
      const id='rollback-touch-'+seat+'-'+(++t.serial),sent=performance.now();
      t.events.push({id,command,planned,sent,lateness:sent-planned});++t.submitted;
      if(helper){
        const accepted=command==='direct-touch'?helper.postDirectTouch(context,fields.type,fields):
          command==='touch-controls'?helper.postTouchControls(context,fields.controls??fields,100):helper.postTouchCancel(context);
        if(accepted===true)++t.dispatchAccepted;else t.errors.push('Launcher input dispatch rejected');
      }else{++t.postMessages;r.postMessage({protocol:'eagler-touhou/1',game:'th11',epoch:1,request:id,command,...fields},location.origin);}
    };
    window.addEventListener('message',event=>{if(event.source!==r||event.origin!==location.origin)return;
      const m=event.data;if(typeof m?.request==='string'&&m.request.startsWith('rollback-touch-'+seat+'-')){++t.acks;if(!m.ok)t.errors.push(m.error);}});
    probe.start();send('touch-controls',{controls:{fireEnabled:true,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0}});
    const point=elapsed=>{if(stationary)elapsed=0;return {x:(224+(seat?1:-1)*(30+65*Math.sin(elapsed*.0017)))/640,y:(390+32*Math.sin(elapsed*.0011+seat))/480};};
    send('direct-touch',{type:'down',id:71+seat,...point(0)});
    let index=1;
    function pump(){
      if(!t.active)return;const now=performance.now();
      if(now>=end){t.end=now;t.active=false;send('touch-cancel');send('touch-controls',{controls:{fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0}});probe.drain();return;}
      const slot=Math.max(index,Math.floor((now-t.start)/step));t.lateSlots+=Math.max(0,slot-index);
      send('direct-touch',{type:'move',id:71+seat,...point(now-t.start)},t.start+slot*step);
      index=slot+1;setTimeout(pump,Math.max(0,t.start+index*step-performance.now()));
    }
    setTimeout(pump,step);
  },{duration,seat,stationary,touchCarrier})));
  const measureDeadline=Date.now()+duration*1000+15000;
  while(true){const s=await smalls();for(const x of s)assert.ok(!x.error&&!x.shellError&&!x.probeErrors.length,JSON.stringify(x));
    if(s.every(x=>x.phase==='drain'))break;if(Date.now()>measureDeadline)throw Error('Wall-clock touch/measurement did not finish');await pause(100);}
  if(cpuSamples){
    for(let seat=0;seat<cdps.length;++seat){const {profile}=await cdps[seat].send('Profiler.stop');
      const file=resolve(out,'peer-'+seat+'.cpuprofile');writeFileSync(file,JSON.stringify(profile)+'\n');
      report.cpuProfiles.files.push({seat,...fileIdentity(file),samples:profile.samples?.length||0});await cdps[seat].send('Profiler.disable');}
    save();
  }
  mark('confirmation-tail-start');
  if(pixelOracle){
    const {installFramebufferProof}=await import('./framebuffer-proof.mjs');
    await Promise.all(pages.map(page=>page.evaluate(installFramebufferProof)));
  }
  if(delayLocation==='relay')relay.send({kind:'relay-delay-phase',phase:'drain'});
  report.pauseLifecycle={before:await smalls(),startedCheckerMs:performance.now(),events:[]};
  await pages[0].evaluate(()=>host.key('Escape',true));
  report.pauseLifecycle.events.push({action:'managed-escape-down',checkerMs:performance.now()});
  const pauseDeadline=Date.now()+10000;
  while(true){const s=await smalls();
    for(const x of s)assert.ok(!x.error&&!x.shellError,JSON.stringify(x));
    if(s.every(x=>x.nativePhase===2)){report.pauseLifecycle.paused=s;break;}
    if(Date.now()>pauseDeadline)throw Error('Real synchronized Pause barrier did not enter on both peers');await pause(50);}
  await pages[0].evaluate(()=>host.key('Escape',false));
  report.pauseLifecycle.events.push({action:'managed-escape-up',checkerMs:performance.now()});
  // Read only small tail metadata until a common, fully-confirmed post-tick
  // owner snapshot exists. Keep native loops/network alive during this phase.
  let common=null,boundaryPixels=null;const tailDeadline=Date.now()+10000;
  while(!common){
    const tails=await Promise.all(pages.map(p=>p.evaluate(pixels=>{
      const w=document.querySelector('iframe').contentWindow,tail=w.__rollbackAcceptance.state.tail.slice(-180);
      if(!pixels)return tail;
      const keys=new Set(w.__framebufferProof.sampleKeys());
      return tail.filter(sample=>sample.pixelSample&&keys.has(sample.pixelSample.key));
    },pixelOracle)));
    const right=new Map(tails[1].map(s=>[s.net[12]+':'+s.game[4],s]));
    let candidate=null;
    for(const a of tails[0]){const b=right.get(a.net[12]+':'+a.game[4]);if(b){assert.deepEqual(canonical(a),canonical(b),'Same confirmed frame full exported native fields differ');candidate={frame:a.game[4],generation:a.net[12],left:a,right:b};}}
    if(candidate&&pixelOracle){
      const key=candidate.generation+':'+candidate.frame;
      const selected=await Promise.all(pages.map(p=>p.evaluate(key=>document.querySelector('iframe').contentWindow.__framebufferProof.readSample(key),key)));
      // A bounded ring may retire a candidate between the metadata query and
      // transfer. Select another common captured frame while both probes run.
      if(selected.every(Boolean)){common=candidate;boundaryPixels=selected;}
    }else if(candidate)common=candidate;
    if(common)break;const s=await smalls();for(const x of s)assert.ok(!x.error&&!x.shellError,JSON.stringify(x));
    if(Date.now()>tailDeadline)throw Error('No common fully-confirmed non-correcting native-field boundary; preserve uncovered result');
    await pause(50);
  }
  if(pixelOracle){
    const sampling=await Promise.all(pages.map(p=>p.evaluate(()=>document.querySelector('iframe').contentWindow.__framebufferProof.stopSampling())));
    report.pixelOracle.liveBoundary={frame:common.frame,generation:common.generation,
      samples:boundaryPixels.map(({key,presentation,width,height,format},seat)=>({seat,key,presentation,width,height,format,...sampling[seat]}))};
  }
  report.confirmedBoundary=common;report.correctness={exportedNativeFieldsEqual:true,frame:common.frame,
    fullWorldBrowserOracle:false,exactWorldComponentEvidence:'world-oracle-01: 8 cases / 150373 checks / 1124 Update+Draw; see source-identified independent receipt',
    limitation:'56-word native status covers hash, stage/phase/frame and player values; it is not a complete body dump.'};
  mark('confirmed-boundary-checked');
  // Release must reach a real native frame before the second Escape edge.
  const releaseFrames=(await smalls()).map(s=>s.net[3]);
  const releaseDeadline=Date.now()+6000;
  while(true){const s=await smalls();if(s.every((x,i)=>x.net[3]>=releaseFrames[i]+3))break;
    if(Date.now()>releaseDeadline)throw Error('Paused native input release did not progress');await pause(40);}
  await pages[0].evaluate(()=>host.key('Escape',true));
  report.pauseLifecycle.events.push({action:'managed-resume-escape-down',checkerMs:performance.now()});
  const resumeDeadline=Date.now()+10000;
  while(true){const s=await smalls();
    for(const x of s)assert.ok(!x.error&&!x.shellError,JSON.stringify(x));
    if(s.every(x=>x.nativePhase===1&&x.rollback[2])){report.pauseLifecycle.resumed=s;break;}
    if(Date.now()>resumeDeadline)throw Error('Real synchronized Pause resume did not restore capture-capable Stage');await pause(50);}
  await pages[0].evaluate(()=>host.key('Escape',false));
  report.pauseLifecycle.events.push({action:'managed-resume-escape-up',checkerMs:performance.now()});
  report.pauseLifecycle.completedCheckerMs=performance.now();
  mark('pause-resume-usability-checked');
  if(disconnect){
    report.disconnect={before:await smalls(),requestedCheckerMs:performance.now()};
    await Promise.all(pages.map(p=>p.evaluate(()=>document.querySelector('iframe').contentWindow.__rollbackAcceptance.lifecycle())));
    await pages[0].evaluate(()=>{
      const r=document.querySelector('iframe').contentWindow,t=r.__eaglerPeerTransport;
      if(t.route==='rtc'){for(const peer of t.peers.values())peer.inputDc.close();}
      else t.relay.close(1000,'acceptance disconnect');
    });
    const deadline=Date.now()+12000;
    while(true){const s=await smalls();report.disconnect.after=s;
      if(s[1].disconnected||s[1].error||s[1].shellError)break;
      if(Date.now()>deadline)throw Error('Actual channel closure was not surfaced to peer');await pause(100);}
    report.disconnect.observed=true;mark('disconnect-lifecycle-observed');
  }
  // Final small results / correctness are settled before transferring bulky rows.
  report.final=await smalls();
  await Promise.all(pages.map(p=>p.evaluate(()=>{const r=document.querySelector('iframe').contentWindow;r.__rollbackAcceptance.stop();r.core.th11_loop_stop();})));
  mark('loops-stopped-after-correctness');
  if(delayLocation==='relay'){
    relay.send({kind:'relay-delay-phase',phase:'settled'});relay.send({kind:'relay-delay-snapshot'});
    const deadline=Date.now()+4000;
    while(!report.relayDelayObservation){if(Date.now()>deadline)throw Error('Missing independent relay timer evidence');await pause(25);}
    const rows=report.relayDelayObservation.rows.filter(x=>x.phase==='measure'&&x.fired!==null);
    assert.ok(rows.length>10&&!report.relayDelayObservation.overflow,'Independent relay impairment coverage/overflow');
    report.relayDelaySummary={count:rows.length,scheduledWait:distribution(rows.map(x=>x.delay)),
      actualTimerWait:distribution(rows.map(x=>x.fired-x.scheduled)),lateness:distribution(rows.map(x=>x.fired-x.scheduled-x.delay)),
      fifoHold:distribution(rows.map(x=>x.fired-(x.timerFired??x.fired))),
      boundary:'Relay sender delivery (global FIFO after timer), not peer RTT. No cross-process timestamp subtraction.'};
  }
  for(let seat=0;seat<2;++seat){
    const data=await pages[seat].evaluate(()=>({probe:document.querySelector('iframe').contentWindow.__rollbackAcceptance.result(),touch:window.__rollbackTouch}));
    const file=resolve(out,'peer-'+seat+'.json');writeFileSync(file,JSON.stringify(data)+'\n');
    report.results.push({seat,file,sha256:hash(readFileSync(file)),metrics:analysis(data.probe),touch:{planned:data.touch.planned,submitted:data.touch.submitted,
      carrier:touchCarrier,dispatchAccepted:data.touch.dispatchAccepted,postMessages:data.touch.postMessages,
      acknowledgements:data.touch.acks,lateSlots:data.touch.lateSlots,errors:data.touch.errors,actualDurationMs:data.touch.end-data.touch.start}});
    await pages[seat].screenshot({path:resolve(out,'peer-'+seat+'-final.png')});
  }
  try{report.networkAfter=await routes();}catch(e){
    if(!disconnect)throw e;report.networkAfter={closedChannelObservationError:String(e)};
  }
  if(replayOracle){
    mark('exact-replay-oracle-start');report.replayOracle=[];
    const {replayPrefixOracle}=await import('./replay-prefix-oracle.mjs');
    for(let seat=0;seat<2;++seat){
      const proof=await replayPrefixOracle({browser:browsers[seat],livePage:pages[seat],origin:base,
        expected:seat?report.confirmedBoundary.right:report.confirmedBoundary.left,out,seat,
        runtimePath:referenceRuntime?'/reference-runtime/':'/runtime/',pixels:pixelOracle,
        livePixels:boundaryPixels?.[seat],sharedResources});
      report.replayOracle.push(proof);save();
    }
    mark('exact-replay-oracle-complete');
  }
  for(const [path,digest] of pinned)assert.equal(hash(readFileSync(path)),digest,'Fixture changed during browser gate: '+path);
  report.sourceStable=true;
  const failures=[];
  if(cpuSamples)failures.push({kind:'diagnostic-cpu-profile-run-not-acceptance'});
  if(stationary)failures.push({kind:'diagnostic-stationary-run-not-movement-acceptance'});
  if(audioProfile)failures.push({kind:'diagnostic-audio-profile-run-not-acceptance'});
  for(const result of report.results){const m=result.metrics,seat=result.seat;
    if(audioProfile&&(!m.audioProfile?.coverageComplete||m.audioProfile.mixFailures||m.audioProfile.putFailures))
      failures.push({seat,kind:'audio-profile-error-or-coverage',audioProfile:m.audioProfile});
    if(m.audio.samples<10||m.audio.errors.length||m.audio.mixedFramesDelta===0)failures.push({seat,kind:'audio-error-or-coverage',audio:m.audio});
    if(m.overflow.length||m.probeErrors.length||result.touch.errors.length)failures.push({seat,kind:'observer-integrity',m});
    if(touchCarrier==='launcher'&&result.touch.dispatchAccepted!==result.touch.submitted)failures.push({seat,kind:'launcher-input-dispatch-rejected'});
    if(m.callbacks<Math.min(100,duration*10)||m.presentCount<30||m.frontierCount<30||m.moves.some(x=>x.distinct<8))failures.push({seat,kind:'movement-or-callback-coverage'});
    if(m.rollbackDelta.restores<1||m.rollbackDelta.historyTicks<1)failures.push({seat,kind:'rollback-not-actually-exercised'});
    if(m.nativeReportedWall.max>maxGap||m.presentWindowHolds.max>maxGap||m.frontierWindowHolds.max>maxGap)
      failures.push({seat,kind:'performance-threshold',nativeMax:m.nativeReportedWall.max,presentMax:m.presentWindowHolds.max,frontierMax:m.frontierWindowHolds.max});
    if(touchCarrier==='rpc'&&result.touch.acknowledgements!==result.touch.submitted)
      failures.push({seat,kind:'managed-input-unacknowledged',touch:result.touch});
    if(touchCarrier==='launcher'&&result.touch.dispatchAccepted!==result.touch.submitted)
      failures.push({seat,kind:'launcher-input-rejected',touch:result.touch});
  }
  if(relayLogOverflow)failures.push({kind:'relay-log-overflow'});
  const unexpectedErrors=report.errors.filter(entry=>!expectedDisconnectConsole(entry,report.disconnect));
  if(report.disconnect)report.disconnect.expectedConsoleNotifications=report.errors.filter(entry=>expectedDisconnectConsole(entry,report.disconnect));
  if(unexpectedErrors.length)failures.push({kind:'page-or-server-errors',errors:unexpectedErrors});
  report.failures=failures;report.performancePassed=!audioProfile&&!failures.some(f=>f.kind==='performance-threshold');
  if(audioProfile){
    report.audioProfileCollectionComplete=report.results.length===2&&report.results.every(r=>
      r.metrics.audioProfile?.coverageComplete&&!r.metrics.audioProfile.mixFailures&&!r.metrics.audioProfile.putFailures);
    report.diagnosticPerformanceThresholdsMet=!failures.some(f=>f.kind==='performance-threshold');
  }
  report.passed=failures.length===0;report.status=report.passed?'passed':'failed';
  if(!report.passed)process.exitCode=1;
}catch(error){
  firstFailure=error;report.status=error.blocked&&!report.watchdog?'blocked':'failed';report.error=error.stack||String(error);process.exitCode=error.blocked&&!report.watchdog?2:1;
  // Preserve every bad run. Snapshot retrieval is best effort and cannot replace
  // the original failure or incorrectly convert missing evidence into a pass.
  for(let seat=0;seat<pages.length;++seat){
    try{const data=await pages[seat].evaluate(()=>({host:host.snapshot(),probe:document.querySelector('iframe')?.contentWindow?.__rollbackAcceptance?.result(),touch:window.__rollbackTouch}));
      writeFileSync(resolve(out,'peer-'+seat+'-failure.json'),JSON.stringify(data)+'\n');}
    catch(e){report.cleanup.push({seat,action:'preserve-failure',error:String(e)});}
  }
}finally{
  mark('cleanup-start');
  for(let seat=0;seat<pages.length;++seat){
    try{await pages[seat].evaluate(()=>{const r=document.querySelector('iframe')?.contentWindow;r?.core?.th11_loop_stop();r?.__rollbackAcceptance?.close();});}
    catch(e){report.cleanup.push({seat,action:'stop-owned-runtime',error:String(e)});}
  }
  for(let seat=0;seat<browsers.length;++seat){try{await browsers[seat].close();report.cleanup.push({seat,action:'close-owned-browser',ok:true});}catch(e){report.cleanup.push({seat,action:'close-owned-browser',error:String(e)});}}
  if(server){server.closeAllConnections?.();await Promise.race([new Promise(r=>server.close(r)),pause(3000)]);}
  if(relay){relay.kill();await Promise.race([new Promise(r=>relay.once('exit',r)),pause(3000)]);report.cleanup.push({action:'stop-owned-relay',pid:relay.pid,exitCode:relay.exitCode,signal:relay.signalCode});}
  writeFileSync(resolve(out,'relay.log'),relayLog);writeFileSync(resolve(out,'relay-heartbeats.json'),JSON.stringify(relayHeartbeats)+'\n');
  report.relayHeartbeat={count:relayHeartbeats.length,lateness:distribution(relayHeartbeats.map(x=>x.late)),clock:'relay child performance.now; not comparable to peer timeOrigins',logOverflow:relayLogOverflow};
  clearTimeout(watchdog);
  if(report.watchdog){report.status='failed';report.passed=false;process.exitCode=1;}
  report.completedAt=new Date().toISOString();save();
  console.log('TH11 rollback browser '+report.status+': '+resolve(out,'receipt.json'));
  if(firstFailure)console.error(String(firstFailure));
}
