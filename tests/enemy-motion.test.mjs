import test,{before} from 'node:test';
import assert from 'node:assert/strict';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {readFile} from 'node:fs/promises';
import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
let createHarness,native;
before(async()=>{
  const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe');
  const probe=spawnSync(python,['scripts/reverse/inspect-enemy-motion.py'],{cwd:root,encoding:'utf8'});
  assert.equal(probe.status,0,probe.stdout+probe.stderr);
  native=JSON.parse(await readFile(path.join(root,'artifacts/reverse/enemy-motion-inspection.json'),'utf8'));
  assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
  const out=path.join(root,'artifacts/enemy-motion-harness.mjs');
  const compile=spawnSync(python,[path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py'),'tests/native/enemy_motion_harness.cpp','-std=c++20','-O2','-ffp-contract=off','-o',out,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS='+JSON.stringify(['_setup','_tick','_snapshot','_quantized']),'-sEXPORTED_RUNTIME_METHODS='+JSON.stringify(['HEAPF32'])],{cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python}});
  assert.equal(compile.status,0,compile.stdout+compile.stderr);createHarness=(await import(pathToFileURL(out))).default;
});
const data=h=>Array.from(new Float32Array(h.HEAPF32.buffer,h._snapshot(),29));
function compare(h,expected){
  const actual=data(h);
  for(const [j,name] of ['absolute','relative','combined'].entries()){
    const m=expected[name];
    const wanted=[m.position[0],m.position[1],...m.velocityOrCenter.slice(0,2),m.speed,m.angle,m.radius,m.radiusDelta,m.flags];
    for(let i=0;i<9;i++)assert.ok(Math.abs(actual[j*9+i]-wanted[i])<.00002,`${name}[${i}] ${actual[j*9+i]} != ${wanted[i]}`);
  }
  if('absoluteRadiusDuration' in expected){assert.equal(actual[27],expected.absoluteRadiusDuration);assert.equal(actual[28],expected.relativeRadiusDuration);}
}
test('retail hundredth-pixel floor preserves float boundaries and negative positions',async()=>{
  const h=await createHarness();
  for(const row of native.quantization){assert.equal(h._quantized(row.input),row.output[0]);assert.equal(h._quantized(-row.input),row.output[1]);}
});
test('actual 308..311 retail dispatch and 120 movement frames match restored circle parameters',async()=>{
  for(const c of native.cases){
    const h=await createHarness(),a=c.args;
    h._setup(a.relative,a.angle,a.initialRadius,a.initialRadiusDelta,a.initialSpeed,a.targetSpeed,a.targetRadius,a.targetDelta,a.duration,a.mode);
    compare(h,c.afterInterpolation);
    for(let tick=1;tick<=120;tick++){h._tick();const expected=c.frames.find(f=>f.tick===tick);if(expected)compare(h,expected);}
  }
});
