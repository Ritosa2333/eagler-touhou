import test from 'node:test';import assert from 'node:assert/strict';import {WorldPresentationInterpolator} from '../../site/runtime/timing.mjs';
const record={id:1,drawPass:'hostile-bullets',bank:0,sprite:0,entry:0,layer:13,mode:0,anchorX:0,anchorY:0,blend:0,color:0xffffffff,flags:6,flags2:0,x:10,y:20,width:16,height:16,sx:1,sy:1,rotation:0,u:0,v:0,uvScaleX:1,uvScaleY:1,u0:0,v0:0,u1:1,v1:1};
const snapshot=frame=>({generation:1,stage:1,frame,records:[{...record,x:frame*10}],background:[]});
test('fresh native presentation ownership skips the redundant record clone',()=>{
 const h=new WorldPresentationInterpolator(),s=snapshot(1);assert.equal(typeof h.captureOwned,'function');h.captureOwned(s);
 assert.equal(typeof h.sampleForDraw,'function');assert.strictEqual(h.sampleForDraw(1).records,s.records);
});
test('borrowed draw sampling never changes the safe public detached sample contract',()=>{
 const h=new WorldPresentationInterpolator();assert.equal(typeof h.captureOwned,'function');h.captureOwned(snapshot(1));
 const safe=h.sample(1);safe.records[0].x=900;assert.equal(h.sampleForDraw(1).records[0].x,10);
});
test('intermediate draw frames retain interpolation and detach from both native snapshots',()=>{
 const h=new WorldPresentationInterpolator(),a=snapshot(1),b=snapshot(2);assert.equal(typeof h.captureOwned,'function');h.captureOwned(a);h.captureOwned(b);
 const mid=h.sampleForDraw(.5);assert.equal(mid.records[0].x,15);mid.records[0].x=-10;assert.equal(a.records[0].x,10);assert.equal(b.records[0].x,20);
});
