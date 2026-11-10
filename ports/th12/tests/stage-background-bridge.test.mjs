import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {StageBackground,logicalBackgroundCamera,primitiveSize} from '../src/stage-background.mjs';
import {readCoreBackground,Renderer} from '../src/renderer.mjs';
const atlases=JSON.parse(fs.readFileSync(new URL('../site/assets/atlases.json',import.meta.url))),data=JSON.parse(fs.readFileSync(new URL('../site/assets/stage01-background.json',import.meta.url)));
const cameraSamples=JSON.parse(fs.readFileSync(new URL('fixtures/presentation-original.json',import.meta.url))).camera,viewSamples=JSON.parse(fs.readFileSync(new URL('fixtures/std-view-inputs-original.json',import.meta.url)));
const canonicalPose=(slot,sprite)=>[slot,sprite,0,0,0,0,0,0,1,1,0,0,1,1,0xffffffff,0,7,0,8,0,0];
function snapshot(frame=0){const original=cameraSamples.find(c=>c.frame===frame)??cameraSamples[0],argb=Number(original.fogArgb);return {loaded:1,flags:1,frame,clock:frame,camera:{position:[...original.position],direction:[...original.direction],up:[...original.up],offset:[0,0,0],eyeOffset:[0,0,1],fov:original.fov,fog:[(argb>>>16&255)/255,(argb>>>8&255)/255,(argb&255)/255,...original.fogRange]},poses:[0,0,1,1,2].map((sprite,i)=>canonicalPose(i,sprite))};}
test('STD view eye/target/up inputs equal four original430a70 samples',()=>{
  for(const r of viewSamples.records){const c=logicalBackgroundCamera({...r,fov:.5,fog:[0,0,0,0,1000],eyeOffset:[.8,.7,.6]});assert.deepEqual(c.position,r.eye,r.name);assert.deepEqual(c.target,r.target,r.name);assert.deepEqual(c.up,r.viewUp,r.name);assert.deepEqual(c.viewDirection,r.target.map((n,i)=>Math.fround(n-r.eye[i])));}
});
test('loaded C++ poses and camera bypass JS ANM/STD seeking across skipped or repeated render frames',()=>{
  const b=new StageBackground(data,atlases);b.sample=()=>{throw Error('JS STD advanced')};b.animations.sample=()=>{throw Error('JS ANM advanced')};
  const state=snapshot(599),copy=structuredClone(state),a=b.geometry(599,state),later=b.geometry(50000,state),again=b.geometry(0,state);
  assert.ok(a.triangles.length>0);assert.deepEqual(later,a);assert.deepEqual(again,a);assert.deepEqual(state,copy);assert.equal(b.frames.length,0);assert.equal(b.clock,0);assert.equal(a.authoritative,true);assert.equal(a.frame,599);
  state.poses=[];assert.deepEqual(b.geometry(999,state).triangles,[]);
});
test('primitive slot identity survives shared instances and core visibility flags hide native layer groups',()=>{
  const b=new StageBackground(data,atlases),s=snapshot(599);s.poses=s.poses.filter(p=>p[0]===4);s.poses[0][14]=0x80335577;const a=b.geometry(599,s);assert.ok(a.triangles.length>0);assert.ok(a.triangles.every(t=>t.layer===1));assert.ok(a.triangles.flatMap(t=>t.vertices).every(v=>v.alpha===128/255&&v.r===51/255&&v.g===85/255&&v.b===119/255));s.flags=0;assert.equal(b.geometry(599,s).triangles.length,0);s.flags=9;assert.equal(b.geometry(599,s).triangles.length,0);
  s.flags=1;s.poses[0][16]&=~2;assert.equal(b.geometry(599,s).triangles.length,0);
});
test('unloaded old core keeps the prior verified camera fallback',()=>{
  const b=new StageBackground(data,atlases),a=b.geometry(600,{loaded:0});assert.equal(a.authoritative,false);assert.ok(a.triangles.length>0);assert.deepEqual(a,b.geometry(600));
});
test('background JSON ABI validates bounds and reads loaded state without advancing core',()=>{
  const raw=new TextEncoder().encode(JSON.stringify(snapshot(599))),HEAPU8=new Uint8Array(raw.length+16);HEAPU8.set(raw,8);let captures=0;const core={HEAPU8,_th12_background:()=>{captures++;return 8},_th12_background_size:()=>raw.length};assert.equal(readCoreBackground(core).frame,599);assert.equal(captures,1);core._th12_background_size=()=>HEAPU8.length;assert.throws(()=>readCoreBackground(core),/背景状态/);assert.equal(readCoreBackground({}),null);
});
test('native zero and negative dimensions remain valid in sprites and zero-size STD primitives',()=>{
  const renderer={atlases:{fixture:{sprites:{0:{entry:0,x:0,y:0,w:8,h:16}}}},region:(...args)=>args};let values;renderer.region=(...args)=>{values=args};
  const a={name:'fixture',sprite:0,visible:true,alpha:1,x:0,y:0,sx:1,sy:1,rotate:false,r:1,g:1,b:1,blend:0,nativeWidth:0,nativeHeight:-6};Renderer.prototype.state.call(renderer,a,0,0);assert.deepEqual(values.slice(8,10),[0,-6]);assert.deepEqual(primitiveSize({size:[0,0]},a,{w:8,h:16}),[0,-6]);a.nativeWidth=a.nativeHeight=-1;Renderer.prototype.state.call(renderer,a,0,0);assert.deepEqual(values.slice(8,10),[8,16]);
});
