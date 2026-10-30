import test from 'node:test';
import assert from 'node:assert/strict';
import {report} from './helpers.mjs';
import {titleOracle} from './title-oracle.mjs';
test('TH17 difficulty and character controllers match original including Extra restrictions and practice return',async()=>{
 const h=await titleOracle();let scenarios=0;
 try{for(let sample=0;sample<48;sample++){
  const type=sample%3,reason=sample>=36?4:1,clear=(v,d)=>type===1?(v===sample%9&&d<4?1:0):((sample*13+v*7+d*19)%8<5?1:0),s=h.scenario({label:'selection '+sample,difficulty:sample%4,character:sample%3,animal:(sample>>1)%3,reason,clear});
  try{
   s.ready();for(let i=0;s.get(0)!==type&&i<10;i++)s.step(0,32);assert.equal(s.get(0),type);s.step(1);s.transition();assert.equal(s.get(1),5);
   if(reason===4){s.step();assert.equal(s.get(1),6);s.step();assert.equal(s.get(1),7);scenarios++;continue;}
   s.ready();s.step(sample&1?0x80:0x40);
   if(sample>>2&1){s.step(0x102);s.transition();assert.equal(s.get(1),1);scenarios++;continue;}
   s.step(0x80000);s.transition();assert.equal(s.get(1),6);s.ready();
   s.step(sample&1?0x20:0x40,sample%4?0:0x80);
   if(sample>>3&1){s.step(0x102);s.transition();assert.equal(s.get(1),5);}
   else{s.step(1);s.transition();assert.equal(s.get(1),7);}scenarios++;
  }finally{s.close();}
 }
 report('title-selection-menu',{passed:true,scenarios,frames:h.frames,animationComparisons:h.animationChecks,originalFunctions:['0x455500','0x4559b0','0x450fb0','0x450e80','0x446150'],scope:'Actual main/difficulty/character animation trees and original controllers, all difficulty choices, completion markers, per-character Extra lock, Practice, return recovery and both cancellation paths. Animal choice, gameplay start, storage and browser remain separate.'});
 }finally{h.close();}
});
