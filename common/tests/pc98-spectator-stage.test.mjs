import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {createPc98SpectatorPublisher} from '../browser/pc98-spectator-publisher.mjs';
import {Pc98SpectatorRelay} from '../../services/relay/src/pc98-spectator.mjs';

test('TH04 publishes stage checkpoints and relay retains the latest stage', async()=>{
  const relay=new Pc98SpectatorRelay(2,{matchInitializers:true}),sent=[];
  const socket={readyState:1,bufferedAmount:0,send(packet){sent.push(packet[6]);relay.accept(packet.subarray(1));}};
  const point=(frame,text,stage)=>{const bytes=Buffer.from(text);return {frame,sha:createHash('sha256').update(bytes).digest('hex'),data:bytes.toString('base64'),scene:stage===null?null:{kind:'stage',stage}};};
  const points=[point(0,'base',null),point(2,'stage one',0),point(4,'stage two',1)];
  const tape={length:6,end:null,slice:(a,b)=>Array.from({length:b-a},()=>({}))};
  const publisher=createPc98SpectatorPublisher({game:'04',players:2},tape,()=>socket,assert.fail,()=>points);
  try{const until=Date.now()+3000;while(sent.filter(n=>n===6).length<2&&Date.now()<until)await new Promise(r=>setTimeout(r,30));
    assert.equal(sent.filter(n=>n===6).length,2);assert.equal(relay.lastPoint,4);
    const init=relay.history.find(packet=>packet[5]===6);assert.equal(init.readUInt32LE(12),4);
    assert(relay.history.some(packet=>packet[5]===4&&packet.readUInt32LE(8)===0));
  }finally{publisher.stop();}
});

test('publisher sends transferred checkpoint bytes without decoding on the page', async()=>{
  const relay=new Pc98SpectatorRelay(2,{matchInitializers:true}),sent=[];
  const socket={readyState:1,bufferedAmount:0,send(packet){sent.push(packet);relay.accept(packet.subarray(1));}};
  const bytes=new TextEncoder().encode('checkpoint bytes'),key=Symbol.for('pc98.spectator.checkpoint.bytes');
  const point={frame:0,sha:createHash('sha256').update(bytes).digest('hex'),data:'invalid base64'};
  Object.defineProperty(point,key,{value:bytes,configurable:true});
  const tape={length:0,end:null,slice:()=>[]};
  const publisher=createPc98SpectatorPublisher({game:'04',players:2},tape,()=>socket,assert.fail,()=>[point]);
  try{
    const until=Date.now()+2000;while(!sent.some(packet=>packet[6]===4)&&Date.now()<until)await new Promise(r=>setTimeout(r,20));
    const packet=sent.find(packet=>packet[6]===4);assert(packet,'checkpoint was published');
    assert.deepEqual(packet.subarray(17+72),bytes);assert.equal(point[key],undefined);
  }finally{publisher.stop();}
});

test('checkpoints progress while confirmed inputs keep arriving', async()=>{
  const relay=new Pc98SpectatorRelay(2,{matchInitializers:true}),sent=[];
  const bytes=Buffer.from('base'),points=[{frame:0,sha:createHash('sha256').update(bytes).digest('hex'),data:bytes.toString('base64')}];
  let length=300;
  const tape={get length(){return length++;},end:null,slice:(a,b)=>Array.from({length:b-a},()=>({}))};
  const socket={readyState:1,bufferedAmount:0,send(packet){sent.push(packet[6]);relay.accept(packet.subarray(1));}};
  const publisher=createPc98SpectatorPublisher({game:'04',players:2},tape,()=>socket,assert.fail,()=>points);
  try{const until=Date.now()+3000;while(!sent.includes(4)&&Date.now()<until)await new Promise(r=>setTimeout(r,20));
    assert(sent.includes(4),'checkpoint must not wait for a moving input frontier');
  }finally{publisher.stop();}
});
