import {emscripten} from '../scripts/toolchain.mjs';
import test from'node:test';import assert from'node:assert/strict';import fs from'node:fs';import path from'node:path';import{spawnSync}from'node:child_process';import{pathToFileURL}from'node:url';import{bossHealthRectangles}from'../src/renderer.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),python=emscripten(root).python,compiler=emscripten(root).compiler,out=path.join(root,'artifacts/boss-hud-test.mjs');
const result=spawnSync(python,[compiler,'tests/fixtures/boss-hud.cpp','src/game/BossHud.cpp','src/game/AnmScene.cpp','src/game/AnmLogic.cpp','-std=c++20','-O2','-o',out,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS=["_malloc","_free","_load_bank","_reset_hud","_marker","_input","_timer_input","_tick_hud","_hud_snapshot"]','-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],{cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python}});assert.equal(result.status,0,result.stderr||result.stdout);
const create=(await import(pathToFileURL(out))).default,native=JSON.parse(fs.readFileSync(new URL('./fixtures/boss-hud-original.json',import.meta.url)));
function snapshot(c){let p=c._hud_snapshot(),end=p;while(c.HEAPU8[end])end++;return JSON.parse(new TextDecoder().decode(c.HEAPU8.subarray(p,end)));}
function rectangles(actual,expected,label){assert.equal(actual.length,expected.length,label);actual.forEach((r,i)=>{assert.equal(r.argb>>>0,expected[i].argb>>>0,label);r.bounds.forEach((v,k)=>assert.ok(Math.abs(v-expected[i].bounds[k])<.00003,`${label} rectangle${i} bound${k}`));});}
test('native Boss HUD fill, drop, name, spell stars and hover branches agree over110 tick frames',async()=>{
  for(const record of native.records.filter(r=>!r.drawOnly)){
    const c=await create();for(const [bank,file]of [[7,'front.anm'],[11,'ascii.anm']]){const b=fs.readFileSync(path.join(root,'site/assets',file)),p=c._malloc(b.length);c.HEAPU8.set(b,p);assert.equal(c._load_bank(bank,p,b.length),1);c._free(p);}
    const before=record.frames[0].before;c._reset_hud(before.displayed,before.target,!!(before.flags&8));before.thresholds.forEach(([fraction,color],i)=>c._marker(i,fraction,color));
    for(const f of record.frames){const i=f.input,label=`${record.name} frame${f.frame}`;
      c._input(i.health,i.maxHealth,i.stage,i.checkpoint,i.remainingSpells,...i.player,i.managerPresent&&i.bossPresent&&!(i.managerFlags&1),i.dialogue);c._tick_hud();const a=snapshot(c);
      assert.ok(Math.abs(a.hud.fill-f.displayed)<1e-7,label);assert.ok(Math.abs(a.hud.target-f.target)<1e-7,label);assert.equal(a.name,f.nameHandle,label);assert.equal(!!a.shifted,!!(f.flags&8),label);assert.deepEqual(a.stars,f.starHandles,label);
      a.hud.markers.forEach((m,k)=>{assert.ok(Math.abs(m[0]-f.thresholds[k][0])<1e-7,label);assert.equal(m[1]>>>0,f.thresholds[k][1]>>>0,label);});
      assert.deepEqual(a.events,f.events.filter(e=>e.kind==='bind'||e.effectiveFixtureHandle).map(e=>[e.kind==='bind'?0:1,e.handle,e.kind==='bind'?e.script:e.code]),label);
      rectangles(bossHealthRectangles(a.hud),f.rectangles,label);
    }
  }
});
test('native solid Boss health rectangles retain threshold ordering and float endpoints',()=>{
  for(const record of native.records.filter(r=>r.drawOnly))for(const f of record.frames)rectangles(bossHealthRectangles({fill:f.displayed,markers:f.thresholds}),f.rectangles,record.name);
});

const times=JSON.parse(fs.readFileSync(new URL('./fixtures/boss-spell-times-original.json',import.meta.url)));
test('Boss timeout display arithmetic and the two original embedded digit VMs agree',async()=>{
  const c=await create(),bytes=fs.readFileSync(path.join(root,'site/assets/ascii.anm')),p=c._malloc(bytes.length);
  c.HEAPU8.set(bytes,p);assert.equal(c._load_bank(11,p,bytes.length),1);c._free(p);
  for(const record of times.countdowns){
    c._reset_hud(0,0,0);c._input(1000,1000,1,0,0,0,400,1,0);c._timer_input(record.age,record.limit,1);c._tick_hud();
    const a=snapshot(c);assert.equal(a.hud.timer.seconds,record.seconds);assert.equal(a.hud.timer.hundredths,record.hundredths);
    assert.equal(a.hud.timer.visible,1);assert.deepEqual(a.digits.map(d=>d.sprite),[243+Math.trunc(record.seconds/10),243+record.seconds%10]);
  }
  for(const track of times.timerTracks){
    c._reset_hud(0,0,0);c._input(1000,1000,1,0,0,0,400,1,0);
    for(const frame of track.frames){
      c._timer_input(0,frame.seconds*60+11,1);c._tick_hud();const a=snapshot(c);
      for(let i=0;i<2;i++){
        const actual=a.digits[i],expected=frame.digits[i];
        assert.equal(actual.sprite,expected.sprite);assert.equal(actual.primary,expected.primary);
        assert.deepEqual(actual.position.map(Math.fround),expected.position);assert.deepEqual(actual.scale.map(Math.fround),expected.scale);
      }
    }
  }
  c._input(1000,1000,1,0,0,0,400,1,1);c._tick_hud();assert.equal(snapshot(c).hud.timer.visible,0);
});
