import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('fire and charge prefixes preserve native call order and partial state in ordinary and UBSan WASM',()=>{
  const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/player-fire-fixtures.json')));
  const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
  assert.equal(report.executable.sha256,report.executable.sha256After);
  assert.equal(report.windowsGameProcessLaunched,false);
  assert.equal(hash(fs.readFileSync(path.join(root,'tools/research/player_fire_probe.py'))),report.generatorSha256);
  for(const [name,digest] of Object.entries(report.engineSha256))assert.equal(hash(fs.readFileSync(path.join(root,'tools/research',name))),digest);
  for(const source of Object.values(report.code))assert.equal(hash(Buffer.from(source.bytes,'hex')),source.sha256);
  const input=[],expected=[];
  for(const sample of report.headCases){
    input.push(['H',sample.state,sample.holding,sample.pressed,sample.previous,sample.current,sample.flags].join(' '));
    const bytes=Buffer.from(sample.memory.shotTimer.after,'hex');
    expected.push([parseInt(sample.stop.address).toString(16),sample.stop.address==='0x0043f25d'?0x43f160:0,0,
      bytes.readInt32LE(0),bytes.readInt32LE(4),bytes.readUInt32LE(8),bytes.readUInt32LE(12),bytes.readUInt32LE(16),0,0,1065353216].join(' '));
  }
  for(const sample of report.chargeCases){
    input.push(['C',sample.held,sample.holding,sample.busy,sample.mode,sample.power,sample.saved].join(' '));
    const bytes=Buffer.from(sample.memory.heldTimer.after,'hex');
    let argument=0;
    if(sample.stop.target==='0x00458410')argument=sample.registers.edx;
    else if(sample.stop.target==='0x00458590')argument=sample.registers.eax;
    expected.push([parseInt(sample.stop.address).toString(16),sample.stop.target?parseInt(sample.stop.target):0,argument,
      bytes.readInt32LE(0),bytes.readInt32LE(4),bytes.readUInt32LE(8),bytes.readUInt32LE(12),bytes.readUInt32LE(16),
      Buffer.from(sample.memory.power.after,'hex').readInt32LE(),Buffer.from(sample.memory.saved.after,'hex').readInt32LE(),
      Buffer.from(sample.memory.scale.after,'hex').readUInt32LE()].join(' '));
  }
  const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
  const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
  fs.mkdirSync(path.join(root,'artifacts/tests'),{recursive:true});
  for(const sanitized of [false,true]){
    const output=path.join(root,`artifacts/tests/player-fire${sanitized?'-ubsan':''}.cjs`);
    const build=spawnSync(python,[compiler,'tests/native/player-fire.cpp','src/game/PlayerFireRules.cpp','src/game/PlayerRules.cpp','src/game/FreezeRules.cpp',
      '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-exceptions','-fno-fast-math','-ffp-contract=off',
      ...(sanitized?['-fsanitize=undefined','-fno-sanitize-recover=all']:[]),'-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
    assert.equal(build.status,0,build.stdout+build.stderr);
    const run=spawnSync(process.execPath,[output],{cwd:root,input:input.join('\n')+'\n',encoding:'utf8',timeout:30000});
    assert.equal(run.status,0,run.stdout+run.stderr);
    assert.deepEqual(run.stdout.trim().split(/\r?\n/),expected);
  }
});
