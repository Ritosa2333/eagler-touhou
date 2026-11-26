import test from 'node:test';
import assert from 'node:assert/strict';
import {challengeStockLayout} from '../browser/pc98-challenge-layouts.mjs';

test('TH04 keeps P1 resident resources and per-seat table in sync',()=>{
  const h=new Uint8Array(3000),p=challengeStockLayout('04',h,100,{resource_schema:{offset:609,stride:16}},1200),marks=[];
  p.write([{lives:2,bombs:0},{lives:2,bombs:0}],0,(at,n)=>marks.push([at,n]));
  assert.equal(h[1211],2);assert.equal(h[1213],0);assert.deepEqual(p.read(2),[{lives:2,bombs:0},{lives:2,bombs:0}]);assert(marks.some(([at])=>at===1211));
  assert.throws(()=>challengeStockLayout('04',h,100,{resource_schema:{offset:609,stride:2}},1200));
});
