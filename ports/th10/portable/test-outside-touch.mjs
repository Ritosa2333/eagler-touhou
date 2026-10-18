import test from 'node:test';
import assert from 'node:assert/strict';
import {bindOutsideTouches} from '../th10_web/sdl-runtime/eagler-host.mjs';

test('outside gestures cross canvas without duplicate input and end once',()=>{
 const listeners=new Map(),calls=[];
 const doc={documentElement:{style:{}},addEventListener:(name,fn)=>listeners.set(name,fn)};
 let rect={left:80,top:0,width:640,height:480},enabled=true;
 const canvas={getBoundingClientRect:()=>rect},outside={setPointerCapture(){}};
 const core={sdl_touch:(...args)=>calls.push(args),sdl_touch_cancel:()=>calls.push(['cancel'])};
 const cancel=bindOutsideTouches(doc,canvas,()=>core,()=>enabled);
 function send(type,target=outside,extra={}){
  const e={type,target,pointerType:'touch',pointerId:4,clientX:40,clientY:240,
   preventDefault(){this.prevented=true;},stopPropagation(){this.stopped=true;},...extra};
  listeners.get(type)(e);return e;
 }
 assert(!send('pointerdown',canvas).stopped);assert.equal(calls.length,0);
 assert(!send('pointerdown',outside,{pointerType:'mouse'}).stopped);
 enabled=false;send('pointerdown');assert.equal(calls.length,0);enabled=true;
 assert(send('pointerdown').stopped);assert.deepEqual(calls[0],[0,-1073741824,-.0625,.5]);
 assert(send('pointermove',canvas,{clientX:120}).stopped);
 assert.deepEqual(calls[1],[1,-1073741824,.0625,.5]);
 rect={left:0,top:80,width:480,height:360};
 send('pointermove',outside,{clientX:240,clientY:40});
 assert.deepEqual(calls[2],[1,-1073741824,.5,-1/9]);
 send('pointerup');assert.equal(calls.at(-1)[0],2);
 const n=calls.length;send('lostpointercapture');assert.equal(calls.length,n);
 send('pointerdown');send('pointercancel');assert.deepEqual(calls.at(-1),['cancel']);
 const m=calls.length;send('pointermove');send('pointerup');assert.equal(calls.length,m);
 send('pointerdown');cancel();const k=calls.length;
 send('pointermove');send('pointerup');assert.equal(calls.length,k);
 send('pointerdown');send('lostpointercapture');assert.deepEqual(calls.at(-1),['cancel']);
});
