import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';import path from 'node:path';import {spawnSync} from 'node:child_process';import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),dir=path.join(root,'artifacts/laser-program-jump');fs.mkdirSync(dir,{recursive:true});
const python=emscripten(root).python,compiler=emscripten(root).compiler,out=path.join(dir,'core.mjs');
const build=spawnSync(python,[compiler,'tests/fixtures/laser-program-jump.cpp','src/game/Laser.cpp','-std=c++20','-O2','-o',out,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS=["_jump_begin","_jump_tick","_jump_missing","_jump_state"]','-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],{cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(build.status,0,build.stdout+'\n'+build.stderr);const create=(await import(pathToFileURL(out))).default;
const native=JSON.parse(fs.readFileSync(new URL('fixtures/laser-program-jump-original.json',import.meta.url)));
test('Curve and Moving program jumps preserve native looping motion, forward skips and Timed ignore behavior',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 for(const r of native.records){const c=await create();c._jump_begin(r.input.kind,r.mode);
  for(const frame of r.frames){if(frame.frame>=0)c._jump_tick();const s=[...new Float32Array(c.HEAPU8.buffer,c._jump_state(),15)];assert.equal(s[0],frame.count);
   if(frame.nodes.length){const n=frame.nodes[0];assert.deepEqual(s.slice(1,5),[n.cursor,n.active,n.protection,n.age]);
    const expected=[...n.position,...n.velocity,...n.geometry.slice(0,4)];expected.forEach((v,i)=>assert.ok(Math.abs(s[i+5]-v)<.0001,`kind${r.input.kind}/mode${r.mode}/frame${frame.frame}/field${i}: ${s[i+5]} vs ${v}`));}
   assert.equal(c._jump_missing(),0);
  }
 }
});
