import test from 'node:test';import assert from 'node:assert/strict';import {quadVertices} from '../site/runtime/draw.mjs';
test('original first-stage spell portrait accepts only its unrotated zero-depth mode2 projection',()=>{
 const record={mode:2,bank:3,script:11,rx:0,ry:0,rotation:0,z:0,x:336,y:0,width:400,height:512,sx:1,sy:1,anchorX:0,anchorY:1};
 assert.deepEqual(quadVertices(record),quadVertices({...record,mode:0}));
 assert.throws(()=>quadVertices({...record,rx:1}));assert.throws(()=>quadVertices({...record,script:12}));assert.throws(()=>quadVertices({...record,z:1}));
});
