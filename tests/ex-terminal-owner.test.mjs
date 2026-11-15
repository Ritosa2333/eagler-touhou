import fs from 'node:fs';
import path from 'node:path';
import assert from 'node:assert/strict';
import {pathToFileURL,fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';

// Regression target: removing the pre-ACK native-owner recheck must ACK a stale
// terminal and fail these assertions. Native/storage/device boundaries below are
// synthetic; the complete production controller and qualification code are real.
// No World, natural MSG, browser or new native permit is created by this test.
const here=path.dirname(fileURLToPath(import.meta.url));
const owned=process.env.TH128_ACCEPTANCE_ROOT;
if(!owned)throw Error('TH128_ACCEPTANCE_ROOT must identify the existing local evidence directory');
const controllerFile=process.argv[2]||path.resolve(here,'../site/runtime/native-extra-run.mjs');
const {OriginalExtraRunController}=await import(pathToFileURL(controllerFile));
const qualification=JSON.parse(fs.readFileSync(path.join(owned,'case560-private99-EX-actual-terminal-fullchain/request.json'))).qualification;
const profile=new Uint8Array(fs.readFileSync(qualification.profile.path));
const name=new Uint8Array(fs.readFileSync(qualification.name.path));
const meta=JSON.parse(fs.readFileSync(qualification.readback.path));
const observed={record:{schema:'th128-owned-native-title-record/1',revision:meta.revision,provenance:'ordinary-native-postrun',tainted:false,profile,
  sha256:meta.profileSHA256,native:meta.native,runId:meta.runId},
  name:{strictRead:true,absent:false,revision:meta.nameRevision,bytes:name,digest:meta.nameDigest},seed:34772};
const sha=b=>createHash('sha256').update(b).digest('hex');
assert.equal(profile.length,52232);assert.equal(sha(profile),meta.profileSHA256);
assert.equal(name.length,9);assert.equal(sha(name),meta.nameDigest);

function fixture({mutate=()=>{},badReceipt=false,cancel=false}={}){
  const heap=new Uint8Array(1024*1024),events=[];
  let pointer=100000,prepared=false,current=true,ackCalls=0,retireCalls=0,receiptClosed=0,receiptWrites=0;
  const driver={bound:false,epoch:0,generation:1,phase:0,lastClockSerial:1,completedFrames:0};
  const record={prepared:false,epoch:0,worldBound:false,failed:false,generation:1,clearRecordEvents:0,sessionPresent:false};
  const native={frame:0,debugTerminal:{requested:false,counter:0,selector:23,runtimeScene:16},stageControl:{current:7,pending:7}};
  const m={HEAPU8:heap,_malloc(n){const p=pointer;pointer+=n+16;return p;},_free(){},UTF8ToString:s=>s,_th128_world_error:()=>'',
    _th128_world_prepare_extra_record_profile(p,n,seed,calls,e){assert.equal(n,52232);assert.equal(seed,34772);assert.equal(calls,0);
      heap.set(heap.slice(p,p+n),4000);driver.epoch=record.epoch=e;record.prepared=true;return 1;},
    _th128_world_bind_debug_record_clock(e){assert.equal(e,driver.epoch);return 1;},
    _th128_world_debug_record_state:()=>JSON.stringify(record),
    _th128_world_bind_debug_replay_record(e){assert.equal(e,driver.epoch);record.sessionPresent=true;return 1;},
    _th128_world_bind_debug_record_driver(e){assert.equal(e,driver.epoch);driver.bound=true;return 1;},
    _th128_world_bind_debug_postrun_asset(e,b){assert.equal(e,driver.epoch);assert([0,24,25,11].includes(b));return 1;},
    _th128_world_debug_record_driver_state:()=>JSON.stringify(driver),
    _th128_world_debug_record_begin_update(e,s){assert.equal(e,driver.epoch);assert.equal(s,driver.lastClockSerial+1);
      assert.equal(driver.phase,0);driver.lastClockSerial=s;driver.phase=1;return 1;},
    _th128_world_input(){assert.equal(driver.phase,1);return 1;},
    _th128_world_tick(){assert.equal(driver.phase,1);++native.frame;driver.phase=2;return 1;},
    _th128_world_debug_record_presentation(e){assert.equal(driver.phase,2);driver.phase=3;
      return JSON.stringify({epoch:e,generation:1,frame:native.frame,draws:[]});},
    _th128_world_debug_record_post_present(e,s,f){assert.equal(e,driver.epoch);assert.equal(s,driver.lastClockSerial+1);
      assert.equal(driver.phase,3);assert.equal(f,native.frame);driver.lastClockSerial=s;driver.completedFrames=f;driver.phase=0;return 1;},
    _th128_world_state:()=>JSON.stringify(native),
    _th128_world_debug_postrun_record(op,e,p,n){
      if(op===2){events.push('ACK');++ackCalls;}
      assert.equal(e,record.epoch);assert.equal(e,driver.epoch);assert.equal(record.failed,false);assert.equal(driver.generation,1);
      if(op===0){assert.equal(n,8);}
      if(op===1){assert.equal(n,48);assert.equal(driver.phase,0);const v=new DataView(heap.buffer,p,n);
        assert.equal(v.getUint32(8,true),driver.lastClockSerial+1);assert.equal(v.getUint32(20,true),driver.lastClockSerial+2);
        driver.lastClockSerial=v.getUint32(20,true);driver.phase=4;prepared=true;heap.set([1,2,3,4],100);events.push('prepare');}
      if(op===2){assert.equal(n,20);assert.equal(driver.phase,4);assert.equal(native.stageControl.pending,16);
        const v=new DataView(heap.buffer,p,n);assert.equal(v.getUint32(8,true),1);assert.equal(v.getUint32(12,true),4);assert.equal(v.getUint32(16,true),55);
        driver.phase=5;}
      return JSON.stringify({accepted:true,epoch:e,generation:1,bound:true,prepared,acknowledged:ackCalls>0,bytes:4,digest32:55,pointer:100});
    },
    _th128_world_debug_record_size:()=>52232,_th128_world_debug_record_data:()=>4000,
    _th128_world_debug_postrun_retire(){events.push('retire');++retireCalls;assert.equal(driver.phase,5);assert.equal(native.stageControl.current,7);
      assert.equal(native.stageControl.pending,16);assert.equal(native.debugTerminal.runtimeScene,16);assert.equal(record.sessionPresent,true);return 1;},
    _th128_world_normal_postrun_state:()=>JSON.stringify({present:retireCalls===1,generation:1,extraSealed:true,provenance:'sealed-extra-terminal23-return'})
  };
  const receipts={async commit(key,metadata,bytes){
    assert.equal(key,'EX-full-statistics');assert.equal(bytes.length,52236);++receiptWrites;events.push('persist');
    await Promise.resolve();mutate({native,driver,record,heap});if(cancel)current=false;
    return {committed:true,fullReadback:true,durability:'strict-indexeddb-transaction',bytes:bytes.length,sha256:badReceipt?'0'.repeat(64):sha(bytes),...metadata};
  },close(){++receiptClosed;}};
  const controller=new OriginalExtraRunController({module:m,isCurrent:()=>current,clock:()=>({low:10,high:30000000,seconds:1}),openReceipts:async()=>receipts});
  return{controller,events,async ready(){await controller.prepare(observed);record.worldBound=true;
    controller.bind([0,24,25,11].map(bank=>({bank,bytes:new Uint8Array([1])})));
    await controller.step({drainAudio:async()=>{},render:async()=>{}});
    // Synthetic terminal API responses only, never a production clear/carry credential.
    native.debugTerminal.requested=true;native.debugTerminal.counter=380;native.stageControl.pending=16;record.clearRecordEvents=1;
  },get counts(){return{ackCalls,retireCalls,receiptClosed,receiptWrites};}};
}
async function run(options={}){
  const f=fixture(options);await f.ready();let result,error;
  try{result=await f.controller.step({drainAudio:async()=>{},render:async()=>{}});}catch(e){error=e;}
  return{f,result,error};
}
let accepted=0,rejected=0;
{
  const {f,result,error}=await run();assert.equal(error,undefined);assert.equal(result.retired,true);
  assert.deepEqual(f.events,['prepare','persist','ACK','retire']);
  assert.deepEqual(f.counts,{ackCalls:1,retireCalls:1,receiptClosed:1,receiptWrites:1});++accepted;
}
const mutations=[
  ['terminal scene',({native})=>{native.debugTerminal.runtimeScene=15;}],
  ['native frame',({native})=>{native.frame++;}],
  ['driver frame',({driver})=>{driver.completedFrames++;}],
  ['driver phase',({driver})=>{driver.phase=5;}],
  ['driver serial behind',({driver})=>{driver.lastClockSerial--;}],
  ['driver serial ahead',({driver})=>{driver.lastClockSerial++;}],
  ['driver epoch',({driver})=>{driver.epoch++;}],
  ['driver generation',({driver})=>{driver.generation++;}],
  ['scene current',({native})=>{native.stageControl.current=4;}],
  ['scene pending',({native})=>{native.stageControl.pending=4;}],
  ['terminal requested',({native})=>{native.debugTerminal.requested=false;}],
  ['terminal counter',({native})=>{native.debugTerminal.counter=379;}],
  ['terminal selector',({native})=>{native.debugTerminal.selector=22;}],
  ['record generation',({record})=>{record.generation++;}],
  ['record clear count',({record})=>{record.clearRecordEvents=2;}],
  ['record failed',({record})=>{record.failed=true;}],
  ['session retired',({record})=>{record.sessionPresent=false;}]
];
for(const [label,mutate] of mutations){
  const {f,error}=await run({mutate});
  assert(error,label+' must reject');assert.equal(f.counts.ackCalls,0,label+': native ACK must not be called after receipt owner drift');
  assert.equal(f.counts.retireCalls,0,label+': retirement must not be called');
  assert.equal(f.counts.receiptClosed,1,label+': receipt handle must close');
  assert.equal(f.counts.receiptWrites,1,label+': completed receipt remains retained');
  assert.equal(f.controller.phase,'failed');++rejected;
}
for(const options of [{cancel:true},{badReceipt:true}]){
  const {f,error}=await run(options);assert(error);if(options.cancel)assert.equal(error.name,'AbortError');
  assert.equal(f.counts.ackCalls,0);assert.equal(f.counts.retireCalls,0);assert.equal(f.counts.receiptClosed,1);++rejected;
}
console.log(JSON.stringify({passed:true,accepted,rejected,wholeProductionController:true,syntheticNativeStorageAndDevice:true,
  realSavedQualificationProfile:true,naturalMSGProof:false,WorldStarts:0,actualNativeOwnerMinted:false}));
