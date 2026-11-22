import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {createServer} from 'node:http';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const web=new URL('../../../ports/th03/web/',import.meta.url);
const modules=new Set(['native-room-recorder.mjs','native-room-recorder.worker.mjs','managed-storage.mjs','native-replay.mjs','touch-input.js','native-charge-history.mjs']);
const server=createServer(async(req,res)=>{
  const name=req.url.slice(1);
  if(!name){res.setHeader('content-type','text/html');res.end('<!doctype html><title>TH03 recorder test</title>');return;}
  res.setHeader('content-type','text/javascript');
  if(name==='fault-worker.mjs'){
    res.end(`import './native-room-recorder.worker.mjs';
const add=IDBObjectStore.prototype.add;
let rejectNext=true;
IDBObjectStore.prototype.add=function(...args){if(rejectNext){rejectNext=false;throw Error('injected replay write failure');}return add.apply(this,args);};`);
  }else if(modules.has(name))res.end(await readFile(new URL(name,web)));
  else res.writeHead(404).end();
});
await new Promise(resolve=>server.listen(0,'127.0.0.1',resolve));
let browser;
try{
  browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true});
  const page=await browser.newPage();
  await page.goto(`http://127.0.0.1:${server.address().port}/`);
  const actual=await page.evaluate(async()=>{
    const {createRoomRecorder}=await import('/native-room-recorder.mjs');
    const {openUserFiles}=await import('/managed-storage.mjs');
    const {decodeReplay}=await import('/native-replay.mjs');
    const store=await openUserFiles({multiplayer:true}),errors=[];
    const settings={focus:false,difficulty:2,seed:123,memory:store.memory,onError:error=>errors.push(error.message)};
    const recorder=createRoomRecorder(settings);
    const native=new Uint8Array([255,23,42,99]);
    const engine={HEAPU8:native,_th03_classic_replay_size:()=>2,_th03_classic_replay_data:()=>1};
    const wait=async predicate=>{const end=performance.now()+10000;while(!predicate()){if(performance.now()>end)throw Error(JSON.stringify({errors,files:[...store.memory.keys()]}));await new Promise(r=>setTimeout(r,10));}};
    for(let i=0;i<125;i++)recorder.append([i,1]);
    const charge=[[0,0,-1,0],[2,0,-1,4]];
    recorder.append([2**44,9],charge);recorder.save(engine);
    native[1]=55;
    recorder.append([42,10]);recorder.save(engine);recorder.stop();
    await wait(()=>store.memory.size===2);
    const faulty=createRoomRecorder({...settings,createWorker:()=>new Worker('/fault-worker.mjs',{type:'module'})});
    faulty.append([1,2]);faulty.save(engine);
    await wait(()=>errors.length===1);
    faulty.append([3,4]);faulty.save(engine);faulty.stop();
    await wait(()=>store.memory.size===3);
    const reopened=await openUserFiles({multiplayer:true}),single=await openUserFiles();
    const files=[];
    for(const [path,bytes] of reopened.memory){
      if(!store.memory.has(path))throw Error('missing live cache entry');
      const replay=await decodeReplay(bytes);
      files.push({frames:replay.rows.length,last:replay.rows.at(-1),first:replay.rows[0],native:replay.native,focus:replay.focus,difficulty:replay.difficulty,seed:replay.seed});
    }
    const singleFiles=single.memory.size;single.close();reopened.close();store.close();
    return {errors,singleFiles,files:files.sort((a,b)=>a.frames-b.frames)};
  });
  assert.deepEqual(actual,{errors:['injected replay write failure'],singleFiles:0,files:[
    {frames:2,first:[1,2],last:[3,4],native:[55,42],focus:false,difficulty:2,seed:123},
    {frames:126,first:[0,1],last:[2**44,9,[0,0,-1,0],[2,0,-1,4]],native:[23,42],focus:false,difficulty:2,seed:123},
    {frames:127,first:[0,1],last:[42,10],native:[55,42],focus:false,difficulty:2,seed:123},
  ]});
  console.log('TH03 worker archives consecutive confirmed prefixes, survives write failure and preserves legacy replay decoding');
}finally{await browser?.close();await new Promise(resolve=>server.close(resolve));}
