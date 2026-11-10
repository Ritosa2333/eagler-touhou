import test from 'node:test';
import assert from 'node:assert/strict';
import {report} from './helpers.mjs';
import {titleOracle} from './title-oracle.mjs';
test('TH17 animal spirits, per-combination Extra locks, practice stages and game-start timing match original',async()=>{
 const h=await titleOracle();let scenarios=0;
 try{for(let sample=0;sample<48;sample++){
  const type=sample%3,reason=sample>=36?4:1,clear=(v,d)=>type===1?(v===sample%9&&d<4?1:0):((sample*13+v*7+d*19)%8<5?1:0),s=h.scenario({label:'animal '+sample,difficulty:sample%4,character:sample%3,animal:(sample>>1)%3,reason,checkpoint:sample%10,clear,stageAvailable:(v,d,stage)=>stage===1?0:1});
  try{
   s.ready();for(let i=0;s.get(0)!==type&&i<10;i++)s.step(0,32);assert.equal(s.get(0),type);s.step(1);s.transition();assert.equal(s.get(1),5);
   if(reason===4){s.step();assert.equal(s.get(1),6);s.step();assert.equal(s.get(1),7);s.step();assert.equal(s.get(1),8);}
   else{s.ready();s.step(1);s.transition();assert.equal(s.get(1),6);s.ready();s.step(sample&1?0x40:0x20);s.step(1);s.transition();assert.equal(s.get(1),7);s.ready();
    for(let i=0;i<5;i++)s.step(i&1?16:32);
    if(sample>>2&1){s.step(0x102);s.transition();assert.equal(s.get(1),6);scenarios++;continue;}
    s.step(0x80001);s.transition();assert.equal(s.get(1),type===2?8:2);
   }
   if(s.get(1)===8){
    s.ready();s.step(32);assert.equal(s.get(0),1);s.step(1);assert.equal(s.get(2),2,'locked practice stage stays selectable');s.step(32);
    if(sample>>3&1){s.step(0x102);s.transition();assert.equal(s.get(1),7);}
    else{s.step(1);s.transition();assert.equal(s.get(1),2);}
   }
   scenarios++;
  }finally{s.close();}
 }
 report('title-animal-menu',{passed:true,scenarios,frames:h.frames,animationComparisons:h.animationChecks,originalFunctions:['0x456250','0x456900','0x450f20'],scope:'Real animal selection and practice stage scripts, individual Extra restrictions, completion markers, input directions, cancellation, return recovery, locked-stage rejection and timing/order of game-start requests. Audio output, loading overlay object, transition effect and full game launch are explicit observed service boundaries, not integrated gameplay.'});
 }finally{h.close();}
});
