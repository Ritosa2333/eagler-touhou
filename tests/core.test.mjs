import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';import path from 'node:path';
import createCore from '../site/runtime/core.mjs';import {parseReplay} from '../src/replay.mjs';
const root=path.resolve(import.meta.dirname,'..');
async function ready(){const core=await createCore();for(const file of ['stage01.ecl','default.ecl']){const b=fs.readFileSync(path.join(root,'site/assets',file)),p=core._malloc(b.length);core.HEAPU8.set(b,p);assert.equal(core._th12_load_ecl(p,b.length),1);core._free(p);}for(let i=0;i<6;i++){const b=fs.readFileSync(path.join(root,'site/assets',`pl0${Math.floor(i/2)}${i%2?'b':'a'}.sht`)),p=core._malloc(b.length);core.HEAPU8.set(b,p);assert.equal(core._th12_load_sht(i,p,b.length),1);core._free(p);}return core;}
const hud=c=>[...new Float32Array(c.HEAPU8.buffer,c._th12_hud(),20)];
test('presentation queries cannot advance logic, RNG or Replay state',async()=>{const c=await ready();c._th12_start(2,1,1,1234);for(let i=0;i<600;i++)c._th12_tick(1,0);const before=hud(c);assert.ok(before[10]>0);for(let i=0;i<240;i++){c._th12_draw();c._th12_hud();}assert.deepEqual(hud(c),before);});
test('first 1200 candidate ticks are independent of presentation count (self-determinism only)',async()=>{
  const a=await ready(),b=await ready(),r=parseReplay(fs.readFileSync(path.join(root,'site/replays/demo2.rpy'))),s=r.stages[0];
  for(const c of [a,b]){c._th12_start(r.character,r.shot,r.difficulty,s.seed);const x=s.initial;c._th12_set_initial(x.x,x.y,x.power,x.lives,x.bombs,x.score);}
  for(let frame=0;frame<1200;frame++){const input=r.input(1,frame);a._th12_tick(input.held,input.pressed);b._th12_tick(input.held,input.pressed);for(let j=0;j<frame%4;j++)b._th12_draw();assert.deepEqual(hud(a),hud(b),'candidate self-determinism tick '+frame);}
  assert.equal(hud(a)[0],1200);assert.ok(hud(a).every(Number.isFinite));
});
