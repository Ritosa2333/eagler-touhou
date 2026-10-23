import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,report,root} from './helpers.mjs';

const schema='eagler-touhou/th11-scene-schedule-reference/1';

test('TH11 scene callback order matches the independently accepted reference',async()=>{
 const referencePath=process.env.TH11_SCENE_SCHEDULE_REFERENCE;
 assert.ok(referencePath,'TH11_SCENE_SCHEDULE_REFERENCE must come from the hash-verified accepted reference receipt');
 const reference=JSON.parse(readFileSync(referencePath,'utf8'));
 assert.equal(reference.schema,schema);
 assert.equal(reference.game,'th11');
 assert.equal(reference.executableSha256,JSON.parse(readFileSync(resolve(root,'target.json'),'utf8')).sha256);
 const expected=reference.registrations;
 assert.ok(Array.isArray(expected)&&expected.length>0,'accepted scene schedule must contain registrations');
 for(const [index,row] of expected.entries()){
  assert.ok(Array.isArray(row)&&row.length===3,'registration '+index+' must contain priority, kind, and layer');
  assert.ok(row.every(Number.isInteger),'registration '+index+' fields must be integers');
 }
 const c=await core(),actual=Array.from({length:c.scene_draw_count()},(_,i)=>[0,1,2].map(k=>c.scene_draw_value(i,k)));
 assert.deepEqual(actual,expected,'candidate scene callback order differs from accepted reference');
 report('scene-schedule-original',{passed:true,registrations:actual.length,scope:'Candidate scene callback order compared with the separately accepted reference fixture.'});
});
