import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,dirname,extname,sep} from 'node:path';
import {launchBrowser} from '../../../../../tools/browser/launch.mjs';

const repo=resolve(import.meta.dirname,'../../../../..');
const runtime=resolve(process.env.EAGLER_TH17_BUILD||resolve(repo,'ports/th17/build-eagler-multiplayer'));
const out=resolve(process.env.TH17_ROOM_EVIDENCE||resolve(repo,'artifacts/th17-room'));
const rollback=!process.argv.includes('--lockstep');
const throughStageTwo=process.argv.includes('--through-stage-two');
const extraFinale=process.argv.includes('--extra-finale');
const launcherPreview=process.argv.includes('--launcher-preview');
const verifyReplay=process.argv.includes('--replay');
const checkPresentation=process.argv.includes('--check-presentation');
const independent=process.argv.includes('--independent-pages');
assert.ok(!independent||!verifyReplay,'Replay fixture uses the shared page');
const renderScale=Number(process.env.TH17_ROOM_RENDER_SCALE??1);
assert.ok(renderScale===1||renderScale===2);
const extraBoss=extraFinale||process.argv.includes('--extra-boss');
const denseExtra=extraBoss||process.argv.includes('--dense-extra');
const extended=throughStageTwo||process.argv.includes('--extended');
const extendedSeats=Number(process.env.TH17_ROOM_SEATS??2);
assert.ok(extendedSeats===2||extendedSeats===3);
const difficulty=Number(process.env.TH17_ROOM_DIFFICULTY??(denseExtra?4:throughStageTwo?0:1));
assert.ok(Number.isInteger(difficulty)&&difficulty>=0&&difficulty<=4);
assert.ok(!throughStageTwo||difficulty!==4);
const minimumSamples=Number(process.env.TH17_ROOM_SAMPLES||(denseExtra?4200:1050));
const extendedTarget=Number(process.env.TH17_ROOM_TICKS||(extraFinale?39000:extraBoss?15000:12000));
const delayMs=Number(process.env.TH17_ROOM_DELAY_MS||110);
const jitterMs=Number(process.env.TH17_ROOM_JITTER_MS||0);
const cpuThrottle=Number(process.env.TH17_ROOM_CPU_THROTTLE||1);
const manifest=JSON.parse(await readFile(resolve(runtime,'manifest.json'),'utf8'));
if(extraBoss)assert.ok(manifest.features.roomBenchmark&&rollback&&difficulty===4,'Extra boss profiling needs the dedicated Extra rollback benchmark build');
const baseline=process.env.TH17_ROOM_BASELINE?JSON.parse(await readFile(process.env.TH17_ROOM_BASELINE,'utf8')):null;
const priorMemory=extraFinale&&baseline?JSON.parse(await readFile(resolve(dirname(process.env.TH17_ROOM_BASELINE),'heap-samples.json'),'utf8')):null;
const baselineBoss=baseline?.reports[0]?.bossPatterns?.find(pattern=>pattern.spell>=91)?.boss;
const baselineBossStart=baseline?.reports[0]?.bossPatterns?.find(pattern=>pattern.boss===baselineBoss)?.first;
const fourthSpell=baseline?.reports[0]?.bossPatterns?.find(pattern=>pattern.boss===baselineBoss&&pattern.spell===94);
const lateStart=fourthSpell?fourthSpell.last+1:15000;
const cpuWarmupThrough=extraFinale?9000:extraBoss&&baselineBossStart?Math.max(0,baselineBossStart-1500):0;
let finaleSamples;
function compareMemory(previous,current,start=0){
 const key=row=>[row[0],row[1],row[2],row[4],row[5]].join('/');
 const samples=rows=>new Map(rows.filter(row=>row[0]>=start&&row[1]>=256).map(row=>[key(row),row]));
 const before=samples(previous),after=samples(current);
 const pairs=[...after].filter(([k])=>before.has(k)).map(([k,row])=>[before.get(k),row]);
 const summarize=(values)=>{
  values.sort((a,b)=>a-b);
  return {median:values[Math.floor(values.length*.5)],p95:values[Math.floor(values.length*.95)],max:values.at(-1)};
 };
 const metrics={heap:3,unusedCapacity:6,bulletState:7,bulletVisual:8,itemVisual:9,idleItemVisual:10,animation:11};
 return {start,matched:pairs.length,metrics:Object.fromEntries(Object.entries(metrics).map(([name,index])=>[name,{
  before:summarize(pairs.map(pair=>pair[0][index])),after:summarize(pairs.map(pair=>pair[1][index])),
  delta:summarize(pairs.map(pair=>pair[1][index]-pair[0][index])),
 }]))};
}
const required=name=>{assert.ok(process.env[name],'Set '+name);return process.env[name];};
const assets={
 '/game.dat':await readFile(required('EAGLER_TH17_DATA')),
 '/msgothic.ttc':await readFile(required('TH17_ROOM_GOTHIC')),
 '/unifont.otf':await readFile(required('TH17_ROOM_UNIFONT')),
};
const relay=spawn(process.execPath,[resolve(repo,'services/relay/src/netplay-relay.mjs')],{
 cwd:repo,windowsHide:true,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',
  EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe'],
});
let log='',options=[],browser;
relay.stdout.on('data',data=>log+=data);
relay.stderr.on('data',data=>log+=data);
const wait=async(predicate,description,ms=90000)=>{
 const end=Date.now()+ms;
 while(Date.now()<end){if(await predicate())return;await new Promise(r=>setTimeout(r,50));}
 throw Error(description+' timed out');
};
const lobby=(socket,message,match)=>new Promise((resolveLobby,reject)=>{
 const timeout=setTimeout(()=>done(Error('Lobby request timed out: '+JSON.stringify(message))),10000);
 const close=event=>done(Error('Lobby closed: '+event.code+' '+event.reason));
 function done(error,result){
  clearTimeout(timeout);socket.removeEventListener('message',receive);socket.removeEventListener('close',close);
  if(error)reject(error);else resolveLobby(result);
 }
 function receive(event){
  const value=JSON.parse(event.data);
  if(value.type==='error')done(Error(value.error));else if(match(value))done(null,value);
 }
 socket.addEventListener('message',receive);socket.addEventListener('close',close);
 if(message)socket.send(JSON.stringify(message));
});
const html=`<!doctype html><style>body{display:flex;margin:0}iframe{width:640px;height:480px;border:0}</style><script>
window.events=[];window.ready=new Set();window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/game.dat')).arrayBuffer()});
addEventListener('message',e=>{if(e.origin!==location.origin||e.data?.protocol!=='eagler-touhou/1')return;events.push(e.data);if(e.data.event==='ready')ready.add(e.data.epoch);});
window.addRuntime=seat=>{const frame=document.createElement('iframe');frame.id='p'+seat;frame.src='/runtime/th17.html?hosted=1&managedData=1&runtimeEpoch='+(seat+1);document.body.append(frame);};
window.rpc=(seat,command,fields={})=>new Promise((resolve,reject)=>{
 const frame=document.getElementById('p'+seat),request=crypto.randomUUID(),timer=setTimeout(()=>{removeEventListener('message',receive);reject(Error(command+' timeout'));},30000);
 function receive(e){if(e.source===frame.contentWindow&&e.data?.request===request){clearTimeout(timer);removeEventListener('message',receive);e.data.ok?resolve(e.data):reject(Error(e.data.error));}}
 addEventListener('message',receive);
 frame.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th17',epoch:seat+1,command,request,...fields},location.origin);
});
window.startRuntime=async seat=>{
 const options=(await(await fetch('/options')).json())[seat];
 const configure={music:'none',language:'ja'};
 await rpc(seat,'configure',{...configure,sharedResources:['/msgothic.ttc','/unifont.otf'].map(path=>({path,url:path})),options:${launcherPreview}?{renderScale:options.renderScale}:options});
 if(${launcherPreview}){
  const result=await rpc(seat,'prepare-room');
  const runtime=document.getElementById('p'+seat).contentWindow.Module;
  if(!result.presented||runtime._th17_phase()!==0||runtime._th17_frame()===0)throw Error('room title was not presented');
  await rpc(seat,'configure',{...configure,options});
 }
 await rpc(seat,'launch');
};
</script>`;
const server=createServer(async(req,res)=>{
 try{
  const path=new URL(req.url,'http://localhost').pathname;
  res.setHeader('Cross-Origin-Opener-Policy','same-origin');
  res.setHeader('Cross-Origin-Embedder-Policy','require-corp');
  if(path==='/'){res.setHeader('Content-Type','text/html');return res.end(html);}
  if(path==='/options'){res.setHeader('Content-Type','application/json');return res.end(JSON.stringify(options));}
  if(assets[path])return res.end(assets[path]);
  assert.ok(path.startsWith('/runtime/'));
  const file=resolve(runtime,path.slice(9));
  assert.ok(file.startsWith(runtime+sep));
  res.setHeader('Content-Type',({'.mjs':'text/javascript','.wasm':'application/wasm','.html':'text/html','.json':'application/json','.css':'text/css'})[extname(file)]||'application/octet-stream');
  res.end(await readFile(file));
 }catch{res.writeHead(404).end();}
});
const sockets=[],reports=[];
await mkdir(out,{recursive:true});
try{
 await wait(()=>/listening ws:\/\/127\.0\.0\.1:(\d+)/.test(log),'relay start',30000);
 const port=Number(/listening ws:\/\/127\.0\.0\.1:(\d+)/.exec(log)[1]);
 const endpoint='ws://127.0.0.1:'+port+'/?';
 await new Promise(r=>server.listen(0,'127.0.0.1',r));
 const hardware=process.env.NATIVE_GPU==='1'||process.env.PIXEL_GPU==='1';
 const gpuFlags=hardware?(process.platform==='win32'?['--enable-gpu','--use-gl=angle','--use-angle=d3d11']:['--enable-gpu']):['--use-angle=swiftshader','--enable-unsafe-swiftshader'];
 browser=await launchBrowser({args:[...gpuFlags,'--disable-background-timer-throttling','--disable-renderer-backgrounding','--disable-backgrounding-occluded-windows','--autoplay-policy=no-user-gesture-required']});
 for(const seats of extraBoss?[3]:extended?[extendedSeats]:[2,3]){
  const room='th17mp-room-test-'+seats,loadouts=Array.from({length:seats},(_,i)=>({character:i,shot:i}));
  const members=[];
  for(let seat=0;seat<seats;seat++){
   const member=room+'-private-'+seat;
   const socket=new WebSocket(endpoint+new URLSearchParams({room,lobby:room+'-'+seat,member,intent:seat?'join':'create',players:String(seats),difficulty:String(difficulty)}));
   sockets.push(socket);members.push(socket);
   await lobby(socket,null,value=>!!value.room);
   await lobby(socket,{type:'take-seat',seat,loadout:seat*3+seat},value=>!!value.room?.seats[seat]);
  }
  for(const [seat,socket] of members.entries())
   await lobby(socket,{type:'set-ready',ready:true},value=>value.room?.seats[seat]?.ready);
  const timing={adonisMode:rollback?0:1,inputDelay:rollback?0:2,inputDelayAuto:false};
  const start=await lobby(members[0],{type:'start',...timing},value=>value.type==='start');
  options=Array.from({length:seats},(_,seat)=>({
   netplayMode:'lan',netplayAdonisMode:timing.adonisMode,netplayInputDelay:timing.inputDelay,netplayAllowWebsocketFallback:!extraBoss,
   netplayPlayer:seat,netplayPlayerCount:seats,netplaySeed:1234,netplayDifficulty:difficulty,netplayLoadouts:loadouts,
   renderScale,netplayUrl:endpoint+new URLSearchParams({room,run:String(start.serial),member:room+'-private-'+seat}),
  }));
  const contexts=await Promise.all(Array.from({length:independent?seats:1},()=>browser.newContext({viewport:{width:independent?640:seats*640,height:480}})));
  if(rollback)await Promise.all(contexts.map(context=>context.addInitScript(({delayMs,jitterMs})=>{
   const send=RTCDataChannel.prototype.send;
   let sequence=0;
   RTCDataChannel.prototype.send=function(payload){
    if(!globalThis.delayInput||typeof payload==='string')return send.call(this,payload);
    // Delay the guest's binary samples and repairs after the RTC handshake.
    // Otherwise a repaired input can arrive before the delayed sample.
    const copy=payload.slice();
    globalThis.delayedPackets=(globalThis.delayedPackets||0)+1;
    setTimeout(()=>{if(this.readyState==='open')send.call(this,copy);},delayMs+(++sequence%5)*jitterMs/4);
   };
  },{delayMs,jitterMs})));
  const pages=await Promise.all(contexts.map(context=>context.newPage())),page=pages[0],errors=[];
  const pageFor=seat=>pages[independent?seat:0];
  let enableCpuThrottle;
  if(cpuThrottle>1){
   const sessions=await Promise.all(contexts.map((context,index)=>context.newCDPSession(pages[index])));
   const throttle=()=>Promise.all(sessions.map(cdp=>cdp.send('Emulation.setCPUThrottlingRate',{rate:cpuThrottle})));
   if(cpuWarmupThrough)enableCpuThrottle=throttle;
   else await throttle();
  }
  for(const current of pages)current.on('pageerror',error=>errors.push(error.message));
  try{
   await Promise.all(pages.map(current=>current.goto('http://127.0.0.1:'+server.address().port)));
   for(let seat=0;seat<seats;seat++){
    await pageFor(seat).evaluate(seat=>addRuntime(seat),seat);
    await pageFor(seat).waitForFunction(epoch=>ready.has(epoch),seat+1,{timeout:45000});
   }
   if(independent)await Promise.all(pages.map((current,seat)=>current.evaluate(seat=>startRuntime(seat),seat)));
   else await page.evaluate(seats=>Promise.all(Array.from({length:seats},(_,seat)=>startRuntime(seat))),seats);
   const frames=pages.flatMap(current=>current.frames().filter(frame=>frame.url().includes('/runtime/')));
   for(const [seat,frame] of frames.entries()){
    await frame.evaluate(({seat,rollback,extended,throughStageTwo,denseExtra,extraBoss,extraFinale,storyTiming})=>{
     const callback=Module.onGameFrame;
     const optimized=typeof Module._th17_room_trace==='function';
     Module._th17_room_trace?.(1);
     globalThis.samples=new Map();
     globalThis.diagnostics={corrections:0,resimulated:0,maxPrediction:0,combatCorrections:0};
     globalThis.frameCosts=[];
     globalThis.presentIntervals=[];
     globalThis.densePresentIntervals=[];
     globalThis.storyPresentIntervals=new Map();
     globalThis.heapSamples=[];
     let recorded=0,loadSample=0,bullets=0;
     let previousCosts=Array(10).fill(0);
     Module.onGameFrame=(ok,time,ticks,presented)=>{
      callback(ok,time,ticks,presented);
      const r=Module._th17_room_rollback()>>>2;
      const [n,committed,corrections,resimulated,combatCorrections,spellCorrections]=Module.HEAPU32.subarray(r,r+6);
      const phase=Module._th17_phase();
      let costs;
      if(extraBoss){
       const d=Module._th17_room_benchmark_draw()>>>2,c=Module._th17_room_performance()>>>2;
       const h=Module._th17_room_benchmark_history?.()>>>2;
       const totals=[...Module.HEAPU32.subarray(d,d+4),...Module.HEAPU32.subarray(c,c+2),
        ...(h?Module.HEAPU32.subarray(h,h+4):[0,0,0,0])];
       costs=totals.map((value,index)=>(value-previousCosts[index])>>>0);
       previousCosts=totals;
      }
      if(n>120&&ticks){
       const now=performance.now();
       diagnostics.measuredFrom??=now;
       diagnostics.measuredThrough=now;
       diagnostics.presentations=(diagnostics.presentations||0)+1;
       diagnostics.simulationTicks=(diagnostics.simulationTicks||0)+ticks;
      }
      if(n>120&&presented!==undefined){
       const now=performance.now();
       diagnostics.presentationStart??=now;
       diagnostics.presentationEnd=now;
       diagnostics.actualPresentations=(diagnostics.actualPresentations||0)+presented;
       if(presented){
        if(diagnostics.lastPresent){
         const gap=now-diagnostics.lastPresent;
         presentIntervals.push(gap);
         if(bullets>=256&&phase===1)densePresentIntervals.push(gap);
         const state=storyTiming&&samples.get(recorded);
         if(state){
          const segment=state[3]>1&&state[4]<90?'entry':state[33]?'dialogue':
           state[30]&1?'spell':state[38]?'nonspell':'route';
          const key=`${state[3]}/${segment}`;
          if(!storyPresentIntervals.has(key))storyPresentIntervals.set(key,[]);
          storyPresentIntervals.get(key).push(gap);
         }
        }
        diagnostics.lastPresent=now;
       }
      }
      if(n>120&&(ticks||resimulated>diagnostics.resimulated)&&(!denseExtra||phase===1)){
       const confirmed=samples.get(recorded);
       bullets=confirmed?.[10]??bullets;
       const entry={time,repair:resimulated>diagnostics.resimulated,bullets};
       if(extraBoss){
        entry.costs=costs;
        entry.frame=n;
        entry.boss=confirmed?.[41]||0;
        entry.spell=confirmed?.[30]&1?confirmed[39]:-1;
       }
       frameCosts.push(entry);
       if(extraFinale&&seat===0&&n>=9000&&n%4===0){
        const p=Module._th17_room_benchmark_memory()>>>2;
        const row=Array.from(Module.HEAPU32.subarray(p,p+12));
        if(row[0]&&row[3]&&(row[1]>=256||row[2]<101)&&row[0]!==heapSamples.at(-1)?.[0])heapSamples.push(row);
       }
       diagnostics.peakLinearMemory=Math.max(diagnostics.peakLinearMemory||0,Module.HEAPU8.buffer.byteLength);
       if(denseExtra&&++loadSample%12===0){
        const pointer=Module._th17_room_load?.()>>>2;
        if(pointer){
         const [liveBullets,heap]=Module.HEAPU32.subarray(pointer,pointer+2);
         diagnostics.peakLiveHeap=Math.max(diagnostics.peakLiveHeap||0,heap);
         if(liveBullets>=256)diagnostics.densePeakLiveHeap=Math.max(diagnostics.densePeakLiveHeap||0,heap);
        }
       }
      }
      diagnostics.corrections=corrections;diagnostics.resimulated=resimulated;
      diagnostics.combatCorrections=combatCorrections;diagnostics.spellCorrections=spellCorrections;
      diagnostics.maxPrediction=Math.max(diagnostics.maxPrediction,n-committed);
      globalThis.delayInput=rollback&&seat===1&&n>=115;
      for(let number=Math.max(recorded+1,committed-15);number<=committed;number++){
       const p=Module._th17_room_confirmed(number)>>>2,words=Array.from(Module.HEAPU32.subarray(p,p+(extraBoss?43:optimized?39:38)));
       words[2]=0;
       if(words[0]===number&&number>120&&words[3]){
        samples.set(number,words);
        if(words[3]===2)diagnostics.stageTwoFrames=(diagnostics.stageTwoFrames||0)+1;
        if(extraFinale){
         if((words[30]&1)&&words[39]===100){
          diagnostics.finalFirst??=number;
          diagnostics.finalLast=number;
         }else if(diagnostics.finalLast)diagnostics.finalFinished??=number;
        }
       }
      }
      recorded=committed;
      diagnostics.lastConfirmed=committed;
      diagnostics.phase=phase;
      if(extraBoss)return;
      const centerGuest=extended&&seat===1&&n>=900&&n<952;
      const reserve=(throughStageTwo||denseExtra)&&seat===1&&n>=1500;
      const moveGuest=seat===1&&n>=600&&!centerGuest&&!reserve;
      Module._th17_key(205,+(seat===1&&n>=120&&n<170||moveGuest&&n%96<12||extended&&seat===0&&n>=300&&n<350));
      Module._th17_key(203,+(seat===0&&n>=120&&n<170||moveGuest&&n%96>=48&&n%96<60||centerGuest||reserve&&n<1630));
      Module._th17_key(200,+(seat===2&&n>=120&&n<150||reserve&&n<1630));
      Module._th17_key(44,+(n>=140&&n<260||extended&&n>=900));
      Module._th17_key(46,+(extended&&seat===0&&n>=1400&&n%120<100));
      Module._th17_key(45,+(seat===1&&n>=185&&n<193||extended&&n>=1800&&(n+seat*420)%840<9||
       denseExtra&&n>=1500&&(n+seat*420)%720<9));
     };
    },{seat,rollback,extended,throughStageTwo,denseExtra,extraBoss,extraFinale,storyTiming:checkPresentation&&difficulty!==4});
   }
   let frontier=0,progressAt=Date.now(),coverageFailure;
   let cutoff=extendedTarget;
   try{await wait(async()=>{
    assert.deepEqual(errors,[]);
    const failures=(await Promise.all(pages.map(current=>current.evaluate(()=>events.filter(e=>e.event==='error'))))).flat();
    assert.deepEqual(failures,[]);
    const status=await Promise.all(frames.map(frame=>frame.evaluate(()=>({samples:samples.size,diagnostics}))));
    const current=Math.min(...status.map(item=>item.diagnostics.lastConfirmed||0));
    if(enableCpuThrottle&&current>=cpuWarmupThrough){
     await enableCpuThrottle();enableCpuThrottle=undefined;
    }
    if(current>frontier){
     if((extended||extraBoss)&&Math.floor(current/1000)>Math.floor(frontier/1000))console.log('room confirmed through '+current);
     frontier=current;progressAt=Date.now();
    }else if(Date.now()-progressAt>20000)throw Error('Room stopped confirming input: '+JSON.stringify(status));
    if(throughStageTwo&&status.every(item=>item.diagnostics.phase===2))
     throw Error('The fixture reached the pause or game-over menu before Stage 2');
    if(throughStageTwo)return status.every(item=>item.diagnostics.stageTwoFrames>=240);
    if(extraFinale&&status.every(item=>item.diagnostics.finalFinished)){
     cutoff=Math.min(...status.map(item=>item.diagnostics.finalFinished));
     return true;
    }
    if(extraBoss)return current>=extendedTarget;
    return status.every(item=>item.samples>=minimumSamples)&&(!extended||!rollback||
     status.some(item=>item.diagnostics.spellCorrections>0)||current>=extendedTarget);
   },throughStageTwo?'Stage 1 to Stage 2 transition':'confirmed room samples',extraFinale?1500000:extraBoss?900000:throughStageTwo?540000:extended||denseExtra?300000:90000*cpuThrottle);}
   catch(error){coverageFailure=error;}
   const states=await Promise.all(frames.map(frame=>frame.evaluate(()=>Array.from(samples))));
   if(extraFinale)finaleSamples=await frames[0].evaluate(()=>heapSamples);
   const first=new Map(states[0]),other=states.slice(1).map(state=>new Map(state));
   const agreed=[...first].filter(([frame])=>(!extraBoss||frame<=cutoff)&&other.every(map=>map.has(frame)));
   assert.ok(agreed.length>100);
   let difference;
   for(const [frame,value] of agreed){
    const peer=other.find(map=>map.get(frame).some((word,index)=>word!==value[index]));
    if(peer){difference={frame,host:value,peer:peer.get(frame),previous:agreed.find(([n])=>n===frame-1)?.[1]};break;}
   }
   const [,initial]=agreed[0];
   const [,last]=agreed.at(-1);
   assert.equal(initial[3],difficulty===4?7:1,'room starts the selected stage');
   if(checkPresentation)
    assert.deepEqual(initial.slice(24,24+seats),Array(seats).fill(difficulty===4?400:100),'all pilots start with the stage power');
   const diagnostics=await Promise.all(frames.map(frame=>frame.evaluate(extraBoss=>{
    const summarize=values=>{
     values.sort((a,b)=>a-b);
     return {count:values.length,p50:values[Math.floor(values.length*.5)],p95:values[Math.floor(values.length*.95)],max:values.at(-1)};
    };
    const pointer=Module._th17_room_performance?.()>>>2;
    const [captures,captureUs,replaySlices,logicalDraws,stageCorrections,bossCorrections,spellCorrections]=pointer?Module.HEAPU32.subarray(pointer,pointer+7):[];
    const bossCosts=[];
    if(extraBoss)for(const boss of new Set(frameCosts.map(entry=>entry.boss).filter(Boolean))){
     const aggregate=rows=>{
      const totals=Array(6).fill(0);
      for(const row of rows)for(let index=0;index<totals.length;index++)totals[index]+=row.costs[index];
      const [physical,physicalUs,logical,logicalUs,capture,captureUs]=totals;
      return {physical,physicalUs,logical,logicalUs,capture,captureUs,repairs:summarize(rows.filter(row=>row.repair).map(row=>row.time))};
     };
     const rows=frameCosts.filter(entry=>entry.boss===boss);
     bossCosts.push({boss,nonspell:aggregate(rows.filter(entry=>entry.spell<0)),spell:aggregate(rows.filter(entry=>entry.spell>=0)),
      dense:aggregate(rows.filter(entry=>entry.bullets>=256))});
    }
    const canvas=Module.canvas,gl=canvas.getContext('webgl2'),info=gl?.getExtension('WEBGL_debug_renderer_info');
    const denseCallbacks=extraBoss?frameCosts.filter(row=>row.frame>=9000&&row.bullets>=256):[];
    const longCallbacks=denseCallbacks.filter(row=>row.time>1000/60);
    const tail=extraBoss?{
     callbacks:denseCallbacks.length,
     overBudget:longCallbacks.length,
     overBudgetWithRepair:longCallbacks.filter(row=>row.repair).length,
     totalMs:summarize(longCallbacks.map(row=>row.time)),
     captureMs:summarize(longCallbacks.map(row=>row.costs[5]/1000)),
     physicalDrawMs:summarize(longCallbacks.map(row=>row.costs[1]/1000)),
     logicalDrawMs:summarize(longCallbacks.map(row=>row.costs[3]/1000)),
     restoreMs:summarize(longCallbacks.map(row=>row.costs[7]/1000)),
     releaseMs:summarize(longCallbacks.map(row=>row.costs[9]/1000)),
     remainingMs:summarize(longCallbacks.map(row=>Math.max(0,row.time-(row.costs[1]+row.costs[3]+row.costs[5]+row.costs[7]+row.costs[9])/1000))),
    }:undefined;
    return {...diagnostics,captures,captureUs,replaySlices,logicalDraws,stageCorrections,bossCorrections,spellCorrections:spellCorrections??diagnostics.spellCorrections,bossCosts,
     tail,
     canvas:[canvas.width,canvas.height],renderer:info?gl.getParameter(info.UNMASKED_RENDERER_WEBGL):null,
     presentIntervals:summarize(presentIntervals),
     storyPresentIntervals:Object.fromEntries([...storyPresentIntervals].map(([segment,gaps])=>[segment,{
      ...summarize(gaps),fps:gaps.length*1000/gaps.reduce((sum,gap)=>sum+gap,0),
     }])),
     densePresentIntervals:{...summarize(densePresentIntervals),fps:densePresentIntervals.length?
      densePresentIntervals.length*1000/densePresentIntervals.reduce((a,b)=>a+b):null},
     delayedPackets:globalThis.delayedPackets,
     updates:summarize(frameCosts.filter(entry=>!entry.repair).map(entry=>entry.time)),
     repairs:summarize(frameCosts.filter(entry=>entry.repair).map(entry=>entry.time)),
     denseUpdates:summarize(frameCosts.filter(entry=>!entry.repair&&entry.bullets>=256).map(entry=>entry.time)),
     denseRepairs:summarize(frameCosts.filter(entry=>entry.repair&&entry.bullets>=256).map(entry=>entry.time))};
   },extraBoss)));
   if(checkPresentation){
    for(const lane of diagnostics){
     assert.deepEqual(lane.canvas,[640*renderScale,480*renderScale],'room uses the requested output resolution');
     assert.ok(lane.actualPresentations>0&&lane.presentationEnd>lane.presentationStart,'window commits are counted');
    }
    const health=(await Promise.all(pages.map(current=>current.evaluate(()=>events.filter(event=>event.event==='frame-health'))))).flat();
    for(let seat=0;seat<seats;seat++){
     const samples=health.filter(event=>event.epoch===seat+1).slice(-5);
     assert.ok(samples.length>0&&samples.every(event=>Number.isFinite(event.fps)&&event.fps>=0),'frame health reports window commits');
     const lane=diagnostics[seat];
     const measured=lane.actualPresentations*1000/(lane.presentationEnd-lane.presentationStart);
     if(measured<57)assert.ok(samples.some(event=>event.fps<59),'frame health reports missed presentations');
    }
   }
   const maxEnemyBullets=Math.max(...agreed.map(([,state])=>state[10]));
   const denseWindows=[];
   if(denseExtra)for(const [frame,state] of [...agreed].sort((a,b)=>b[1][10]-a[1][10])){
    if(denseWindows.some(window=>Math.abs(window.stageClock-state[4])<=120))continue;
    denseWindows.push({frame,stageClock:state[4],bullets:state[10],spell:state[30]});
    if(denseWindows.length===5)break;
   }
   const maxLasers=Math.max(...agreed.map(([,state])=>state[29]));
   const activeSpellFrames=agreed.filter(([,state])=>state[30]&1).length;
   const dialogueFrames=agreed.filter(([,state])=>state[33]).length;
   const bossNonspellFrames=agreed.filter(([,state])=>state[38]&&!(state[30]&1)&&!state[33]).length;
   const stageTwoFrames=agreed.filter(([,state])=>state[3]===2).length;
   const bossPatterns=[],settledPeaks=[];
   if(extraBoss){
    assert.ok(diagnostics[1].delayedPackets>0,'guest used the delayed RTC channel');
    let active,settled;
    for(const [frame,state] of agreed){
     const boss=state[41],spell=state[30]&1?state[39]:-1,routine=state[42];
     if(!boss||state[33]){active=undefined;continue;}
     if(!active||active.boss!==boss||active.spell!==spell||(spell<0&&active.routine!==routine)){
      active={boss,spell,routine,first:frame,last:frame,peak:0,lasers:0};
      bossPatterns.push(active);
      settled={boss,spell,routine,first:frame,bullets:0,lasers:0};
      settledPeaks.push(settled);
     }
     active.last=frame;
     active.lasers=Math.max(active.lasers,state[29]);
     if(state[10]>active.peak){active.peak=state[10];active.peakFrame=frame;active.peakClock=state[4];}
     // The next pattern may inherit projectiles from its predecessor.
     if(frame>=active.first+90&&state[10]>settled.bullets){
      settled.bullets=state[10];settled.bulletClock=state[4];
     }
     if(state[29]>settled.lasers){
      settled.lasers=state[29];settled.laserClock=state[4];
     }
    }
   }
   reports.push({room,seats,comparedFrames:agreed.length,maxEnemyBullets,maxLasers,activeSpellFrames,dialogueFrames,bossNonspellFrames,stageTwoFrames,denseWindows,
    bossPatterns:bossPatterns.filter(pattern=>pattern.last-pattern.first>=30),
    ...(priorMemory?{matchedMemory:compareMemory(priorMemory.samples,finaleSamples),lateMatchedMemory:compareMemory(priorMemory.samples,finaleSamples,lateStart)}:{}),
    settledPeaks:settledPeaks.filter(peak=>peak.bullets||peak.lasers),diagnostics,initialPower:initial.slice(24,24+seats),last,difference});
   if(coverageFailure)throw coverageFailure;
   if(difference)assert.deepEqual(difference.peer,difference.host,'world at frame '+difference.frame);
   if(rollback){
    assert.ok(diagnostics.some(item=>item.corrections>0&&item.resimulated>0),'late input triggers correction and replay');
    assert.ok(diagnostics.some(item=>item.combatCorrections>0),'at least one correction happens while enemy projectiles are present');
    assert.ok(diagnostics.every(item=>item.maxPrediction<=12));
    if(diagnostics[0].captures!==undefined){
     assert.ok(diagnostics.some(item=>item.logicalDraws>0),'intermediate replay omits GPU draws');
     assert.ok(diagnostics.some(item=>item.stageCorrections>0),'regular stage play includes late corrections');
     if(extended&&difficulty!==4){
      assert.ok(bossNonspellFrames>0&&diagnostics.some(item=>item.bossCorrections>0),'boss nonspell includes late corrections');
      assert.ok(activeSpellFrames>0&&diagnostics.some(item=>item.spellCorrections>0),'spell combat includes late corrections');
     }
     if(throughStageTwo)assert.ok(stageTwoFrames>=200,'next stage confirmed on both peers');
    }
   }
   assert.ok(maxEnemyBullets>0,'fight includes enemy projectiles');
   if(extraBoss){
    const mainId=bossPatterns.find(pattern=>pattern.spell>=91)?.boss;
    const main=diagnostics[0].bossCosts.find(row=>row.boss===mainId);
    assert.ok(main?.nonspell.repairs.count>0&&main?.spell.repairs.count>0,'final boss nonspells and spells both include late corrections');
    assert.ok(new Set(bossPatterns.filter(pattern=>pattern.boss===main.boss&&pattern.spell>=0).map(pattern=>pattern.spell)).size>=2,
     'final boss has multiple spell cards');
    if(extraFinale){
     assert.ok(diagnostics.every(item=>item.finalFinished===cutoff&&item.finalLast-item.finalFirst>=180),'all seats finish the last spell');
     for(let spell=91;spell<=100;++spell)
      assert.ok(bossPatterns.some(pattern=>pattern.spell===spell&&pattern.last-pattern.first>=30),'covered final boss spell '+spell);
    }
    if(baseline){
     const prior=baseline.reports.find(report=>report.seats===seats);
     assert.deepEqual(last,prior.last,'fixed-input endpoint agrees with baseline');
     assert.deepEqual(reports.at(-1).bossPatterns,prior.bossPatterns,'boss patterns and projectile peaks agree with baseline');
    }
   }
   assert.equal(initial[19],4,'Otter starts with four bombs');
   assert.ok(agreed.some(([,state])=>state[19]===3&&state[15]===initial[15]),'guest spends their own bomb');
   assert.ok(agreed.some(([,state])=>(state[12]|0)<(state[16]|0)),'left and right belong to different pilots');
   if(seats===3)assert.ok(agreed.some(([,state])=>state[21]<state[13]),'third pilot moved up independently');
   if(independent)await Promise.all(pages.map((current,seat)=>current.screenshot({path:resolve(out,seats+'p-seat'+(seat+1)+'.png')})));
   else await page.screenshot({path:resolve(out,seats+'p.png')});
   console.log(seats+' players, '+agreed.length+' matching confirmed frames, '+diagnostics.map(item=>item.corrections).join('/')+' corrections');
   if(verifyReplay){
    const replay=await frames[0].evaluate(()=>{
     if(!Module._th17_mp_replay_save())throw Error('Native Replay save failed');
     return Array.from(FS.readFile('/savesth17mp/replay/th17mp_01.rpy'));
    });
    await Promise.all(frames.map(frame=>frame.evaluate(()=>Module._th17_loop_stop())));
    await page.evaluate(seats=>addRuntime(seats),seats);
    await page.waitForFunction(epoch=>ready.has(epoch),seats+1,{timeout:45000});
    const viewer=page.frames().at(-1),path='replay/th17mp_98.rpy';
    await page.evaluate(async({seat,path,bytes})=>{
     await rpc(seat,'configure',{music:'none',language:'ja',options:{replayViewer:true,renderScale:1},
      sharedResources:['/msgothic.ttc','/unifont.otf'].map(path=>({path,url:path}))});
     await rpc(seat,'launch');
     await rpc(seat,'write',{path,bytes});
     await rpc(seat,'playback',{action:'open',path});
    },{seat:seats,path,bytes:replay});
    const waitReplay=frame=>viewer.waitForFunction(target=>{
     const pointer=Module._eagler_replay_state()>>>2;
     return Module.HEAPU32[pointer]>=target;
    },frame,{timeout:15000});
    await waitReplay(120);
    const canvas=viewer.locator('canvas');
    const firstImage=await canvas.screenshot();
    await waitReplay(220);
    assert.notDeepEqual(await canvas.screenshot(),firstImage,'Replay presents new gameplay frames');
    await page.evaluate(async seat=>{
     await rpc(seat,'playback',{action:'pause'});
     await rpc(seat,'playback',{action:'seek',value:150});
    },seats);
    await viewer.waitForFunction(()=>Module.HEAPU32[Module._eagler_replay_state()>>>2]===150,null,{timeout:15000});
    assert.notDeepEqual(await canvas.screenshot(),firstImage,'Paused seek presents its destination');
    await page.evaluate(seat=>rpc(seat,'playback',{action:'play'}),seats);
    await waitReplay(240);
    assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);
    reports.at(-1).replay={playedThrough:240,seekTarget:150,canvasAdvanced:true};
    console.log(seats+' players, saved Replay plays, presents and seeks');
   }
  }finally{await Promise.all(contexts.map(context=>context.close()));for(const socket of members)socket.close();}
 }
}finally{
 await browser?.close();for(const socket of sockets)socket.close();relay.kill();server.closeAllConnections();server.close();
 await writeFile(resolve(out,'room-report.json'),JSON.stringify({rollback,extended,difficulty,version:manifest.version,wasm:manifest.execution.sha256,
  throughStageTwo,denseExtra,extraBoss,extraFinale,independent,renderScale,injectedDelayMs:rollback?delayMs:0,jitterMs:rollback?jitterMs:0,cpuThrottle,cpuWarmupThrough,reports},null,2)+'\n');
 if(finaleSamples)await writeFile(resolve(out,'heap-samples.json'),JSON.stringify({
  fields:['nextFrame','bullets','spell','heap','lasers','snapshots','unusedCapacity','bulletState','bulletVisual','itemVisual','idleItemVisual','animation'],
  samples:finaleSamples})+'\n');
 await writeFile(resolve(out,'relay.log'),log);
}
