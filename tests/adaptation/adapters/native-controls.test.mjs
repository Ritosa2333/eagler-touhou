import test from 'node:test';
import assert from 'node:assert/strict';
import {decodeNativeControls,nativeControlPoint} from './native-controls.mjs';
function raw(){const w=Array(1024).fill(0);w.splice(0,8,1,1,10,10,1,1,1,0);w.splice(8,8,0,25,600,455,0,500,42,12);w.splice(128,12,5,300,20,30,400,20,0,1,1,10,2,0);return{available:true,words:w};}
test('Native widget target uses the actual draw generation, visible clip and enabled state',()=>{
  const r=raw(),s=decodeNativeControls(r);assert.deepEqual(nativeControlPoint(s,5),{x:220,y:40});
  for(const change of [w=>w[3]++,w=>w[135]=0,w=>w[136]=0,w=>w[131]=-100]){
    const changed=raw();change(changed.words);assert.throws(()=>nativeControlPoint(decodeNativeControls(changed),5));
  }
});
test('Native widget decoding rejects dropped, repeated, malformed and foreign-frame controls',()=>{
  for(const change of [w=>w[7]=1,w=>w[137]++,w=>w[132]=0,w=>w[129]=0,w=>{w[6]=2;w.splice(140,12,...w.slice(128,140));}]){
    const r=raw();change(r.words);assert.throws(()=>decodeNativeControls(r));
  }
});

test('Version 2 popup items use their actual window clip outside the scrolling parent',()=>{
  const r=raw(),w=r.words;w[0]=2;w[139]=1;w.splice(130,4,20,460,200,18);w.splice(140,4,10,450,300,480);
  const s=decodeNativeControls(r);assert.equal(s.controls[0].popup,true);assert.deepEqual(nativeControlPoint(s,5),{x:120,y:469});
  for(const change of [w=>w[143]=440,w=>w[139]=2,w=>w[6]=57]){const bad=structuredClone(r);change(bad.words);assert.throws(()=>decodeNativeControls(bad));}
  const clipped=structuredClone(r);clipped.words[143]=455;assert.throws(()=>nativeControlPoint(decodeNativeControls(clipped),5));
});
