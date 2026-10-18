import test from 'node:test';
import assert from 'node:assert/strict';
import {sceneFlags,stageCleared} from './original-progress.mjs';
test('Original ending receipt uses scene flags, never player mode',()=>{
  const scene=0x9000000,values=new Map([[0x4e7794,0x4000],[scene+0x90,0],[0x4e73f0,6]]),m={u32:address=>values.get(address)||0};
  assert.equal(sceneFlags(m,scene),0);assert.equal(stageCleared(m,scene,6),false);
  values.set(0x4e7794,4);values.set(scene+0x90,0x4010);assert.equal(stageCleared(m,scene,6),true);
});
test('Original intermediate stage ends only on the next-stage request',()=>{
  const scene=0x9000000,values=new Map([[scene+0x90,0],[0x4e73f0,3]]),m={u32:address=>values.get(address)||0};
  assert.equal(stageCleared(m,scene,3),false);values.set(0x4e73f0,4);assert.equal(stageCleared(m,scene,3),true);
});
