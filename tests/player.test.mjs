import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';import path from 'node:path';
import createCore from '../site/runtime/core.mjs';
const root=path.resolve(import.meta.dirname,'..');
const files=Array.from({length:6},(_,i)=>fs.readFileSync(path.join(root,'site/assets',`pl0${i>>1}${i%2?'b':'a'}.sht`)));
async function fixture(){const c=await createCore();for(let i=0;i<6;i++){const p=c._malloc(files[i].length);c.HEAPU8.set(files[i],p);assert.equal(c._th12_load_sht(i,p,files[i].length),1);c._free(p);}return c;}
function draw(c){const count=c._th12_draw(),p=c._th12_draw_ptr(),stride=c._th12_draw_stride(),v=new DataView(c.HEAPU8.buffer);return Array.from({length:count},(_,i)=>{const q=p+i*stride;return{x:v.getFloat32(q,true),y:v.getFloat32(q+4,true),scale:v.getFloat32(q+12,true),animation:v.getInt32(q+20,true),kind:v.getInt32(q+24,true),age:v.getInt32(q+32,true)};});}
test('all six retail SHT formations use native triangular indices and focus table',async()=>{
  const c=await fixture(),scripts=[18,19,11,12,15,16];
  for(let loadout=0;loadout<6;loadout++)for(let power=0;power<=400;power+=100)for(const focused of [false,true]){
    c._th12_start(loadout>>1,loadout%2,1,1);c._th12_set_initial(0,51200,power,2,2,0);
    for(let i=0;i<100;i++)c._th12_tick(focused?8:0,0);
    const options=draw(c).filter(s=>s.kind===5),count=power/100;assert.equal(options.length,count);
    const start=count*(count-1)/2+(focused?10:0);
    for(let i=0;i<count;i++){
      const offset=40+(start+i)*8;
      assert.equal(options[i].animation,scripts[loadout]);
      assert.equal(options[i].x,224+Math.trunc(files[loadout].readFloatLE(offset)*128)/128);
      assert.equal(options[i].y,416+Math.trunc(files[loadout].readFloatLE(offset+4)*128)/128);
    }
  }
});
test('retail firing point and ANM script offset survive the same-tick movement',async()=>{
  const c=await fixture();c._th12_start(0,0,1,1);c._th12_tick(1,0);
  const main=draw(c).filter(s=>s.kind===2&&s.animation===5);assert.equal(main.length,2);
  assert.deepEqual(main.map(s=>[s.x,s.y]),[[214,408],[234,408]]);
});
test('Reimu B uses a 0..14 firing cycle and finishes the burst after release',async()=>{
  const c=await fixture();c._th12_start(0,1,1,1);const spawnFrames=[];
  for(let i=0;i<24;i++){c._th12_tick(1,0);if(draw(c).some(s=>s.kind===2&&s.animation===11&&s.age===1))spawnFrames.push(i);}
  assert.deepEqual(spawnFrames,[0,8,15,23]);
  c._th12_start(0,1,1,1);const afterRelease=[];
  for(let i=0;i<18;i++){c._th12_tick(i===0?1:0,0);if(draw(c).some(s=>s.kind===2&&s.animation===5&&s.age===1))afterRelease.push(i);}
  assert.deepEqual(afterRelease,[0,3,6,9,12]);
});
test('Marisa A creates one growing attached laser per option and releases it',async()=>{
  const c=await fixture();c._th12_start(1,0,1,1);c._th12_set_initial(0,51200,400,2,2,0);c._th12_tick(1,0);
  let beams=draw(c).filter(s=>s.kind===6);assert.equal(beams.length,2);assert.ok(beams.every(s=>s.animation===7&&Math.abs(s.scale-28/448)<1e-7));
  for(let i=0;i<60;i++)c._th12_tick(1|128,0);
  beams=draw(c).filter(s=>s.kind===6);assert.equal(beams.length,4);assert.ok(beams.every(s=>s.scale===1));
  const options=draw(c).filter(s=>s.kind===5);assert.deepEqual(beams.map(s=>[s.x,s.y]).sort(),options.map(s=>[s.x,s.y]).sort());
  for(let i=0;i<16;i++)c._th12_tick(0,0);
  const ptr=c._th12_state(),size=c._th12_state_size(),snapshot=JSON.parse(new TextDecoder().decode(c.HEAPU8.subarray(ptr,ptr+size)));
  const retiring=snapshot.bullets.filter(b=>b.friendly&&b.type===2);
  assert.equal(retiring.length,4);assert.ok(retiring.every(b=>b.weapon[0]===2),'released lasers retain their native interrupt-1 retirement phase');
  for(let i=0;i<8;i++)c._th12_tick(0,0);assert.equal(draw(c).filter(s=>s.kind===6).length,0);
});
test('a malformed SHT is rejected atomically without replacing a valid loadout',async()=>{
  const c=await fixture(),b=Buffer.from(files[0]);b.writeFloatLE(NaN,40);const p=c._malloc(b.length);c.HEAPU8.set(b,p);assert.equal(c._th12_load_sht(0,p,b.length),0);c._free(p);
  c._th12_start(0,0,1,1);const o=draw(c).find(s=>s.kind===5);assert.equal(o.y,384);
});
