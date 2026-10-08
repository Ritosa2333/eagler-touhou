import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import createCore from '../site/runtime/core.mjs';

const native=JSON.parse(fs.readFileSync(new URL('fixtures/ecl-animation-controls-native.json',import.meta.url)));
assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
const snapshot=c=>{const pointer=c._th12_state(),size=c._th12_state_size();return JSON.parse(new TextDecoder().decode(c.HEAPU8.subarray(pointer,pointer+size)));};
const positionCommand=()=>{const b=Buffer.alloc(24);b.writeUInt16LE(300,4);b.writeUInt16LE(24,6);b[10]=63;b[11]=2;b.writeFloatLE(25,16);b.writeFloatLE(100,20);return b;};
async function ready(row){
  const c=await createCore();
  const load=(bytes,method,index=null)=>{const pointer=c._malloc(bytes.length);try{c.HEAPU8.set(bytes,pointer);assert.equal(index===null?method(pointer,bytes.length):method(index,pointer,bytes.length),1);}finally{c._free(pointer);}};
  load(fs.readFileSync(new URL('../local/retail/enemy.anm',import.meta.url)),c._th12_load_anm,1);
  // Standalone core startup selects main at(0,0). The original fixture creates
  // Probe at(25,100). Rename only synthetic metadata and add the equivalent
  // original ECL300 position instruction; property instruction bytes stay raw.
  const raw=Buffer.from(row.syntheticEclHex,'hex'),header=raw.readUInt32LE(36);
  raw.fill(0,40,header);raw.write('main',40);
  load(Buffer.concat([raw.subarray(0,header+16),positionCommand(),raw.subarray(header+16)]),c._th12_load_ecl);
  c._th12_start(0,0,1,native.seed);
  return c;
}
function poses(c){
  const nodes=new Map(snapshot(c).animations.nodes.map(n=>[n.id,n]));
  const count=c._th12_anm_draw(),pointer=c._th12_anm_draw_ptr(),view=new DataView(c.HEAPU8.buffer),out=[];
  for(let index=0;index<count;index++){
    const at=pointer+index*128;if(view.getUint32(at+108,true)!==1)continue;
    const node=nodes.get(view.getUint32(at+104,true));
    out.push({slot:node.key%256,rotation:view.getFloat32(at+20,true),scale:[view.getFloat32(at+24,true),view.getFloat32(at+28,true)],position:[view.getFloat32(at,true),view.getFloat32(at+4,true),view.getFloat32(at+8,true)],flags:view.getUint32(at+96,true)});
  }
  return out;
}
async function compare(row,withPosition=false){
  const c=await ready(row),actual=poses(c),expected=row.final.vms;
  assert.equal(actual.length,expected.length,row.name);
  for(let index=0;index<expected.length;index++){
    // The native sampler captures Enemy's slot-handle array in ascending
    // slot order; actual4612d0 registration/draw traverses Primary head order.
    // Match the same slot instead of equating those independent sequences.
    const vm=expected[index],p=actual.find(p=>p.slot===vm.slot);
    assert.ok(p,row.name+' slot '+vm.slot);
    assert.equal(p.rotation,vm.rotation[2],row.name+' rotation');
    assert.deepEqual(p.scale,vm.scale,row.name+' scale');
    assert.equal(p.flags,vm.flags,row.name+' VM flags');
    if(withPosition)assert.deepEqual(p.position,vm.enginePosition.map((n,i)=>Math.fround(n+vm.scriptPosition[i])),row.name+' engine position');
  }
  const s=snapshot(c);assert.deepEqual([s.rng.seed,s.rng.calls],row.final.rng,row.name+' RNG');
  for(const op of [277,278,279,281,444])assert.equal(new Uint32Array(c.HEAPU8.buffer,c._th12_unsupported(),1024)[op],0,row.name+' unsupported');
  return {core:c,state:s};
}
test('native277 writes raw rotation and skips floating reference resolution for a missing VM',async()=>{
  for(const row of native.cases.filter(c=>c.name.startsWith('rotation-')))await compare(row);
});
test('native278 reads operand1 twice and preserves independent random scale components',async()=>{
  for(const row of native.cases.filter(c=>c.name.startsWith('scale-')))await compare(row);
  const random=native.cases.find(c=>c.name==='scale-reference-twice').final;
  assert.notEqual(random.vms[1].scale[0],random.vms[1].scale[1]);assert.equal(random.rng[1],4);
});
test('native279 persists an offset before binding and Enemy tail applies it after ECL',async()=>{
  for(const row of native.cases.filter(c=>c.name.startsWith('offset-')))await compare(row,true);
});
test('native281 accepts slot0 and negative unlink sentinel without reparenting the VM tree',async()=>{
  for(const row of native.cases.filter(c=>c.name.startsWith('parent-')))await compare(row,true);
});
test('native444 replaces only bit26 using the low input bit, including clearing a previous value',async()=>{
  for(const row of native.cases.filter(c=>c.name.startsWith('flag444-'))){const {state}=await compare(row);assert.equal(state.enemies[0].flags&0x4000000,row.final.flags&0x4000000,row.name);}
});
