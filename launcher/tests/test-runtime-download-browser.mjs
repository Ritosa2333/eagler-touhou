// Real HTTP caching, native streams and Runtime workers. No game/retail data.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {createHash} from 'node:crypto';
import {mkdir, writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {build} from 'esbuild';
import {loadPlaywright} from './support/playwright-loader.mjs';
import {authoredSourcesPlugin} from './react-main/authored-sources.mjs';
import {buildRuntimeProgramWorkerSource} from '../lib/frontend-assets.mjs';
import {canonicalRuntimePayload, RUNTIME_MANIFEST_SCHEMA, RUNTIME_GENERATION_SCHEMA, RUNTIME_PROTOCOL} from '../lib/contracts/runtime-generations.mjs';

const project=resolve(import.meta.dirname,'..'),mount='/download/',root='runtime/th10/';
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const bodies={
  'game.html':'<!doctype html><script type="module" src="./game.mjs"></script>',
  'game.mjs':`const body=await(await fetch('./game.wasm')).text();parent.postMessage({body,controlled:!!navigator.serviceWorker.controller},'*');`,
  'game.wasm':'verified wasm fixture '.repeat(16384),
};
const files=Object.entries(bodies).map(([path,body])=>({path,bytes:Buffer.byteLength(body),sha256:hash(body)}));
const descriptor={entry:'game.html',files,generation:hash(canonicalRuntimePayload('game.html',files))};
const directory=root+descriptor.generation+'/';
const manifest={schema:RUNTIME_MANIFEST_SCHEMA,protocol:RUNTIME_PROTOCOL,groups:[{root,current:descriptor,previous:[]}]};
const worker=await buildRuntimeProgramWorkerSource({project});
const api=await build({absWorkingDir:project,stdin:{resolveDir:project,contents:`
 export {prepareRuntimeLaunch,runtimeProgramWorker} from './src/launcher/runtime-launch.mts';
 export {createNetworkActivityTracker} from './src/launcher/network-activity.mts';
 export {linkAbortSignals} from './app/services/abort-signals.ts';
`},bundle:true,write:false,format:'esm',platform:'browser',plugins:[authoredSourcesPlugin(project)]});
const requests=[];let hold=false,release,poison=false;
const server=createServer(async(req,res)=>{
  const path=new URL(req.url,'http://localhost').pathname.slice(mount.length);requests.push(path);
  res.setHeader('Cache-Control',path.startsWith(directory)?'public, max-age=31536000, immutable':'no-store');
  res.setHeader('Content-Type',path.endsWith('.mjs')||path.endsWith('-sw.js')?'text/javascript':path.endsWith('.json')?'application/json':path.endsWith('.wasm')?'application/wasm':'text/html');
  if(path===''){res.end('<!doctype html><iframe id="game"></iframe>');return;}
  if(path==='api.mjs'){res.end(api.outputFiles[0].contents);return;}
  if(path==='runtime-program-sw.js'){res.end(worker);return;}
  if(path==='runtime-manifest.json'){res.end(JSON.stringify(manifest));return;}
  if(path===directory+'runtime-generation.json'){res.end(JSON.stringify({schema:RUNTIME_GENERATION_SCHEMA,protocol:RUNTIME_PROTOCOL,root,...descriptor}));return;}
  const name=path.slice(directory.length),body=path.startsWith(directory)?bodies[name]:undefined;
  if(body!==undefined){
    const data=Buffer.from(poison&&name==='game.wasm'?'damaged cached wasm':body);
    res.setHeader('Content-Length',data.length);
    if(hold&&name==='game.wasm'){
      res.write(data.subarray(0,65536));await new Promise(resolve=>{release=resolve;});res.end(data.subarray(65536));
    }else res.end(data);
    return;
  }
  res.writeHead(404).end('Not found');
});
await new Promise(resolve=>server.listen(0,'127.0.0.1',resolve));
const base='http://127.0.0.1:'+server.address().port+mount,playwright=await loadPlaywright({preferPython:process.argv.includes('--python-driver')}),report=[];
const engines=process.argv.find(arg=>arg.startsWith('--engine='))?.slice(9).split(',')??['chromium','firefox','webkit'];
async function held(){for(let i=0;!release&&i<400;i++)await new Promise(resolve=>setTimeout(resolve,10));assert.ok(release,'stream reached its held body');}
try{
 for(const engine of engines){
  const browser=await playwright[engine].launch({headless:true});
  async function fixture(){
    const context=await browser.newContext();
    await context.addInitScript(()=>Object.defineProperty(AbortSignal,'any',{configurable:true,value:undefined}));
    const page=await context.newPage(),errors=[];page.on('pageerror',error=>errors.push(error.message));
    await page.goto(base);
    await page.evaluate(async()=>{
      window.api=await import('./api.mjs');window.events=[];window.progress=[];window.states=[];
      window.tracker=api.createNetworkActivityTracker({onChange:state=>states.push(state)});
      const a=new AbortController(),b=new AbortController(),linked=api.linkAbortSignals(a.signal,b.signal);
      b.abort('test cancellation');if(!linked.signal.aborted||linked.signal.reason!=='test cancellation')throw Error('Chrome 108 signal regression');
      addEventListener('message',event=>{if(event.source===document.querySelector('iframe').contentWindow)events.push(event.data);});
    });
    return {context,page,errors};
  }
  async function start(page,useWorker){
    await page.evaluate(({entry,useWorker})=>{
      window.state='pending';window.preparing=api.prepareRuntimeLaunch(entry,{worker:useWorker==='late'?window.lateWorker:useWorker?window.worker:null,
        fetchImpl:tracker.fetch,onProgress:value=>progress.push(value)}).then(value=>{window.prepared=value;window.state='ready';return value;});
    },{entry:root+'game.html',useWorker});
  }
  async function navigate(page,controlled){
    await page.evaluate(()=>{events.length=0;document.querySelector('iframe').src=prepared.url;});
    await page.waitForFunction(()=>events.length>0);
    assert.deepEqual(await page.evaluate(()=>events[0]),{body:bodies['game.wasm'],controlled});
  }
  try{
    // Establish native delivery independently. Some Firefox builds keep this
    // fetch pending until EOF even with a flushed partial HTTP response.
    const native=await fixture();hold=true;release=null;let streamsPartially;
    try{
      await native.page.evaluate(path=>{
        window.nativePhase='pending';window.nativeRead=fetch(path+'?native-probe=1',{cache:'reload'}).then(async response=>{
          window.nativePhase='headers';const reader=response.body.getReader();await reader.read();window.nativePhase='partial';
          while(!(await reader.read()).done){};
        });
      },directory+'game.wasm');
      await held();await native.page.waitForTimeout(700);
      streamsPartially=await native.page.evaluate(()=>nativePhase==='partial');
      hold=false;release();await native.page.evaluate(()=>nativeRead);
    }finally{hold=false;release?.();await native.context.close();}
    const {context,page,errors}=await fixture();requests.length=0;hold=true;release=null;
    try{
      await page.evaluate(()=>{window.lateWorker=new Promise(resolve=>{window.resolveWorker=resolve;});});
      await start(page,'late');await held();
      if(streamsPartially)await page.waitForFunction(()=>states.some(state=>state.active.some(task=>task.url.endsWith('game.wasm')&&task.loaded>0)));
      assert.equal(await page.evaluate(()=>tracker.activeCount),1,'native body remains tracked after response headers');
      assert.equal(await page.evaluate(()=>state),'pending');
      await page.evaluate(async()=>{window.resolveWorker(await api.runtimeProgramWorker(location.href));});
      hold=false;release();await page.evaluate(()=>preparing);
      assert.equal(await page.evaluate(()=>tracker.activeCount),0);
      assert.equal(await page.evaluate(()=>prepared.cached),true,'late worker commits the verified buffers before navigation');
      const before=requests.filter(path=>files.some(file=>path===directory+file.path));
      assert.equal(before.length,3);await navigate(page,true);
      assert.deepEqual(requests.filter(path=>files.some(file=>path===directory+file.path)),before,'late worker/iframe reuse the verified buffers without another server download');
      assert.deepEqual(errors,[]);report.push({engine,case:'fallback native stream + verified handoff to late worker',nativeDelivery:streamsPartially?'partial':'buffered until release',passed:true});
    }finally{hold=false;release?.();await context.close();}

    const observed=await fixture();requests.length=0;hold=true;release=null;
    try{
      await observed.page.evaluate(async()=>{window.worker=await api.runtimeProgramWorker(location.href);});
      await start(observed.page,true);await held();
      await observed.page.waitForFunction(()=>progress.some(value=>value.path.endsWith('game.wasm')&&value.phase==='requesting'));
      if(streamsPartially)await observed.page.waitForFunction(()=>progress.some(value=>value.path.endsWith('game.wasm')&&value.phase==='receiving'&&value.loaded>0&&value.loaded<value.total));
      assert.equal(await observed.page.evaluate(()=>state),'pending','worker progress must not acknowledge a complete program');
      hold=false;release();await observed.page.evaluate(()=>preparing);await navigate(observed.page,true);
      assert.equal(await observed.page.evaluate(()=>progress.filter(value=>value.path.endsWith('game.wasm')).at(-1).phase),'complete');
      assert.equal(requests.filter(path=>path===directory+'game.wasm').length,1);
      assert.deepEqual(observed.errors,[]);report.push({engine,case:'worker request + partial bytes + real completion',passed:true});
    }finally{hold=false;release?.();await observed.context.close();}

    const damaged=await fixture();requests.length=0;
    try{
      poison=true;await damaged.page.evaluate(async path=>{await(await fetch(path)).arrayBuffer();},directory+'game.wasm');poison=false;
      await damaged.page.evaluate(async()=>{window.worker=await api.runtimeProgramWorker(location.href);});
      await start(damaged.page,true);await damaged.page.evaluate(()=>preparing);await navigate(damaged.page,true);
      const rejectedCachedBytes=await damaged.page.evaluate(()=>progress.some(value=>value.path.endsWith('game.wasm')&&value.phase==='failed'));
      assert.equal(requests.filter(path=>path===directory+'game.wasm').length,2,'one poisoned cache seed plus one fresh repair');
      assert.deepEqual(damaged.errors,[]);report.push({engine,case:'poisoned page HTTP cache cannot replace verified worker bytes',rejectedCachedBytes,passed:true});
    }finally{poison=false;await damaged.context.close();}
  }finally{await browser.close();}
 }
 const out=resolve(project,'artifacts/validation/runtime-download');await mkdir(out,{recursive:true});
 await writeFile(resolve(out,'report.json'),JSON.stringify({passed:true,report},null,2));console.log(JSON.stringify({passed:true,report}));
}finally{hold=false;release?.();server.closeAllConnections();await new Promise(resolve=>server.close(resolve));}
