import test from 'node:test';
import assert from 'node:assert/strict';
import {runLoadedDialogueSkipScene} from './loaded-dialogue-skip-scenes.mjs';

const masks={fire:1,confirm:1,skip:256};
const ENTRY=3,WAIT=200;
function options(ingress){return{ingress,expected:{variant:'normal',stage:0,touchContext:2,gameplayTouchContext:1},inputMasks:masks,waitOpcode:4,consumerLatencyTicks:1,
  gesture:{skipHoldMs:500,source:'common/include/eagler/input/DialogueGesture.hpp | skip'},
  pointer:{id:71,point:{x:.5,y:.6}},
  controls:{heldFire:{fireEnabled:true,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:100},
    neutral:{fireEnabled:false,focusEnabled:false,bombSerial:0,escapeSerial:0,joystickX:0,joystickY:0,touchSensitivity:100}},
  budgets:{baselineTicks:3,openTicks:6,waitTicks:6,skipTicks:3,source:'GuiFlow native minimumWait and MessageProgram Wait timeout'},
  sources:{manifest:'GameHost loaded ECL/MSG manifest',consumer:'GuiFlow::update Dialogue consumer receipt'}};}
function manifest(){return{schema:'eagler-touhou/dialogue-touch-manifest/1',available:true,runtime:{generation:1,pointer:0x4321,epoch:2},
  fixture:{used:true,gateEnabled:true,writeLocked:true,loadScope:false,committedBytes:0},
  source:{variant:'normal',stage:0,difficultyMask:1,ecl:{asset:'ecldata1.ecl',byteLength:400,fnv64:'0123456789abcdef'},
    message:{asset:'msg1a.dat',byteLength:250,fnv64:'fedcba9876543210'}},
  candidates:[{entry:ENTRY,ecl:{timeline:2,time:10,offset:64,difficultyBits:1,referenceCount:1},
    message:{waitOffset:WAIT,waitTime:1,timeoutFrames:90,instructionCount:5}}]};}

function makeIO({noSkip=false,skipAlsoConfirm=false,wrongEntry=false,sequenceGap=false,shortPhysicalHold=false,missingConsumer=false}={}){
  let now=1000,frame=0,actionSequence=0,dialogueSequence=0,fireEnabled=false,skipHeld=false,pointerDown=false,publishedSkip=false;
  const runtime={generation:1,pointer:0x4321,epoch:2};
  const state={opened:false,present:false,message:-1,instructionOffset:-1,opcode:0,timer:0,pausedFrames:0,minimumWait:2,skippable:true,
    currentInput:0,previousInput:0,freshConfirmEdge:false,consumer:{valid:false,sequence:0}};
  const obs=()=>({available:true,runtime:{...runtime},scene:{variant:'normal',stage:0},fixture:{used:true,gateEnabled:true,writeLocked:true,loadScope:false,committedBytes:0},
    dialogue:{present:state.present,message:state.message,instructionOffset:state.instructionOffset,opcode:state.opcode,timer:state.timer,
      pausedFrames:state.pausedFrames,minimumWait:state.minimumWait,skippable:state.skippable,currentInput:state.currentInput,previousInput:state.previousInput,
      freshConfirmEdge:state.freshConfirmEdge,consumer:{...state.consumer}},touch:{context:state.present?2:1,fireEnabled}});
  const advance=async()=>{
    frame++;actionSequence++;
    if(state.present){
      const sampledSkip=(skipHeld||pointerDown&&now-pointerDown.startedAtMs>=500)&&!noSkip;
      const skip=publishedSkip;publishedSkip=sampledSkip;
      state.previousInput=state.currentInput;state.currentInput=skip?masks.skip:0;
      if(skipAlsoConfirm&&skip)state.currentInput|=masks.confirm;
      state.freshConfirmEdge=Boolean((state.currentInput&masks.confirm)&&!(state.previousInput&masks.confirm));
      state.consumer={valid:!missingConsumer,sequence:sequenceGap?dialogueSequence+2:dialogueSequence+1};dialogueSequence=state.consumer.sequence;
      if(state.instructionOffset===-1){state.instructionOffset=WAIT;state.opcode=4;state.timer=2;state.pausedFrames=0;}
      else if(state.instructionOffset===WAIT){
        if(skip){state.instructionOffset=WAIT+16;state.opcode=8;state.timer=90;}
        else state.pausedFrames++;
      }
    }else{state.currentInput=fireEnabled?masks.fire:0;state.previousInput=0;}
  };
  const action=()=>({available:true,complete:true,phase:'consumer-run',sequence:actionSequence,stage:0,consumed:{current:state.present?0:(fireEnabled?masks.fire:0),previous:0}});
  return{
    async readManifest(){return structuredClone(manifest());},
    async observe(){return structuredClone(obs());},
    async requestMessage(entry){if(wrongEntry)return{accepted:false,status:'entry-not-referenced'};assert.equal(entry,ENTRY);state.opened=true;state.present=true;state.message=ENTRY;return{accepted:true,status:'accepted',entry};},
    async controls(value){fireEnabled=value.fireEnabled;return{acknowledged:true};},
    async readActions(){return structuredClone(action());},
    async step(){await advance();},
    async waitPhysical(ms){const startedAtMs=now;now+=shortPhysicalHold?ms-1:ms;return{startedAtMs,completedAtMs:now};},
    async send(operation){
      if(operation.source==='touch'){
        const startedAtMs=now;
        if(operation.type==='down')pointerDown={id:operation.id,startedAtMs};
        if(operation.type==='up'){pointerDown=false;now++;}
        return{available:true,acknowledged:true,source:'touch',type:operation.type,id:operation.id,startedAtMs,completedAtMs:now};
      }
      assert.ok(['keyboard','gamepad'].includes(operation.source));
      if(operation.action==='skip')skipHeld=operation.down;
      return{available:true,acknowledged:true,source:operation.source,action:operation.action,down:operation.down};
    },
  };
}

test('material preparation waits inside the same native Wait before sending Skip',async()=>{
  const declaration=options('keyboard');declaration.budgets.minimumWaitTicks=5;
  const result=await runLoadedDialogueSkipScene(makeIO(),declaration);assert.equal(result.checkStatus,'passed');
  assert.ok(result.evidence.wait.at(-1).dialogue.pausedFrames>=5);
});
test('material wait cannot exceed the existing scene wait budget',async()=>{
  const declaration=options('keyboard');declaration.budgets.minimumWaitTicks=7;
  await assert.rejects(runLoadedDialogueSkipScene(makeIO(),declaration),/bounded wait budget/);
});
for(const ingress of ['keyboard','gamepad'])test(`${ingress} Skip reaches the real Dialogue consumer and advances the loaded MessageProgram`,async()=>{
  const result=await runLoadedDialogueSkipScene(makeIO(),options(ingress));
  assert.equal(result.checkStatus,'passed');
  assert.equal(result.evidence.cleanup.skipReleased,true);
  const consumed=result.evidence.skipSamples.find(sample=>sample.dialogue.currentInput&masks.skip).dialogue;
  assert.equal(consumed.currentInput&masks.skip,masks.skip);
  assert.equal(consumed.currentInput&masks.confirm,0);
  assert.notEqual(consumed.instructionOffset,WAIT);
});

test('a real long touch hold emits native Skip and does not become a confirm tap',async()=>{
  const result=await runLoadedDialogueSkipScene(makeIO(),options('touch'));
  assert.equal(result.checkStatus,'passed');
  assert.equal(result.evidence.cleanup.pointerReleased,true);
  assert.ok(result.evidence.holdDurationMs>=500);
  const consumed=result.evidence.skipSamples.find(sample=>sample.dialogue.currentInput&masks.skip).dialogue;
  assert.equal(consumed.currentInput&masks.skip,masks.skip);
  assert.equal(consumed.freshConfirmEdge,false);
});

test('a touch hold shorter than the title threshold stays incomplete',async()=>{
  const result=await runLoadedDialogueSkipScene(makeIO({shortPhysicalHold:true}),options('touch'));
  assert.equal(result.checkStatus,'incomplete');
  assert.match(result.reason,/real transport-time interval/);
});

test('an acknowledged input without a native Skip bit cannot pass',async()=>{
  const result=await runLoadedDialogueSkipScene(makeIO({noSkip:true}),options('keyboard'));
  assert.equal(result.checkStatus,'incomplete');
  assert.match(result.reason,/did not reach the actual Dialogue consumer/);
});

test('Skip that also creates a confirm edge fails the independent confirmation boundary',async()=>{
  await assert.rejects(runLoadedDialogueSkipScene(makeIO({skipAlsoConfirm:true}),options('keyboard')),
    /Skip gesture also produced a confirm input/);
});

test('a sequence gap in the GuiFlow consumer receipt fails closed',async()=>{
  const result=await runLoadedDialogueSkipScene(makeIO({sequenceGap:true}),options('keyboard'));
  assert.equal(result.checkStatus,'incomplete');
  assert.match(result.reason,/consumer sequence skipped or repeated/);
});

test('missing actual Dialogue consumer evidence is incomplete, not inferred from cursor change',async()=>{
  const result=await runLoadedDialogueSkipScene(makeIO({missingConsumer:true}),options('keyboard'));
  assert.equal(result.checkStatus,'incomplete');
  assert.match(result.reason,/did not publish a valid actual consumer receipt/);
});

test('requesting an unreferenced entry does not reuse another loaded message',async()=>{
  const result=await runLoadedDialogueSkipScene(makeIO({wrongEntry:true}),options('keyboard'));
  assert.equal(result.checkStatus,'incomplete');
  assert.match(result.reason,/rejected loaded ECL entry/);
});
