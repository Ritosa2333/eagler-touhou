import {test} from 'node:test';
import assert from 'node:assert/strict';
import {AsyncLockstep} from '../web/netplay/async-lockstep.js';
import {TICK_MS,neutral} from '../web/frame-queue.js';

test('worker input wake arriving during simulation is serviced immediately',async()=>{
  let release,entered;
  const started=new Promise(resolve=>entered=resolve),waiting=new Promise(resolve=>release=resolve);
  const network={localSeat:0,capture:()=>neutral(),sendPacket(){},setPacketHandler(handler){this.handler=handler;}};
  const sync=new AsyncLockstep({network,rollbackEnabled:false,
    execute:async()=>{entered();await waiting;return {proof:'ok'};},
    confirmQueued(){},recorder:{stage(){},confirm(){}},onError:error=>{throw error;}});
  sync.running=true;sync.last=performance.now()-TICK_MS-1;sync.budget=0;
  const wakes=[];sync.wake=delay=>wakes.push(delay);sync.schedule=()=>{};
  try{
    const task=sync.pump(performance.now());await started;
    await sync.pump(performance.now());
    release();await task;
    assert.equal(wakes.at(-1),0,'do not defer a wake that arrived during the worker RPC');
  }finally{sync.running=false;sync.wakeChannel?.port1.close();sync.wakeChannel?.port2.close();}
});
