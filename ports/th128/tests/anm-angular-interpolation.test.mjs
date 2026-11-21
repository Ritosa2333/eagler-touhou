import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'),'utf8'));
const base=path.dirname(path.resolve(root,'local',catalog.variants.ja.manifest));
const metadata=JSON.parse(fs.readFileSync(path.join(base,'animations/title.json'),'utf8'));
const PI=Math.fround(Math.PI),TAU=Math.fround(2*Math.PI);

function f32(value){return Math.fround(value);}
function normalized(value){
  value=f32(value);
  if(!Number.isFinite(value))return value;
  for(let steps=0;value>PI&&steps<34;++steps)value=f32(value-TAU);
  for(let steps=0;value<-PI&&steps<34;++steps)value=f32(value+TAU);
  return value;
}
function ease9(t){
  const twice=f32(2*f32(t));
  return twice<1?f32(0.5*twice*twice):f32(1-0.5*(2-twice)*(2-twice));
}
function advance(track,delta){
  if(delta>=0.99&&delta<=1.01){track.time=f32(track.time+1);++track.elapsed;}
  else{track.time=f32(track.time+delta);track.elapsed=Math.trunc(track.time);}
  if(track.elapsed>=track.duration)return {value:track.to,done:true};
  const t=f32(track.time/track.duration),eased=ease9(t);
  let difference=track.to-track.from;
  if(difference>PI)difference-=TAU;
  else if(difference<-PI)difference+=TAU;
  const step=normalized(f32(difference*eased));
  return {value:normalized(f32(track.from+step)),done:false};
}
function bitsOf(value){
  const buffer=new ArrayBuffer(4);new DataView(buffer).setFloat32(0,value,true);
  return new DataView(buffer).getUint32(0,true);
}
function valueOf(bits){
  const buffer=new ArrayBuffer(4);new DataView(buffer).setUint32(0,bits>>>0,true);
  return new DataView(buffer).getFloat32(0,true);
}
function normalizedBits(value){return bitsOf(normalized(value));}

const scriptIds=[120,121,122,123,131];
const titlePayloads=[];
for(const id of scriptIds){
  const script=metadata.scripts[id];
  const rotation=script.commands.find(command=>command.opcode===49);
  const startBits=rotation
    ?new DataView(Uint8Array.from(Buffer.from(rotation.raw,'hex')).buffer).getUint32(8,true)
    :bitsOf(0);
  const commands=script.commands.filter(command=>command.opcode===113);
  assert.equal(commands.length,4,`title script ${id} opcode113 count`);
  for(const command of commands){
    const bytes=Uint8Array.from(Buffer.from(command.raw,'hex'));
    const view=new DataView(bytes.buffer);
    assert.equal(command.mask,0,`title script ${id} mask`);
    titlePayloads.push({script:id,startBits,targetBits:view.getUint32(8,true),
      duration:view.getInt32(0,true),mode:view.getInt32(4,true)});
  }
}
let allCommands=0;
for(const script of metadata.scripts)
  for(const command of script.commands)if(command.opcode===113)++allCommands;
assert.equal(allCommands,20,'all original opcode113 commands');
assert.equal(titlePayloads.length,20);
for(const payload of titlePayloads){
  assert.equal(payload.duration,10,`title script ${payload.script} duration`);
  assert.equal(payload.mode,9,`title script ${payload.script} mode`);
}
const targets=[...new Set(titlePayloads.map(payload=>payload.targetBits))].sort((a,b)=>a-b);
assert.deepEqual(targets,[bitsOf(-Math.PI/2),bitsOf(0),bitsOf(Math.PI/2),bitsOf(Math.PI)].sort((a,b)=>a-b));

const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
assert.ok(fs.existsSync(compiler),'workspace Emscripten 4.0.22 compiler');
const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
  EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),
  EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
fs.mkdirSync(path.join(root,'artifacts/tests'),{recursive:true});
const output=path.join(root,'artifacts/tests/anm-angular-interpolation.cjs');
const build=spawnSync(python,[compiler,'tests/native/anm-angular-interpolation.cpp','src/animation/AnmLogic.cpp','-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-fast-math','-ffp-contract=off','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
assert.equal(build.status,0,build.stdout+build.stderr);

const cases=[];
function addAngular(name,start,target,duration,mode,delta,advances){
  cases.push({name,line:`A ${duration} ${mode} ${bitsOf(start)} ${bitsOf(target)} ${delta} ${advances}`,
    startBits:bitsOf(start),targetBits:normalizedBits(target),duration,mode,delta,advances,expected:null});
}
for(const payload of titlePayloads)
  addAngular(`title S${payload.script}`,valueOf(payload.startBits),valueOf(payload.targetBits),payload.duration,payload.mode,1,10);
addAngular('shortest 3 to -3',3,-3,10,9,1,10);
addAngular('shortest -3 to 3',-3,3,10,9,1,10);
addAngular('boundary 0 to pi',0,PI,10,9,1,10);
addAngular('boundary 0 to -pi',0,-PI,10,9,1,10);
addAngular('boundary pi to -pi',PI,-PI,10,9,1,10);
addAngular('boundary -pi to pi',-PI,PI,10,9,1,10);
addAngular('boundary 90 to -90',PI/2,-PI/2,10,9,1,10);
addAngular('duration 1',0.25,1,1,9,1,1);
addAngular('near-one clock',0,1,10,9,0.995,1);
addAngular('fractional clock',0,1,10,9,0.5,4);
addAngular('fractional clock 0.25',0,1,10,9,0.25,3);
cases.push({name:'zero duration',line:`Z ${bitsOf(7)} ${bitsOf(Math.PI/2)} 3`,startBits:bitsOf(7),advances:3,zero:true});
for(const mode of [0,1,5,7,8,17])
  cases.push({name:`unsupported mode ${mode}`,line:`A 10 ${mode} ${bitsOf(0)} ${bitsOf(1)} 1 1`,fault:113});
cases.push({name:'negative duration',line:`A -1 9 ${bitsOf(0)} ${bitsOf(1)} 1 1`,fault:113});
cases.push({name:'unknown opcode 112',line:'U 112',fault:112});
assert.equal(cases.length,40);
assert.equal(cases.filter(item=>item.name.startsWith('title ')).length,20);

const run=spawnSync(process.execPath,[output],{cwd:root,input:cases.map(item=>item.line).join('\n')+'\n',encoding:'utf8',timeout:30000});
assert.equal(run.status,0,run.stdout+run.stderr);
const lines=run.stdout.trim().split(/\r?\n/);
assert.equal(lines.length,cases.length);
for(let index=0;index<cases.length;++index){
  const item=cases[index],line=lines[index],parts=line.trim().split(/\s+/);
  if(item.fault!==undefined){
    assert.equal(parts[0],'fault',`${item.name}: ${line}`);
    assert.equal(Number(parts[1]),item.fault,`${item.name}: fault opcode ${line}`);
    if(item.startBits!==undefined)assert.equal(Number(parts[2]),item.startBits,`${item.name}: rollback ${line}`);
    continue;
  }
  assert.equal(parts[0],'ok',`${item.name}: ${line}`);
  assert.equal(parts.length,item.advances+2,`${item.name}: value count`);
  const values=parts.slice(1).map(Number).map(valueOf);
  if(item.zero){
    for(const value of values)assert.equal(bitsOf(value),item.startBits,`${item.name}: unchanged`);
    continue;
  }
  const track={duration:item.duration,elapsed:0,time:0,
    from:normalized(valueOf(item.startBits)),to:normalized(valueOf(item.targetBits))};
  const expected=[advance(track,f32(item.delta))];
  for(let tick=0;tick<item.advances;++tick)expected.push(advance(track,f32(item.delta)));
  assert.equal(expected.length,values.length,`${item.name}: model length`);
  for(let tick=0;tick<values.length;++tick)
    assert.ok(Math.abs(values[tick]-expected[tick].value)<3e-6,
      `${item.name}: tick ${tick} got ${values[tick]} expected ${expected[tick].value}`);
  if(expected.at(-1).done){
    assert.equal(bitsOf(values.at(-1)),item.targetBits,`${item.name}: completion target`);
    assert.equal(bitsOf(values.at(-2)),item.targetBits,`${item.name}: inactive after completion`);
  }
  if(item.name==='shortest 3 to -3'){
    const start=valueOf(item.startBits);
    for(const value of values){
      const delta=normalized(f32(value-start));
      assert.ok(delta>=-1e-6&&delta<=0.2831856+3e-6,`shortest +pi arc ${value}`);
    }
    assert.ok(values.some(value=>value>=3.1415),'shortest +pi crossing');
  }
  if(item.name==='shortest -3 to 3'){
    const start=valueOf(item.startBits);
    for(const value of values){
      const delta=normalized(f32(value-start));
      assert.ok(delta<=1e-6&&delta>=-0.2831856-3e-6,`shortest -pi arc ${value}`);
    }
    assert.ok(values.some(value=>value<=-3.1415),'shortest -pi crossing');
  }
  if(item.name==='boundary pi to -pi'){
    for(const value of values.slice(0,-2))assert.equal(bitsOf(value),bitsOf(PI),'pi boundary hold');
  }
  if(item.name==='boundary 0 to pi'||item.name==='boundary 0 to -pi'){
    const direction=item.name.endsWith('-pi')?-1:1;
    for(let tick=1;tick<values.length;++tick)
      assert.ok(direction*(values[tick]-values[tick-1])>=-1e-6,`${item.name}: monotone`);
  }
  if(item.name==='near-one clock'){
    assert.ok(Math.abs(values[0]-0.02)<1e-6,`near-one first value ${values[0]}`);
    assert.ok(Math.abs(values[1]-0.08)<1e-6,`near-one second value ${values[1]}`);
  }
}
