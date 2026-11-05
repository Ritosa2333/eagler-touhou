// Retail-resource memory workloads through TH15's application correction loop.
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {createServer} from 'node:http';
import {createRequire} from 'node:module';
import {mkdir,readFile,writeFile} from 'node:fs/promises';
import {extname,resolve,sep} from 'node:path';
import {pathToFileURL} from 'node:url';

const repo=resolve(import.meta.dirname,'../..');
const launcher=resolve(process.env.TH15_LAUNCHER_ROOT||resolve(repo,'launcher'));
const puppeteer=createRequire(resolve(launcher,'package.json'))('puppeteer-core');
const {findChromiumExecutable}=await import(pathToFileURL(resolve(launcher,'lib/chromium-executable.mjs')));
const build=resolve(process.env.TH15_PROFILE_BUILD);
const runtime=resolve(process.env.TH15_TEST_RUNTIME);
const output=resolve(process.env.TH15_MEMORY_OUTPUT);
const samples=Number(process.env.TH15_MEMORY_FRAMES||6000);
const continuous=process.env.TH15_MEMORY_CONTINUOUS==='1';
const cases=JSON.parse(process.env.TH15_MEMORY_CASES||JSON.stringify([2,3].flatMap(players=>[4,5,6].map(stage=>({players,stage,depth:5})))));
assert(Number.isInteger(samples)&&samples>=120);
assert(cases.length&&cases.every(c=>[2,3].includes(c.players)&&c.stage>=1&&c.stage<=7&&c.depth>=0&&c.depth<=12));
await mkdir(output,{recursive:true});
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const archive=await readFile(process.env.TH15_TEST_ARCHIVE);
const fonts=new Map(await Promise.all([['msgothic.ttc',process.env.TH15_TEST_FONT],['unifont.otf',process.env.TH15_TEST_UNICODE_FONT]].map(async([name,path])=>[name,await readFile(path)])));
const html=`<!doctype html><script>
window.ready=false;
window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
addEventListener('message',event=>{if(event.data?.event==='ready')ready=true;});
window.rpc=(command,fields={})=>new Promise((yes,no)=>{
 const request=crypto.randomUUID(),timer=setTimeout(()=>no(Error(command+' timeout')),90000);
 function receive(event){if(event.data?.request!==request)return;clearTimeout(timer);removeEventListener('message',receive);event.data.ok?yes(event.data):no(Error(event.data.error));}
 addEventListener('message',receive);
 document.querySelector('iframe').contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th15',epoch:1,request,command,...fields},location.origin);
});</script><iframe src="/runtime/th15.html?hosted=1&managedData=1&runtimeEpoch=1" width="640" height="480"></iframe>`;
const server=createServer(async(request,response)=>{
 try{
  const path=new URL(request.url,'http://localhost').pathname;
  if(path==='/'){response.setHeader('Content-Type','text/html');response.end(html);return;}
  if(path==='/data'){response.end(archive);return;}
  if(fonts.has(path.slice(1))){response.end(fonts.get(path.slice(1)));return;}
  if(path==='/favicon.ico'){response.writeHead(204).end();return;}
  assert(path.startsWith('/runtime/'));
  const name=decodeURIComponent(path.slice(9));
  let file=resolve(runtime,name);assert(file.startsWith(runtime+sep));
  if(name==='th15.mjs')file=resolve(build,'th15-application.mjs');
  if(name==='th15-application.wasm')file=resolve(build,name);
  response.setHeader('Content-Type',({'.js':'text/javascript','.mjs':'text/javascript','.html':'text/html','.json':'application/json','.wasm':'application/wasm','.css':'text/css'})[extname(file)]||'application/octet-stream');
  response.end(await readFile(file));
 }catch(error){response.writeHead(404).end(String(error));}
});
const report={archive:hash(archive),wasm:hash(await readFile(resolve(build,'th15-application.wasm'))),
 driver:hash(await readFile(import.meta.filename)),continuous,build:JSON.parse(await readFile(resolve(build,'build.json'))),cases:[]};
let browser;
try{
 await new Promise(resolve=>server.listen(0,'127.0.0.1',resolve));
 browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,protocolTimeout:600000,
  args:['--no-proxy-server','--autoplay-policy=no-user-gesture-required','--enable-unsafe-swiftshader','--disable-background-timer-throttling','--disable-renderer-backgrounding']});
 report.browser=await browser.version();
 for(const workload of cases){
  const context=await browser.createBrowserContext();
  const page=await context.newPage();const errors=[];
  page.on('pageerror',error=>errors.push(String(error)));
  const result={...workload,samples:[],errors};report.cases.push(result);
  try{
   console.log('START '+JSON.stringify(workload));
   await page.goto('http://127.0.0.1:'+server.address().port);
   await page.waitForFunction(()=>ready,{timeout:90000});
   await page.evaluate(async()=>{await rpc('configure',{music:'none',sharedResources:[{path:'/msgothic.ttc',url:'/msgothic.ttc'},{path:'/unifont.otf',url:'/unifont.otf'}],options:{thpracEnabled:false,renderScale:1,replayViewer:true}});await rpc('launch');});
   const frame=page.frames().find(frame=>frame.url().includes('runtimeEpoch=1'));
   await frame.evaluate(c=>{
    Module._th15_loop_stop();
    if(!Module._th15_probe_rollback_prepare(c.players,c.stage,1))throw Error('Memory fixture setup failed');
    const status=()=>Array.from(Module.HEAPU32.subarray(Module._th15_probe_memory_status()>>>2,(Module._th15_probe_memory_status()>>>2)+6));
    if(status()[3]!==c.stage)throw Error('Wrong retail stage');
    for(let i=0;i<240;++i){const p=Module._th15_probe_rollback_work(0,0);if(!p||!new Float64Array(Module.HEAPU8.buffer,p,15)[11])throw Error('Memory fixture warmup failed');}
    window.memoryWork=depth=>{
     let p=Module._th15_probe_rollback_work(depth,0);
     if(!p)throw Error('Memory workload has no scene');
     let row=new Float64Array(Module.HEAPU8.buffer,p,15);
     if(!row[11])throw Error('Memory workload failed');
     // Real peers continue delivering while a factory barrier waits. Drain the
     // delayed prefix rather than freezing the test's remote-input generator.
     if(row[1]===0){
      if(!Module._th15_probe_rollback_receive(row[5],0))throw Error('Boundary confirmation failed');
      p=Module._th15_probe_rollback_work(0,0);
      if(!p)throw Error('Memory workload lost its boundary scene');
      row=new Float64Array(Module.HEAPU8.buffer,p,15);
      if(!row[11]||row[1]===0)throw Error('Memory workload stopped at a confirmed boundary');
     }
    };
   },workload);
   for(let completed=0;completed<samples;completed+=120){
    const sample=await frame.evaluate(({c,count,continuous})=>{
     if(continuous)Module._th15_probe_memory_protect();
     for(let i=0;i<count;++i){
      if(continuous&&Module.HEAPU32[(Module._th15_probe_memory_status()>>>2)+6])break;
      memoryWork(c.depth);
     }
     const p=Module._th15_probe_memory_status()>>>2;
     return Array.from(Module.HEAPU32.subarray(p,p+8));
    },{c:workload,count:Math.min(120,samples-completed),continuous});
    result.samples.push(sample);
    if(completed%1200===0)console.log(JSON.stringify({players:workload.players,startStage:workload.stage,frame:sample[4],stage:sample[3],usedMiB:sample[0]/1048576,heapMiB:sample[2]/1048576}));
    assert.deepEqual(errors,[]);
    if(continuous&&sample[6])break;
   }
   result.peakAllocatedBytes=Math.max(...result.samples.map(value=>value[0]));
   result.peakHeapBytes=Math.max(...result.samples.map(value=>value[2]));
   result.final=result.samples.at(-1);
   result.stages=[...new Set(result.samples.map(sample=>sample[3]))];
   result.completed=!!result.final[6];
   result.fingerprint=await frame.evaluate(()=>{const p=Module._th15_probe_rollback_fingerprint()>>>2;return p?Array.from(Module.HEAPU32.subarray(p,p+2)):null;});
   const target=resolve(output,`${workload.players}p-stage${workload.stage}.png`);await page.screenshot({path:target});
   assert(result.final[4]>=samples||(continuous&&result.completed));
   if(continuous)assert.deepEqual(result.stages,[1,2,3,4,5,6],'Continuous run must visit all six stages');
   result.passed=true;
   console.log('RESULT '+JSON.stringify({...result,samples:undefined}));
  }catch(error){
   result.error=error.stack;
   await page.screenshot({path:resolve(output,`${workload.players}p-stage${workload.stage}-error.png`)}).catch(()=>{});
   throw error;
  }finally{
   await context.close();
   await writeFile(resolve(output,'report.json'),JSON.stringify(report,null,2)+'\n');
  }
 }
}finally{
 await browser?.close();server.closeAllConnections();server.close();
 await writeFile(resolve(output,'report.json'),JSON.stringify(report,null,2)+'\n');
}
