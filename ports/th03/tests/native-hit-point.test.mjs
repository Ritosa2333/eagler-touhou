import test from 'node:test';
import assert from 'node:assert/strict';
import {drawHitPoint,showHitPoint} from '../web/native-hit-point.mjs';
test('hit point settings are independent of focus and room rules',()=>{
 assert.equal(showHitPoint({playing:true,focus:true}),true);
 assert.equal(showHitPoint({playing:true,focus:false}),false);
 assert.equal(showHitPoint({playing:true,focus:true,focusPoint:false}),false);
 assert.equal(showHitPoint({playing:true,focus:true,focusEnabled:false}),false);
 assert.equal(showHitPoint({playing:true,focus:false,focusEnabled:false,focusPoint:false,always:true}),true);
 assert.equal(showHitPoint({playing:false,focus:true,always:true}),false);
});
test('marker is centered on the native player and does not alter canvas state',()=>{
 const calls=[],context={save(){calls.push('save');},restore(){calls.push('restore');},beginPath(){},arc(x,y,r){calls.push([x,y,r]);},fill(){},set fillStyle(value){calls.push(value);}};
 drawHitPoint(context,100,200);assert.deepEqual(calls.filter(Array.isArray).map(a=>a.slice(0,2)),[[102,202],[102,202],[102,202],[102,202]]);assert.equal(calls[0],'save');assert.equal(calls.at(-1),'restore');
 const count=calls.length;drawHitPoint(context,-1,0);assert.equal(calls.length,count);
});
