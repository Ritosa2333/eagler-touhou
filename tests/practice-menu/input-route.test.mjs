// Actual page key handlers and WebInput; native input/DOM boundaries are explicit fixtures.
import test from 'node:test';import assert from 'node:assert/strict';
import {bootPlay} from '../web-input/play-harness.mjs';
for(const worldMode of [true,false])test('open practice isolates '+(worldMode?'World':'title')+' keyboard and touch input, closing restores dispatch',()=>{
 const practiceMenu={isOpen:true},e=bootPlay({worldMode,practiceMenu});
 e.key('keydown','KeyZ');e.key('keyup','KeyZ');
 e.emit(e.button('shot'),'pointerdown',8);e.emit(e.button('shot'),'pointerup',8);
 assert.deepEqual(e.receipts,[]);assert.deepEqual(e.menus,[]);assert.equal(e.state.pressed.size,0);
 practiceMenu.isOpen=false;e.key('keydown','KeyZ');e.key('keyup','KeyZ');
 if(worldMode)assert.deepEqual(e.receipts,[[1,1,0],[0,0,1]]);else assert.deepEqual(e.menus,[['native','KeyZ']]);
 e.input.dispose();
});
