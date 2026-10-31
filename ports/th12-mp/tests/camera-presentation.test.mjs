import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {shakePresentation} from '../src/renderer.mjs';
const native=JSON.parse(fs.readFileSync(new URL('./fixtures/draw-shake-native.json',import.meta.url),'utf8'));
assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
test('native closed rings/UFO strips/curve lasers bypass shake while straight main/cap quads translate',()=>{
  assert.equal(native.records.length,6);
  for(const row of native.records)for(let draw=0;draw<row.before.length;draw++){
    const before=row.before[draw],after=row.after[draw];
    const pose={mesh:true,drawPriority:29,primitive:before.primitive==='triangle-list'?4:5,
      vertices:before.vertices.map(v=>({x:v[0],y:v[1],z:v[2],color:v[4],u:v[5],v:v[6]}))};
    const copy=structuredClone(pose),translated=shakePresentation(pose,row.offset);
    assert.equal(translated.vertices.length,after.vertices.length);
    for(let i=0;i<after.vertices.length;i++)for(const [key,field] of [['x',0],['y',1]])
      assert.ok(Math.abs(translated.vertices[i][key]-after.vertices[i][field])<.0001,`${row.name} draw${draw} vertex${i} ${key}`);
    assert.deepEqual(pose,copy,'presentation never mutates the logical record');
  }
});
test('draw42 reset keeps layer20, HUD, Font and solid shapes stable',()=>{
  const offset=native.records[0].offset;
  for(const priority of [42,44,45,50,59,60,69]){
    const pose={x:244,y:316,drawPriority:priority,a:{mode:0}};
    assert.equal(shakePresentation(pose,offset),pose);
  }
  for(const mode of [14,15,16,17,18,19,20]){
    const pose={x:244,y:316,drawPriority:18,a:{mode}};
    assert.equal(shakePresentation(pose,offset),pose);
  }
  const flat={x:244,y:316,drawPriority:18,a:{mode:1}};
  const deferred=shakePresentation(flat,offset);
  assert.deepEqual([deferred.x,deferred.y],[244,316]);
  assert.deepEqual(deferred.screenShake,offset,'quad builder applies screen offset after its native corners');
  const line={mesh:true,primitive:3,drawPriority:26,vertices:[{x:244,y:316}]};
  assert.equal(shakePresentation(line,offset),line);
});
