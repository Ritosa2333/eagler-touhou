import test from 'node:test';
import assert from 'node:assert/strict';
import {nativeRecordPriority,orderDrawLayers,applyWorldShake} from '../../site/runtime/draw.mjs';
test('spell bonus is between its label and top HUD',()=>{
 const rows=[{id:3,layer:0,drawPass:'original-fps'},{id:2,layer:0,drawPass:'original-spell-bonus'},{id:1,layer:20}];
 assert.equal(nativeRecordPriority(rows[1]),47);assert.deepEqual(orderDrawLayers(rows,{world:true}).map(r=>r.id),[1,2,3]);
});
test('spell bonus does not follow gameplay shake',()=>{
 const row={id:30050000,layer:0,drawPass:'original-spell-bonus',x:379,y:61};assert.strictEqual(applyWorldShake(row,[10,5]),row);
});
