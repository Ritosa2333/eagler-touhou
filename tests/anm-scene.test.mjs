import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';import path from 'node:path';import{spawnSync}from'node:child_process';import{pathToFileURL}from'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),output=path.join(root,'artifacts/anm-scene-test-runtime');fs.mkdirSync(output,{recursive:true});
const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe'),compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py');
const result=spawnSync(python,[compiler,'tests/fixtures/anm-scene.cpp','src/game/AnmLogic.cpp','src/game/AnmScene.cpp','-std=c++20','-O2','-o',path.join(output,'core.mjs'),'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS=["_malloc","_free","_load","_load_bank","_reset","_bind","_bind_at","_scene_interrupt","_scene_rotate","_primary","_secondary","_snapshot","_scene_remove"]','-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],{cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});assert.equal(result.status,0,result.stderr||result.stdout);
const create=(await import(pathToFileURL(path.join(output,'core.mjs')).href)).default;
function instruction(op,formats='',values=[],time=0,mask=0){const b=Buffer.alloc(8+values.length*4);b.writeUInt16LE(op&65535);b.writeUInt16LE(b.length,2);b.writeInt16LE(time,4);b.writeUInt16LE(mask,6);values.forEach((v,i)=>formats[i]==='f'?b.writeFloatLE(v,8+i*4):b.writeInt32LE(v,8+i*4));return b;}
function bank(scripts){const h=Buffer.alloc(64+scripts.length*8);h.writeUInt32LE(7);h.writeUInt16LE(scripts.length,6);let offset=h.length;scripts.forEach((s,i)=>{h.writeInt32LE(i,64+i*8);h.writeUInt32LE(offset,68+i*8);offset+=s.length;});return Buffer.concat([h,...scripts]);}
function snapshot(c){const start=c._snapshot();let end=start;while(c.HEAPU8[end])end++;return JSON.parse(new TextDecoder().decode(c.HEAPU8.subarray(start,end)));}
test('primary and secondary saved-successor traversal matches all ten original scheduler samples',async()=>{
  const original=JSON.parse(fs.readFileSync(path.join(root,'tests/fixtures/anm-scheduler-original.json')));
  for(const sample of original.records){const parent=Buffer.concat([instruction(0),instruction(sample.childOpcode,'i',[1],1),instruction(63,'',[],1),instruction(-1,'',[],1)]),child=Buffer.concat([instruction(7,'ff',[10004,10011],0,3),instruction(4,'ii',[0,-1]),instruction(-1)]),dummy=Buffer.concat([instruction(63),instruction(-1)]),bytes=bank([parent,child,dummy]);
    const c=await create(),p=c._malloc(bytes.length);c.HEAPU8.set(bytes,p);assert.equal(c._load(p,bytes.length),1);c._free(p);c._reset();assert.equal(c._bind(1,0),1);if(sample.existingSuccessor)c._bind(2,2);
    for(const frame of sample.frames){c._primary();const actual=snapshot(c);assert.deepEqual(actual.gameSeedCounter,frame.gameSeedCounter);for(const list of ['primary','secondary']){assert.equal(actual[list].length,frame[list].length,`${sample.childOpcode} ${sample.existingSuccessor} frame${frame.frame} ${list}`);actual[list].forEach((s,i)=>{assert.equal(s.script,frame[list][i].script);assert.equal(s.clock,frame[list][i].clock);assert.ok(Math.abs(s.float0-frame[list][i].float0)<1e-7);});}c._secondary();}
  }
});

const native=JSON.parse(fs.readFileSync(path.join(root,'tests/fixtures/anm-native-semantics-original.json')));
async function nativeScene(hex){const c=await create(),bytes=Buffer.from(hex,'hex'),p=c._malloc(bytes.length);c.HEAPU8.set(bytes,p);assert.equal(c._load(p,bytes.length),1);c._free(p);c._reset();return c;}
function compareNative(c,expected){
  const actual=snapshot(c);assert.equal(actual.unsupported,0);assert.deepEqual(actual.gameSeedCounter,expected.gameSeedCounter);assert.deepEqual(actual.displaySeedCounter,expected.displaySeedCounter);assert.equal(actual.primary.length,expected.primary.length);
  for(let i=0;i<expected.primary.length;i++){const a=actual.primary[i],b=expected.primary[i];for(const key of ['script','clock','pendingInterrupt','attached','children'])assert.deepEqual(a[key],b[key],`script${b.script}:${key}`);for(const key of ['scale','position','engine','offset'])a[key].forEach((v,n)=>assert.ok(Math.abs(v-b[key][n])<1e-6,`script${b.script}:${key}[${n}] ${v} != ${b[key][n]}`));assert.ok(Math.abs(a.float0-b.float0)<1e-7);}
}
test('queued and immediate interrupts follow native intrusive child links and birth-time RNG order',async()=>{
  for(const sample of native.interrupts){const c=await nativeScene(sample.scriptBankHex);c._bind(1,0);compareNative(c,sample.before);c._scene_interrupt(sample.targetScript,7,+sample.immediate);compareNative(c,sample.after);c._primary();compareNative(c,sample.afterPrimary);}
});
test('op96/97 writes extra child offset after original child first tick and preserves detached engine',async()=>{
  for(const sample of native.offsets){const c=await nativeScene(sample.scriptBankHex);c._bind_at(1,0,12,34);compareNative(c,sample.snapshot);}
});
test('every negative sprite resolves to original ascii264 while retaining its script bank',async()=>{
  const ascii=fs.readFileSync(path.join(root,'local/retail/ascii.anm'));
  for(const sample of native.whiteSprites){assert.equal(sample.usesAsciiRecord,true);assert.equal(sample.scriptBankRetained,true);const c=await nativeScene(sample.scriptBankHex),p=c._malloc(ascii.length);c.HEAPU8.set(ascii,p);assert.equal(c._load_bank(11,p,ascii.length),1);c._free(p);c._bind(1,0);compareNative(c,sample.snapshot);const [pose]=snapshot(c).primary;assert.equal(pose.sprite,sample.sprite);assert.equal(pose.spriteBank,11);assert.equal(pose.scriptBank,0);}
});
test('native external angles normalize only axes with nonzero angular velocity',async()=>{
  for(const sample of native.externalRotations){const c=await nativeScene(sample.scriptBankHex);c._bind(1,0);c._scene_rotate(0,sample.angleWritten,sample.angleWritten,sample.angleWritten);c._primary();const [pose]=snapshot(c).primary;pose.rotation.forEach((angle,i)=>assert.ok(Math.abs(angle-sample.rotationAfterTick[i])<1e-6));}
});
