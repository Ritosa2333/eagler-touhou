// Original local assets, production WASM, Launcher RPC and the shared relay.
import assert from 'node:assert/strict';
import {readFile,readdir,mkdir,writeFile} from 'node:fs/promises';
import {createServer} from 'node:http';
import {spawn} from 'node:child_process';
import {createHash} from 'node:crypto';
import {resolve,extname,sep} from 'node:path';
import {launchBrowser} from '../../../tools/browser/launch.mjs';
import {DATA_SCHEMA,DATA_MAGIC,DATA_LAYOUT_SPEC} from '../src/managed-data.mjs';
const root=resolve(import.meta.dirname,'..'),repo=resolve(root,'../..'),assets=resolve(process.env.TH12_ASSETS??resolve(root,'../th12/site/assets'));
const fonts=resolve(process.env.TH12_FONTS??resolve(assets,'../shared'));
const difficulty=Number(process.env.TH12_MULTIPLAYER_DIFFICULTY??1),slowPresentation=process.env.TH12_MULTIPLAYER_SLOW_PRESENTATION==='1';
assert([0,1,2,3,4].includes(difficulty));
const inputLatency=Number(process.env.TH12_INPUT_LATENCY_MS??25);
const runtime=resolve(process.env.TH12_RUNTIME??resolve(root,'build-eagler-multiplayer')),out=resolve(root,'artifacts/sdl-multiplayer-browser');await mkdir(out,{recursive:true});
const sha=b=>createHash('sha256').update(b).digest('hex'),entries=[],chunks=[];let offset=0;
for(const name of (await readdir(assets)).sort())if(/^[a-zA-Z0-9_-]+\.(?:json|png|anm|ecl|std|sht|msg|wav|rpy)$/.test(name)){
 const b=await readFile(resolve(assets,name));entries.push({path:'assets/'+name,offset,bytes:b.length,sha256:sha(b)});chunks.push(b);offset+=b.length;
}
const manifest=Buffer.from(JSON.stringify({schema:DATA_SCHEMA,game:'th12',layout:'sha256-'+sha(JSON.stringify(DATA_LAYOUT_SPEC)),files:entries})),header=Buffer.alloc(12);header.write(DATA_MAGIC);header.writeUInt32LE(manifest.length,8);
const data=Buffer.concat([header,manifest,...chunks]);chunks.length=0;
const html=`<!doctype html><style>body{margin:0;background:#222}iframe{width:640px;height:480px;border:0}</style><script>
window.events=[];window.ready=new Set();let data;
window.__eaglerPrepareManagedRuntimeDataV1=async()=>{data??=fetch('/data').then(r=>r.arrayBuffer());const buffer=await data;return {buffer,bytes:buffer.byteLength};};
addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1'){events.push(e.data);if(e.data.event==='ready')ready.add(e.data.epoch);}});
window.addRuntime=s=>{const f=document.createElement('iframe');f.id='p'+s;f.src='/runtime/th12.html?hosted=1&managedData=1&runtimeVariant=multiplayer&runtimeEpoch='+(s+1);document.body.append(f);};
window.rpc=(s,command,fields={})=>new Promise((yes,no)=>{const f=document.querySelector('#p'+s),request=crypto.randomUUID(),timer=setTimeout(()=>no(Error(command+' timed out')),90000);function receive(e){if(e.source===f.contentWindow&&e.data?.request===request){clearTimeout(timer);removeEventListener('message',receive);e.data.ok?yes(e.data):no(Error(e.data.error));}}addEventListener('message',receive);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th12',epoch:s+1,command,request,...fields},location.origin);});
</script>`;
const server=createServer(async(req,res)=>{try{const url=new URL(req.url,'http://localhost');if(url.pathname==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}if(url.pathname==='/data'){res.end(data);return;}
 if(url.pathname==='/favicon.ico'){res.writeHead(204).end();return;}
 if(url.pathname.startsWith('/shared/')){res.end(await readFile(resolve(fonts,url.pathname.split('/').at(-1))));return;}
 const p=resolve(runtime,decodeURIComponent(url.pathname).replace(/^\/runtime\//,''));if(!p.startsWith(runtime+sep))throw Error('invalid path');res.setHeader('Content-Type',({'.mjs':'text/javascript','.js':'text/javascript','.html':'text/html','.css':'text/css','.json':'application/json','.wasm':'application/wasm'})[extname(p)]??'application/octet-stream');res.end(await readFile(p));
 }catch(e){res.writeHead(404).end(e.message);}});await new Promise(r=>server.listen(0,'127.0.0.1',r));
let browser,relayLog='';const sockets=[];
const relay=spawn(process.execPath,['services/relay/src/netplay-relay.mjs'],{cwd:repo,windowsHide:true,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:'',EAGLER_NETPLAY_RELAY_DELAY_MS:'25',EAGLER_NETPLAY_RELAY_JITTER_MS:'0'},stdio:['ignore','pipe','pipe']});
relay.stdout.on('data',b=>relayLog+=b);relay.stderr.on('data',b=>relayLog+=b);
async function wait(fn,label,ms=90000){const end=Date.now()+ms;while(Date.now()<end){if(await fn())return;await new Promise(r=>setTimeout(r,50));}throw Error(label+' timeout\n'+relayLog.slice(-6000));}
function send(ws,message,predicate){return new Promise((yes,no)=>{const timer=setTimeout(()=>{cleanup();no(Error('relay message timeout'));},15000);function cleanup(){clearTimeout(timer);ws.removeEventListener('message',receive);}function receive(e){const m=JSON.parse(e.data);if(m.type==='error'){cleanup();no(Error(m.error));}else if(predicate(m)){cleanup();yes(m);}}ws.addEventListener('message',receive);if(message)ws.send(JSON.stringify(message));});}
try{
 await wait(()=>/listening ws:\/\/127.0.0.1:(\d+)/.test(relayLog),'relay');const port=Number(/listening ws:\/\/127.0.0.1:(\d+)/.exec(relayLog)[1]);
 browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
 const reports=[];
 const scenarios=process.env.TH12_MULTIPLAYER_SCENARIOS?JSON.parse(process.env.TH12_MULTIPLAYER_SCENARIOS):[[2,1,0,false],[3,1,3,false],[2,0,0,false],[3,0,0,false],[2,1,0,true]];
 for(const [count,mode,delay,automatic]of scenarios){
  const context=await browser.newContext({viewport:{width:count*640,height:1040}}),page=await context.newPage(),errors=[],requests=[];
  page.on('pageerror',e=>errors.push(e.message));
  await context.addInitScript(delay=>{if(!delay)return;const send=RTCDataChannel.prototype.send;RTCDataChannel.prototype.send=function(data){const copy=typeof data==='string'?data:data.slice(0),channel=this;setTimeout(()=>{if(channel.readyState==='open')send.call(channel,copy);},delay);};},inputLatency);page.on('request',r=>requests.push(r.url()));
  page.on('console',m=>{if(m.type()==='error')console.error('BROWSER',m.text());});

  const room='th12mp-'+(2000+reports.length),members=[];
  for(let s=0;s<count;++s){const ws=new WebSocket('ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,lobby:room+'-public-'+s,member:room+'-private-'+s,intent:s?'join':'create',players:String(count),difficulty:String(difficulty)}));sockets.push(ws);members.push(ws);await send(ws,null,m=>m.type==='state');await send(ws,{type:'take-seat',seat:s,loadout:s===0?1:s===1?3:5},m=>!!m.room?.seats[s]);}
  await page.goto('http://127.0.0.1:'+server.address().port);await page.evaluate(n=>{for(let s=0;s<n;s++)addRuntime(s);},count+(count===2?1:0));await page.waitForFunction(n=>ready.size===n,count+(count===2?1:0),{timeout:90000});
  await page.evaluate(async c=>{for(let s=0;s<c.count+(c.count===2?1:0);s++){await rpc(s,'configure',{music:'none',language:'ja',sharedResources:[{path:'/unifont.otf',url:'/shared/unifont.otf'},{path:'/msgothic.ttc',url:'/shared/msgothic.ttc'}],options:{thpracEnabled:false,netplayDifficulty:c.difficulty}});await rpc(s,'prepare-room');}},{count,difficulty});
  const frames=()=>page.frames().filter(f=>f.url().includes('runtimeEpoch=')).slice(0,count);
  for(const f of frames())await f.evaluate(()=>{const r=window.__th12Runtime;window.preloadedCore=r.core;window.samples={};const tick=r.multiplayer.tick;r.multiplayer.tick=(...args)=>{const out=tick(...args),s=r.status();if(s[2]&&s[3]&&s[3]<5000)window.samples[s[3]]=r.core._th12_mp_hash();return out;};});
  assert((await Promise.all(frames().map(f=>f.evaluate(()=>__th12Runtime.scene())))).every(s=>s==='title'));
  if(reports.length===0){
    const before=await Promise.all(frames().map(f=>f.evaluate(()=>__th12Runtime.menu().age)));
    await new Promise(r=>setTimeout(r,3000));
    const after=await Promise.all(frames().map(f=>f.evaluate(()=>__th12Runtime.menu())));
    for(const [s,menu] of after.entries()){
      assert.equal(menu.scene,'title','waiting beyond the attract-mode deadline must stay at the title');
      assert(menu.age>before[s]+100,'room title keeps animating rather than freezing its clock');
    }
    assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);
    console.log('ROOM_IDLE_TITLE_PASS',after);
  }
  await page.screenshot({path:resolve(out,`title-${count}-${mode}-${delay}-${automatic}.png`)});
  for(let s=0;s<count;++s)await send(members[s],{type:'set-ready',ready:true},m=>m.room?.seats[s]?.ready);
  const start=await send(members[0],{type:'start',adonisMode:mode,inputDelay:delay,inputDelayAuto:automatic,predictionReserve:2},m=>m.type==='start');const requestsAtStart=requests.length;
  await page.evaluate(async cfg=>{await Promise.all(Array.from({length:cfg.count},async(_,s)=>{await rpc(s,'configure',{music:'none',language:'ja',options:{thpracEnabled:false,multiplayerLocalPlayerVisibility:true,netplayMode:'lan',netplayUrl:'ws://127.0.0.1:'+cfg.port+'/?'+new URLSearchParams({room:cfg.room,run:String(cfg.serial),member:cfg.room+'-private-'+s,player:String(s)}),netplayPlayer:s,netplayPlayerCount:cfg.count,netplayDifficulty:cfg.difficulty,netplaySeed:1234,netplayLoadouts:[{character:0,shot:1},{character:1,shot:1},{character:2,shot:1}].slice(0,cfg.count),netplayInputDelay:cfg.delay,netplayInputDelayAuto:cfg.automatic,netplayAdonisMode:cfg.mode,netplayPredictionLimit:cfg.mode===1?0:12,netplayPredictionReserve:2}});await rpc(s,'launch',{launchSerial:1});}));},{count,mode,delay,automatic,difficulty,port,room,serial:start.serial});
  console.log('LAUNCHED',{count,mode,delay,automatic});let diagnosticsAt=0;
  await wait(async()=>{const e=await page.evaluate(()=>events.filter(e=>e.event==='error'));assert.deepEqual(e,[]);const status=await Promise.all(frames().map(f=>f.evaluate(()=>({status:__th12Runtime.status(),scene:__th12Runtime.scene(),world:__th12Runtime.state().frame}))));if(Date.now()-diagnosticsAt>10000){diagnosticsAt=Date.now();console.log('FRONTIERS',JSON.stringify(status));}return status.every(n=>n.status[3]>=180);},'first native frames');
  for(const f of frames())await f.evaluate(async()=>{
    window.dispatchEvent(new KeyboardEvent('keydown',{code:'ShiftLeft',key:'Shift',shiftKey:true}));
    await new Promise(r=>setTimeout(r,150));
    window.__eaglerKeyboardMapping={schema:'eagler-touhou/keyboard-input/1',protocol:'eagler-touhou/1',game:'th12',epoch:Number(new URL(location.href).searchParams.get('runtimeEpoch')),origin:location.origin,map:(event,down)=>[{code:event.code,down}]};
    window.dispatchEvent(new KeyboardEvent('keyup',{code:'ShiftLeft',key:'Shift',shiftKey:false}));
    delete window.__eaglerKeyboardMapping;
  });
  await wait(async()=>{const states=await Promise.all(frames().map(f=>f.evaluate(()=>__th12Runtime.state())));return states.every(s=>s.players.every(p=>!(p.held&8)));},'released keyboard Shift after mapping owner attachment');
  const paceStart=Date.now(),paceFrames=await Promise.all(frames().map(f=>f.evaluate(()=>__th12Runtime.status()[3])));
  for(let round=0;round<4;++round){await page.evaluate(async round=>{await rpc(round%2,'keyboard',{code:'ArrowRight',down:true,inputSource:'physical'});await rpc(1-round%2,'keyboard',{code:'KeyZ',down:true,inputSource:'physical'});},round);await new Promise(r=>setTimeout(r,250));await page.evaluate(async round=>{await rpc(round%2,'keyboard',{code:'ArrowRight',down:false,inputSource:'physical'});await rpc(1-round%2,'keyboard',{code:'KeyZ',down:false,inputSource:'physical'});},round);}
  await page.evaluate(()=>rpc(0,'keyboard',{code:'Escape',down:true,inputSource:'physical'}));
  await wait(async()=>{const states=await Promise.all(frames().map(f=>f.evaluate(()=>({s:__th12Runtime.state(),n:__th12Runtime.status(),hash:__th12Runtime.core._th12_mp_hash()}))));assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);return states.every(p=>p.s.paused&&p.n[4]!==0xffffffff&&p.n[4]>=p.n[3]-1)&&states.every(p=>p.s.frame===states[0].s.frame);},'confirmed shared pause');
  await page.evaluate(()=>rpc(0,'keyboard',{code:'Escape',down:false,inputSource:'physical'}));await new Promise(r=>setTimeout(r,500));
  const paused=await Promise.all(frames().map(f=>f.evaluate(()=>({world:__th12Runtime.state(),status:__th12Runtime.status(),hash:__th12Runtime.core._th12_mp_hash(),sameCore:preloadedCore===__th12Runtime.core,samples:window.samples,rollback:globalThis.__eaglerNetplayLanRollback??0}))));
  if(mode===0&&inputLatency>16)assert(paused.some(p=>p.rollback>0),'delayed real RTC input must exercise rollback');
  assert(paused.every(p=>p.hash===paused[0].hash),'shared pause must have the same authoritative world');assert(paused.every(p=>p.sameCore),'Start reuses preloaded WASM');
  let exactMatches=0;if(mode===1)for(const [f,h]of Object.entries(paused[0].samples))if(paused.every(p=>p.samples[f]!==undefined)){assert(paused.every(p=>p.samples[f]===h),'exact world at '+f);exactMatches++;}if(mode===1)assert(exactMatches>=50);
  assert(!requests.slice(requestsAtStart).some(u=>/\/(data|core\.wasm)$/.test(u)),'Start must reuse game DATA and core');
  await page.screenshot({path:resolve(out,`pause-${count}-${mode}-${delay}-${automatic}.png`)});
  let spectatorVerified=false;
  if(count===2&&!automatic){
    const spectatorId=room+'-observer',member=room+'-observer-private';
    const lobby=new WebSocket('ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,lobby:spectatorId,member,intent:'join',players:String(count)}));sockets.push(lobby);
    await send(lobby,null,m=>m.type==='state');await send(lobby,{type:'spectate'},m=>m.room?.spectators.some(s=>s.clientId===spectatorId));
    // The observer installs assets in the room preview, then joins the live confirmed stream here.
    await page.evaluate(async c=>{await rpc(c.count,'configure',{music:'none',language:'ja',sharedResources:[{path:'/unifont.otf',url:'/shared/unifont.otf'},{path:'/msgothic.ttc',url:'/shared/msgothic.ttc'}],options:{thpracEnabled:false,netplayMode:'lan',netplayUrl:'ws://127.0.0.1:'+c.port+'/?'+new URLSearchParams({room:c.room,run:String(c.serial),member:c.member}),netplayPlayer:0,netplayPlayerCount:c.count,netplayDifficulty:c.difficulty,netplaySeed:1234,netplayLoadouts:[{character:0,shot:1},{character:1,shot:1}],netplayInputDelay:c.delay,netplayInputDelayAuto:false,netplayAdonisMode:c.mode,netplayPredictionLimit:c.mode===1?0:12,netplayPredictionReserve:2,netplaySpectator:true,netplaySpectatorId:c.spectatorId}});await rpc(c.count,'launch',{launchSerial:1});},{count,mode,delay,difficulty,port,room,serial:start.serial,member,spectatorId});
    const observer=page.frames().find(f=>f.url().includes('runtimeEpoch='+(count+1))),report=()=>page.evaluate(epoch=>events.findLast(e=>e.epoch===epoch&&e.spectatorPlayback)?.spectatorPlayback,count+1);
    await wait(async()=>{assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);const s=await report();return s&&s.edge>=paused[0].status[3];},'native spectator bootstrap');
    const view=await report();await page.evaluate(c=>rpc(c.s,'spectator-playback',{run:c.run,action:'pause'}),{s:count,run:view.run});
    for(const target of [40,paused[0].status[3]]){
      await page.evaluate(c=>rpc(c.s,'spectator-playback',{run:c.run,action:'seek',frame:c.target}),{s:count,run:view.run,target});
      await wait(async()=>{const s=await report();assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);return s?.position===target&&!s.seeking;},'native spectator seek '+target);
    }
    assert.equal(await observer.evaluate(()=>__th12Runtime.core._th12_mp_hash()),paused[0].hash,'spectator reconstructs the same authoritative world');
    await page.locator('#p'+count).screenshot({path:resolve(out,`spectator-${mode}.png`)});
    await page.evaluate(s=>rpc(s,'return-room'),count);lobby.close();spectatorVerified=true;
  }
  if(count===2&&mode===1&&!automatic){
    const press=async code=>{await page.evaluate(c=>rpc(0,'keyboard',{code:c,down:true,inputSource:'physical'}),code);await new Promise(r=>setTimeout(r,160));await page.evaluate(c=>rpc(0,'keyboard',{code:c,down:false,inputSource:'physical'}),code);await new Promise(r=>setTimeout(r,120));};
    await press('ArrowDown');await press('ArrowDown');await press('Enter');
    await wait(async()=>{const states=await Promise.all(frames().slice(0,count).map(f=>f.evaluate(()=>__th12Runtime.state())));return states.every(s=>s.menuConfirm&&s.menuSelection===1&&!s.finished);},'shared original Yes/No');
    await press('ArrowUp');await press('Enter');
    await wait(async()=>{const menus=await Promise.all(frames().slice(0,count).map(f=>f.evaluate(()=>__th12Runtime.menu())));return menus.every(s=>s.scene==='replay-save'&&s.ready);},'confirmed native Replay Save');
    await press('Enter');
    await wait(()=>frames()[0].evaluate(()=>__th12Runtime.menu().scene==='name'&&__th12Runtime.menu().ready),'original Replay name ready');
    await press('Enter');
    await wait(async()=>{const listing=await page.evaluate(()=>rpc(0,'list'));return listing.files.some(f=>f.path.endsWith('/th12_mp_01.rpy')&&f.size>176);},'original numbered Replay slot and name');
    await page.locator('#p0').screenshot({path:resolve(out,'replay-save.png')});
  }
  await page.evaluate(n=>Promise.all(Array.from({length:n},(_,s)=>rpc(s,'return-room'))),count);
  const files=await page.evaluate(()=>rpc(0,'list')),replay=files.files.find(f=>f.path.includes('th12_mp_'));assert(replay?.size>176,'confirmed Replay saved');
  await page.evaluate(()=>rpc(0,'configure',{music:'none',language:'ja',options:{replayViewer:true,thpracEnabled:false}}));
  const description=await page.evaluate(path=>rpc(0,'playback',{action:'inspect',path}),replay.path);assert.equal(description.description.mode,'multiplayer');
  await page.evaluate(path=>rpc(0,'playback',{action:'open',path}),replay.path);
  await page.evaluate(()=>rpc(0,'playback',{action:'pause'}));
  const replayFrame=frames()[0];await page.evaluate(value=>rpc(0,'playback',{action:'seek',value}),paused[0].status[3]);
  await wait(()=>replayFrame.evaluate(target=>__th12Runtime.status()[3]===target,paused[0].status[3]),'Replay reconstruction');
  assert.equal(await replayFrame.evaluate(()=>__th12Runtime.core._th12_mp_hash()),paused[0].hash,'Replay reconstructs the confirmed live world including pause');
  assert.deepEqual(errors,[]);assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);
  reports.push({count,mode,delay,automatic,difficulty,restores:paused.map(p=>p.rollback),slowPresentation,frames:paused[0].status[3],exactMatches,hash:paused[0].hash,spectatorVerified});console.log('PASS',reports.at(-1));
  members.forEach(ws=>ws.close());await context.close();
 }
 await writeFile(resolve(out,'result.json'),JSON.stringify({scope:'Original local TH12 assets, production WASM, browser Launcher RPC and ordered WebSocket relay with 25ms delay',reports},null,2));
}catch(error){console.error("TEST_FAILURE",error);throw error;}finally{sockets.forEach(ws=>ws.close());await browser?.close();relay.kill();server.closeAllConnections();await new Promise(r=>server.close(r));}
