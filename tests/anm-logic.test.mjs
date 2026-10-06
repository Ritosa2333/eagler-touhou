import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';import path from 'node:path';import{spawnSync}from'node:child_process';import{pathToFileURL}from'node:url';import{createHash}from'node:crypto';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),output=path.join(root,'artifacts/anm-test-runtime');
fs.mkdirSync(output,{recursive:true});
const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe'),compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py'),cache=path.join(root,'artifacts/emscripten-cache');
const result=spawnSync(python,[compiler,'tests/fixtures/anm-harness.cpp','-std=c++20','-O2','-o',path.join(output,'anm.mjs'),'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sALLOW_MEMORY_GROWTH=1','-sEXPORTED_FUNCTIONS=["_malloc","_free","_load","_bind","_rebind","_tick","_interrupt","_state","_bullet_sprite"]','-sEXPORTED_RUNTIME_METHODS=["HEAPU8","HEAPF32"]'],{cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:cache,EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(result.status,0,result.stderr||result.stdout);const create=(await import(pathToFileURL(path.join(output,'anm.mjs')).href)).default;
function instruction(op,formats='',values=[],time=0,mask=0){const b=Buffer.alloc(8+values.length*4);b.writeUInt16LE(op&65535);b.writeUInt16LE(b.length,2);b.writeInt16LE(time,4);b.writeUInt16LE(mask,6);values.forEach((v,i)=>formats[i]==='f'?b.writeFloatLE(v,8+i*4):b.writeInt32LE(v,8+i*4));return b;}
function bank(scripts){const h=Buffer.alloc(64+scripts.length*8);h.writeUInt32LE(7);h.writeUInt16LE(scripts.length,6);let offset=h.length;scripts.forEach((s,i)=>{h.writeInt32LE(i,64+i*8);h.writeUInt32LE(offset,68+i*8);offset+=s.length;});return Buffer.concat([h,...scripts]);}
const end=()=>instruction(-1),hold=()=>instruction(63);
async function fixture(bytes){const c=await create(),p=c._malloc(bytes.length);c.HEAPU8.set(bytes,p);assert.equal(c._load(0,p,bytes.length),1);c._free(p);return c;}
function state(c){return Array.from(c.HEAPF32.subarray(c._state()/4,c._state()/4+47));}
function close(a,b,label){assert.ok(Math.abs(a-b)<=Math.max(1e-6,Math.abs(b)*2e-7),`${label}: ${a} != ${b}`);}
test('all hostile bullet sprite roles and colors match 1488 original callback samples',async()=>{
    const c=await create(),original=JSON.parse(fs.readFileSync(path.join(root,'tests/fixtures/bullet-visual-original.json')));
    for(const row of original.records)for(let color=0;color<16;color++)for(let role=0;role<3;role++)assert.equal(c._bullet_sprite(row.type,color,role),row.nativeColors[color][role],`type${row.type} color${color} role${role}`);
    for(const row of original.records.filter(row=>!row.remap))assert.equal(c._bullet_sprite(row.type,15,405),405);
    assert.equal(c._bullet_sprite(0,16,0),-2147483648);
});
test('native binding resets RNG domain and immediately executes the masked angle',async()=>{
    const originals=JSON.parse(fs.readFileSync(path.join(root,'tests/fixtures/anm-logic-original.json')));
    for(const sample of originals.binding){const code=Buffer.concat([...(sample.explicitOpcode87===null?[]:[instruction(87,'i',[sample.explicitOpcode87])]),instruction(49,'fff',[0,0,10010],0,4),hold(),end()]);const c=await fixture(bank([code]));assert.equal(c._bind(0,0,sample.initialDomain),1);const s=state(c);
        close(s[5],sample.rotationZ,'rotation');assert.deepEqual(s.slice(38,42),[...sample.gameSeedCounter,...sample.displaySeedCounter]);assert.equal(s[37],sample.boundFlags2&1);assert.equal(s[42],0);assert.equal(s[29],1);assert.equal(s[28],0);
    }
});
test('native variable readers use distinct float and integer builtin tables',async()=>{
    const original=JSON.parse(fs.readFileSync(path.join(root,'tests/fixtures/anm-logic-original.json')));
    for(const sample of original.variables){const code=Buffer.concat([instruction(7,'ff',[10004,sample.id],0,3),instruction(6,'ii',[10000,sample.id],0,3),hold(),end()]);const c=await fixture(bank([code]));c._bind(0,0,0);const s=state(c);close(s[24],sample.floatValue,'float builtin '+sample.id);close(s[18],sample.intValue,'integer builtin '+sample.id);assert.deepEqual(s.slice(38,42),[...sample.gameSeedCounter,...sample.displaySeedCounter]);assert.equal(s[42],0);}
});
test('native random-sprite selection uses game RNG even after opcode87 selects display RNG',async()=>{
    const original=JSON.parse(fs.readFileSync(path.join(root,'tests/fixtures/anm-logic-original.json')));
    for(const sample of original.randomSprites){const c=await fixture(bank([Buffer.concat([instruction(87,'i',[sample.displayDomain]),instruction(102,'ii',[0,2]),hold(),end()])]));c._bind(0,0,0);const s=state(c);assert.equal(s[30],sample.sprite);assert.deepEqual(s.slice(38,42),[...sample.gameSeedCounter,...sample.displaySeedCounter]);assert.equal(s[37],sample.displayDomain);assert.equal(s[42],0);}
});
test('native arithmetic, motion, interpolation and held timer match original VM samples',async()=>{
    const original=JSON.parse(fs.readFileSync(path.join(root,'tests/fixtures/presentation-original.json')));
    // sample-anm.py constructs a VM with secondary color FFFFFFFF explicitly;
    // native binding reset uses 00000000. Reproduce that fixture initialization.
    for(const sample of original.anm){const code=Buffer.concat([instruction(76,'iii',[255,255,255]),instruction(77,'i',[255]),...sample.instructions.map(i=>instruction(i.op,i.formats,i.values)),hold(),end()]);const c=await fixture(bank([code]));c._bind(0,0,0);
        for(const frame of sample.frames){const s=state(c);const expected=[...frame.position,...frame.rotation,...frame.scale,...frame.uv];expected.forEach((v,i)=>close(s[i],v,sample.name+' field '+i));
            const primary=Number(frame.primary),secondary=Number(frame.secondary);assert.deepEqual(s.slice(10,14),[24,16,8,0].map(shift=>(primary>>>shift)&255));assert.deepEqual(s.slice(14,18),[24,16,8,0].map(shift=>(secondary>>>shift)&255));assert.equal(s[28],frame.clock);assert.equal(s[42],0);c._tick();}
    }
});
test('children bind synchronously using game RNG unless their own script changes domain',async()=>{
    const c=await fixture(bank([Buffer.concat([instruction(88,'i',[1]),hold(),end()]),Buffer.concat([instruction(49,'fff',[0,0,10010],0,4),hold(),end()])]));c._bind(0,0,1);const s=state(c);assert.deepEqual(s.slice(38,42),[7814,2,48879,0]);assert.equal(s[43],1);assert.equal(s[46],1);assert.equal(s[42],0);
});
test('missing interrupt has no effect; matching interrupt runs on the next logical tick',async()=>{
    const c=await fixture(bank([Buffer.concat([instruction(50,'ff',[2,3]),hold(),instruction(64,'i',[7]),instruction(50,'ff',[4,5]),instruction(1),end()])]));c._bind(0,0,0);c._interrupt(42);c._tick();assert.deepEqual(state(c).slice(6,8),[2,3]);c._interrupt(7);assert.deepEqual(state(c).slice(6,8),[2,3]);c._tick();assert.deepEqual(state(c).slice(6,8),[4,5]);assert.equal(state(c)[36],1);
});
test('invalid raw bank is rejected without replacing active registry resources',async()=>{
    const c=await fixture(bank([Buffer.concat([hold(),end()])]));const b=Buffer.from(bank([Buffer.concat([hold(),end()])]));b.writeUInt32LE(0xffffffff,68);const p=c._malloc(b.length);c.HEAPU8.set(b,p);assert.equal(c._load(0,p,b.length),0);c._free(p);assert.equal(c._bind(0,0,0),1);
});
test('actual retail ANM bytecode matches original logical poses, RNG counts and lifetime',async()=>{
    const samples=JSON.parse(fs.readFileSync(path.join(root,'tests/fixtures/anm-retail-original.json')));
    for(const sample of samples.records){const raw=fs.readFileSync(path.join(root,'local/retail',sample.name+'.anm'));assert.equal(createHash('sha256').update(raw).digest('hex'),sample.resourceSha256);const c=await fixture(raw);c._bind(0,sample.script,0);
        for(const frame of sample.frames){if(frame.tick===1&&sample.interrupt)c._interrupt(sample.interrupt);if(frame.tick)c._tick();const s=state(c),prefix=`${sample.name}:${sample.script}:${frame.tick}`;
            [...frame.position,...frame.rotation,...frame.scale,...frame.uv].forEach((v,i)=>close(s[i],v,prefix+' pose '+i));
            [...frame.integers,...frame.floats].forEach((v,i)=>close(s[18+i],v,prefix+' variable '+i));assert.deepEqual(s.slice(10,14),[24,16,8,0].map(shift=>(frame.primary>>>shift)&255),prefix+' primary');assert.deepEqual(s.slice(14,18),[24,16,8,0].map(shift=>(frame.secondary>>>shift)&255),prefix+' secondary');assert.equal(s[28],frame.clock,prefix+' clock');assert.equal(s[30],frame.sprite,prefix+' sprite');assert.equal(s[31],frame.layer,prefix+' layer');assert.equal(s[33],+frame.visible,prefix+' visible');assert.equal(s[35],+frame.ended,prefix+' ended');assert.deepEqual(s.slice(38,42),[...frame.gameSeedCounter,...frame.displaySeedCounter],prefix+' RNG');assert.equal(s[42],0,prefix+' unsupported');
        }
    }
});
test('non-reset binding preserves RNG domain and native local variables',async()=>{
    const c=await fixture(bank([Buffer.concat([instruction(87,'i',[1]),instruction(6,'ii',[10000,12],0,1),hold(),end()]),Buffer.concat([instruction(49,'fff',[0,0,10010],0,4),hold(),end()])]));c._bind(0,0,0);assert.equal(c._rebind(0,1),1);const s=state(c);assert.equal(s[18],12);assert.equal(s[37],1);assert.deepEqual(s.slice(38,42),[4660,0,51904,2]);
});
