import {test} from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,cp,writeFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {pathToFileURL} from 'node:url';

test('managed spectator starts, draws confirmed frames and stops its replica',async()=>{
  const root=await mkdtemp(join(tmpdir(),'pc98-watch-'));
  const saved=Object.fromEntries(['document','fetch','Worker','BroadcastChannel','__eaglerPC98FileCommands'].map(k=>[k,globalThis[k]]));
  let channel,worker,draws=0,removed=false;const sent=[];
  try{
    await cp(new URL('../browser/pc98-managed-spectator.mjs',import.meta.url),join(root,'pc98-managed-spectator.mjs'));
    await cp(new URL('../browser/pc98-local-spectator.mjs',import.meta.url),join(root,'pc98-local-spectator.mjs'));
    await cp(new URL('../../ports/th02/web/replay/',import.meta.url),join(root,'replay'),{recursive:true});
    await writeFile(join(root,'package.json'),'{"type":"module"}');
    globalThis.document={body:{append(){}},createElement:()=>({style:{},getContext:()=>({drawImage(){draws++;}}),remove(){removed=true;}})};
    globalThis.fetch=async()=>new Response('runtime');
    globalThis.BroadcastChannel=class {constructor(name){channel=this;this.name=name;}postMessage(data){sent.push(data);}close(){}};
    globalThis.Worker=class {constructor(){worker=this;this.frame=0;}postMessage(data){if(data.type==='frame-ack')return;
      queueMicrotask(()=>{if(data.type==='execute')this.frame++;if(data.type==='present')this.onmessage({data:{type:'frame',bitmap:{close(){}},sequence:1}});
        this.onmessage({data:{id:data.id,result:data.type==='load'?{}:undefined,status:{proof:'ok'}}});});}terminate(){this.terminated=true;}};
    const {createManagedSpectator}=await import(pathToFileURL(join(root,'pc98-managed-spectator.mjs')));
    const {fingerprint,VERSION}=await import(pathToFileURL(join(root,'replay/tape.js')));
    const {SOURCE_PATHS}=await import(pathToFileURL(join(root,'replay/profile.js')));
    const errors=[],api=createManagedSpectator('02',event=>errors.push(event));
    await api.start({options:{netplayUrl:'ws://localhost/?room=th02mp-1234&run=1'},music:'none'});
    const header={version:VERSION,game:'02',adapter:'confirmed-worker/1',players:2,hostSlot:0,tickMs:10,settings:{},identity:await fingerprint(SOURCE_PATHS),opening:{}};
    const row={inputs:[32,0],offlineMask:0,proof:'ok',hash:null};
    channel.onmessage({data:{type:'slice',viewer:sent[0].viewer,header,first:0,rows:[row],available:1,end:null}});
    await new Promise(resolve=>setTimeout(resolve,100));
    assert.equal(api.health().gameFrame,1);assert(draws>=1);assert.deepEqual(errors,[]);
    api.stop();assert.equal(worker.terminated,true);assert.equal(removed,true);
  }finally{Object.assign(globalThis,saved);await rm(root,{recursive:true,force:true});}
});
