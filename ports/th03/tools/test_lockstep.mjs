import assert from 'node:assert/strict';
import {FrameQueue,neutral,BOOT_DELAY,TICK_MS} from '../web/frame-queue.js';
import {Lockstep} from '../web/lockstep.js';

const q=new FrameQueue(0);
assert.equal(q.rollbackEnabled,false);
const first=q.capture({...neutral(),actions:['shot']});
assert.equal(first.frame,BOOT_DELAY);
assert.equal(q.capture(neutral()),null,'A stalled frame cannot recapture changed input');
for(let n=0;n<BOOT_DELAY;n++){const pair=q.peek();assert(pair);q.commit(pair);q.capture(neutral());}
assert.equal(q.peek(),null,'A missing remote frame must stop advancement');
q.receive(BOOT_DELAY,neutral());assert(q.peek());
assert.throws(()=>q.receive(BOOT_DELAY,{...neutral(),actions:['up']}),/冲突/);
assert.throws(()=>q.receive(BOOT_DELAY+121,neutral()),/越界/);

const delayed=[[],[]],states=[0,0],frames=[[],[]],errors=[];
const peers=[0,1].map(seat=>new Lockstep({
  network:{localSeat:seat,capture:()=>({...neutral(),actions:[seat?'shot':'up']}),
    sendPacket:packet=>delayed[seat^1].push(structuredClone(packet))},
  step:(inputs,frame)=>{frames[seat].push(frame);states[seat]+=inputs[0].actions.length+inputs[1].actions.length;},
  hash:()=>String(states[seat]),onError:error=>errors.push(error),
}));
for(const peer of peers){peer.schedule=()=>{};peer.start();}
for(let n=0;n<900;n++){
  for(let seat=0;seat<2;seat++){
    if(seat===0||n%7===0)for(const packet of delayed[seat].splice(0))peers[seat].network.onPacket(packet);
    peers[seat].last=performance.now()-TICK_MS;peers[seat].pump();
  }
}
assert.deepEqual(errors,[]);
const common=Math.min(...frames.map(l=>l.length));assert(common>600);
assert.deepEqual(frames[0].slice(0,common),frames[1].slice(0,common),'Delayed input must preserve common frame order');
const peer=peers[0],statesBefore=peer.onState;
let mismatch='';peer.onState=value=>{mismatch=value;};const checkFrame=peer.queue.frame;peer.hashes.set(checkFrame,'same');
peer.network.onPacket({type:'check',frame:checkFrame,hash:'different'});
assert.equal(peer.running,true);assert.match(mismatch,/校验不同/);peer.onState=statesBefore;
peers[1].stop();
peers[0].stop();

const packets=[[],[]],values=[0,0],checkpoints=[new Map(),new Map()],history=[new Map(),new Map()];
let restores=0,captures=0;
const rollbackPeers=[0,1].map(seat=>new Lockstep({
  rollbackEnabled:true,
  network:{localSeat:seat,capture:()=>({...neutral(),actions:(Math.floor(rollbackPeers[seat].queue.frame/3)+seat)%2?['shot']:[]}),
    sendPacket:packet=>packets[seat^1].push(structuredClone(packet))},
  capture:frame=>{assert(!checkpoints[seat].has(frame));checkpoints[seat].set(frame,values[seat]);captures++;assert(checkpoints[seat].size<=13);},
  restore:frame=>{assert(checkpoints[seat].has(frame),`Missing checkpoint ${frame}`);values[seat]=checkpoints[seat].get(frame);
    for(const key of checkpoints[seat].keys())if(key>=frame)checkpoints[seat].delete(key);
    for(const key of history[seat].keys())if(key>frame)history[seat].delete(key);
    restores++;return frame;},
  confirm:prefix=>{for(const key of checkpoints[seat].keys())if(key<prefix)checkpoints[seat].delete(key);},
  step:(pair,frame)=>{values[seat]=(Math.imul(values[seat],31)+pair[0].actions.length+pair[1].actions.length*2)|0;history[seat].set(frame+1,values[seat]);},
  hash:()=>String(values[seat]),onError:error=>errors.push(error),
}));
for(const item of rollbackPeers){item.schedule=()=>{};item.start();}
for(let n=0;n<600;n++)for(let seat=0;seat<2;seat++){
  if(n%9===0)for(const packet of packets[seat].splice(0))rollbackPeers[seat].network.onPacket(packet);
  rollbackPeers[seat].last=performance.now()-TICK_MS;rollbackPeers[seat].pump();
}
assert.deepEqual(errors,[]);assert(restores>0);assert(captures>0);
const confirmed=Math.min(...rollbackPeers.map(item=>Math.min(item.queue.frame,item.queue.confirmed)));
assert(confirmed>150);assert.equal(history[0].get(confirmed),history[1].get(confirmed),'Replayed authoritative state must agree');
for(const item of [...peers,...rollbackPeers]){item.stop();item.wakeChannel?.port1.close();item.wakeChannel?.port2.close();}
console.log('PASS: immutable delayed frames, missing-input stall, conflict rejection, ordered dual-peer ticks, hash mismatch is reported');
console.log('PASS: delayed changing inputs trigger rollback, recapture checkpoints and converge within the snapshot window');
