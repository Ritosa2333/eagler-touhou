import test from 'node:test';
import assert from 'node:assert/strict';
import {createAutoScan} from '../tools/frame-inspector/auto-scan.mjs';

function fixture(status='diverged'){
  let native={status:'paused',replayActive:true,stage:6,replayCursor:10},candidateFrame=10;
  const seeks=[],commands=[];
  const api={
    readNative:async()=>native,
    request:async(command,args)=>{
      if(command==='seek'){candidateFrame=args.frame;seeks.push(args.frame);return {};}
      if(command==='scan-batch'){const base={stage:6,replayCursor:candidateFrame};const rows=[11,12,13].map(replayCursor=>({stage:6,replayCursor}));candidateFrame=13;return {base,rows,identity:{stage:6},ended:status==='batch-complete'};}
      throw Error(command);
    },
    command:async(kind,fields)=>{
      commands.push(kind);
      if(kind==='scan-batch')native={...native,replayCursor:status==='diverged'?12:13,scan:{batchId:fields.batchId,status,checkedFrame:status==='diverged'?12:13,...(status==='diverged'?{firstDifference:{frame:12,fields:{rng:{original:[1,2],candidate:[3,4]}}}}:{})}};
    },newId:()=> 'batch-1'
  };
  return {api,seeks,commands};
}
test('automatic scan leaves both sides at the first native mismatch, not the candidate batch end',async()=>{
  const f=fixture(),scan=createAutoScan(f.api),result=await scan.start();
  assert.equal(result.firstDifference.frame,12);assert.deepEqual(f.seeks,[10,12]);assert.equal(scan.running(),false);
});
test('successful terminal batch reports its actual checked range',async()=>{
  const f=fixture('batch-complete'),result=await createAutoScan(f.api).start();
  assert.deepEqual(result,{status:'ended',startFrame:10,lastFrame:13});
});
test('failed identity/start validation leaves the original paused and never claims success',async()=>{
  const f=fixture();f.api.command=async(kind)=>{f.commands.push(kind);if(kind==='scan-batch')throw Error('录像不同');};
  await assert.rejects(createAutoScan(f.api).start(),/录像不同/);assert.deepEqual(f.commands,['scan-batch','pause']);
});
test('cancel restores the candidate from its lookahead to the native stopped cursor',async()=>{
  let native={status:'paused',replayActive:true,stage:6,replayCursor:10},scan;const seeks=[];
  scan=createAutoScan({readNative:async()=>native,newId:()=> '1',request:async(command,args)=>{
    if(command==='seek'){seeks.push(args.frame);return {};}
    return {base:{},rows:[{replayCursor:11}],identity:{},ended:false};
  },command:async(kind)=>{
    if(kind==='scan-batch')native={...native,status:'running'};
    if(kind==='pause')native={...native,status:'paused',replayCursor:11};
  },delay:async()=>scan.cancel()});
  const result=await scan.start();assert.equal(result.status,'cancelled');assert.deepEqual(seeks,[10,11]);
});
