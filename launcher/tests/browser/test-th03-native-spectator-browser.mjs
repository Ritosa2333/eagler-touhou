// Three production multiplayer Wasm instances, real local Relay and private DATA.
// Tests live playback, pause, seeking, and original sound effects.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import {spawn} from 'node:child_process';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
import {PRODUCT_GAMES,multiplayerInputTimingPolicy} from '../../../common/contracts/product-catalog.mjs';
import {verifyPc98IceRestart} from './verify-pc98-ice-restart.mjs';
const game=process.env.ROOM_RUNTIME_GAME;
assert(['th01','th03','th04'].includes(game),'ROOM_RUNTIME_GAME must be a multiplayer title');
const forceRelay=game==='th03'&&process.env.TH03_TEST_RELAY==='1';
const profileNative=game==='th03'&&process.env.TH03_TEST_PROFILE==='1';
const cpuThrottle=Number(process.env.TH03_TEST_CPU_THROTTLE??1);
const lateJoin=process.env.PC98_TEST_INPUT_ONLY_LATE==='1'||process.env.PC98_TEST_LATE_JOIN==='1';
const timing=multiplayerInputTimingPolicy(PRODUCT_GAMES[game].multiplayer);
const spectatorCanvas=game==='th03'?'#canvas':'body > canvas:last-of-type';
const replayRequests=[];
for(const key of ['ROOM_RUNTIME_ROOT','ROOM_RUNTIME_DATA'])assert(process.env[key],key+' is required');
const runtime=resolve(process.env.ROOM_RUNTIME_ROOT),out=resolve('.cache/native-room-retention',game);
await mkdir(out,{recursive:true});
const data=await readFile(process.env.ROOM_RUNTIME_DATA),fonts=new Map();
let profiledRoom;
if(profileNative){
 const source=await readFile(resolve(runtime,'managed-native-room.mjs'),'utf8');
 const factory="return globalThis.createTh03Classic({locateFile:name=>name.endsWith('.wasm')?'./native-classic.wasm':name});";
 assert(source.includes(factory),'native module profiling hook');
 profiledRoom=source.replace(factory,`const module=await globalThis.createTh03Classic({locateFile:name=>name.endsWith('.wasm')?'./native-classic.wasm':name});
 const profile=globalThis.th03NativeProfile={};
 for(const name of ['step_pair','capture','restore','audio_commit','pixels']){
  const key='_th03_classic_'+name, original=module[key];
  module[key]=(...args)=>{const start=performance.now();try{return original(...args);}finally{const values=profile[name]??=[];values.push(performance.now()-start);}};
 }
 return module;`);
}
let instrumentedRuntime=null;
if(process.env.PC98_TEST_CHALLENGE_EVENTS==='1'){
  assert.equal(game,'th04');
  const source=await readFile(resolve(runtime,'netplay/runtime.js'),'utf8');
  const marker='  try{emulator.step();}catch(error){challenge?.abort();throw error;}';
  assert(source.includes(marker),'challenge instrumentation boundary');
  const inject=`if(challengeLayout&&[180,360,540,720].includes(ticks)){
    const h=emulator.module.HEAPU8,v=new DataView(h.buffer),cb=mailbox-patch.mailbox_cs_offset,d=gameDataBase();
    if(ticks===540){emulator.module.thDirtyMark?.(d+0x4662,9);h[d+0x4662]=0;h[d+0x4669]=1;h[d+0x466a]=0;}
    else if(ticks===720){emulator.module.thDirtyMark?.(cb+patch.personal_stats.cs_offset,4);v.setUint32(cb+patch.personal_stats.cs_offset,3000000,true);}
    else{
      let at=-1;for(let i=0;i<32;i++){const q=d+0xaf34+i*20;if(h[q]===0){at=q;break;}}if(at<0)throw Error('test item slot');
      emulator.module.thDirtyMark?.(at,20);h.fill(0,at,at+20);h[at]=1;h[at+14]=ticks===180?4:5;
      const x=v.getInt16(d+0x464e,true),y=v.getInt16(d+0x4650,true);
      for(const k of [2,6])v.setInt16(at+k,x,true);for(const k of [4,8])v.setInt16(at+k,y,true);
    }
  }`;
  instrumentedRuntime=Buffer.from(source.replace(marker,inject+'\n'+marker));
}
let relayLog='',browser,page;
const clients=[],errors=[],relay=spawn(process.execPath,['services/relay/src/netplay-relay.mjs'],{
  cwd:resolve('..'),windowsHide:true,env:{...process.env,...(forceRelay?{EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK:'1'}:{}),EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:'',EAGLER_NETPLAY_TH09_SPECTATOR_TIMELINE:'1'},stdio:['ignore','pipe','pipe']});
relay.stdout.on('data',b=>relayLog+=b);relay.stderr.on('data',b=>relayLog+=b);
async function wait(check,label,timeout=90000){const until=Date.now()+timeout;while(Date.now()<until){if(await check())return;await new Promise(r=>setTimeout(r,30));}throw Error(label+' timed out\n'+relayLog);}
const html=`<!doctype html><meta charset="utf-8"><style>body{margin:0;background:#111;display:flex}iframe{width:640px;height:480px;border:0}</style><script>
window.events=[];window.__eaglerPrepareManagedRuntimeDataV1=async()=>{const buffer=await(await fetch('/data')).arrayBuffer();return{buffer,bytes:buffer.byteLength};};
addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1')events.push(e.data)});
window.addRuntime=n=>{const f=document.createElement('iframe');f.id='p'+n;f.src='/runtime/${game}.html?hosted=1&managedData=1&runtimeVariant=multiplayer&runtimeEpoch='+(n+1);document.body.append(f);};
window.rpc=(n,command,fields={})=>new Promise((ok,no)=>{const f=document.querySelector('#p'+n),request=crypto.randomUUID(),timer=setTimeout(()=>{removeEventListener('message',recv);no(Error(command+' timeout'));},90000);function recv(e){if(e.source===f.contentWindow&&e.data?.event==='exit'){clearTimeout(timer);removeEventListener('message',recv);no(Error('Native exit: '+e.data.status));return;}if(e.source===f.contentWindow&&e.data?.request===request){clearTimeout(timer);removeEventListener('message',recv);e.data.ok?ok(e.data):no(Error(e.data.error));}}addEventListener('message',recv);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'${game}',epoch:n+1,request,command,...fields},location.origin);});</script>`;
const server=createServer(async(req,res)=>{try{
  const path=new URL(req.url,'http://localhost').pathname;
  if(/\/api\/replay(?:\/|$)/.test(path))replayRequests.push(path);
  if(path==='/'){res.setHeader('content-type','text/html');res.end(html);return;}
  if(path==='/data'){res.end(data);return;}
  if(fonts.has(path)){res.end(fonts.get(path));return;}
  if(!path.startsWith('/runtime/'))throw Error('route');
  const file=resolve(runtime,path.slice(9));if(!file.startsWith(runtime+sep))throw Error('path');
  if(profiledRoom&&path==='/runtime/managed-native-room.mjs'){res.setHeader('content-type','text/javascript');res.end(profiledRoom);return;}
  res.setHeader('content-type',({'.html':'text/html','.mjs':'text/javascript','.js':'text/javascript','.wasm':'application/wasm','.json':'application/json','.css':'text/css'})[extname(file)]||'application/octet-stream');if(instrumentedRuntime&&path==='/runtime/netplay/runtime.js'){res.end(instrumentedRuntime);return;}
  if(process.env.PC98_TEST_LATE_FRAME==='1'&&path==='/runtime/netplay/managed-runtime.js'){const source=await readFile(file,'utf8');if(!source.includes('return sync;'))throw Error('test runtime hook missing');res.end(source.replace('return sync;','globalThis.testManagedRuntime=sync;return sync;'));return;}
  res.end(await readFile(file));
}catch{res.writeHead(404).end();}});
await new Promise(r=>server.listen(0,'127.0.0.1',r));
function send(client,message,predicate){return new Promise((ok,no)=>{const timer=setTimeout(()=>{cleanup();no(Error('Relay timeout '+JSON.stringify(message)));},15000);
  function receive(e){const m=JSON.parse(e.data);if(m.type==='error'){cleanup();no(Error(m.error));}else if(predicate(m)){cleanup();ok(m);}}
  function cleanup(){clearTimeout(timer);client.removeEventListener('message',receive);}
  client.addEventListener('message',receive);if(message)client.send(JSON.stringify(message));});}
const playerFrame=()=>{
  if(typeof globalThis.th03NativeFrame==='number')return globalThis.th03NativeFrame;
  if(typeof globalThis.__eaglerNetplayLanFrame==='number')return globalThis.__eaglerNetplayLanFrame;
  const c=globalThis.Module;return c?._th09_rollback_info?c.HEAPU32[(c._th09_rollback_info()>>>2)+1]:0;
};
try{
  await wait(()=>/listening ws:\/\/127.0.0.1:(\d+)/.test(relayLog),'Relay startup');
  const port=/listening ws:\/\/127.0.0.1:(\d+)/.exec(relayLog)[1],room=game+'mp-1234';
  for(let actor=0;actor<(lateJoin?2:3);actor++){
    const client=new WebSocket('ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,lobby:'retention_client_'+actor,member:'retention_member_'+actor,intent:actor?'join':'create',players:'2',difficulty:'1'}));clients.push(client);
    client.addEventListener('message',e=>{const message=JSON.parse(e.data);if(message.room)client.retentionRoom=message.room;});
    await send(client,null,m=>m.type==='state');
    await send(client,actor<2?{type:'take-seat',seat:actor,loadout:0}:{type:'spectate'},m=>actor<2?!!m.room?.seats[actor]:m.room?.spectators?.some(s=>s.clientId==='retention_client_2'));
  }
  browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
  page=await browser.newPage();page.on('pageerror',e=>errors.push(e.message));
  if(cpuThrottle!==1)await page.emulateCPUThrottling(cpuThrottle);
  if(forceRelay)await page.evaluateOnNewDocument(()=>Object.defineProperty(globalThis,'RTCPeerConnection',{value:undefined,configurable:true}));
  page.on('dialog',dialog=>{errors.push(dialog.message());void dialog.dismiss();});
  page.on('console',m=>console.log('Runtime:',m.text()));
  await page.setViewport({width:1920,height:540});await page.goto('http://127.0.0.1:'+server.address().port);
  await page.evaluate(()=>{for(let n=0;n<3;n++)addRuntime(n);});
  await page.waitForFunction(()=>events.filter(e=>e.event==='ready').length===3,{timeout:120000});
  const frames=()=>[1,2,3].map(epoch=>page.frames().find(f=>new URL(f.url()).searchParams.get('runtimeEpoch')===String(epoch)));
  for(let actor=0;actor<2;actor++)await send(clients[actor],{type:'set-ready',ready:true},m=>m.room?.seats[actor]?.ready);
  const start=await send(clients[0],{type:'start',inputDelay:0,adonisMode:timing.rollback?0:1,predictionLimit:timing.predictionLimit},m=>m.type==='start');
  let run=start.serial,roles=[0,1,null];
  const options=actor=>({touchEnabled:process.env.PC98_TOUCH_INPUT==='1',touchMovementMode:'touch-unlimited',touchSensitivity:150,netplayMode:'lan',netplayShowHitboxes:game==='th03'&&actor!==2,netplayChallengeMode:process.env.PC98_TEST_CHALLENGE==='1',netplayUrl:'ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,run:String(run),member:'retention_member_'+actor,...roles[actor]===null?{spectator:'retention_client_'+actor}:{player:String(roles[actor])}}),netplayPlayer:roles[actor]??0,netplayPlayerCount:2,netplaySeed:1234,netplayDifficulty:game==='th03'?3:1,netplayLoadouts:[{character:0,shot:0},{character:0,shot:1}],netplayInputDelay:0,netplayInputDelayAuto:false,netplayAdonisMode:timing.rollback?0:1,netplayPredictionLimit:timing.predictionLimit,netplaySpectator:roles[actor]===null,netplaySpectatorId:roles[actor]===null?'retention_client_'+actor:'',netplaySpectatorCount:1,netplaySpectatorTimeline:true});
  const configure=async(actor,serial)=>{
    await page.evaluate(async({actor,options})=>rpc(actor,'configure',{music:'none',options,resources:[],runtimeResources:[],sharedResources:[]}),{actor,options:options(actor)});
    // Give the running native title several ticks between protocol commands.
    // Configuration must not initialize or connect the upcoming run early.
    await new Promise(r=>setTimeout(r,500));
    await page.evaluate(({actor,serial})=>rpc(actor,'launch',{launchSerial:serial}),{actor,serial});
  };
  await Promise.all([configure(0,1),configure(1,1)]);
  if(game==='th03')for(let i=0;i<4;i++)for(const actor of [0,1]){
    await frames()[actor].click('#canvas');
    if(process.env.PC98_TOUCH_INPUT==='1'){
      await page.evaluate(()=>{let button=document.getElementById('focus-out');if(!button){button=document.createElement('button');button.id='focus-out';button.textContent='focus';document.body.append(button);}button.focus();});
      for(const type of ['down','up']){await page.evaluate(({actor,type})=>rpc(actor,'direct-touch',{type,id:1,x:.5,y:.5}),{actor,type});await new Promise(r=>setTimeout(r,250));}
    }else for(const down of [true,false]){await page.evaluate(({actor,down})=>rpc(actor,'keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down}),{actor,down});await new Promise(r=>setTimeout(r,250));}
  }
  if(game==='th04')await page.evaluate(()=>Promise.all([0,1].map(n=>rpc(n,'keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down:true}))));
  await wait(async()=>{const values=await Promise.all(frames().slice(0,2).map(f=>f.evaluate(playerFrame)));return values.every(v=>v>120);},'native players advancing');
  if(game!=='th03')assert.deepEqual(await Promise.all(frames().slice(0,2).map(f=>f.evaluate(game=>globalThis[game+'Replay']?.active,game))),[true,false],'only the host keeps the live viewer history');
  if(process.env.PC98_TEST_RTC_RESTART==='1')await verifyPc98IceRestart(game,frames().slice(0,2),playerFrame,wait);
  let spectatorStarted=false;
  // Exercise local input after a network delay has had time to fill the queue.
  if(game==='th03' && process.env.TH03_TEST_INPUT_LATENCY==='1'){
    await wait(async()=>(await Promise.all(frames().slice(0,2).map(f=>f.evaluate(()=>globalThis.th03SyncState?.phase===1)))).every(Boolean),'battle',30000);
    if(!lateJoin){
      await configure(2,1);spectatorStarted=true;
      await wait(async()=>await frames()[2].evaluate(playerFrame)>0,'spectator rendering during latency test',30000);
    }
    const delay=Number((forceRelay?process.env.EAGLER_NETPLAY_RELAY_DELAY_MS:process.env.TH03_TEST_NETWORK_DELAY_MS)??0);
    const jitter=Number((forceRelay?process.env.EAGLER_NETPLAY_RELAY_JITTER_MS:process.env.TH03_TEST_NETWORK_JITTER_MS)??0);
    const rawRtc=!forceRelay&&process.env.TH03_TEST_RTC_JITTER==='1';
    if(forceRelay)for(const frame of frames().slice(0,2))assert.equal(await frame.evaluate(()=>globalThis.__eaglerNetplayPath),'relay');
    if(!forceRelay)for(const [seat,frame] of frames().slice(0,2).entries())await frame.evaluate(({ms,jitter,rawRtc,seat})=>{
      const peer=[...globalThis.__th03PeerTransport.peers.values()][0];
      for(const channel of rawRtc?[peer.rtc.fast,peer.rtc.control]:[peer.channel]){
        const send=channel.send.bind(channel);
        let sequence=seat*11,lastDue=0;
        channel.send=message=>{const offset=jitter*Math.sin(sequence++*0.37),now=performance.now(),due=now+Math.max(0,ms+offset);lastDue=channel.ordered===false?due:Math.max(lastDue,due);setTimeout(()=>{if(channel.readyState==='open')send(message);},lastDue-now);};
      }
    },{ms:delay,jitter,rawRtc,seat});
    await new Promise(resolve=>setTimeout(resolve,1500));
    const samples=[];
    for(let repeat=0;repeat<5;repeat++)for(let seat=0;seat<2;seat++){
      const sample=await frames()[seat].evaluate(async ({seat,repeat})=>{
        const api=(await import('./managed-native-room.mjs')).managedAPI;
        const before=api.health(), x=before.players.find(player=>player.seat===seat)?.x;
        const key=repeat%2?'ArrowLeft':'ArrowRight', keyCode=repeat%2?37:39;
        const start=performance.now();
        window.__eaglerPC98Input.key({code:key,key,keyCode,down:true});
        return await new Promise(resolve=>{
          function check(){
            const current=api.health(), player=current.players.find(item=>item.seat===seat);
            if((player && player.x!==x) || performance.now()-start>2000){
              window.__eaglerPC98Input.key({code:key,key,keyCode,down:false});
              resolve({seat,ms:performance.now()-start,frames:current.gameFrame-before.gameFrame,moved:!!player && player.x!==x});
            }else requestAnimationFrame(check);
          }
          requestAnimationFrame(check);
        });
      },{seat,repeat});
      samples.push(sample);
      await new Promise(resolve=>setTimeout(resolve,150));
    }
    const start=await Promise.all(frames().slice(0,2).map(frame=>frame.evaluate(async()=>{
      const api=(await import('./managed-native-room.mjs')).managedAPI,health=api.health();
      if(globalThis.th03NativeProfile)for(const values of Object.values(globalThis.th03NativeProfile))values.length=0;
      const stats=globalThis.th03FrameTiming={last:performance.now(),lastProgress:performance.now(),frame:health.gameFrame,gaps:[],progressGaps:[],chargeGaps:[],releaseAt:-Infinity,running:true};
      let direction=false,charge=true;
      window.__eaglerPC98Input.key({code:'ShiftLeft',key:'Shift',keyCode:16,down:true});
      stats.controls=setInterval(()=>{direction=!direction;for(const [code,keyCode,down] of [['ArrowLeft',37,direction],['ArrowRight',39,!direction]])window.__eaglerPC98Input.key({code,key:code,keyCode,down});},400);
      stats.charge=setInterval(()=>{charge=!charge;if(!charge)stats.releaseAt=performance.now();window.__eaglerPC98Input.key({code:'ShiftLeft',key:'Shift',keyCode:16,down:charge});},2400);
      function measure(now){if(!stats.running)return;stats.gaps.push(now-stats.last);stats.last=now;const frame=api.health().gameFrame;if(frame!==stats.frame){const gap=now-stats.lastProgress;stats.progressGaps.push(gap);if(now>=stats.releaseAt&&stats.lastProgress<stats.releaseAt+500)stats.chargeGaps.push(gap);stats.lastProgress=now;stats.frame=frame;}requestAnimationFrame(measure);}
      requestAnimationFrame(measure);
      return {frame:health.gameFrame,draw:health.frames,rollbacks:health.rollbacks,at:performance.now()};
    })));
    await new Promise(resolve=>setTimeout(resolve,8000));
    const rates=await Promise.all(frames().slice(0,2).map((frame,index)=>frame.evaluate(async before=>{
      const health=(await import('./managed-native-room.mjs')).managedAPI.health();
      const seconds=(performance.now()-before.at)/1000;
      const stats=globalThis.th03FrameTiming;stats.running=false;clearInterval(stats.controls);clearInterval(stats.charge);
      for(const [code,key,keyCode] of [['ArrowLeft','ArrowLeft',37],['ArrowRight','ArrowRight',39],['ShiftLeft','Shift',16]])window.__eaglerPC98Input.key({code,key,keyCode,down:false});
      const summary=values=>{values.sort((a,b)=>a-b);return {count:values.length,p95:values[Math.floor(values.length*.95)]??0,p99:values[Math.floor(values.length*.99)]??0,max:values.at(-1)??0,over50:values.filter(value=>value>50).length};};
      const native=globalThis.th03NativeProfile&&Object.fromEntries(Object.entries(globalThis.th03NativeProfile).map(([name,values])=>[name,{calls:values.length,totalMs:values.reduce((sum,value)=>sum+value,0),...summary(values)}]));
      return {simulationFps:(health.gameFrame-before.frame)/seconds,drawFps:(health.frames-before.draw)/seconds,rollbacks:health.rollbacks-before.rollbacks,renderGapsMs:summary(stats.gaps),gameProgressGapsMs:summary(stats.progressGaps),chargeReleaseGapsMs:summary(stats.chargeGaps),native};
    },start[index])));
    console.log('TH03_INPUT_LATENCY',JSON.stringify({oneWayDelayMs:delay,jitterMs:jitter,rawRtc,forceRelay,samples,rates}));
    assert(samples.every(sample=>sample.moved),'local input did not move the player');
    assert(samples.every(sample=>sample.frames<=3),'local input was queued for more than three game frames');
    const pressEscape=async()=>{
      for(const down of [true,false]){
        await page.evaluate(down=>rpc(0,'keyboard',{code:'Escape',key:'Escape',keyCode:27,location:0,down}),down);
        await new Promise(resolve=>setTimeout(resolve,250));
      }
    };
    const paused=expected=>async()=> (await Promise.all(frames().slice(0,2).map(frame=>frame.evaluate(()=>globalThis.th03SyncState?.pauseSeat)))).every(seat=>seat===expected);
    await pressEscape();await wait(paused(0),'both players enter pause',15000);
    await pressEscape();await wait(paused(null),'both players resume',15000);
  }
  if(process.env.PC98_TEST_LATE_FRAME==='1'){
    await page.evaluate(()=>{globalThis.testStall=setInterval(()=>{const until=performance.now()+100;while(performance.now()<until){}},400);});
    await new Promise(resolve=>setTimeout(resolve,5000));await page.evaluate(()=>clearInterval(globalThis.testStall));
    for(const f of frames().slice(0,2)){
      const metrics=await f.evaluate(()=>globalThis.testManagedRuntime?.renderInfo()?.pageRpc??globalThis.testManagedRuntime?.performanceInfo?.()?.page);
      assert(metrics&&metrics.lateDisplayed>0,'late newest frames must still reach the canvas');
      assert.equal(metrics.expired,0,'latest frames must not be discarded merely because the UI stalled');
    }
  }
  if(process.env.PC98_TEST_CHALLENGE==='1'){
    await (await frames()[0].$('canvas')).screenshot({path:resolve(out,'challenge-native-hud.png')});
    for(const f of frames().slice(0,2)){
      const challenge=await f.evaluate(async()=>{const {managedAPI}=await import('./managed-native-room.mjs');return managedAPI.health().challenge;});
      assert.deepEqual(challenge,[0,0],'challenge starts each player at zero');
    }
    if(process.env.PC98_TEST_CHALLENGE_EVENTS==='1'){
      const expected=[{frame:240,value:[-1,0]},{frame:420,value:[-2,0]},{frame:650,value:[-1,0]},{frame:850,value:[-2,-1]}];
      for(const checkpoint of expected){
        await wait(async()=>await frames()[0].evaluate(playerFrame)>=checkpoint.frame,'challenge event '+checkpoint.frame,90000);
        for(const f of frames().slice(0,2)){const value=await f.evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health().challenge);assert.deepEqual(value,checkpoint.value,`challenge native event at ${checkpoint.frame}`);}
      }
      await (await frames()[0].$('canvas')).screenshot({path:resolve(out,'challenge-native-events.png')});
    }

  }

  if(game==='th03'&&process.env.PC98_TOUCH_INPUT==='1'){
    await wait(async()=>(await Promise.all(frames().slice(0,2).map(f=>f.evaluate(()=>globalThis.th03SyncState?.phase===1)))).every(Boolean),'touch selection enters gameplay',30000);
    const position=()=>frames()[1].evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health().players.find(p=>p.seat===1)?.x);
    const before=await position();assert.equal(typeof before,'number');
    await page.evaluate(()=>rpc(1,'direct-touch',{type:'down',id:42,x:.4,y:.5}));
    for(let i=1;i<=8;i++){await page.evaluate(i=>rpc(1,'direct-touch',{type:'move',id:42,x:.4+i*.015,y:.5}),i);await new Promise(r=>setTimeout(r,100));}
    await page.evaluate(()=>rpc(1,'direct-touch',{type:'up',id:42,x:.52,y:.5}));await new Promise(r=>setTimeout(r,500));
    assert.notEqual(await position(),before,'P2 drag moves the actual player');
    await page.evaluate(()=>rpc(1,'keyboard',{code:'Space',key:' ',keyCode:32,location:0,down:true}));
    await wait(async()=>frames()[1].evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health().players.some(p=>p.seat===1&&p.focused)),'native focus activated',20000);
    const marker=await frames()[1].evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health().players.find(p=>p.seat===1));
    await (await frames()[1].$('#canvas')).screenshot({path:resolve(out,'native-marker.png')});
    console.log('Native marker position',JSON.stringify(marker));
    const beforeFocus=await frames()[1].evaluate(playerFrame);
    await wait(async()=>(await frames()[1].evaluate(playerFrame))>=beforeFocus+300,'native marker keeps match synchronized',20000);
    await page.evaluate(()=>rpc(1,'keyboard',{code:'Space',key:' ',keyCode:32,location:0,down:false}));
  }
  if(game==='th01'){
    for(let press=0;press<6&&!await frames()[0].evaluate(()=>th01Net.session.sawGameplay);press++){
      for(const down of [true,false]){
        await page.evaluate(down=>rpc(0,'keyboard',{code:'Enter',key:'Enter',keyCode:13,location:0,down}),down);
        await new Promise(r=>setTimeout(r,250));
      }
    }
    await wait(async()=>await frames()[0].evaluate(()=>th01Net.session.sawGameplay),'TH01 host enters gameplay',10000);
  }
  if(game==='th04')await page.evaluate(()=>Promise.all([0,1].map(n=>rpc(n,'keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down:false}))));
  if((game==='th04')&&process.env.PC98_NATIVE_MARKER==='1'){
    await page.evaluate(()=>rpc(1,'keyboard',{code:'ShiftLeft',key:'Shift',keyCode:16,location:1,down:true}));
    const start=await frames()[1].evaluate(playerFrame);
    await wait(async()=>await frames()[1].evaluate(playerFrame)>=start+300,'native marker keeps cooperative match synchronized',30000);
    await (await frames()[1].frameElement()).screenshot({path:resolve(out,'native-marker.png')});
    await page.evaluate(()=>rpc(1,'keyboard',{code:'ShiftLeft',key:'Shift',keyCode:16,location:1,down:false}));
  }
  async function measure(label){const start=await Promise.all(frames().slice(0,2).map(f=>f.evaluate(playerFrame))),at=performance.now();await new Promise(r=>setTimeout(r,5000));const end=await Promise.all(frames().slice(0,2).map(f=>f.evaluate(playerFrame)));const seconds=(performance.now()-at)/1000;console.log('QA player rate',label,end.map((value,i)=>((value-start[i])/seconds).toFixed(1)));}
  if(process.env.PC98_SPECTATOR_PERF==='1')await measure('before spectator');
  if(process.env.PC98_TEST_LATE_STAGE==='1'){await wait(async()=>await frames()[0].evaluate(game=>globalThis[game+'Replay']?.publishedSegment>0,game),'published stage checkpoint',120000);await new Promise(resolve=>setTimeout(resolve,10000));}
  if(process.env.PC98_TEST_INPUT_ONLY_LATE==='1'){
    await wait(async()=>await frames()[0].evaluate(playerFrame)>=1800,'late input-only spectator join',120000);
    const points=await frames()[0].evaluate(game=>globalThis[game+'Replay']?.snapshot()?.points??[],game);
    assert.equal(points.length,0,'players must not create a replay simulation to build checkpoints');
  }
  if(lateJoin){
    const client=new WebSocket('ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,lobby:'retention_client_2',member:'retention_member_2',intent:'join',players:'2',difficulty:'1'}));clients.push(client);
    await send(client,null,m=>m.type==='state');
    await send(client,{type:'spectate'},m=>m.type==='spectator-start');
  }
  if(!spectatorStarted)await configure(2,1);
  await page.waitForFunction(()=>events.some(e=>e.epoch===3&&['first-frame','error'].includes(e.event)),{timeout:120000});const viewerError=await page.evaluate(()=>events.find(e=>e.epoch===3&&e.event==='error'));if(viewerError)throw Error(JSON.stringify(viewerError));
  await page.waitForFunction(()=>{const s=events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback;return s&&!s.seeking;},{timeout:90000});
  if(game==='th03'){
    await wait(async()=>(await Promise.all(frames().map(f=>f.evaluate(playerFrame)))).every(v=>v>420),'TH03 players and spectator enter battle');
    const nativeRules=await Promise.all(frames().map(f=>f.evaluate(async()=>{const api=(await import('./managed-native-room.mjs')).managedAPI;return api.health().roomRules;})));
    assert.deepEqual(nativeRules,[{difficulty:3,seed:1234},{difficulty:3,seed:1234},{difficulty:3,seed:1234}],'players and spectator receive the same room rules');
  }
  if(process.env.PC98_TEST_INPUT_ONLY_LATE==='1')await page.waitForFunction(()=>{const s=events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback;return s&&s.received>1800&&s.received-s.position<180;},{timeout:120000});
  if(process.env.PC98_TEST_LATE_STAGE==='1')assert((await page.evaluate(()=>events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback?.min))>0,'late spectator should start from a stage checkpoint');
  if((game==='th04')&&process.env.PC98_NATIVE_MARKER==='1')
    await page.evaluate(()=>rpc(1,'keyboard',{code:'ShiftLeft',key:'Shift',keyCode:16,location:1,down:true}));
  if(game==='th03'){await frames()[2].waitForFunction(async()=>(await import('./managed-native-room.mjs')).managedAPI.health().showHitboxes,{timeout:30000});assert.equal(await frames()[2].evaluate(async()=>(await import('./managed-native-room.mjs')).managedAPI.health().showHitboxes),true,'spectator follows the host rule even when its local option is false');}
  assert(await frames()[2].evaluate(game=>{const peer=globalThis['__'+game+'PeerTransport'];return peer?.relay?.readyState===WebSocket.OPEN&&peer.peers instanceof Map;},game),'launcher can observe the live spectator relay');
  await frames()[2].waitForFunction(selector=>{const c=document.querySelector(selector);if(!c)return false;const context=c.getContext('2d'),pixels=context.getImageData(0,0,c.width,c.height).data;for(let i=0;i<pixels.length;i+=4)if(pixels[i]||pixels[i+1]||pixels[i+2])return true;return false;},{timeout:90000},spectatorCanvas);
  if(process.env.PC98_SPECTATOR_PERF==='1'){
    await page.waitForFunction(()=>{const s=events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback;return s&&s.received-s.position<120;},{timeout:120000});
    await frames()[2].evaluate(selector=>{const c=document.querySelector(selector),ctx=c.getContext('2d'),draw=ctx.drawImage.bind(ctx);globalThis.__spectatorDraws=0;ctx.drawImage=(...args)=>{globalThis.__spectatorDraws++;return draw(...args);};},spectatorCanvas);
    const start=await frames()[2].evaluate(()=>globalThis.__spectatorDraws),position=await page.evaluate(()=>events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback?.position),at=performance.now();
    await measure('spectator active');const elapsed=(performance.now()-at)/1000,draws=await frames()[2].evaluate(()=>globalThis.__spectatorDraws),state=await page.evaluate(()=>events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback);
    console.log('QA spectator rate',JSON.stringify({drawFps:(draws-start)/elapsed,simulationFps:(state.position-position)/elapsed,elapsed,state}));
  }
  assert(await frames()[2].evaluate(selector=>{const c=document.querySelector(selector),r=c.getBoundingClientRect();return document.elementFromPoint(r.left+r.width/2,r.top+r.height/2)===c;},spectatorCanvas),'spectator canvas is visible above the room');
  if(game==='th01'){
    const read=()=>frames()[2].evaluate(selector=>{const c=document.querySelector(selector),p=c.getContext('2d').getImageData(0,0,c.width,c.height).data;let hash=2166136261;for(let i=0;i<p.length;i+=4)hash=Math.imul(hash^p[i]^p[i+1]<<8^p[i+2]<<16,16777619);return hash>>>0;},spectatorCanvas);
    const before=await read(),state=await page.evaluate(()=>events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback);
    await page.evaluate(()=>rpc(0,'keyboard',{code:'ArrowRight',key:'ArrowRight',keyCode:39,location:0,down:true}));
    await wait(async()=>await read()!==before,'TH01 spectator picture changes during play',15000);
    await page.evaluate(()=>rpc(0,'keyboard',{code:'ArrowRight',key:'ArrowRight',keyCode:39,location:0,down:false}));
    await wait(()=>page.evaluate(position=>events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback.position>position,state.position),'TH01 spectator advances during play',10000);
  }
  await page.waitForFunction(()=>events.some(e=>e.epoch===3&&e.spectatorPlayback?.position>30),{timeout:120000});
  const control=async(action,frame)=>page.evaluate(async({action,frame})=>{const run=events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback.run;await rpc(2,'spectator-playback',{run,action,...frame===undefined?{}:{frame}});},{action,frame});
  await control('pause');await page.waitForFunction(()=>{const s=events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback;return s?.paused&&!s.seeking;},{timeout:30000});
  const playerBefore=await Promise.all(frames().slice(0,2).map(f=>f.evaluate(playerFrame)));
  const pausedAt=await page.evaluate(()=>events.findLast(e=>e.epoch===3&&e.spectatorPlayback).spectatorPlayback.position);
  await wait(async()=>{const now=await Promise.all(frames().slice(0,2).map(f=>f.evaluate(playerFrame)));return now.every((value,i)=>value>=playerBefore[i]+90);},'players advance while spectator is paused',30000);
  assert.equal(await page.evaluate(()=>events.findLast(e=>e.epoch===3&&e.spectatorPlayback).spectatorPlayback.position),pausedAt,'spectator remains paused while players continue');
  const target=await page.evaluate(()=>Math.floor(events.findLast(e=>e.epoch===3&&e.spectatorPlayback).spectatorPlayback.position/2));
  await control('seek',target);await page.waitForFunction(target=>{const s=events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback;return s?.paused&&!s.seeking&&s.position===target;},{timeout:90000},target);
  await control('live');await page.waitForFunction(()=>{const s=events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback;return s?.following&&!s.paused&&!s.seeking;},{timeout:90000});
  if(process.env.PC98_TEST_INPUT_ONLY_LATE==='1')await page.waitForFunction(()=>{const s=events.findLast(e=>e.epoch===3&&e.spectatorPlayback)?.spectatorPlayback;return s&&s.received-s.position<180;},{timeout:120000});
  if((game==='th04')&&process.env.PC98_NATIVE_MARKER==='1'){
    const start=await frames()[1].evaluate(playerFrame);
    await wait(async()=>await frames()[1].evaluate(playerFrame)>=start+300,'marker stays synchronized while spectating',30000);
    await page.evaluate(()=>rpc(1,'keyboard',{code:'ShiftLeft',key:'Shift',keyCode:16,location:1,down:false}));
  }
  if((game==='th04')&&process.env.PC98_TEST_LATE_STAGE!=='1'&&process.env.PC98_TEST_INPUT_ONLY_LATE!=='1'){
    await page.click('#p0');await page.evaluate(()=>rpc(0,'keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down:true}));
    await page.waitForFunction(()=>events.some(e=>e.epoch===3&&e.event==='audio-health'&&e.effects>0&&e.effectsPeak>0.001),{timeout:20000});
    await page.evaluate(()=>rpc(0,'keyboard',{code:'KeyZ',key:'z',keyCode:90,location:0,down:false}));
  }
  if(game==='th03'&&process.env.TH03_TEST_ROOM_REPLAY==='1'){
    const press=async(code,key,keyCode)=>{
      for(const down of [true,false]){
        await page.evaluate(fields=>rpc(0,'keyboard',fields),{code,key,keyCode,down,location:0});
        await new Promise(resolve=>setTimeout(resolve,200));
      }
    };
    await press('Escape','Escape',27);
    await wait(async()=>await frames()[0].evaluate(()=>globalThis.th03SyncState?.pauseSeat===0),'host pauses the match',10000);
    await press('ArrowDown','ArrowDown',40);await press('ArrowDown','ArrowDown',40);await press('KeyZ','z',90);
    const playerFrames=frames().slice(0,2);
    const saved=async()=>(await Promise.all(playerFrames.map(f=>f.evaluate(()=>[...__eaglerPC98Files.memory.keys()].some(path=>path.endsWith('.th03r')))))).every(Boolean);
    await wait(async()=>{
      if(await saved())return true;
      // The result scene accepts dismissal after its animation has played.
      await press('KeyZ','z',90);
      return false;
    },'both players archive the completed native match',45000);
    const archives=await Promise.all(playerFrames.map(f=>f.evaluate(async()=>{
      const {decodeReplay}=await import('./native-replay.mjs');
      const [,bytes]=[...__eaglerPC98Files.memory].find(([path])=>path.endsWith('.th03r'));
      const {rows,native,seed,difficulty}=await decodeReplay(bytes);
      return {rows,nativeSize:native.length,seed,difficulty};
    })));
    assert(archives.every(a=>a.nativeSize>0&&a.rows.length>420&&a.seed===1234&&a.difficulty===3));
    assert.deepEqual(archives[0].rows,archives[1].rows,'both saved replays use the same confirmed inputs');
    console.log('TH03 archived confirmed native match',archives.map(a=>({frames:a.rows.length,nativeBytes:a.nativeSize})));
  }
  if(game!=='th03')assert.deepEqual(replayRequests,[],'managed rooms do not upload to the legacy replay endpoint');
  assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);
  console.log('PASS PC98 real managed players and spectator',JSON.stringify(await page.evaluate(()=>events.filter(e=>e.epoch===3&&e.spectatorPlayback).at(-1))));
}finally{if(page){console.log('QA errors',JSON.stringify(await page.evaluate(()=>events.filter(e=>e.event==='error'))));console.log('QA tail',JSON.stringify(await page.evaluate(()=>events.filter(e=>e.epoch===3).slice(-6))));console.log('QA relay',relayLog);console.log('QA game',JSON.stringify(await Promise.all(page.frames().filter(f=>f.url().includes('runtimeEpoch=')).map(f=>f.evaluate(()=>({state:globalThis.th03SyncState||null,replayError:globalThis.th03Replay?.error||globalThis.th04Replay?.error||'',replayEnd:(globalThis.th03Replay||globalThis.th04Replay)?.snapshot()?.end||null,recorderEnded:(globalThis.th03Replay||globalThis.th04Replay)?.ended??null,pointError:(globalThis.th03Replay||globalThis.th04Replay)?.pointError||'',replayPoints:(globalThis.th03Replay||globalThis.th04Replay)?.snapshot()?.points?.map(p=>({frame:p.frame,scene:p.scene}))||[]}))))));}for(const c of clients)c.close();await browser?.close();await new Promise(r=>server.close(r));relay.kill();}
