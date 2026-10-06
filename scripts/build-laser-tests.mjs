import {emscripten} from './toolchain.mjs';
import fs from 'node:fs';import path from 'node:path';import {spawnSync} from 'node:child_process';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const {python,compiler}=emscripten(root),cache=path.join(root,'artifacts/emscripten-cache');
fs.mkdirSync(path.join(root,'artifacts/laser-tests'),{recursive:true});
const names=['test_parameters','test_reset','test_spawn','test_tick','test_clear','test_state','test_query','test_find','test_clear_id','test_events','test_event','test_unsupported','test_extension','test_line','test_factory'];
const r=spawnSync(python,[compiler,'tests/fixtures/laserHarness.cpp','src/game/Laser.cpp','src/game/LaserCollision.cpp','-std=c++20','-O2','-o','artifacts/laser-tests/core.mjs','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS='+JSON.stringify(names.map(x=>'_'+x)),'-sEXPORTED_RUNTIME_METHODS='+JSON.stringify(['HEAPU8','HEAPF32'])],{cwd:root,stdio:'inherit',env:{...process.env,EM_CACHE:cache,EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
if(r.status!==0)process.exit(r.status||1);
