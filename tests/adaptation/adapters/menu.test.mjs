import assert from 'node:assert/strict';
import test from 'node:test';
import {readOriginalMenuState} from './menu.mjs';

const contract={runtime:{global:'menuFixture',core:'wasm',module:'mod'},statusExport:'titleStatus',titleScene:7,
  fields:{scene:0,screen:1,cursor:2,phase:3,stateTimer2:4,confirmationReady:5,currentInput:6,previousInput:7},
  identity:{export:'identityWords',wordCount:8,magic:1234,version:1,fields:{generation:3,pointer:4,epoch:5}}};
function install(){
  const memory=new WebAssembly.Memory({initial:1}),words=new Uint32Array(memory.buffer,16,8);words.set([1234,1,8,2,512,9,0,0]);
  const status=[7,4,2,1,20,1,32,0];
  const wasm={memory,titleStatus:index=>status[index],identityWords:()=>16};
  globalThis.menuFixture={wasm,mod:{HEAPU8:new Uint8Array(memory.buffer)}};return{memory,words,status,wasm};
}
test('data-only original menu reader consumes the selected Runtime identity and native input owner',t=>{
  install();t.after(()=>delete globalThis.menuFixture);
  const serialized=(0,eval)(`(${readOriginalMenuState.toString()})`),value=serialized({contract});
  assert.deepEqual(value,{available:true,runtime:{generation:2,pointer:512,epoch:9},scene:'title',
    title:{screen:4,cursor:2,phase:1,stateTimer2:20,confirmationReady:true,input:{current:32,previous:0}}});
});
test('missing export stays unavailable and does not borrow another Runtime',t=>{
  const f=install();t.after(()=>delete globalThis.menuFixture);delete f.wasm.titleStatus;
  assert.equal(readOriginalMenuState({contract}).available,false);
});
test('malformed identity and competing native memory are rejected',t=>{
  const f=install();t.after(()=>delete globalThis.menuFixture);f.words[0]=999;
  assert.throws(()=>readOriginalMenuState({contract}),/identity ABI/);f.words[0]=1234;
  globalThis.menuFixture.mod.HEAPU8=new Uint8Array(new ArrayBuffer(65536));
  assert.throws(()=>readOriginalMenuState({contract}),/competing Runtime memories/);
});
