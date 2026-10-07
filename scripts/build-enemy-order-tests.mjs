import {emscripten} from './toolchain.mjs';
import fs from 'node:fs';import path from 'node:path';import {spawnSync} from 'node:child_process';import {runtimeSources} from './runtime-sources.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe'),compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py');
fs.mkdirSync(path.join(root,'artifacts/enemy-order-tests'),{recursive:true});
const names=['enemy_order_reset','enemy_order_load','enemy_order_load_bank','enemy_order_origin','enemy_order_boss','enemy_order_spawn','enemy_order_pass','enemy_order_state','enemy_order_state_size','enemy_order_unsupported'];
const r=spawnSync(python,[compiler,'tests/fixtures/enemyOrderHarness.cpp',...runtimeSources(root),'-std=c++20','-O2','-o','artifacts/enemy-order-tests/core.mjs','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS='+JSON.stringify([...names.map(n=>'_'+n),'_malloc','_free']),'-sEXPORTED_RUNTIME_METHODS='+JSON.stringify(['HEAPU8'])],{cwd:root,stdio:'inherit',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
if(r.status!==0)process.exit(r.status||1);
