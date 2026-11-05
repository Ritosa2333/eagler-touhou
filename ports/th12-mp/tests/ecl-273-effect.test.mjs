import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import createCore from '../site/runtime/core.mjs';
const native=JSON.parse(fs.readFileSync(new URL('fixtures/ecl-273-effect-original.json',import.meta.url)));
function state(c){const at=c._th12_state(),size=c._th12_state_size();return JSON.parse(new TextDecoder().decode(c.HEAPU8.subarray(at,at+size)));}
function load(c,bytes,fn,bank=null){const at=c._malloc(bytes.length);c.HEAPU8.set(bytes,at);try{assert.equal(bank===null?fn(at,bytes.length):fn(bank,at,bytes.length),1);}finally{c._free(at);}}
test('actual273 Anchor trail binds a detached rotated VM with the original coordinate space and primary lifetime',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 for(const row of native.records){
  const c=await createCore();load(c,fs.readFileSync(new URL('../local/retail/stgenm04.anm',import.meta.url)),c._th12_load_anm,2);
  const raw=Buffer.from(row.syntheticEclHex,'hex'),header=raw.readUInt32LE(36);raw.fill(0,40,header);raw.write('main',40);
  load(c,raw,c._th12_load_ecl);c._th12_start(0,0,1,4660);
  for(const frame of row.frames){
   if(frame.inputTick>=0)c._th12_tick(0,0);
   const s=state(c),nodes=s.animations.chains[0].map(id=>s.animations.nodes.find(n=>n.id===id)).filter(n=>n.bank===2&&n.script===36);
   assert.equal(nodes.length,frame.nodes.length,row.name+' tick'+frame.inputTick);
   for(let i=0;i<nodes.length;i++){
    const node=nodes[i],expected=frame.nodes[i];
    assert.equal(node.clock,expected.clock);assert.equal(node.render[4],expected.sprite);assert.equal(node.render[5],expected.layer);
    assert.equal(node.render[2],expected.flags);assert.deepEqual(node.pose.slice(9,12),expected.rotation.map(n=>Number(n.toPrecision(9))));
    const poses=c._th12_anm_draw(),ptr=c._th12_anm_draw_ptr(),view=new DataView(c.HEAPU8.buffer);
    const record=Array.from({length:poses},(_,k)=>ptr+k*128).find(at=>view.getUint32(at+104,true)===node.id);
    assert.ok(record!==undefined);assert.deepEqual([0,4,8].map(offset=>view.getFloat32(record+offset,true)),expected.engine);
   }
   assert.deepEqual([s.rng.seed,s.rng.calls],frame.rng,row.name+' tick'+frame.inputTick+' RNG');
  }
 }
});
