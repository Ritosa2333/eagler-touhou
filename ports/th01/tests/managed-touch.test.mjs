import test from 'node:test';
import assert from 'node:assert/strict';
globalThis.location={origin:'http://localhost'};globalThis.addEventListener=()=>{};globalThis.document={addEventListener(){}};
const {managedInput}=await import('../build-eagler/managed-input.mjs');
const input=()=>managedInput({game:'th01',epoch:1,active:()=>true,options:()=>({netplayMode:'lan',touchSensitivity:150})});
test('quick swipe and following tap survive until the multiplayer input sample',()=>{
 const i=input(),p=(type,id,x)=>i.pointer({type,id,x,y:.5});
 p('down',1,.5);p('move',1,.4);p('up',1,.4);p('down',2,.4);p('up',2,.4);
 const value=i.nativeSample();assert.equal(Math.floor(value/2**30)%2,1);assert.equal(Math.floor(value/2**31)%2,1);assert((Math.floor(value/65536)%16384)>=8192);
 assert.equal(Math.floor(i.nativeSample()/65536)%16384,0);
});
test('multiplayer native menus receive touch swipes and taps',()=>{
 const i=input();i.pointer({type:'down',id:1,x:.5,y:.5});i.pointer({type:'move',id:1,x:.5,y:.4});i.pointer({type:'up',id:1,x:.5,y:.4});assert(i.nativeSample({menu:true})&16);
 i.pointer({type:'down',id:2,x:.5,y:.5});i.pointer({type:'up',id:2,x:.5,y:.5});assert(i.nativeSample({menu:true})&64);
});
