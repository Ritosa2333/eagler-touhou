import test from 'node:test';
import assert from 'node:assert/strict';
import {interpolatePresentation,capturePresentationOwners} from '../src/presentation-interpolation.mjs';
const PLAYER=0xffff0000;
function endpoint(frame,x,{native=true,legacy=false,revision=1}={}){
  const poses=[{owner:PLAYER,presentationKey:'player-hitbox:0',x,y:400,drawPriority:34,clip:2,a:{name:'bullet',sprite:380,mode:1}}];
  const owners=[{id:PLAYER,revision,kind:'player',position:[x,400,0]}];
  const state={epoch:1,frame,owners,nodes:[],lasers:[],ownerMap:new Map(owners.map(o=>[o.id,o])),nodeMap:new Map(),laserMap:new Map()};
  const hud=[frame,x-224,384],records=[{owner:PLAYER,animation:0,age:frame}];
  return {epoch:1,frame,stage:6,poses,records,nativeMotion:native?state:null,motion:legacy?capturePresentationOwners(hud,records,poses):null};
}
test('authoritative interpolation works without allocating a duplicate legacy graph',()=>{
  const a=endpoint(10,210),b=endpoint(11,214),before=structuredClone(b);
  assert.deepEqual(interpolatePresentation(a,b,.5).poses[0].presentationOffset,[-2,0]);
  assert.deepEqual(b,before,'render does not mutate the fixed endpoint');
  assert.equal(interpolatePresentation(a,b,1),b);
});
test('native discontinuities still snap without the fallback graph',()=>{
  const a=endpoint(10,210),b=endpoint(11,214,{revision:2});
  assert.equal(interpolatePresentation(a,b,.5).poses[0],b.poses[0]);
});
test('locked60 Hz endpoints without either graph remain valid',()=>{
  const a=endpoint(10,210,{native:false}),b=endpoint(11,214,{native:false});
  assert.equal(interpolatePresentation(a,b,.5),b);
});
test('older cores retain their legacy interpolation fallback',()=>{
  const a=endpoint(10,210,{native:false,legacy:true}),b=endpoint(11,214,{native:false,legacy:true});
  assert.deepEqual(interpolatePresentation(a,b,.5).poses[0].presentationOffset,[-2,-0]);
});
