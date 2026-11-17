// Current page routing: the TH128 selector itself decides consumed keys; route mode remains nonmodal.
import test from 'node:test';import assert from 'node:assert/strict';
import{bootPlay}from'../web-input/play-harness.mjs';
for(const worldMode of[true,false])test('current page honors selector consumption while route keyboard remains available: '+worldMode,()=>{
 const practiceMenu={isOpen:true,keyDown:()=>worldMode},e=bootPlay({worldMode,practiceMenu});e.key('keydown','KeyZ');e.key('keyup','KeyZ');
 if(worldMode){assert.deepEqual(e.receipts,[]);assert.deepEqual(e.menus,[]);}else assert.deepEqual(e.menus,[['native','KeyZ']]);
 practiceMenu.isOpen=false;e.key('keydown','KeyX');e.key('keyup','KeyX');if(!worldMode)assert.deepEqual(e.menus.at(-1),['native','KeyX']);e.input.dispose();
});
