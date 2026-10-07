import test from'node:test';import assert from'node:assert/strict';import fs from'node:fs';import path from'node:path';import{spawnSync}from'node:child_process';import{pathToFileURL}from'node:url';import{bossHealthRectangles}from'../src/renderer.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe'),compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py'),out=path.join(root,'artifacts/boss-hud-test.mjs');
const result=spawnSync(python,[compiler,'tests/fixtures/boss-hud.cpp','src/game/BossHud.cpp','src/game/AnmScene.cpp','src/game/AnmLogic.cpp','-std=c++20','-O2','-o',out,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS=["_malloc","_free","_load_bank","_reset_hud","_marker","_input","_tick_hud","_hud_snapshot"]','-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],{cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python}});assert.equal(result.status,0,result.stderr||result.stdout);
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
