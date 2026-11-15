import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const compiler=process.env.TH128_EMCC??path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk=path.resolve(path.dirname(compiler),'../..');
const python=process.env.TH128_PYTHON??path.join(sdk,'python/3.13.3_64bit/python.exe');
const environment={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
  EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),
  EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};

test('native title pages 0/1/2 bind original roots and use real IRQ selection',()=>{
  const exports=fs.readFileSync(path.join(root,'src/runtime/Exports.cpp'),'utf8');
  assert.match(exports,/th128_title_menu_page\(int page, int choice\)/);
  assert.match(exports,/th128_title_menu_select\(int choice\)/);
  assert.match(exports,/th128_title_menu_state\(\)/);
  assert.match(exports,/bindRoot\(newOwner, 24, page == 1 \? 129 : 130\)/,'page roots 129/130');
  assert.match(exports,/scene\.interruptNode\(root->id, 7 \+ choice, false\)/,'root selection IRQ');
  assert.match(exports,/page == 1 \? selectDifficulty\(choice\) : selectRoute\(choice\)/,'page-entry root IRQ');
  const rootIrq=exports.indexOf('scene.interruptNode(root->id, 7 + choice, false)');
  const hideDifficulty=exports.indexOf('titleInterrupt(10, 116 + index, 3, false)');
  const hideRoute=exports.indexOf('titleInterrupt(11, 124 + index, 3, false)');
  assert.ok(rootIrq>=0 && hideDifficulty>rootIrq && hideRoute>rootIrq,'root IRQ must precede label3 hides');
  assert.ok(!exports.includes('titleInterrupt(10, 116 + choice'), 'selected difficulty card must not be double-sent');
  assert.ok(!exports.includes('titleInterrupt(11, 124 + choice'), 'selected route card must not be double-sent');
  assert.match(exports,/titleInterrupt\(10, 116 \+ index, 3, false\)/,'difficulty hide IRQ');
  assert.match(exports,/titleInterrupt\(11, 124 \+ index, 3, false\)/,'route hide IRQ');
  assert.match(exports,/removeOwner\(newOwner\)/,'new-owner-only rollback');
  assert.match(exports,/titleNode\(10, 129\)/,'difficulty ready root');
  assert.match(exports,/titleNode\(11, 130\)/,'route ready root');
  assert.match(exports,/item < selection \? 30 : 31/,'main row navigation IRQ');
  assert.match(exports,/selection \+ 17/,'main selected IRQ');
  assert.match(exports,/state\.spriteBank < 0 && state\.geometryCount == 0/,'transform-only capture exclusion');
  assert.match(exports,/th128_draw_count\(\)/,'draw capture exercised');
  const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json')));
  const raw=path.join(path.dirname(path.join(root,'local',catalog.variants.ja.manifest)),'raw');
  const title=path.join(raw,'title.anm');
  const version=path.join(raw,'title_v.anm');
  assert.equal(createHash('sha256').update(fs.readFileSync(title)).digest('hex'),
    '96e6a62d282b4f181db0548e3364d200d4633c3e8416ca3a0a9e02862de41ffd');
  fs.mkdirSync(path.join(root,'artifacts/tests'),{recursive:true});
  const output=path.join(root,'artifacts/tests/native-title-menu.cjs');
  const build=spawnSync(python,[compiler,'tests/native/native-title-menu.cpp',
    'src/animation/AnmLogic.cpp','src/animation/AnmScene.cpp','src/game/FreezeRules.cpp','-Isrc','-std=c++20','-O1',
    '-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math','-ffp-contract=off',
    '-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sEXIT_RUNTIME=1',
    '-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env:environment,encoding:'utf8',timeout:240000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const run=spawnSync(process.execPath,[output,title,version],{cwd:root,encoding:'utf8',timeout:120000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/PASS native title pages 0\/1\/2 roots, IRQ selection, transition and return/);
});
