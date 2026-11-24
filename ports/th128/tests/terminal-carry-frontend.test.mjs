import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import test from 'node:test';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const source=fs.readFileSync(path.join(root,'site/play.mjs'),'utf8');

function setFromSource(name){
  const match=new RegExp(`const ${name}=new Set\\(\\[([^\\]]*)\\]\\);`).exec(source);
  assert.ok(match,`missing ${name}`);
  return new Set(match[1].split(',').filter(Boolean).map(value=>Number(value.trim())));
}
const ordinaryStages=setFromSource('stageTransitionStages');
const terminalStages=setFromSource('terminalStageIds');

function functionSource(name){
  const start=source.indexOf(`function ${name}(`);
  assert.ok(start>=0,`missing function ${name}`);
  const open=source.indexOf('{',start);
  let depth=0;
  for(let index=open;index<source.length;++index){
    if(source[index]==='{')++depth;
    else if(source[index]==='}'&&--depth===0)return source.slice(start,index+1);
  }
  throw new Error(`unterminated ${name}`);
}
const factory=new Function('stageTransitionStages','terminalStageIds',
  `${functionSource('incomingCarryRoute')}\n${functionSource('canRestoreCarryScalars')}\nreturn {incomingCarryRoute,canRestoreCarryScalars};`);
const {incomingCarryRoute,canRestoreCarryScalars}=factory(ordinaryStages,terminalStages);

function mockCarry(route,{transitionEnabled=true,bind=true,random=true,scalar=true,ack=true}={}){
  const state={calls:[],pendingToken:17};
  try{
    if(!canRestoreCarryScalars(route,transitionEnabled))throw new Error('carry scalars require the matching route');
    if(!bind)throw new Error('bind rejected; token retained');
    state.calls.push('bind');
    if(!random)throw new Error('random rejected; token retained');
    state.calls.push('random');
    if(!scalar)throw new Error('scalar rejected; token retained');
    state.calls.push('scalar');
    if(!ack)throw new Error('ACK rejected; token retained');
    state.calls.push('ack');
    state.pendingToken=0;
    state.calls.push('clear');
    return {state};
  }catch(error){return {state,error};}
}

test('terminal gate is exact, token/capability-gated, and separate from ordinary transitions',()=>{
  assert.deepEqual([...ordinaryStages].sort((a,b)=>a-b),[1,2,4,6,7,9,11,12,14]);
  assert.deepEqual([...terminalStages].sort((a,b)=>a-b),[3,5,8,10,13,15]);
  for(const stage of terminalStages){
    assert.equal(incomingCarryRoute(stage,9,{terminalCarry:true}),'terminal');
    assert.equal(incomingCarryRoute(stage,0,{terminalCarry:true}),null);
    assert.equal(incomingCarryRoute(stage,9,{terminalCarry:false}),null);
    assert.equal(incomingCarryRoute(stage,9,{}),null);
    assert.equal(canRestoreCarryScalars('terminal',false),true);
    assert.equal(canRestoreCarryScalars('terminal',true),true);
  }
  for(const stage of ordinaryStages){
    assert.equal(incomingCarryRoute(stage,9,{terminalCarry:true}),'ordinary');
    assert.equal(incomingCarryRoute(stage,9,{}),'ordinary');
    assert.equal(canRestoreCarryScalars('ordinary',true),true);
    assert.equal(canRestoreCarryScalars('ordinary',false),false);
  }
  for(const stage of [0,16])assert.equal(incomingCarryRoute(stage,9,{terminalCarry:true}),null);
  assert.equal(canRestoreCarryScalars(null,true),false);
});

test('actual source keeps bind -> random -> scalar -> ACK -> token clear order',()=>{
  const start=source.indexOf('async function checkWorld');
  const end=source.indexOf('function prepareStage',start);
  assert.ok(start>=0&&end>start);
  const body=source.slice(start,end);
  const route=body.indexOf('const carryRoute=incomingCarryRoute');
  const bind=body.indexOf('_th128_world_bind_stage_carry_target(carryToken');
  const random=body.indexOf('_th128_world_restore_stage_carry_random(carryToken');
  const scalar=body.indexOf('module._th128_world_restore_stage_carry(carryToken)');
  const ack=body.indexOf('_th128_world_acknowledge_stage_transition(carryToken)');
  const clear=body.indexOf('state.pendingCarryToken=0');
  assert.ok(route>=0&&bind>route&&random>bind&&scalar>random&&ack>scalar&&clear>ack);
  assert.ok(!body.slice(0,ack).includes('state.pendingCarryToken=0'));
  assert.match(body,/if\(!carryRoute\)throw/);
  assert.match(body,/canRestoreCarryScalars\(carryRoute,transitionEnabled\)/);
  assert.match(source,/terminalTransitions!==false/);
  assert.match(source,/const transitionEligible=flowReady&&stageTransitionStages\.has\(stage\.id\)/);
});

test('mock native accepts a valid terminal path and ordinary path with transition',()=>{
  assert.deepEqual(mockCarry('terminal').state,{calls:['bind','random','scalar','ack','clear'],pendingToken:0});
  assert.deepEqual(mockCarry('ordinary').state,{calls:['bind','random','scalar','ack','clear'],pendingToken:0});
  const ordinaryWithoutTransition=mockCarry('ordinary',{transitionEnabled:false});
  assert.match(ordinaryWithoutTransition.error.message,/matching route/);
  assert.deepEqual(ordinaryWithoutTransition.state.calls,[]);
  assert.equal(ordinaryWithoutTransition.state.pendingToken,17);
});

test('failures before ACK retain the token and never fake ACK/clear',()=>{
  const expected={
    bind:[],random:['bind'],scalar:['bind','random'],ack:['bind','random','scalar'],
  };
  for(const [step,calls] of Object.entries(expected)){
    const result=mockCarry('terminal',{[step]:false});
    assert.match(result.error.message,/retained/);
    assert.deepEqual(result.state.calls,calls);
    assert.equal(result.state.pendingToken,17);
    assert.ok(!result.state.calls.includes('ack')||step==='ack');
    assert.notEqual(result.state.pendingToken,0);
  }
});

