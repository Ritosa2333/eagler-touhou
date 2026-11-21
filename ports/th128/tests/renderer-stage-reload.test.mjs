import assert from 'node:assert/strict';
import test from 'node:test';
import {WebGlRenderer} from '../site/runtime/renderer.mjs';

function entry(index=0,file='textures/one.png',format=1){
  return {index,name:'fixture',width:2,height:2,texture:{width:2,height:2,format,file}};
}

async function harness(action){
  const originalFetch=globalThis.fetch,originalBitmap=globalThis.createImageBitmap;
  const events={fetches:0,closed:0,deleted:[],allocated:0};
  globalThis.fetch=async()=>{++events.fetches;return {ok:true,blob:async()=>({})};};
  globalThis.createImageBitmap=async()=>({width:2,height:2,close:()=>++events.closed});
  const renderer=Object.create(WebGlRenderer.prototype);
  renderer.textures=new Map();renderer.textureBytes=0;renderer.current=null;
  renderer.gl={TEXTURE_2D:1,RGBA:2,UNSIGNED_BYTE:3,MAX_TEXTURE_SIZE:4,TEXTURE_WRAP_S:5,TEXTURE_WRAP_T:6,
    TEXTURE_MIN_FILTER:7,TEXTURE_MAG_FILTER:8,REPEAT:9,LINEAR:10,getParameter:()=>8192,
    createTexture:()=>({id:++events.allocated}),bindTexture:()=>{},texImage2D:()=>{},texSubImage2D:()=>{},texParameteri:()=>{},
    deleteTexture:texture=>events.deleted.push(texture)};
  try{await action(renderer,events);}finally{globalThis.fetch=originalFetch;globalThis.createImageBitmap=originalBitmap;}
}

test('reloading identical immutable bank does not allocate or count texture memory again',()=>harness(async(renderer,events)=>{
  for(let count=0;count<12;++count)await renderer.loadBank(3,{entries:[entry()]},'http://127.0.0.1/assets/');
  assert.equal(renderer.textureBytes,16);assert.equal(renderer.textures.size,1);
  assert.equal(events.allocated,1);assert.equal(events.fetches,1);assert.equal(events.closed,1);
}));

test('a replaced sheet releases its GPU owner and subtracts exactly its own allocation',()=>harness(async(renderer,events)=>{
  await renderer.loadBank(3,{entries:[entry()]},'http://127.0.0.1/assets/');
  const old=renderer.textures.get('3:0').texture;
  await renderer.loadBank(3,{entries:[entry(0,'textures/two.png')]},'http://127.0.0.1/assets/');
  assert.deepEqual(events.deleted,[old]);assert.equal(renderer.textureBytes,16);assert.equal(events.allocated,2);
}));

test('obsolete entries in the replaced bank retire without touching another bank',()=>harness(async(renderer,events)=>{
  await renderer.loadBank(3,{entries:[entry(0),entry(1,'textures/two.png')]},'http://127.0.0.1/assets/');
  await renderer.loadBank(7,{entries:[entry(0)]},'http://127.0.0.1/assets/');
  const unrelated=renderer.textures.get('7:0').texture,retired=renderer.textures.get('3:1').texture;
  await renderer.loadBank(3,{entries:[entry(0)]},'http://127.0.0.1/assets/');
  assert.equal(renderer.textures.has('3:1'),false);assert.equal(renderer.textures.get('7:0').texture,unrelated);
  assert.deepEqual(events.deleted,[retired]);assert.equal(renderer.textureBytes,32);
}));

test('retired render-target ownership uses actual RGB capture dimensions',()=>harness(async(renderer,events)=>{
  const texture={id:'capture'};renderer.textures.set('6:2',{texture,dynamic:true,initialized:true,captureWidth:1280,captureHeight:960,width:640,height:480});
  renderer.textureBytes=1280*960*3;
  await renderer.loadBank(6,{entries:[]},'http://127.0.0.1/assets/');
  assert.equal(renderer.textureBytes,0);assert.deepEqual(events.deleted,[texture]);assert.equal(renderer.textures.size,0);
}));

test('unchanged 256MiB safety budget still refuses a genuine over-allocation',()=>harness(async(renderer,events)=>{
  renderer.textureBytes=256*1024*1024;
  await assert.rejects(renderer.loadBank(3,{entries:[entry()]},'http://127.0.0.1/assets/'),/budget/);
  assert.equal(events.fetches,0);assert.equal(events.allocated,0);
}));

test('an aborted repeated load cannot turn into a cached successful operation',()=>harness(async(renderer)=>{
  await renderer.loadBank(3,{entries:[entry()]},'http://127.0.0.1/assets/');
  const controller=new AbortController();controller.abort();
  await assert.rejects(renderer.loadBank(3,{entries:[entry()]},'http://127.0.0.1/assets/',{signal:controller.signal}),/Aborted/);
}));

test('a cancelled older same-bank load cannot overwrite the newer committed texture',()=>harness(async(renderer,events)=>{
  const responses=[];
  globalThis.fetch=()=>new Promise(resolve=>responses.push(resolve));
  const older=renderer.loadBank(3,{entries:[entry()]},'http://127.0.0.1/assets/');
  const oldRefusal=assert.rejects(older,/Aborted/);
  const newer=renderer.loadBank(3,{entries:[entry(0,'textures/new.png')]},'http://127.0.0.1/assets/');
  responses[1]({ok:true,blob:async()=>({})});await newer;
  const owner=renderer.textures.get('3:0');
  responses[0]({ok:true,blob:async()=>({})});await oldRefusal;
  assert.equal(renderer.textures.get('3:0'),owner);
  assert.equal(events.allocated,1);assert.equal(events.closed,2);assert.equal(renderer.textureBytes,16);
}));

test('parallel distinct banks recheck the live total budget after asynchronous reads',()=>harness(async(renderer,events)=>{
  const responses=[];
  globalThis.fetch=()=>new Promise(resolve=>responses.push(resolve));
  renderer.textureBytes=256*1024*1024-16;
  const first=renderer.loadBank(3,{entries:[entry()]},'http://127.0.0.1/assets/');
  const firstRefusal=assert.rejects(first,/budget/);
  const second=renderer.loadBank(7,{entries:[entry()]},'http://127.0.0.1/assets/');
  responses[1]({ok:true,blob:async()=>({})});await second;
  responses[0]({ok:true,blob:async()=>({})});await firstRefusal;
  assert.equal(renderer.textureBytes,256*1024*1024);
  assert.equal(renderer.textures.has('3:0'),false);assert.equal(renderer.textures.has('7:0'),true);
  assert.equal(events.allocated,1);assert.equal(events.closed,2);
}));
