import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('enemy appearance bounds are proved from the complete constant-size ANM subset, not guessed hitboxes',()=>{
  assert.ok(fs.existsSync(path.join(root,'src/game/AppearanceBounds.cpp')),'constant ANM appearance bounds are missing');
  const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk=path.resolve(path.dirname(compiler),'../..');
  const python=path.join(sdk,'python/3.13.3_64bit/python.exe');
  const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
    EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
  fs.mkdirSync(path.join(root,'artifacts/tests'),{recursive:true});
  for(const sanitize of [false,true]){
  const output=path.join(root,`artifacts/tests/appearance-bounds${sanitize?'-ubsan':''}.cjs`);
  const result=spawnSync(python,[compiler,'tests/native/appearance-bounds.cpp','src/game/AppearanceBounds.cpp',
    'src/animation/AnmLogic.cpp','-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-fast-math','-ffp-contract=off',
    ...(sanitize?['-fsanitize=undefined','-fno-sanitize-recover=all']:[]),
    '-sENVIRONMENT=node','-sNODERAWFS=1','-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
  assert.equal(result.status,0,result.stdout+result.stderr);
  const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'),'utf8'));
  const asset=path.join(path.dirname(path.join(root,'local',catalog.variants.ja.manifest)),'raw/enemy.anm');
  const run=spawnSync(process.execPath,[output,asset,path.join(path.dirname(asset),'boss01.anm')],{cwd:root,encoding:'utf8',timeout:30000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/enemy-appearance-fixtures.json')));
  assert.equal(report.executable.sha256,report.executable.sha256After);
  assert.equal(createHash('sha256').update(fs.readFileSync(path.join(root,'tools/research/enemy_appearance_probe.py'))).digest('hex'),report.generatorSha256);
  const bits=value=>{const data=Buffer.alloc(4);data.writeFloatLE(value);return data.readUInt32LE();};
  const input=[],expected=[];
  for(const sample of report.cases){
    input.push([sample.width,sample.height,sample.scaleX,sample.scaleY].map(bits).join(' '));
    const raw=Buffer.from(sample.memory.bounds.after,'hex');expected.push(`${raw.readUInt32LE()} ${raw.readUInt32LE(4)}`);
  }
  input.push([NaN,32,1,1].map(bits).join(' '));expected.push('none');
  input.push([32,32,10000,1].map(bits).join(' '));expected.push('none');
  input.push([32,32,256,1].map(bits).join(' '));expected.push('none');
  const samples=spawnSync(process.execPath,[output],{cwd:root,input:input.join('\n')+'\n',encoding:'utf8',timeout:30000});
  assert.equal(samples.status,0,samples.stdout+samples.stderr);assert.deepEqual(samples.stdout.trim().split(/\r?\n/),expected);
  }
});
