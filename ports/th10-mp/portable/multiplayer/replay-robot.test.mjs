import test from 'node:test';
import assert from 'node:assert/strict';
import {probeState,allKeyboardInputs,reachedNativeStage} from './replay-robot.mjs';

function observation(count=2,frame=8){
  const words=new Array(52).fill(0);words[0]=1;words[1]=count;words[13]=frame;
  for(let seat=0;seat<count;++seat)
    words.splice(16+seat*12,12,0,380,1,2,100,2,1,0,100,4,1,0);
  return words;
}
function controls(){const words=new Array(16).fill(0);words[0]=1;return words;}

test('native dialogue uses host-only recorded skip/press/release input',()=>{
  for(const count of [2,3]){
    const control=controls();control[1]=1;
    assert.deepEqual(allKeyboardInputs(observation(count,8),control),[0x101,...new Array(count-1).fill(0)]);
    assert.deepEqual(allKeyboardInputs(observation(count,9),control),[0x100,...new Array(count-1).fill(0)]);
  }
});
test('observer does not mutate source owners and produces only keyboard bits',()=>{
  const words=observation(3),control=controls();control[5]=1;control[6]=120;control[7]=80;control[8]=1000;
  const before=structuredClone([words,control]);
  const inputs=allKeyboardInputs(words,control);
  assert.deepEqual([words,control],before);
  assert.ok(inputs.every(value=>Number.isInteger(value)&&(value&~0x1f7)===0));
  assert.ok(inputs.every(value=>(value&0x80)!==0));
});
test('result menus take precedence over a retained dialogue observation',()=>{
  const words=observation(3),control=controls();words[6]=6;words[10]=16;control[1]=1;
  assert.deepEqual(allKeyboardInputs(words,control),[1,0,0]);
});
test('malformed control observations fail closed',()=>{
  const words=observation();
  for(const bad of [[],new Array(16).fill(0),[1,...new Array(14).fill(0),NaN]])
    assert.throws(()=>probeState(words,bad),/control observation/);
  assert.equal(probeState(words).dialogue,false);
});
test('stage completion waits for the native loading boundary, not MSG stage advance',()=>{
  const status=new Array(44).fill(0),life=new Array(12).fill(0);
  status[3]=1;status[6]=2;status[7]=13344;life[1]=7;life[2]=11;
  assert.equal(reachedNativeStage(status,life,2),false);
  life[1]=11;status[4]=1;
  assert.equal(reachedNativeStage(status,life,2),false);
  status[4]=0;status[7]=0;
  assert.equal(reachedNativeStage(status,life,2),false);
  status[7]=61;
  assert.equal(reachedNativeStage(status,life,2),true);
  assert.equal(reachedNativeStage(status,life,0),false);
  assert.equal(reachedNativeStage(status,life,3),false);
  status[5]=-4;
  assert.equal(reachedNativeStage(status,life,2),false);
});
test('a cornered robot requests only an affordable native Bomb edge',()=>{
  const words=observation(2),control=controls();words[2]=1;
  words.push(0,380,0,0,500,500,1);
  assert.ok(allKeyboardInputs(words,control).every(value=>(value&2)!==0));
  words[20]=19;words[32]=0;
  assert.ok(allKeyboardInputs(words,control).every(value=>(value&2)===0));
  words[20]=100;words[23]=90;
  assert.equal(allKeyboardInputs(words,control)[0]&2,0);
});
test('clear firing lanes do not spend Power or camp at the playfield wall',()=>{
  const words=observation(2),control=controls();words[16]=-179;words[28]=-175;
  control[5]=1;control[6]=0;control[7]=100;control[8]=1000;
  const inputs=allKeyboardInputs(words,control);
  assert.ok(inputs.every(value=>(value&0x80)!==0&&(value&2)===0));
});
