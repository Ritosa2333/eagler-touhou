import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {writeFile,mkdir} from 'node:fs/promises';
import {resolve} from 'node:path';
import {WebSocket} from 'ws';
import {launchBrowser} from '../../../tools/browser/launch.mjs';
import {multiplayerFixture} from './multiplayer-fixture.mjs';
const repo=resolve(import.meta.dirname,'../../..'),out=resolve(process.env.TH05_TEST_OUTPUT??resolve(repo,'ports/th05-mp/artifacts/multiplayer-browser'));await mkdir(out,{recursive:true});
const count=Number(process.env.TH05_MP_PLAYERS??2),route=process.env.TH05_MP_ROUTE??'rtc',mode=Number(process.env.TH05_MP_MODE??1),jitter=process.env.TH05_MP_JITTER==='1';
let log='',port;const sockets=[];
const relay=spawn(process.execPath,['services/relay/src/netplay-relay.mjs'],{cwd:repo,windowsHide:true,stdio:['ignore','pipe','pipe'],env:{...process.env,EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK:'1',EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_PUBLIC_NICKNAMES:'0',EAGLER_NETPLAY_STUN_URLS:'',EAGLER_NETPLAY_RTC_TIMEOUT_MS:'30000'}});
relay.stdout.on('data',b=>{log+=b;const m=log.match(/listening ws:\/\/127\.0\.0\.1:(\d+)/);if(m)port=Number(m[1]);});relay.stderr.on('data',b=>log+=b);
const wait=async(check,label)=>{const end=Date.now()+15000;while(!check()){if(Date.now()>end)throw Error(label+' timeout\n'+log);await new Promise(r=>setTimeout(r,10));}};
const roomId='th05mp-native-'+Date.now().toString(36),generation='a'.repeat(64),members=Array.from({length:count},(_,i)=>'native_member_'+i+'_'+Date.now().toString(36));
async function lobby(seat){const ws=new WebSocket(`ws://127.0.0.1:${port}/?${new URLSearchParams({room:roomId,lobby:'native_client_'+seat,member:members[seat],players:String(count),difficulty:'1'})}`),messages=[];sockets.push(ws);ws.on('message',b=>messages.push(JSON.parse(String(b))));await wait(()=>messages.some(m=>m.type==='state'),'lobby');return {messages,send:m=>ws.send(JSON.stringify(m)),room:()=>messages.findLast(m=>m.room)?.room};}
let browser,site;const report={count,route,mode,jitter};
try{
 await wait(()=>port,'relay startup');const clients=[];
 for(let seat=0;seat<count;seat++){const client=await lobby(seat);clients.push(client);client.send({type:'take-seat',seat,loadout:seat,name:'Native '+seat});await wait(()=>client.room()?.seats[seat],'seat');if(seat===0){client.send({type:'settings',playerCount:count,difficulty:1});await wait(()=>client.room()?.playerCount===count,'capacity');}}
 for(const client of clients)client.send({type:'set-ready',ready:true,runtimeGeneration:generation,movementMode:'touch'});
 await wait(()=>clients[0].room()?.seats.slice(0,count).every(s=>s?.ready),'ready');clients[0].send({type:'start',adonisMode:mode,inputDelay:0,inputDelayAuto:mode!==0,predictionLimit:8});
 await wait(()=>clients[0].messages.some(m=>m.type==='start'),'run');const serial=clients[0].messages.find(m=>m.type==='start').serial;
 site=await multiplayerFixture(count);browser=await launchBrowser();const page=await browser.newPage({viewport:{width:1320,height:1020}}),errors=[];
 if(route==='relay')await page.addInitScript(()=>{delete window.RTCPeerConnection;});
 page.on('pageerror',e=>errors.push(e.message));await page.goto(site.url);await page.waitForFunction(count=>events.filter(e=>e.event==='ready').length===count,count,{timeout:30000});
 const configure=async seat=>{const opts={netplayMode:'lan',netplayUrl:`ws://127.0.0.1:${port}/?${new URLSearchParams({room:roomId,run:String(serial),member:members[seat],runtimeGeneration:generation})}`,netplayPlayer:seat,netplayPlayerCount:count,netplaySeed:739,netplayDifficulty:1,netplayInputDelay:0,netplayInputDelayAuto:mode!==0,netplayAdonisMode:mode,netplayPredictionLimit:8,netplayLoadouts:Array.from({length:count},(_,character)=>({character,shot:0})),touchEnabled:true,touchMovementMode:'touch-unlimited'};
  const result=await page.evaluate(({seat,opts})=>command(seat,'configure',{music:'none',language:'ja',sharedResources:[{path:'/data/gothic.ttf',url:'/font'}],options:opts}),{seat,opts});assert.equal(result.ok,true,JSON.stringify(result));
  const preview=await page.evaluate(seat=>command(seat,'prepare-room'),seat);assert.equal(preview.presented,true,JSON.stringify(preview));};
 for(let seat=0;seat<count;seat++)await configure(seat);
 const launch=await page.evaluate(count=>Promise.all(Array.from({length:count},(_,seat)=>command(seat,'launch',{launchSerial:1}))),count);assert.ok(launch.every(r=>r.ok),JSON.stringify(launch));
 await page.waitForFunction(count=>[...document.querySelectorAll('iframe')].every(f=>f.contentWindow.__eaglerNetplayLanFrame>250),count,{timeout:30000});
 report.running=await page.evaluate(()=>[...document.querySelectorAll('iframe')].map(f=>({frame:f.contentWindow.__eaglerNetplayLanFrame,route:f.contentWindow.__eaglerNetplayTransport,inputDelay:f.contentWindow.__eaglerNetplayInputDelay,saveRoot:f.contentWindow.Module.FS.readlink('/savesth05')})));
 if(process.env.TH05_MP_CRASH==='1'){
  const started=Date.now();
  await page.evaluate(()=>{
   const m=document.querySelector('iframe').contentWindow.Module;
   for(const name of ['_th05_clear','_th05_shutdown','_th05_net_close','_th05_save','_th05_key','_th05_pointer','_th05_touch_motion'])m[name]=()=>{throw Error('crashed Wasm called again: '+name);};
   m.onAbort('Mismatching io.KeyCtrl/io.KeyShift vs io.KeyMods');
  });
  await page.waitForFunction(count=>events.filter(e=>e.event==='exit'&&e.status==='error').length===count,count,{timeout:5000});
  report.crash=await page.evaluate(()=>events.filter(e=>e.event==='error'||e.event==='exit'));
  report.notificationMs=Date.now()-started;
  for(const command of ['keyboard-clear','touch-cancel']){
   const result=await page.evaluate(command=>window.command(0,command),command);
   assert.equal(result.ok,true,JSON.stringify(result));
  }
  for(const [command,payload] of [
   ['keyboard',{code:'KeyZ',down:true}],
   ['direct-touch',{type:'down',id:1,x:.5,y:.5}],
   ['thprac-mouse',{type:'down',x:200,y:200}],
   ['touch-controls',{fireEnabled:true,focusEnabled:true,joystickX:0,joystickY:0,bombSerial:1,escapeSerial:1}],
  ]){
   const result=await page.evaluate(({command,payload})=>window.command(0,command,payload),{command,payload});
   assert.equal(result.ok,true,JSON.stringify(result));
  }
  for(let seat=1;seat<count;seat++)assert(report.crash.some(e=>e.epoch===7+seat&&e.event==='error'&&/P1.*TH05.*KeyMods/.test(e.message)),JSON.stringify(report.crash));
  assert.deepEqual(errors,[],'aborted engine cleanup does not call Wasm again');
 }else{
 if(jitter){
  await page.evaluate(()=>{for(const iframe of document.querySelectorAll('iframe')){
   const w=iframe.contentWindow;w.__rollbackTestDelays=0;
   for(const peer of w.__th05PeerTransport.peers.values()){
    const send=peer.channel.send.bind(peer.channel);
    peer.channel.send=text=>{const message=JSON.parse(text);if(message.type!=='input')return send(text);
     ++w.__rollbackTestDelays;w.setTimeout(()=>{if(peer.channel.readyState==='open')send(text);},Math.max(30+(message.frame*37+peer.id*17)%95,(w.__holdInputUntil??0)-w.performance.now()));
    };
   }
  }});
  for(let pulse=0;pulse<10;pulse++){await page.evaluate(async({count,pulse})=>{for(let seat=0;seat<count;seat++){await command(seat,'keyboard',{code:'KeyZ',down:pulse%2===0});await command(seat,'keyboard',{code:seat%2?'ArrowRight':'ArrowLeft',down:pulse%2===0});}},{count,pulse});await page.waitForTimeout(110);}
  await page.waitForFunction(()=>[...document.querySelectorAll('iframe')].every(f=>f.contentWindow.__eaglerNetplayLanFrame>750),null,{timeout:30000});
  report.rollback=await page.evaluate(()=>[...document.querySelectorAll('iframe')].map(f=>({corrections:f.contentWindow.__eaglerNetplayRollback,replayed:f.contentWindow.__eaglerNetplayResimulated,delayed:f.contentWindow.__rollbackTestDelays,confirmed:f.contentWindow.__eaglerNetplayConfirmed,frame:f.contentWindow.__eaglerNetplayFrame})));
  assert.ok(report.rollback.every(r=>r.delayed>0&&(mode===1?r.corrections===0&&r.replayed===0:r.corrections>0&&r.replayed>0)),JSON.stringify(report.rollback));
  report.waitReuse=await page.evaluate(async()=>{
   const worlds=[...document.querySelectorAll('iframe')].map(f=>f.contentWindow),saved=[];
   for(const w of worlds){w.__holdInputUntil=w.performance.now()+900;w.__waitDraws=0;const draw=w.Module._th05_coop_draw;saved.push(draw);w.Module._th05_coop_draw=(...args)=>{++w.__waitDraws;return draw(...args);};}
   const pause=ms=>new Promise(resolve=>setTimeout(resolve,ms));
   try{await pause(550);const before=worlds.map(w=>({frame:w.__eaglerNetplayLanFrame,draws:w.__waitDraws}));await pause(160);return worlds.map((w,i)=>({before:before[i],after:{frame:w.__eaglerNetplayLanFrame,draws:w.__waitDraws}}));}
   finally{worlds.forEach((w,i)=>{w.Module._th05_coop_draw=saved[i];w.__holdInputUntil=0;});}
  });
  assert.ok(report.waitReuse.every(r=>r.before.frame===r.after.frame&&r.before.draws===r.after.draws),JSON.stringify(report.waitReuse));
  await page.waitForTimeout(600);
 }
 assert.ok(report.running.every(r=>r.route===route));assert.ok(report.running.every(r=>r.saveRoot==='/savesth05mp'));
 const beforeX=await page.evaluate(()=>document.querySelector('iframe').contentWindow.Module._th05_coop_value(1,5));
 // Chromium emits real pen PointerEvents through the iframe canvas path.
 const pen=await page.context().newCDPSession(page),surface=await page.frameLocator('iframe').first().locator('canvas').boundingBox();
 const penX=await page.evaluate(()=>document.querySelector('iframe').contentWindow.Module._th05_coop_value(0,5));
 const stylus=async(type,x,down=false)=>pen.send('Input.dispatchMouseEvent',{type,pointerType:'pen',x:surface.x+x,y:surface.y+300,button:type==='mouseMoved'?'none':'left',buttons:down?1:0});
 await stylus('mouseMoved',200);await page.waitForTimeout(100);
 assert.equal(await page.evaluate(()=>document.querySelector('iframe').contentWindow.Module._th05_coop_value(0,5)),penX,'Pen hover must not move the player');
 await stylus('mousePressed',200,true);await stylus('mouseMoved',230,true);await page.waitForTimeout(180);await stylus('mouseReleased',230);await page.waitForTimeout(150);
 const penAfter=await page.evaluate(()=>[...document.querySelectorAll('iframe')].map(f=>f.contentWindow.Module._th05_coop_value(0,5)));
 assert.ok(penAfter.every(x=>x>penX&&x-penX<1600),JSON.stringify({penX,penAfter}));
 await stylus('mouseMoved',280);await page.waitForTimeout(150);
 const penHover=await page.evaluate(()=>document.querySelector('iframe').contentWindow.Module._th05_coop_value(0,5));assert.equal(penHover,penAfter[0],'Lifted pen must release movement');
 report.pen={penX,penAfter,penHover};await pen.detach();
 await page.evaluate(()=>command(1,'keyboard',{code:'ArrowLeft',down:true}));await page.waitForTimeout(300);await page.evaluate(()=>command(1,'keyboard',{code:'ArrowLeft',down:false}));
 const positions=await page.evaluate(()=>[...document.querySelectorAll('iframe')].map(f=>Array.from({length:2},(_,seat)=>f.contentWindow.Module._th05_coop_value(seat,5))));assert.ok(positions[0][1]<beforeX,JSON.stringify({beforeX,positions}));report.positions=positions;
 // The phone path owns a real short touch edge in the native pause menu.
 await page.evaluate(()=>command(0,'keyboard',{code:'Escape',down:true}));await page.waitForTimeout(80);await page.evaluate(()=>command(0,'keyboard',{code:'Escape',down:false}));
 await page.waitForFunction(()=>[...document.querySelectorAll('iframe')].every(f=>f.contentWindow.Module._th05_input_scene()===0),null,{timeout:5000});
 if(jitter){await page.evaluate(()=>{for(const frame of document.querySelectorAll('iframe')){const w=frame.contentWindow;w.__holdInputUntil=w.performance.now()+500;}});await page.waitForTimeout(180);}
 await page.evaluate(()=>command(0,'direct-touch',{type:'down',id:23,x:200/640,y:195/480}));await page.waitForTimeout(35);await page.evaluate(()=>command(0,'direct-touch',{type:'up',id:23,x:200/640,y:195/480}));
 await page.waitForFunction(()=>[...document.querySelectorAll('iframe')].every(f=>f.contentWindow.Module._th05_input_scene()===4),null,{timeout:5000});report.touchPause=true;
 const touchBefore=await page.evaluate(()=>document.querySelector('iframe').contentWindow.Module._th05_coop_value(0,5));
 await page.evaluate(()=>command(0,'direct-touch',{type:'down',id:24,x:.5,y:.75}));await page.evaluate(()=>command(0,'direct-touch',{type:'move',id:24,x:.55,y:.75}));await page.waitForTimeout(220);
 await page.evaluate(()=>command(0,'direct-touch',{type:'up',id:24,x:.55,y:.75}));
 const touchAfter=await page.evaluate(()=>document.querySelector('iframe').contentWindow.Module._th05_coop_value(0,5));assert.ok(touchAfter>touchBefore&&touchAfter-touchBefore<1600,JSON.stringify({touchBefore,touchAfter}));report.touchMovement={touchBefore,touchAfter};
 await page.screenshot({path:resolve(out,`${count}p-${route}.png`)});
 const replayFrame=jitter?720:120;
 const replayHash=await page.evaluate(frame=>document.querySelector('iframe').contentWindow.Module._th05_net_hash(frame)>>>0,replayFrame);
 const returned=await page.evaluate(()=>command(0,'return-room'));assert.equal(returned.presented,true,JSON.stringify(returned));
 await page.waitForFunction(count=>events.filter(e=>e.event==='room-menu'&&e.launchSerial===1).length===count,count,{timeout:15000});
 report.returned=true;report.errors=errors;report.runtimeErrors=await page.evaluate(()=>events.filter(e=>e.event==='error'));assert.deepEqual(errors,[]);assert.deepEqual(report.runtimeErrors,[]);
 if(process.env.TH05_MP_REPLAY==='1'){
  assert.ok(replayHash);
  report.replay=await page.evaluate(async({replayHash,replayFrame,count})=>{
   const expect=(value,message)=>{if(!value)throw Error(message);};
   const request=async(commandName,payload,seat=0)=>{
    const result=await command(seat,commandName,payload);
    expect(result.ok,JSON.stringify(result));return result;
   };
   const records=await Promise.all(Array.from({length:count},(_,seat)=>request('list',{},seat)));
   const paths=records.map(record=>record.files.find(f=>/^replay\/th05mp_ud[0-9a-f]{4}\.rpy$/.test(f.path))?.path);
   expect(paths.every(Boolean),'every seat should save its confirmed run');
   const path=paths[0],{bytes}=await request('read',{path});
   const rejected=await command(0,'write',{path:'replay/invalid.rpy',bytes:bytes.slice(0,-1)});
   expect(!rejected.ok,'truncated multiplayer replay accepted');
   const imported='replay/th05mp_udf000.rpy';
   await request('write',{path:imported,bytes});
   await request('configure',{music:'none',language:'ja',options:{replayViewer:true}});
   await request('launch',{launchSerial:2});
   const opened=await request('playback',{action:'inspect',path:imported});
   const stage=opened.description.stages[0];
   expect(stage.id===1&&stage.frames>=250&&opened.description.characters.length===count,'incorrect stage metadata');
   await request('playback',{action:'open',path:imported,stage:stage.id});
   await request('playback',{action:'pause'});
   const seek=await request('playback',{action:'seek',value:replayFrame});
   expect(seek.playback.frame===replayFrame&&seek.playback.paused,'paused seek failed');
   const runtime=document.querySelector('iframe').contentWindow;
   expect(runtime.Module._th05_coop_checksum()>>>0===replayHash,'replay differs from confirmed live world');
   const speed=await request('playback',{action:'rate',value:4});
   expect(speed.playback.rate===4,'rate update failed');
   const end=await request('playback',{action:'seek',value:stage.frames});
   expect(end.playback.ended&&end.playback.frame===stage.frames,'replay end was not reached');
   return {paths,stage,seek:seek.playback.frame,hash:replayHash,bytes:bytes.length};
  },{replayHash,replayFrame,count});
 }
 // The same native documents can start a new confirmed run after room return.
 report.secondStart=await page.evaluate(count=>{
  return [...document.querySelectorAll('iframe')].map((f,seat)=>f.contentWindow.Module._th05_coop_start(count,seat,1,740,0,1,2));
 },count);assert.ok(report.secondStart.every(v=>v===1));
 }
 await writeFile(resolve(out,`${count}p-${route}.json`),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify(report,null,2));
}catch(error){console.error(report,log);throw error;}
finally{await browser?.close();await site?.close();for(const ws of sockets)ws.terminate();relay.kill();}
