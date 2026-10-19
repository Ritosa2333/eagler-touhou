// Actual production Wasm + privately supplied DATA. Different title waiting
// durations must not change the shared match state. Run from launcher/.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,extname} from 'node:path';
import {spawn} from 'node:child_process';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';
const game=process.argv[2];assert(['th06','th07','th08','th11','th09','th075'].includes(game));
const mode=Number(process.argv[3]||0),runtime=resolve(process.env.ROOM_PREVIEW_RUNTIME),data=await readFile(process.env.ROOM_PREVIEW_DATA);
const out=resolve('.cache/room-title-preload',game+'-'+mode);await mkdir(out,{recursive:true});
const fonts=new Map(await Promise.all([['/msgothic.ttc',process.env.EAGLER_SHARED_GOTHIC],['/unifont.otf',process.env.EAGLER_SHARED_UNIFONT]].map(async([url,path])=>[url,await readFile(path)])));
const html=`<!doctype html><style>iframe{width:640px;height:480px}</style><script>
window.events=[];window.ready=new Set();window.__eaglerPrepareManagedRuntimeDataV1=async()=>{const buffer=await(await fetch('/data')).arrayBuffer();return {buffer,bytes:buffer.byteLength};};
addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1'){events.push(e.data);if(e.data.event==='ready')ready.add(e.data.epoch);}});
window.addRuntime=s=>{const f=document.createElement('iframe');f.id='p'+s;f.src='/runtime/${game}.html?hosted=1&runtimeVariant=multiplayer&managedData=1&runtimeEpoch='+(s+1);document.body.append(f);};
window.rpc=(s,command,fields={})=>new Promise((ok,no)=>{const f=document.querySelector('#p'+s),request=crypto.randomUUID(),timer=setTimeout(()=>no(Error(command+' timeout')),90000);function receive(e){if(e.source===f.contentWindow&&e.data?.request===request){clearTimeout(timer);removeEventListener('message',receive);e.data.ok?ok(e.data):no(Error(e.data.error));}}addEventListener('message',receive);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'${game}',epoch:s+1,request,command,...fields},location.origin);});</script>`;
const server=createServer(async(req,res)=>{try{const p=new URL(req.url,'http://localhost').pathname;if(p==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}if(p==='/data'){res.end(data);return;}if(fonts.has(p)){res.end(fonts.get(p));return;}assert(p.startsWith('/runtime/')&&!p.includes('..'));const file=resolve(runtime,p.slice(9));res.setHeader('Content-Type',({'.js':'application/javascript','.mjs':'application/javascript','.wasm':'application/wasm','.html':'text/html','.css':'text/css','.json':'application/json'})[extname(file)]||'application/octet-stream');res.end(await readFile(file));}catch(e){res.writeHead(404).end(String(e));}});
await new Promise(r=>server.listen(0,'127.0.0.1',r));
let relayLog='',browser;const sockets=[];
const relay=spawn(process.execPath,['services/relay/src/netplay-relay.mjs'],{cwd:resolve('..'),windowsHide:true,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:''},stdio:['ignore','pipe','pipe']});
relay.stdout.on('data',b=>relayLog+=b);relay.stderr.on('data',b=>relayLog+=b);
const wait=async(fn,label,ms=90000)=>{const end=Date.now()+ms;while(Date.now()<end){if(await fn())return;await new Promise(r=>setTimeout(r,50));}throw Error(label+' '+relayLog);};
function send(socket,message,predicate){return new Promise((ok,no)=>{const timer=setTimeout(()=>{cleanup();no(Error('relay timeout'));},10000);function receive(e){const m=JSON.parse(e.data);if(m.type==='error'){cleanup();no(Error(m.error));}else if(predicate(m)){cleanup();ok(m);}}function cleanup(){clearTimeout(timer);socket.removeEventListener('message',receive);}socket.addEventListener('message',receive);if(message)socket.send(JSON.stringify(message));});}
try{
 await wait(()=>/listening ws:\/\/127.0.0.1:(\d+)/.test(relayLog),'relay');
 const port=/listening ws:\/\/127.0.0.1:(\d+)/.exec(relayLog)[1],room=game+'mp-1234';
 for(let s=0;s<2;s++){const ws=new WebSocket('ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,lobby:'public-'+s,member:'private-'+s,intent:s?'join':'create',players:'2',difficulty:'1'}));sockets.push(ws);await send(ws,null,m=>m.type==='state');await send(ws,{type:'take-seat',seat:s,loadout:s},m=>!!m.room?.seats[s]);}
 browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
 const page=await browser.newPage(),errors=[],requests=[];page.on('pageerror',e=>{errors.push(e.message);console.log('PAGE ERROR',e.message);});page.on('console',m=>{if(m.type()==='error')console.log('BROWSER',m.text());});page.on('request',r=>requests.push(r.url()));
 page.on('requestfailed',r=>console.log('FAILED',r.url(),r.failure()));
 await page.setViewport({width:1320,height:600});await page.goto('http://127.0.0.1:'+server.address().port);await page.evaluate(()=>{addRuntime(0);addRuntime(1);});
 try{await page.waitForFunction(()=>ready.size===2,{timeout:90000});}catch(e){console.log('BOOT',await Promise.all(page.frames().map(f=>f.evaluate(()=>({url:location.href,text:document.body?.innerText})))));throw e;}
 console.log('READY');
 await page.evaluate(async game=>{for(let s=0;s<2;s++){await rpc(s,'configure',{music:'none',resources:[],runtimeResources:[],sharedResources:game==='th075'?[]:[{path:'/msgothic.ttc',url:'/msgothic.ttc'},{path:'/unifont.otf',url:'/unifont.otf'}],options:{thpracEnabled:false,renderScale:1,...(game==='th08'?{multiplayerPreflight:true}:{})}});await rpc(s,'prepare-room');if(s===0)await new Promise(r=>setTimeout(r,4000));}},game);
 const frames=()=>page.frames().filter(f=>f.url().includes('runtimeEpoch='));
 const states=()=>Promise.all(frames().map(f=>f.evaluate(game=>{const r=window['__'+game+'Runtime'];if(game==='th06'||game==='th07')return {phase:Module._eagler_room_preview_ready(),frame:window.preloadPresents||0};if(game==='th09'){const s=r.status().title;return {phase:s[1],frame:s[0]};}if(game==='th075')return {phase:Module._th075_scene(),frame:Module._th075_frame()};return game==='th11'?{phase:r.core.th11_phase(),frame:r.core.th11_frame()}:{phase:r.status()[0],frame:new Uint32Array(r.core.memory.buffer)[(r.core.sdl_stats()>>>2)+5]};},game)));
 if(game==='th06'||game==='th07')for(const f of frames())await f.evaluate(()=>{window.preloadPresents=0;const original=EaglerTouhouFrameHealth;EaglerTouhouFrameHealth=(fps,...args)=>{window.preloadPresents+=fps;original(fps,...args);};});
 const before=await states();console.log('PREVIEW',before);console.log('RUNTIME ERRORS',await page.evaluate(()=>events.filter(e=>e.event==='error')));await page.screenshot({path:resolve(out,'title.png')});
 await page.evaluate(game=>{for(let s=0;s<2;s++)for(const [code,key,keyCode]of [['KeyZ','z',90],['Enter','Enter',13],['ArrowDown','ArrowDown',40]])for(const down of [true,false])document.querySelector('#p'+s).contentWindow.postMessage({protocol:'eagler-touhou/1',game,epoch:s+1,command:'keyboard',code,key,keyCode,location:0,down},location.origin);},game);
 // TH11's attract threshold is 1800 native frames; TH08's is 1500.
 await new Promise(r=>setTimeout(r,game==='th075'?65000:33000));const after=await states();console.log('WAITED',after);assert(after.every((v,i)=>v.phase===before[i].phase&&v.frame>before[i].frame+300),'title must animate without entering demo');
 for(const f of frames())await f.evaluate(game=>{
  window.preloadSamples={};const r=window['__'+game+'Runtime'];
  if(game==='th06'||game==='th07'){window.preloadModule=Module;return;}
  if(game==='th075'){window.preloadModule=Module;const sample=()=>{const h=Module.HEAPU32,p=Module._th075_mp_status()>>>2,n=h[p+53];if(n>0)preloadSamples[n]=h[p+54];requestAnimationFrame(sample);};requestAnimationFrame(sample);return;}
  if(game==='th09'){window.preloadModule=r.core;const original=r.core.onGameFrame;r.core.onGameFrame=(...args)=>{original?.(...args);const h=r.core.HEAPU32,p=r.core._th09_network_info()>>>2,n=h[p+3];if(n>0)preloadSamples[n]=r.core._th09_network_hash();};return;}
  window.preloadModule=r.Module;window.preloadApp=r.app;const key=game==='th11'?'onGameFrame':'runtimeFinish',original=r.Module[key];r.Module[key]=(...args)=>{original?.(...args);const h=new Uint32Array(r.core.memory.buffer);if(game==='th11'){const p=r.core.th11_mp_game_status()>>>2,frame=h[p+6];if(frame>0)preloadSamples[frame]=h[p+1];}else{const p=r.core.multiplayer_netplay_status(r.app)>>>2,frame=h[p+3];if(frame>0){const c=r.core.multiplayer_canonical_state(r.app)>>>2;preloadSamples[frame]=Array.from(h.subarray(c,c+13));}}};
 },game);
 for(let s=0;s<2;s++)await send(sockets[s],{type:'set-ready',ready:true},m=>m.room?.seats[s]?.ready);
 const start=await send(sockets[0],['th06','th07'].includes(game)?{type:'start'}:{type:'start',adonisMode:mode,inputDelay:0,inputDelayAuto:false,predictionReserve:2},m=>m.type==='start'),count=requests.length;
 await page.evaluate(async({game,port,room,serial,mode})=>Promise.all([0,1].map(async s=>{await rpc(s,'configure',{music:'none',resources:[],runtimeResources:[],sharedResources:[],options:{thpracEnabled:false,renderScale:1,netplayMode:'lan',netplayUrl:'ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,run:String(serial),member:'private-'+s,player:String(s)}),netplayPlayer:s,netplayPlayerCount:2,netplayDifficulty:1,netplaySeed:1234,netplayLoadouts:[{character:0,shot:0},{character:1,shot:0}],netplayInputDelay:0,netplayInputDelayAuto:false,netplayAdonisMode:mode,netplayPredictionLimit:game==='th08'?8:mode?0:game==='th11'?12:8,netplayPredictionReserve:2}});await rpc(s,'launch');})),{game,port,room,serial:start.serial,mode});
 const flat=game==='th06'||game==='th07';
 try{await wait(async()=>{const samples=await Promise.all(frames().map(f=>f.evaluate(flat=>Object.keys(flat?window.__eaglerNetplayLanHashes||{}:preloadSamples).length,flat)));return samples.every(n=>n>=(flat||game==='th075'?2:100));},'match frames');}catch(e){console.log('MATCH',await Promise.all(frames().map(f=>f.evaluate(()=>({samples:preloadSamples,failed:window.__eaglerNetplayError,frame:window.__eaglerNetplayLanFrame,status:typeof Module._th075_mp_status==='function'?Array.from(Module.HEAPU32.subarray(Module._th075_mp_status()>>>2,(Module._th075_mp_status()>>>2)+64)):null,text:document.body.innerText})))));throw e;}
 const samples=await Promise.all(frames().map(f=>f.evaluate(flat=>flat?window.__eaglerNetplayLanHashes:preloadSamples,flat)));let matches=0;
 for(const [n,v]of Object.entries(samples[0]))if(samples[1][n]!==undefined){assert.deepEqual(v,samples[1][n],'same-frame world '+n);matches++;}assert(matches>=(flat||game==='th075'?2:20));
 assert(!requests.slice(count).some(u=>/\/(data|resources.json|[^/]+\.wasm)$/.test(u)),'Start must not cold-load again');
 for(const f of frames())assert(await f.evaluate(game=>{if(['th06','th07','th075'].includes(game))return Module===preloadModule;const r=window['__'+game+'Runtime'];return (game==='th09'?r.core:r.Module)===preloadModule;},game));
 assert.deepEqual(errors,[]);await writeFile(resolve(out,'result.json'),JSON.stringify({game,mode,before,after,matches,scope:'Production native Runtime, original local DATA, two local peers; not physical mobile/WAN acceptance'},null,2));console.log('PASS',game,mode,matches);
}finally{for(const s of sockets)s.close();await browser?.close();server.close();relay.kill();}
