import test from 'node:test';
import assert from 'node:assert/strict';
import {createReplaySaveQueue,createMultiplayerRuntime} from '../src/multiplayer.mjs';

const flush=async()=>{for(let n=0;n<12;n++)await Promise.resolve();};
function fixture(){
 let frame=100,generation=1,active=true,ending=false,block=null,fail=false;
 const captures=[],writes=[];
 const queue=createReplaySaveQueue({
  key:()=>active&&frame!==0xffffffff?generation+':'+frame:null,
  capture:()=>{const job={path:'run-'+generation,bytes:new Uint8Array([frame,Number(ending)])};captures.push(job);return job;},
  write:async job=>{writes.push(job);if(block)await block;if(fail){fail=false;throw Error('storage unavailable');}},
 });
 return {queue,captures,writes,setFrame:value=>{frame=value;},nextRun:()=>{generation++;frame=0;},
  end:()=>{ending=true;},stop:()=>{active=false;},fail:()=>{fail=true;},
  block:()=>{let release;block=new Promise(resolve=>{release=()=>{block=null;resolve();};});return release;}};
}

test('unchanged confirmation and unconfirmed inputs do not encode another archive',async()=>{
 const f=fixture();await f.queue.save();await f.queue.save();await f.queue.save();
 assert.equal(f.captures.length,1);assert.equal(f.writes.length,1);
 f.setFrame(0xffffffff);await f.queue.save();assert.equal(f.captures.length,1);
 f.setFrame(101);await f.queue.save();assert.equal(f.captures.length,2);
});

test('requests during a slow write capture only the latest confirmed revision',async()=>{
 const f=fixture(),release=f.block(),first=f.queue.save();await flush();
 const requests=[];
 for(let n=101;n<=200;n++){f.setFrame(n);requests.push(f.queue.save());}
 assert.equal(f.captures.length,1,'encoding waits for storage instead of making 100 full copies');
 release();await Promise.all([first,...requests]);
 assert.deepEqual(f.writes.map(job=>Array.from(job.bytes)),[[100,0],[200,0]]);
 assert.equal(f.captures.length,2);
});

test('forced exit captures current metadata before native stop, even at the same confirmed frame',async()=>{
 const f=fixture(),release=f.block();const first=f.queue.save();await flush();
 f.end();const final=f.queue.save({force:true});f.stop();
 assert.equal(f.captures.length,2,'the final archive is captured synchronously');
 release();await Promise.all([first,final]);
 assert.deepEqual(f.writes.map(job=>Array.from(job.bytes)),[[100,0],[100,1]]);
});

test('retry generation saves a new archive even when its confirmed frame repeats',async()=>{
 const f=fixture();await f.queue.save();f.nextRun();f.setFrame(100);await f.queue.save();
 assert.deepEqual(f.writes.map(job=>job.path),['run-1','run-2']);
});

test('a failed storage write remains dirty and can be retried',async()=>{
 const f=fixture();f.fail();await assert.rejects(f.queue.save(),/storage unavailable/);
 await f.queue.save();assert.equal(f.captures.length,2);assert.equal(f.writes.length,2);
 await f.queue.save();assert.equal(f.captures.length,2);
});

test('several forced snapshots for a waiting path retain the latest final data',async()=>{
 const f=fixture(),release=f.block();const first=f.queue.save();await flush();
 f.setFrame(101);const second=f.queue.save({force:true});f.setFrame(102);f.end();const third=f.queue.save({force:true});
 release();await Promise.all([first,second,third]);
 assert.deepEqual(f.writes.map(job=>Array.from(job.bytes)),[[100,0],[102,1]]);
});

async function runtimeFixture(){
 const heap=new Uint8Array(4096),status=new Uint32Array(heap.buffer,64,24),rollback=new Uint32Array(heap.buffer,192,8);
 status[3]=200;status[4]=199;status[12]=1;rollback[3]=100;
 const writes=[],encodes=[];let ending=false;
 const core={HEAPU8:heap,_malloc:()=>256,_free:()=>{},_th12_mp_status:()=>64,_th12_mp_rollback_status:()=>192,
  _th12_mp_configure:()=>1,_th12_mp_connect:()=>1,_th12_mp_start:()=>1,
  _th12_mp_stop:()=>{status.fill(0);rollback.fill(0);heap.fill(0,512,520);},
  _th12_mp_replay_encode:()=>{const out=new Uint32Array(heap.buffer,512,2);out.set([rollback[3],Number(ending)]);encodes.push(Array.from(out));return 8;},
  _th12_mp_replay_data:()=>512,
 };
 const options={netplayMode:'lan',netplayUrl:'ws://localhost/?room=fixture&run=one',netplayPlayerCount:2,netplayPlayer:0,
  netplaySeed:1,netplayDifficulty:0,netplayAdonisMode:0,netplayLoadouts:[{character:0,shot:0},{character:1,shot:1}]};
 const runtime=createMultiplayerRuntime({core,
  manifest:{game:'th12',product:'th12mp',variant:'multiplayer',profile:'multiplayer',features:{multiplayer:true},execution:{sha256:'a'.repeat(64)}},
  getOptions:()=>options,emit:()=>{},loadStage:async()=>{},onError:error=>{throw error;},onExit:async()=>{},
  store:()=>({write:async(path,bytes)=>writes.push({path,words:Array.from(new Uint32Array(bytes.buffer,bytes.byteOffset,2))}),sync:async()=>{}}),
 });
 await runtime.launch();return {runtime,status,rollback,writes,encodes,end:()=>{ending=true;}};
}

test('runtime refreshes the archive when correction commits more inputs at unchanged confirmation',async t=>{
 const f=await runtimeFixture();t.after(()=>f.runtime.stop());
 await f.runtime.save();await f.runtime.save();assert.deepEqual(f.encodes,[[100,0]]);
 f.rollback[3]=200;await f.runtime.save();
 assert.equal(f.status[4],199,'confirmation itself has not advanced');
 assert.deepEqual(f.encodes,[[100,0],[200,0]],'the native committed count invalidates the saved revision');
 await f.runtime.save();assert.equal(f.encodes.length,2);
 f.status[12]++;await f.runtime.save();assert.equal(f.encodes.length,3,'a new native generation also invalidates it');
});

test('runtime forced exit saves its captured final archive after native stop clears the recorder',async()=>{
 const f=await runtimeFixture();await f.runtime.save();f.end();
 const saved=f.runtime.save({force:true});
 assert.deepEqual(f.encodes,[[100,0],[100,1]],'force captures before returning to the caller');
 f.runtime.stop();await saved;
 assert.deepEqual(f.writes.map(write=>write.words),[[100,0],[100,1]]);
});
