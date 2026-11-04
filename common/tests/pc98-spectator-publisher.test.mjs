import {test} from 'node:test';
import assert from 'node:assert/strict';
import {gunzipSync} from 'node:zlib';
import {createPc98SpectatorPublisher} from '../browser/pc98-spectator-publisher.mjs';

test('publisher sends the confirmed prefix in ordered compressed batches',async()=>{
  const packets=[],errors=[],ws={readyState:1,bufferedAmount:0,send:p=>packets.push(p)},rows=Array.from({length:121},(_,frame)=>({frame}));
  const tape={length:0,end:null,slice:(a,b)=>rows.slice(a,b)},header={players:2,game:'02'};
  const publisher=createPc98SpectatorPublisher(header,tape,()=>ws,e=>errors.push(e));
  tape.length=121;tape.end={frame:121};publisher.kick();
  for(let n=0;n<100&&!publisher.ended;n++)await new Promise(resolve=>setTimeout(resolve,10));
  assert.equal(publisher.ended,true);assert.deepEqual(errors,[]);
  assert.deepEqual(packets.map(p=>p[6]),[1,2,2,2,3]);
  const decoded=packets.filter(p=>p[6]===2).flatMap(p=>JSON.parse(gunzipSync(p.subarray(17))));
  assert.deepEqual(decoded,rows);
  assert.deepEqual(packets.filter(p=>p[6]===2).map(p=>new DataView(p.buffer).getUint32(9,true)),[0,60,120]);
  publisher.stop();
});
