import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {StageBackground} from '../src/stage-background.mjs';
import {nativeQuadCorners} from '../src/renderer.mjs';
const native=JSON.parse(fs.readFileSync(new URL('fixtures/std-screen-shake-native.json',import.meta.url)));
const main=JSON.parse(fs.readFileSync(new URL('fixtures/background-main-quad-native.json',import.meta.url)));
const atlases=JSON.parse(fs.readFileSync(new URL('../site/assets/atlases.json',import.meta.url)));
const data=JSON.parse(fs.readFileSync(new URL('../site/assets/stage01-background.json',import.meta.url)));
function state(offset){return {loaded:1,flags:1,frame:0,camera:{position:[0,0,-600],direction:[0,300,600],
 up:[0,1,0],offset:[0,0,0],eyeOffset:[0,0,1],screenShake:offset,fov:.5235987901687622,fog:[0,0,0,30,1800]},
 poses:[0,0,1,1,2].map((sprite,slot)=>[slot,sprite,0,0,0,0,0,0,1,1,0,0,1,1,0xffffffff,0,7,0,8,0,0,-1,-1])};}
test('actual430a70 preserves STD View/Projection arguments and publishes Camera14 shift only to flat ANM',()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 assert.equal(native.records.length,2);const [base,shaken]=native.records;
 assert.deepEqual(shaken.viewInputs,base.viewInputs);assert.deepEqual(shaken.projectionInputs,base.projectionInputs);
 assert.deepEqual(shaken.setTransformKinds,[2,3]);assert.deepEqual(shaken.anmScreenOffset,shaken.screenShake);
 const normal=main.records.filter(r=>r.interrupt===0);
 for(const record of normal){
  const row=record.samples[0],quad=nativeQuadCorners(32,16,384,448,0,1,1,0,record.offset);
  assert.deepEqual(quad,[0,1,5,2].map(i=>row.vertices[i].slice(0,2).map(v=>v+.5)),
   'original Main81 copy translates/rounds the background exactly once');
 }
});
test('authoritative STD background projection omits Camera14 screen shift while preserving real camera position offsets',()=>{
 const background=new StageBackground(data,atlases),base=background.geometry(0,state([0,0])),shaken=background.geometry(0,state([3.25,-2.5]));
 assert.ok(base.triangles.length>0);assert.deepEqual(shaken.triangles,base.triangles,'screen shift belongs to later Main81 copy');
 const moved=state([0,0]);moved.camera.offset=[20,0,-5];
 assert.notDeepEqual(background.geometry(0,moved).triangles,base.triangles,'actual camera.offset still affects the3D view');
});
