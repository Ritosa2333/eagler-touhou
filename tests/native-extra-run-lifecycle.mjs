import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import {pathToFileURL,fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
const home=path.dirname(fileURLToPath(import.meta.url));
if(!process.env.TH128_ACCEPTANCE_ROOT)throw new Error('Set TH128_ACCEPTANCE_ROOT to the local accepted-case directory; original evidence is not distributed.');
const owned=path.resolve(process.env.TH128_ACCEPTANCE_ROOT);
const {OriginalExtraRunController}=await import(pathToFileURL(path.join(home,'../site/runtime/native-extra-run.mjs')));
const qualification=JSON.parse(fs.readFileSync(path.join(owned,'case560-private99-EX-actual-terminal-fullchain/request.json'))).qualification;
const profile=new Uint8Array(fs.readFileSync(qualification.profile.path)),name=new Uint8Array(fs.readFileSync(qualification.name.path)),meta=JSON.parse(fs.readFileSync(qualification.readback.path));
const observed={record:{schema:'th128-owned-native-title-record/1',revision:meta.revision,provenance:'ordinary-native-postrun',tainted:false,profile,sha256:meta.profileSHA256,native:meta.native,runId:meta.runId},
 name:{strictRead:true,absent:false,revision:meta.nameRevision,bytes:name,digest:meta.nameDigest},seed:34772};
const sha=b=>createHash('sha256').update(b).digest('hex');
let cases=0;
function fixture(){
 const events=[],heap=new Uint8Array(1024*1024);let ptr=100000,current=true,epoch=0,frames=0,phase=0,serial=0,prepared=false,bound=false,worldBound=false,terminal=false,acked=false,retired=false,closed=0;
 let gate=null,badReceipt=false;
 const m={HEAPU8:heap,_malloc:n=>{const p=ptr;ptr+=n+16;return p;},_free(){},UTF8ToString:s=>s,
 _th128_world_error:()=>'',
 _th128_world_prepare_extra_record_profile(p,n,s,c,e){assert.equal(n,52232);heap.set(heap.slice(p,p+n),4000);epoch=e;prepared=true;events.push('profile');return 1;},
 _th128_world_bind_debug_record_clock(){events.push('clock');return 1;},
 _th128_world_debug_record_state:()=>JSON.stringify({prepared,epoch,worldBound,failed:false,generation:1,clearRecordEvents:terminal?1:0}),
 _th128_world_bind_debug_replay_record(){events.push('replay');return 1;},
 _th128_world_bind_debug_record_driver(){bound=true;events.push('driver');return 1;},
 _th128_world_bind_debug_postrun_asset(e,bank){assert.equal(e,epoch);events.push('asset'+bank);return 1;},
 _th128_world_debug_record_driver_state:()=>JSON.stringify({bound,epoch,generation:1,phase,lastClockSerial:serial,completedFrames:frames}),
 _th128_world_debug_record_begin_update(e,s){assert.equal(e,epoch);assert.equal(s,serial+1);assert.equal(phase,0);serial=s;phase=1;events.push('begin');return 1;},
 _th128_world_input(h,p,r){assert.equal(phase,1);events.push(['input',h,p,r]);return 1;},
 _th128_world_tick(){assert.equal(phase,1);phase=2;events.push('tick');return 1;},
 _th128_world_debug_record_presentation(e){assert.equal(phase,2);phase=3;events.push('packet');return JSON.stringify({epoch:e,generation:1,frame:frames+1,draws:[]});},
 _th128_world_debug_record_post_present(e,s,f){assert.equal(phase,3);assert.equal(s,serial+1);assert.equal(f,frames+1);serial=s;frames=f;phase=0;events.push('post');return 1;},
 _th128_world_state:()=>JSON.stringify({frame:frames,debugTerminal:{requested:terminal,counter:terminal?380:0,selector:23},stageControl:{current:7,pending:terminal?16:7}}),
 _th128_world_debug_postrun_record(op,e,p,n){assert.equal(e,epoch);if(op===0){assert.equal(n,8);events.push('writer');}if(op===1){assert.equal(n,48);assert.equal(phase,0);events.push('prepareStats');phase=4;heap.set([1,2,3,4],100);}if(op===2){assert.equal(n,20);assert.equal(phase,4);acked=true;phase=5;events.push('ackStats');}return JSON.stringify({accepted:true,epoch:e,generation:1,bound:true,prepared:op!==0,acknowledged:acked,bytes:4,digest32:55,pointer:100});},
 _th128_world_debug_record_size:()=>52232,_th128_world_debug_record_data:()=>4000,
 _th128_world_debug_postrun_retire(){assert(acked);assert.equal(phase,5);retired=true;events.push('retire');return 1;},
 _th128_world_normal_postrun_state:()=>JSON.stringify({present:retired,generation:1,extraSealed:true,provenance:'sealed-extra-terminal23-return'})};
 const receipt={async commit(key,metadata,bytes){events.push('persist');if(gate)await gate;return{committed:true,fullReadback:true,durability:'strict-indexeddb-transaction',bytes:bytes.length,sha256:badReceipt?'0'.repeat(64):sha(bytes),...metadata};},close(){++closed;}};
 const owner=new OriginalExtraRunController({module:m,isCurrent:()=>current,clock:()=>({low:10,high:30000000,seconds:1+serial/60}),openReceipts:async()=>receipt});
 return{owner,events,async start(){await owner.prepare(observed);worldBound=true;owner.bind([0,24,25,11].map(bank=>({bank,bytes:new Uint8Array([1])})));},
 terminal(){terminal=true;},cancel(){current=false;},bad(){badReceipt=true;},gate(p){gate=p;},get closed(){return closed;},get acked(){return acked;},get retired(){return retired;}};
}
const stepOptions=f=>({drainAudio:async()=>{f.events.push('audio');},render:async()=>{f.events.push('render');}});
{
 const f=fixture();await f.start();f.events.length=0;f.owner.queueInput(512,512,0);f.owner.queueInput(0,0,512);
 const step=await f.owner.step(stepOptions(f));assert.equal(step.frame,1);assert.deepEqual(f.events,['begin',['input',0,512,512],'tick','audio','packet','render','post']);
 f.terminal();f.events.length=0;const result=await f.owner.step(stepOptions(f));assert(result.retired);assert.deepEqual(f.events,['prepareStats','persist','ackStats','retire']);assert.equal(f.closed,1);f.owner.close();++cases;
}
{
 const f=fixture();await f.start();f.events.length=0;
 await assert.rejects(()=>f.owner.step({drainAudio:async()=>{},render:async()=>{throw Error('real renderer failed');}}),/real renderer failed/);
 assert(!f.events.includes('post'));assert.equal(f.owner.phase,'failed');f.owner.close();++cases;
}
{
 const f=fixture();await f.start();await f.owner.step(stepOptions(f));f.terminal();f.bad();
 await assert.rejects(()=>f.owner.step(stepOptions(f)),/strict statistics receipt/);assert(!f.acked&&!f.retired);assert.equal(f.closed,1);f.owner.close();++cases;
}
{
 const f=fixture();await f.start();await f.owner.step(stepOptions(f));f.terminal();let release;f.gate(new Promise(resolve=>{release=resolve;}));
 const pending=f.owner.step(stepOptions(f));while(!f.events.includes('persist'))await new Promise(resolve=>setTimeout(resolve,1));f.cancel();release();
 await assert.rejects(()=>pending,error=>error.name==='AbortError');assert(!f.acked&&!f.retired);assert.equal(f.closed,1);f.owner.close();++cases;
}
{
 const f=fixture();await f.start();let release;const gate=new Promise(resolve=>{release=resolve;});const pending=f.owner.step({drainAudio:()=>gate,render:async()=>{}});
 await assert.rejects(()=>f.owner.step(stepOptions(f)),/serial update unavailable/);release();await pending;f.owner.close();++cases;
}
{
 const f=fixture();await assert.rejects(()=>f.owner.prepare({...observed,name:{...observed.name,digest:'0'.repeat(64)}}));assert(!f.events.includes('profile'));f.owner.close();++cases;
}
console.log(JSON.stringify({passed:true,cases,productionController:true,syntheticNativeAndDeviceSeams:true,realSavedQualificationProfile:true,WorldStarts:0,actualNativeOwnerMinted:false}));
