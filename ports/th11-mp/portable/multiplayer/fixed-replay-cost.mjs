// Diagnostic CPU attribution, not a gameplay smoothness acceptance test.
// Executes one frozen real Replay through the matching production Runtime.
// Natural-rAF and manually supplied 60Hz-rAF timestamps use the SAME loop;
// the manual lane deliberately removes inter-callback scheduling/transport.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {createReadStream,readFileSync,writeFileSync,mkdirSync,existsSync} from 'node:fs';
import {resolve,extname,relative,isAbsolute,sep} from 'node:path';
import {createRequire} from 'node:module';
import {createHash} from 'node:crypto';
import {spawnSync} from 'node:child_process';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'../..');
const option=(name,fallback)=>{const at=process.argv.indexOf(name);return at<0?fallback:process.argv[at+1];};
const runtime=resolve(option('--runtime','artifacts/rollback-smoothness/20261014c-release29-input-send/runtime'));
const sourceReplay=resolve(option('--replay','artifacts/rollback-smoothness/20261014c-r29-input-send-4x/peer-0-actual-inputs.rpy'));
let replay=sourceReplay;
const out=resolve(option('--output','artifacts/rollback-smoothness/20261014d-fixed-replay-01'));
const cpu=Number(option('--cpu','4')),until=Number(option('--frame','500'));
const profile=process.argv.includes('--profile');
assert.ok([1,4].includes(cpu)&&until>=200&&until<=2000);
assert.ok(!existsSync(resolve(out,'receipt.json')),'Preserve every previous run');mkdirSync(out,{recursive:true});
const archive=resolve(root,'../../th11-eagler/[th11] 东方地灵殿 (汉化版+日文版)/th11.dat');
const sha=b=>createHash('sha256').update(b).digest('hex');
const inventory=JSON.parse(readFileSync(resolve(runtime,'runtime-files.json')));
let derived=null;
if(process.argv.includes('--derive-input-recording')){
 const common=resolve(root,'third_party/eagler-common'),native=resolve(root,'th11_web/cpp/multiplayer');
 const compiler=resolve(root,'../../toolchains/wasi-sdk-34.0-x86_64-windows/bin/clang++.exe'),wasm=resolve(out,'fixed-input-recording.wasm');
 const sources=[resolve(import.meta.dirname,'fixed-input-recording.cpp'),...['ReplayArchive','SessionSetup','InputLanes'].map(n=>resolve(native,n+'.cpp')),
  ...['InputReplay','NetplayProtocol'].map(n=>resolve(common,'src/netplay',n+'.cpp'))];
 const compile=spawnSync(compiler,['--target=wasm32-wasip1','-std=c++17','-O2','-fno-exceptions','-fno-rtti','-DTH11_MULTIPLAYER=1','-ffp-contract=off','-I'+native,'-I'+resolve(common,'include'),...sources,'-Wl,-z,stack-size=1048576','-o',wasm],{encoding:'utf8',windowsHide:true});
 writeFileSync(resolve(out,'recording-compile.log'),compile.stdout+compile.stderr);assert.equal(compile.status,0,compile.stderr);
 const manifest=JSON.parse(readFileSync(resolve(runtime,'manifest.json'))),words=manifest.execution.sha256.slice(0,32).match(/.{8}/g);
 // The new file is visibly labelled derived; never rewrite metadata in-place
 // or disable the product's build/ABI guards just to load a foreign recording.
 replay=resolve(out,'DERIVED-fixed-inputs.rpy');
 // Preserve the real source basename rather than assuming a mounted alias.
 const actualOwner=new WASI({version:'preview1',args:['recording','/source/'+relative(resolve(sourceReplay,'..'),sourceReplay),'/out/DERIVED-fixed-inputs.rpy',...words],preopens:{'/source':resolve(sourceReplay,'..'),'/out':out},returnOnExit:true});
 const instance=await WebAssembly.instantiate(await WebAssembly.compile(readFileSync(wasm)),{wasi_snapshot_preview1:actualOwner.wasiImport});assert.equal(actualOwner.start(instance),0);
 derived={scope:'Synthetic benchmark recording; not candidate live-game evidence',sourceReplay,sourceSha256:sha(readFileSync(sourceReplay)),sha256:sha(readFileSync(replay)),nativeWriterSha256:sha(readFileSync(wasm)),buildWords:words};
}
const files=new Map(Object.keys(inventory.files).map(name=>[name,resolve(runtime,name)]));
for(const [name,file]of files){const rel=relative(runtime,file);assert.ok(!isAbsolute(rel)&&rel!=='..'&&!rel.startsWith('..'+sep));assert.equal(sha(readFileSync(file)),inventory.files[name].sha256);}
files.set('runtime-files.json',resolve(runtime,'runtime-files.json'));
const report={schema:'th11-fixed-replay-cost/1',startedAt:new Date().toISOString(),completedAt:null,passed:false,
  scope:'CPU decomposition only. Manual lane is not real-time FPS; no input, game logic, GPU calls or audio are removed.',runtime,replay,cpu,until,
  inputSha256:sha(readFileSync(replay)),wasmSha256:inventory.files['th11-sdl.wasm'].sha256,derived,profile,runs:[],errors:[]};
const save=()=>writeFileSync(resolve(out,'receipt.json'),JSON.stringify(report,null,2)+'\n');save();
const require=createRequire(resolve(root,'../../eagler-touhou/th11/tools/diagnostics/package.json'));
const {chromium}=require('playwright');let browser,server;
const html=readFileSync(resolve(import.meta.dirname,'browser-host.html'));
const mime={'.mjs':'text/javascript','.js':'text/javascript','.html':'text/html','.wasm':'application/wasm','.css':'text/css','.json':'application/json'};
function stats(a){const sorted=a.slice().sort((a,b)=>a-b);const p=q=>sorted[Math.floor((sorted.length-1)*q)];return {count:a.length,mean:a.reduce((x,y)=>x+y,0)/a.length,p50:p(.5),p95:p(.95),p99:p(.99),max:sorted.at(-1)};}
try{
 server=createServer((req,res)=>{
  const url=new URL(req.url,'http://localhost');
  if(url.pathname==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}
  const file=url.pathname==='/data/th11'?archive:url.pathname==='/input.rpy'?replay:
    url.pathname.startsWith('/runtime/')?files.get(url.pathname.slice(9)):null;
  if(!file){res.writeHead(404);res.end();return;}res.setHeader('Content-Type',mime[extname(file)]||'application/octet-stream');
  createReadStream(file).on('error',e=>res.destroy(e)).pipe(res);
 });server.keepAliveTimeout=65000;server.headersTimeout=70000;
 await new Promise(ok=>server.listen(0,'127.0.0.1',ok));const origin='http://127.0.0.1:'+server.address().port;
 const temp=resolve(out,'browser-temp');mkdirSync(temp,{recursive:true});
 browser=await chromium.launch({executablePath:option('--chrome','C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe'),headless:true,
  env:{...process.env,TEMP:temp,TMP:temp,TMPDIR:temp},args:['--disable-background-timer-throttling','--disable-backgrounding-occluded-windows','--disable-renderer-backgrounding','--autoplay-policy=no-user-gesture-required','--enable-gpu','--use-gl=angle','--use-angle=d3d11']});
 report.browser=browser.version();
 for(const mode of ['natural','manual']){
  const context=await browser.newContext({viewport:{width:660,height:510},serviceWorkers:'block'});
  try{
   const page=await context.newPage(),cdp=await context.newCDPSession(page);
   page.on('pageerror',e=>report.errors.push({mode,error:String(e)}));
   await cdp.send('Emulation.setCPUThrottlingRate',{rate:cpu});await page.goto(origin);await page.evaluate(()=>host.open());
   await page.waitForFunction(()=>host.ready(),null,{timeout:120000});
   await page.evaluate(async()=>{await host.configure({replayViewer:true});await host.launch();});
   await page.evaluate(async({mode,until,profile})=>{
    const w=document.querySelector('iframe').contentWindow,c=w.core;
    const error=()=>{const p=c.th11_mp_error(),h=new Uint8Array(c.memory.buffer);let end=p;while(h[end])++end;return new TextDecoder().decode(h.subarray(p,end));};
    c.th11_loop_stop();const bytes=new Uint8Array(await(await fetch('/input.rpy')).arrayBuffer()),p=c.malloc(bytes.length);
    try{new Uint8Array(c.memory.buffer,p,bytes.length).set(bytes);if(!c.th11_mp_replay_load(p,bytes.length))throw Error(error());}finally{c.free(p);}
    const old=w.Module.onGameFrame;
    if(profile){c.th11_mp_rollback_profile(1);c.th11_audio_pump_profile(1);}
    const b=w.__fixedCost={done:false,rows:[],started:performance.now(),last:null,error:null,frame:0};
    w.Module.onGameFrame=(ok,ms,ticks)=>{
     // One inexpensive counter read; the complete state is copied only at end.
     const frame=new Uint32Array(c.memory.buffer,c.th11_mp_game_status(),56)[4],now=performance.now();
     if(frame>=125)b.rows.push({frame,ms,ticks,at:now,gap:b.last===null?null:now-b.last,
       ...(profile?{profile:Array.from(new Float64Array(c.memory.buffer,c.th11_mp_rollback_profile(-1),16)),audio:Array.from(new Float64Array(c.memory.buffer,c.th11_audio_pump_profile(-1),18))}:{})});b.last=now;b.frame=frame;
     if(!ok||frame>=until){b.error=ok?null:error();b.done=true;b.final=Array.from(new Uint32Array(c.memory.buffer,c.th11_mp_game_status(),56));c.th11_loop_stop();return;}
     old?.(ok,ms,ticks);
    };
    if(mode==='manual'){
     let next=null,sequence=0;w.requestAnimationFrame=callback=>{if(next)throw Error('Unexpected concurrent rAF owner');next=callback;return ++sequence;};
     let time=performance.now();c.th11_loop_start();
     b.advance=()=>{if(b.done)return;const callback=next;next=null;if(!callback)throw Error('Missing production rAF callback');time+=1000/60;callback(time);};
    }else c.th11_loop_start();
   },{mode,until,profile});
   if(mode==='manual'){
    for(let i=0;i<until+100&&!await page.evaluate(()=>document.querySelector('iframe').contentWindow.__fixedCost.done);i+=10)
     await page.evaluate(()=>{const b=document.querySelector('iframe').contentWindow.__fixedCost;for(let n=0;n<10&&!b.done;++n)b.advance();});
   }else await page.waitForFunction(()=>document.querySelector('iframe').contentWindow.__fixedCost.done,null,{timeout:90000,polling:100});
   const data=await page.evaluate(()=>{const b=document.querySelector('iframe').contentWindow.__fixedCost;return {done:b.done,error:b.error,rows:b.rows,final:b.final};});
   assert.ok(data.done&&!data.error,JSON.stringify(data));assert.equal(data.final[4],until);
   writeFileSync(resolve(out,mode+'.json'),JSON.stringify(data)+'\n');
   const rows=data.rows.filter(r=>r.frame>=150&&r.frame<until),one=rows.filter(r=>r.ticks===1);
   const result={mode,wallPerCallback:stats(rows.map(r=>r.ms)),oneTickNative:stats(one.map(r=>r.ms)),gap:stats(rows.filter(r=>r.gap!==null).map(r=>r.gap)),
     ticks:rows.reduce((n,r)=>n+r.ticks,0),nativeTotal:rows.reduce((n,r)=>n+r.ms,0),final:data.final};
   result.nativePerTick=result.nativeTotal/result.ticks;report.runs.push(result);save();console.log(JSON.stringify(result));
   if(profile){
    const aggregate=(key,size)=>Array.from({length:size},(_,i)=>stats(rows.map(r=>r[key][i])));
    result.profile=aggregate('profile',16);result.audio=aggregate('audio',18);save();
   }
  }finally{await context.close();}
 }
 const fields=g=>[...g.slice(1,5),...g.slice(8)];assert.deepEqual(fields(report.runs[0].final),fields(report.runs[1].final));
 assert.equal(report.errors.length,0);report.passed=true;
}catch(e){report.error=e.stack||String(e);process.exitCode=1;}
finally{if(browser)await browser.close();if(server){server.closeAllConnections?.();await new Promise(ok=>server.close(ok));}report.completedAt=new Date().toISOString();save();console.log(resolve(out,'receipt.json'));}
