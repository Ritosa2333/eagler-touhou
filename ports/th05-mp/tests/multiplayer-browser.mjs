import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {writeFile,mkdir} from 'node:fs/promises';
import {resolve} from 'node:path';
import {WebSocket} from 'ws';
import {launchBrowser} from '../../../tools/browser/launch.mjs';
import {multiplayerFixture} from './multiplayer-fixture.mjs';
const repo=resolve(import.meta.dirname,'../../..'),out=resolve(process.env.TH05_TEST_OUTPUT??resolve(repo,'ports/th05-mp/artifacts/multiplayer-browser'));await mkdir(out,{recursive:true});
const count=Number(process.env.TH05_MP_PLAYERS??2),route=process.env.TH05_MP_ROUTE??'rtc';
let log='',port;const sockets=[];
const relay=spawn(process.execPath,['services/relay/src/netplay-relay.mjs'],{cwd:repo,windowsHide:true,stdio:['ignore','pipe','pipe'],env:{...process.env,EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK:'1',EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_PUBLIC_NICKNAMES:'0',EAGLER_NETPLAY_STUN_URLS:''}});
relay.stdout.on('data',b=>{log+=b;const m=log.match(/listening ws:\/\/127\.0\.0\.1:(\d+)/);if(m)port=Number(m[1]);});relay.stderr.on('data',b=>log+=b);
const wait=async(check,label)=>{const end=Date.now()+15000;while(!check()){if(Date.now()>end)throw Error(label+' timeout\n'+log);await new Promise(r=>setTimeout(r,10));}};
const roomId='th05mp-native-'+Date.now().toString(36),generation='a'.repeat(64),members=Array.from({length:count},(_,i)=>'native_member_'+i+'_'+Date.now().toString(36));
async function lobby(seat){const ws=new WebSocket(`ws://127.0.0.1:${port}/?${new URLSearchParams({room:roomId,lobby:'native_client_'+seat,member:members[seat],players:String(count),difficulty:'1'})}`),messages=[];sockets.push(ws);ws.on('message',b=>messages.push(JSON.parse(String(b))));await wait(()=>messages.some(m=>m.type==='state'),'lobby');return {messages,send:m=>ws.send(JSON.stringify(m)),room:()=>messages.findLast(m=>m.room)?.room};}
let browser,site;const report={count,route};
try{
 await wait(()=>port,'relay startup');const clients=[];
 for(let seat=0;seat<count;seat++){const client=await lobby(seat);clients.push(client);client.send({type:'take-seat',seat,loadout:seat,name:'Native '+seat});await wait(()=>client.room()?.seats[seat],'seat');if(seat===0){client.send({type:'settings',playerCount:count,difficulty:1});await wait(()=>client.room()?.playerCount===count,'capacity');}}
 for(const client of clients)client.send({type:'set-ready',ready:true,runtimeGeneration:generation,movementMode:'touch'});
 await wait(()=>clients[0].room()?.seats.slice(0,count).every(s=>s?.ready),'ready');clients[0].send({type:'start',adonisMode:1,inputDelay:0,inputDelayAuto:true,predictionLimit:0});
 await wait(()=>clients[0].messages.some(m=>m.type==='start'),'run');const serial=clients[0].messages.find(m=>m.type==='start').serial;
 site=await multiplayerFixture(count);browser=await launchBrowser();const page=await browser.newPage({viewport:{width:1320,height:1020}}),errors=[];
 if(route==='relay')await page.addInitScript(()=>{delete window.RTCPeerConnection;});
 page.on('pageerror',e=>errors.push(e.message));await page.goto(site.url);await page.waitForFunction(count=>events.filter(e=>e.event==='ready').length===count,count,{timeout:30000});
 const configure=async seat=>{const opts={netplayMode:'lan',netplayUrl:`ws://127.0.0.1:${port}/?${new URLSearchParams({room:roomId,run:String(serial),member:members[seat],runtimeGeneration:generation})}`,netplayPlayer:seat,netplayPlayerCount:count,netplaySeed:739,netplayDifficulty:1,netplayInputDelay:0,netplayInputDelayAuto:true,netplayAdonisMode:1,netplayPredictionLimit:0,netplayLoadouts:Array.from({length:count},(_,character)=>({character,shot:0})),touchEnabled:true,touchMovementMode:'touch-unlimited'};
  const result=await page.evaluate(({seat,opts})=>command(seat,'configure',{music:'none',language:'ja',sharedResources:[{path:'/data/gothic.ttf',url:'/font'}],options:opts}),{seat,opts});assert.equal(result.ok,true,JSON.stringify(result));
  const preview=await page.evaluate(seat=>command(seat,'prepare-room'),seat);assert.equal(preview.presented,true,JSON.stringify(preview));};
 for(let seat=0;seat<count;seat++)await configure(seat);
 const launch=await page.evaluate(count=>Promise.all(Array.from({length:count},(_,seat)=>command(seat,'launch',{launchSerial:1}))),count);assert.ok(launch.every(r=>r.ok),JSON.stringify(launch));
 await page.waitForFunction(count=>[...document.querySelectorAll('iframe')].every(f=>f.contentWindow.__eaglerNetplayLanFrame>250),count,{timeout:30000});
 report.running=await page.evaluate(()=>[...document.querySelectorAll('iframe')].map(f=>({frame:f.contentWindow.__eaglerNetplayLanFrame,route:f.contentWindow.__eaglerNetplayTransport,inputDelay:f.contentWindow.__eaglerNetplayInputDelay,saveRoot:f.contentWindow.Module.FS.readlink('/savesth05')})));
 assert.ok(report.running.every(r=>r.route===route));assert.ok(report.running.every(r=>r.saveRoot==='/savesth05mp'));
 const beforeX=await page.evaluate(()=>document.querySelector('iframe').contentWindow.Module._th05_coop_value(1,5));
 await page.evaluate(()=>command(1,'keyboard',{code:'ArrowLeft',down:true}));await page.waitForTimeout(300);await page.evaluate(()=>command(1,'keyboard',{code:'ArrowLeft',down:false}));
 const positions=await page.evaluate(()=>[...document.querySelectorAll('iframe')].map(f=>Array.from({length:2},(_,seat)=>f.contentWindow.Module._th05_coop_value(seat,5))));assert.ok(positions[0][1]<beforeX,JSON.stringify({beforeX,positions}));report.positions=positions;
 // The phone path owns a real short touch edge in the native pause menu.
 await page.evaluate(()=>command(0,'keyboard',{code:'Escape',down:true}));await page.waitForTimeout(80);await page.evaluate(()=>command(0,'keyboard',{code:'Escape',down:false}));
 await page.waitForFunction(()=>[...document.querySelectorAll('iframe')].every(f=>f.contentWindow.Module._th05_input_scene()===0),null,{timeout:5000});
 await page.evaluate(()=>command(0,'direct-touch',{type:'down',id:23,x:200/640,y:195/480}));await page.waitForTimeout(35);await page.evaluate(()=>command(0,'direct-touch',{type:'up',id:23,x:200/640,y:195/480}));
 await page.waitForFunction(()=>[...document.querySelectorAll('iframe')].every(f=>f.contentWindow.Module._th05_input_scene()===4),null,{timeout:5000});report.touchPause=true;
 const touchBefore=await page.evaluate(()=>document.querySelector('iframe').contentWindow.Module._th05_coop_value(0,5));
 await page.evaluate(()=>command(0,'direct-touch',{type:'down',id:24,x:.5,y:.75}));await page.evaluate(()=>command(0,'direct-touch',{type:'move',id:24,x:.55,y:.75}));await page.waitForTimeout(220);
 await page.evaluate(()=>command(0,'direct-touch',{type:'up',id:24,x:.55,y:.75}));
 const touchAfter=await page.evaluate(()=>document.querySelector('iframe').contentWindow.Module._th05_coop_value(0,5));assert.ok(touchAfter>touchBefore&&touchAfter-touchBefore<1600,JSON.stringify({touchBefore,touchAfter}));report.touchMovement={touchBefore,touchAfter};
 await page.screenshot({path:resolve(out,`${count}p-${route}.png`)});
 const returned=await page.evaluate(()=>command(0,'return-room'));assert.equal(returned.presented,true,JSON.stringify(returned));
 await page.waitForFunction(count=>events.filter(e=>e.event==='room-menu'&&e.launchSerial===1).length===count,count,{timeout:15000});
 report.returned=true;report.errors=errors;report.runtimeErrors=await page.evaluate(()=>events.filter(e=>e.event==='error'));assert.deepEqual(errors,[]);assert.deepEqual(report.runtimeErrors,[]);
 // The same native documents can start a new confirmed run after room return.
 report.secondStart=await page.evaluate(count=>{
  return [...document.querySelectorAll('iframe')].map((f,seat)=>f.contentWindow.Module._th05_coop_start(count,seat,1,740,0,1,2));
 },count);assert.ok(report.secondStart.every(v=>v===1));
 await writeFile(resolve(out,`${count}p-${route}.json`),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify(report,null,2));
}catch(error){console.error(report,log);throw error;}
finally{await browser?.close();await site?.close();for(const ws of sockets)ws.terminate();relay.kill();}
