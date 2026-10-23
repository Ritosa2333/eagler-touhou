import test from 'node:test';
import assert from 'node:assert/strict';
import {runInNewContext} from 'node:vm';
import {controlReplayTrace,readReplayInputReceipt,readReplayTrace} from './replay-trace.mjs';
import {TH08_REPLAY_SEMANTICS} from './titles/th08-replay-format.mjs';

const runtime={global:'__replayTraceTestRuntime',core:'core',module:'Module',app:'app'};
const contract=TH08_REPLAY_SEMANTICS.nativeTrace;
const sample=(sequence,{liveInput=0}={})=>{
  const words=Array(contract.wordCount).fill(0),fields=contract.sample;
  const put=(key,value)=>{words[fields[key]]=value>>>0;};
  put('sequence',sequence);put('boundary',contract.values.postUpdateBoundary);put('stage',4);
  put('pollSequence',sequence);put('liveInput',liveInput);
  put('recordingActive',1);put('recordingStage',4);put('recordingFrame',sequence);put('recordingInputCursor',sequence*2);
  put('recordingPhysicalInput',liveInput);put('playbackLoaded',1);put('playbackStage',4);put('playbackFrame',sequence);
  put('playbackInputCursor',sequence*2);put('playbackCurrent',liveInput);put('playbackPrevious',0);
  put('motionPlaying',1);put('motionStage',4);put('motionTick',sequence);put('movementCursor',0);
  put('movementCount',0);put('touchCursor',0);put('touchCount',0);put('pointCount',0);put('pointId',0xffffffff);
  put('pointXFloat32Bits',0);put('pointYFloat32Bits',0);put('pointReleased',0);
  put('consumerPhase',contract.values.consumerRunPhase);put('consumerValidMask',contract.values.consumedInputValidBit);
  put('consumerSequence',sequence);put('consumerControlFrame',sequence);put('consumerStage',4);
  put('consumerCurrent',liveInput);put('consumerPrevious',0);
  return words;
};

function mount({samples=[sample(1)],capacity=256,available=true,touchContext=3,touchFire=0,receiptAvailable=true}={}){
  const calls=[];
  const memory={buffer:new ArrayBuffer(4096)};
  const touchAddress=1024,touchWords=new Int32Array(memory.buffer,touchAddress,10);
  touchWords[5]=touchContext;touchWords[7]=touchFire;
  const exports={
    memory,
    sdl_replay_trace_control_v1:(enabled,reset)=>{calls.push(['control',enabled,reset]);return available?1:0;},
    sdl_game_status:()=>receiptAvailable?touchAddress:0,
    sdl_replay_trace_status_v1(field){
      if(field===contract.statusFields.schema)return contract.schema;
      if(field===contract.statusFields.nextSequence)return samples.length?samples.at(-1)[contract.sample.sequence]+1:1;
      if(field===contract.statusFields.firstAvailable)return samples[0]?.[contract.sample.sequence]??1;
      if(field===contract.statusFields.count)return samples.length;
      if(field===contract.statusFields.capacity)return capacity;
      const offset=field-contract.statusFields.samplesStart,index=Math.floor(offset/contract.wordCount),word=offset%contract.wordCount;
      return samples[index]?.[word]===undefined?-1:samples[index][word]|0;
    },
  };
  globalThis.__replayTraceTestRuntime={core:exports};
  return{exports,calls};
}

test('Replay control, receipts and trace reads stay inside the declared Runtime mount',()=>{
  const wrong=mount(),context={Module:wrong.exports,core:wrong.exports};
  const operations=[
    [controlReplayTrace,{runtime,contract,enabled:1,reset:1}],
    [readReplayInputReceipt,{runtime,contract,receiptId:'touchControl'}],
    [readReplayTrace,{runtime,contract}],
  ];
  for(const [fn,request] of operations){
    const read=runInNewContext(`(${fn.toString()})`,context);
    assert.equal(read(request).status,'incomplete','missing mount cannot borrow global exports');
    context[runtime.global]={core:{}};
    assert.equal(read(request).status,'incomplete','an empty selected mount cannot borrow global exports');
    delete context[runtime.global];
  }
  assert.deepEqual(wrong.calls,[],'no mutation reaches the unrelated Runtime');
});

test('controlReplayTrace applies only explicit binary controls through the declared runtime export',()=>{
  const mounted=mount();
  assert.deepEqual(controlReplayTrace({runtime,contract,enabled:1,reset:1}),
    {schema:'eagler-touhou/replay-native-trace-control/1',status:'passed',enabled:1,reset:1,nativeResult:1});
  assert.deepEqual(mounted.calls,[['control',1,1]]);
  assert.equal(controlReplayTrace({runtime,contract,enabled:2,reset:0}).status,'incomplete');
  assert.equal(controlReplayTrace({runtime,contract:{...contract,controlExport:'missing'},enabled:1,reset:0}).status,'incomplete');
  mount({available:false});
  assert.equal(controlReplayTrace({runtime,contract,enabled:1,reset:0}).status,'failed');
});

test('readReplayTrace returns only new mapped samples and repeated reads are non-consuming',()=>{
  const mounted=mount({samples:[sample(1),sample(2,{liveInput:1})]});
  const args={runtime,contract,afterSequence:1};
  const first=readReplayTrace(args),second=readReplayTrace(args);
  assert.deepEqual(first,second);
  assert.equal(first.status,'passed');assert.equal(first.gap,false);
  assert.equal(first.samples.length,1);assert.equal(first.samples[0].sequence,2);
  assert.equal(first.samples[0].boundary,1);assert.equal(first.samples[0].recording.physical,1);
  assert.equal(first.samples[0].playback.stage,4);assert.equal(first.samples[0].consumer.current,1);
  assert.equal(first.samples[0].motion.point,null);
  assert.equal(mounted.calls.length,0,'reading does not invoke the trace control export');
});

test('readReplayInputReceipt reads only title-declared native words without changing Runtime state',()=>{
  const mounted=mount({touchContext:3,touchFire:1});
  const receipt=readReplayInputReceipt({runtime,contract,receiptId:'touchControl'});
  assert.deepEqual(receipt,{schema:'eagler-touhou/replay-native-input-receipt/1',status:'passed',receiptId:'touchControl',
    fields:{context:3,fire:1},wordCount:10,source:contract.inputReceipts.touchControl.source});
  assert.equal(mounted.calls.length,0);
  assert.equal(readReplayInputReceipt({runtime,contract,receiptId:'missing'}).status,'incomplete');
  assert.equal(readReplayInputReceipt({runtime,contract:{...contract,inputReceipts:{touchControl:{...contract.inputReceipts.touchControl,
    fields:{context:{index:5,type:'i32'},fire:{index:5,type:'bool01'}}}}},receiptId:'touchControl'}).status,'incomplete',
  'aliased native receipt words cannot be read as two independent fields');
});

test('readReplayInputReceipt leaves missing memory and non-binary input fields incomplete',()=>{
  mount({touchFire:2});
  assert.equal(readReplayInputReceipt({runtime,contract,receiptId:'touchControl'}).status,'incomplete');
  mount({receiptAvailable:false});
  assert.equal(readReplayInputReceipt({runtime,contract,receiptId:'touchControl'}).status,'incomplete');
  mount();
  delete globalThis.__replayTraceTestRuntime.core.memory;
  assert.equal(readReplayInputReceipt({runtime,contract,receiptId:'touchControl'}).status,'incomplete');
});

test('readReplayTrace reports overwritten history and rejects inconsistent ABI metadata',()=>{
  mount({samples:[sample(3),sample(4)],capacity:2});
  const gap=readReplayTrace({runtime,contract,afterSequence:1});
  assert.equal(gap.status,'incomplete');assert.equal(gap.gap,true);
  assert.deepEqual(gap.samples.map(item=>item.sequence),[3,4]);
  assert.equal(readReplayTrace({runtime,contract,afterSequence:5}).status,'incomplete');

  mount({samples:[sample(1)]});
  const wrongSchema={...contract,schema:contract.schema+1};
  assert.equal(readReplayTrace({runtime,contract:wrongSchema}).status,'incomplete');
  const noSequence={...contract,sample:{...contract.sample}};delete noSequence.sample.sequence;
  assert.equal(readReplayTrace({runtime,contract:noSequence}).status,'incomplete');
  const aliasedField={...contract,sample:{...contract.sample,consumerCurrent:contract.sample.consumerPrevious}};
  assert.equal(readReplayTrace({runtime,contract:aliasedField}).status,'incomplete');
});

test('readReplayTrace rejects malformed native boolean words instead of coercing them',()=>{
  const corrupt=sample(1);corrupt[contract.sample.playbackLoaded]=2;mount({samples:[corrupt]});
  assert.equal(readReplayTrace({runtime,contract}).status,'incomplete');
});

test.afterEach(()=>{delete globalThis.__replayTraceTestRuntime;});
