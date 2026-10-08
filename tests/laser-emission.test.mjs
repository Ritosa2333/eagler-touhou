import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';
import path from 'node:path';import {spawnSync} from 'node:child_process';import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),dir=path.join(root,'artifacts/laser-emission-tests');fs.mkdirSync(dir,{recursive:true});
const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe'),compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py'),output=path.join(dir,'core.mjs');
const built=spawnSync(python,[compiler,'tests/fixtures/laser-emission.cpp','src/game/Laser.cpp','-std=c++20','-O2','-o',output,
 '-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS=["_malloc","_free","_emission_begin","_emission_tick","_emission_snapshot"]','-sEXPORTED_RUNTIME_METHODS=["HEAPU8","UTF8ToString"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(built.status,0,built.stdout+'\n'+built.stderr);
const create=(await import(pathToFileURL(output))).default,native=JSON.parse(fs.readFileSync(new URL('./fixtures/laser-emission-original.json',import.meta.url)));
test('actual moving/curve paired80000 emitter payload, Timed no-op and optional parent clear',async()=>{
  const core=await create();assert.equal(native.records.length,9);
  for(const row of native.records){
    const floats=new Float32Array([row.program[0][0],row.program[1][0]]),words=new Int32Array([row.program[0][2],row.program[1][2]]);
    const pointers=[floats,words].map(v=>{const p=core._malloc(v.byteLength);core.HEAPU8.set(new Uint8Array(v.buffer),p);return p;});
    core._emission_begin(row.input.kind,...pointers);pointers.forEach(p=>core._free(p));
    for(let frame=0;frame<2;frame++){
      core._emission_tick();const actual=JSON.parse(core.UTF8ToString(core._emission_snapshot()));
      const events=row.events.filter(e=>e.kind==='emission'&&e.frame<=frame);
      assert.equal(actual.missing,0,row.name);assert.equal(actual.emissions.length,events.length,row.name);
      for(let i=0;i<events.length;i++){
        const b=Buffer.from(events[i].emitterHex,'hex');
        const expected=[b.readUInt16LE(0),b.readUInt16LE(2),b.readFloatLE(4),b.readFloatLE(8),b.readFloatLE(16),b.readFloatLE(20),
          b.readFloatLE(24),b.readFloatLE(28),b.readInt16LE(0x1f8),b.readInt16LE(0x1fa),b.readInt16LE(0x1fc),b.readInt32LE(0x20c),b.readUInt32LE(0x200),b.readInt32LE(0x208)];
        assert.equal(actual.emissions[i].length,expected.length);
        for(let field=0;field<expected.length;field++)assert.ok(Math.abs(actual.emissions[i][field]-expected[field])<.00004,`${row.name} emission${i} field${field}: ${actual.emissions[i][field]} != ${expected[field]}`);
      }
      const nodes=row.frames[frame+1].nodes;assert.equal(actual.nodes.length,nodes.length,row.name+' lifetime');
      for(let i=0;i<nodes.length;i++){
        const expected=[...nodes[i].position.slice(0,2),nodes[i].phase,nodes[i].cursor];
        for(let j=0;j<4;j++)assert.ok(Math.abs(actual.nodes[i][j]-expected[j])<.00004,`${row.name} parent field${j}: ${actual.nodes[i][j]} != ${expected[j]}`);
      }
    }
  }
});
