import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {environment} from './surface.mjs';
const url=new URL('../../site/runtime/web-input.mjs',import.meta.url),product=fs.existsSync(url)?await import(url):{};
function boot(options={}){
 assert.equal(typeof product.mountWebInput,'function','Web touch input owner is not implemented');
 const env=environment(),edges=[],held=new Set();let context='game',enabled=true;
 const input=product.mountWebInput({...env,getContext:()=>context,isEnabled:()=>enabled});
 const down=event=>{if(input.acceptKeyDown(event)){if(!held.has(event.code)){held.add(event.code);edges.push(['down',event.code]);}}};
 const up=event=>{if(input.acceptKeyUp(event)){if(held.delete(event.code))edges.push(['up',event.code]);}};
 env.window.addEventListener('keydown',down);env.window.addEventListener('keyup',up);
 const button=action=>env.find(input.layer,'data-web-action',action),pad=env.find(input.layer,'data-web-drag','1');
 return {...env,input,edges,held,button,pad,setContext:value=>context=value,setEnabled:value=>enabled=value};
}
test('two touch pointers share an action until the last finger is released',()=>{
 const e=boot();e.emit(e.button('shot'),'pointerdown',1);e.emit(e.button('shot'),'pointerdown',2);e.emit(e.button('shot'),'pointerup',1);
 assert.deepEqual(e.edges,[['down','KeyZ']]);assert(e.held.has('KeyZ'));
 e.emit(e.button('shot'),'pointerup',2);assert.deepEqual(e.edges,[['down','KeyZ'],['up','KeyZ']]);e.input.dispose();
});
test('releasing touch cannot release a key still held by the keyboard',()=>{
 const e=boot();e.key('keydown','KeyZ');e.emit(e.button('shot'),'pointerdown',1);e.emit(e.button('shot'),'pointerup',1);
 assert.deepEqual(e.edges,[['down','KeyZ']]);e.key('keyup','KeyZ');assert.deepEqual(e.edges,[['down','KeyZ'],['up','KeyZ']]);e.input.dispose();
});
test('releasing a keyboard key cannot release a touch owned action',()=>{
 const e=boot();e.emit(e.button('freeze'),'pointerdown',1);e.key('keydown','KeyX');e.key('keyup','KeyX');
 assert.deepEqual(e.edges,[['down','KeyX']]);e.emit(e.button('freeze'),'pointerup',1);assert.deepEqual(e.edges,[['down','KeyX'],['up','KeyX']]);e.input.dispose();
});
test('unmatched physical keyup cannot release a touch owned key',()=>{
 const e=boot();e.emit(e.button('focus'),'pointerdown',1);e.key('keyup','ShiftLeft');assert(e.held.has('ShiftLeft'));
 e.emit(e.button('focus'),'pointerup',1);assert(!e.held.has('ShiftLeft'));e.input.dispose();
});
test('mouse events do not claim touch input or prevent normal desktop interaction',()=>{
 const e=boot();const event=e.emit(e.button('shot'),'pointerdown',1,20,20,'mouse');
 assert.deepEqual(e.edges,[]);assert.equal(event.defaultPrevented,undefined);e.input.dispose();
});
test('pointer capture belongs to the same element that observes its release',()=>{
 const e=boot(),button=e.button('shot');e.emit(button,'pointerdown',7);assert(button.hasPointerCapture(7));assert(!e.canvas.hasPointerCapture(7));
 e.emit(button,'pointerup',7);assert(!button.hasPointerCapture(7));assert.deepEqual(e.edges,[['down','KeyZ'],['up','KeyZ']]);e.input.dispose();
});
test('lost capture releases its own action without affecting another finger',()=>{
 const e=boot();e.emit(e.button('shot'),'pointerdown',1);e.emit(e.button('freeze'),'pointerdown',2);
 e.emit(e.button('shot'),'lostpointercapture',1);assert(!e.held.has('KeyZ'));assert(e.held.has('KeyX'));
 e.emit(e.button('freeze'),'pointercancel',2);assert.equal(e.held.size,0);e.input.dispose();
});
test('drag applies the original digital diagonal directions through CSS scaling',()=>{
 const e=boot();e.canvas.rect={left:30,top:20,width:640,height:480};
 e.emit(e.pad,'pointerdown',1,100,400);e.emit(e.pad,'pointermove',1,112,388);
 assert.deepEqual([...e.held].sort(),['ArrowRight','ArrowUp']);e.emit(e.pad,'pointerup',1,112,388);assert.equal(e.held.size,0);e.input.dispose();
});
test('drag deadzone does not invent movement on small or stationary touches',()=>{
 const e=boot();e.emit(e.pad,'pointerdown',1,100,800);e.emit(e.pad,'pointermove',1,105,805);
 assert.deepEqual(e.edges,[]);e.emit(e.pad,'pointerup',1,105,805);assert.deepEqual(e.edges,[]);e.input.dispose();
});
test('letterbox bars reject movement origins but captured movement may leave the canvas',()=>{
 const e=boot();e.canvas.rect={left:0,top:0,width:1600,height:900};
 e.emit(e.pad,'pointerdown',1,10,800);e.emit(e.pad,'pointermove',1,100,800);assert.equal(e.held.size,0);
 e.emit(e.pad,'pointerdown',2,210,700);e.emit(e.pad,'pointermove',2,-100,700);assert(e.held.has('ArrowLeft'));
 e.emit(e.pad,'pointercancel',2,-100,700);assert.equal(e.held.size,0);e.input.dispose();
});
test('a repeated pointerdown cannot lose or duplicate an active action owner',()=>{
 const e=boot();e.emit(e.button('shot'),'pointerdown',1);e.emit(e.button('freeze'),'pointerdown',1);
 assert.deepEqual(e.edges,[['down','KeyZ']]);e.emit(e.button('shot'),'pointerup',1);assert.equal(e.held.size,0);e.input.dispose();
});
test('scene ownership change cancels touches without releasing physical keys',()=>{
 const e=boot();e.key('keydown','ArrowLeft');e.emit(e.button('shot'),'pointerdown',1);
 e.setContext('pause');e.input.syncContext();assert(!e.held.has('KeyZ'));assert(e.held.has('ArrowLeft'));
 e.emit(e.button('shot'),'pointermove',1);assert(!e.held.has('KeyZ'));e.key('keyup','ArrowLeft');assert.equal(e.held.size,0);e.input.dispose();
});
test('loading and inactive contexts cannot receive new pointer holds',()=>{
 const e=boot();e.setEnabled(false);e.emit(e.button('shot'),'pointerdown',1);assert.deepEqual(e.edges,[]);
 e.setEnabled(true);e.emit(e.button('shot'),'pointerup',1);assert.deepEqual(e.edges,[]);e.input.dispose();
});
test('blur forgets pointer owners without releasing another source again',()=>{
 const e=boot();e.emit(e.button('shot'),'pointerdown',1);e.held.clear();e.window.dispatchEvent({type:'blur'});
 e.emit(e.button('shot'),'pointerup',1);assert.deepEqual(e.edges,[['down','KeyZ']]);
 e.emit(e.button('shot'),'pointerdown',2);assert(e.held.has('KeyZ'));e.input.dispose();
});
test('disposal releases owned keys and removes all module lifecycle listeners',()=>{
 const e=boot(),before=e.window.listenerCount,button=e.button('shot');e.emit(button,'pointerdown',1);
 e.input.dispose();assert.equal(e.held.size,0);assert(!e.container.children.includes(e.input.layer));assert.equal(button.listenerCount,0);
 assert.equal(e.window.listenerCount,before-2);assert.equal(e.document.listenerCount,0);
 e.input.dispose();assert.deepEqual(e.edges,[['down','KeyZ'],['up','KeyZ']]);
});
test('hidden visibility releases bookkeeping and subsequent pointerup cannot reenter a new scene',()=>{
 const e=boot();e.emit(e.button('freeze'),'pointerdown',1);e.held.clear();e.document.hidden=true;e.document.dispatchEvent({type:'visibilitychange'});
 e.document.hidden=false;e.emit(e.button('freeze'),'pointerup',1);assert.equal(e.edges.length,1);e.input.dispose();
});
test('pause and return use the existing Escape and X keyboard routes',()=>{
 const e=boot();e.emit(e.button('pause'),'pointerdown',1);e.emit(e.button('pause'),'pointerup',1);
 e.emit(e.button('back'),'pointerdown',2);e.emit(e.button('back'),'pointerup',2);
 assert.deepEqual(e.edges,[['down','Escape'],['up','Escape'],['down','KeyX'],['up','KeyX']]);e.input.dispose();
});

test('right Shift held before touch focus is released through the original contributing code',()=>{
 const e=boot();e.key('keydown','ShiftRight');e.emit(e.button('focus'),'pointerdown',1);e.key('keyup','ShiftRight');
 assert(e.held.has('ShiftRight'));assert.deepEqual(e.edges,[['down','ShiftRight']]);
 e.emit(e.button('focus'),'pointerup',1);assert.equal(e.held.size,0);assert.deepEqual(e.edges,[['down','ShiftRight'],['up','ShiftRight']]);e.input.dispose();
});
test('right Shift held after touch focus preserves the same logical key until its final release',()=>{
 const e=boot();e.emit(e.button('focus'),'pointerdown',1);e.key('keydown','ShiftRight');e.emit(e.button('focus'),'pointerup',1);
 assert(e.held.has('ShiftLeft'));assert(!e.held.has('ShiftRight'));assert.deepEqual(e.edges,[['down','ShiftLeft']]);
 e.key('keyup','ShiftRight');assert.equal(e.held.size,0);assert.deepEqual(e.edges,[['down','ShiftLeft'],['up','ShiftLeft']]);e.input.dispose();
});
test('an idle touch adapter does not resample game context on every presentation',()=>{
 const env=environment();let reads=0;const input=product.mountWebInput({...env,getContext:()=>{reads++;return 1;}});
 const initial=reads;for(let i=0;i<100;i++)input.syncContext();assert.equal(reads,initial);input.dispose();
});

test('launcher key lease releases only its own holds while the keyboard remains down',()=>{
 const e=boot(),owner=Symbol('launcher');assert.equal(typeof e.input.setKeySource,'function','External input lease is missing');
 e.key('keydown','KeyZ');e.input.setKeySource(owner,['KeyZ']);e.input.clearKeySource(owner);assert.deepEqual(e.edges,[['down','KeyZ']]);
 e.key('keyup','KeyZ');assert.deepEqual(e.edges,[['down','KeyZ'],['up','KeyZ']]);e.input.dispose();
});
test('keyboard release cannot cancel a key held by the launcher source',()=>{
 const e=boot(),owner=Symbol('launcher');assert.equal(typeof e.input.setKeySource,'function','External input lease is missing');
 e.input.setKeySource(owner,['KeyZ']);e.key('keydown','KeyZ');e.key('keyup','KeyZ');assert(e.held.has('KeyZ'));
 e.input.clearKeySource(owner);assert.equal(e.held.size,0);assert.deepEqual(e.edges,[['down','KeyZ'],['up','KeyZ']]);e.input.dispose();
});
test('launcher clear does not release another touch pointer or another launcher lease',()=>{
 const e=boot(),one=Symbol('one'),two=Symbol('two');assert.equal(typeof e.input.setKeySource,'function','External input lease is missing');
 e.input.setKeySource(one,['KeyX']);e.input.setKeySource(two,['KeyX']);e.emit(e.button('freeze'),'pointerdown',1);
 e.input.clearKeySource(one);e.input.clearKeySource(two);assert(e.held.has('KeyX'));e.emit(e.button('freeze'),'pointerup',1);
 assert.deepEqual(e.edges,[['down','KeyX'],['up','KeyX']]);e.input.dispose();
});
test('repeated identical launcher key sets never emit extra edges',()=>{
 const e=boot(),owner=Symbol('launcher');assert.equal(typeof e.input.setKeySource,'function','External input lease is missing');
 e.input.setKeySource(owner,['ArrowLeft','KeyZ']);e.input.setKeySource(owner,['ArrowLeft','KeyZ']);e.input.setKeySource(owner,['KeyZ']);
 assert.deepEqual(e.edges,[['down','ArrowLeft'],['down','KeyZ'],['up','ArrowLeft']]);e.input.clearKeySource(owner);assert.equal(e.held.size,0);e.input.dispose();
});
test('scene changes forget external sources and dispose releases them exactly once',()=>{
 const e=boot(),owner=Symbol('launcher');assert.equal(typeof e.input.setKeySource,'function','External input lease is missing');
 e.input.setKeySource(owner,['KeyZ']);e.setContext('ending');e.input.syncContext();assert.equal(e.held.size,0);
 e.input.setKeySource(owner,['KeyZ']);e.input.dispose();e.input.dispose();assert.equal(e.edges.filter(row=>row[0]==='up').length,2);
});
test('invalid source identities and non game keys are rejected before changing held state',()=>{
 const e=boot();assert.equal(typeof e.input.setKeySource,'function','External input lease is missing');
 for(const source of [null,'launcher',1])assert.throws(()=>e.input.setKeySource(source,['KeyZ']),/source|owner/i);
 assert.throws(()=>e.input.setKeySource(Symbol('launcher'),['F12']),/key/i);assert.deepEqual(e.edges,[]);e.input.dispose();
});
