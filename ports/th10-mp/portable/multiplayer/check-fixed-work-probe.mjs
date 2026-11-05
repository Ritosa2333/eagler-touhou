// ROOT runs: node --test portable/multiplayer/check-fixed-work-probe.mjs
// Positive cases instantiate the actual assembled WASM in Node; no browser,
// game module or emulated kernel is used as a positive result. The two explicit
// fault-injection cases only verify failure reporting, never kernel equivalence.
import test,{after} from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {fixedWorkProbePolicy,fixedWorkJsKernel,fixedWorkWasmBytes,runFixedWorkProbe} from './fixed-work-probe.mjs';

const sources=['fixed-work-probe.mjs','check-fixed-work-probe.mjs'];
const inventory=()=>Object.fromEntries(sources.map(path=>[path,
 createHash('sha256').update(readFileSync(new URL(path,import.meta.url))).digest('hex')]));
const before=inventory();
after(()=>assert.deepEqual(inventory(),before,'Probe/check source changed during validation'));
const boundary='post-state-audio-fence/pre-bulk';
const poison=name=>()=>{throw Error('Unexpected access: '+name);};

function withGlobals(descriptors,action){
 const previous=Object.fromEntries(Object.keys(descriptors).map(name=>[name,Object.getOwnPropertyDescriptor(globalThis,name)]));
 try{for(const [name,descriptor] of Object.entries(descriptors))
  Object.defineProperty(globalThis,name,{configurable:true,...descriptor});
  return action();
 }finally{for(const [name,descriptor] of Object.entries(previous)){
  if(descriptor)Object.defineProperty(globalThis,name,descriptor);else delete globalThis[name];
 }}
}

// Independent mathematical reference: positive BigInt values, explicit modulo
// 2^32 after each update. It does not use JS bitwise/imul or the WASM assembler.
function reference(iterations,seed){
 const mask=(1n<<32n)-1n;let x=BigInt(seed>>>0);
 for(let i=0n;i<BigInt(iterations);++i){
  x=(x^(x<<13n))&mask;x=x^(x>>17n);x=(x^(x<<5n))&mask;
  x=(x+i*0x9e3779b9n)&mask;
 }
 return Number(x);
}

// Independently read only the standard section envelopes. This catches even an
// unexported private memory/import/start addition, which export inspection alone
// cannot establish is absent. It also rejects trailing or truncated payloads.
function sections(bytes){
 assert.deepEqual(Array.from(bytes.subarray(0,8)),[0,97,115,109,1,0,0,0]);
 const result=[];let at=8;
 while(at<bytes.length){
  const id=bytes[at++];let length=0,shift=0,b;
  do{assert.ok(at<bytes.length&&shift<35,'valid bounded ULEB section length');
   b=bytes[at++];length+=(b&127)*2**shift;shift+=7;
  }while(b&128);
  const end=at+length;assert.ok(end<=bytes.length,'whole section payload');
  result.push({id,payload:bytes.slice(at,end)});at=end;
 }
 assert.equal(at,bytes.length);return result;
}

function checkTiming(value,label){
 assert.equal(value.completed,true,label+' completed');
 for(const field of ['startedAtPerformanceMs','finishedAtPerformanceMs','wallMs'])
  assert.ok(Number.isFinite(value[field]),label+' finite '+field);
 assert.ok(value.finishedAtPerformanceMs>=value.startedAtPerformanceMs,label+' monotonic');
 assert.equal(value.wallMs,value.finishedAtPerformanceMs-value.startedAtPerformanceMs,label+' raw duration');
}

test('readable assembler produces one actual function and no imports, memories or start section',t=>{
 const bytes=fixedWorkWasmBytes(),list=sections(bytes);
 assert.deepEqual(list.map(s=>s.id),[1,3,7,10]);
 assert.deepEqual(Array.from(list[0].payload),[1,0x60,2,0x7f,0x7f,1,0x7f]);
 assert.deepEqual(Array.from(list[1].payload),[1,0]);
 assert.deepEqual(Array.from(list[2].payload),[1,6,107,101,114,110,101,108,0,0]);
 assert.equal(WebAssembly.validate(bytes),true);
 const module=new WebAssembly.Module(bytes),instance=new WebAssembly.Instance(module);
 assert.deepEqual(WebAssembly.Module.imports(module),[]);
 assert.deepEqual(WebAssembly.Module.exports(module),[{name:'kernel',kind:'function'}]);
 assert.deepEqual(Object.keys(instance.exports),['kernel']);
 assert.equal(typeof instance.exports.kernel,'function');
 assert.ok(!Object.values(instance.exports).some(value=>value instanceof WebAssembly.Memory));
 assert.ok(bytes.byteLength<512,'small inspectable private kernel');
 t.diagnostic(JSON.stringify({kind:'fixed-work-probe-source-inventory',sources:before,wasmBytes:bytes.byteLength}));
});

test('actual WASM and JS match independent u32 mathematics across overflow, sign and loop boundaries',()=>{
 const {kernel}=new WebAssembly.Instance(new WebAssembly.Module(fixedWorkWasmBytes())).exports;
 for(const n of [0,1,2,3,7,31,32,33,127,1023,4096])
  for(const seed of [0,1,-1,0x7fffffff,0x80000000,-2147483648,0xffffffff,123456789]){
   const expected=reference(n,seed);
   assert.equal(fixedWorkJsKernel(n,seed),expected,'JS n='+n+' seed='+seed);
   assert.equal(kernel(n,seed)>>>0,expected,'WASM n='+n+' seed='+seed);
  }
});

test('separate bytes/instances have no mutable state or alias to an unrelated game memory',()=>{
 const bytes=fixedWorkWasmBytes(),other=fixedWorkWasmBytes();
 assert.notEqual(bytes.buffer,other.buffer);const pristine=other.slice();bytes.fill(0);
 assert.deepEqual(other,pristine);
 const module=new WebAssembly.Module(other),a=new WebAssembly.Instance(module),b=new WebAssembly.Instance(module);
 const gameMemory=new WebAssembly.Memory({initial:1,maximum:2}),view=new Uint8Array(gameMemory.buffer);
 for(let i=0;i<view.length;++i)view[i]=(i*73+19)&255;
 const prior=view.slice(),buffer=gameMemory.buffer;
 for(const [n,seed] of [[1000,0xffffffff],[0,0x80000000],[33,123456789],[1000,0xffffffff]]){
  const expected=reference(n,seed);
  assert.equal(a.exports.kernel(n,seed)>>>0,expected);
  assert.equal(b.exports.kernel(n,seed)>>>0,expected);
 }
 assert.equal(gameMemory.buffer,buffer);assert.deepEqual(view,prior);
});

test('default OFF does not touch clocks, WASM, boundary, runtime or scheduler',()=>{
 const descriptors=Object.fromEntries(['performance','WebAssembly','__th10Runtime',
  'requestAnimationFrame','cancelAnimationFrame','setTimeout','setInterval','fetch']
  .map(name=>[name,{get:poison(name)}]));
 withGlobals(descriptors,()=>{
  assert.equal(runFixedWorkProbe(),null);assert.equal(runFixedWorkProbe({}),null);
  assert.equal(runFixedWorkProbe({enabled:false,get boundary(){throw Error('Disabled boundary read');}}),null);
 });
});

test('opt-in and the caller boundary token are mandatory before any work',()=>{
 withGlobals({performance:{get:poison('performance')},WebAssembly:{get:poison('WebAssembly')}},()=>{
  for(const enabled of [0,1,'true',null])assert.throws(()=>runFixedWorkProbe({enabled,boundary}),/enabled must be boolean/);
  for(const value of [undefined,null,'before-fence','post-state-fence',''])
   assert.throws(()=>runFixedWorkProbe({enabled:true,boundary:value}),/explicit post-state\/audio-fence/);
  for(const seat of [-1,3,1.5,'0'])assert.throws(()=>runFixedWorkProbe({enabled:true,boundary,seat}),/seat must/);
  assert.throws(()=>runFixedWorkProbe({enabled:true,boundary,label:'x'.repeat(121)}),/label must/);
 });
});

test('actual fixed run separates construction, compilation, instantiation, two warmups and five alternating measured pairs',()=>{
 const gameMemory=new WebAssembly.Memory({initial:1}),view=new Uint8Array(gameMemory.buffer);
 view.fill(0xa7);const prior=view.slice(),buffer=gameMemory.buffer;
 const descriptors=Object.fromEntries(['__th10Runtime','requestAnimationFrame','cancelAnimationFrame',
  'setTimeout','setInterval','fetch'].map(name=>[name,{get:poison(name)}]));
 const report=withGlobals(descriptors,()=>runFixedWorkProbe({enabled:true,boundary,seat:1,label:'node-real-private-wasm'}));
 assert.equal(report.passed,true);assert.equal(report.completed,true);assert.deepEqual(report.errors,[]);
 assert.equal(report.schema,1);assert.equal(report.kind,'same-page-post-fence-fixed-work');
 assert.equal(report.boundary,boundary);assert.match(report.boundaryAuthority,/caller-declared/);
 assert.equal(report.seat,1);assert.equal(report.label,'node-real-private-wasm');
 assert.equal(report.timeOriginMs,performance.timeOrigin);assert.equal(report.finalTimeOriginMs,report.timeOriginMs);
 assert.equal(report.totalElapsedMs,report.finishedAtPerformanceMs-report.startedAtPerformanceMs);
 assert.equal(report.threadTimeMeasured,false);assert.match(report.scope,/does not establish normal gameplay/);
 assert.doesNotMatch(JSON.stringify(report),/"(?:tdur|cpuMs|threadDurationMs|gameplayPassed|performancePassed)"\s*:/);
 assert.deepEqual(report.policy,fixedWorkProbePolicy);assert.equal(report.policy.iterations,2000000);
 assert.deepEqual(report.module.imports,[]);assert.deepEqual(report.module.sectionIds,[1,3,7,10]);
 assert.equal(report.module.definedMemories,0);assert.equal(report.module.byteLength,fixedWorkWasmBytes().length);
 assert.equal(report.warmup.groups.length,2);assert.equal(report.measurements.groups.length,5);
 let previous=report.startedAtPerformanceMs;
 for(const phase of ['construction','compilation','instantiation','warmup','measurements']){
  const section=report[phase];checkTiming(section,phase);
  assert.ok(section.startedAtPerformanceMs>=previous,phase+' does not overlap previous phase');
  if(section.groups){let runEnd=section.startedAtPerformanceMs;
   for(const row of section.groups){
    assert.equal(row.phase,phase);assert.equal(row.matches,true);assert.equal(row.iterations,2000000);
    assert.equal(row.globalIndex,row.index+(phase==='measurements'?2:0));
    const seed=Number((123456789n+BigInt(row.globalIndex)*0x9e3779b9n)&0xffffffffn);
    assert.equal(row.seed,seed);assert.deepEqual(row.order,row.index%2?['js','wasm']:['wasm','js']);
    assert.deepEqual(row.runs.map(run=>run.engine),row.order);assert.equal(row.runs.length,2);
    assert.equal(row.runs[0].result,row.runs[1].result);
    for(const run of row.runs){checkTiming(run,phase+' '+row.index+' '+run.engine);
     assert.ok(Number.isInteger(run.result)&&run.result>=0&&run.result<=0xffffffff,'explicit u32 result');
     assert.ok(run.startedAtPerformanceMs>=runEnd,'each raw run ordered separately');
     assert.ok(run.finishedAtPerformanceMs<=section.finishedAtPerformanceMs,'run inside its phase');
     runEnd=run.finishedAtPerformanceMs;
    }
   }
  }
  previous=section.finishedAtPerformanceMs;
 }
 assert.ok(report.finishedAtPerformanceMs>=previous);
 assert.equal(gameMemory.buffer,buffer);assert.deepEqual(view,prior);
 assert.ok(Object.isFrozen(fixedWorkProbePolicy));
});

test('deliberately corrupted WASM return values retain all raw results and cannot become a PASS',()=>{
 const original=WebAssembly,facade=Object.create(original);
 Object.defineProperty(facade,'Instance',{value:class {
  constructor(module){const real=new original.Instance(module);
   this.exports={kernel:(n,seed)=>real.exports.kernel(n,seed)^1};
  }
 }});
 const report=withGlobals({WebAssembly:{value:facade}},()=>runFixedWorkProbe({enabled:true,boundary}));
 assert.equal(report.completed,true);assert.equal(report.passed,false);
 assert.equal(report.warmup.groups.length,2);assert.equal(report.measurements.groups.length,5);
 assert.equal(report.errors.length,7);
 for(const section of [report.warmup,report.measurements])for(const row of section.groups){
  assert.equal(row.matches,false);assert.equal(row.runs.length,2);
  for(const run of row.runs)checkTiming(run,'fault-injection raw run');
  assert.notEqual(row.runs[0].result,row.runs[1].result);
 }
});

test('compilation failure is explicit and preserves stage timing without fabricated warmup or samples',()=>{
 const facade=Object.create(WebAssembly);
 Object.defineProperty(facade,'Module',{value:function(){throw Error('Intentional compile failure');}});
 const report=withGlobals({WebAssembly:{value:facade}},()=>runFixedWorkProbe({enabled:true,boundary}));
 assert.equal(report.completed,false);assert.equal(report.passed,false);
 assert.equal(report.construction.completed,true);assert.equal(report.compilation.completed,false);
 assert.ok(Number.isFinite(report.compilation.wallMs));assert.equal(report.module,null);
 assert.equal(report.warmup.groups.length,0);assert.equal(report.measurements.groups.length,0);
 assert.ok(report.errors.some(error=>error.stage==='compilation'&&error.message==='Intentional compile failure'));
 assert.ok(Number.isFinite(report.totalElapsedMs));
});
