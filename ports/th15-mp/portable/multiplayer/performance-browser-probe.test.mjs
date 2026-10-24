import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
import {createSenderDelayQueue} from './performance-sender-delay.mjs';

// Exercise the actual serialized init-script function, including the same
// prototype hooks used by Chromium, without loading or mutating a game.
const source=readFileSync(new URL('./performance-browser.mjs',import.meta.url),'utf8');
const probe=source.slice(source.indexOf('function installProbe('),source.indexOf('\nfunction host('));
function fixture({save=false}={}){
  const clock={at:0},pending=[],heap=new Uint8Array(1024),calls=[];
  const context={performance:{timeOrigin:10000,now:()=>clock.at},setTimeout,clearTimeout,TextDecoder,
    requestAnimationFrame:callback=>(pending.push(callback),pending.length),advance:ms=>clock.at+=ms,
    document:{querySelector:()=>({textContent:''}),addEventListener(){}},__eaglerPeerTransport:{route:'relay'},
    Module:{HEAPU8:heap,HEAPU32:new Uint32Array(heap.buffer),_th15_mp_status:()=>4,
      _th15_mp_rollback_status:()=>128,_th15_phase:()=>1,onGameFrame:()=>{}},
    WebSocket:class {send(){}},WebGL2RenderingContext:class {
      constructor(){this.canvas={id:'canvas'};}bindFramebuffer(...args){calls.push(['bind',...args]);}
      blitFramebuffer(...args){calls.push(['blit',...args]);}
    },
  };
  if(save){context.saveStatus=0;context.fsCallback=null;
    Object.assign(context.Module,{_th15_mp_save_request:()=>31,_th15_mp_save_poll:()=>context.saveStatus,_th15_mp_save_cancel:()=>1,
      FS:{syncfs:(populate,callback)=>{context.fsCallback=callback;calls.push(['syncfs',populate]);}}});
  }
  runInNewContext('('+probe+')({seat:0,duration:1},('+createSenderDelayQueue.toString()+'));'+
    'globalThis.gl=new WebGL2RenderingContext();globalThis.notify=true;globalThis.draw=false;'+
    'globalThis.nativeTick=function tick(time){advance(5);if(draw){gl.bindFramebuffer(36009,null);gl.blitFramebuffer(0,0,640,480,0,0,640,480,16384,9728);}'+
    'if(notify)Module.onGameFrame(1,4,1);requestAnimationFrame(tick);};'+
    '__th15Performance.attach();requestAnimationFrame(nativeTick);',context);
  const step=at=>{clock.at=at;const callback=pending.shift();assert.ok(callback);callback(at);};
  return {context,clock,pending,calls,step};
}

test('stable native rAF is learned before the measured correction-only callback',()=>{
  const f=fixture();f.step(10);f.context.__th15Performance.start();f.context.notify=false;f.step(30);
  const data=f.context.__th15Performance.result();assert.equal(data.nativeTokens.length,1);assert.equal(data.rows.length,1);
  assert.equal(data.rows[0].native,true);assert.equal(data.rows[0].nativeMs,null);assert.equal(data.rows[0].exit-data.rows[0].entry,5);
});

test('only color blits into the game canvas default draw framebuffer are submissions',()=>{
  const f=fixture();f.step(10);f.context.__th15Performance.start();f.context.draw=true;f.step(30);
  const {gl}=f.context;gl.bindFramebuffer(36009,{});gl.blitFramebuffer(0,0,1,1,0,0,1,1,16384,9728);
  gl.bindFramebuffer(36160,null);gl.blitFramebuffer(0,0,1,1,0,0,1,1,256,9728);
  gl.canvas={id:'another-canvas'};gl.blitFramebuffer(0,0,1,1,0,0,1,1,16384,9728);
  const data=f.context.__th15Performance.result();assert.equal(data.presentations.length,1);assert.equal(data.presentationsOutsideNativeCallback,0);
});

test('post-callback diagnostic getter cost is outside native callback wall',()=>{
  const f=fixture();f.step(10);f.context.__th15Performance.start();
  f.context.Module._th15_mp_rollback_status=()=>{f.clock.at+=20;return 128;};f.step(30);
  const row=f.context.__th15Performance.result().rows[0];assert.equal(row.exit-row.entry,5);assert.equal(row.observerMs,20);
});

test('save observer records status transitions and real FS callbacks without extra journal scans',()=>{
  const f=fixture({save:true});f.step(10);f.context.__th15Performance.start();let scans=0,completed=false;
  f.context.Module._th15_mp_rollback_status=()=>{++scans;return 128;};
  const c=f.context.Module,token=c._th15_mp_save_request();assert.equal(token,31);
  assert.equal(c._th15_mp_save_poll(token),0);assert.equal(c._th15_mp_save_poll(token),0);
  f.context.saveStatus=1;assert.equal(c._th15_mp_save_poll(token),1);
  c.FS.syncfs(false,error=>{assert.equal(error,null);completed=true;});assert.equal(completed,false);
  const before=f.context.__th15Performance.result().saveEvents;
  assert.deepEqual(Array.from(before,e=>e.kind),['native-request','native-poll','native-poll','fs-sync-start']);
  assert.equal(before[1].result,0);assert.equal(before[2].result,1);assert.equal(scans,0);
  f.context.fsCallback(null);assert.equal(completed,true);
  const events=f.context.__th15Performance.result().saveEvents;assert.equal(events.at(-1).kind,'fs-sync-complete');
  assert.equal(events.at(-1).error,null);assert.equal(scans,0);
});

const persistenceSource=source.slice(source.indexOf('async function inspectPersistence('),source.indexOf('\nfunction nextJson('));
function persistenceFixture({mismatch=false,databaseError=false,signed=false,replay=false,replayMismatch=false}={}){
  const root='/savesth15mp',memory=new Map([[root+'/scoreth15.dat',new Uint8Array(signed?[0,127,128,197,255]:[1,2,3])],[root+'/th15.cfg',new Uint8Array([4,5])]]);
  if(replay)memory.set(root+'/replay/th15mp_01.rpy',new Uint8Array([69,65,71,76,82,80,89,49]));
  const disk=new Map([...memory].map(([path,bytes])=>{if(!signed)return [path,{contents:bytes.slice()}];
    const backing=new Int8Array(bytes.length+4);backing.set(bytes,2);return [path,{contents:backing.subarray(2,bytes.length+2)}];}));
  if(mismatch)disk.get(root+'/scoreth15.dat').contents[0]=9;
  if(replayMismatch)disk.get(root+'/replay/th15mp_01.rpy').contents[0]=9;
  const heap=new Uint8Array(1024),calls=[];let closed=0;
  const db={close:()=>++closed,transaction:(name,mode)=>{calls.push(['transaction',name,mode]);let outstanding=0;
    const transaction={objectStore:()=>({get:path=>{++outstanding;const request={};queueMicrotask(()=>{
      request.result=disk.get(path);request.onsuccess();if(!--outstanding)queueMicrotask(()=>transaction.oncomplete());});return request;}})};
    return transaction;
  }};
  const context={performance:{now:()=>42},setTimeout,clearTimeout,
    Module:{HEAPU8:heap,FS:{readFile:path=>memory.get(path)},_malloc:()=>128,_free:()=>{},
      _th15_validate_file:(kind,pointer,size)=>{calls.push(['validate',kind,Array.from(heap.subarray(pointer,pointer+size))]);return 1;},
      _th15_mp_replay_validate:(pointer,size)=>{calls.push(['validate-replay',Array.from(heap.subarray(pointer,pointer+size))]);return 1;}},
    indexedDB:{databases:async()=>[{name:root}],open:name=>{calls.push(['open',name]);const request={};
      queueMicrotask(()=>{if(databaseError){request.error=Error('IDB unavailable');request.onerror();}
        else{request.result=db;request.onsuccess();}});return request;}},
  };
  runInNewContext(persistenceSource,context);return {context,calls,get closed(){return closed;}};
}

test('post-window persistence inspection reads real IDBFS paths and validates stored bytes',async()=>{
  const f=persistenceFixture(),result=await f.context.inspectPersistence();
  assert.equal(result.database,'/savesth15mp');assert.equal(result.store,'FILE_DATA');assert.equal(result.files.length,2);
  assert.ok(result.files.every(file=>file.equal&&file.nativeValid));assert.equal(f.closed,1);
  assert.deepEqual(f.calls.find(call=>call[0]==='transaction'),['transaction','FILE_DATA','readonly']);
  assert.deepEqual(f.calls.filter(call=>call[0]==='validate').map(call=>call[1]),[0,2]);
});

test('persistence inspection retains byte mismatch instead of claiming successful persistence',async()=>{
  const f=persistenceFixture({mismatch:true}),result=await f.context.inspectPersistence();
  assert.equal(result.files[0].equal,false);assert.deepEqual(Array.from(result.files[0].memory),[1,2,3]);
  assert.deepEqual(Array.from(result.files[0].indexedDb),[9,2,3]);assert.equal(f.closed,1);
});

test('persistence compares high-bit Int8Array bytes and still detects a real stored-byte change',async()=>{
  const signed=persistenceFixture({signed:true}),valid=await signed.context.inspectPersistence();
  assert.equal(valid.files[0].equal,true);assert.deepEqual(Array.from(valid.files[0].indexedDb),[0,127,128,197,255]);
  assert.deepEqual(signed.calls.find(call=>call[0]==='validate')[2],[0,127,128,197,255]);
  const changed=persistenceFixture({signed:true,mismatch:true}),invalid=await changed.context.inspectPersistence();
  assert.equal(invalid.files[0].equal,false);assert.deepEqual(Array.from(invalid.files[0].indexedDb),[9,127,128,197,255]);
});

test('persistence inspection rejects actual IndexedDB read failure',async()=>{
  const f=persistenceFixture({databaseError:true});await assert.rejects(f.context.inspectPersistence(),/IDB unavailable/);
  assert.equal(f.calls.some(call=>call[0]==='validate'),false);
});

test('completed evidence Replay is compared to IDB bytes and uses the native MP reader',async()=>{
  const f=persistenceFixture({replay:true}),result=await f.context.inspectPersistence(true);
  assert.equal(result.files.length,2);assert.equal(result.replay.name,'replay/th15mp_01.rpy');
  assert.equal(result.replay.equal,true);assert.equal(result.replay.nativeValid,true);
  assert.deepEqual(f.calls.filter(call=>call[0]==='validate-replay'),[['validate-replay',[69,65,71,76,82,80,89,49]]]);
  assert.deepEqual(f.calls.filter(call=>call[0]==='validate').map(call=>call[1]),[0,2]);
});

test('a stale or changed persisted Replay remains a failed byte comparison',async()=>{
  const f=persistenceFixture({replay:true,replayMismatch:true}),result=await f.context.inspectPersistence(true);
  assert.equal(result.replay.equal,false);assert.equal(result.files.every(file=>file.equal),true);
});

function validSaveEvidence(){
  return {saved:{timeOrigin:1000,saveApi:true,events:[
    {kind:'native-request',at:30000,token:1},{kind:'native-poll',at:30050,token:1,result:1},
    {kind:'fs-sync-start',at:30051,id:1,populate:false},{kind:'fs-sync-complete',at:30060,id:1,populate:false,error:null},
  ]},host:{timeOrigin:1000,start:31000,errors:[],inputAckBeforeSync:true},
  persistence:{files:[{equal:true,nativeValid:true},{equal:true,nativeValid:true}]}};
}

test('independent save evidence gate rejects an input ACK delayed behind sync',()=>{
  const f=persistenceFixture(),{saved,host,persistence}=validSaveEvidence();
  assert.equal(f.context.summarizeSaveEvidence(saved,host,persistence).passed,true);
  host.inputAckBeforeSync=false;const result=f.context.summarizeSaveEvidence(saved,host,persistence);
  assert.equal(result.passed,false);assert.ok(result.failures.some(error=>error.includes('concurrent input delivery was not demonstrated')));
});

test('periodic persistence evidence must use the matching FS transaction callback',()=>{
  const f=persistenceFixture(),{saved,host,persistence}=validSaveEvidence();saved.events[3].id=99;
  const result=f.context.summarizeSaveEvidence(saved,host,persistence);
  assert.equal(result.periodicSavesRequestedBeforeHostSync[0].persistedAt,null);assert.equal(result.passed,false);
});

test('baseline proof sync cannot be counted as the required periodic autosave',()=>{
  const f=persistenceFixture(),{saved,host,persistence}=validSaveEvidence();host.start=33000;host.periodicCutoff=31000;
  const baseline=[{kind:'native-request',at:32000,token:2},{kind:'native-poll',at:32010,token:2,result:1},
    {kind:'fs-sync-start',at:32011,id:2,populate:false},{kind:'fs-sync-complete',at:32020,id:2,populate:false,error:null}];
  saved.events.push(...baseline);let result=f.context.summarizeSaveEvidence(saved,host,persistence);
  assert.deepEqual(Array.from(result.periodicSavesRequestedBeforeHostSync,event=>event.token),[1]);assert.equal(result.passed,true);
  saved.events=baseline;result=f.context.summarizeSaveEvidence(saved,host,persistence);assert.equal(result.passed,false);
  assert.equal(result.periodicSavesRequestedBeforeHostSync.length,0);
});
