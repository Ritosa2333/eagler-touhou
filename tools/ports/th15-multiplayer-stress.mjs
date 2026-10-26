// Production TH15 multiplayer, real relay, retail assets and deterministic faults.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,writeFile,mkdir} from 'node:fs/promises';
import {spawn} from 'node:child_process';
import {resolve,extname,sep} from 'node:path';
import {pathToFileURL} from 'node:url';
import {createRequire} from 'node:module';
import {createHash} from 'node:crypto';
const repo=resolve(import.meta.dirname,'../..'),root=resolve(repo,'ports/th15-mp');
const launcher=resolve(process.env.TH15_LAUNCHER_ROOT||resolve(repo,'launcher'));
const puppeteer=createRequire(resolve(launcher,'package.json'))('puppeteer-core');
const {findChromiumExecutable}=await import(pathToFileURL(resolve(launcher,'lib/chromium-executable.mjs')));
const runtime=resolve(root,process.env.TH15_TEST_RUNTIME||'build-eagler-multiplayer');
const out=resolve(process.argv.find(v=>v.startsWith('--output='))?.slice(9)||resolve(repo,'artifacts/th15-multiplayer-stress'));
await mkdir(out,{recursive:true});
assert.ok(process.env.TH15_TEST_ARCHIVE,'Set TH15_TEST_ARCHIVE to legally supplied retail DATA');
const archive=await readFile(process.env.TH15_TEST_ARCHIVE),sha=b=>createHash('sha256').update(b).digest('hex');
const fonts=[['msgothic.ttc',process.env.TH15_TEST_FONT],['unifont.otf',process.env.TH15_TEST_UNICODE_FONT]];
for(const [name,file]of fonts)assert.ok(file,'Missing '+name);
const cases=process.env.TH15_STRESS_CASES?JSON.parse(process.env.TH15_STRESS_CASES):[
 {name:'2p-normal',players:2,mode:0,difficulty:1,cpu:1,seconds:30},
 {name:'3p-extra',players:3,mode:0,difficulty:4,cpu:1,seconds:30},
 {name:'3p-extra-cpu6',players:3,mode:0,difficulty:4,cpu:6,seconds:40},
 {name:'3p-extra-delay-cpu6',players:3,mode:1,difficulty:4,cpu:6,seconds:40},
 {name:'3p-extra-weak',players:3,mode:0,difficulty:4,cpu:1,seconds:60,fault:{latency:90,jitter:60,dropEvery:20,burstMs:1100,burstEveryMs:15000}},
 {name:'3p-normal-soak',players:3,mode:0,difficulty:1,cpu:1,seconds:180},
];
await writeFile(resolve(out,'inputs.json'),JSON.stringify({head:process.env.TH15_STRESS_HEAD||null,driver:{file:import.meta.filename,sha256:sha(await readFile(import.meta.filename))},runtime:JSON.parse(await readFile(resolve(runtime,'manifest.json'),'utf8')),archiveSha256:sha(archive),cases},null,2));
let options=[],browser,relayLog='';const sockets=[],reports=[];
const relay=spawn(process.execPath,[resolve(repo,'services/relay/src/netplay-relay.mjs')],{cwd:repo,windowsHide:true,env:{...process.env,EAGLER_NETPLAY_RELAY_HOST:'127.0.0.1',EAGLER_NETPLAY_RELAY_PORT:'0',EAGLER_NETPLAY_STUN_URLS:'',EAGLER_NETPLAY_RELAY_DELAY_MS:'0',EAGLER_NETPLAY_RELAY_JITTER_MS:'0',EAGLER_NETPLAY_RELAY_DROP_EVERY:'0'},stdio:['ignore','pipe','pipe']});
relay.stdout.on('data',b=>relayLog+=b);relay.stderr.on('data',b=>relayLog+=b);
async function wait(fn,label,ms=90000){const until=Date.now()+ms;while(Date.now()<until){if(await fn())return;await new Promise(r=>setTimeout(r,100));}throw Error(label+' timeout');}
function send(ws,message,predicate=()=>true){return new Promise((yes,no)=>{const timer=setTimeout(()=>{cleanup();no(Error('Lobby timeout'));},15000);function cleanup(){clearTimeout(timer);ws.removeEventListener('message',receive);}function receive(e){const m=JSON.parse(String(e.data));if(m.type==='error'){cleanup();no(Error(m.error));}else if(predicate(m)){cleanup();yes(m);}}ws.addEventListener('message',receive);if(message)ws.send(JSON.stringify(message));});}
const html=`<!doctype html><style>body{margin:0;background:#111}iframe{width:640px;height:480px;border:0}</style><script>
window.events=[];window.ready=new Set();window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1'){events.push(e.data);if(e.data.event==='ready')ready.add(e.data.epoch);}});
window.addRuntime=s=>{const f=document.createElement('iframe');f.id='p'+s;f.src='/runtime/th15.html?hosted=1&managedData=1&runtimeEpoch='+(s+1);document.body.append(f);};
window.rpc=(s,command,fields={})=>new Promise((yes,no)=>{const f=document.querySelector('#p'+s),request=crypto.randomUUID(),timer=setTimeout(()=>{removeEventListener('message',receive);no(Error(command+' timeout'));},90000);function receive(e){if(e.source===f.contentWindow&&e.data?.request===request){clearTimeout(timer);removeEventListener('message',receive);e.data.ok?yes(e.data):no(Error(e.data.error));}}addEventListener('message',receive);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th15',epoch:s+1,command,request,...fields},location.origin);});
window.launchRuntime=async s=>{const o=await(await fetch('/options')).json();await rpc(s,'configure',{music:'none',sharedResources:[{path:'/msgothic.ttc',url:'/shared/msgothic.ttc'},{path:'/unifont.otf',url:'/shared/unifont.otf'}],options:o[s]});await rpc(s,'launch');};
</script>`;
const server=createServer(async(req,res)=>{try{const p=new URL(req.url,'http://localhost').pathname;
 if(p==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}if(p==='/favicon.ico'){res.writeHead(204).end();return;}
 if(p==='/data'){res.end(archive);return;}if(p==='/options'){res.setHeader('Content-Type','application/json');res.end(JSON.stringify(options));return;}
 const font=fonts.find(([n])=>p==='/shared/'+n);if(font){res.end(await readFile(font[1]));return;}
 assert.ok(p.startsWith('/runtime/'));const file=resolve(runtime,decodeURIComponent(p.slice(9)));assert.ok(file.startsWith(runtime+sep));res.setHeader('Content-Type',({'.mjs':'text/javascript','.html':'text/html','.json':'application/json','.wasm':'application/wasm','.css':'text/css'})[extname(file)]||'application/octet-stream');res.end(await readFile(file));
 }catch(e){res.writeHead(404).end(String(e));}});
function inputPayload(bytes){const d=bytes.readUInt32LE(24),chapters=bytes.readUInt32LE(28),frames=bytes.readUInt32LE(32);return {frames,chapters:bytes.subarray(40+d,40+d+chapters*8),data:bytes.subarray(40+d+chapters*8)};}
try{
 await wait(()=>/listening ws:\/\/127.0.0.1:(\d+)/.test(relayLog),'relay');const port=Number(/listening ws:\/\/127.0.0.1:(\d+)/.exec(relayLog)[1]);
 await new Promise(r=>server.listen(0,'127.0.0.1',r));const base='http://127.0.0.1:'+server.address().port;
 browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding','--enable-unsafe-swiftshader']});
 for(const [index,c]of cases.entries()){
  const report={...c,passed:false};reports.push(report);const context=await browser.createBrowserContext(),page=await context.newPage(),errors=[],members=[],room='th15mp-stress-'+index;
  console.log('START '+c.name);await page.setViewport({width:1280,height:1000});await page.evaluateOnNewDocument(()=>{globalThis.RTCPeerConnection=undefined;});page.on('pageerror',e=>errors.push(String(e)));
  try{
   for(let s=0;s<c.players;++s){const ws=new WebSocket('ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,lobby:room+'-'+s,member:room+'-private-'+s,intent:s?'join':'create',players:String(c.players),difficulty:String(c.difficulty)}));sockets.push(ws);members.push(ws);await send(ws,null);await send(ws,{type:'take-seat',seat:s,loadout:s===2?3:s},m=>!!m.room?.seats[s]);}
   await send(members[0],{type:'settings',playerCount:c.players,difficulty:c.difficulty},m=>m.room?.difficulty===c.difficulty);
   for(let s=0;s<c.players;++s)await send(members[s],{type:'set-ready',ready:true},m=>m.room?.seats[s]?.ready);
   const start=await send(members[0],{type:'start',adonisMode:c.mode,inputDelay:c.mode===1?2:0,inputDelayAuto:false,predictionReserve:2},m=>m.type==='start');
   options=Array.from({length:c.players},(_,s)=>({netplayMode:'lan',netplayUrl:'ws://127.0.0.1:'+port+'/?'+new URLSearchParams({room,run:String(start.serial),member:room+'-private-'+s}),netplayPlayer:s,netplayPlayerCount:c.players,netplayDifficulty:c.difficulty,netplaySeed:1234,netplayLoadouts:Array.from({length:c.players},(_,i)=>({character:i===2?3:i,shot:0})),netplayInputDelay:c.mode===1?2:0,netplayInputDelayAuto:false,netplayAdonisMode:c.mode,netplayPredictionLimit:c.mode===1?0:12,netplayPredictionReserve:2,netplayChallengeMode:false,thpracEnabled:false,renderScale:1}));
   await page.goto(base);for(let s=0;s<c.players;++s){await page.evaluate(s=>addRuntime(s),s);await page.waitForFunction(s=>ready.has(s+1),{timeout:60000},s);await page.evaluate(s=>launchRuntime(s),s);}
   const frames=()=>Array.from({length:c.players},(_,s)=>page.frames().find(f=>f.url().includes('runtimeEpoch='+(s+1))));
   const states=()=>Promise.all(frames().map(f=>f.evaluate(()=>{const p=Module._th15_mp_status()>>>2;return Array.from(Module.HEAPU32.subarray(p,p+24));})));
   await wait(async()=>{assert.deepEqual(errors,[]);const s=await states();assert.ok(s.every(v=>!v[23]),'Native error during warmup');return s.every(v=>v[3]>=180);},'warmup');
   const cdp=await page.createCDPSession();await cdp.send('Emulation.setCPUThrottlingRate',{rate:c.cpu});
   for(const [seat,f]of frames().entries())await f.evaluate(({seat,fault})=>{
    const core=Module,t=__eaglerPeerTransport;globalThis.stress={active:true,frameMs:[],gaps:[],ticks:0,callbacks:0,mem:[],fps:[],samples:[],faults:{seen:0,dropped:0,delivered:0,pending:0,peakPending:0},faultActive:!!fault};
    const original=core.onGameFrame;let last=performance.now(),seed=1234+seat*997;const begin=last;
    core.onGameFrame=function(ok,ms,count){const now=performance.now();if(stress.active){stress.frameMs.push(ms);stress.gaps.push(now-last);stress.ticks+=count;++stress.callbacks;}last=now;return original?.call(this,ok,ms,count);};
    const receive=t.receiveBinary.bind(t);t.receiveBinary=data=>{const m=stress.faults;++m.seen;if(!stress.faultActive){++m.delivered;return receive(data);}if(fault.dropEvery&&m.seen%fault.dropEvery===0){++m.dropped;return;}seed=(Math.imul(seed,1664525)+1013904223)>>>0;let delay=Math.max(0,fault.latency+(seed/4294967296*2-1)*fault.jitter);const phase=(performance.now()-begin)%fault.burstEveryMs;if(fault.burstMs&&phase<fault.burstMs)delay+=fault.burstMs-phase;++m.pending;m.peakPending=Math.max(m.peakPending,m.pending);setTimeout(()=>{--m.pending;++m.delivered;receive(data);},delay);};
    let beat=0;const apply=()=>{const left=(Math.floor(beat/3)+seat)%2===0;core._th15_key(203,left?1:0);core._th15_key(205,left?0:1);core._th15_key(44,beat%16===15?0:1);core._th15_key(45,beat%25===8?1:0);core._th15_key(42,beat%8<4?1:0);++beat;};apply();stress.inputTimer=setInterval(apply,180);
    stress.sampleTimer=setInterval(()=>{if(!stress.active)return;const p=core._th15_mp_status()>>>2,s=Array.from(core.HEAPU32.subarray(p,p+24));stress.mem.push(core.HEAPU8.buffer.byteLength);stress.fps.push(__eaglerNetplayPresentationFps);stress.samples.push({time:performance.now()-begin,frame:s[3],confirmed:s[4],rollback:__eaglerNetplayRollback,heap:core.HEAPU8.buffer.byteLength,phase:core._th15_phase()});},1000);
   },{seat,fault:c.fault||null});
   const startFrames=(await states()).map(v=>v[3]),started=Date.now();
   for(let second=0;second<c.seconds;second+=5){await new Promise(r=>setTimeout(r,Math.min(5,c.seconds-second)*1000));const s=await states();console.log(c.name+' '+(second+5)+'s frames='+s.map(v=>v[3]).join('/'));assert.deepEqual(errors,[]);assert.ok(s.every(v=>!v[23]),'Native error during measurement');}
   const elapsed=Date.now()-started;report.observedSeconds=elapsed/1000;
   report.measured=await Promise.all(frames().map((f,seat)=>f.evaluate(({seat,start,elapsed})=>{stress.active=false;stress.faultActive=false;clearInterval(stress.inputTimer);clearInterval(stress.sampleTimer);Module._th15_keys_clear();const stats=a=>{a=[...a].sort((a,b)=>a-b);return {count:a.length,mean:a.reduce((a,b)=>a+b,0)/Math.max(1,a.length),p50:a[Math.floor(a.length*.5)]||0,p95:a[Math.floor(a.length*.95)]||0,p99:a[Math.floor(a.length*.99)]||0,max:a.at(-1)||0};};const p=Module._th15_mp_status()>>>2,s=Array.from(Module.HEAPU32.subarray(p,p+24));const gl=Module.canvas.getContext('webgl2'),ext=gl?.getExtension('WEBGL_debug_renderer_info');return {seat,frame:s[3],progressFps:(s[3]-start)*1000/elapsed,nativeCallbackMs:stats(stress.frameMs),callbackGapMs:stats(stress.gaps),visibleFps:stats(stress.fps),heapBytes:stats(stress.mem),simulatedTicks:stress.ticks,rollback:__eaglerNetplayRollback,status:s,samples:stress.samples,faults:stress.faults,renderer:ext?gl.getParameter(ext.UNMASKED_RENDERER_WEBGL):null,error:document.querySelector('#error').textContent};},{seat,start:startFrames[seat],elapsed})));
   await cdp.send('Emulation.setCPUThrottlingRate',{rate:1});
   // Neutral input and clean delivery let all peers confirm the recorded prefix.
   const endState=await states();report.naturalRunEnd=endState.every(v=>v[13]===1);const frontier=Math.max(...endState.map(v=>v[3]));if(report.naturalRunEnd)assert.ok(endState.every(v=>v[4]>=v[3]-1),'Retired before confirmation');else await wait(async()=>{const s=await states();return s.every(v=>v[4]!==0xffffffff&&v[4]>frontier+30);},'post-fault confirmation',30000);
   await page.screenshot({path:resolve(out,c.name+'.png'),fullPage:true});
   const archives=[];for(const [seat,f]of frames().entries()){const bytes=Buffer.from(await f.evaluate(()=>{if(!Module._th15_mp_replay_save())throw Error('Save failed');return Array.from(Module.FS.readFile('/save/replay/th15mp_01.rpy'));}));await writeFile(resolve(out,c.name+'-p'+seat+'.rpy'),bytes);archives.push(inputPayload(bytes));}
   const commonFrames=Math.min(...archives.map(v=>v.frames)),size=commonFrames*c.players*12;
   assert.ok(commonFrames>180);for(const a of archives)assert.ok(a.data.subarray(0,size).equals(archives[0].data.subarray(0,size)),'Confirmed replay inputs disagree');
   report.agreement={confirmedFrames:commonFrames,inputSha256:sha(archives[0].data.subarray(0,size)),kind:'all-seat confirmed input bytes; native world consistency is checked separately'};
   assert.deepEqual(errors,[]);assert.ok(report.measured.every(v=>!v.error&&v.progressFps>0));report.passed=true;
  }catch(e){report.error=e.stack;console.error(c.name+': '+e.stack);await page.screenshot({path:resolve(out,c.name+'-error.png'),fullPage:true}).catch(()=>{});}
  finally{for(const ws of members)ws.close();await context.close();await writeFile(resolve(out,'report.json'),JSON.stringify({reports},null,2));}
  console.log('RESULT '+JSON.stringify({...report,measured:report.measured?.map(({samples,...v})=>v)}));
 }
 if(reports.some(r=>!r.passed))process.exitCode=1;
}finally{await browser?.close();for(const ws of sockets)ws.close();relay.kill();server.closeAllConnections();server.close();await writeFile(resolve(out,'relay.log'),relayLog);}
