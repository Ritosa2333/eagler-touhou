import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {emscripten} from '../scripts/toolchain.mjs';
import {readCoreAnimationPoses} from '../src/renderer.mjs';
import {runtimeSources} from '../scripts/runtime-sources.mjs';
const root=path.resolve(import.meta.dirname,'..'),output=path.join(root,'artifacts/reward-feedback-tests');fs.mkdirSync(output,{recursive:true});
const {python,compiler}=emscripten(root),file=path.join(output,'core.mjs');
const build=spawnSync(python,[compiler,'tests/fixtures/reward-feedback.cpp',...runtimeSources(root),'-std=c++20','-O1','-DTH12_DIAGNOSTIC=0','-o',file,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sALLOW_MEMORY_GROWTH=1','-sEXPORTED_FUNCTIONS='+JSON.stringify(['_malloc','_free','_load_feedback_bank','_check_spell_feedback','_check_power_feedback','_check_stock_feedback','_feedback_preview','_th12_anm_draw','_th12_anm_draw_ptr','_th12_anm_draw_stride']),'-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],{cwd:root,encoding:'utf8',windowsHide:true,env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache')}});
assert.equal(build.status,0,build.error?.message||build.stderr||build.stdout);
const create=(await import(pathToFileURL(file))).default;
async function fixture(){const c=await create();for(const [bank,name]of [[0,'bullet'],[7,'front'],[11,'ascii']]){const raw=fs.readFileSync(path.join(process.env.TH12_RETAIL_DIR??path.join(root,'local/retail'),name+'.anm')),p=c._malloc(raw.length);c.HEAPU8.set(raw,p);assert.equal(c._load_feedback_bank(bank,p,raw.length),1);c._free(p);}return c;}
test('actual runtime retains SC result digits and comma257 throughout retail animation',async()=>{const c=await fixture();assert.equal(c._check_spell_feedback(),1);});
test('stage six full-power pickup shows retail front45 only below full power',async()=>{const c=await fixture();assert.equal(c._check_power_feedback(),1);});
test('stage scene reconstruction retains logical UFO slots for all carried physical rotations',async()=>{const c=await fixture();assert.equal(c._check_stock_feedback(),1);});

test('retain actual runtime poses for retail feedback visual inspection',async()=>{const c=await fixture();const poses=[];for(let kind=0;kind<3;kind++){c._feedback_preview(kind);poses.push(readCoreAnimationPoses(c));}fs.writeFileSync(path.join(output,'preview-poses.json'),JSON.stringify(poses));});
