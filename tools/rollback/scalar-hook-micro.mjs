#!/usr/bin/env node
// Test-only Wasm hook microbenchmark. No build or timing occurs in default plan
// mode. Authorize an exclusive run explicitly with --phase=all (or build/measure).
import assert from 'node:assert/strict';
import {spawn,execFileSync} from 'node:child_process';
import {readFile,writeFile,mkdir,appendFile} from 'node:fs/promises';
import {existsSync,readFileSync} from 'node:fs';
import {dirname,resolve,join} from 'node:path';
import {fileURLToPath,pathToFileURL} from 'node:url';
import {createRequire} from 'node:module';
import {performance} from 'node:perf_hooks';
import {cpus,platform,release,arch} from 'node:os';
import {digest,patchAndAudit} from './scalar-hook-micro-wasm.mjs';

const here=dirname(fileURLToPath(import.meta.url));
const options={phase:'plan',repo:resolve(here,'../..'),rounds:12,warmups:3};
for(const arg of process.argv.slice(2)){
  const m=/^--(phase|repo|output|expected-head|emxx|rounds|warmups)=(.+)$/.exec(arg);
  assert(m,'unknown option: '+arg);options[m[1]]=['rounds','warmups'].includes(m[1])?Number(m[2]):m[2];
}
assert(['plan','build','measure','all'].includes(options.phase));
assert(Number.isInteger(options.rounds)&&options.rounds>=12&&options.rounds%6===0,'rounds must be a multiple of six, at least twelve');
assert(Number.isInteger(options.warmups)&&options.warmups>=3,'at least three warmup rounds required');
const repo=resolve(options.repo),output=options.output?resolve(options.output):null;
const variants=['plain','generic','scalar'],widths=[1,2,4,8,16];
const kinds=['heap-dirty','heap-first','outside','cross-dirty','cross-first','cross-first-dirty','cross-last-dirty','mixed'];
const cases=[];
for(let kind=0;kind<7;kind++)for(const width of (kind>=3?widths.slice(1):widths))
  cases.push({id:cases.length,kind,width,name:kinds[kind]+'-'+width});
cases.push({id:cases.length,kind:7,width:0,name:'mixed-widths-and-paths'});
assert.equal(cases.length,32);
const permutationOrders=[[0,1,2],[0,2,1],[1,0,2],[1,2,0],[2,0,1],[2,1,0]];
const allLoopNames=variants.flatMap(v=>[...widths,'mixed'].map(w=>'micro_loop_'+v+'_'+w));
const hookNames=['eagler_rb_touch','eagler_rb_capture','eagler_rb_config_address',
  'eagler_rb_dirty_config_address','eagler_rb_dirty_config_abi','eagler_rb_instrumented',
  'eagler_rb_scalar_abi',...widths.map(w=>'eagler_rb_touch'+w)];
const exports=[...hookNames,...allLoopNames,'micro_initialize','micro_prepare','micro_metadata','micro_finish'];
const description={
  schema:'eagler-hook-micro-plan/1',phase:options.phase,repo,output,rounds:options.rounds,warmups:options.warmups,
  cases,variants,order:permutationOrders.map(p=>p.map(i=>variants[i])),
  hotOperations:1048576,hotSlots:256,firstOperations:65536,firstSlots:65536,heapBytes:8388608,
  timedWork:'One direct Wasm-export invocation executes the entire loop; no per-store JS bridge.',
  plainBaseline:'Same addresses/data/volatile stores. Its heap targets are prelogged outside timing so full restore still passes.',
  customModule:'Production native-generic instrumentation plus exact raw-body substitution for 18 named timing loops.',
  checks:['Raw marker rejects initialization.','Real generic/scalar entry points and cold capture stay instrumented/excluded by the production pass.',
    'Every function body is audited raw/instrumented/final.','Final loops direct-call only their corresponding hook exports.',
    'Full heap/host bytes, capture count, unique block count, guards, checksum and Restore are checked on every trial.'],
  execution:'Use the pinned Node with --no-liftoff --no-wasm-lazy-compilation; source the restored runtime env first.',
  scope:'Hook/store-loop wall cost in optimized single-thread Wasm. This is not gameplay FPS or a production module.',
};
if(options.phase==='plan'){console.log(JSON.stringify(description,null,2));process.exit(0);}
assert(output,'--output is required');
assert(/^[0-9a-f]{40}$/.test(options['expected-head']||''),'--expected-head must be a full frozen commit');
assert.equal(process.version,'v24.19.0','use the recovered pinned Node, not an old hard-coded /opt path');
for(const flag of ['--no-liftoff','--no-wasm-lazy-compilation'])
  assert(process.execArgv.includes(flag),'missing '+flag+'; avoid asynchronous tier-up during measurements');
const git=args=>execFileSync('git',args,{cwd:repo,encoding:'utf8'}).trim();
const freezeSource=()=>{
  const head=git(['rev-parse','HEAD']),tree=git(['rev-parse','HEAD^{tree}']);
  assert.equal(head,options['expected-head'],'source HEAD changed');
  assert.equal(git(['status','--porcelain','--untracked-files=no']),'','tracked source is not frozen');
  return {head,tree};
};
const sources=[
  join(here,'scalar-hook-micro.cpp'),fileURLToPath(import.meta.url),join(here,'scalar-hook-micro-wasm.mjs'),
  join(repo,'common/src/netplay/WriteJournal.cpp'),join(repo,'common/include/eagler/netplay/WriteJournal.hpp'),
  join(repo,'common/src/netplay/SimulationHeap.cpp'),join(repo,'common/third_party/dlmalloc/dlmalloc.inc'),
  join(repo,'tools/rollback/instrument.mjs'),join(repo,'tools/rollback/package.json'),join(repo,'tools/rollback/package-lock.json'),
];
const identity=async path=>{const bytes=await readFile(path);return {path,bytes:bytes.length,sha256:digest(bytes)};};
const identities=async paths=>Promise.all(paths.map(identity));
const verifyFiles=async records=>{for(const record of records)assert.deepEqual(await identity(record.path),record,'changed input '+record.path);};
const exclusiveJson=async(name,value)=>writeFile(join(output,name),JSON.stringify(value,null,2)+'\n',{flag:'wx'});
// Under the fixed eager-optimization flags, an asynchronous Wasm compile can
// outlive Node's other handles. Keep initialization alive, with a real timeout;
// the handle is always cleared before any benchmark loop starts.
async function initializeWasm(label,operation){
  console.log('INITIALIZE '+label);
  let timer;
  try{return await Promise.race([operation(),new Promise((_,reject)=>{
    timer=setTimeout(()=>reject(new Error(label+' did not initialize within 60 seconds')),60000);
  })]);}finally{clearTimeout(timer);}
}
const compiler=options.emxx||process.env.EAGLER_EMXX||join(process.env.EMSDK||'/missing-sdk','upstream/emscripten/em++');
const commandRecords=[];
async function command(label,program,args){
  console.log('COMMAND '+label);
  const record={label,program,args,cwd:repo,startedAt:new Date().toISOString()};
  commandRecords.push(record);
  let stdout='',stderr='',timedOut=false,killTimer;
  await new Promise((ok,no)=>{
    const child=spawn(program,args,{cwd:repo,env:{...process.env,EMCC_CORES:'1'},detached:true,stdio:['ignore','pipe','pipe']});
    record.pid=child.pid;
    const timer=setTimeout(()=>{
      timedOut=true;try{process.kill(-child.pid,'SIGTERM');}catch{}
      killTimer=setTimeout(()=>{try{process.kill(-child.pid,'SIGKILL');}catch{}},5000);
    },240000);
    child.stdout.on('data',x=>stdout+=x);child.stderr.on('data',x=>stderr+=x);
    child.once('error',error=>{clearTimeout(timer);clearTimeout(killTimer);no(error);});
    child.once('close',(code,signal)=>{
      clearTimeout(timer);clearTimeout(killTimer);record.exitCode=code;record.signal=signal;
      record.timedOut=timedOut;record.finishedAt=new Date().toISOString();
      code===0&&!timedOut?ok():no(new Error(label+' failed: '+code+' '+signal+'\n'+stderr));
    });
  }).finally(async()=>{
    await writeFile(join(output,label+'.log'),stdout+stderr,{flag:'wx'});
    await writeFile(join(output,label+'.command.json'),JSON.stringify(record,null,2)+'\n',{flag:'wx'});
  });
  return stdout;
}
function environment(){
  const read=path=>{try{return readFileSync(path,'utf8');}catch(error){return {error:error.code||String(error)};}};
  return {node:process.version,versions:process.versions,execPath:process.execPath,execArgv:process.execArgv,
    platform:platform(),release:release(),arch:arch(),cpuModels:[...new Set(cpus().map(x=>x.model))],
    visibleLogicalCpus:cpus().length,pid:process.pid,cgroup:read('/proc/self/cgroup'),
    cpuMax:read('/sys/fs/cgroup/cpu.max'),cpuStat:read('/sys/fs/cgroup/cpu.stat'),
    cpuPressure:read('/sys/fs/cgroup/cpu.pressure')};
}
async function build(){
  await mkdir(output,{recursive:false});
  const source=freezeSource(),sourceFiles=await identities(sources),startedAt=new Date().toISOString();
  await exclusiveJson('plan.json',description);
  const emccVersion=await command('compiler-version',compiler,['--version']);
  assert(/\b6\.0\.9\b/.test(emccVersion),'expected recovered Emscripten 6.0.9');
  const objectSources=[join(here,'scalar-hook-micro.cpp'),join(repo,'common/src/netplay/WriteJournal.cpp'),
    join(repo,'common/src/netplay/SimulationHeap.cpp')],objects=[];
  const flags=['-O2','-g0','-std=c++17','-fno-lto','-UNDEBUG','-ffp-contract=off','-fno-strict-aliasing',
    '-fno-exceptions','-fno-rtti','-DEAGLER_WRITE_JOURNAL=1','-I'+join(repo,'common/include')];
  for(let i=0;i<objectSources.length;i++){
    const target=join(output,'object-'+i+'.o');objects.push(target);
    await command('compile-'+i,compiler,[...flags,'-c',objectSources[i],'-o',target]);
  }
  const loader=join(output,'micro.mjs');
  await command('link',compiler,[...flags,...objects,'--no-entry','--emit-symbol-map',
    '-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sALLOW_MEMORY_GROWTH=1',
    '-sINITIAL_MEMORY=134217728','-sMAXIMUM_MEMORY=536870912','-sSTACK_SIZE=1048576',
    '-sINVOKE_RUN=0','-sASSERTIONS=0','-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]',
    '-sINCOMING_MODULE_JS_API=["wasmBinary","print","printErr"]',
    '-sEXPORTED_FUNCTIONS='+JSON.stringify(exports.map(x=>'_'+x)),'-o',loader]);
  const rawPath=join(output,'micro.wasm'),raw=await readFile(rawPath),symbolPath=loader+'.symbols';
  const instrumentPath=join(repo,'tools/rollback/instrument.mjs');
  const {instrumentWasm,verifyInstrumentedWasm,INSTRUMENTATION_MAGIC}=await initializeWasm('Binaryen instrumenter',()=>import(pathToFileURL(instrumentPath).href));
  const instrumentOptions={nativeFilter:true,inlineDirtyBlocks:false,scalarHooks:false};
  const generated=await instrumentWasm(raw,instrumentOptions);
  const verified=await verifyInstrumentedWasm(generated.bytes,instrumentOptions);
  const instrumentedPath=join(output,'micro.instrumented.wasm');
  await writeFile(instrumentedPath,generated.bytes,{flag:'wx'});
  await exclusiveJson('instrumentation-before-loop-substitution.json',{options:instrumentOptions,report:generated.report,verified,
    appliesTo:'micro.instrumented.wasm before deliberate test-only timing-loop body substitution'});
  const require=createRequire(pathToFileURL(instrumentPath));
  const binaryen=(await import(pathToFileURL(require.resolve('binaryen')).href)).default;
  await binaryen.ready;
  const patched=patchAndAudit({raw,instrumented:generated.bytes,symbolText:await readFile(symbolPath,'utf8'),binaryen,
    configAddress:generated.report.configAddress});
  const finalPath=join(output,'micro.test-only.wasm');
  await writeFile(finalPath,patched.bytes,{flag:'wx'});
  await exclusiveJson('body-audit.final.json',patched.report);
  const createModule=(await import(pathToFileURL(loader).href)).default;
  const runtimeMessages=[];
  const messages={print:x=>runtimeMessages.push({stream:'stdout',text:String(x)}),printErr:x=>runtimeMessages.push({stream:'stderr',text:String(x)})};
  const negative=await initializeWasm('raw rejection instance',()=>createModule({...messages,wasmBinary:raw}));
  assert.equal(negative._eagler_rb_instrumented(),0);
  assert.equal(negative._micro_initialize(),0,'raw Wasm cannot serve as a successful journal baseline');
  const positive=await initializeWasm('instrumented instance',()=>createModule({...messages,wasmBinary:patched.bytes}));
  assert.equal(positive._eagler_rb_instrumented(),INSTRUMENTATION_MAGIC);
  assert.equal(positive._eagler_rb_scalar_abi(),1);
  assert.equal(positive._micro_initialize(),1);
  assert.equal(positive._micro_metadata(0),1);
  assert(!(positive.HEAPU8.buffer instanceof SharedArrayBuffer),'threaded Wasm is outside this benchmark contract');
  await verifyFiles(sourceFiles);assert.deepEqual(freezeSource(),source);
  const record={schema:'eagler-hook-micro-build/1',status:'BUILT_ONLY',testOnly:true,source,sourceFiles,startedAt,
    finishedAt:new Date().toISOString(),environment:environment(),compilerVersion:emccVersion,
    commands:commandRecords,objects:await identities(objects),raw:await identity(rawPath),
    instrumented:await identity(instrumentedPath),module:await identity(finalPath),loader:await identity(loader),
    symbolMap:await identity(symbolPath),bodyAudit:await identity(join(output,'body-audit.final.json')),
    runtimeMessages,rawInitializationRejected:true,instrumentedInitializationPassed:true,
    sampleCount:0,timingExecuted:false};
  await exclusiveJson('build.final.json',record);
  console.log('BUILD PASS '+JSON.stringify({module:record.module,bodyAudit:record.bodyAudit}));
  return record;
}
const readMetadata=module=>Array.from({length:40},(_,i)=>module._micro_metadata(i)>>>0);
const sameWork=metadata=>[metadata[2],metadata[3],metadata[4],metadata[5],metadata[6],metadata[15],metadata[16],metadata[17],...metadata.slice(30,37)];
const median=values=>{const v=[...values].sort((a,b)=>a-b),n=v.length;return n%2?v[n>>1]:(v[n/2-1]+v[n/2])/2;};
async function measure(buildRecord){
  assert.equal(buildRecord.status,'BUILT_ONLY');
  assert.deepEqual(freezeSource(),buildRecord.source);await verifyFiles(buildRecord.sourceFiles);
  await verifyFiles([buildRecord.raw,buildRecord.instrumented,buildRecord.module,buildRecord.loader,buildRecord.symbolMap,buildRecord.bodyAudit]);
  assert(!existsSync(join(output,'measurements.jsonl')),'measurement output already exists');
  const environmentBefore=environment(),startedAt=new Date().toISOString(),rows=[],runtimeMessages=[];
  const createModule=(await import(pathToFileURL(buildRecord.loader.path).href)).default;
  const wasmBinary=await readFile(buildRecord.module.path);
  const module=await initializeWasm('measurement instance',()=>createModule({wasmBinary,
    print:x=>runtimeMessages.push({stream:'stdout',text:String(x)}),printErr:x=>runtimeMessages.push({stream:'stderr',text:String(x)})}));
  assert.equal(module._micro_initialize(),1);
  const memoryBytes=module.HEAPU8.byteLength;
  assert(!(module.HEAPU8.buffer instanceof SharedArrayBuffer));
  await exclusiveJson('measurement-start.json',{schema:'eagler-hook-micro-measurement-start/1',
    startedAt,environmentBefore,buildReceipt:await identity(join(output,'build.final.json')),description,memoryBytes});
  async function trial(test,variant,round,warmup,position){
    const seed=((warmup?0x13700000:0x6b900000)^(test.id*0x1009)^(round*0x101))>>>0;
    assert.equal(module._micro_prepare(test.kind,test.width,variant,seed),1);
    const before=readMetadata(module),name='micro_loop_'+variants[variant]+'_'+(test.width||'mixed');
    assert.equal(before[2],(test.kind===0||test.kind===2||test.kind===3)?1048576:65536);
    assert.equal(before[20],test.width);assert.equal(before[21],variant);assert.equal(module.HEAPU8.byteLength,memoryBytes);
    const call=module['_'+name];assert.equal(typeof call,'function');
    const start=performance.now();
    const checksum=call(before[1],before[2],before[3],before[4])>>>0;
    const end=performance.now();
    assert.equal(checksum,before[5]);assert.equal(module.HEAPU8.byteLength,memoryBytes);
    assert.equal(module._micro_finish(checksum),1);
    const after=readMetadata(module);
    assert.equal(after[24],before[6]);assert.equal(after[25],before[8]);assert.equal(after[28],before[18]);assert.equal(after[29],1);
    if(variant===0)assert.equal(before[8],0,'plain restore must be fully prelogged');
    const elapsedMs=end-start;assert(Number.isFinite(elapsedMs)&&elapsedMs>0,'unresolved trial clock');
    const row={case:test.name,caseId:test.id,kind:test.kind,width:test.width,variant:variants[variant],round,warmup,position,
      export:name,start,end,elapsedMs,operations:before[2],logicalStoreBytes:before[15]+before[16]*2**32,
      nsPerIteration:elapsedMs*1e6/before[2],plannedHookCalls:variant===0?0:before[2],checksum,metadataBefore:before,
      metadataAfter:after,memoryBytes,fullWorkAndRestorePassed:true};
    await appendFile(join(output,'measurements.jsonl'),JSON.stringify(row)+'\n',{flag:'a'});
    rows.push(row);return row;
  }
  for(const test of cases){
    for(let round=0;round<options.warmups;round++)
      for(const [position,variant] of permutationOrders[round%6].entries())await trial(test,variant,round,true,position);
    console.log('WARM '+test.name);
  }
  const measuredWallStart=performance.now(),measuredCpuStart=process.cpuUsage();
  for(const test of cases){
    for(let round=0;round<options.rounds;round++){
      const current=[];
      for(const [position,variant] of permutationOrders[round%6].entries())current.push(await trial(test,variant,round,false,position));
      for(const row of current)assert.deepEqual(sameWork(row.metadataBefore),sameWork(current[0].metadataBefore),'variant work differs');
      const generic=current.find(x=>x.variant==='generic'),scalar=current.find(x=>x.variant==='scalar');
      assert.equal(generic.metadataBefore[7],scalar.metadataBefore[7]);
      assert.equal(generic.metadataBefore[8],scalar.metadataBefore[8]);
    }
    console.log('MEASURE '+test.name+' '+options.rounds+' complete rounds');
  }
  const measuredCpu=process.cpuUsage(measuredCpuStart),measuredEnvelopeMs=performance.now()-measuredWallStart;
  const summaries=cases.map(test=>{
    const selected=rows.filter(r=>!r.warmup&&r.caseId===test.id),perVariant={};
    for(const variant of variants){
      const values=selected.filter(r=>r.variant===variant);
      assert.equal(values.length,options.rounds);
      perVariant[variant]={trials:values.length,medianMs:median(values.map(x=>x.elapsedMs)),
        minimumMs:Math.min(...values.map(x=>x.elapsedMs)),maximumMs:Math.max(...values.map(x=>x.elapsedMs)),
        medianNsPerIteration:median(values.map(x=>x.nsPerIteration))};
    }
    const paired=Array.from({length:options.rounds},(_,round)=>{
      const values=Object.fromEntries(selected.filter(x=>x.round===round).map(x=>[x.variant,x]));
      return {round,scalarOverGeneric:values.scalar.elapsedMs/values.generic.elapsedMs,
        genericMinusPlainNs:values.generic.nsPerIteration-values.plain.nsPerIteration,
        scalarMinusPlainNs:values.scalar.nsPerIteration-values.plain.nsPerIteration};
    });
    return {...test,perVariant,paired,medianPairedScalarOverGeneric:median(paired.map(x=>x.scalarOverGeneric))};
  });
  await verifyFiles(buildRecord.sourceFiles);assert.deepEqual(freezeSource(),buildRecord.source);
  await verifyFiles([buildRecord.raw,buildRecord.instrumented,buildRecord.module,buildRecord.loader,buildRecord.symbolMap,buildRecord.bodyAudit]);
  const final={schema:'eagler-hook-micro-measurement/1',status:'PASS',testOnly:true,gameplayPerformanceResult:false,
    startedAt,finishedAt:new Date().toISOString(),environmentBefore,environmentAfter:environment(),
    source:buildRecord.source,buildReceipt:await identity(join(output,'build.final.json')),
    module:buildRecord.module,bodyAudit:buildRecord.bodyAudit,description,memoryBytes,
    sampleCount:rows.length,measuredSampleCount:cases.length*options.rounds*3,warmupSampleCount:cases.length*options.warmups*3,
    rawSamples:await identity(join(output,'measurements.jsonl')),summaries,runtimeMessages,
    processEnvelope:{elapsedMs:measuredEnvelopeMs,cpuMicroseconds:measuredCpu,
      includes:'All measured trials plus intervening preparation, full validation, disk logging and scheduling.'},
    limitations:['Only the direct loop invocation is in each per-trial timer.',
      'Raw warmup and measured trials are all retained; no slow sample is discarded.',
      'Plain prelogging and all frame/dirty preparation occur outside timing.',
      'Generic and scalar use identical prepared dirty state, work and capture counts.',
      'A 16-byte logical store is two volatile i64 stores in this fixture, preceded by one 16-byte hook.',
      'The optimized pinned Node engine and this loop mix do not reproduce gameplay, CPU4 throttling or GPU load.',
      'Differences above the plain baseline are contextual measurements, not an additive model of game frame cost.',
      'The custom module is not a production artifact or a substitute for the actual instrumenter memory/oracle suites.']};
  await exclusiveJson('measurement.final.json',final);
  console.log('MICRO PASS '+JSON.stringify({report:await identity(join(output,'measurement.final.json')),sampleCount:rows.length}));
}
try{
  const record=options.phase==='measure'?JSON.parse(await readFile(join(output,'build.final.json'),'utf8')):await build();
  if(options.phase==='measure'||options.phase==='all')await measure(record);
}catch(error){
  console.error(error.stack||error);
  if(output&&existsSync(output)){
    const path=join(output,'failure-'+Date.now()+'.json');
    await writeFile(path,JSON.stringify({status:'FAIL',phase:options.phase,time:new Date().toISOString(),
      error:String(error.stack||error),commands:commandRecords},null,2)+'\n',{flag:'wx'}).catch(()=>{});
  }
  process.exitCode=1;
}
