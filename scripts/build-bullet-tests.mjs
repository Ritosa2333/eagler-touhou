import {emscripten} from './toolchain.mjs';
import fs from 'node:fs';import path from 'node:path';import {spawnSync} from 'node:child_process';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const {python,compiler}=emscripten(root),cache=path.join(root,'artifacts/emscripten-cache');
fs.mkdirSync(path.join(root,'artifacts/bullet-tests'),{recursive:true});
const names=['test_reset','test_set','test_first','test_initialize','test_tick','test_cancel','test_state','test_active','test_collide','test_current_collision','test_emission'];
const r=spawnSync(python,[compiler,'tests/fixtures/bullet.cpp','-std=c++20','-O2','-o','artifacts/bullet-tests/core.mjs','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS='+JSON.stringify(names.map(x=>'_'+x)),'-sEXPORTED_RUNTIME_METHODS='+JSON.stringify(['HEAPU8','HEAPF32'])],{cwd:root,stdio:'inherit',env:{...process.env,EM_CACHE:cache,EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
if(r.status!==0)process.exit(r.status||1);
