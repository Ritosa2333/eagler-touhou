import test from 'node:test';
import assert from 'node:assert/strict';
import {runTouchDialogueFreshConfirmScene} from './touch-dialogue-confirm-scenes.mjs';

const ENTRY=17,WAIT_OFFSET=84,FIRE=1;
function options(){return{
  expected:{variant:'normal',stage:0,touchContext:2},inputMasks:{fire:FIRE,confirm:1},waitOpcode:4,
  consumerLatencyTicks:0,consumerLatencySource:'Direct modal-consumption component fixture',
  pointer:{id:41,point:{x:.5,y:.5}},
  gesture:{maxTapDurationMs:450,confirmPulseTicks:3,source:'common/include/eagler/input/DialogueGesture.hpp:end (<500 ms)'},
  controls:{heldFire:{fireEnabled:true,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:100},
    neutral:{fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:100}},
  budgets:{baselineTicks:2,dialogueOpenTicks:3,waitTicks:8,source:'adapter bounded native scene budget'}};
}
function manifest(){return{
  schema:'eagler-touhou/dialogue-touch-manifest/1',available:true,reason:null,
  runtime:{generation:3,pointer:4096,epoch:2},fixture:{used:true,gateEnabled:true,writeLocked:true,loadScope:false,committedBytes:0},
  source:{variant:'normal',stage:0,character:0,team:0,difficultyMask:1,
    ecl:{asset:'ecldata1.ecl',byteLength:2048,fnv64:'0123456789abcdef'},
    message:{asset:'msg1a.dat',byteLength:512,fnv64:'fedcba9876543210'}},
  candidates:[{entry:ENTRY,ecl:{timeline:1,time:90,offset:512,difficultyBits:1,referenceCount:2},
    message:{waitOffset:WAIT_OFFSET,waitTime:0,timeoutFrames:90,instructionCount:8}}]};
}
function makeIO(behavior={}){
  let fire=false,requested=false,tapSent=false,postTapTick=0,sequence=0,stepCount=0;
  const state={available:true,runtime:{generation:3,pointer:4096,epoch:2},scene:{variant:'normal',stage:0},
    fixture:{used:true,gateEnabled:true,writeLocked:true,loadScope:false,committedBytes:0},
    touch:{fireEnabled:false,context:1},dialogue:{present:false,message:-1,instructionOffset:0xffffffff,opcode:0xffffffff,
      consumer:{sequence:0,valid:false},
      instructionTime:0xffffffff,timer:0,pausedFrames:0,minimumWait:6,skippable:true,currentInput:0,previousInput:0,freshConfirmEdge:false},
    request:{serial:0,status:0,entry:-1,timeline:-1}};
  return{
    calls:{},state,
    async readManifest(){return structuredClone(manifest());},
    async controls(value){this.calls.controls=(this.calls.controls??[]).concat([structuredClone(value)]);fire=value.fireEnabled;state.touch.fireEnabled=fire;return{acknowledged:true};},
    async step(count){assert.equal(count,1);stepCount++;sequence++;
      state.dialogue.consumer.valid=state.dialogue.present;
      if(state.dialogue.consumer.valid)state.dialogue.consumer.sequence++;
      if(requested&&!tapSent){
        if(behavior.autoEdge){state.dialogue.currentInput=1;state.dialogue.previousInput=0;state.dialogue.freshConfirmEdge=true;}
        else if(behavior.autoAdvance&&state.dialogue.pausedFrames>=state.dialogue.minimumWait){state.dialogue.instructionOffset=96;state.dialogue.pausedFrames++;}
        else{state.dialogue.instructionOffset=WAIT_OFFSET;state.dialogue.opcode=behavior.waitOpcode??4;state.dialogue.instructionTime=0;state.dialogue.timer++;
          state.dialogue.pausedFrames=Math.min(state.dialogue.pausedFrames+1,8);state.dialogue.currentInput=0;state.dialogue.previousInput=fire?1:0;state.dialogue.freshConfirmEdge=false;}
      }else if(tapSent){
        postTapTick++;
        const consumerTick=postTapTick-(behavior.commandLatencyTicks??0);
        if(consumerTick<1){state.dialogue.currentInput=0;state.dialogue.previousInput=0;state.dialogue.freshConfirmEdge=false;}
        else if(consumerTick===1){state.dialogue.currentInput=behavior.confirmMask??1;state.dialogue.previousInput=0;state.dialogue.freshConfirmEdge=true;
          if(!behavior.noAdvance)state.dialogue.instructionOffset=96;
          if(behavior.closeOnConfirm){state.dialogue.present=false;state.touch.context=1;}}
        else if(consumerTick<=3){state.dialogue.currentInput=behavior.confirmMask??1;state.dialogue.previousInput=behavior.confirmMask??1;state.dialogue.freshConfirmEdge=Boolean(behavior.secondEdge&&consumerTick===2);}
        else{state.dialogue.currentInput=behavior.closeOnConfirm&&fire?1:0;state.dialogue.previousInput=1;state.dialogue.freshConfirmEdge=false;}
      }
      return{advanced:true};},
    async readActions(){return{available:true,complete:true,phase:'consumer-run',sequence,stage:0,consumed:{current:fire?FIRE:0}};},
    async observe(){return structuredClone(state);},
    async requestMessage(entry){assert.equal(entry,ENTRY);requested=true;state.request={serial:1,status:1,entry,timeline:1};
      state.touch.context=2;state.dialogue.present=true;state.dialogue.message=ENTRY;state.dialogue.instructionOffset=0;state.dialogue.opcode=0;return 1;},
    async directTouchBatch(operations){this.calls.touch=operations.map(item=>({...item}));tapSent=true;return{available:true,acknowledged:true,events:[
      {acknowledged:true,type:'down',id:41,startedAtMs:10,completedAtMs:11},
      {acknowledged:true,type:'up',id:41,startedAtMs:12,completedAtMs:14}]};},
  };
}

test('fresh touch confirm uses a loaded ECL-referenced MSG Wait and consumes exactly one edge',async()=>{
  const io=makeIO();const result=await runTouchDialogueFreshConfirmScene(io,options());
  assert.equal(result.checkStatus,'passed',result.reason);
  assert.equal(result.evidence.candidate.entry,ENTRY);
  assert.equal(result.evidence.baseline.length,1);
  assert.equal(result.evidence.noTapWait.length,6);
  assert.equal(result.evidence.confirmSamples.length,4);
  assert.deepEqual(result.evidence.confirmSamples.map(row=>row.dialogue.freshConfirmEdge),[true,false,false,false]);
  assert.equal(result.evidence.confirmSamples[0].dialogue.instructionOffset,96);
  assert.equal(io.calls.touch.length,2);
  assert.equal(result.evidence.cleanup.controlsNeutral,true);
});

test('missing real ECL/MSG entry is incomplete rather than a synthetic success',async()=>{
  const io=makeIO();const actual=manifest();actual.candidates=[];actual.available=false;actual.reason='No confirm Wait entry';
  io.readManifest=async()=>actual;
  const result=await runTouchDialogueFreshConfirmScene(io,options());
  assert.equal(result.checkStatus,'incomplete');
  assert.match(result.reason,/No confirm Wait/);
});

test('a modal that closes on the real edge retires its pulse while gameplay Fire resumes',async()=>{
  const result=await runTouchDialogueFreshConfirmScene(makeIO({closeOnConfirm:true}),options());
  assert.equal(result.checkStatus,'passed',result.reason);
  assert.equal(result.evidence.ownerRetiredAtTick,1);
  assert.equal(result.evidence.confirmSamples.at(-1).touch.context,1);
});

test('confirmation follows the actual modal consumer after its declared queue latency',async()=>{
  const runOptions={...options(),consumerLatencyTicks:1};
  const result=await runTouchDialogueFreshConfirmScene(makeIO({commandLatencyTicks:1}),runOptions);
  assert.equal(result.checkStatus,'passed',result.reason);
  assert.equal(result.evidence.queuedConfirmSamples.length,1);
  assert.equal(result.evidence.queuedConfirmSamples[0].dialogue.freshConfirmEdge,false);
  assert.equal(result.evidence.confirmSamples[0].dialogue.freshConfirmEdge,true);
});

test('wrong variant in source receipt is rejected',async()=>{
  const io=makeIO();const actual=manifest();actual.source.variant='practice';io.readManifest=async()=>actual;
  await assert.rejects(runTouchDialogueFreshConfirmScene(io,options()),/adapter-declared Runtime variant/);
});

test('the native confirm bit, Wait opcode and transport threshold come from title data',async()=>{
  const io=makeIO({waitOpcode:9,confirmMask:8}),runOptions=options();
  runOptions.inputMasks.confirm=8;runOptions.waitOpcode=9;
  runOptions.gesture.maxTapDurationMs=750;runOptions.gesture.source='ports/next/input.cpp | tap threshold';
  const result=await runTouchDialogueFreshConfirmScene(io,runOptions);
  assert.equal(result.checkStatus,'passed',result.reason);
  assert.equal(result.evidence.tapDurationMs,4);
});

test('touch Fire must be consumed by gameplay before the dialogue stimulus',async()=>{
  const io=makeIO();let readSequence=0;io.readActions=async()=>({available:true,complete:true,phase:'consumer-run',sequence:++readSequence,stage:0,consumed:{current:0}});
  await assert.rejects(runTouchDialogueFreshConfirmScene(io,options()),/did not produce a consumed native gameplay Fire sample/);
});

test('held Fire cannot create a confirm edge while the real message opens',async()=>{
  const io=makeIO({autoEdge:true});
  await assert.rejects(runTouchDialogueFreshConfirmScene(io,options()),/fresh Dialogue confirm edge while opening/);
});

test('an automatic MessageProgram transition before the tap is a failure',async()=>{
  const io=makeIO({autoAdvance:true});
  await assert.rejects(runTouchDialogueFreshConfirmScene(io,options()),/left its real confirmation Wait/);
});

test('tap must advance the actual Dialogue cursor',async()=>{
  const io=makeIO({noAdvance:true});
  await assert.rejects(runTouchDialogueFreshConfirmScene(io,options()),error=>{
    assert.match(error.message,/without advancing its MessageProgram cursor/);
    assert.equal(error.evidence.schema,'eagler-touhou/touch-dialogue-confirm-scenes/1');return true;
  });
});

test('the three-tick touch pulse cannot synthesize repeated fresh edges',async()=>{
  const io=makeIO({secondEdge:true});
  await assert.rejects(runTouchDialogueFreshConfirmScene(io,options()),/more than one native press edge/);
});

test('manifest candidates must have an active ECL difficulty reference',async()=>{
  const io=makeIO();const actual=manifest();actual.candidates[0].ecl.difficultyBits=2;io.readManifest=async()=>actual;
  await assert.rejects(runTouchDialogueFreshConfirmScene(io,options()),/not enabled by the active difficulty mask/);
});

test('missing Runtime identity and persistent-write receipt do not pass',async()=>{
  const io=makeIO();io.observe=async()=>{const state=structuredClone(io.state);state.runtime.pointer=null;return state;};
  await assert.rejects(runTouchDialogueFreshConfirmScene(io,options()),/Runtime pointer identity is missing/);
  const second=makeIO();const actual=manifest();actual.fixture.committedBytes=1;second.readManifest=async()=>actual;
  await assert.rejects(runTouchDialogueFreshConfirmScene(second,options()),/committed persistent bytes/);
});
