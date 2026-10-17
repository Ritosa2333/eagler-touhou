import test from 'node:test';
import assert from 'node:assert/strict';
import {core,memory,report} from './helpers.mjs';
test('Network inputs wait for both players and preserve ordered frames under jitter',async()=>{
 const c=await core(),peers=[c.network_create(0,6),c.network_create(1,6)],out=c.allocate(6),messages=[],history=[[],[]];let tick=0;
 const keys=(side,frame)=>(frame*73+side*1351)&65535;
 try{for(;tick<30000&&history.some(h=>h.length<12000);++tick){
  for(let side=0;side<2;++side){const p=peers[side];if(history[side].length>=12000)continue;if(c.network_wants(p)){const frame=c.network_sending(p),word=keys(side,frame);assert.equal(c.network_submit(p,side,frame,word),1);messages.push({side,frame,word,arrival:tick+(frame%113===0?35:(frame*7+side)%8)});}const before=c.network_frame(p);if(c.network_take(p,out)){const words=Array.from(new Uint16Array(c.memory.buffer,out,3));assert.deepEqual(words,before<6?[0,0,0]:[keys(0,before),keys(1,before),keys(0,before)|keys(1,before)]);history[side].push(words);}else assert.equal(c.network_frame(p),before);}
  for(let side=0;side<2;++side){for(;;){const index=messages.findIndex(m=>m.side===side);if(index<0||messages[index].arrival>tick)break;const m=messages.splice(index,1)[0];assert.equal(c.network_submit(peers[1-side],side,m.frame,m.word),1);}}
 }
 assert.equal(history[0].length,12000);assert.deepEqual(history[0],history[1]);
 for(const [frame,label] of [[5,'stale'],[7,'out of order'],[600,'excessive lead']]){const p=c.network_create(0,6);assert.equal(c.network_submit(p,1,frame,42),0,label);assert.equal(c.network_take(p,out),0,'fail closed');c.network_delete(p);}
 report('network-input',{frames:12000,checks:24000,ticks:tick,delays:'0–7 ticks and 35-tick stalls',scope:'Ordered input queue only; browser relay and game-state synchronization tested separately.'});
 }finally{for(const p of peers)c.network_delete(p);}
});
