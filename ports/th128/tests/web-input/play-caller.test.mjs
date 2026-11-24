import test from 'node:test';
import assert from 'node:assert/strict';
import {bootPlay} from './play-harness.mjs';
test('C and left Ctrl share one rapid-fire press and release; right Ctrl is unbound',()=>{
 for(const code of ['KeyC','ControlLeft']){
  const e=bootPlay();e.key('keydown',code);e.key('keyup',code);
  assert.deepEqual(e.receipts,[[512,512,0],[0,0,512]],code);e.input.dispose();
 }
 const e=bootPlay();e.key('keydown','ControlLeft');e.key('keydown','KeyC');
 e.key('keydown','ControlRight');e.key('keyup','ControlRight');e.key('keyup','ControlLeft');
 assert.deepEqual(e.receipts,[[512,512,0]]);
 e.key('keyup','KeyC');assert.deepEqual(e.receipts,[[512,512,0],[0,0,512]]);e.input.dispose();
 const idle=bootPlay();idle.key('keydown','ControlRight');idle.key('keyup','ControlRight');
 assert.deepEqual(idle.receipts,[]);idle.input.dispose();
});
test('production touch shot reaches real input caller with held and press then release',()=>{
 const e=bootPlay();e.emit(e.button('shot'),'pointerdown',1);e.emit(e.button('shot'),'pointerup',1);
 assert.deepEqual(e.receipts,[[1,1,0],[0,0,1]]);assert.equal(e.state.pressed.size,0);e.input.dispose();
});
test('production keyboard release keeps touch shot holding without a native release edge',()=>{
 const e=bootPlay();e.emit(e.button('shot'),'pointerdown',1);e.key('keydown','KeyZ');e.key('keyup','KeyZ');
 assert(e.state.pressed.has('KeyZ'));assert.deepEqual(e.receipts,[[1,1,0]]);
 e.emit(e.button('shot'),'pointerup',1);assert.deepEqual(e.receipts,[[1,1,0],[0,0,1]]);e.input.dispose();
});
test('production touch release leaves a keyboard owned direction active',()=>{
 const e=bootPlay();e.key('keydown','ArrowRight');e.emit(e.pad,'pointerdown',1,100,800);e.emit(e.pad,'pointermove',1,130,800);
 e.emit(e.pad,'pointerup',1,130,800);assert(e.state.pressed.has('ArrowRight'));assert.deepEqual(e.receipts,[[128,128,0]]);
 e.key('keyup','ArrowRight');assert.deepEqual(e.receipts.at(-1),[0,0,128]);e.input.dispose();
});
test('title touch navigation follows actual title menu dispatch and never calls World',()=>{
 const e=bootPlay({worldMode:false});e.emit(e.pad,'pointerdown',1,100,800);e.emit(e.pad,'pointermove',1,100,860);e.input.sampleTouch();
 e.emit(e.pad,'pointerup',1,100,860);e.input.sampleTouch();e.emit(e.button('shot'),'pointerdown',2);e.emit(e.button('shot'),'pointerup',2);
 assert.deepEqual(e.menus,[['native','ArrowDown'],['native','KeyZ']]);assert.deepEqual(e.receipts,[]);assert.equal(e.state.pressed.size,0);e.input.dispose();
});
test('postrun touch confirmation is owned by the actual normal postrun input route',()=>{
 const e=bootPlay(),events=[];e.state.normalPostrun={input:{down:code=>events.push(['down',code]),up:code=>events.push(['up',code])}};
 e.state.module._th128_world_touch_state=()=>{throw new Error('retired battle touch context was read');};
 e.input.syncContext();
 e.emit(e.button('shot'),'pointerdown',1);e.emit(e.button('shot'),'pointerup',1);
 assert.deepEqual(events,[['down','KeyZ'],['up','KeyZ']]);assert.deepEqual(e.receipts,[]);assert.equal(e.state.pressed.size,0);e.input.dispose();
});
test('pause context cancels a held touch shot through the existing pause keyup owner',()=>{
 const e=bootPlay();e.emit(e.button('shot'),'pointerdown',1);e.emit(e.button('pause'),'pointerdown',2);e.input.syncContext();
 assert.equal(e.menu.pauseOpen,true);assert(!e.state.pressed.has('KeyZ'));assert(!e.state.pressed.has('Escape'));assert.equal(e.pause[0],'pause');
 assert.deepEqual(e.receipts,[[1,1,0],[0,0,1]]);e.input.dispose();
});
test('actual X caller capability still gates touch Perfect freeze',()=>{
 for(const capability of [true,false]){
  const e=bootPlay({capability});e.emit(e.button('freeze'),'pointerdown',1);e.emit(e.button('freeze'),'pointerup',1);
  assert.deepEqual(e.receipts,capability?[[2,2,0],[0,0,2]]:[]);e.input.dispose();
 }
});
test('blur releases native input once and late touch completion does not change the paused owner',()=>{
 const e=bootPlay();e.emit(e.button('shot'),'pointerdown',1);e.window.dispatchEvent({type:'blur'});
 assert.equal(e.state.pressed.size,0);const before=e.receipts.length;e.emit(e.button('shot'),'pointerup',1);
 assert.equal(e.receipts.length,before);assert.equal(e.state.backgroundPaused,true);
 assert.equal(e.state.paused,false);assert.equal(e.menu.pauseOpen,false);e.input.dispose();
});
test('changed game action cancels touches while physical keyboard input is preserved',()=>{
 const e=bootPlay();e.key('keydown','ArrowLeft');e.emit(e.button('shot'),'pointerdown',1);e.state.action++;e.input.syncContext();
 assert(!e.state.pressed.has('KeyZ'));assert(e.state.pressed.has('ArrowLeft'));assert.deepEqual(e.receipts.at(-1),[64,0,1]);
 e.key('keyup','ArrowLeft');e.input.dispose();
});
test('ending touch confirm and key release use its existing owner and no game input',()=>{
 const e=bootPlay(),events=[];e.state.endingHost={owner:{input:{down:code=>events.push(['down',code]),up:code=>events.push(['up',code])}}};
 e.state.module._th128_world_touch_state=()=>{throw new Error('ending accessed the battle touch context');};
 e.input.syncContext();
 e.emit(e.button('shot'),'pointerdown',1);e.emit(e.button('shot'),'pointerup',1);
 assert.deepEqual(events,[['down','KeyZ'],['up','KeyZ']]);assert.deepEqual(e.receipts,[]);e.input.dispose();
});
test('pagehide disposes the production touch layer with no held key or remaining element',()=>{
 const e=bootPlay();e.emit(e.button('shot'),'pointerdown',1);e.window.dispatchEvent({type:'pagehide'});
 assert.equal(e.state.pressed.size,0);assert(!e.container.children.includes(e.input.layer));e.input.dispose();
});
test('finished replay surface sends touch selection and confirmation only to the pause menu',()=>{
 const e=bootPlay();e.state.replayPlayback={snapshot:()=>({complete:true,auxiliaryKnown:true,auxiliaryLive:false})};
 e.state.paused=true;e.menu.pauseOpen=true;e.menu.page='pause';e.input.syncContext();
 e.emit(e.pad,'pointerdown',1,100,800);e.emit(e.pad,'pointermove',1,100,860);e.input.sampleTouch();
 e.emit(e.pad,'pointerup',1,100,860);e.input.sampleTouch();
 e.emit(e.button('shot'),'pointerdown',2);e.emit(e.button('shot'),'pointerup',2);
 assert.deepEqual(e.menus,[['pause','ArrowDown'],['pause','KeyZ']]);assert.deepEqual(e.receipts,[]);e.input.dispose();
});
