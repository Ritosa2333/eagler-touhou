import test from'node:test';import assert from'node:assert/strict';import{bootPlay}from'../web-input/play-harness.mjs';
test('whole current page refuses a direct touch command when the paired native core lacks its actual consumer',()=>{
 const e=bootPlay();e.state.module._th128_world_state=()=>JSON.stringify({playerState:1,playerX:0,playerY:400,flow:{worldGeneration:7},message:{active:false}});
 e.input.directTouch({type:'down',id:1,x:.5,y:.5});e.input.directTouch({type:'move',id:1,x:.6,y:.5});assert.throws(()=>e.input.sampleTouch(),/触控|touch|native/i);e.input.dispose();
});
