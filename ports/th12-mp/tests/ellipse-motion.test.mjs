import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {runtimeSources} from '../scripts/runtime-sources.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const dir=path.join(root,'artifacts/ellipse-motion-tests');fs.mkdirSync(dir,{recursive:true});
const python=emscripten(root).python,compiler=emscripten(root).compiler;
const output=path.join(dir,'core.mjs');
const built=spawnSync(python,[compiler,'tests/fixtures/ellipse-motion.cpp',...runtimeSources(root),
 '-std=c++20','-O0','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=33554432',
 '-sEXPORTED_FUNCTIONS=["_ellipse_setup","_ellipse_command","_ellipse_tick","_ellipse_state"]',
 '-sEXPORTED_RUNTIME_METHODS=["HEAPF32"]'],{cwd:root,encoding:'utf8',env:{...process.env,
 EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,
 PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(built.status,0,built.stdout+'\n'+built.stderr);
const create=(await import(pathToFileURL(output))).default;
const native=JSON.parse(fs.readFileSync(new URL('fixtures/ellipse-motion-native.json',import.meta.url)));
function compare(c,expected,label){
 const actual=[...c.HEAPF32.subarray(c._ellipse_state()/4,c._ellipse_state()/4+30)];
 for(const [side,name] of ['absolute','relative'].entries()){
  const m=expected[name],want=[...m.position.slice(0,2),...m.centerOrVelocity.slice(0,2),m.speed,m.angle,m.radius,m.radiusDelta,m.ellipseAngle,m.ellipseRatio,m.flags];
  // Linear velocity belongs to the polar component while circle/ellipse
  // centers retain their saved fields. Geometry is compared independently.
  for(let k=0;k<want.length;k++){
   if((k===2||k===3)&&!(m.flags&1))continue;
   assert.ok(Math.abs(actual[side*11+k]-want[k])<.00002,`${label} ${name}[${k}] ${actual[side*11+k]} != ${want[k]}`);
  }
 }
 assert.deepEqual(actual.slice(22,24),expected.combined.position.slice(0,2),label+' combined coordinates');
 assert.deepEqual(actual.slice(24),expected.durations,label+' independent track durations');
}
test('original320..323 ellipse geometry, interpolation and flag transitions match all21 native slices',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 const c=await create();
 for(const row of native.rows){
  c._ellipse_setup();
  for(let i=0;i<row.commands.length;i++){
   const command=row.commands[i],a=command.args.slice(),integer=command.op===325||command.op===326?1:[301,303,305,307,309,311,321,323].includes(command.op)?2:0;
   const duration=integer?a.shift():0,mode=integer===2?a.shift():0;
   c._ellipse_command(command.op,duration,mode,...Array.from({length:6},(_,k)=>a[k]??0));
   compare(c,row.afterCommands[i],row.name+' command'+i);
  }
  for(const frame of row.frames){c._ellipse_tick();compare(c,frame,row.name+' tick'+frame.tick);}
 }
});
