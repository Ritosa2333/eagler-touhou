import assert from 'node:assert/strict';
import {installFramePresenter} from '../web/frame-presenter.js';

const uniforms=new Map();
const gl=new Proxy({
  getShaderParameter:()=>true,getProgramParameter:()=>true,isContextLost:()=>false,
  getAttribLocation:()=>0,getUniformLocation:(_p,name)=>name,
  uniform1i:(name,value)=>uniforms.set(name,value),
  uniform3f:(name,...values)=>uniforms.set(name,values),
  createShader:()=>({}),createProgram:()=>({}),createBuffer:()=>({}),createTexture:()=>({}),
}, {get(target,key){return key in target?target[key]:(typeof key==='string'&&key===key.toUpperCase()?1:()=>{});}});
const heap=new Uint8Array(640*400*3+12000),palette=heap.length-3;
new DataView(heap.buffer).setInt16(palette-0x26de,100,true);
const mod={gl,HEAPU8:heap,sidebarPaletteAddress:palette};
const presenter=installFramePresenter(mod,{deferred:true});
heap.set([0xf0,0x80,0x20],palette);
mod.updateTexture(0,640,400);
heap.set([0x10,0x20,0x30],palette);
presenter.flush();
assert.equal(uniforms.get('normalizeSidebar'),1);
assert.deepEqual(uniforms.get('sidebarBackground'),[1,138/255,32/255]);
mod.updateTexture(0,640,400);presenter.flush();
assert.deepEqual(uniforms.get('sidebarBackground'),[16/255,32/255,48/255]);
presenter.flush({redraw:true});
assert.deepEqual(uniforms.get('sidebarBackground'),[16/255,32/255,48/255]);
new DataView(heap.buffer).setInt16(palette-0x26de,50,true);
heap.set([0xf0,0x80,0x20],palette);mod.updateTexture(0,640,400);presenter.flush();
assert.deepEqual(uniforms.get('sidebarBackground'),[117/255,69/255,16/255]);
new DataView(heap.buffer).setInt16(palette-0x26de,150,true);
mod.updateTexture(0,640,400);presenter.flush();
assert.deepEqual(uniforms.get('sidebarBackground'),[1,207/255,154/255]);
presenter.dispose();
console.log('TH04 deferred frame palette association passed');



