import test from 'node:test';
import assert from 'node:assert/strict';
import {NativeRollback} from '../web/native-rollback.mjs';
import {packTouch,unpackTouch} from '../web/touch-input.js';
import {NativeChargeHistory} from '../web/native-charge-history.mjs';

test('charge records gate confirmation and late corrections rewind',()=>{
 let state=0;const charges=new NativeChargeHistory(),confirmed=[];
 const predicted=[0,0,-1,0],actual=[1024,0,-1,4];
 const net=new NativeRollback({seat:0,send:()=>{},capture:()=>state,restore:s=>{state=s;},
  canConfirm:f=>charges.get(f)!==undefined,
  step:(_pair,frame)=>{const sample=charges.get(frame)??predicted;charges.markUsed(frame,sample);state+=sample[0];},
  confirm:frame=>{charges.confirm(frame);confirmed.push(frame);}});
 net.receive(JSON.stringify({type:'native-rollback-input',frame:0,input:0}));
 assert.equal(net.submit(0,true),true);assert.equal(net.confirmed,0);assert.equal(state,0);
 assert.equal(charges.accept(0,actual,net.frame),true);
 charges.rewind(0);net.rewind(0);net.flush();
 assert.equal(state,1024);assert.deepEqual(confirmed,[0]);assert.equal(net.rollbacks,1);
});
test('a non-snapshot boundary proceeds only with confirmed input',()=>{
 let boundary=false,steps=0,notifications=0;
 const net=new NativeRollback({seat:0,send:()=>{},capture:()=>null,restore:()=>{},
  canConfirm:()=>boundary,onCaptureUnavailable:()=>{notifications++;boundary=true;return true;},
  step:()=>{steps++;}});
 assert.equal(net.submit(0,true),false);assert.equal(notifications,0);
 net.receive(JSON.stringify({type:'native-rollback-input',frame:0,input:0}));
 assert.equal(net.advance(false),true);assert.equal(net.confirmed,1);assert.equal(notifications,1);assert.equal(steps,1);
});

test('late input restores and replays the predicted suffix',()=>{
 let state=0;const errors=[],sent=[],confirmed=[];
 const net=new NativeRollback({seat:0,send:m=>sent.push(m),capture:()=>state,restore:s=>{state=s;},step:(pair)=>{state+=pair[0]+pair[1];},confirm:f=>confirmed.push(f),onError:e=>errors.push(e)});
 net.receive(JSON.stringify({type:'native-rollback-input',frame:0,input:1}));net.submit(1,true);
 net.submit(1,true);net.submit(1,true);assert.equal(state,6);
 net.receive(JSON.stringify({type:'native-rollback-input',frame:1,input:4}));
 assert.equal(net.rollbacks,1);assert.equal(state,12);
 net.receive(JSON.stringify({type:'native-rollback-input',frame:2,input:4}));
 assert.equal(state,12);assert.equal(net.confirmed,3);assert.deepEqual(confirmed,[0,1,2]);assert.equal(errors.length,0);
});
test('prediction is bounded and duplicate inputs are checked',()=>{
 let state=0;const errors=[];const net=new NativeRollback({seat:0,send:()=>{},capture:()=>({state}),restore:s=>{state=s.state;},step:()=>state++,onError:e=>errors.push(e),maxPrediction:2});
 assert.equal(net.submit(0,true),true);assert.equal(net.submit(0,true),true);assert.equal(net.submit(0,true),false);
 net.receive(JSON.stringify({type:'native-rollback-input',frame:1,input:1}));
 net.receive(JSON.stringify({type:'native-rollback-input',frame:1,input:2}));assert.equal(errors.length,1);
});
test('a reliable boundary holds input until both players confirm it',()=>{
 const sent=[],steps=[];let state=1;
 const net=new NativeRollback({seat:1,send:m=>sent.push(m),capture:()=>state,restore:s=>state=s,step:pair=>steps.push(pair)});
 assert.equal(net.submit(4,false),false);assert.equal(net.submit(9,false),false);assert.equal(sent.length,1);
 net.receive(JSON.stringify({type:'native-rollback-input',frame:0,input:8}));
 assert.equal(net.advance(false),true);assert.deepEqual(steps,[[8,4]]);assert.equal(net.confirmed,1);
});
test('prediction carries held controls and corrects their release',()=>{
 let state=0;const steps=[];const net=new NativeRollback({seat:0,send:()=>{},capture:()=>state,restore:s=>state=s,step:pair=>{steps.push(pair);state=pair[1];}});
 net.receive(JSON.stringify({type:'native-rollback-input',frame:0,input:512|32|8}));net.submit(0,true);net.submit(0,true);
 assert.deepEqual(steps,[[0,552],[0,552]]);
 net.receive(JSON.stringify({type:'native-rollback-input',frame:1,input:8}));
 assert.equal(net.rollbacks,1);assert.equal(state,8);assert.equal(net.confirmed,2);
});
test('held rapid fire does not repeatedly roll back and missing input stays bounded',()=>{
 let state=0;const net=new NativeRollback({seat:0,send:()=>{},capture:()=>state,restore:s=>state=s,step:pair=>{state=pair[1];}});
 net.receive(JSON.stringify({type:'native-rollback-input',frame:0,input:512}));net.submit(0,true);
 for(let frame=1;frame<100;frame++){
  assert.equal(net.submit(0,true),true);
  net.receive(JSON.stringify({type:'native-rollback-input',frame,input:512}));
 }
 assert.equal(net.rollbacks,0);
 for(let i=0;i<6;i++)assert.equal(net.submit(0,true),true);
 assert.equal(net.submit(0,true),false,'prediction stops when the peer stops sending');
 net.receive(JSON.stringify({type:'native-rollback-input',frame:100,input:0}));
 assert.equal(net.rollbacks,1);assert.equal(state,0,'confirmed release is restored');
});
test('predicted touch holds contact without replaying movement or attacks',()=>{
 const steps=[];const net=new NativeRollback({seat:0,send:()=>{},capture:()=>({}),restore:()=>{},step:pair=>steps.push(pair)});
 const input=packTouch(512|32|8,{active:true,begin:true,unlimited:true,x:120,y:-80});
 net.receive(JSON.stringify({type:'native-rollback-input',frame:0,input}));net.submit(0,true);net.submit(0,true);
 assert.equal(steps[1][1]&2047,552);
 assert.deepEqual(unpackTouch(steps[1][1]),unpackTouch(packTouch(552,{active:true,unlimited:true})));
});
test('snapshots are released on correction, confirmation and close',()=>{
 let id=0,state=0;const captured=[],released=[];
 const net=new NativeRollback({seat:0,send:()=>{},capture:()=>{const saved={id:id++,state};captured.push(saved.id);return saved;},restore:s=>{state=s.state;},release:s=>released.push(s.id),step:pair=>{state+=pair[1];}});
 net.submit(0,true);net.submit(0,true);
 net.receive(JSON.stringify({type:'native-rollback-input',frame:0,input:1}));
 net.receive(JSON.stringify({type:'native-rollback-input',frame:1,input:1}));
 net.submit(0,true);net.close();
 assert.deepEqual([...released].sort((a,b)=>a-b),captured);
 assert.equal(new Set(released).size,released.length,'each snapshot is released exactly once');
});
