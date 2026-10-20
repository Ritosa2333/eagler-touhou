import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import crypto from 'node:crypto';
import {parseReplay} from '../src/replay.mjs';
import {initializeCandidate,restoreStage,state} from '../tools/replay-verifier/capture-candidate.mjs';
import {readCoreAnimationPoses,readCoreMeshes} from '../src/renderer.mjs';
import {readCoreDrawSchedule,orderPresentation} from '../src/draw-scheduler.mjs';

test('retail th12_14 Sanae B leaves use their original Primary draw chain and whole LineStrip mesh without old segment duplicates',async()=>{
 const bytes=fs.readFileSync(new URL('../site/replays/th12_14.rpy',import.meta.url));
 assert.equal(crypto.createHash('sha256').update(bytes).digest('hex'),
  'c92747c90ceacc9907440dd03b00afd88ef206bc76abfe1e64b7bc1de24ab516');
 const replay=parseReplay(bytes),stage=replay.stages.find(s=>s.number===1),core=await initializeCandidate(1);
 restoreStage(core,replay,stage);core._th12_oracle_fixture(0);
 let firstBomb=-1;
 for(let tick=0;tick<stage.frames;tick++){
  const input=replay.input(1,tick);if(firstBomb<0&&(input.pressed&2))firstBomb=tick;
  core._th12_tick(input.held,input.pressed);
  if(firstBomb<0||tick!==firstBomb+40)continue;
  const snapshot=state(core),poses=readCoreAnimationPoses(core),meshes=readCoreMeshes(core);
  const schedule=readCoreDrawSchedule(core),ordered=orderPresentation([...poses,...meshes],schedule);
  const leaves=snapshot.player.bombState.leaves.filter(leaf=>leaf.state[3]);
  assert.ok(leaves.length>=8,'multiple actual leaf nodes from original replay Bomb');
  const leafIds=leaves.map(leaf=>leaf.state[0]),owners=new Set(leafIds.map(id=>0xa0000000+id));
  const presented=ordered.filter(p=>p.mesh&&owners.has(p.owner));
  assert.deepEqual(presented.map(p=>p.owner),leafIds.map(id=>0xa0000000+id),'native Primary list order');
  for(const leaf of leaves){
   const owner=0xa0000000+leaf.state[0],mesh=presented.find(p=>p.owner===owner);
   assert.ok(mesh,'one custom mesh per leaf');assert.equal(mesh.primitive,3);assert.equal(mesh.untextured,true);
   assert.equal(mesh.renderPass,0);assert.equal(mesh.a.layer,13);assert.equal(mesh.drawPriority,26);
   assert.equal(mesh.a.blend,0);assert.equal(mesh.clip,2);
   assert.ok(schedule.has(mesh.id),'original registered node schedule retained');
   assert.equal(mesh.vertices.length,leaf.state[1]);
   assert.ok(!poses.some(p=>p.owner===owner),'transparent factory script0 creates no ordinary quad');
   for(let i=0;i<mesh.vertices.length;i++){
    const p=leaf.points[i],v=mesh.vertices[i];
    assert.deepEqual([v.x,v.y,v.color],[Math.fround(p[0]),Math.fround(p[1]),p[2]],'screen-space leaf points reach renderer unchanged');
   }
  }
  const count=core._th12_draw(),pointer=core._th12_draw_ptr(),stride=core._th12_draw_stride(),view=new DataView(core.HEAPU8.buffer);
  for(let i=0;i<count;i++)assert.notEqual(view.getInt32(pointer+i*stride+24,true),13,
   'old per-segment fallback must not duplicate native leaf mesh');
  return;
 }
 assert.fail('original replay did not reach its first active Sanae B leaf presentation');
});
