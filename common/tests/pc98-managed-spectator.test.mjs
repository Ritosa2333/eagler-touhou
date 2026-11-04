import {test} from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,cp,writeFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {pathToFileURL} from 'node:url';

test('managed spectator starts, draws confirmed frames and stops its replica',async()=>{
  const root=await mkdtemp(join(tmpdir(),'pc98-watch-'));
  const saved=Object.fromEntries(['document','fetch','Worker','WebSocket','__eaglerPC98FileCommands'].map(k=>[k,globalThis[k]]));
  let socket,worker,api,draws=0,removed=false;
  try{
    await cp(new URL('../browser/pc98-managed-spectator.mjs',import.meta.url),join(root,'pc98-managed-spectator.mjs'));
    await cp(new URL('../browser/pc98-local-spectator.mjs',import.meta.url),join(root,'pc98-local-spectator.mjs'));
    await cp(new URL('../browser/pc98-spectator-channel.mjs',import.meta.url),join(root,'pc98-spectator-channel.mjs'));
    await cp(new URL('../browser/pc98-spectator-audio.mjs',import.meta.url),join(root,'pc98-spectator-audio.mjs'));
    await cp(new URL('../../ports/th02/web/replay/',import.meta.url),join(root,'replay'),{recursive:true});
    await writeFile(join(root,'package.json'),'{"type":"module"}');
    globalThis.document={body:{append(){}},createElement:()=>({style:{},getContext:()=>({drawImage(){draws++;}}),remove(){removed=true;}})};
    globalThis.fetch=async()=>new Response('runtime');
    globalThis.WebSocket=class {constructor(url){socket=this;this.url=url;}close(){}};
    globalThis.Worker=class {constructor(){worker=this;this.frame=0;}postMessage(data){if(data.type==='frame-ack')return;
      queueMicrotask(()=>{if(data.type==='execute')this.frame++;if(data.type==='present')this.onmessage({data:{type:'frame',bitmap:{close(){}},sequence:1}});
        this.onmessage({data:{id:data.id,result:data.type==='load'?{}:undefined,status:{proof:'ok'}}});});}terminate(){this.terminated=true;}};
    const {createManagedSpectator}=await import(pathToFileURL(join(root,'pc98-managed-spectator.mjs')));
    const {fingerprint,VERSION}=await import(pathToFileURL(join(root,'replay/tape.js')));
    const {SOURCE_PATHS}=await import(pathToFileURL(join(root,'replay/profile.js')));
    const errors=[];api=createManagedSpectator('02',event=>{if(event.event==='error')errors.push(event);},{createAudio:()=>({start:async()=>{},mode(){},event(){},stop(){}})});
    await api.start({options:{netplayUrl:'ws://localhost/?room=th02mp-1234&run=1',netplayPlayerCount:2,netplaySpectatorId:'viewer'},music:'none'});
    const header={version:VERSION,game:'02',adapter:'confirmed-worker/1',players:2,hostSlot:0,tickMs:10,settings:{},identity:await fingerprint(SOURCE_PATHS),opening:{}};
    const row={inputs:[32,0],offlineMask:0,proof:'ok',hash:null};
    const pack=(kind,first,count,body)=>{const data=new Uint8Array(16+body.length),view=new DataView(data.buffer);data.set([80,57,56,83,1,kind,2,0]);view.setUint32(8,first,true);view.setUint32(12,count,true);data.set(body,16);return data.buffer;};
    const raw=new TextEncoder().encode(JSON.stringify([row]));
    const compressed=new Uint8Array(await new Response(new Blob([raw]).stream().pipeThrough(new CompressionStream('gzip'))).arrayBuffer());
    socket.onmessage({data:pack(1,0,0,new TextEncoder().encode(JSON.stringify(header)))});
    socket.onmessage({data:pack(2,0,1,compressed)});
    socket.onmessage({data:pack(5,1,0,new Uint8Array())});
    await new Promise(resolve=>setTimeout(resolve,350));
    assert.equal(api.health().gameFrame,1);assert(draws>=1);assert.deepEqual(errors,[]);
    api.stop();assert.equal(worker.terminated,true);assert.equal(removed,true);
  }finally{api?.stop();Object.assign(globalThis,saved);await rm(root,{recursive:true,force:true});}
});
