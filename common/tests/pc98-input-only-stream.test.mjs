import test from 'node:test';
import assert from 'node:assert/strict';
import {createPc98SpectatorPublisher} from '../browser/pc98-spectator-publisher.mjs';
import {Pc98SpectatorRelay} from '../../services/relay/src/pc98-spectator.mjs';

test('TH03 input-only stream permits late viewers after multiple matches',async()=>{
  const relay=new Pc98SpectatorRelay(2,{matchInitializers:true}),sent=[];
  const socket={readyState:1,bufferedAmount:0,send(packet){sent.push(packet[6]);relay.accept(packet.subarray(1));}};
  const rows=[{scene:2},{scene:1},{scene:2},{scene:1}];
  const tape={length:rows.length,end:null,slice:(a,b)=>rows.slice(a,b),row:i=>rows[i]};
  const publisher=createPc98SpectatorPublisher({game:'03',players:2,spectatorInputOnly:true},tape,()=>socket,assert.fail);
  try{
    const until=Date.now()+3000;while(relay.next<rows.length&&Date.now()<until)await new Promise(r=>setTimeout(r,20));
    assert.equal(relay.next,rows.length);assert.equal(relay.pendingMatch,0);
    assert(!sent.includes(7));assert(!sent.includes(4));assert.equal(relay.history[0][5],1);
  }finally{publisher.stop();}
});
