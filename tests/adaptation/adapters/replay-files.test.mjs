import test from 'node:test';
import assert from 'node:assert/strict';
import {runInNewContext} from 'node:vm';
import {callNativeReplayFile} from './replay-files.mjs';

const runtime={global:'nextRuntime',core:'exports',module:'module',app:'instance'};
function world(result=1){
  const memory=new WebAssembly.Memory({initial:1}),calls=[],freed=[];
  return{calls,freed,nextRuntime:{instance:73,exports:{memory,allocate:()=>64,deallocate:p=>freed.push(p),
    save(...args){calls.push({args,text:new TextDecoder().decode(new Uint8Array(memory.buffer,64,128)).split('\0')[0]});return result;},
    open(...args){calls.push({args,text:new TextDecoder().decode(new Uint8Array(memory.buffer,64,128)).split('\0')[0]});return result;}}}};
}
const finalize={export:'save',app:true,allocateExport:'allocate',freeExport:'deallocate',slot:1,name:'ADAPTEST',path:'replay/next.rpy'};
const opener={export:'open',app:false,allocateExport:'allocate',freeExport:'deallocate',saveRoot:'/savesnext'};
const run=(context,request)=>runInNewContext(`(${callNativeReplayFile.toString()})(request)`,{...context,request,TextEncoder});

test('serialized Replay file calls use the declared production signatures and free native strings',()=>{
  const context=world();
  assert.equal(run(context,{runtime,contract:finalize,action:'finalize'}).status,'passed');
  assert.deepEqual(context.calls[0],{args:[73,1,64],text:'ADAPTEST'});
  assert.equal(run(context,{runtime,contract:opener,action:'open',path:'replay/next.rpy',stage:2}).status,'passed');
  assert.deepEqual(context.calls[1],{args:[64,2],text:'/savesnext/replay/next.rpy'});
  assert.deepEqual(context.freed,[64,64]);
});

test('native rejection remains failed and missing export stays incomplete',()=>{
  assert.equal(run(world(0),{runtime,contract:finalize,action:'finalize'}).status,'failed');
  const context=world();delete context.nextRuntime.exports.save;
  assert.equal(run(context,{runtime,contract:finalize,action:'finalize'}).status,'incomplete');
  assert.equal(context.calls.length,0);
  assert.throws(()=>run(context,{runtime,contract:opener,action:'open',path:'../outside',stage:0}),/Invalid native Replay open path/);
});
