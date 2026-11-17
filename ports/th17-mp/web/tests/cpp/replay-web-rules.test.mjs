import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,memory,root,report} from './helpers.mjs';
test('Replay rules identify only the confirmed old web file and explicitly mark new exports',async()=>{
 const c=await core(),r=c.replay_create(),w=c.recording_create();
 const open=b=>{const p=c.allocate(b.length);try{memory(c,p,b.length).set(b);return c.replay_open(r,p,b.length);}finally{c.deallocate(p);}};
 try{
  const legacy=readFileSync(resolve(root,'../[th17] 东方鬼形兽（汉化版+日文版）/th17_01.rpy'));assert.equal(open(legacy),1);assert.equal(c.replay_web_rules(r),1);
  // Filename and metadata guessing must never reclassify an original replay.
  for(const name of ['demo1.rpy','demo2.rpy','demo3.rpy','../[th17] 东方鬼形兽（汉化版+日文版）/Th17_ud0134.rpy']){const path=name.startsWith('../')?resolve(root,name):resolve(root,'reference/assets',name);assert.equal(open(readFileSync(path)),1);assert.equal(c.replay_web_rules(r),0);}
  const legacyAgain=Buffer.from(legacy);legacyAgain[12]^=1;assert.equal(open(legacyAgain),1);assert.equal(c.replay_web_rules(r),1,'Non-game outer header does not change decoded gameplay identity');
  assert.equal(open(readFileSync(resolve(root,'reference/assets/demo1.rpy'))),1);const header=c.allocate(0x158),name=c.allocate(9);try{
   assert.equal(c.replay_initial_state(r,3,header),1);memory(c,c.recording_metadata(w),0xa0).set(memory(c,c.replay_data(r),0xa0));assert.equal(c.recording_begin(w,header,0x158),1);c.recording_activate(w);assert.equal(c.recording_tick(w,1,1,0,60,0),1);c.recording_web_rules(w);memory(c,name,9).set(Buffer.from('RULES   \0'));assert.equal(c.recording_write(w,name,1),1);
   const b=Buffer.from(memory(c,c.recording_data(w),c.recording_size(w)));assert.equal(open(b),1);assert.equal(c.replay_web_rules(r),2);assert.equal(c.replay_select(r,3),1);assert.equal(new DataView(c.memory.buffer).getUint16(c.replay_tick(r,1),true),1);
   const duplicate=Buffer.concat([b,b.subarray(-24)]);assert.equal(open(duplicate),0,'Ambiguous rule blocks rejected');b.writeUInt32LE(999,b.length-4);assert.equal(open(b),0,'Unknown future rules are explicit, not silently interpreted');assert.equal(c.replay_web_rules(r),0);
  }finally{c.deallocate(header);c.deallocate(name);}
  report('replay-web-rules',{passed:true,recognizedOldSample:'ccfa0a03ea4fdad5954b70de93b0c93359eccd557feeffa22f76ca96d718e839',scope:'Decoded full-file fingerprint for the user-confirmed early web recording, native replay isolation, explicit current web rule marker, identical held input and rejection of unknown/duplicate revisions. Historical single-stage snapshots are incomplete: this file must play continuously from Stage 1.'});
 }finally{c.replay_delete(r);c.recording_delete(w);}
});
