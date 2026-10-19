import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,extname} from 'node:path';
import {spawn} from 'node:child_process';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
for(const name of ['TH10_PREVIEW_RUNTIME','TH10_TEST_ARCHIVE','EAGLER_SHARED_GOTHIC','EAGLER_SHARED_UNIFONT'])assert(process.env[name],name+' is required (private local input)');
const mode=Number(process.env.TH10_PREVIEW_MODE??1);
assert([0,1].includes(mode));
const runtime=resolve(process.env.TH10_PREVIEW_RUNTIME),out=resolve('.cache/native-room-preview-th10-proof-mode-'+mode);await mkdir(out,{recursive:true});
const archive=await readFile(process.env.TH10_TEST_ARCHIVE);
const sharedFonts=new Map([['/msgothic.ttc',await readFile(process.env.EAGLER_SHARED_GOTHIC)],['/unifont.otf',await readFile(process.env.EAGLER_SHARED_UNIFONT)]]);
let browser,relayLog='';const sockets=[];
const relay=spawn(process.execPath,['services/relay/src/netplay-relay.mjs'],{cwd:resolve('..'),windowsHide:true,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe']});
relay.stdout.on('data',b=>relayLog+=b);relay.stderr.on('data',b=>relayLog+=b);
const wait=async(check,label,ms=30000)=>{const end=Date.now()+ms;while(Date.now()<end){if(await check())return;await new Promise(r=>setTimeout(r,30));}throw Error(label+' '+relayLog);};
const html=`<!doctype html><meta charset="utf-8"><style>body{margin:0;background:#111;color:#eee}iframe{width:640px;height:480px;border:0}</style><script>
window.events=[];window.ready=new Set();window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1'){events.push(e.data);if(e.data.event==='ready')ready.add(e.data.epoch);}});
window.addRuntime=s=>{const f=document.createElement('iframe');f.id='p'+s;f.src='/runtime/th10.html?runtimeVariant=multiplayer&managedData=1&runtimeEpoch='+(s+1);document.body.append(f);};
window.rpc=(s,command,fields={})=>new Promise((ok,no)=>{const f=document.querySelector('#p'+s),request=crypto.randomUUID(),timer=setTimeout(()=>no(Error(command+' timeout')),90000);function recv(e){if(e.source===f.contentWindow&&e.data?.request===request){clearTimeout(timer);removeEventListener('message',recv);e.data.ok?ok(e.data):no(Error(e.data.error));}}addEventListener('message',recv);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th10',epoch:s+1,request,command,...fields},location.origin);});</script>`;
const server=createServer(async(req,res)=>{try{const p=new URL(req.url,'http://localhost').pathname;if(p==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}if(p==='/data'){res.end(archive);return;}if(sharedFonts.has(p)){res.end(sharedFonts.get(p));return;}if(!p.startsWith('/runtime/')||p.includes('..')){res.writeHead(404).end();return;}const file=resolve(runtime,p.slice(9));if(!file.startsWith(runtime+'\\')&&!file.startsWith(runtime+'/'))throw Error('path');res.setHeader('Content-Type',({'.mjs':'application/javascript','.wasm':'application/wasm','.html':'text/html','.css':'text/css','.json':'application/json'})[extname(file)]||'application/octet-stream');res.end(await readFile(file));}catch(e){res.writeHead(404).end(String(e));}});
await new Promise(r=>server.listen(0,'127.0.0.1',r));
function send(socket,message,predicate){return new Promise((ok,no)=>{const timer=setTimeout(()=>{cleanup();no(Error('lobby timeout '+JSON.stringify(message)));},10000);function recv(e){const m=JSON.parse(e.data);if(m.type==='error'){cleanup();no(Error(m.error));}else if(predicate(m)){cleanup();ok(m);}}function cleanup(){clearTimeout(timer);socket.removeEventListener('message',recv);}socket.addEventListener('message',recv);if(message)socket.send(JSON.stringify(message));});}
try{
 await wait(()=>/listening ws:\/\/127.0.0.1:(\d+)/.test(relayLog),'relay startup');
 const port=/listening ws:\/\/127.0.0.1:(\d+)/.exec(relayLog)[1],room='th10mp-native-preview';
 for(let s=0;s<2;s++){const socket=new WebSocket('ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,lobby:'preview-public-'+s,member:'preview-private-'+s,intent:s?'join':'create',players:'2',difficulty:'1'}));sockets.push(socket);await send(socket,null,m=>m.type==='state');await send(socket,{type:'take-seat',seat:s,loadout:s},m=>!!m.room?.seats[s]);}
 browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
 const page=await browser.newPage(),errors=[],requests=[];page.on('pageerror',e=>{errors.push(e.message);console.log('PAGE ERROR',e.message);});page.on('console',m=>console.log('BROWSER',m.type(),m.text()));page.on('response',r=>{if(!r.ok())console.log('HTTP',r.status(),r.url());});page.on('request',r=>requests.push(r.url()));await page.setViewport({width:1280,height:480});await page.goto('http://127.0.0.1:'+server.address().port);
 await page.evaluate(()=>{addRuntime(0);addRuntime(1);});await page.waitForFunction(()=>ready.size===2,{timeout:90000});console.log('Both runtimes ready');
 await page.evaluate(async()=>{for(let s=0;s<2;s++){await rpc(s,'configure',{music:'none',sharedResources:[{path:'/msgothic.ttc',url:'/msgothic.ttc'},{path:'/unifont.otf',url:'/unifont.otf'}],options:{thpracEnabled:false,renderScale:1}});await rpc(s,'prepare-room');if(s===0)await new Promise(r=>setTimeout(r,5000));}});
 const frames=()=>page.frames().filter(f=>f.url().includes('runtimeEpoch='));
 const states=()=>Promise.all(frames().map(f=>f.evaluate(()=>{const {core,app}=__th10Runtime,heap=new Uint32Array(core.memory.buffer);return {phase:heap[core.sdl_game_status()>>>2],frame:heap[(core.sdl_stats()>>>2)+5],instance:!!core.sdl_room_preview};})));
 const before=await states();console.log('Title states',before);assert(before.every(s=>s.phase===4&&s.instance));assert(before[0].frame>before[1].frame+100);await page.screenshot({path:resolve(out,'native-title-before-start.png')});
 await page.evaluate(()=>{for(let s=0;s<2;s++)for(const code of ['KeyZ','Enter','ArrowDown']){rpc(s,'keyboard',{code,down:true});rpc(s,'keyboard',{code,down:false});}});
 await new Promise(r=>setTimeout(r,18000));const after=await states();assert(after.every((s,i)=>s.phase===4&&s.frame>900&&s.frame>before[i].frame+200),'title must animate beyond the 900-frame demo threshold');
 for(const f of frames())await f.evaluate(()=>{const {Module,core}=__th10Runtime;window.previewModule=Module;window.previewApp=__th10Runtime.app;window.rngSamples={};const original=Module.runtimeFinish;Module.runtimeFinish=(...args)=>{original?.(...args);const app=__th10Runtime.app,p=core.multiplayer_canonical_hashes(app)>>>2,v=Array.from(new Uint32Array(core.memory.buffer).subarray(p,p+44)),frame=new Uint32Array(core.memory.buffer)[(core.multiplayer_netplay_status(app)>>>2)+2];if(frame>0)rngSamples[frame]={rng:v.slice(13,17),semantic:v.slice(2,11)};};});
 for(let s=0;s<2;s++)await send(sockets[s],{type:'set-ready',ready:true},m=>m.room?.seats[s]?.ready);
 const start=await send(sockets[0],{type:'start',adonisMode:mode,inputDelay:0,inputDelayAuto:false,predictionReserve:2},m=>m.type==='start');
 const startAt=Date.now(),requestCount=requests.length;
 await page.evaluate(async({port,room,serial,mode})=>Promise.all([0,1].map(async s=>{await rpc(s,'configure',{music:'none',options:{thpracEnabled:false,renderScale:1,netplayMode:'lan',netplayUrl:'ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,run:String(serial),member:'preview-private-'+s}),netplayPlayer:s,netplayPlayerCount:2,netplayDifficulty:1,netplaySeed:1234,netplayLoadouts:[{character:0,shot:0},{character:1,shot:0}],netplayInputDelay:0,netplayInputDelayAuto:false,netplayAdonisMode:mode,netplayPredictionLimit:mode?0:8,netplayPredictionReserve:2}});await rpc(s,'launch');})),{port,room,serial:start.serial,mode});
 await wait(async()=>{const a=await Promise.all(frames().map(f=>f.evaluate(()=>{const {Module,core,app}=__th10Runtime;return new Uint32Array(core.memory.buffer)[(core.multiplayer_netplay_status(app)>>>2)+2];})));return a.every(n=>n>=300);},'multiplayer ticks',60000);
 const startMs=Date.now()-startAt;
 assert((await Promise.all(frames().map(f=>f.evaluate(()=>__th10Runtime.Module===previewModule&&__th10Runtime.app===previewApp)))).every(Boolean));
 assert(!requests.slice(requestCount).some(u=>/\/(data|resources.json|th10-sdl.wasm|th10-sdl.mjs)$/.test(u)),'start must reuse DATA and the native program');
 let matchedFrames=0;
 {const samples=await Promise.all(frames().map(f=>f.evaluate(()=>rngSamples)));for(const [frame,value]of Object.entries(samples[0]))if(samples[1][frame]){assert.deepEqual(value,samples[1][frame],'native RNG/semantic world differs at frame '+frame);matchedFrames++;}assert(matchedFrames>30,'insufficient matching-frame RNG samples');}
 assert.deepEqual(errors,[]);await page.screenshot({path:resolve(out,'native-multiplayer-after-start.png')});
 await writeFile(resolve(out,'result.json'),JSON.stringify({passed:true,mode,startMs,previewStates:before,afterWaitStates:after,waitMs:18000,matchedFrames,sameNativeInstances:true,noColdProgramOrDataRequests:true,scope:'Actual production TH10 multiplayer Wasm and original DATA, two local players and relay; same-frame native RNG and semantic world hashes. No real mobile hardware or remote network acceptance'},null,2));
 console.log('PASS native animated title, input rejection, no demo beyond 900 title ticks, same-engine 2P startup, matched RNG/semantic frames',matchedFrames,startMs+'ms including 300 gameplay ticks');
}finally{for(const s of sockets)s.close();await browser?.close();server.close();relay.kill();}
