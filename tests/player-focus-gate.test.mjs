import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {runtimeSources} from '../scripts/runtime-sources.mjs';
import {loadLiveMain} from './helpers/live-main.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const directory=path.join(root,'artifacts/player-focus-gate');fs.mkdirSync(directory,{recursive:true});
const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe');
const compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py');
const output=path.join(directory,'core.mjs');
const names=['focus_prepare','focus_update','focus_player_phase','focus_dialogue','th12_load_ecl','th12_load_anm','th12_load_sht','th12_state','th12_state_size','th12_tick','th12_start','malloc','free'];
const build=spawnSync(python,[compiler,'tests/fixtures/player-focus-gate.cpp',...runtimeSources(root),
 '-std=c++20','-O0','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=33554432','-sEXPORTED_FUNCTIONS='+JSON.stringify(names.map(n=>'_'+n)),
 '-sEXPORTED_RUNTIME_METHODS='+JSON.stringify(['HEAPU8'])],
 {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(build.status,0,build.stdout+'\n'+build.stderr);
const create=(await import(pathToFileURL(output))).default;
const native=JSON.parse(fs.readFileSync(new URL('fixtures/player-focus-gate-original.json',import.meta.url)));
assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
const state=c=>{const p=c._th12_state(),n=c._th12_state_size();return JSON.parse(new TextDecoder().decode(c.HEAPU8.subarray(p,p+n)));};
function load(c,name,method,index){const bytes=fs.readFileSync(new URL('../site/assets/'+name,import.meta.url)),p=c._malloc(bytes.length);c.HEAPU8.set(bytes,p);assert.equal(index===undefined?c[method](p,bytes.length):c[method](index,p,bytes.length),1);c._free(p);}
async function ready(){const c=await create();loadLiveMain(c);load(c,'bullet.anm','_th12_load_anm',0);return c;}
test('actual PlayerSystem matches twelve original movement/focus gates and restores the pre-movement field',async()=>{
 const c=await ready();
 for(const row of native.rows){const i=row.initial;c._focus_prepare(i.totalAge,i.restoredFocus,i.liveEnemyCount);assert.equal(state(c).player.focused,i.restoredFocus);c._focus_update(i.held);const s=state(c);
  assert.equal(s.player.focused,row.after.focused);assert.deepEqual([s.player.xFixed,s.player.yFixed],row.after.positionFixed);
  assert.equal(!!s.player.focusAnimation,row.after.markerLinked);assert.deepEqual([s.rng.seed,s.rng.calls],[0x1234,0]);
 }
});
test('focus uses the total player age through respawn and remains available during dialogue',async()=>{
 const c=await ready();c._focus_prepare(10,0,1);c._focus_dialogue(1);c._focus_update(0x88);
 assert.equal(state(c).player.focused,1);assert.ok(state(c).player.focusAnimation);
 c._focus_prepare(10,1,1);c._focus_player_phase(3,0);c._focus_update(0);assert.equal(state(c).player.focused,1,'state3 does not run movement or rewrite C598');
 c._focus_player_phase(0,60);c._focus_update(8);assert.equal(state(c).player.state,1);assert.equal(state(c).player.focused,1,'state timer reset does not restart the total-age gate');
 c._focus_prepare(10,0,1);c._focus_update(8);c._focus_player_phase(4,8);c._focus_update(0);assert.equal(state(c).player.state,2);assert.equal(state(c).player.focused,1,'death body rebind does not restore the initial replay focus');
});
test('all six retail firing/option consumers use the gated field rather than raw held Shift',async()=>{
 const c=await ready(),normal=await ready();for(const core of [c,normal])for(let i=0;i<6;i++)load(core,`pl0${i>>1}${i%2?'b':'a'}.sht`,'_th12_load_sht',i);
 for(let i=0;i<6;i++){
  c._th12_start(i>>1,i%2,1,0x1234);normal._th12_start(i>>1,i%2,1,0x1234);
  for(let tick=0;tick<5;tick++){
   c._th12_tick(9,0);normal._th12_tick(1,0);const a=state(c),b=state(normal);assert.equal(a.player.focused,tick>=4?1:0,`loadout${i} input${tick}`);
   if(tick<4){assert.deepEqual(a.player.options,b.player.options,'gated option formation');assert.deepEqual(a.bullets,b.bullets,'gated retail firing group/projectile callbacks');}
  }
 }
});
