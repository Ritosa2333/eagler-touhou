import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe');
const compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py');
const directory=path.join(root,'artifacts/ufo-effects-tests');fs.mkdirSync(directory,{recursive:true});
const output=path.join(directory,'core.mjs');
const names=['load','begin','defeat_begin','tick','rng_seed','rng_calls','count','effect_values','effect_words','draw','mesh_count','mesh_primitive','mesh_bank','mesh_priority','malloc','free'];
const build=spawnSync(python,[compiler,'tests/fixtures/ufo-effects.cpp','src/game/UfoEffects.cpp',
  'src/game/AnmLogic.cpp','src/game/AnmScene.cpp','-std=c++20','-O2','-o',output,
  '-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
  '-sEXPORTED_FUNCTIONS='+JSON.stringify(names.map(n=>'_'+n)),
  '-sEXPORTED_RUNTIME_METHODS='+JSON.stringify(['HEAPU8','HEAPU32','HEAPF32'])],
  {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(build.status,0,build.stderr||build.stdout);
const create=(await import(pathToFileURL(output))).default;
const native=JSON.parse(fs.readFileSync(new URL('fixtures/ufo-summon-effect-native.json',import.meta.url)));
assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
async function ready(scenario){
  const c=await create(),raw=fs.readFileSync(new URL('../local/retail/bullet.anm',import.meta.url)),pointer=c._malloc(raw.length);
  c.HEAPU8.set(raw,pointer);assert.equal(c._load(0,pointer,raw.length),1);c._free(pointer);
  if(scenario.scenario==='defeat-ten-trails'){
    const ascii=fs.readFileSync(new URL('../local/retail/ascii.anm',import.meta.url)),p=c._malloc(ascii.length);
    c.HEAPU8.set(ascii,p);assert.equal(c._load(11,p,ascii.length),1);c._free(p);
  }
  if(scenario.scenario==='defeat-ten-trails')c._defeat_begin(scenario.seed,...scenario.position);
  else c._begin(scenario.seed,...scenario.position,+(scenario.scenario==='following-held-VM'));
  return c;
}
const close=(a,b,label)=>assert.ok(Math.abs(a-b)<=.00006,`${label}: ${a} != ${b}`);
function nodes(c){
  const out=[];
  for(let i=0;i<c._count();i++){
    const fp=c._effect_values(i)/4,wp=c._effect_words(i)/4;
    out.push({index:i,f:[...c.HEAPF32.subarray(fp,fp+45)],w:[...c.HEAPU32.subarray(wp,wp+22)]});
  }
  return out;
}
function compare(c,frame,label){
  const expected=frame.nodes.filter(n=>n.type),actual=nodes(c);
  assert.deepEqual([c._rng_seed(),c._rng_calls()],[frame.rng.seed,frame.rng.calls],label+' RNG');
  assert.equal(actual.length,expected.length,label+' effect count');
  for(let i=0;i<expected.length;i++){
    const a=actual[i],n=expected[i];assert.equal(a.w[0],n.type,label+' type');assert.equal(a.w[1],n.id,label+' chain ID');
    if(!n.effect)continue; // Compact fixture retains full state at selected frames.
    assert.deepEqual(a.f.slice(3,6),[n.effect.previous,n.effect.timer,n.effect.timerValue],label+' timer');
    a.f.slice(40,43).forEach((v,k)=>close(v,n.enginePosition[k],label+' engine'));
    assert.equal(a.f[43],n.clock,label+' ANM clock');assert.equal(a.f[44],n.nativeAnmTickCalls,label+' ANM calls');
    assert.equal(a.w[20],13);assert.equal(a.w[21],n.flags,label+' VM flags');
    if(n.type===2){a.f.slice(0,3).forEach((v,k)=>close(v,n.effect.position[k],label+' origin'));assert.equal(a.f[6],n.effect.radius);assert.deepEqual(a.w.slice(2,4),n.effect.colors);}
    else {close(a.f[7],n.effect.heading,label+' heading');a.f.slice(8,40).forEach((v,k)=>close(v,n.effect.pointsXY[k],label+' points '+k));assert.deepEqual(a.w.slice(4,20),n.effect.colors,label+' colors');}
  }
}
function compareDraw(c,row,label){
  const at=nodes(c).find(n=>n.w[1]===row.id);assert.ok(at,label+' draw node');
  const pointer=c._draw(at.index),count=c._mesh_count(),view=new DataView(c.HEAPU8.buffer),actual=[];
  for(let i=0;i<count;i++){const p=pointer+i*28;actual.push([view.getFloat32(p,true),view.getFloat32(p+4,true),view.getFloat32(p+8,true),view.getFloat32(p+12,true),view.getUint32(p+16,true)]);}
  const expected=row.primitive==='triangle-fan'?Array.from({length:32},(_,i)=>[row.vertices[0],row.vertices[i+1],row.vertices[i+2]]).flat():row.vertices;
  assert.equal(c._mesh_primitive(),row.primitive==='triangle-fan'?4:3);assert.equal(c._mesh_bank(),-1);assert.equal(c._mesh_priority(),26);
  assert.equal(actual.length,expected.length,label+' vertex count');
  for(let i=0;i<expected.length;i++){for(let axis=0;axis<2;axis++)close(actual[i][axis],expected[i][axis],label+' vertex '+i);assert.deepEqual(actual[i].slice(2),expected[i].slice(2),label+' z/rhw/color');}
}
test('native summon birth and121 primary passes preserve saved-next RNG, motion/color and removal',async()=>{
  for(const scenario of native.scenarios){const c=await ready(scenario);compare(c,scenario.effectBirth,scenario.scenario+' birth');
    for(const frame of scenario.frames){const before=c._rng_calls();c._tick();compare(c,{...frame,rng:frame.rngAfter},scenario.scenario+' tick'+frame.tick);assert.equal(c._rng_calls()-before,frame.rngCallDelta);}
    assert.equal(c._rng_calls(),6200);assert.equal(c._count(),0);
  }
});
test('304 original circle-fan and line-strip submissions match mesh vertices without consuming RNG',async()=>{
  let compared=0;
  for(const scenario of native.scenarios){const c=await ready(scenario),draws=scenario.draws??[];
    for(let tick=-1;tick<=120;tick++){if(tick>=0)c._tick();const before=[c._rng_seed(),c._rng_calls()];for(const row of draws.filter(d=>d.tick===tick)){compareDraw(c,row,scenario.scenario+' tick'+tick+' id'+row.id);compared++;}assert.deepEqual([c._rng_seed(),c._rng_calls()],before);}
  }
  assert.equal(compared,304);
});
test('first tick differs between an isolated tail and an actual following-node chain',async()=>{
  const calls=[];for(const s of native.scenarios){const c=await ready(s);assert.equal(c._rng_calls(),0);c._tick();calls.push(c._rng_calls());c._tick();assert.equal(c._rng_calls()-calls.at(-1),20);}
  assert.deepEqual(calls,[4,12]);
});
test('original UFO defeat binds ten actual trails after bullet208 and retains their RNG, motion and lifetime',async()=>{
 const original=JSON.parse(fs.readFileSync(new URL('fixtures/ufo-defeat-effect-native.json',import.meta.url)));
 assert.equal(original.originalExeSha256,native.originalExeSha256);
 for(const scenario of original.scenarios){
  const c=await ready(scenario);compare(c,scenario.effectBirth,scenario.scenario+' birth');
  const checkDraws=tick=>{
   const before=[c._rng_seed(),c._rng_calls()];
   for(const row of scenario.draws.filter(d=>d.tick===tick))compareDraw(c,row,scenario.scenario+' tick'+tick+' id'+row.id);
   assert.deepEqual([c._rng_seed(),c._rng_calls()],before,'defeat draw does not consume RNG');
  };
  checkDraws(-1);
  for(const frame of scenario.frames){c._tick();compare(c,{...frame,rng:frame.rngAfter},scenario.scenario+' tick'+frame.tick);checkDraws(frame.tick);}
  assert.equal(c._count(),0);assert.equal(scenario.effectBirth.nodes.filter(n=>n.type===1).length,10);
 }
});
