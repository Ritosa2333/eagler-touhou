import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,report} from '../../tests/cpp/helpers.mjs';
test('Original application dispatch resets the selected stage and new-run flag for both restart destinations',async()=>{
 const m=await oracle(),base=0x4e77d0;let destroyed=0,calls=[],cases=0;
 try{
  m.replace(0x43cc00,'departing scene destructor boundary',()=>{destroyed++;return 1;});m.replace(0x43cb80,'new scene constructor boundary',()=>{calls.push({replay:m.reg('ECX'),newRun:m.u32(0x4e7ed8),stage:m.i32(0x4e73f0),starting:m.i32(0x4e73f4)});return 1;});m.view(0x503d86,1)[0]=0;
  for(const destination of [10,11])for(const starting of [1,2,3,7])for(const current of [starting,Math.min(7,starting+1),7]){
   m.i32(base+0x6f8,7);m.i32(base+0x6fc,destination);m.u32(base+0x708,0);m.i32(0x4e73f0,current);m.i32(0x4e73f4,starting);calls=[];const before=destroyed;assert.equal(m.call(0x44e600,{ecx:base}),1);assert.equal(destroyed,before+1);assert.deepEqual(calls,[{replay:destination===11?1:0,newRun:1,stage:starting,starting}]);assert.equal(m.i32(base+0x6f8),7);assert.equal(m.i32(base+0x6fc),7);assert.equal(m.u32(0x4e9bd8),0x4e3f50+starting*0xd4);cases++;
  }
  report('native-application-restart-destination',{passed:true,cases,originalFunctions:['0x44e600','0x44f6c0'],scope:'Actual original main application dispatcher executes both destination 10 and 11 branches. Only departing destructor/new constructor are explicit boundaries; new-run flag, selected initial stage, replay constructor argument, next/current destination and stage definition pointer are produced by unmodified native instructions. Real SDL pause/restart is validated separately.'});
 }finally{m.close();}
});

test('Original completion destinations create an ending only for 15 and a title/results scene for 16',async()=>{
 const m=await oracle(),base=0x4e77d0;let calls=[];try{for(const [at,name]of [[0x43cc00,'game-release'],[0x4237a0,'ending-release'],[0x423720,'ending-create'],[0x460820,'title-create']])m.replace(at,'application scene boundary '+name,()=>{calls.push(name);return 1;});m.view(0x503d86,1)[0]=0;const routes=[];for(const [previous,next]of [[7,15],[7,16],[15,16]]){m.i32(base+0x6f8,previous);m.i32(base+0x6fc,next);calls=[];assert.equal(m.call(0x44e600,{ecx:base}),1);if(next===15){assert.ok(calls.includes('ending-create'));assert.ok(!calls.includes('title-create'));}else{assert.ok(calls.includes('title-create'));assert.ok(!calls.includes('ending-create'));}routes.push({previous,next,calls:[...calls]});}report('native-application-completion-destination',{passed:true,routes,originalFunctions:['0x44e600'],scope:'Original dispatcher routes from gameplay and from ending. Scene constructors/destructors are explicit boundaries; the original branch selection between normal ending and title/results is executed unchanged.'});}finally{m.close();}
});
