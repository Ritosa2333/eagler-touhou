import test from 'node:test';
import assert from 'node:assert/strict';
import {NativeLockstep} from '../web/native-lockstep.mjs';

test('both seats advance identical confirmed inputs despite delayed delivery', () => {
  const packets=[[],[]], seen=[[],[]], errors=[];
  const peers=[0,1].map(seat=>new NativeLockstep({seat,send:m=>packets[seat].push(m),step:(pair,frame)=>seen[seat].push([frame,...pair]),onError:e=>errors.push(e)}));
  for(let frame=0;frame<100;frame++){
    assert.equal(peers[0].submit(frame%16),true);
    assert.equal(peers[0].submit(0),false);
    peers[1].receive(packets[0].shift());
    assert.equal(peers[1].frame,frame);
    peers[1].submit(512+(frame%16));
    peers[0].receive(packets[1].shift());
  }
  assert.deepEqual(seen[0],seen[1]);assert.equal(errors.length,0);
});
test('conflicting and out-of-window input closes the session',()=>{
  const errors=[];const peer=new NativeLockstep({seat:0,send:()=>{},step:()=>{},onError:e=>errors.push(e)});
  peer.receive(JSON.stringify({type:'native-input',frame:2,input:0}));
  assert.equal(peer.closed,true);assert.equal(errors.length,1);
});
test('state proof mismatch stops synchronization',()=>{
 const packets=[[],[]],errors=[];
 const peers=[0,1].map(seat=>new NativeLockstep({seat,send:m=>packets[seat].push(m),step:()=>{},digest:()=>seat+1,onError:e=>errors.push(e)}));
 for(let frame=0;frame<121;frame++){
  peers[0].submit(0);peers[1].submit(0);
  peers[0].receive(packets[1].shift());peers[1].receive(packets[0].shift());
 }
 assert.equal(errors.length,2);assert.match(errors[0].message,/不同步/);
});
