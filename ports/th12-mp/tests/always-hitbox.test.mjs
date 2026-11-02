import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {AnimationSampler} from '../src/animation.mjs';
import {alwaysHitboxPoses} from '../src/renderer.mjs';
const atlases=JSON.parse(fs.readFileSync(new URL('../site/assets/atlases.json',import.meta.url)));
test('always-visible marker retains both original settled focus rings and the native layer',()=>{
  const sampler=new AnimationSampler(atlases);
  for(const frame of [0,1,18,60,120]){
    sampler.beginFrame();const poses=alwaysHitboxPoses(sampler,frame,210,400);sampler.endFrame();
    assert.equal(poses.length,2);
    assert.deepEqual(poses.map(p=>p.a.script),[77,76]);
    for(const p of poses){
      assert.equal(p.a.sprite,380);assert.equal(p.a.sx,1);assert.equal(p.a.sy,1);
      assert.equal(p.a.alpha,1);assert.equal(p.drawPriority,34);assert.equal(p.clip,2);
      assert.deepEqual([p.x+p.a.x,p.y+p.a.y],[210,400]);
    }
  }
  assert.equal(sampler.diagnostics().unresolvedGameRandom.length,0);
});
