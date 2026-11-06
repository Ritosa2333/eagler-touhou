import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import {launchBrowser} from '../../../../../tools/browser/launch.mjs';

const repo=resolve(import.meta.dirname,'../../../../..');
const runtime=resolve(process.env.EAGLER_TH17_BUILD||resolve(repo,'ports/th17/build-eagler-multiplayer'));
const out=resolve(process.env.TH17_ROOM_EVIDENCE||resolve(repo,'artifacts/th17-room'));
const rollback=!process.argv.includes('--lockstep');
const extended=process.argv.includes('--extended');
const minimumSamples=Number(process.env.TH17_ROOM_SAMPLES||1050);
const extendedTarget=Number(process.env.TH17_ROOM_TICKS||7200);
const manifest=JSON.parse(await readFile(resolve(runtime,'manifest.json'),'utf8'));
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
 await rpc(seat,'configure',{music:'none',language:'ja',sharedResources:['/msgothic.ttc','/unifont.otf'].map(path=>({path,url:path})),options});
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
 browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--disable-background-timer-throttling','--disable-renderer-backgrounding','--autoplay-policy=no-user-gesture-required']});
 for(const seats of extended?[2]:[2,3]){
  const room='th17mp-room-test-'+seats,loadouts=Array.from({length:seats},(_,i)=>({character:i,shot:i}));
  const members=[];
  for(let seat=0;seat<seats;seat++){
   const member=room+'-private-'+seat;
   const socket=new WebSocket(endpoint+new URLSearchParams({room,lobby:room+'-'+seat,member,intent:seat?'join':'create',players:String(seats),difficulty:'1'}));
   sockets.push(socket);members.push(socket);
   await lobby(socket,null,value=>!!value.room);
   await lobby(socket,{type:'take-seat',seat,loadout:seat*3+seat},value=>!!value.room?.seats[seat]);
  }
  for(const [seat,socket] of members.entries())
   await lobby(socket,{type:'set-ready',ready:true},value=>value.room?.seats[seat]?.ready);
  const timing={adonisMode:rollback?0:1,inputDelay:rollback?0:2,inputDelayAuto:false};
  const start=await lobby(members[0],{type:'start',...timing},value=>value.type==='start');
  options=Array.from({length:seats},(_,seat)=>({
   netplayMode:'lan',netplayAdonisMode:timing.adonisMode,netplayInputDelay:timing.inputDelay,netplayAllowWebsocketFallback:true,
   netplayPlayer:seat,netplayPlayerCount:seats,netplaySeed:1234,netplayDifficulty:1,netplayLoadouts:loadouts,
   renderScale:1,netplayUrl:endpoint+new URLSearchParams({room,run:String(start.serial),member:room+'-private-'+seat}),
  }));
  const context=await browser.newContext({viewport:{width:seats*640,height:480}});
  if(rollback)await context.addInitScript(()=>{
   const send=RTCDataChannel.prototype.send;
   RTCDataChannel.prototype.send=function(payload){
    if(!globalThis.delayInput||typeof payload==='string')return send.call(this,payload);
    // Delay the guest's binary samples and repairs after the RTC handshake.
    // Otherwise a repaired input can arrive before the delayed sample.
    const copy=payload.slice();
    setTimeout(()=>{if(this.readyState==='open')send.call(this,copy);},110);
   };
  });
  const page=await context.newPage(),errors=[];
  page.on('pageerror',error=>errors.push(error.message));
  try{
   await page.goto('http://127.0.0.1:'+server.address().port);
   for(let seat=0;seat<seats;seat++){
    await page.evaluate(seat=>addRuntime(seat),seat);
    await page.waitForFunction(epoch=>ready.has(epoch),seat+1,{timeout:45000});
   }
   await page.evaluate(seats=>Promise.all(Array.from({length:seats},(_,seat)=>startRuntime(seat))),seats);
   const frames=page.frames().filter(frame=>frame.url().includes('/runtime/'));
   for(const [seat,frame] of frames.entries()){
    await frame.evaluate(({seat,rollback,extended})=>{
     const callback=Module.onGameFrame;
     globalThis.samples=new Map();
     globalThis.diagnostics={corrections:0,resimulated:0,maxPrediction:0,combatCorrections:0};
     let recorded=0;
     Module.onGameFrame=(ok,time,ticks)=>{
      callback(ok,time,ticks);
      const r=Module._th17_room_rollback()>>>2;
      const [n,committed,corrections,resimulated,combatCorrections,spellCorrections]=Module.HEAPU32.subarray(r,r+6);
      diagnostics.corrections=corrections;diagnostics.resimulated=resimulated;
      diagnostics.combatCorrections=combatCorrections;diagnostics.spellCorrections=spellCorrections;
      diagnostics.maxPrediction=Math.max(diagnostics.maxPrediction,n-committed);
      globalThis.delayInput=rollback&&seat===1&&n>=115;
      for(let number=Math.max(recorded+1,committed-15);number<=committed;number++){
       const p=Module._th17_room_confirmed(number)>>>2,words=Array.from(Module.HEAPU32.subarray(p,p+38));
       words[2]=0;
       if(words[0]===number&&number>120&&words[3]===1)samples.set(number,words);
      }
      recorded=committed;
      diagnostics.lastConfirmed=committed;
      const centerGuest=extended&&seat===1&&n>=900&&n<952;
      const moveGuest=seat===1&&n>=600&&!centerGuest;
      Module._th17_key(205,+(seat===1&&n>=120&&n<170||moveGuest&&n%96<12||extended&&seat===0&&n>=300&&n<350));
      Module._th17_key(203,+(seat===0&&n>=120&&n<170||moveGuest&&n%96>=48&&n%96<60||centerGuest));
      Module._th17_key(200,+(seat===2&&n>=120&&n<150));
      Module._th17_key(44,+(n>=140&&n<260||extended&&n>=900));
      Module._th17_key(46,+(extended&&seat===0&&n>=1400&&n%120<100));
      Module._th17_key(45,+(seat===1&&n>=185&&n<193||extended&&n>=1800&&(n+seat*420)%840<9));
     };
    },{seat,rollback,extended});
   }
   let frontier=0,progressAt=Date.now();
   await wait(async()=>{
    assert.deepEqual(errors,[]);
    const failures=await page.evaluate(()=>events.filter(e=>e.event==='error'));
    assert.deepEqual(failures,[]);
    const status=await Promise.all(frames.map(frame=>frame.evaluate(()=>({samples:samples.size,diagnostics}))));
    const current=Math.min(...status.map(item=>item.diagnostics.lastConfirmed||0));
    if(current>frontier){
     if(extended&&Math.floor(current/1000)>Math.floor(frontier/1000))console.log('extended room confirmed through '+current);
     frontier=current;progressAt=Date.now();
    }else if(Date.now()-progressAt>20000)throw Error('Room stopped confirming input: '+JSON.stringify(status));
    return status.every(item=>item.samples>=minimumSamples)&&(!extended||!rollback||
     status.some(item=>item.diagnostics.spellCorrections>0)||current>=extendedTarget);
   },'independent pilot motion through Stage 1',extended?220000:90000);
   const states=await Promise.all(frames.map(frame=>frame.evaluate(()=>Array.from(samples))));
   const first=new Map(states[0]),other=states.slice(1).map(state=>new Map(state));
   const agreed=[...first].filter(([frame])=>other.every(map=>map.has(frame)));
   assert.ok(agreed.length>100);
   let difference;
   for(const [frame,value] of agreed){
    const peer=other.find(map=>map.get(frame).some((word,index)=>word!==value[index]));
    if(peer){difference={frame,host:value,peer:peer.get(frame),previous:agreed.find(([n])=>n===frame-1)?.[1]};break;}
   }
   const [,initial]=agreed[0];
   const [,last]=agreed.at(-1);
   const diagnostics=await Promise.all(frames.map(frame=>frame.evaluate(()=>globalThis.diagnostics)));
   const maxEnemyBullets=Math.max(...agreed.map(([,state])=>state[10]));
   const maxLasers=Math.max(...agreed.map(([,state])=>state[29]));
   const activeSpellFrames=agreed.filter(([,state])=>state[30]&1).length;
   const dialogueFrames=agreed.filter(([,state])=>state[33]).length;
   reports.push({room,seats,comparedFrames:agreed.length,maxEnemyBullets,maxLasers,activeSpellFrames,dialogueFrames,diagnostics,last,difference});
   if(difference)assert.deepEqual(difference.peer,difference.host,'world at frame '+difference.frame);
   if(rollback){
    assert.ok(diagnostics.some(item=>item.corrections>0&&item.resimulated>0),'late input triggers correction and replay');
    assert.ok(diagnostics.some(item=>item.combatCorrections>0),'at least one correction happens while enemy projectiles are present');
    assert.ok(diagnostics.every(item=>item.maxPrediction<=12));
   }
   assert.ok(maxEnemyBullets>0,'fight includes enemy projectiles');
   assert.equal(initial[19],4,'Otter starts with four bombs');
   assert.ok(agreed.some(([,state])=>state[19]===3&&state[15]===initial[15]),'guest spends their own bomb');
   assert.ok(agreed.some(([,state])=>(state[12]|0)<(state[16]|0)),'left and right belong to different pilots');
   if(seats===3)assert.ok(agreed.some(([,state])=>state[21]<state[13]),'third pilot moved up independently');
   await page.screenshot({path:resolve(out,seats+'p.png')});
   console.log(seats+' players, '+agreed.length+' matching confirmed frames, '+diagnostics.map(item=>item.corrections).join('/')+' corrections');
  }finally{await context.close();for(const socket of members)socket.close();}
 }
}finally{
 await browser?.close();for(const socket of sockets)socket.close();relay.kill();server.closeAllConnections();server.close();
 await writeFile(resolve(out,'room-report.json'),JSON.stringify({rollback,extended,version:manifest.version,wasm:manifest.execution.sha256,
  injectedDelayMs:rollback?110:0,reports},null,2)+'\n');
 await writeFile(resolve(out,'relay.log'),log);
}
