import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const compiler=process.env.TH128_EMCC ?? path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk=path.resolve(path.dirname(compiler),'../..');
const bundledPython=path.join(sdk,'python/3.13.3_64bit/python.exe');
const python=process.env.TH128_PYTHON ?? (fs.existsSync(bundledPython)?bundledPython:'python');
const environment={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
  EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,PYTHONUTF8:'1',PYTHONDONTWRITEBYTECODE:'1',EMCC_CORES:'2',BINARYEN_CORES:'2'};

let executable;
function buildFixture(){
  if(executable)return executable;
  assert.ok(fs.existsSync(path.join(root,'src/game/EnemyRules.cpp')),'bounded enemy implementation is missing');
  const output=path.join(root,'local/research/enemy-native.json');
  const build=spawnSync(python,[compiler,'tests/native/enemy-rules.cpp','src/game/EnemyRules.cpp','-Isrc','-std=c++20','-O1',
    '-ffp-contract=off','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-sSINGLE_FILE=1','-o',output],
    {cwd:root,encoding:'utf8',env:environment,maxBuffer:16*1024*1024});
  assert.equal(build.status,0,build.stderr);
  const javascript=fs.readFileSync(output,'utf8');
  fs.writeFileSync(output,JSON.stringify({schema:'th128-enemy-test-build/1',javascript})+'\n');
  executable=output;
  return output;
}

function execute(input=''){
  const run=spawnSync(process.execPath,['-e','(0,eval)(JSON.parse(require("node:fs").readFileSync(process.argv[1],"utf8")).javascript)',buildFixture()],
    {cwd:root,encoding:'utf8',input,maxBuffer:16*1024*1024});
  assert.equal(run.status,0,run.stdout+run.stderr);
  return run.stdout;
}

test('bounded enemy C++ rules satisfy synthetic edge and unsupported cases', {skip:!fs.existsSync(compiler)},()=>{
  const output=execute();
  assert.match(output,/enemy fixed vectors passed/);
});

function bits(value){const bytes=Buffer.alloc(4);bytes.writeFloatLE(value);return bytes.readUInt32LE();}
const fixture=path.join(root,'local/research/enemy-fixtures.json');
test('C++ enemy rules match every bounded private original-byte fixture bit for bit',
  {skip:!fs.existsSync(compiler)||!fs.existsSync(fixture)},()=>{
  const report=JSON.parse(fs.readFileSync(fixture,'utf8'));
  assert.equal(report.schema,'th128-enemy-probe/1');
  assert.equal(report.exeSha256,'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  assert.equal(report.caseCount,report.cases.length);
  const expected=new Map();
  const queries=[];
  for(const record of report.cases){
    const input=record.input;
    const memory=record.memory;
    const word=name=>memory[name].afterU32;
    let argumentsList;
    let result;
    switch(record.operation){
    case 'flags':
      assert.equal(record.stop.reason,'boundary');
      argumentsList=[record.opcode,input.flags,input.mask];
      result=[word('flags'),record.instructionTrace.includes('0x004678b0')?1:record.instructionTrace.includes('0x00467870')?2:0];
      break;
    case 'bounds':
      assert.equal(record.stop.reason,'boundary');
      argumentsList=[bits(input.width),bits(input.height)];result=[1,word('width'),word('height')];break;
    case 'bank':
      assert.equal(record.stop.reason,'boundary');
      argumentsList=[input.bank];result=[memory.bank.afterI32];break;
    case 'animation-request':
      assert.equal(record.stop.reason,'unsupported_call');assert.equal(record.stop.target,'0x0041f2a0');
      argumentsList=[input.bank,input.layerOffset,input.slot,input.script,input.flags];
      result=[1,input.bank,input.slot,record.animationArguments.script,record.animationArguments.layer,input.flags&32?1:0,input.slot===0?1:0];
      break;
    case 'polar':
      assert.equal(record.stop.reason,'boundary');
      argumentsList=[bits(input.oldAngle),bits(input.oldSpeed),input.modeFlags,input.flags,bits(input.angle),bits(input.speed)];
      result=[1,word('angle'),word('speed'),word('polarModeFlags')];break;
    case 'polar-interpolate':
      assert.equal(record.stop.reason,'boundary');
      argumentsList=[bits(input.oldAngle),bits(input.oldSpeed),input.modeFlags,input.flags,input.duration,input.mode,bits(input.angle),bits(input.speed)];
      result=[1,word('angle'),word('speed'),word('polarModeFlags'),word('interpolationStartAngle'),word('interpolationStartSpeed'),
        word('interpolationTargetAngle'),word('interpolationTargetSpeed'),memory.interpolationDuration.afterI32,memory.timerCurrent.afterI32,word('timerFloat')];break;
    case 'linear-step':
      assert.equal(record.stop.reason,'returned');
      argumentsList=[bits(input.startAngle),bits(input.startSpeed),bits(input.targetAngle),bits(input.targetSpeed),input.duration,input.elapsed];
      result=[1,word('outputAngle'),word('outputSpeed'),memory.interpolationDuration.afterI32,memory.timerCurrent.afterI32,word('timerFloat')];break;
    case 'spawn': {
      assert.equal(record.stop.reason,'unsupported_call');assert.equal(record.stop.target,'0x00414ff0');
      const spawn=record.spawnArguments;
      argumentsList=[record.opcode,spawn.name,bits(input.parentX),bits(input.parentY),input.flags,bits(input.x),bits(input.y),
        input.life,input.score,input.drop,...input.inheritedWords,input.contextTag];
      result=[1,...spawn.position.map(bits),spawn.life,spawn.score,spawn.drop,spawn.mode,...spawn.inheritedWords,spawn.contextTag];
      break;
    }
    case 'cartesian':
      assert.equal(record.stop.reason,'returned');
      argumentsList=[...input.position.map(bits),bits(input.angle),bits(input.speed)];
      result=[1,...['X','Y','Z','VelocityX','VelocityY','VelocityZ'].map(suffix=>word('position'+suffix))];break;
    case 'motion-frame':
      assert.equal(record.stop.reason,'boundary');assert.equal(record.stop.address,'0x004163f5');
      argumentsList=[...input.absolute.map(bits),...input.relative.map(bits),...input.combined.map(bits),
        bits(input.angle),bits(input.speed),bits(input.relativeAngle),bits(input.relativeSpeed),input.flags];
      result=[1,...['absolute','relative','combined'].flatMap(prefix=>['X','Y','Z','VelocityX','VelocityY','VelocityZ'].map(suffix=>word(prefix+suffix)))];break;
    default:assert.fail(`fixture operation lacks a C++ differential check: ${record.operation}`);
    }
    queries.push([record.id,record.operation,...argumentsList].join(' '));
    expected.set(record.id,result);
  }
  const output=execute(queries.join('\n')+'\n');
  let compared=0;
  for(const line of output.split(/\r?\n/)){
    const [identifier,...values]=line.trim().split(/\s+/);
    if(!expected.has(identifier))continue;
    assert.deepEqual(values.map(Number),expected.get(identifier),identifier);
    expected.delete(identifier);++compared;
  }
  assert.equal(expected.size,0,`missing original-byte comparisons: ${[...expected.keys()]}`);
  assert.equal(compared,report.caseCount);
  assert.match(output,new RegExp(`enemy evidence queries passed: ${report.caseCount}`));
});
