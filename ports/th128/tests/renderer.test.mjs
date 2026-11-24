import assert from 'node:assert/strict';
import test from 'node:test';
import {WebGlRenderer} from '../site/runtime/renderer.mjs';

test('texture allocation failure closes bitmap and deletes provisional GPU texture',async()=>{
  let closed=0,deleted=0;
  const oldFetch=globalThis.fetch,oldBitmap=globalThis.createImageBitmap;
  globalThis.fetch=async()=>({ok:true,blob:async()=>({})});
  globalThis.createImageBitmap=async()=>({width:2,height:2,close:()=>closed++});
  const renderer=Object.create(WebGlRenderer.prototype);
  renderer.textures=new Map();
  renderer.gl={TEXTURE_2D:1,RGBA:2,UNSIGNED_BYTE:3,MAX_TEXTURE_SIZE:4,
    getParameter:()=>4096,createTexture:()=>({}),bindTexture:()=>{},
    texImage2D:()=>{throw new Error('injected allocation failure');},deleteTexture:()=>deleted++};
  try{
    await assert.rejects(()=>renderer.loadBank(24,{entries:[{index:0,width:2,height:2,name:'fixture',
      texture:{width:2,height:2,format:1,file:'textures/fixture.png'}}]},'http://127.0.0.1/assets/'),/allocation/);
    assert.equal(closed,1);
    assert.equal(deleted,1);
    assert.equal(renderer.textures.size,0);
  }finally{globalThis.fetch=oldFetch;globalThis.createImageBitmap=oldBitmap;}
});
