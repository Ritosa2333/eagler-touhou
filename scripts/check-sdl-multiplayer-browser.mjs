// Original local assets, production WASM, Launcher RPC and the shared relay.
import assert from 'node:assert/strict';
import {readFile,readdir,mkdir,writeFile} from 'node:fs/promises';
import {createServer} from 'node:http';
import {spawn} from 'node:child_process';
import {createHash} from 'node:crypto';
import {resolve,extname,sep} from 'node:path';
import {launchBrowser} from '../../../tools/browser/launch.mjs';
import {installPcmProbe,samplePcm,installSfxStartProbe} from '../../../tools/browser/audio-output-probe.mjs';
import {verifyRuntimeDelivery} from '../../../tools/browser/verified-runtime.mjs';
import {DATA_SCHEMA,DATA_MAGIC,DATA_LAYOUT_SPEC} from '../src/managed-data.mjs';
const root=resolve(import.meta.dirname,'..'),repo=resolve(root,'../..'),assets=resolve(process.env.TH12_ASSETS??resolve(root,'../th12/site/assets'));
const fonts=resolve(process.env.TH12_FONTS??resolve(assets,'../shared'));
const music=process.env.TH12_TEST_MUSIC==='ogg'?'ogg':'none',audioChecks=[];
const musicResources=music==='ogg'?['th12_00','th12_01','th12_02'].map(track=>({path:'/music/'+track+'.ogg',url:'/music/'+track+'.ogg'})):[];
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
const program=await readFile(resolve(runtime,'game.mjs')),wasm=await readFile(resolve(runtime,'core.wasm'));
const runtimeIdentity={entry:sha(await readFile(resolve(runtime,'th12.html'))),program:sha(program),module:sha(await readFile(resolve(runtime,'core.mjs'))),wasm:sha(wasm)};
const html=`<!doctype html><style>body{margin:0;background:#222}iframe{width:640px;height:480px;border:0}</style><script>
window.events=[];window.ready=new Set();let data;
window.__eaglerPrepareManagedRuntimeDataV1=async()=>{data??=fetch('/data').then(r=>r.arrayBuffer());const buffer=await data;return {buffer,bytes:buffer.byteLength};};
addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1'){events.push(e.data);if(e.data.event==='ready')ready.add(e.data.epoch);}});
window.addRuntime=s=>{const f=document.createElement('iframe');f.id='p'+s;f.src='/runtime/th12.html?hosted=1&managedData=1&runtimeVariant=multiplayer&runtimeEpoch='+(s+1);document.body.append(f);};
window.rpc=(s,command,fields={})=>new Promise((yes,no)=>{const f=document.querySelector('#p'+s),request=crypto.randomUUID(),timer=setTimeout(()=>no(Error(command+' timed out')),90000);function receive(e){if(e.source===f.contentWindow&&e.data?.request===request){clearTimeout(timer);removeEventListener('message',receive);e.data.ok?yes(e.data):no(Error(e.data.error));}}addEventListener('message',receive);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th12',epoch:s+1,command,request,...fields},location.origin);});
</script>`;
const server=createServer(async(req,res)=>{try{const url=new URL(req.url,'http://localhost');if(url.pathname==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}if(url.pathname==='/data'){res.end(data);return;}
 if(url.pathname==='/favicon.ico'){res.writeHead(204).end();return;}
 if(/^\/music\/th12_\d\d\.ogg$/.test(url.pathname)){res.end(await readFile(resolve(assets,'music',url.pathname.split('/').at(-1))));return;}
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
  const delivery=await verifyRuntimeDelivery(page,{scriptUrl:'http://127.0.0.1:'+server.address().port+'/runtime/game.mjs',scriptBytes:program,wasmName:'core.wasm',wasmBytes:wasm});
  page.on('pageerror',e=>errors.push(e.message));page.on('response',r=>{if(r.status()>=400)errors.push('HTTP '+r.status()+' '+r.url());});await context.addInitScript(installPcmProbe);await context.addInitScript(installSfxStartProbe);
  if(music==='ogg'&&reports.length===0)await page.route('**/music/th12_01.ogg',route=>route.fulfill({status:200,contentType:'audio/ogg',body:Buffer.from('invalid OGG for decoder regression')}));
  await page.route('**/game.css',route=>route.abort());
  await context.addInitScript(delay=>{if(!delay)return;const send=RTCDataChannel.prototype.send;RTCDataChannel.prototype.send=function(data){const copy=typeof data==='string'?data:data.slice(0),channel=this;setTimeout(()=>{if(channel.readyState==='open')send.call(channel,copy);},delay);};},inputLatency);page.on('request',r=>requests.push(r.url()));
  page.on('console',m=>{if(m.type()==='error')errors.push(m.text());});

  const room='th12mp-'+(2000+reports.length),members=[];
  for(let s=0;s<count;++s){const ws=new WebSocket('ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,lobby:room+'-public-'+s,member:room+'-private-'+s,intent:s?'join':'create',players:String(count),difficulty:String(difficulty)}));sockets.push(ws);members.push(ws);await send(ws,null,m=>m.type==='state');await send(ws,{type:'take-seat',seat:s,loadout:s===0?1:s===1?3:5},m=>!!m.room?.seats[s]);}
  await page.goto('http://127.0.0.1:'+server.address().port);await page.evaluate(n=>{for(let s=0;s<n;s++)addRuntime(s);},count+(count===2?1:0));await page.waitForFunction(n=>ready.size===n,count+(count===2?1:0),{timeout:90000});
  await page.evaluate(async c=>{for(let s=0;s<c.count+(c.count===2?1:0);s++){await rpc(s,'configure',{music:c.music,language:'ja',sharedResources:[{path:'/unifont.otf',url:'/shared/unifont.otf'},{path:'/msgothic.ttc',url:'/shared/msgothic.ttc'},...c.musicResources],options:{thpracEnabled:false,netplayDifficulty:c.difficulty}});await rpc(s,'prepare-room');}},{count,difficulty,music,musicResources});
  const frames=()=>page.frames().filter(f=>f.url().includes('runtimeEpoch=')).slice(0,count);
  if(music==='ogg'&&reports.length===0){
  await page.waitForFunction(n=>events.filter(e=>e.event==='error').length>=n,count+(count===2?1:0),{timeout:90000});
  await page.waitForTimeout(200);
  const failures=await page.evaluate(()=>events.filter(e=>e.event==='error'));
  assert.equal(failures.length,count+(count===2?1:0),'one recoverable error per Runtime');
  assert(failures.every(e=>e.code==='audio-resource'&&e.error.includes('th12_01')));
  const silent=await samplePcm(frames()[0]);assert(silent.frames>0&&silent.peak<1e-6);
  await page.unroute('**/music/th12_01.ogg');
  await page.evaluate(n=>Promise.all(Array.from({length:n},(_,s)=>rpc(s,'resources',{resources:[{path:'/music/th12_01.ogg',url:'/music/th12_01.ogg'}]}))),count+(count===2?1:0));
  const recovered=await samplePcm(frames()[0]);assert(recovered.frames>0&&recovered.peak>1e-4,'corrected music resumes in the same Runtime');
  audioChecks.push({action:'corrupt-resource-recovery',failures,silent,recovered});
  await page.evaluate(()=>{window.events=events.filter(e=>e.event!=='error');});
  }
  for(const f of page.frames().filter(f=>f.url().includes('runtimeEpoch='))){assert.equal(await f.locator('.toolbar').isVisible(),false);assert.equal(await f.locator('footer').isVisible(),false);assert.equal(await f.evaluate(()=>getComputedStyle(document.documentElement).backgroundColor),'rgb(8, 9, 12)');}
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
    // The room title permits navigation and Options but never exits the host.
    for(const code of ['Escape','Escape','Enter','KeyZ','ArrowDown']){
      console.log('ROOM_PRESS',code);
      await page.evaluate(async code=>{await rpc(0,'keyboard',{code,down:true,inputSource:'physical'});},code);
      await new Promise(r=>setTimeout(r,90));
      await page.evaluate(async code=>{await rpc(0,'keyboard',{code,down:false,inputSource:'physical'});},code);
      await new Promise(r=>setTimeout(r,500));
    }
    assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[],'room input must not terminate the native application');
    assert.equal(await frames()[0].evaluate(()=>__th12Runtime.menu().scene),'title');
    assert.equal(await frames()[0].evaluate(()=>__th12Runtime.menu().selection),2,'room navigation remains available (Extra is locked)');
    for(let n=0;n<4;++n){
      await page.evaluate(()=>rpc(0,'keyboard',{code:'ArrowDown',down:true,inputSource:'physical'}));await new Promise(r=>setTimeout(r,90));
      await page.evaluate(()=>rpc(0,'keyboard',{code:'ArrowDown',down:false,inputSource:'physical'}));await new Promise(r=>setTimeout(r,90));
    }
    assert.equal(await frames()[0].evaluate(()=>__th12Runtime.menu().selection),6);
    await page.evaluate(()=>rpc(0,'keyboard',{code:'Enter',down:true,inputSource:'physical'}));await new Promise(r=>setTimeout(r,90));
    await page.evaluate(()=>rpc(0,'keyboard',{code:'Enter',down:false,inputSource:'physical'}));
    await wait(()=>frames()[0].evaluate(()=>__th12Runtime.menu().scene==='options'),'room Options');console.log('ROOM_OPTIONS_ENTERED');
    await new Promise(r=>setTimeout(r,250));
    await page.evaluate(()=>rpc(0,'keyboard',{code:'Escape',down:true,inputSource:'physical'}));await new Promise(r=>setTimeout(r,90));
    await page.evaluate(()=>rpc(0,'keyboard',{code:'Escape',down:false,inputSource:'physical'}));
    await page.evaluate(()=>rpc(0,'keyboard',{code:'Enter',down:true,inputSource:'physical'}));await new Promise(r=>setTimeout(r,90));
    await page.evaluate(()=>rpc(0,'keyboard',{code:'Enter',down:false,inputSource:'physical'}));
    await wait(()=>frames()[0].evaluate(()=>__th12Runtime.menu().scene==='title'),'return from room Options');
    const effects=await frames()[0].evaluate(()=>__sfxStarts);assert(effects.some(s=>s.when===0&&s.peak>1e-4&&s.channels===2),'native room menu effects reach immediate Web Audio output');
    audioChecks.push({count,mode,action:'native-immediate-effects',starts:effects});
    console.log('ROOM_OPTIONS_NAVIGATION_PASS');
  }
  await page.screenshot({path:resolve(out,`title-${count}-${mode}-${delay}-${automatic}.png`)});
  for(let s=0;s<count;++s)await send(members[s],{type:'set-ready',ready:true},m=>m.room?.seats[s]?.ready);
  const start=await send(members[0],{type:'start',adonisMode:mode,inputDelay:delay,inputDelayAuto:automatic,predictionReserve:2},m=>m.type==='start');const requestsAtStart=requests.length;
  await page.evaluate(async cfg=>{await Promise.all(Array.from({length:cfg.count},async(_,s)=>{await rpc(s,'configure',{music:cfg.music,language:'ja',options:{thpracEnabled:false,touchEnabled:true,touchMovementMode:'touch-unlimited',touchSensitivity:100,multiplayerLocalPlayerVisibility:true,netplayMode:'lan',netplayUrl:'ws://127.0.0.1:'+cfg.port+'/?'+new URLSearchParams({room:cfg.room,run:String(cfg.serial),member:cfg.room+'-private-'+s,player:String(s)}),netplayPlayer:s,netplayPlayerCount:cfg.count,netplayDifficulty:cfg.difficulty,netplaySeed:1234,netplayLoadouts:[{character:0,shot:1},{character:1,shot:1},{character:2,shot:1}].slice(0,cfg.count),netplayInputDelay:cfg.delay,netplayInputDelayAuto:cfg.automatic,netplayAdonisMode:cfg.mode,netplayPredictionLimit:cfg.mode===1?0:12,netplayPredictionReserve:2}});await rpc(s,'launch',{launchSerial:1});}));},{count,mode,delay,automatic,difficulty,port,room,serial:start.serial,music});
  console.log('LAUNCHED',{count,mode,delay,automatic});let diagnosticsAt=0;
  await wait(async()=>{const e=await page.evaluate(()=>events.filter(e=>e.event==='error'));assert.deepEqual(e,[]);const status=await Promise.all(frames().map(f=>f.evaluate(()=>({status:__th12Runtime.status(),scene:__th12Runtime.scene(),world:__th12Runtime.state().frame}))));if(Date.now()-diagnosticsAt>10000){diagnosticsAt=Date.now();console.log('FRONTIERS',JSON.stringify(status));}return status.every(n=>n.status[3]>=180);},'first native frames');
  assert.equal(await page.evaluate(count=>events.filter(e=>e.event==='first-frame'&&e.epoch<=count&&e.launchSerial===1).length,count),count,'Launcher must accept first-frame for this room launch');
  for(const f of frames())await f.evaluate(async()=>{
    window.dispatchEvent(new KeyboardEvent('keydown',{code:'ShiftLeft',key:'Shift',shiftKey:true}));
    await new Promise(r=>setTimeout(r,150));
    window.__eaglerKeyboardMapping={schema:'eagler-touhou/keyboard-input/1',protocol:'eagler-touhou/1',game:'th12',epoch:Number(new URL(location.href).searchParams.get('runtimeEpoch')),origin:location.origin,map:(event,down)=>[{code:event.code,down}]};
    window.dispatchEvent(new KeyboardEvent('keyup',{code:'ShiftLeft',key:'Shift',shiftKey:false}));
    delete window.__eaglerKeyboardMapping;
  });
  await wait(async()=>{const states=await Promise.all(frames().map(f=>f.evaluate(()=>__th12Runtime.state())));return states.every(s=>s.players.every(p=>!(p.held&8)));},'released keyboard Shift after mapping owner attachment');
  await page.evaluate(()=>rpc(0,'keyboard',{code:'ArrowRight',down:true,inputSource:'physical'}));
  await wait(async()=>(await Promise.all(frames().map(f=>f.evaluate(()=>__th12Runtime.state().players[0].held)))).every(held=>held&128),'held movement before pagehide');
  const beforeHide=await frames()[1].evaluate(()=>__th12Runtime.status()[3]);
  await frames()[0].evaluate(()=>{dispatchEvent(new PageTransitionEvent('pagehide',{persisted:true}));dispatchEvent(new PageTransitionEvent('pageshow',{persisted:true}));});
  await wait(async()=>(await Promise.all(frames().map(f=>f.evaluate(()=>({s:__th12Runtime.state(),frame:__th12Runtime.status()[3]}))))).every(({s,frame})=>!(s.players[0].held&128)&&!s.paused&&frame>beforeHide+10),'pagehide releases input while both peers continue');
  console.log('MP_PAGEHIDE_INPUT_RELEASE_PASS');
  const paceStart=Date.now(),paceFrames=await Promise.all(frames().map(f=>f.evaluate(()=>__th12Runtime.status()[3])));
  for(let round=0;round<4;++round){await page.evaluate(async round=>{await rpc(round%2,'keyboard',{code:'ArrowRight',down:true,inputSource:'physical'});await rpc(1-round%2,'keyboard',{code:'KeyZ',down:true,inputSource:'physical'});},round);await new Promise(r=>setTimeout(r,250));await page.evaluate(async round=>{await rpc(round%2,'keyboard',{code:'ArrowRight',down:false,inputSource:'physical'});await rpc(1-round%2,'keyboard',{code:'KeyZ',down:false,inputSource:'physical'});},round);}
  await page.evaluate(()=>rpc(0,'touch-controls',{fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:1,joystickX:0,joystickY:0}));
  await wait(async()=>{const states=await Promise.all(frames().map(f=>f.evaluate(()=>({s:__th12Runtime.state(),n:__th12Runtime.status(),hash:__th12Runtime.core._th12_mp_hash()}))));assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);return states.every(p=>p.s.paused&&p.n[4]!==0xffffffff&&p.n[4]>=p.n[3]-1)&&states.every(p=>p.s.frame===states[0].s.frame);},'confirmed shared pause');
  await page.evaluate(()=>rpc(0,'keyboard',{code:'Escape',down:false,inputSource:'physical'}));await new Promise(r=>setTimeout(r,500));
  const paused=await Promise.all(frames().map(f=>f.evaluate(()=>({world:__th12Runtime.state(),status:__th12Runtime.status(),hash:__th12Runtime.core._th12_mp_hash(),sameCore:preloadedCore===__th12Runtime.core,samples:window.samples,rollback:globalThis.__eaglerNetplayLanRollback??0}))));
  if(mode===0&&inputLatency>16)assert(paused.some(p=>p.rollback>0),'delayed real RTC input must exercise rollback');
  assert(paused.every(p=>p.hash===paused[0].hash),'shared pause must have the same authoritative world');assert(paused.every(p=>p.sameCore),'Start reuses preloaded WASM');
  let exactMatches=0;if(mode===1)for(const [f,h]of Object.entries(paused[0].samples))if(paused.every(p=>p.samples[f]!==undefined)){assert(paused.every(p=>p.samples[f]===h),'exact world at '+f);exactMatches++;}if(mode===1)assert(exactMatches>=50);
  assert(!requests.slice(requestsAtStart).some(u=>/\/(data|core\.wasm)$/.test(u)),'Start must reuse game DATA and core');
  const capturedAtPause=await Promise.all(frames().map(f=>f.evaluate(()=>JSON.parse(__th12Runtime.core.UTF8ToString(__th12Runtime.core._th12_host_state())).captureSerial)));
  await new Promise(r=>setTimeout(r,1100));
  const capturedLater=await Promise.all(frames().map(f=>f.evaluate(()=>JSON.parse(__th12Runtime.core.UTF8ToString(__th12Runtime.core._th12_host_state())).captureSerial)));
  assert.deepEqual(capturedLater,capturedAtPause,'paused multiplayer redraws must reuse the captured world');
  console.log('PAUSED_CAPTURE_REUSE_PASS',{capturedAtPause,capturedLater});
  await page.screenshot({path:resolve(out,`pause-${count}-${mode}-${delay}-${automatic}.png`)});
  let spectatorVerified=false;const roomReturns=[];
  if(count===2&&!automatic){
   const observer=page.frames().find(f=>f.url().includes('runtimeEpoch='+(count+1)));
   await observer.evaluate(()=>{window.roomLoopCore=__th12Runtime.core;});
   for(let round=0;round<3;++round){
    const requestStart=requests.length,eventStart=await page.evaluate(()=>events.length);
    const spectatorId=room+'-observer-'+round,member=room+'-observer-private-'+round;
    const lobby=new WebSocket('ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,lobby:spectatorId,member,intent:'join',players:String(count)}));sockets.push(lobby);
    await send(lobby,null,m=>m.type==='state');await send(lobby,{type:'spectate'},m=>m.room?.spectators.some(s=>s.clientId===spectatorId));
    // The observer installs assets in the room preview, then joins the live confirmed stream here.
    await page.evaluate(async c=>{await rpc(c.count,'configure',{music:c.music,language:'ja',sharedResources:[{path:'/unifont.otf',url:'/shared/unifont.otf'},{path:'/msgothic.ttc',url:'/shared/msgothic.ttc'}],options:{thpracEnabled:false,netplayMode:'lan',netplayUrl:'ws://127.0.0.1:'+c.port+'/?'+new URLSearchParams({room:c.room,run:String(c.serial),member:c.member}),netplayPlayer:0,netplayPlayerCount:c.count,netplayDifficulty:c.difficulty,netplaySeed:1234,netplayLoadouts:[{character:0,shot:1},{character:1,shot:1}],netplayInputDelay:c.delay,netplayInputDelayAuto:false,netplayAdonisMode:c.mode,netplayPredictionLimit:c.mode===1?0:12,netplayPredictionReserve:2,netplaySpectator:true,netplaySpectatorId:c.spectatorId}});await rpc(c.count,'launch',{launchSerial:c.round+2});},{count,mode,delay,difficulty,port,room,serial:start.serial,member,spectatorId,music,round});
    const report=()=>page.evaluate(({epoch,start})=>events.slice(start).findLast(e=>e.epoch===epoch&&e.spectatorPlayback)?.spectatorPlayback,{epoch:count+1,start:eventStart});
    await wait(async()=>{assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);const s=await report();return s&&s.edge>=paused[0].status[3];},'native spectator bootstrap');
    const view=await report();await page.evaluate(c=>rpc(c.s,'spectator-playback',{run:c.run,action:'pause'}),{s:count,run:view.run});
    for(const target of [40,paused[0].status[3]]){
      await page.evaluate(c=>rpc(c.s,'spectator-playback',{run:c.run,action:'seek',frame:c.target}),{s:count,run:view.run,target});
      await wait(async()=>{const s=await report();assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);return s?.position===target&&!s.seeking;},'native spectator seek '+target);
      if(music==='ogg'&&target===40){
        let pcm=await samplePcm(observer);audioChecks.push({count,mode,action:'spectator-seek',...pcm});assert(pcm.frames>0&&pcm.peak<1e-6,'paused spectator seek is silent');
        await page.evaluate(c=>rpc(c.s,'spectator-playback',{run:c.run,action:'play'}),{s:count,run:view.run});
        pcm=await samplePcm(observer);audioChecks.push({count,mode,action:'spectator-play',...pcm});assert(pcm.frames>0&&pcm.peak>1e-4,'spectator music resumes after seek');
        await page.evaluate(c=>rpc(c.s,'spectator-playback',{run:c.run,action:'pause'}),{s:count,run:view.run});
        pcm=await samplePcm(observer);audioChecks.push({count,mode,action:'spectator-pause',...pcm});assert(pcm.frames>0&&pcm.peak<1e-6,'spectator pause stops actual output');
      }
    }
    assert.equal(await observer.evaluate(()=>__th12Runtime.core._th12_mp_hash()),paused[0].hash,'spectator reconstructs the same authoritative world');
    await page.locator('#p'+count).screenshot({path:resolve(out,`spectator-${mode}-${round}.png`)});
    const returned=await page.evaluate(s=>rpc(s,'return-room'),count);assert.equal(returned.presented,true);
    const menu=await observer.evaluate(()=>({sameCore:__th12Runtime.core===roomLoopCore,...__th12Runtime.menu()}));
    assert(menu.sameCore&&menu.scene==='title','return keeps the loaded core and presents the native title');
    await page.waitForTimeout(500);assert((await observer.evaluate(()=>__th12Runtime.menu().age))>menu.age,'returned title keeps animating');
    assert(!requests.slice(requestStart).some(u=>/\/(data|core\.wasm)$/.test(u)),'spectator reentry reuses DATA and Wasm');
    roomReturns.push({round,launchSerial:round+2,...menu});lobby.close();spectatorVerified=true;
   }
  }
  if(count===2&&mode===1&&!automatic){
    const worlds=()=>Promise.all(frames().map(f=>f.evaluate(()=>__th12Runtime.state())));
    const press=async code=>{
      const bit={ArrowDown:32,ArrowUp:16,Enter:0x1000}[code];
      await page.evaluate(c=>rpc(0,'keyboard',{code:c,down:true,inputSource:'physical'}),code);
      await page.waitForTimeout(160);
      await wait(async()=>(await worlds()).every(s=>s.finished||(s.players[0].held&bit)!==0),'all seats consume '+code);
      await page.evaluate(c=>rpc(0,'keyboard',{code:c,down:false,inputSource:'physical'}),code);
      await wait(async()=>(await worlds()).every(s=>s.finished||(s.players[0].held&bit)===0),'all seats release '+code);
      await page.waitForTimeout(120);
    };
    await press('ArrowDown');await press('ArrowDown');await press('Enter');
    await wait(async()=>{const states=await Promise.all(frames().slice(0,count).map(f=>f.evaluate(()=>__th12Runtime.state())));return states.every(s=>s.menuConfirm&&s.menuSelection===1&&!s.finished&&!(s.players[0].held&0x30f3));},'shared original Yes/No');
    await press('ArrowUp');await wait(async()=>(await worlds()).every(s=>s.menuConfirm&&s.menuSelection===0&&!s.finished),'all seats select Yes');await press('Enter');
    await wait(async()=>{const menus=await Promise.all(frames().slice(0,count).map(f=>f.evaluate(()=>__th12Runtime.menu())));return menus.every(s=>s.scene==='replay-save'&&s.ready);},'confirmed native Replay Save');
    await press('Enter');
    await wait(()=>frames()[0].evaluate(()=>__th12Runtime.menu().scene==='name'&&__th12Runtime.menu().ready),'original Replay name ready');
    await press('Enter');
    await wait(async()=>{const listing=await page.evaluate(()=>rpc(0,'list'));return listing.files.some(f=>f.path.endsWith('/th12_mp_01.rpy')&&f.size>176);},'original numbered Replay slot and name');
    await page.locator('#p0').screenshot({path:resolve(out,'replay-save.png')});
  }
  await page.evaluate(n=>Promise.all(Array.from({length:n},(_,s)=>rpc(s,'return-room'))),count);
  const files=await page.evaluate(()=>rpc(0,'list')),replay=files.files.find(f=>f.path.includes('th12_mp_'));assert(replay?.size>176,'confirmed Replay saved');
  await page.evaluate(music=>rpc(0,'configure',{music,language:'ja',options:{replayViewer:true,thpracEnabled:false}}),music);
  const description=await page.evaluate(path=>rpc(0,'playback',{action:'inspect',path}),replay.path);assert.equal(description.description.mode,'multiplayer');
  await page.evaluate(path=>rpc(0,'playback',{action:'open',path}),replay.path);
  await page.evaluate(()=>rpc(0,'playback',{action:'pause'}));
  const replayFrame=frames()[0];await page.evaluate(value=>rpc(0,'playback',{action:'seek',value}),paused[0].status[3]);
  await wait(()=>replayFrame.evaluate(target=>__th12Runtime.status()[3]===target,paused[0].status[3]),'Replay reconstruction');
  assert.equal(await replayFrame.evaluate(()=>__th12Runtime.core._th12_mp_hash()),paused[0].hash,'Replay reconstructs the confirmed live world including pause');
  await delivery.assertComplete();assert.deepEqual(errors,[]);assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='error')),[]);
  reports.push({count,mode,delay,automatic,difficulty,restores:paused.map(p=>p.rollback),slowPresentation,frames:paused[0].status[3],exactMatches,hash:paused[0].hash,spectatorVerified,roomReturns,delivery:delivery.receipts});console.log('PASS',reports.at(-1));
  members.forEach(ws=>ws.close());await context.close();
 }
 await writeFile(resolve(out,'result.json'),JSON.stringify({scope:'Original local th12 assets, production WASM, browser Launcher RPC, real RTC players and Relay spectators',inputLatency,music,audioChecks,runtime:runtimeIdentity,dataSha256:sha(data),reports},null,2));
}catch(error){
 const failure=[];
 for(const context of browser?.contexts()??[])for(const page of context.pages()){
   for(const frame of page.frames())if(frame.url().includes('runtimeEpoch='))try{
     failure.push(await frame.evaluate(()=>({url:location.href,host:JSON.parse(__th12Runtime.core.UTF8ToString(__th12Runtime.core._th12_host_state())),world:__th12Runtime.state(),native:__th12Runtime.status(),menu:__th12Runtime.menu()})));
   }catch{}
   try{await page.screenshot({path:resolve(out,'failure.png')});}catch{}
 }
 await writeFile(resolve(out,'failure.json'),JSON.stringify({error:String(error),failure},null,2));
 console.error("TEST_FAILURE",error);throw error;
}finally{sockets.forEach(ws=>ws.close());await browser?.close();relay.kill();server.closeAllConnections();await new Promise(r=>server.close(r));}
