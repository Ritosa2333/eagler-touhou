import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import createCore from '../site/runtime/core.mjs';
const native=JSON.parse(fs.readFileSync(new URL('fixtures/ecl-message-wait-original.json',import.meta.url)));
function state(core){const at=core._th12_state(),size=core._th12_state_size();return JSON.parse(new TextDecoder().decode(core.HEAPU8.subarray(at,at+size)));}
function load(core,bytes,method,index=null){const at=core._malloc(bytes.length);try{core.HEAPU8.set(bytes,at);assert.equal(index===null?method(at,bytes.length):method(index,at,bytes.length),1);}finally{core._free(at);}}
test('actual ECL419 releases on the one-frame MSG12 pulse while dialogue is still active',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 for(const row of native.records){
  const core=await createCore();
  for(const [name,index] of [['bullet.anm',0],['front.anm',7],['text.anm',8]])load(core,fs.readFileSync(new URL('../local/retail/'+name,import.meta.url)),core._th12_load_anm,index);
  const raw=Buffer.from(row.syntheticEclHex,'hex'),header=raw.readUInt32LE(36);raw.fill(0,40,header);raw.write('main',40);
  load(core,raw,core._th12_load_ecl);load(core,Buffer.from(row.syntheticMsgHex,'hex'),core._th12_load_msg,5);
  core._th12_start(2,1,3,4796);
  for(const frame of row.frames){
   if(frame.inputTick>=0)core._th12_tick(0,0);
   const actual=state(core),label=row.name+' input'+frame.inputTick;
   assert.equal(actual.ecl[0].byteOffset,frame.eclByteOffset,label+' ECL instruction');
   assert.equal(actual.ecl[0].time,frame.eclTime,label+' ECL clock');
   assert.equal(!!actual.message.active,frame.messageActive,label+' active MSG');
   if(frame.messageActive){
    assert.equal(actual.message.clock,frame.messageClock,label+' MSG clock');
    assert.equal(actual.message.wait,frame.messageWait,label+' MSG wait');
    assert.equal(actual.message.pc,frame.messageByteOffset,label+' MSG instruction');
   }
  }
 }
});
