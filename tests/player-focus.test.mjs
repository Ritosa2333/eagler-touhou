import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import createCore from '../site/runtime/core.mjs';
import {readCoreAnimationPoses} from '../src/renderer.mjs';

const native=JSON.parse(fs.readFileSync(new URL('./fixtures/player-focus-original.json',import.meta.url))),
  bytes=fs.readFileSync(new URL('../site/assets/bullet.anm',import.meta.url));
function state(c){const ptr=c._th12_state(),size=c._th12_state_size();return JSON.parse(new TextDecoder().decode(c.HEAPU8.subarray(ptr,ptr+size)));}
function near(actual,expected,label){assert.equal(actual.length,expected.length,label);actual.forEach((v,i)=>assert.ok(Math.abs(v-expected[i])<1e-6,`${label}[${i}]: ${v} != ${expected[i]}`));}

test('six loadouts retain all64 native focus-marker birth, tracking, fade and repress frames',async()=>{
  assert.equal(native.gpuExecuted,false);
  assert.ok(native.frames.some(f=>f.nodes.length===4),'retirement overlaps a new focus press');
  for(let loadout=0;loadout<6;loadout++){
    const c=await createCore(),ptr=c._malloc(bytes.length);c.HEAPU8.set(bytes,ptr);
    assert.equal(c._th12_load_anm(0,ptr,bytes.length),1);c._free(ptr);
    c._th12_start(loadout>>1,loadout%2,1,0x1234);
    for(const f of native.frames){
      c._th12_set_initial(...f.playerFixed,0,2,2,0);c._th12_tick(f.held,0);
      const snapshot=state(c),nodes=snapshot.animations.nodes.filter(n=>n.alive&&n.bank===0&&[76,77].includes(n.script)),
        ordered=snapshot.animations.chains[0].map(id=>nodes.find(n=>n.id===id)).filter(Boolean);
      assert.equal(ordered.length,f.nodes.length,`loadout${loadout} frame${f.frame}`);
      ordered.forEach((n,i)=>{const expected=f.nodes[i],label=`loadout${loadout} frame${f.frame} script${n.script}`;
        assert.equal(n.script,expected.script,label);assert.equal(n.clock,expected.clock,label);
        assert.equal(n.execution[3],expected.pendingInterrupt,label);
        near(n.pose.slice(0,3),expected.position,label);near(n.pose.slice(3,6),expected.engine,label);
        near(n.pose.slice(6,9),expected.offset,label);near(n.pose.slice(9,12),expected.rotation,label);
        near(n.pose.slice(12,14),expected.scale,label);
        assert.equal(n.render[0],expected.color,label);assert.equal(n.render[2]&3,expected.flags&3,label);
        assert.equal(n.render[4],380,label);assert.equal(n.render[5],16,label);
      });
      const poses=readCoreAnimationPoses(c).filter(p=>p.owner===0xffff0020);
      assert.equal(poses.length,ordered.length);
      for(const p of poses){assert.equal(p.a.name,'bullet');assert.ok(p.a.visible);assert.equal(p.a.layer,16);assert.equal(p.a.sprite,380);}
      assert.deepEqual([snapshot.rng.seed,snapshot.rng.calls],f.rng);
    }
    c._th12_start(loadout>>1,loadout%2,1,0x1234);
    assert.equal(readCoreAnimationPoses(c).filter(p=>p.owner===0xffff0020).length,0,'restart retires old focus VMs');
  }
});
