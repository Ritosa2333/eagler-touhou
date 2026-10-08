import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {runtimeSources} from '../scripts/runtime-sources.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe');
const compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py');
const directory=path.join(root,'artifacts/enemy-manager-motion-tests');
fs.mkdirSync(directory,{recursive:true});
const output=path.join(directory,'core.mjs');
const probe=spawnSync(python,['scripts/reverse/inspect-enemy-motion.py'],{cwd:root,encoding:'utf8'});
assert.equal(probe.status,0,probe.stdout+probe.stderr);
const compile=spawnSync(python,[compiler,...runtimeSources(root),
  'tests/native/enemy_manager_motion.cpp','-std=c++20','-O2','-ffp-contract=off',
  '-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
  '-sINITIAL_MEMORY=33554432','-sALLOW_MEMORY_GROWTH=1',
  '-sEXPORTED_FUNCTIONS='+JSON.stringify(['_setup','_restore_position','_tick','_combine','_snapshot','_bounds','_set_position','_quantized']),
  '-sEXPORTED_RUNTIME_METHODS='+JSON.stringify(['HEAPF32'])],
  {cwd:root,encoding:'utf8',env:{...process.env,
    EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,
    PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(compile.status,0,compile.stdout+compile.stderr);
const create=(await import(pathToFileURL(output))).default;
const native=JSON.parse(fs.readFileSync(new URL('../artifacts/reverse/enemy-motion-inspection.json',import.meta.url)));
const floor=JSON.parse(fs.readFileSync(new URL('fixtures/enemy-quantization-native.json',import.meta.url)));
assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
assert.equal(floor.originalExeSha256,native.originalExeSha256);
const values=c=>Array.from(c.HEAPF32.subarray(c._snapshot()/4,c._snapshot()/4+30));
function compare(c,expected,label){
  const actual=values(c);
  for(const [j,name] of ['absolute','relative'].entries()){
    const m=expected[name],wanted=[...m.position.slice(0,2),...m.velocityOrCenter.slice(0,2),m.speed,m.angle,m.radius,m.radiusDelta,m.flags];
    for(let i=0;i<9;i++)assert.ok(Math.abs(actual[j*9+i]-wanted[i])<.00002,`${label} ${name}[${i}] ${actual[j*9+i]} != ${wanted[i]}`);
  }
  for(let i=0;i<2;i++)assert.equal(actual[18+i],expected.combined.position[i],label+' combined '+i);
  if('relativeRadiusDuration' in expected){assert.equal(actual[27],expected.absoluteRadiusDuration);assert.equal(actual[28],expected.relativeRadiusDuration);}
  assert.equal(actual[29],1,label+' active');
}
test('production EnemyManager applies retail circle coordinate floor once, across120 updates',async()=>{
  for(const row of native.cases){
    const c=await create(),a=row.args;
    c._setup(a.relative,a.angle,a.initialRadius,a.initialRadiusDelta,a.initialSpeed,a.targetSpeed,a.targetRadius,a.targetDelta,a.duration,a.mode,0);
    compare(c,row.afterInterpolation,row.name+' initialization');
    for(let tick=1;tick<=120;tick++){
      c._tick();const expected=row.frames.find(f=>f.tick===tick);
      if(expected)compare(c,expected,row.name+' frame'+tick);
    }
  }
});
function restore(c,state){
  c._restore_position(...state.position.slice(0,2),...state.absolute.slice(0,2),
    ...state.relative.slice(0,2),state.angle,state.relativeAngle,state.relativeRadius);
}
function exactPosition(c,state,label){
  const data=values(c);
  assert.deepEqual(data.slice(0,2),state.absolute.slice(0,2),label+' absolute');
  assert.deepEqual(data.slice(9,11),state.relative.slice(0,2),label+' relative');
  assert.deepEqual(data.slice(18,20),state.position.slice(0,2),label+' combined');
}
test('retail float quantization boundaries demonstrate that a second floor can move a component',async()=>{
  const c=await create();
  for(const row of floor.quantizationCases){
    for(let index=0;index<2;index++){
      const first=c._quantized(row.before.xy[index]);
      assert.equal(first,row.once.xy[index]);
      assert.equal(c._quantized(first),row.twice.xy[index]);
    }
  }
});
test('original1204 UFO checkpoint advances through1205/1206 without transferring the floor residue to relative',async()=>{
  const row=floor.motionCases.find(c=>!c.extraRelativeQuantization),c=await create();
  const a=row.setupArgs;
  c._setup(a.relative,a.angle,a.initialRadius,a.initialRadiusDelta,a.initialSpeed,
    a.targetSpeed,a.targetRadius,a.targetDelta,a.duration,a.mode,1);
  c._bounds(...row.preparedClamp);
  for(let i=0;i<row.preTicks;i++)c._tick();
  restore(c,row.initial);
  exactPosition(c,row.initial,'restored1204');
  for(const f of row.frames){c._tick();exactPosition(c,f.state,'original'+f.traceFrame);}
  const control=floor.motionCases.find(c=>c.extraRelativeQuantization);
  assert.equal(control.frames[1].state.absolute[0],0);
  assert.notEqual(control.frames[1].state.position[0],row.frames[1].state.position[0]);
});
test('combined clamp applies the absolute inverse even within bounds, including synchronousECL300/302',async()=>{
  for(const row of [...floor.combineCases,...floor.directSetCases]){
    const c=await create();c._setup(1,0,0,0,0,0,0,0,0,0,1);
    c._bounds(...row.preparedClamp);restore(c,row.before);
    if(row.opcode)c._set_position(row.opcode,...row.coordinates);else c._combine();
    exactPosition(c,row.after,(row.opcode?'ECL'+row.opcode:'combine')+' '+row.name);
  }
});
