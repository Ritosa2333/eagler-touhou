import test from 'node:test';
import assert from 'node:assert/strict';
import {appendMotionReplay,readMotionReplay} from '../src/replay-motion.mjs';

test('launcher inspection accepts versioned THPrac tool changes and original motion files',()=>{
  const stages=[{number:4,frames:3,motion:[]}];
  const tools=[{number:4,events:[[0,511,3,0x3e800000],[2,0,0,0x3f000000]]}];
  const file=appendMotionReplay(new Uint8Array(36),stages,false,{tools});
  const decoded=readMotionReplay(file,stages);
  assert.equal(decoded.schema,'th12-web-motion/2');
  assert.deepEqual(decoded.tools,tools);
  const old=readMotionReplay(appendMotionReplay(new Uint8Array(36),
    [{...stages[0],motion:[[1,128,0]]}]),stages);
  assert.equal(old.schema,'th12-web-motion/1');
  assert.deepEqual(old.motion(4,1),[128,0]);
});

test('tool metadata rejects out-of-stage frames, unsupported flags and duplicate changes',()=>{
  const stages=[{number:4,frames:3,motion:[]}];
  for(const events of [[[3,0,0,0]],[[0,512,0,0]],[[0,0,8,0]],
    [[0,0,0,0x3f800001]],[[0,0,0,0],[0,1,0,0]]]){
    assert.throws(()=>appendMotionReplay(new Uint8Array(36),stages,false,
      {tools:[{number:4,events}]}));
  }
});
