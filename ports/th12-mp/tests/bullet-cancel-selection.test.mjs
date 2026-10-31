import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';import path from 'node:path';
import {spawnSync} from 'node:child_process';import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),folder=path.join(root,'artifacts/bullet-cancel-selection-tests'),temp=path.join(folder,'tmp');
fs.mkdirSync(temp,{recursive:true});
const python=emscripten(root).python,compiler=emscripten(root).compiler,output=path.join(folder,'core.mjs');
const built=spawnSync(python,[compiler,'tests/fixtures/bullet-cancel-selection.cpp','src/game/BulletTargetMotion.cpp','-std=c++20','-O0','-o',output,
 '-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS=["_cancel_selection"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMCC_TEMP_DIR:temp,EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(built.status,0,built.stdout+'\n'+built.stderr);
const create=(await import(pathToFileURL(output))).default;
test('original class5 birth retains the slot word while both ETEX transforms select12',async()=>{
 const fixture=JSON.parse(fs.readFileSync(new URL('./fixtures/bullet-cancel-selection-original.json',import.meta.url))),core=await create();
 assert.equal(fixture.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 assert.equal(fixture.rows.length,23);
 for(const row of fixture.rows)assert.equal(core._cancel_selection(row.type,row.color,row.previous,row.transform??-1),row.selected,JSON.stringify(row));
});
