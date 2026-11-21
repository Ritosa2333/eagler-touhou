import test from 'node:test';import assert from 'node:assert/strict';import * as draws from '../site/runtime/draw.mjs';
test('MSG subtitle layout decodes actual bytes and preserves speaker positions and inline controls',()=>{
 assert.equal(typeof draws.messageTextLayout,'function');
 const state={active:true,x:180,y:256,speaker:0,font:4,lines:[[0x82,0xb3,0x81,0x5b,0x82,0xc1,0x82,0xc4,0x82,0xc6]]};
 const left=draws.messageTextLayout(state,'ja');assert.equal(left[0].text,'さーってと');assert.equal(left[0].x,180);assert.equal(left[0].align,'left');
 const right=draws.messageTextLayout({...state,x:480,speaker:1},'ja');assert.equal(right[0].x,480);assert.equal(right[0].align,'right');
 const controlled=draws.messageTextLayout({...state,lines:[[...new TextEncoder().encode('|8,18,....')],state.lines[0]]},'ja');
 assert.equal(controlled[0].text,'....');assert.equal(controlled[0].x,188);assert.equal(controlled[0].y,274);assert.equal(controlled[1].y,283);
 assert.deepEqual(draws.messageTextLayout({...state,active:false},'ja'),[]);assert.throws(()=>draws.messageTextLayout({...state,x:Infinity},'ja'));
});
