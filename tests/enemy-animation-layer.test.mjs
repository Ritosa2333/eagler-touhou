import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import createCore from '../site/runtime/core.mjs';
import {initializeCandidate,state as worldState} from '../tools/replay-verifier/capture-candidate.mjs';

const native=JSON.parse(fs.readFileSync(new URL('fixtures/enemy-animation-layer-native.json',import.meta.url)));
assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
const snapshot=core=>{
  const at=core._th12_state(),size=core._th12_state_size();
  return JSON.parse(new TextDecoder().decode(core.HEAPU8.subarray(at,at+size)));
};
async function start(row){
  const core=await createCore();
  const load=(bytes,method,index=null)=>{
    const at=core._malloc(bytes.length);
    try{
      core.HEAPU8.set(bytes,at);
      assert.equal(index===null?method(at,bytes.length):method(index,at,bytes.length),1);
    }finally{core._free(at);}
  };
  load(fs.readFileSync(new URL('../local/retail/stgenm01.anm',import.meta.url)),core._th12_load_anm,2);
  const raw=Buffer.from(row.syntheticEclHex,'hex'),header=raw.readUInt32LE(36);
  // Native creates LayerProbe at(25,100). Startup creates main at(0,0);
  // only rename the synthetic entry and supply the same ECL300 position.
  raw.fill(0,40,header);raw.write('main',40);
  const position=Buffer.alloc(24);
  position.writeUInt16LE(300,4);position.writeUInt16LE(24,6);
  position[10]=63;position[11]=2;
  position.writeFloatLE(25,16);position.writeFloatLE(100,20);
  load(Buffer.concat([raw.subarray(0,header+16),position,raw.subarray(header+16)]),core._th12_load_ecl);
  core._th12_start(0,0,1,0x1234);
  return core;
}

test('actual Enemy constructor and ECL452 seed layer before each script overrides it',async()=>{
  for(const row of native.cases){
    const core=await start(row),state=snapshot(core);
    assert.equal(state.enemies[0].animationLayerOffset,row.enemyLayerOffset,'offset '+row.offset);
    const nodes=state.animations.nodes.filter(n=>n.owner===1&&n.bank===2);
    assert.equal(nodes.length,3);
    for(const expected of row.birthPrimaryChain){
      const node=nodes.find(n=>n.script===expected.script);
      assert.ok(node,'script '+expected.script);
      assert.equal(node.render[5],expected.layer,'initial layer, offset '+row.offset);
      assert.equal(node.render[4],expected.sprite,'initial sprite, offset '+row.offset);
      assert.equal(node.render[2],expected.flags,'initial flags, offset '+row.offset);
      assert.equal(node.membership,1);
      assert.deepEqual(node.pose.slice(3,6),expected.enginePosition.map((v,i)=>v-[224,16,0][i]));
    }
    assert.equal(new Uint32Array(core.HEAPU8.buffer,core._th12_unsupported(),1024)[452],0);
  }
});

test('native41c723 registers enemy animation slots at the Primary head',async()=>{
  for(const row of native.cases){
    const state=snapshot(await start(row)),nodes=new Map(state.animations.nodes.map(n=>[n.id,n]));
    const scripts=state.animations.chains[0].map(id=>nodes.get(id)).filter(n=>n?.owner===1&&n.bank===2).map(n=>n.script);
    assert.deepEqual(scripts,row.birthPrimaryChain.map(n=>n.script));
    assert.equal(row.enemyManagerDraw21.returned,1);
    assert.equal(row.enemyManagerDraw21.submissionCount,0);
    const core=await start(row),count=core._th12_draw(),view=new DataView(core.HEAPU8.buffer),ptr=core._th12_draw_ptr();
    for(let i=0;i<count;i++)assert.notEqual(view.getInt32(ptr+i*core._th12_draw_stride()+24,true),0,
      'Enemy draw21 has no separate sprite fallback; registered ANM slots own the visible enemy');
  }
});

test('retail stage2 yin-yang entry delay cannot display the default fairy or advance logic while drawing',async()=>{
  const core=await initializeCandidate(2);core._th12_start(0,0,1,0x1234);
  let delayed=0,visibleYinYang=0;
  for(let frame=0;frame<1100;frame++){
    core._th12_tick(0,0);
    if(frame<940)continue;
    const before=worldState(core),circles=before.enemies.filter(e=>e.interrupts.some(i=>i[2]==='Cir00Dead'));
    if(!circles.length)continue;
    for(const enemy of circles){
      const owned=before.animations.nodes.filter(n=>n.owner===enemy.id);
      if(!owned.length)delayed++;
      else if(owned.some(n=>n.bank===1&&n.script>=86&&n.script<=97&&n.render[4]>=141&&n.render[4]<=148))visibleYinYang++;
    }
    const count=core._th12_draw(),view=new DataView(core.HEAPU8.buffer),ptr=core._th12_draw_ptr();
    for(let i=0;i<count;i++)assert.notEqual(view.getInt32(ptr+i*core._th12_draw_stride()+24,true),0,
      'no invented default fairy before the original body ANM binds');
    core._th12_anm_draw();
    assert.deepEqual(worldState(core),before,'presentation must not change stage timing or game RNG');
  }
  assert.ok(delayed>0,'the real Cir00 entry contains a period without a body VM');
  assert.ok(visibleYinYang>0,'the real yin-yang body subsequently becomes visible');
});
