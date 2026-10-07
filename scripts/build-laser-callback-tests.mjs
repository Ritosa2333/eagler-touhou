import {emscripten} from './toolchain.mjs';
import fs from 'node:fs';import path from 'node:path';import {spawnSync} from 'node:child_process';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const {python,compiler}=emscripten(root),cache=path.join(root,'artifacts/emscripten-cache');
fs.mkdirSync(path.join(root,'artifacts/laser-callback-tests'),{recursive:true});
const names=['callback_input','callback_load','callback_reset','callback_rng','callback_add_laser','callback_add_bullet','callback_run','callback_emissions','callback_emission','callback_shooter','callback_result','callback_unknown','callback_seed','callback_calls','callback_events','callback_event','callback_lasers','target_begin','target_tick','target_integrate','target_state','callback_owner_migration'];
const r=spawnSync(python,[compiler,'tests/fixtures/laserCallbackHarness.cpp','src/game/Laser.cpp','src/game/LaserEnemyCallbacks.cpp','src/game/BulletTargetMotion.cpp','src/game/AnmLogic.cpp','src/game/AnmScene.cpp','-std=c++20','-O2','-o','artifacts/laser-callback-tests/core.mjs','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS='+JSON.stringify([...names.map(x=>'_'+x),'_malloc','_free']),'-sEXPORTED_RUNTIME_METHODS='+JSON.stringify(['HEAPU8','HEAPF32'])],{cwd:root,stdio:'inherit',env:{...process.env,EM_CACHE:cache,EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
if(r.status!==0)process.exit(r.status||1);
