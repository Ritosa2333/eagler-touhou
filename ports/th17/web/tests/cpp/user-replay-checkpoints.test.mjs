import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve,basename} from 'node:path';
import {core,memory,root,report,sha} from './helpers.mjs';
import {expectedReplay} from '../sdl/replay-playback-fixture.mjs';

test('User original replays reach their original recorded next-stage score and stock checkpoints',async()=>{
 const observations=[],mismatches=[];
 // th17_01 was confirmed by the user as an earlier web build's export. Its
 // next-stage headers include that build's retained-state defects and cannot
 // serve as an independent native-game checkpoint. Its recording path has a
 // separate live/export/fresh-playback browser regression.
 for(const name of ['Th17_ud0134.rpy']){
  const path=resolve(root,'../[th17] 东方鬼形兽（汉化版+日文版）',name),bytes=readFileSync(path),c=await core(),r=c.replay_create(),p=c.allocate(bytes.length),entry=c.allocate(0x158);
  try{
   memory(c,p,bytes.length).set(bytes);assert.equal(c.replay_open(r,p,bytes.length),1);
   for(let stage=1;stage<=6;stage++){
    const result=await expectedReplay(path,stage),last=result.frames.at(-1),value={name:basename(path),replaySha256:sha(bytes),stage,frames:result.frames.length,last};
    if(c.replay_initial_state(r,stage+1,entry)){
     const d=new DataView(c.memory.buffer),get=off=>d.getInt32(entry+0x14+off,true);
     value.expected=[get(0x64),get(0x68),get(0x70),get(0x74),get(0x54),get(0x3c),get(0x20),get(0x48),get(0x30)];
     value.actual=last.slice(4,13);if(value.actual.some((n,i)=>n!==value.expected[i]))mismatches.push(value);
    }
    observations.push(value);console.log(JSON.stringify(value));
   }
  }finally{c.deallocate(entry);c.deallocate(p);c.replay_delete(r);}
 }
 report('user-replay-checkpoints',{passed:mismatches.length===0,observations,mismatches,scope:'Entire recorded input streams execute in production StageGameplay without protection. End-of-stage lives/pieces/bombs/power/deaths/score/point-value/graze compare to the next original replay header. Last-stage final state is reported; it has no subsequent header. This supplements the independent native callback comparison and full browser tests.'});
 assert.deepEqual(mismatches,[],'Original replay checkpoint divergence');
});
