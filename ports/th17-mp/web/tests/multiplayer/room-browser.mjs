import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,dirname,extname,sep} from 'node:path';
import {launchBrowser} from '../../../../../tools/browser/launch.mjs';
import {checkDirectInput} from '../direct-input-browser.mjs';

const repo=resolve(import.meta.dirname,'../../../../..');
const runtime=resolve(process.env.EAGLER_TH17_BUILD||resolve(repo,'ports/th17-mp/build-eagler-multiplayer'));
const out=resolve(process.env.TH17_ROOM_EVIDENCE||resolve(repo,'artifacts/th17-room'));
const rollback=!process.argv.includes('--lockstep');
const throughStageThree=process.argv.includes('--through-stage-three');
const throughStageTwo=throughStageThree||process.argv.includes('--through-stage-two');
const verifySpectators=process.argv.includes('--spectator');
assert.ok(!verifySpectators||throughStageTwo&&!throughStageThree,'Spectator fixture crosses the Stage 2 boundary');
const fixtureStage=Number(process.env.TH17_ROOM_STORY_STAGE||0);
assert.ok(Number.isInteger(fixtureStage)&&fixtureStage>=0&&fixtureStage<=6);
const storyBulletFloor=Number(process.env.TH17_ROOM_STORY_BULLET_FLOOR||0);
const storyBulletFrames=Number(process.env.TH17_ROOM_STORY_BULLET_FRAMES||0);
const storySpellFrames=Number(process.env.TH17_ROOM_STORY_SPELL_FRAMES||0);
assert.ok([storyBulletFloor,storyBulletFrames,storySpellFrames].every(value=>Number.isInteger(value)&&value>=0));
assert.ok(!storyBulletFrames||fixtureStage&&storyBulletFloor>0);
assert.ok(!storySpellFrames||fixtureStage);
const storyStage=throughStageThree?3:2;
const storySamples=throughStageThree?1800:240;
const extraFinale=process.argv.includes('--extra-finale');
const launcherPreview=process.argv.includes('--launcher-preview');
const verifyReplay=process.argv.includes('--replay');
const checkPresentation=process.argv.includes('--check-presentation');
const verifyTransfer=process.argv.includes('--life-transfer');
const verifyGhost=process.argv.includes('--ghost-revive');
const challenge=process.argv.includes('--challenge');
const spectatorState=words=>challenge?[...words.slice(0,39),...words.slice(51)]:words;
const independent=process.argv.includes('--independent-pages');
const cpuProfile=process.argv.includes('--cpu-profile');
const traceGaps=process.argv.includes('--trace-gaps');
const tracePhases=process.argv.includes('--trace-phases');
assert.ok(!tracePhases||traceGaps,'Phase timing requires gap tracing');
const touchComparison=process.argv.includes('--touch-comparison');
const touchDrag=touchComparison||process.argv.includes('--touch-drag');
const touchWindow=touchComparison?{keyboard:480,start:960,end:3500}:{keyboard:600,start:600,end:1800};
const symmetricDelay=process.argv.includes('--symmetric-delay');
const touchSeat=Number(process.env.TH17_ROOM_TOUCH_SEAT??1);
const touchSeats=process.env.TH17_ROOM_TOUCH_SEATS?.split(',').map(Number)??[touchSeat];
const observeDisplay=process.argv.includes('--observe-display');
const throttleSeat=process.env.TH17_ROOM_CPU_THROTTLE_SEAT===undefined?null:Number(process.env.TH17_ROOM_CPU_THROTTLE_SEAT);
const glTrace=process.env.TH17_ROOM_GL_TRACE==='1';
const musicDirectory=process.env.TH17_ROOM_MUSIC_DIR;
const music=musicDirectory?'ogg':'none';
assert.ok(!independent||!verifyReplay,'Replay fixture uses the shared page');
const requestedRenderScale=process.env.TH17_ROOM_RENDER_SCALE;
const renderScale=Number(requestedRenderScale??2);
assert.ok(renderScale===1||renderScale===2);
const seatRenderScales=process.env.TH17_ROOM_RENDER_SCALES?.split(',').map(Number);
if(seatRenderScales){
 assert.ok(requestedRenderScale===undefined,'Choose a single room scale or per-seat scales');
 assert.ok([2,3].includes(seatRenderScales.length)&&seatRenderScales.every(scale=>scale===1||scale===2),
  'Use two or three comma-separated render scales (1 or 2)');
}
const extraBoss=extraFinale||process.argv.includes('--extra-boss');
const denseExtra=extraBoss||process.argv.includes('--dense-extra');
const extended=throughStageTwo||fixtureStage!==0||process.argv.includes('--extended');
const extendedSeats=Number(process.env.TH17_ROOM_SEATS??2);
assert.ok(extendedSeats===2||extendedSeats===3);
const roomSeatCounts=seatRenderScales?[seatRenderScales.length]:
 touchDrag?[extendedSeats]:extraBoss?[3]:extended?[extendedSeats]:[2,3];
const validSeat=seat=>Number.isInteger(seat)&&seat>=0&&seat<Math.min(...roomSeatCounts);
if(touchDrag)assert.ok(touchSeats.every(validSeat)&&new Set(touchSeats).size===touchSeats.length&&
 (touchSeats.length===1||independent),'Multiple moving seats require independent pages and distinct valid seat indices');
if(throttleSeat!==null)assert.ok(independent&&validSeat(throttleSeat),'Per-seat CPU throttling needs independent pages');
const difficulty=Number(process.env.TH17_ROOM_DIFFICULTY??(denseExtra?4:throughStageTwo?0:1));
assert.ok(Number.isInteger(difficulty)&&difficulty>=0&&difficulty<=4);
assert.ok(!throughStageTwo||difficulty!==4);
const minimumSamples=Number(process.env.TH17_ROOM_SAMPLES||(touchComparison?3850:denseExtra?4200:1050));
const extendedTarget=Number(process.env.TH17_ROOM_TICKS||(extraFinale?39000:extraBoss?15000:12000));
const delayMs=Number(process.env.TH17_ROOM_DELAY_MS||110);
const jitterMs=Number(process.env.TH17_ROOM_JITTER_MS||0);
const cpuThrottle=Number(process.env.TH17_ROOM_CPU_THROTTLE||1);
const manifest=JSON.parse(await readFile(resolve(runtime,'manifest.json'),'utf8'));
if(fixtureStage)assert.ok(manifest.features.roomBenchmark&&difficulty!==4&&!throughStageTwo,
 'Direct story entry requires the story benchmark build');
const directInput=touchDrag&&!!JSON.parse(await readFile(resolve(runtime,'runtime-files.json'),'utf8')).files['direct-input-bridge.mjs'];
if(extraBoss)assert.ok(manifest.features.roomBenchmark&&rollback&&difficulty===4,'Extra boss profiling needs the dedicated Extra rollback benchmark build');
if(verifyGhost)assert.ok(manifest.features.roomBenchmark&&throughStageTwo&&roomSeatCounts[0]===3,
 'Ghost revival requires a three-seat benchmark room through Stage 2');
if(challenge)assert.ok(manifest.features.roomBenchmark&&difficulty!==4&&!verifyGhost&&!fixtureStage,
 'Challenge fixture requires the story benchmark runtime');
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
const musicResources=[];
const storyMusic=Array.from({length:Math.max(throughStageThree?3:2,fixtureStage)*2},(_,i)=>'th17_'+String(i+3).padStart(2,'0'));
if(musicDirectory)for(const name of ['th17_01',...storyMusic,'th128_08']){
 const path='/music/'+name+'.ogg';
 assets[path]=await readFile(resolve(musicDirectory,name+'.ogg'));
 musicResources.push({path,url:path});
}
assert.ok(!musicDirectory||!denseExtra,'OGG profiling fixture covers the title and story stages 1–2');
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
 const configure={music:${JSON.stringify(music)},language:'ja',resources:${JSON.stringify(musicResources)}};
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
 for(const seats of roomSeatCounts){
  const room='th17mp-room-test-'+seats,loadouts=Array.from({length:seats},(_,i)=>({character:i,shot:i}));
  const members=[];
  for(let seat=0;seat<seats;seat++){
   const member=room+'-private-'+seat;
   const socket=new WebSocket(endpoint+new URLSearchParams({room,lobby:room+'-'+seat,member,intent:seat?'join':'create',players:String(seats),difficulty:String(difficulty)}));
   sockets.push(socket);members.push(socket);
   await lobby(socket,null,value=>!!value.room);
   await lobby(socket,{type:'take-seat',seat,loadout:seat*3+seat},value=>!!value.room?.seats[seat]);
  }
  if(challenge)await lobby(members[0],{type:'settings',playerCount:seats,difficulty,challengeMode:true},value=>value.room?.challengeMode===true);
  for(const [seat,socket] of members.entries())
   await lobby(socket,{type:'set-ready',ready:true},value=>value.room?.seats[seat]?.ready);
  const timing={adonisMode:rollback?0:1,inputDelay:rollback?0:2,inputDelayAuto:false};
  const start=await lobby(members[0],{type:'start',...timing},value=>value.type==='start');
  assert.equal(start.room.challengeMode,challenge);
  options=Array.from({length:seats},(_,seat)=>({
   ...(touchDrag?{touchEnabled:true,touchMovementMode:'touch-unlimited'}:{}),
   netplayMode:'lan',netplayAdonisMode:timing.adonisMode,netplayInputDelay:timing.inputDelay,netplayAllowWebsocketFallback:!extraBoss,
   netplayChallengeMode:challenge,netplayPlayer:seat,netplayPlayerCount:seats,netplaySeed:1234,netplayDifficulty:difficulty,netplayLoadouts:loadouts,
   ...(seatRenderScales||requestedRenderScale?{renderScale:seatRenderScales?.[seat]??renderScale}:{}),
   netplayUrl:endpoint+new URLSearchParams({room,run:String(start.serial),member:room+'-private-'+seat}),
  }));
  const contexts=await Promise.all(Array.from({length:independent?seats:1},()=>browser.newContext({viewport:{width:independent?640:seats*640,height:480}})));
  if(glTrace)await Promise.all(contexts.map(context=>context.addInitScript(()=>{
   globalThis.__th17GlCalls={};
   for(const name of ['enableVertexAttribArray','vertexAttribDivisor','vertexAttribPointer','drawArraysInstanced']){
    const original=WebGL2RenderingContext.prototype[name];
    WebGL2RenderingContext.prototype[name]=function(...args){
     __th17GlCalls[name]=(__th17GlCalls[name]||0)+1;
     return original.apply(this,args);
    };
   }
  })));
  if(rollback)await Promise.all(contexts.map(context=>context.addInitScript(({delayMs,jitterMs,touchDrag})=>{
   const send=RTCDataChannel.prototype.send;
   if(touchDrag)globalThis.touchInputs=new Map();
   let sequence=0;
   RTCDataChannel.prototype.send=function(payload){
    if(touchDrag&&typeof payload!=='string'){
     const bytes=payload instanceof ArrayBuffer?new Uint8Array(payload):new Uint8Array(payload.buffer,payload.byteOffset,payload.byteLength);
     if(bytes.length>=56&&bytes[5]===1){
      const view=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength),count=bytes[42],offset=44+(count-1)*12;
      if(count&&offset+12<=bytes.length&&bytes[offset+2]===2)
       touchInputs.set(view.getUint32(24,true),[view.getFloat32(offset+4,true),view.getFloat32(offset+8,true)]);
     }
    }
    if(!globalThis.delayInput||typeof payload==='string')return send.call(this,payload);
    // Delay the guest's binary samples and repairs after the RTC handshake.
    // Otherwise a repaired input can arrive before the delayed sample.
    const copy=payload.slice();
    globalThis.delayedPackets=(globalThis.delayedPackets||0)+1;
    setTimeout(()=>{if(this.readyState==='open')send.call(this,copy);},delayMs+(++sequence%5)*jitterMs/4);
   };
  },{delayMs,jitterMs,touchDrag})));
  const pages=await Promise.all(contexts.map(context=>context.newPage())),page=pages[0],errors=[];
  const pageFor=seat=>pages[independent?seat:0];
  let enableCpuThrottle;
  if(cpuThrottle>1){
   const sessions=await Promise.all(contexts.map((context,index)=>context.newCDPSession(pages[index])));
   const throttle=()=>Promise.all(sessions.map((cdp,seat)=>cdp.send('Emulation.setCPUThrottlingRate',
    {rate:throttleSeat===null||seat===throttleSeat?cpuThrottle:1})));
   if(cpuWarmupThrough)enableCpuThrottle=throttle;
   else await throttle();
  }
  for(const current of pages)current.on('pageerror',error=>errors.push(error.message));
  let profiler;
  try{
   if(cpuProfile){
    profiler=await contexts[0].newCDPSession(page);
    await profiler.send('Profiler.enable');
    await profiler.send('Profiler.setSamplingInterval',{interval:1000});
    await profiler.send('Profiler.start');
   }
   await Promise.all(pages.map(current=>current.goto('http://127.0.0.1:'+server.address().port)));
   for(let seat=0;seat<seats;seat++){
    await pageFor(seat).evaluate(seat=>addRuntime(seat),seat);
    await pageFor(seat).waitForFunction(epoch=>ready.has(epoch),seat+1,{timeout:45000});
   }
   if(fixtureStage)for(const frame of pages.flatMap(current=>current.frames().filter(frame=>frame.url().includes('/runtime/'))))
    await frame.evaluate(stage=>{
     const open=Module._th17_room_open;
     Module._th17_room_open=(...args)=>{
      const ok=open(...args);
      if(ok&&!Module._th17_room_fixture_story_stage(stage))throw Error('Could not select benchmark story stage');
      return ok;
     };
    },fixtureStage);
   if(independent)await Promise.all(pages.map((current,seat)=>current.evaluate(seat=>startRuntime(seat),seat)));
   else await page.evaluate(seats=>Promise.all(Array.from({length:seats},(_,seat)=>startRuntime(seat))),seats);
   if(musicDirectory)await Promise.all(Array.from({length:seats},async(_,seat)=>{
    const current=pageFor(seat),epoch=seat+1;
    const startup=await current.waitForFunction(epoch=>events.find(event=>event.epoch===epoch&&
     (event.event==='error'||event.event==='audio-health'&&event.musicEnabled)),epoch,{timeout:45000});
    const event=await startup.jsonValue();
    assert.equal(event.event,'audio-health',JSON.stringify(event));
   }));
   const frames=pages.flatMap(current=>current.frames().filter(frame=>frame.url().includes('/runtime/')));
   if(verifyGhost||challenge)for(const frame of frames)
    assert.equal(await frame.evaluate(seat=>Module._th17_room_fixture_ghost(seat),seats-1),1,
     'schedule the same last-life hit on each peer');
   if(touchDrag&&directInput)for(const [seat,frame] of frames.entries())await checkDirectInput(frame,seat+1);
   if(touchDrag)for(const touchSeat of touchSeats)await pageFor(touchSeat).evaluate(({seat,touchWindow,touchComparison,stagger})=>{
    let active=false,established=false,sent=0,lastDown=-100,downs=0,direct=0,fallback=0;
    let startedAt=0;
    globalThis.touchDriver=setInterval(()=>{
     const target=document.getElementById('p'+seat)?.contentWindow;
     if(!target?.Module)return;
     const module=target.Module,pointer=module._th17_room_rollback()>>>2,n=module.HEAPU32[pointer];
     const dragging=n>=touchWindow.start&&n<touchWindow.end;
     if(!dragging&&!active)return;
     established||=target.touchInputs?.size>0;
     const now=performance.now();
     if(dragging&&!active)startedAt=now;
     const time=(touchComparison?(now-startedAt)/1000:n/60)+stagger*seat;
     const message={protocol:'eagler-touhou/1',game:'th17',epoch:seat+1,command:'direct-touch',
      type:dragging?active&&(established||n-lastDown<20)?'move':'down':'up',id:73,
      x:.5+Math.sin(time*2)*.13+(touchComparison?Math.sin(time*11)*.025:0),
      y:.78+Math.cos(time*2)*.025};
     if(message.type==='down'){lastDown=n;++downs;}
     const bridge=target.__eaglerDirectInputBridge;
     if(bridge?.submit(message))++direct;
     else {++fallback;target.postMessage(message,location.origin);}
     active=dragging;++sent;
     globalThis.touchEvents=sent;
     globalThis.touchDelivery={downs,direct,fallback};
    },1000/120);
   },{seat:touchSeat,touchWindow,touchComparison,stagger:touchSeats.length>1?1:0});
   for(const [seat,frame] of frames.entries()){
    await frame.evaluate(({seat,rollback,extended,throughStageTwo,storyStage,fixtureStage,storyBulletFloor,denseExtra,extraBoss,extraFinale,storyTiming,traceGaps,tracePhases,verifyTransfer,verifyGhost,challenge,touchDrag,touchSeats,touchWindow,symmetricDelay,observeDisplay})=>{
     const callback=Module.onGameFrame;
     if(traceGaps&&!Module._th17_room_wait_state)throw Error('Gap trace requires a runtime with room wait state');
     globalThis.stopRoomMeasurement=()=>{Module.onGameFrame=callback;};
     let displayPhase,displaySegment;
     if(observeDisplay){
      const video=document.createElement('video');
      video.muted=true;video.playsInline=true;
      video.style.cssText='position:fixed;top:0;left:0;width:2px;height:2px;pointer-events:none';
      video.srcObject=Module.canvas.captureStream();
      document.body.append(video);
      globalThis.displayIntervals={};
      let previousPhase,previousSegment,previousTime,previousPresentation,previousFrame,id;
      const recordDisplay=(key,now,metadata)=>{
        const interval=displayIntervals[key]??={
         time:0,frames:0,callbacks:0,maxGapMs:0,maxAdjacentPresentationGapMs:0,adjacentPresentationGapsOver25Ms:0,
        };
        const gap=now-previousTime;
        const frames=metadata.presentedFrames-previousFrame;
        interval.time+=gap;interval.frames+=frames;
        ++interval.callbacks;interval.maxGapMs=Math.max(interval.maxGapMs,gap);
        // A busy main thread can coalesce notifications for several video frames.
        if(frames===1){
         const presentationGap=metadata.presentationTime-previousPresentation;
         interval.maxAdjacentPresentationGapMs=Math.max(interval.maxAdjacentPresentationGapMs,presentationGap);
         if(presentationGap>25)++interval.adjacentPresentationGapsOver25Ms;
        }
      };
      const observe=(now,metadata)=>{
       if(displayPhase&&previousPhase===displayPhase)recordDisplay(displayPhase,now,metadata);
       if(displaySegment&&previousSegment===displaySegment)recordDisplay(displaySegment,now,metadata);
       previousPhase=displayPhase;previousSegment=displaySegment;previousTime=now;previousFrame=metadata.presentedFrames;
       previousPresentation=metadata.presentationTime;
       id=video.requestVideoFrameCallback(observe);
      };
      id=video.requestVideoFrameCallback(observe);
      void video.play();
      globalThis.stopRoomMeasurement=()=>{
       Module.onGameFrame=callback;video.cancelVideoFrameCallback(id);
       for(const track of video.srcObject.getTracks())track.stop();
       video.remove();
      };
     }
     if(globalThis.__th17GlCalls)globalThis.__th17GlCalls={};
     const optimized=typeof Module._th17_room_trace==='function';
     const storyStatusSize=fixtureStage?Module._th17_room_benchmark_story_status_size?.():0;
     Module._th17_room_trace?.(1);
     if(tracePhases&&!Module._th17_room_phase_costs)throw Error('Phase timing requires a runtime with room phase costs');
     if(tracePhases){
      Module._th17_room_phase_costs(1);
      globalThis.roomPhaseTrace={slow:[],browser:[],storage:[]};
      const {browser,storage}=roomPhaseTrace;
      if(PerformanceObserver.supportedEntryTypes.includes('long-animation-frame')){
       const observer=new PerformanceObserver(list=>{
        for(const frame of list.getEntries())browser.push({start:frame.startTime,ms:frame.duration,renderStart:frame.renderStart,
         scripts:frame.scripts.map(script=>({name:script.invoker,ms:script.duration,forcedLayoutMs:script.forcedStyleAndLayoutDuration}))});
       });
       observer.observe({type:'long-animation-frame'});
      }
      for(const [target,method] of [[Module,'_th17_mp_replay_save'],[Module,'_th17_save_scores'],[Module.FS,'syncfs']]){
       const action=target[method];
       if(typeof action!=='function')continue;
       target[method]=function(...args){
        const start=performance.now();
        try{return action.apply(this,args);}finally{storage.push({method,start,ms:performance.now()-start});}
       };
      }
     }
     globalThis.samples=new Map();
     globalThis.diagnostics={corrections:0,resimulated:0,maxPrediction:0,combatCorrections:0};
     globalThis.frameCosts=[];
     globalThis.presentIntervals=[];
     globalThis.presentFrames=[];
     globalThis.longPresentations=[];
     globalThis.densePresentIntervals=[];
     globalThis.storyPresentIntervals=new Map();
     globalThis.storyTimeline={};
     globalThis.touchPresentIntervals=new Map();
     globalThis.touchCosts={};
     globalThis.touchLayoutReads=0;
     if(touchDrag){
      const measure=Module.canvas.getBoundingClientRect.bind(Module.canvas);
      Module.canvas.getBoundingClientRect=()=>{++globalThis.touchLayoutReads;return measure();};
     }
     globalThis.heapSamples=[];
     let recorded=0,loadSample=0,bullets=0;
     let pendingCallbacks=[],pendingCallbackMs=0;
     let previousCallbackStart, maxCallbackStartGapMs=0, boundaryWaitCallbacks=0;
     let repairStreak=0,repairSegment;
     diagnostics.repairWaits={};
     let previousCosts=Array(10).fill(0);
     Module.onGameFrame=(ok,time,ticks,presented)=>{
      const shellStart=tracePhases?performance.now():0;
      callback(ok,time,ticks,presented);
      const shellMs=tracePhases?performance.now()-shellStart:0;
      const r=Module._th17_room_rollback()>>>2;
      const [n,committed,corrections,resimulated,combatCorrections,spellCorrections]=Module.HEAPU32.subarray(r,r+6);
      const phase=Module._th17_phase();
      const waitState=traceGaps||touchDrag?Module._th17_room_wait_state?.()??0:0;
      const storyState=samples.get(recorded);
      const segment=storyState?.[33]?'dialogue':storyState?.[30]&1?'spell':storyState?.[38]?'nonspell':'route';
      if(traceGaps&&fixtureStage&&n>120&&(waitState&1)&&!presented){
       if(segment!==repairSegment){repairStreak=0;repairSegment=segment;}
       const waits=diagnostics.repairWaits[segment]??={callbacks:0,longestStreak:0};
       waits.callbacks++;
       waits.longestStreak=Math.max(waits.longestStreak,++repairStreak);
      }else repairStreak=0;
      const phasePointer=tracePhases?Module._th17_room_phase_costs(1)>>>2:0;
      const touchPhase=n<touchWindow.keyboard?'stationary':n<touchWindow.start?'keyboard':
       n<touchWindow.end?'dragging':'released';
      const stage=storyState?.[3];
      if(n>120)displayPhase=touchDrag?touchPhase:stage?'stage'+stage:undefined;
      displaySegment=observeDisplay&&!touchDrag&&fixtureStage&&stage===fixtureStage&&n>120?displayPhase+'/'+segment:undefined;
      if(storyTiming&&stage){
       const timeline=storyTimeline[stage]??={ticks:0,commits:0,advancingCommits:0,start:performance.now()};
       timeline.end=performance.now();
       timeline.ticks+=ticks;
       timeline.commits+=presented;
       if(presented&&ticks)++timeline.advancingCommits;
      }
      if(touchDrag&&n>120){
       const cost=touchCosts[touchPhase]??={
        callbacks:0,withoutPresentation:0,repairWithoutPresentation:0,atPredictionCap:0,corrections:0,resimulated:0,
       };
       ++cost.callbacks;
       if(!presented){
        ++cost.withoutPresentation;
        if(waitState&1)++cost.repairWithoutPresentation;
        if(n-committed>=12)++cost.atPredictionCap;
       }
       cost.corrections+=corrections-diagnostics.corrections;
       cost.resimulated+=resimulated-diagnostics.resimulated;
      }
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
       if(traceGaps){
        // The engine time excludes the shell and browser scheduling. Idle
        // time between presentations can include regular callbacks waiting
        // for inputs, so keep their actual start gaps as separate evidence.
        const callbackStart=now-time-shellMs;
        if(diagnostics.lastPresent&&previousCallbackStart!==undefined)
         maxCallbackStartGapMs=Math.max(maxCallbackStartGapMs,callbackStart-previousCallbackStart);
        previousCallbackStart=callbackStart;
       }
       if(traceGaps&&diagnostics.lastPresent){
        pendingCallbackMs+=time;
        if(!ticks&&!presented&&(waitState&6))++boundaryWaitCallbacks;
        const timing={
         frame:n,confirmed:committed,offsetMs:now-diagnostics.lastPresent,callbackMs:time,ticks,presented,
         repair:resimulated-diagnostics.resimulated,waitState,
         ...(phasePointer?{shellMs,phaseUs:Array.from(Module.HEAPU32.subarray(phasePointer,phasePointer+10))}:{}),
        };
        if(pendingCallbacks.length<120)pendingCallbacks.push(timing);
        if(tracePhases&&time>1000/60)roomPhaseTrace.slow.push({...timing,end:now,stageClock:samples.get(recorded)?.[4]});
       }
       if(presented){
        if(diagnostics.lastPresent){
         const gap=now-diagnostics.lastPresent;
         presentIntervals.push(gap);
         if(traceGaps)presentFrames.push(n);
         if(touchDrag){
          if(!touchPresentIntervals.has(touchPhase))touchPresentIntervals.set(touchPhase,[]);
          touchPresentIntervals.get(touchPhase).push(gap);
         }
         if(bullets>=(storyBulletFloor||256)&&phase===1)densePresentIntervals.push(gap);
         const state=storyTiming&&samples.get(recorded);
         if(storyTiming&&(gap>=50||(traceGaps&&gap>25)))longPresentations.push({frame:n,confirmed:recorded,
          stage:state?.[3],stageClock:state?.[4],gap,callbackMs:time,
          resimulated:resimulated-diagnostics.resimulated,
          ...(traceGaps?{startMs:diagnostics.lastPresent,gameCallbacksMs:pendingCallbackMs,
           betweenCallbacksMs:Math.max(0,gap-pendingCallbackMs),maxCallbackStartGapMs,
           boundaryWaitCallbacks,callbacks:pendingCallbacks}:{}),
         });
         if(state){
          const segment=state[3]>1&&state[4]<90?'entry':state[33]?'dialogue':
           state[30]&1?'spell':state[38]?'nonspell':'route';
          const key=`${state[3]}/${segment}`;
          if(!storyPresentIntervals.has(key))storyPresentIntervals.set(key,[]);
          storyPresentIntervals.get(key).push(gap);
         }
        }
        if(traceGaps){pendingCallbacks=[];pendingCallbackMs=0;maxCallbackStartGapMs=0;boundaryWaitCallbacks=0;}
        diagnostics.lastPresent=now;
       }
      }
      if(n>120&&(ticks||resimulated>diagnostics.resimulated)&&(!denseExtra||phase===1)){
       const confirmed=samples.get(recorded);
       bullets=confirmed?.[10]??bullets;
       const entry={time,repair:resimulated>diagnostics.resimulated,bullets};
       if(storyBulletFloor&&bullets>=storyBulletFloor&&phasePointer){
        entry.updateMs=Module.HEAPU32[phasePointer+7]/1000;
        entry.drawMs=Module.HEAPU32[phasePointer+8]/1000;
       }
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
      globalThis.delayInput=rollback&&(symmetricDelay||seat===1)&&n>=115;
      for(let number=Math.max(recorded+1,committed-15);number<=committed;number++){
       const p=Module._th17_room_confirmed(number)>>>2;
       const words=Array.from(Module.HEAPU32.subarray(p,p+(storyStatusSize||(challenge?54:verifyGhost?44:extraBoss?43:optimized?39:38))));
       words[2]=0;
       if(words[0]===number&&number>120&&words[3]){
        samples.set(number,words);
        if(words[3]===storyStage)diagnostics.storyFrames=(diagnostics.storyFrames||0)+1;
        if(fixtureStage===words[3]){
         if(storyBulletFloor&&words[10]>=storyBulletFloor)diagnostics.storyBulletFrames=(diagnostics.storyBulletFrames||0)+1;
         if(words[30]&1)diagnostics.storySpellFrames=(diagnostics.storySpellFrames||0)+1;
        }
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
      if(verifyTransfer)Module._th17_key(42,+(seat===0&&n>=75&&n<105&&(n-75)%6<3));
      const centerGuest=extended&&seat===1&&n>=900&&n<952;
      const keyboardDodge=fixtureStage||storyStage===3&&stage===3;
      const reserve=(throughStageTwo||denseExtra)&&seat===1&&n>=1500&&!keyboardDodge;
      const touchControlled=touchDrag&&touchSeats.includes(seat);
      const moveGuest=seat===1&&n>=600&&!centerGuest&&!reserve&&!touchControlled||
       touchControlled&&touchPhase==='keyboard';
      const move=moveGuest||keyboardDodge&&seat===0||fixtureStage&&seat===2;
      const dodgeOffset=seat===2?6:seat*16;
      const dodge=(n+(fixtureStage?dodgeOffset:0))%96;
      Module._th17_key(205,+(seat===1&&n>=120&&n<170||move&&dodge<12||extended&&seat===0&&n>=300&&n<350));
      Module._th17_key(203,+(seat===0&&n>=120&&n<170||move&&dodge>=48&&dodge<60||centerGuest||reserve&&n<1630));
      Module._th17_key(200,+(seat===2&&n>=120&&n<150||reserve&&n<1630));
      Module._th17_key(44,+(n>=140&&(touchDrag||fixtureStage||n<260)||extended&&n>=900));
      Module._th17_key(46,+(extended&&seat===0&&n>=1400&&n%120<100));
      Module._th17_key(45,+(seat===1&&n>=185&&n<193||extended&&n>=1800&&(n+seat*420)%840<9||
       denseExtra&&n>=1500&&(n+seat*420)%720<9));
     };
    },{seat,rollback,extended,throughStageTwo,storyStage,fixtureStage,storyBulletFloor,denseExtra,extraBoss,extraFinale,traceGaps,tracePhases,verifyTransfer,verifyGhost,challenge,touchDrag,touchSeats,touchWindow,symmetricDelay,observeDisplay,storyTiming:checkPresentation&&difficulty!==4});
   }
   const viewers=[];
   let matchedOpening=false;
   const addViewer=async label=>{
    const seat=options.length,client=room+'-'+label,member=client+'-private';
    const socket=new WebSocket(endpoint+new URLSearchParams({room,lobby:client,member,intent:'join'}));
    sockets.push(socket);
    await lobby(socket,null,value=>!!value.room);
    await lobby(socket,{type:'spectate',name:label},value=>value.type==='spectator-start');
    options.push({...options[0],renderScale:1,netplaySpectator:true,netplaySpectatorId:client,
     netplayUrl:endpoint+new URLSearchParams({room,run:String(start.serial),member})});
    const context=await browser.newContext({viewport:{width:640,height:480}});
    contexts.push(context);
    const view=await context.newPage();
    view.on('pageerror',error=>errors.push(error.message));
    await view.goto('http://127.0.0.1:'+server.address().port);
    await view.evaluate(seat=>addRuntime(seat),seat);
    await view.waitForFunction(epoch=>ready.has(epoch),seat+1,{timeout:45000});
    const frame=view.frames().at(-1);
    await view.evaluate(seat=>startRuntime(seat),seat);
    if(challenge)assert.equal(await frame.evaluate(seat=>Module._th17_room_fixture_ghost(seat),seats-1),1);
    await frame.evaluate(challenge=>{
     const callback=Module.onGameFrame;
     const decoder=new TextDecoder();
     globalThis.spectatorSamples=new Map();
     globalThis.spectatorTimeline=()=>{
      const pointer=Module._th17_mp_spectator_state();
      return pointer?JSON.parse(decoder.decode(Module.HEAPU8.subarray(pointer,Module.HEAPU8.indexOf(0,pointer)))):null;
     };
     Module._th17_room_trace(1);
     Module.onGameFrame=(...args)=>{
      callback(...args);
      const timeline=spectatorTimeline();
      if(!timeline||timeline.seeking||spectatorSamples.has(timeline.position))return;
      const p=Module._th17_room_status()>>>2;
      const words=Array.from(Module.HEAPU32.subarray(p,p+(challenge?54:39)));
      words[0]=timeline.position;words[2]=0;
      if(words[3])spectatorSamples.set(timeline.position,words);
     };
    },challenge);
    viewers.push({label,view,frame});
   };
   if(verifySpectators){
    await wait(async()=>{
     const failures=(await Promise.all(pages.map(current=>current.evaluate(()=>events.filter(e=>e.event==='error'))))).flat();
     assert.deepEqual(failures,[]);
     return frames[0].evaluate(()=>diagnostics.lastConfirmed>180);
    },'first spectator history');
    await addViewer('early');
    console.log('early spectator joined Stage 1');
   }
   let frontier=0,progressAt=Date.now(),coverageFailure;
   let cutoff=extendedTarget;
   const sampleTimeout=fixtureStage?Math.max(300000,minimumSamples*80):
    extraFinale?1500000:extraBoss||throughStageThree?900000:throughStageTwo?540000:extended||denseExtra?300000:90000*cpuThrottle;
   try{await wait(async()=>{
    assert.deepEqual(errors,[]);
    const failures=(await Promise.all(pages.map(current=>current.evaluate(()=>events.filter(e=>e.event==='error'))))).flat();
    assert.deepEqual(failures,[]);
    const status=await Promise.all(frames.map(frame=>frame.evaluate(()=>({samples:samples.size,diagnostics}))));
    const current=Math.min(...status.map(item=>item.diagnostics.lastConfirmed||0));
    if(verifySpectators){
     for(const {view,label} of viewers){
      const failures=await view.evaluate(()=>events.filter(e=>e.event==='error'));
      assert.deepEqual(failures,[],label+' spectator Runtime errors');
     }
     if(!matchedOpening){
      const opening=await viewers[0].frame.evaluate(()=>Array.from(spectatorSamples).filter(([,words])=>words[3]===1).slice(0,200));
      if(opening.length===200){
       const hosts=await frames[0].evaluate(numbers=>numbers.map(number=>samples.get(number)),opening.map(([number])=>number));
       let compared=0;
       for(const [index,[number,words]] of opening.entries())if(hosts[index]){
        assert.deepEqual(spectatorState(words),spectatorState(hosts[index]),'opening spectator world at frame '+number);
        ++compared;
       }
       assert.ok(compared>=50);
       matchedOpening=true;
      }
     }
     if(viewers.length===1&&status[0].diagnostics.storyFrames>=100){
      await addViewer('late');
      console.log('late spectator joined Stage 2');
     }
     if(viewers.length===2){
      const counts=await Promise.all(viewers.map(({frame})=>frame.evaluate(()=>
       Array.from(spectatorSamples.values()).filter(words=>words[3]===2).length)));
      if(counts.every(count=>count>=180))return true;
     }
    }
    if(enableCpuThrottle&&current>=cpuWarmupThrough){
     await enableCpuThrottle();enableCpuThrottle=undefined;
    }
    if(current>frontier){
     if((extended||extraBoss||touchDrag)&&Math.floor(current/1000)>Math.floor(frontier/1000))console.log('room confirmed through '+current);
     frontier=current;progressAt=Date.now();
    }else if(Date.now()-progressAt>20000)throw Error('Room stopped confirming input: '+JSON.stringify(status));
    if(throughStageTwo&&status.every(item=>item.diagnostics.phase===2))
     throw Error('The fixture reached the pause or game-over menu before the story sample was complete');
    if(throughStageTwo)return !verifySpectators&&status.every(item=>item.diagnostics.storyFrames>=storySamples);
    if(fixtureStage)return status.every(({samples,diagnostics})=>samples>=minimumSamples&&
     (diagnostics.storyBulletFrames||0)>=storyBulletFrames&&(diagnostics.storySpellFrames||0)>=storySpellFrames);
    if(extraFinale&&status.every(item=>item.diagnostics.finalFinished)){
     cutoff=Math.min(...status.map(item=>item.diagnostics.finalFinished));
     return true;
    }
    if(extraBoss)return current>=extendedTarget;
    return status.every(item=>item.samples>=minimumSamples)&&(!extended||!rollback||
     status.some(item=>item.diagnostics.spellCorrections>0)||current>=extendedTarget);
   },throughStageTwo?'confirmed Stage '+storyStage+' samples':'confirmed room samples',sampleTimeout);}
   catch(error){coverageFailure=error;}
   // Exporting thousands of snapshots can block the renderer and inflate the
   // presentation tail. End timing before Playwright serializes those values.
   await Promise.all(frames.map(frame=>frame.evaluate(()=>stopRoomMeasurement())));
   const states=await Promise.all(frames.map(frame=>frame.evaluate(()=>Array.from(samples))));
   if(extraFinale)finaleSamples=await frames[0].evaluate(()=>heapSamples);
   const first=new Map(states[0]),other=states.slice(1).map(state=>new Map(state));
   const agreed=[...first].filter(([frame])=>(!extraBoss||frame<=cutoff)&&other.every(map=>map.has(frame)));
   assert.ok(agreed.length>100);
   let difference;
   for(const [frame,value] of agreed){
    // The viewer can have its own HUD animations.
    const peer=other.find(map=>map.get(frame).some((word,index)=>index!==47&&word!==value[index]));
    if(peer){difference={frame,host:value,peer:peer.get(frame),previous:agreed.find(([n])=>n===frame-1)?.[1]};break;}
   }
   const [,initial]=agreed[0];
   const [,last]=agreed.at(-1);
   assert.equal(initial[3],fixtureStage||(difficulty===4?7:1),'room starts the selected stage');
   if(checkPresentation&&!fixtureStage)
    assert.deepEqual(initial.slice(24,24+seats),Array(seats).fill(difficulty===4?400:100),'all pilots start with the stage power');
   const initialLives=Array.from({length:seats},(_,seat)=>initial[14+seat*4]);
   if(verifyTransfer)assert.deepEqual(initialLives,seats===2?[1,3]:[1,3,2],'one life goes to P2 without changing P3');
   const diagnostics=await Promise.all(frames.map(frame=>frame.evaluate(({extraBoss,traceGaps,storyBulletFloor})=>{
    const summarize=values=>{
     values.sort((a,b)=>a-b);
     return {count:values.length,p50:values[Math.floor(values.length*.5)],p95:values[Math.floor(values.length*.95)],max:values.at(-1)};
    };
    const load=Module._th17_room_load?.()>>>2;
    const liveHeapBytes=load?Module.HEAPU32[load+1]:null;
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
    const spellEntryGaps=traceGaps?[...samples].flatMap(([frame,state])=>{
     const previous=samples.get(frame-1);
     if(!(state[30]&1)||!previous||(previous[30]&1))return [];
     const gaps=presentFrames.flatMap((present,index)=>Math.abs(present-frame)<=2?[presentIntervals[index]]:[]);
     const waits=longPresentations.flatMap(gap=>gap.callbacks.filter(callback=>
      callback.frame===frame-1&&(callback.waitState&2))).length;
     return [{frame,stage:state[3],stageClock:state[4],maxGapMs:gaps.length?Math.max(...gaps):null,waits}];
    }):undefined;
    return {...diagnostics,touchInputFrames:globalThis.touchInputs?.size||0,touchInputTargets:new Set([...globalThis.touchInputs?.values()||[]].map(value=>value.join(','))).size,
     touchCombatInputs:[...samples].filter(([frame,state])=>state[10]>0&&globalThis.touchInputs?.has(frame)).length,touchLayoutReads:globalThis.touchLayoutReads,
     touchPresentIntervals:Object.fromEntries([...touchPresentIntervals].map(([segment,gaps])=>[segment,{
      ...summarize(gaps),fps:gaps.length*1000/gaps.reduce((sum,gap)=>sum+gap,0),
      overFrameBudget:gaps.filter(gap=>gap>1000/60).length,
      over25Ms:gaps.filter(gap=>gap>25).length,
     }])),
     touchCosts,displayIntervals:globalThis.displayIntervals,
     ...(traceGaps?{browserTimeOrigin:performance.timeOrigin}:{}),
     storyTimeline,
     glCalls:globalThis.__th17GlCalls,liveHeapBytes,captures,captureUs,replaySlices,logicalDraws,stageCorrections,bossCorrections,spellCorrections:spellCorrections??diagnostics.spellCorrections,bossCosts,
     tail,
     canvas:[canvas.width,canvas.height],renderer:info?gl.getParameter(info.UNMASKED_RENDERER_WEBGL):null,
     presentIntervals:summarize(presentIntervals),
     spellEntryGaps,
     longPresentations:longPresentations.sort((a,b)=>b.gap-a.gap).slice(0,20),
     phaseTrace:globalThis.roomPhaseTrace&&{...roomPhaseTrace,slow:roomPhaseTrace.slow.sort((a,b)=>b.callbackMs-a.callbackMs).slice(0,30)},
     presentationsOver50Ms:longPresentations.filter(gap=>gap.gap>=50).length,
     storyPresentIntervals:Object.fromEntries([...storyPresentIntervals].map(([segment,gaps])=>[segment,{
      ...summarize(gaps),fps:gaps.length*1000/gaps.reduce((sum,gap)=>sum+gap,0),
     }])),
     densePresentIntervals:{...summarize(densePresentIntervals),fps:densePresentIntervals.length?
      densePresentIntervals.length*1000/densePresentIntervals.reduce((a,b)=>a+b):null},
     ...(storyBulletFloor?{denseStoryPhases:{
      updateMs:summarize(frameCosts.filter(entry=>entry.updateMs!==undefined).map(entry=>entry.updateMs)),
      drawMs:summarize(frameCosts.filter(entry=>entry.drawMs!==undefined).map(entry=>entry.drawMs)),
     }}:{}),
     delayedPackets:globalThis.delayedPackets,
     updates:summarize(frameCosts.filter(entry=>!entry.repair).map(entry=>entry.time)),
     repairs:summarize(frameCosts.filter(entry=>entry.repair).map(entry=>entry.time)),
     denseUpdates:summarize(frameCosts.filter(entry=>!entry.repair&&entry.bullets>=(storyBulletFloor||256)).map(entry=>entry.time)),
     denseRepairs:summarize(frameCosts.filter(entry=>entry.repair&&entry.bullets>=(storyBulletFloor||256)).map(entry=>entry.time))};
   },{extraBoss,traceGaps,storyBulletFloor})));
   if(seatRenderScales||checkPresentation){
    for(const [seat,lane] of diagnostics.entries()){
     const scale=seatRenderScales?.[seat]??renderScale;
     assert.deepEqual(lane.canvas,[640*scale,480*scale],`Seat ${seat+1} uses its requested output resolution`);
    }
   }
   if(checkPresentation){
    for(const lane of diagnostics){
     assert.ok(lane.actualPresentations>0&&lane.presentationEnd>lane.presentationStart,'window commits are counted');
    }
    const health=(await Promise.all(pages.map(current=>current.evaluate(()=>events.filter(event=>event.event==='frame-health'))))).flat();
    for(let seat=0;seat<seats;seat++){
     const samples=health.filter(event=>event.epoch===seat+1);
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
   const stageThreeFrames=agreed.filter(([,state])=>state[3]===3).length;
   const keyboardMovement=fixtureStage?Array.from({length:seats},(_,seat)=>{
    const x=12+seat*4,y=x+1;
    let frames=0,denseFrames=0;
    for(let i=1;i<agreed.length;i++){
     const [number,state]=agreed[i], [prior,previous]=agreed[i-1];
     if(number!==prior+1||state[3]!==fixtureStage||state[x]===previous[x]&&state[y]===previous[y])continue;
     ++frames;
     if(storyBulletFloor&&state[10]>=storyBulletFloor)++denseFrames;
    }
    return {frames,denseFrames};
   }):undefined;
   if(keyboardMovement)assert.ok(keyboardMovement.every(seat=>seat.frames>40),'both pilots move using keyboard input');
   const fightLoad=fixtureStage&&initial.length>=50?Object.fromEntries(
    ['route','nonspell','spell'].map(segment=>{
     const frames=agreed.filter(([,state])=>state[3]===fixtureStage&&!state[33]&&
      (state[30]&1?'spell':state[38]?'nonspell':'route')===segment).map(([,state])=>state);
     const maximum=index=>frames.reduce((peak,state)=>Math.max(peak,state[index]),0);
     return [segment,{frames:frames.length,enemyBullets:maximum(10),animations:maximum(47),
      ...(initial.length>=51?{routeEnemyDefeats:frames.reduce((total,state)=>total+state[50],0)}:{}),
      playerShots:Array.from({length:seats},(_,seat)=>maximum(44+seat)),
      shotDamageFrames:frames.filter(state=>state[48]>0).length}];
    })):undefined;
   let ghostRevival;
   if(verifyGhost){
    const ghosts=agreed.filter(([,state])=>state[3]===1&&(state[43]&4));
    const positions=new Set(ghosts.map(([,state])=>state.slice(20,22).join('/')));
    const revived=agreed.find(([,state])=>state[3]===2&&!(state[43]&4)&&state[24+2]===200&&state[14+8]<0x80000000);
    assert.ok(ghosts.length>=90&&positions.size>1,'third pilot drifts as a ghost through Stage 1');
    assert.ok(revived,'third pilot revives with half power on Stage 2 activation');
    ghostRevival={ghostFrames:ghosts.length,firstGhost:ghosts[0][0],positions:positions.size,revived:revived[0]};
   }
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
   const movingSeats=touchDrag?await Promise.all(touchSeats.map(async seat=>({
    seat,...await pageFor(seat).evaluate(()=>({touchEvents:touchEvents||0,touchDelivery})),
   }))):[];
   const touchFrames=touchDrag&&touchSeats.length>1?await Promise.all(touchSeats.map(seat=>
    frames[seat].evaluate(()=>Array.from(touchInputs.keys())))):[];
   const otherTouches=touchFrames.slice(1).map(frames=>new Set(frames));
   const simultaneousTouchFrames=touchFrames[0]?.filter(frame=>otherTouches.every(touches=>touches.has(frame))).length;
   reports.push({room,seats,comparedFrames:agreed.length,maxEnemyBullets,maxLasers,activeSpellFrames,dialogueFrames,bossNonspellFrames,stageTwoFrames,stageThreeFrames,keyboardMovement,fightLoad,ghostRevival,denseWindows,
    ...(movingSeats.length===1?{touchEvents:movingSeats[0].touchEvents,touchDelivery:movingSeats[0].touchDelivery}:{}),
    ...(movingSeats.length>1?{movingSeats,simultaneousTouchFrames}:{}),
    bossPatterns:bossPatterns.filter(pattern=>pattern.last-pattern.first>=30),
    ...(priorMemory?{matchedMemory:compareMemory(priorMemory.samples,finaleSamples),lateMatchedMemory:compareMemory(priorMemory.samples,finaleSamples,lateStart)}:{}),
    settledPeaks:settledPeaks.filter(peak=>peak.bullets||peak.lasers),diagnostics,initialPower:initial.slice(24,24+seats),initialLives,last,difference});
   if(coverageFailure)throw coverageFailure;
   if(verifySpectators){
    const spectatorReports=[];
    for(const {label,frame} of viewers){
     const values=await frame.evaluate(()=>Array.from(spectatorSamples));
     const counts=[0,0,0];
     for(const [number,actual] of values){
      const expected=first.get(number);
      if(!expected)continue;
      assert.deepEqual(spectatorState(actual),spectatorState(expected),label+' spectator world at frame '+number);
      ++counts[actual[3]];
     }
     if(label==='early')assert.ok(counts[1]>=100,'early spectator matches Stage 1');
     assert.ok(counts[2]>=100,label+' spectator matches Stage 2');
     spectatorReports.push({label,stageOneFrames:counts[1],stageTwoFrames:counts[2]});
    }
    reports.at(-1).spectators=spectatorReports;
    console.log(JSON.stringify(spectatorReports));
   }
   if(touchDrag)for(const touchSeat of touchSeats){
    assert.ok(diagnostics[touchSeat].touchInputFrames>500,'touch movement reaches captured network inputs');
    assert.ok(diagnostics[touchSeat].touchInputTargets>100,'continuous drag produces changing field targets');
    assert.ok(diagnostics[touchSeat].touchCombatInputs>0,'touch movement overlaps enemy projectiles');
   }
   if(touchFrames.length)assert.ok(simultaneousTouchFrames>500,'the sliding seats supply input on the same frames');
   if(storySpellFrames)assert.ok(bossNonspellFrames>0&&activeSpellFrames>=storySpellFrames-15,'room covers boss nonspell and spell combat');
   if(difference)assert.deepEqual(difference.peer,difference.host,'world at frame '+difference.frame);
   if(rollback){
    assert.ok(diagnostics.some(item=>item.corrections>0&&item.resimulated>0),'late input triggers correction and replay');
    if(!touchDrag)assert.ok(diagnostics.some(item=>item.combatCorrections>0),'at least one correction happens while enemy projectiles are present');
    assert.ok(diagnostics.every(item=>item.maxPrediction<=12));
    if(diagnostics[0].captures!==undefined){
     assert.ok(diagnostics.some(item=>item.logicalDraws>0),'intermediate replay omits GPU draws');
     assert.ok(diagnostics.some(item=>item.stageCorrections>0),'regular stage play includes late corrections');
     if(extended&&!fixtureStage&&difficulty!==4){
      assert.ok(bossNonspellFrames>0&&diagnostics.some(item=>item.bossCorrections>0),'boss nonspell includes late corrections');
      assert.ok(activeSpellFrames>0&&diagnostics.some(item=>item.spellCorrections>0),'spell combat includes late corrections');
     }
     if(throughStageTwo)assert.ok(throughStageThree?stageThreeFrames>=1700:stageTwoFrames>=200,'target stage confirmed on both peers');
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
   if(challenge){
    assert.ok(agreed.every(([,state])=>!state[43]&&loadouts.every((_,seat)=>state[15+seat*4]===0)),
     'challenge pilots have no bombs and never become ghosts');
    const seat=seats-1;
    assert.ok(agreed.some(([frame,state])=>frame>=920&&state[51+seat]>0&&state[14+seat*4]===0&&state[13+seat*4]<480*128),
     'the last-life hit increments this pilot’s Miss count and the pilot respawns');
   }else{
    assert.equal(initial[19],4,'Otter starts with four bombs');
    assert.ok(agreed.some(([,state])=>state[19]===3&&state[15]===initial[15]),'guest spends their own bomb');
   }
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
    if(challenge)assert.equal(await viewer.evaluate(()=>Module.HEAPU32[(Module._th17_room_status()>>>2)+19]),0,
     'the saved challenge replay starts without bombs');
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
  }finally{
   try{
    if(profiler){const {profile}=await profiler.send('Profiler.stop');await writeFile(resolve(out,seats+'p.cpuprofile'),JSON.stringify(profile));}
   }finally{await Promise.all(contexts.map(context=>context.close()));for(const socket of members)socket.close();}
  }
 }
}finally{
 await browser?.close();for(const socket of sockets)socket.close();relay.kill();server.closeAllConnections();server.close();
 await writeFile(resolve(out,'room-report.json'),JSON.stringify({rollback,extended,difficulty,version:manifest.version,wasm:manifest.execution.sha256,
  throughStageTwo,throughStageThree,fixtureStage,denseExtra,extraBoss,extraFinale,independent,
  ...(fixtureStage?{storyBulletFloor,storyBulletFrames,storySpellFrames}:{}),
  ...(seatRenderScales?{seatRenderScales}:{renderScale}),music,cpuProfile,traceGaps,
  ...(tracePhases?{phaseOrder:['input','capture','restore','confirm','canvas','pcm','imageRelease','update','draw','flow']}:{}),
  glTrace,touchDrag,touchComparison,touchSeat,touchSeats,observeDisplay,throttleSeat,symmetricDelay,
  injectedDelayMs:rollback?delayMs:0,jitterMs:rollback?jitterMs:0,cpuThrottle,cpuWarmupThrough,reports},null,2)+'\n');
 if(finaleSamples)await writeFile(resolve(out,'heap-samples.json'),JSON.stringify({
  fields:['nextFrame','bullets','spell','heap','lasers','snapshots','unusedCapacity','bulletState','bulletVisual','itemVisual','idleItemVisual','animation'],
  samples:finaleSamples})+'\n');
 await writeFile(resolve(out,'relay.log'),log);
}
