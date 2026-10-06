import {emscripten} from './toolchain.mjs';
import fs from 'node:fs';import path from 'node:path';import {spawnSync} from 'node:child_process';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const {python,compiler}=emscripten(root),cache=path.join(root,'artifacts/emscripten-cache');
fs.mkdirSync(path.join(root,'artifacts/spell-tests'),{recursive:true});
const names=['spell_reset','spell_start','spell_survival','spell_prepare','spell_tick','spell_loss','spell_end','spell_snapshot','spell_ring','spell_text','spell_decrypt','spell_title','spell_card'];
names.push('spell_attempts','spell_retry','spell_bindings','spell_digits');
const r=spawnSync(python,[compiler,'tests/fixtures/spell.cpp','-std=c++20','-O2','-o','artifacts/spell-tests/core.mjs','-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS='+JSON.stringify(names.map(x=>'_'+x)),'-sEXPORTED_RUNTIME_METHODS='+JSON.stringify(['HEAPU8'])],{cwd:root,stdio:'inherit',env:{...process.env,EM_CACHE:cache,EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
if(r.status!==0)process.exit(r.status||1);
