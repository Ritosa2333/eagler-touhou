// Deterministic full-application workloads through BrowserRuntime's real MP loop.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,writeFile,mkdir} from 'node:fs/promises';
import {resolve,extname,sep} from 'node:path';
import {createRequire} from 'node:module';
import {createHash} from 'node:crypto';
import {cpus,totalmem,release} from 'node:os';
const repo=resolve(import.meta.dirname,'../..'),root=resolve(repo,'ports/th15-mp');
const launcher=resolve(repo,'launcher'),puppeteer=createRequire(resolve(launcher,'package.json'))('puppeteer-core');
const {findChromiumExecutable}=await import('../../launcher/lib/chromium-executable.mjs');
const build=resolve(process.env.TH15_PROFILE_BUILD||resolve(root,'web/artifacts/rollback-performance'));
const runtime=resolve(process.env.TH15_TEST_RUNTIME||resolve(root,'build-eagler-multiplayer'));
const out=resolve(process.argv.find(v=>v.startsWith('--output='))?.slice(9)||resolve(repo,'artifacts/th15mp-performance/current'));
const samples=Number(process.env.TH15_PROFILE_FRAMES||600),requireBudget=process.argv.includes('--require-budget');
const sha=b=>createHash('sha256').update(b).digest('hex');await mkdir(out,{recursive:true});
const archive=await readFile(process.env.TH15_TEST_ARCHIVE);
const fontFiles=[['msgothic.ttc',process.env.TH15_TEST_FONT],['unifont.otf',process.env.TH15_TEST_UNICODE_FONT]];
const cases=process.env.TH15_PROFILE_CASES?JSON.parse(process.env.TH15_PROFILE_CASES):[...[1,7].flatMap(stage=>[2,3].flatMap(players=>[
 {stage,players,depth:5,mismatch:false,cpu:4},{stage,players,depth:5,mismatch:true,cpu:2},
])),{stage:7,players:3,depth:5,mismatch:false,cpu:4,dense:true},{stage:7,players:3,depth:5,mismatch:true,cpu:2,dense:true}];
assert.ok(Number.isInteger(samples)&&samples>=120,'Measure at least 120 advancing frames');
assert.ok(cases.length&&cases.every(c=>[1,7].includes(c.stage)&&[2,3].includes(c.players)&&Number.isInteger(c.depth)&&c.depth>=1&&c.depth<=12&&Number.isFinite(c.cpu)&&c.cpu>=1&&typeof c.mismatch==='boolean'&&(c.warmup===undefined||(Number.isInteger(c.warmup)&&c.warmup>=0))),'Invalid rollback workload');
const html=`<!doctype html><script>
window.ready=false;window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
addEventListener('message',e=>{if(e.data?.event==='ready')ready=true;});
window.rpc=(command,fields={})=>new Promise((yes,no)=>{const request=crypto.randomUUID(),timer=setTimeout(()=>no(Error(command+' timeout')),90000);function receive(e){if(e.data?.request!==request)return;clearTimeout(timer);removeEventListener('message',receive);e.data.ok?yes(e.data):no(Error(e.data.error));}addEventListener('message',receive);document.querySelector('iframe').contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th15',epoch:1,request,command,...fields},location.origin);});
</script><iframe src="/runtime/th15.html?hosted=1&managedData=1&runtimeEpoch=1" width="640" height="480"></iframe>`;
const server=createServer(async(req,res)=>{try{
 const p=new URL(req.url,'http://localhost').pathname;
 if(p==='/'){res.setHeader('Content-Type','text/html');res.end(html);return;}
 if(p==='/data'){res.end(archive);return;}
 const font=fontFiles.find(([name])=>p==='/'+name);if(font){res.end(await readFile(font[1]));return;}
 if(p==='/favicon.ico'){res.writeHead(204).end();return;}
 assert.ok(p.startsWith('/runtime/'));const name=decodeURIComponent(p.slice(9));
 let file=resolve(runtime,name);assert.ok(file.startsWith(runtime+sep));
 if(name==='th15.mjs')file=resolve(build,'th15-application.mjs');
 if(name==='th15-application.wasm')file=resolve(build,name);
 res.setHeader('Content-Type',({'.js':'text/javascript','.mjs':'text/javascript','.html':'text/html','.json':'application/json','.wasm':'application/wasm','.css':'text/css'})[extname(file)]||'application/octet-stream');res.end(await readFile(file));
 }catch(e){res.writeHead(404).end(String(e));}});
const report={identity:{archive:sha(archive),fonts:await Promise.all(fontFiles.map(async([name,file])=>{const bytes=await readFile(file);return {kind:name==='msgothic.ttc'?'vanilla':'unicode',bytes:bytes.length,sha256:sha(bytes)};})),wasm:sha(await readFile(resolve(build,'th15-application.wasm'))),build:JSON.parse(await readFile(resolve(build,'build.json'),'utf8')),driver:sha(await readFile(import.meta.filename)),renderScale:2},environment:{cpu:cpus()[0].model,memoryBytes:totalmem(),platform:process.platform,osRelease:release()},cases:[]};
let browser;
async function openFixture(c){
 const context=await browser.createBrowserContext(),page=await context.newPage(),errors=[];
 page.on('pageerror',e=>errors.push(String(e)));
 await page.goto('http://127.0.0.1:'+server.address().port);await page.waitForFunction(()=>ready,{timeout:90000});
 await page.evaluate(async()=>{await rpc('configure',{music:'none',sharedResources:[{path:'/msgothic.ttc',url:'/msgothic.ttc'},{path:'/unifont.otf',url:'/unifont.otf'}],options:{thpracEnabled:false,renderScale:2,replayViewer:true}});await rpc('launch');});
 const frame=page.frames().find(f=>f.url().includes('runtimeEpoch=1'));
 await frame.evaluate(c=>{
  const work=depth=>{const p=Module._th15_probe_rollback_work(depth,c.mismatch?1:0);if(!p||!new Float64Array(Module.HEAPU8.buffer,p,15)[11])throw Error('Fixture step failed');};
  Module._th15_loop_stop();if(!Module._th15_probe_rollback_prepare(c.players,c.stage,c.dense?1:0))throw Error('Fixture setup failed');
  for(let i=0;i<(c.warmup??240);++i)work(0);for(let i=0;i<32;++i)work(c.depth);
 },c);
 return {context,page,frame,errors};
}
async function endpoint(frame,c,steps=0){
 const state=await frame.evaluate(async({c,steps})=>{
  for(let i=0;i<=steps;++i){if(i%32===0)await new Promise(requestAnimationFrame);const p=Module._th15_probe_rollback_work(0,c.mismatch?1:0);if(!p||!new Float64Array(Module.HEAPU8.buffer,p,15)[11])throw Error('Confirmation drain failed');}
  const p=Module._th15_probe_rollback_fingerprint?.();
  if(!Module._th15_mp_replay_save())throw Error('Confirmed Replay save failed');
  return {fingerprint:p?Array.from(Module.HEAPU32.subarray(p>>>2,(p>>>2)+2)):null,replay:Array.from(Module.FS.readFile('/save/replay/th15mp_01.rpy'))};
 },{c,steps});
 const bytes=Buffer.from(state.replay),offset=40+bytes.readUInt32LE(24)+bytes.readUInt32LE(28)*8;
 const canvas=await frame.$('canvas'),image=await canvas.screenshot();
 return {fingerprint:state.fingerprint,replayInputs:sha(bytes.subarray(offset)),confirmedFrames:bytes.readUInt32LE(32),image:sha(image)};
}
async function trace(frame){return frame.evaluate(()=>{const p=Module._th15_probe_rollback_trace();return JSON.parse(new TextDecoder().decode(Module.HEAPU8.subarray(p,Module.HEAPU8.indexOf(0,p))));});}
try{
 await new Promise(r=>server.listen(0,'127.0.0.1',r));
 browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding','--enable-unsafe-swiftshader']});
 report.browser=await browser.version();
 for(const c of cases){
  const name=`${c.players}p-stage${c.stage}-${c.mismatch?'rollback'+c.depth:'journal'}-cpu${c.cpu}${c.dense?'-dense':''}${c.warmup?'-from'+c.warmup:''}`;
  console.log('START '+name);const {context,page,frame,errors}=await openFixture(c);
  try{
   const cdp=await page.createCDPSession();await cdp.send('Emulation.setCPUThrottlingRate',{rate:c.cpu});
   if(c.profile){await cdp.send('Profiler.enable');await cdp.send('Profiler.start');}
   const measured=await frame.evaluate(async({c,samples})=>{
    const rows=[];for(let i=0;i<samples;++i){await new Promise(requestAnimationFrame);const p=Module._th15_probe_rollback_work(c.depth,c.mismatch?1:0);if(!p)throw Error('Workload failed');const v=Array.from(new Float64Array(Module.HEAPU8.buffer,p,15));if(!v[11])throw Error('Native step failed');if(Module._th15_probe_capture_parts)v.push(...new Float64Array(Module.HEAPU8.buffer,Module._th15_probe_capture_parts(),5));rows.push(v);}
    const stats=a=>{if(!a.length)return {count:0};a.sort((a,b)=>a-b);return {count:a.length,mean:a.reduce((a,b)=>a+b,0)/a.length,p50:a[Math.floor(a.length*.5)],p95:a[Math.floor(a.length*.95)],p99:a[Math.floor(a.length*.99)],max:a.at(-1)};};
    const gl=Module.canvas.getContext('webgl2'),ext=gl.getExtension('WEBGL_debug_renderer_info');
    return {rows,workMs:stats(rows.map(v=>v[0])),captureMs:stats(rows.map(v=>v[12])),capturePartsMs:rows[0].length>15?Object.fromEntries(['animations','playersRecordsCallbacks','enemiesLasers','bullets','items'].map((name,i)=>[name,stats(rows.map(v=>v[15+i]))])):null,restoreMs:stats(rows.filter(v=>v[2]).map(v=>v[13])),heapBytes:Module.HEAPU8.length,renderer:ext?gl.getParameter(ext.UNMASKED_RENDERER_WEBGL):null};
   },{c,samples});
   if(c.profile){const {profile}=await cdp.send('Profiler.stop');await writeFile(resolve(out,name+'.cpuprofile'),JSON.stringify(profile));}
   await cdp.send('Emulation.setCPUThrottlingRate',{rate:1});
   assert.deepEqual(errors,[]);assert.ok(measured.rows.every(v=>v[1]===1&&v[7]===1&&v[8]===1&&v[14]>0),'Workload must advance, draw and capture each frame');
   assert.ok(measured.rows.every(v=>c.mismatch?v[2]===1&&v[3]===c.depth&&v[4]===c.depth:v[2]===0&&v[3]===0),'Wrong correction workload');
   const depths={};for(const v of measured.rows)if(v[2])depths[v[4]]=(depths[v[4]]||0)+1;
   const {rows,...summary}=measured,result={...c,name,...summary,depths,frameStart:rows[0][5]-1,frameEnd:rows.at(-1)[5],corrections:rows.reduce((n,v)=>n+v[2],0),replayedFrames:rows.reduce((n,v)=>n+v[3],0),loggedFrames:rows.reduce((n,v)=>n+v[14],0),maxBullets:Math.max(...rows.map(v=>v[9])),maxAnimationNodes:Math.max(...rows.map(v=>v[10])),passed:measured.workMs.p95<16.67};
   result.endpoint=await endpoint(frame,c);
   if(result.endpoint.fingerprint){
    const oracle=await openFixture({...c,depth:0});
    try{
     result.oracle=await endpoint(oracle.frame,c,samples);assert.deepEqual(oracle.errors,[]);
     if(JSON.stringify(result.endpoint)!==JSON.stringify(result.oracle)){
      const actual=await trace(frame),expected=await trace(oracle.frame);await writeFile(resolve(out,name+'-difference.json'),JSON.stringify({actual,expected}));
      const at=actual.findIndex((v,i)=>v[0]!==expected[i]?.[0]||v[1]!==expected[i]?.[1]);console.error('First state difference: '+JSON.stringify({at,actual:actual.slice(Math.max(0,at-3),at+3),expected:expected.slice(Math.max(0,at-3),at+3)}));
     }
     assert.deepEqual(result.endpoint,result.oracle,'Corrected world, confirmed inputs or canvas differs from complete-input execution');
    }
    finally{await oracle.context.close();}
   }else assert.ok(!requireBudget,'Acceptance requires the complete-input oracle');
   report.cases.push(result);await writeFile(resolve(out,name+'-samples.json'),JSON.stringify(rows));
   await page.screenshot({path:resolve(out,name+'.png')});console.log(JSON.stringify(result));
  }finally{await context.close();await writeFile(resolve(out,'report.json'),JSON.stringify(report,null,2)+'\n');}
 }
 if(requireBudget)assert.ok(report.cases.every(c=>c.passed),'16.67ms P95 budget exceeded');
}finally{await browser?.close();server.closeAllConnections();server.close();}
