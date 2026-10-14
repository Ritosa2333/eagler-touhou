import assert from 'node:assert/strict';
import {spawn} from 'node:child_process';
import {createHash} from 'node:crypto';
import {appendFileSync, existsSync, mkdirSync, readFileSync, renameSync, writeFileSync} from 'node:fs';
import {dirname, isAbsolute, relative, resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';

// Independent experiment only. Compile serially, outside a title build/test.
// It never writes production sources, build scripts, or the existing harness.
const root=resolve(dirname(fileURLToPath(import.meta.url)), '../..');
const usage=[
  'node portable/multiplayer/check-arithmetic-wrapper-equivalence.mjs',
  '  [--output artifacts/.../receipt.json] [--sdk PATH] [--python EXE]',
  '  [--print-plan] [--no-run]',
  'Use a fresh output directory. --print-plan performs reads only.',
  'The candidate is generated from pinned bytes ONLY inside that directory.',
].join('\n');
const options={};
for(let index=2;index<process.argv.length;++index) {
  const key=process.argv[index];
  if(key==='--help') {console.log(usage);process.exit(0);}
  if(key==='--print-plan') {options.printPlan=true;continue;}
  if(key==='--no-run') {options.noRun=true;continue;}
  if(!['--output','--sdk','--python'].includes(key)||index+1>=process.argv.length)
    throw Error('Invalid argument: '+key+'\n'+usage);
  options[key.slice(2)]=process.argv[++index];
}
const stamp=new Date().toISOString().replace(/[-:.]/g,'');
const output=resolve(root,options.output||
  'artifacts/rollback-smoothness/arithmetic-flatten/'+stamp+'/receipt.json');
const out=dirname(output),buildDir=resolve(out,'experiment-build');
const artifacts=resolve(root,'artifacts'),withinArtifacts=relative(artifacts,out);
assert(withinArtifacts&&!withinArtifacts.startsWith('..')&&!isAbsolute(withinArtifacts),
       'Use an isolated directory below this worktree artifacts directory.');
assert(output.endsWith('.json'),'--output must name a JSON receipt.');
const manifestPath=resolve(root,'tests/fixtures/arithmetic-flatten/manifest.json');
const probePath=resolve(root,'tests/arithmetic-wrapper-equivalence-probe.cpp');
const runnerPath=fileURLToPath(import.meta.url);
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
const normalized=path=>relative(root,path).replaceAll('\\','/');
const errorInfo=error=>({
  name:String(error?.name||'Error'),message:String(error?.message||error).slice(0,4096),
  stack:String(error?.stack||'').slice(0,8192),
});
const expectedDependencies=[
  'th10_web/cpp/game/Arithmetic.hpp','th10_web/cpp/game/Types.hpp',
  'portable/numeric/ExactFloat.hpp','portable/numeric/ScalarMath.hpp',
  'th10_web/cpp/rebuild/third_party/softfloat.c',
];
const manifest=JSON.parse(readFileSync(manifestPath,'utf8'));
assert.equal(manifest.schema,1);
assert.equal(manifest.source,'th10_web/cpp/game/Arithmetic.cpp');
assert.equal(manifest.snapshot,'tests/fixtures/arithmetic-flatten/Arithmetic.cpp');
assert.equal(manifest.candidate.attribute,'__attribute__((flatten)) ');
assert.deepEqual(manifest.candidate.operators,['+','-','*','/']);
assert.deepEqual(Object.keys(manifest.dependencies).sort(),[...expectedDependencies].sort());
const baselinePath=resolve(root,manifest.snapshot);
const liveArithmetic=resolve(root,manifest.source);
const pinned=readFileSync(baselinePath);
assert.equal(pinned.length,manifest.snapshotBytes,'Pinned source byte count');
assert.equal(sha(pinned),manifest.snapshotSha256,'Pinned original byte hash');
assert.equal(manifest.sourceSha256,manifest.snapshotSha256,'Original/snapshot pin mismatch');
assert(readFileSync(liveArithmetic).equals(pinned),
       'Live Arithmetic.cpp drifted from the frozen baseline; this experiment does not validate that revision.');
const dependencyBytes=new Map();
for(const name of expectedDependencies) {
  const bytes=readFileSync(resolve(root,name)),pin=manifest.dependencies[name];
  assert.equal(bytes.length,pin.bytes,'Dependency byte count: '+name);
  assert.equal(sha(bytes),pin.sha256,'Pinned dependency drift: '+name);
  dependencyBytes.set(name,bytes);
}
const originalText=pinned.toString('utf8');
assert(Buffer.from(originalText,'utf8').equals(pinned),'Baseline UTF-8 did not round-trip byte-exactly');
const attribute=manifest.candidate.attribute;
assert(!originalText.includes(attribute),'Baseline already contains the experiment attribute');
let candidateText=originalText;
const insertions=[];
for(const operation of manifest.candidate.operators) {
  const signature='Extended operator'+operation+'(Extended a, Extended b) noexcept {';
  const offset=originalText.indexOf(signature);
  assert(offset>=0&&originalText.indexOf(signature,offset+signature.length)===-1,
         'Expected exactly one original wrapper signature: '+operation);
  assert(offset===0||originalText[offset-1]==='\n','Wrapper signature must start its own line');
  candidateText=candidateText.replace(signature,attribute+signature);
  insertions.push({operation,signature,originalByteOffset:Buffer.byteLength(originalText.slice(0,offset)),
    line:originalText.slice(0,offset).split('\n').length,insertedBytes:Buffer.byteLength(attribute)});
}
let stripped=candidateText;
for(const insertion of insertions)stripped=stripped.replace(attribute+insertion.signature,insertion.signature);
assert(Buffer.from(stripped,'utf8').equals(pinned),'Candidate changes exceed the four exact insertions');
assert.equal(candidateText.split(attribute).length-1,4,'Candidate attribute count');
const candidate=Buffer.from(candidateText,'utf8');
assert.equal(candidate.length,pinned.length+4*Buffer.byteLength(attribute));
const sourcePaths=[manifestPath,baselinePath,liveArithmetic,probePath,runnerPath,
  ...expectedDependencies.map(name=>resolve(root,name))];
const sourceHashes=()=>Object.fromEntries(sourcePaths.map(path=>[normalized(path),sha(readFileSync(path))]));
const capturedHashes=sourceHashes();

function compilerPlan() {
  const explicit=options.sdk||process.env.EMSDK;
  const candidates=explicit?[resolve(explicit)]:[
    resolve(root,'tools/emsdk'),resolve(root,'../toolchains/emsdk'),
    resolve(root,'../../toolchains/emsdk'),
    ...(process.env.EAGLER_WORKSPACE?[resolve(process.env.EAGLER_WORKSPACE,'toolchains/emsdk')]:[]),
  ];
  for(const sdk of candidates) {
    const emcc=['install/emscripten/emcc.py','upstream/emscripten/emcc.py']
      .map(name=>resolve(sdk,name)).find(existsSync);
    if(!emcc)continue;
    const config=process.env.EM_CONFIG||resolve(sdk,'.emscripten');
    assert(existsSync(config),'Missing Emscripten config: '+config);
    const inherited=process.env.EMCC_CFLAGS||'';
    assert(!/(^|\s)(-flto(?:[=\s]|$)|-ffast-math(?:\s|$)|-funsafe-math-optimizations(?:\s|$))/.test(inherited),
           'This experiment requires non-LTO objects and the production strict numeric flags.');
    const python=options.python||process.env.TH_PYTHON||process.env.EMSDK_PYTHON||'python';
    const settings=resolve(dirname(emcc),'src/settings.js');
    const safari=existsSync(settings)?/var\s+MIN_SAFARI_VERSION\s*=\s*(\d+)/.exec(readFileSync(settings,'utf8')):null;
    const numeric=['-O2','-g0','-fno-strict-aliasing','-ffp-contract=off',
      '-DTH_SDL3=1','-DTH_NATIVE_PLATFORM=1',
      '-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1','-DTH_ENABLE_NETPLAY=1'];
    const cpp=[...numeric,'-std=c++17','-fno-exceptions','-fno-rtti',
      '-Werror=unknown-attributes','-Werror=ignored-attributes'];
    const c=[...numeric,'-x','c','-std=c11','-DSOFTFLOAT_FAST_INT64','-DINLINE_LEVEL=5'];
    const link=[...cpp,'-sDEFAULT_TO_CXX=1','--no-entry','-sMODULARIZE=1','-sEXPORT_ES6=1',
      '-sENVIRONMENT=node','-sALLOW_MEMORY_GROWTH=1','-sSTACK_SIZE=1048576',
      '-sINITIAL_MEMORY=67108864','-sMAXIMUM_MEMORY=1073741824','-sFILESYSTEM=0',
      '-sINVOKE_RUN=0','-sEXIT_RUNTIME=0','--emit-symbol-map'];
    return {sdk,emcc,python,config,numeric,cpp,c,link,
      sdkDefaultMinSafariVersion:safari?Number(safari[1]):null,
      env:{...process.env,EMSDK:sdk,EM_CONFIG:config,EMCC_CORES:process.env.EMCC_CORES||'1'},
      recordedEnv:{EMSDK:sdk,EM_CONFIG:config,EMCC_CORES:process.env.EMCC_CORES||'1',EMCC_CFLAGS:inherited},
    };
  }
  throw Error('Local Emscripten SDK not found; set --sdk or EMSDK. No download is attempted.');
}
const plan=compilerPlan();
const bridgeObject=resolve(buildDir,'bridge.o'),softfloatObject=resolve(buildDir,'softfloat.o');
const softfloatSource=resolve(root,'th10_web/cpp/rebuild/third_party/softfloat.c');
const variants=['reference','candidate'].map(name=>({
  name,tree:resolve(buildDir,name,'source'),
  arithmetic:resolve(buildDir,name,'source',manifest.source),
  object:resolve(buildDir,name,'arithmetic.o'),ir:resolve(buildDir,name,'arithmetic.ll'),
  loader:resolve(buildDir,name,'arithmetic-oracle.mjs'),
  wasm:resolve(buildDir,name,'arithmetic-oracle.wasm'),
}));
const commands=[
  {name:'bridge',args:[...plan.cpp,'-c',probePath,'-o',bridgeObject]},
  {name:'production-softfloat',args:[...plan.c,'-c',softfloatSource,'-o',softfloatObject]},
];
for(const variant of variants) {
  commands.push({name:variant.name+'-arithmetic',args:[...plan.cpp,'-c',variant.arithmetic,'-o',variant.object]});
  commands.push({name:variant.name+'-ir',args:[...plan.cpp,'-S','-emit-llvm',variant.arithmetic,'-o',variant.ir]});
}
for(const variant of variants)
  commands.push({name:variant.name+'-link',args:[...plan.link,bridgeObject,softfloatObject,variant.object,'-o',variant.loader]});
const receipt={
  schema:1,experiment:'th10-four-wrapper-flatten',status:'RUNNING',
  startedAt:new Date().toISOString(),sourceRoot:root,output,
  sourceHashes:capturedHashes,
  node:{executable:process.execPath,version:process.version,platform:process.platform},
  baseline:{path:baselinePath,bytes:pinned.length,sha256:sha(pinned),manifest:manifestPath},
  experimentCandidate:{generatedOnlyInOutput:true,productionModified:false,
    bytes:candidate.length,sha256:sha(candidate),strippedByteExact:true,insertions},
  toolchain:{sdk:plan.sdk,emcc:plan.emcc,python:plan.python,config:plan.config,
    sdkDefaultMinSafariVersion:plan.sdkDefaultMinSafariVersion,env:plan.recordedEnv,
    compilerDriverSha256:sha(readFileSync(plan.emcc))},
  differencesFromTitleBuild:[
    'Only an isolated probe, one Arithmetic.cpp object, and the unchanged production SoftFloat object per module.',
    'The same bridge.o and SoftFloat.o are linked into two separate modules with separate linear memories.',
    'Candidate source is generated in the output tree from the pinned original with exactly four flatten attributes.',
    'Node instead of web,worker; diagnostic exports; no SDL/title/resources/filesystem.',
    'Same core O2/numeric flags and 64 MiB initial / 1 GiB maximum memory / 1 MiB stack.',
    'No LTO, unsafe math, Safari version, or WASM memory-feature override.',
    'Results include the full 16-byte Extended representation; valid10 diagnostics never weaken the raw-byte gate.',
    'Correctness and compiler-output evidence only; no FPS, latency, full44, audio, or performance claim.',
  ],
  builds:[],modules:[],comparisons:{},coverage:null,
  correctnessPassed:false,productionReady:false,
};
function saveReceipt() {
  mkdirSync(out,{recursive:true});
  const temporary=output+'.tmp';
  writeFileSync(temporary,JSON.stringify(receipt,null,2)+'\n');renameSync(temporary,output);
}
if(options.printPlan) {
  console.log(JSON.stringify({...receipt,status:'PLAN_ONLY',
    commands:commands.map(command=>({name:command.name,command:plan.python,argv:[plan.emcc,...command.args]}))},null,2));
  process.exit(0);
}
assert(!existsSync(out),'Refusing to overwrite an existing directory; use a fresh --output directory.');
mkdirSync(buildDir,{recursive:true});
saveReceipt();
const compileLog=resolve(out,'compile.log');
writeFileSync(compileLog,'');
function write(path,bytes) {mkdirSync(dirname(path),{recursive:true});writeFileSync(path,bytes);}
function assertSourcesUnchanged() {assert.deepEqual(sourceHashes(),capturedHashes,'Source changed during the experiment');}
async function runCommand(command) {
  const result={name:command.name,command:plan.python,argv:[plan.emcc,...command.args],status:'RUNNING',
    stdoutTail:'',stderrTail:'',log:compileLog};
  receipt.builds.push(result);saveReceipt();
  console.log('BUILD '+command.name);
  const started=Date.now();
  await new Promise((done,reject)=>{
    const child=spawn(plan.python,result.argv,{
      cwd:root,env:plan.env,shell:false,windowsHide:true,stdio:['ignore','pipe','pipe'],
    });
    let bytes=0,reason=null,finished=false;
    const timer=setTimeout(()=>{reason=Error('Compiler step exceeded 180 seconds: '+command.name);child.kill();},180000);
    const collect=(key,chunk)=>{
      result[key]=(result[key]+chunk.toString()).slice(-8192);
      const remaining=Math.max(0,256*1024-bytes);
      if(remaining)appendFileSync(compileLog,chunk.subarray(0,remaining));
      bytes+=chunk.length;
      if(bytes>256*1024&&!reason) {reason=Error('Compiler output exceeded 256 KiB: '+command.name);child.kill();}
    };
    child.stdout.on('data',chunk=>collect('stdoutTail',chunk));
    child.stderr.on('data',chunk=>collect('stderrTail',chunk));
    child.on('error',error=>{if(finished)return;finished=true;clearTimeout(timer);reject(error);});
    child.on('close',(code,signal)=>{
      if(finished)return;finished=true;clearTimeout(timer);
      result.exitCode=code;result.signal=signal;result.elapsedMs=Date.now()-started;result.outputBytes=bytes;
      if(reason||code!==0)reject(reason||Error('Compiler exit '+code+': '+command.name));
      else done();
    });
  });
  result.status='PASS';saveReceipt();assertSourcesUnchanged();
}
function codeSection(bytes) {
  assert(bytes.subarray(0,8).equals(Buffer.from([0,97,115,109,1,0,0,0])),'Invalid WASM header');
  let position=8;
  while(position<bytes.length) {
    const id=bytes[position++];let size=0,shift=0,complete=false;
    for(let count=0;count<5;++count) {
      assert(position<bytes.length,'Truncated section length');
      const value=bytes[position++];size+=(value&127)*2**shift;
      if(!(value&128)) {complete=true;break;}shift+=7;
    }
    assert(complete&&position+size<=bytes.length,'Invalid section length');
    if(id===10)return bytes.subarray(position,position+size);
    position+=size;
  }
  throw Error('WASM module contains no code section');
}
function wrapperIR(path) {
  const text=readFileSync(path,'utf8'),lines=text.split(/\r?\n/),functions={};
  for(let index=0;index<lines.length;++index) {
    const match=/^define\b.*@(_ZN4th10(pl|mi|ml|dv)E[^(\s]*)\(/.exec(lines[index]);
    if(!match)continue;
    const body=[lines[index]];
    while(++index<lines.length) {body.push(lines[index]);if(lines[index]==='}')break;}
    assert.equal(body.at(-1),'}','Truncated wrapper IR');
    const value=body.join('\n')+'\n';
    const symbol=match[1],operation={pl:'+',mi:'-',ml:'*',dv:'/'}[match[2]];
    assert(!functions[operation],'Duplicate wrapper IR: '+operation);
    const calls=[...value.matchAll(/\bcall\b[^\n@]*@([^\s(]+)\(/g)].map(item=>item[1]);
    const floatOps=Object.fromEntries(['fadd','fsub','fmul','fdiv'].map(name=>
      [name,(value.match(new RegExp('\\b'+name+'\\b','g'))||[]).length]));
    functions[operation]={symbol,body:value,sha256:sha(value),lines:body.length,calls,floatOps,
      binaryHelperCalls:calls.filter(name=>name.includes('numeric6binary')).length};
  }
  assert.deepEqual(Object.keys(functions).sort(),['+','-','*','/'].sort(),'Expected four production wrappers in IR');
  return functions;
}
function compilerEvidence() {
  const ref=wrapperIR(variants[0].ir),cand=wrapperIR(variants[1].ir),wrappers=[];
  for(const [operation,expectedFloat] of [['+','fadd'],['-','fsub'],['*','fmul'],['/','fdiv']]) {
    const a=ref[operation],b=cand[operation],name={'+':'add','-':'sub','*':'mul','/':'div'}[operation];
    const refFile=resolve(out,'ir',name+'-reference.ll'),candFile=resolve(out,'ir',name+'-candidate.ll');
    write(refFile,a.body);write(candFile,b.body);
    const singleOperation=b.floatOps[expectedFloat]>0&&
      Object.entries(b.floatOps).every(([key,value])=>key===expectedFloat||value===0);
    wrappers.push({
      operation,reference:{...a,body:undefined,file:refFile},candidate:{...b,body:undefined,file:candFile},
      bodyTextChanged:a.body!==b.body,
      constantOperationVisible:singleOperation,
      helperDispatchRemoved:a.binaryHelperCalls>0&&b.binaryHelperCalls===0,
    });
  }
  const objects={
    bridge:{path:bridgeObject,sha256:sha(readFileSync(bridgeObject))},
    softfloat:{path:softfloatObject,sha256:sha(readFileSync(softfloatObject)),
      sourceSha256:sha(readFileSync(softfloatSource))},
  };
  for(const variant of variants) {
    const bytes=readFileSync(variant.wasm),compiled=new WebAssembly.Module(bytes);
    const section=codeSection(bytes);
    receipt.modules.push({
      name:variant.name,loader:variant.loader,wasm:variant.wasm,bytes:bytes.length,
      sha256:sha(bytes),codeBytes:section.length,codeSha256:sha(section),
      ir:variant.ir,irSha256:sha(readFileSync(variant.ir)),
      loaderSha256:sha(readFileSync(variant.loader)),arithmeticSource:variant.arithmetic,
      arithmeticSourceSha256:sha(readFileSync(variant.arithmetic)),
      arithmeticObject:variant.object,arithmeticObjectSha256:sha(readFileSync(variant.object)),
      linkedObjects:[objects.bridge,objects.softfloat,{path:variant.object,sha256:sha(readFileSync(variant.object))}],
      exports:WebAssembly.Module.exports(compiled),imports:WebAssembly.Module.imports(compiled),
    });
  }
  assert.notEqual(receipt.modules[0].arithmeticSourceSha256,receipt.modules[1].arithmeticSourceSha256);
  const evidence={wrappers,sharedObjects:objects,
    distinctSourceModules:true,
    wasmCodeSectionChanged:receipt.modules[0].codeSha256!==receipt.modules[1].codeSha256,
    allFourConstantOperationsVisible:wrappers.every(item=>item.constantOperationVisible),
    removedBinaryDispatchWrappers:wrappers.filter(item=>item.helperDispatchRemoved).map(item=>item.operation),
    interpretation:'IR/body/code differences are compiler evidence, not performance measurements. Root review is required.',
  };
  write(resolve(out,'code-diff.json'),JSON.stringify(evidence,null,2)+'\n');
  return evidence;
}
const recordBytes=64,recordWords=16;
const expected={cases:140928,records:169728,directed:110592,random:24576,conversion:2304,chains:3456};
function readWords(fixture,pointer,count) {
  assert(Number.isInteger(pointer)&&pointer>0&&pointer%4===0&&pointer+count*4<=fixture.memory.buffer.byteLength,
         'Invalid probe word range');
  const view=new DataView(fixture.memory.buffer);
  return Array.from({length:count},(_,index)=>view.getUint32(pointer+4*index,true));
}
function copyBytes(fixture,pointer,count) {
  assert(pointer>0&&count>=0&&pointer+count<=fixture.memory.buffer.byteLength,'Invalid probe byte range');
  return Buffer.from(new Uint8Array(fixture.memory.buffer,pointer,count));
}
function invoke(fixture,name,...args) {
  assert(!fixture.poisoned,'Do not reenter a trapped probe');
  try {return fixture.module['_arithmetic_oracle_'+name](...args);}
  catch(error) {fixture.poisoned=true;throw error;}
}
async function instantiate(variant) {
  const artifact=receipt.modules.find(item=>item.name===variant.name);
  assert(artifact,'Missing compiled module provenance');
  assert.equal(sha(readFileSync(variant.loader)),artifact.loaderSha256,'Loader changed before execution');
  const bytes=readFileSync(variant.wasm);
  assert.equal(sha(bytes),artifact.sha256,'WASM changed before execution');
  const create=(await import(pathToFileURL(variant.loader).href)).default;
  const compiled=new WebAssembly.Module(bytes);
  let instance=null,stdout='',stderr='';
  const module=await create({
    noInitialRun:true,wasmBinary:bytes,
    print:value=>{stdout=(stdout+String(value)+'\n').slice(-8192);},
    printErr:value=>{stderr=(stderr+String(value)+'\n').slice(-8192);},
    instantiateWasm(imports,receive) {
      instance=new WebAssembly.Instance(compiled,imports);
      receive(instance,compiled);return instance.exports;
    },
  });
  assert(instance,'Probe did not instantiate');
  const memories=Object.values(instance.exports).filter(value=>value instanceof WebAssembly.Memory);
  assert.equal(memories.length,1,'Expected exactly one exported linear memory');
  const fixture={name:variant.name,module,instance,memory:memories[0],poisoned:false,stdout,stderr};
  for(const name of ['info','layout','describe','run'])
    assert.equal(typeof module['_arithmetic_oracle_'+name],'function','Missing C ABI export: '+name);
  const values=readWords(fixture,invoke(fixture,'info')>>>0,24);
  assert.deepEqual(values.slice(0,17),[
    1,16,8,0,8,10,12,1,recordWords,256,2048,expected.cases,expected.records,
    expected.directed,expected.random,expected.conversion,expected.chains,
  ],'Actual wasm32 probe layout/case schema');
  assert(values[17]>0&&values[18]>0&&values[17]!==values[18],'Invalid errno constants');
  assert.deepEqual(values.slice(21),[32,20,24]);
  fixture.info={raw:values,edom:values[17],erange:values[18],records:values[19],status:values[20]};
  const actual=readWords(fixture,invoke(fixture,'layout')>>>0,20);
  const wanted=[
    0,0,0,0,0,0xc0000000,0x3fff,0,
    0,0xa0000000,0xa55abfff,0xdeadbeef,21,80,3,0,fixture.info.edom,0,0,0,
  ];
  assert.deepEqual(actual,wanted,'Extended default/fast conversion/negation or numeric-state layout contract');
  fixture.layout=actual;
  return fixture;
}
function recordsForCase(id) {
  if(id<expected.directed+expected.random)return 1;
  return id<expected.directed+expected.random+expected.conversion?3:8;
}
function runBatch(fixture,first,count,poison) {
  let pointer;
  try {pointer=invoke(fixture,'run',first,count,poison)>>>0;}
  catch(error) {
    const live=readWords(fixture,fixture.info.status,12);
    receipt.runtimeFailure={module:fixture.name,first,count,poison,status:live,
      inferredCase:first+live[9],error:errorInfo(error)};
    throw error;
  }
  const status=readWords(fixture,fixture.info.status,12);
  assert.equal(pointer,fixture.info.records,'Probe returned a different trace buffer');
  assert.deepEqual(status.slice(0,3),[1,first,count]);
  assert.equal(status[4],0,'Probe fixture failure: '+JSON.stringify(status));
  assert.equal(status[5],0xffffffff);
  assert.deepEqual(status.slice(6,10),[0,0,0,count]);
  assert.equal(status[10],poison>>>0);assert.equal(status[11],expected.cases);
  let records=0;for(let id=first;id<first+count;++id)records+=recordsForCase(id);
  assert.equal(status[3],records,'Missing or duplicated result records');
  return copyBytes(fixture,pointer,records*recordBytes);
}
function describe(fixture,id) {
  const words=readWords(fixture,invoke(fixture,'describe',id)>>>0,32);
  assert.equal(words[0],1);assert.equal(words[1],id);
  return {case:id,group:words[2],scenario:words[3],operation:words[4],
    mode:words[5],initialFlags:words[6],initialErrno:words[7]|0,
    a:words.slice(8,12),b:words.slice(12,16),c:words.slice(16,20),
    conversionInputBits:words.slice(20,22),expectedRecords:words[22]};
}
const comparisons=Object.fromEntries(['equivalence','equivalenceRepeat','baselineRepeat','candidateRepeat']
  .map(name=>[name,{records:0,metadataMismatches:0,stateMismatches:0,validValueMismatches:0,
    fullResultMismatches:0,reservedOnlyMismatches:0,firstMismatches:[]}]));
receipt.comparisons=comparisons;
const diagnosticTraceNames=['reference-primary','candidate-primary','candidate-repeat','reference-repeat'];
const traceFiles=Object.fromEntries(diagnosticTraceNames.map(name=>[name,resolve(out,name+'.bin')]));
const traceDigests=Object.fromEntries(diagnosticTraceNames.map(name=>[name,createHash('sha256')]));
const validDigests=Object.fromEntries(diagnosticTraceNames.map(name=>[name,createHash('sha256')]));
const modesSeen=new Set(),flagsSeen=new Set(),errorsSeen=new Set(),opsSeen=new Set(),eventsSeen=new Set();
const coverage={cases:0,records:0,newFlagCounts:{1:0,2:0,4:0,8:0,16:0},
  positiveZero:0,negativeZero:0,extendedSubnormal:0,nan:0,infinity:0};
function valueBytes(event) {return event<=2?10:event===3?4:8;}
function persistTrace(name,bytes) {
  appendFileSync(traceFiles[name],bytes);traceDigests[name].update(bytes);
  for(let offset=0;offset<bytes.length;offset+=recordBytes) {
    const event=bytes.readUInt32LE(offset+8);
    validDigests[name].update(bytes.subarray(offset,offset+48+valueBytes(event)));
  }
}
function checkTrace(bytes,first,count,fixture) {
  let expectedCase=first,expectedStep=0,previousFlags=0;
  for(let offset=0;offset<bytes.length;offset+=recordBytes) {
    const w=index=>bytes.readUInt32LE(offset+index*4);
    const id=w(0),step=w(1),event=w(2),op=w(3),mode=w(4),initialFlags=w(5),error=w(6)|0;
    assert.equal(id,expectedCase);assert.equal(step,expectedStep);
    if(step===0) {++coverage.cases;previousFlags=initialFlags;}
    const size=recordsForCase(id),extended=event<=2;
    const wantedEvent=size===1?0:size===8?(step<6?0:step===6?3:4):
      step===0?(((id-expected.directed-expected.random)%96)<48?1:2):step===1?3:4;
    assert.equal(event,wantedEvent,'Unexpected result event kind');
    assert(mode<24&&initialFlags<32&&w(7)<32);
    assert.equal(w(8),[32,64,80][Math.floor(mode/8)],'Arithmetic changed precision');
    assert.equal(w(9),Math.floor(mode/2)%4,'Arithmetic changed rounding');
    assert.equal(w(10),mode%2,'Arithmetic changed tininess');
    assert.equal(w(11)|0,error,'Arithmetic changed errno');
    assert.equal(w(7)&previousFlags,previousFlags,'Sticky exception flags were cleared');
    const added=w(7)&~previousFlags;
    for(const bit of [1,2,4,8,16])if(added&bit)++coverage.newFlagCounts[bit];
    previousFlags=w(7);
    if(event===0) {assert(op<4);opsSeen.add(op);}
    else assert.equal(op,0xffffffff);
    modesSeen.add(mode);flagsSeen.add(initialFlags);errorsSeen.add(error);eventsSeen.add(event);
    if(extended) {
      const significand=bytes.readBigUInt64LE(offset+48),exponent=bytes.readUInt16LE(offset+56);
      if((exponent&0x7fff)===0) {
        if(significand===0n)++coverage[exponent&0x8000?'negativeZero':'positiveZero'];
        else ++coverage.extendedSubnormal;
      } else if((exponent&0x7fff)===0x7fff) {
        if(significand&0x7fffffffffffffffn)++coverage.nan;
        else ++coverage.infinity;
      }
    } else {
      assert(bytes.subarray(offset+48+valueBytes(event),offset+64).every(value=>value===0),
             'Scalar trace tail is not the explicit probe zero-fill');
    }
    ++coverage.records;
    if(++expectedStep===size) {++expectedCase;expectedStep=0;}
  }
  assert.equal(expectedCase,first+count);assert.equal(expectedStep,0);
  assert([0,fixture.info.edom,fixture.info.erange].every(value=>Number.isInteger(value)));
}
function compare(name,a,b,fixture) {
  assert.equal(a.length,b.length,'Trace length mismatch: '+name);
  const result=comparisons[name];
  for(let offset=0;offset<a.length;offset+=recordBytes) {
    ++result.records;
    const event=a.readUInt32LE(offset+8),length=valueBytes(event);
    const meta=!a.subarray(offset,offset+28).equals(b.subarray(offset,offset+28));
    const state=!a.subarray(offset+28,offset+48).equals(b.subarray(offset+28,offset+48));
    const valid=!a.subarray(offset+48,offset+48+length).equals(b.subarray(offset+48,offset+48+length));
    const full=!a.subarray(offset+48,offset+64).equals(b.subarray(offset+48,offset+64));
    if(meta)++result.metadataMismatches;
    if(state)++result.stateMismatches;
    if(valid)++result.validValueMismatches;
    if(full)++result.fullResultMismatches;
    if(full&&!valid&&event<=2)++result.reservedOnlyMismatches;
    if((meta||state||valid||full)&&result.firstMismatches.length<12) {
      const id=a.readUInt32LE(offset);
      result.firstMismatches.push({
        case:describe(fixture,id),step:a.readUInt32LE(offset+4),event,
        metadataMismatch:meta,stateMismatch:state,validValueMismatch:valid,full16Mismatch:full,
        expectedRecord:a.subarray(offset,offset+recordBytes).toString('hex'),
        actualRecord:b.subarray(offset,offset+recordBytes).toString('hex'),
      });
    }
  }
}
function comparisonPassed(result) {
  return result.metadataMismatches===0&&result.stateMismatches===0&&
    result.validValueMismatches===0&&result.fullResultMismatches===0;
}
function summarizeCoverage() {
  return {...coverage,modes:[...modesSeen].sort((a,b)=>a-b),
    initialFlags:[...flagsSeen].sort((a,b)=>a-b),initialErrnos:[...errorsSeen].sort((a,b)=>a-b),
    operators:[...opsSeen].sort(),events:[...eventsSeen].sort()};
}
async function runOracle() {
  for(const path of Object.values(traceFiles))writeFileSync(path,'');
  const reference=await instantiate(variants[0]),candidateFixture=await instantiate(variants[1]);
  assert.notEqual(reference.memory,candidateFixture.memory,'The variants share linear memory');
  assert.notEqual(reference.memory.buffer,candidateFixture.memory.buffer);
  assert.deepEqual(reference.info.raw.slice(0,19),candidateFixture.info.raw.slice(0,19));
  assert.deepEqual(reference.layout,candidateFixture.layout);
  receipt.actualLayout={reference:reference.info.raw,candidate:candidateFixture.info.raw,
    controls:reference.layout,separateMemories:true};
  saveReceipt();
  const started=Date.now();let batches=0;
  for(let first=0;first<expected.cases;first+=256) {
    const count=Math.min(256,expected.cases-first);
    const poisonA=0x13572468,poisonB=0xeca8db97;
    // A/B/B/A: both modules repeat the same cases with a different legal stack
    // history. The full16 repeat gates cannot be waived as "only padding".
    const refA=runBatch(reference,first,count,poisonA);
    const candA=runBatch(candidateFixture,first,count,poisonA);
    const candB=runBatch(candidateFixture,first,count,poisonB);
    const refB=runBatch(reference,first,count,poisonB);
    persistTrace('reference-primary',refA);persistTrace('candidate-primary',candA);
    persistTrace('candidate-repeat',candB);persistTrace('reference-repeat',refB);
    checkTrace(refA,first,count,reference);
    compare('equivalence',refA,candA,reference);
    compare('equivalenceRepeat',refB,candB,reference);
    compare('baselineRepeat',refA,refB,reference);
    compare('candidateRepeat',candA,candB,candidateFixture);
    ++batches;
    if(batches%64===0||first+count===expected.cases) {
      receipt.coverage=summarizeCoverage();receipt.progress={cases:first+count,batches};
      saveReceipt();
      console.log('ORACLE '+(first+count)+' / '+expected.cases+' cases; full16 mismatches '+
        Object.values(comparisons).reduce((sum,item)=>sum+item.fullResultMismatches,0));
      await new Promise(done=>setTimeout(done,0));
    }
  }
  receipt.coverage=summarizeCoverage();
  assert.equal(coverage.cases,expected.cases);assert.equal(coverage.records,expected.records);
  assert.deepEqual(receipt.coverage.modes,Array.from({length:24},(_,index)=>index));
  assert.deepEqual(receipt.coverage.initialFlags,Array.from({length:32},(_,index)=>index));
  assert.deepEqual(receipt.coverage.initialErrnos,[0,reference.info.edom,reference.info.erange].sort((a,b)=>a-b));
  assert.deepEqual(receipt.coverage.operators,[0,1,2,3]);assert.deepEqual(receipt.coverage.events,[0,1,2,3,4]);
  assert(Object.values(coverage.newFlagCounts).every(count=>count>0),'An exception bit was never newly raised');
  for(const key of ['positiveZero','negativeZero','extendedSubnormal','nan','infinity'])
    assert(coverage[key]>0,'Missing output class coverage: '+key);
  for(const result of Object.values(comparisons))assert.equal(result.records,expected.records);
  receipt.traces=Object.fromEntries(diagnosticTraceNames.map(name=>[name,{
    path:traceFiles[name],bytes:expected.records*recordBytes,
    fullRecordSha256:traceDigests[name].digest('hex'),
    validValueAndStateDiagnosticSha256:validDigests[name].digest('hex'),
  }]));
  receipt.diagnosticElapsedMs=Date.now()-started;
  receipt.numericAndStatePassed=Object.values(comparisons).every(item=>
    item.metadataMismatches===0&&item.stateMismatches===0&&item.validValueMismatches===0);
  receipt.full16RepresentationPassed=Object.values(comparisons).every(item=>item.fullResultMismatches===0);
  receipt.baselineStable=comparisonPassed(comparisons.baselineRepeat);
  receipt.candidateStable=comparisonPassed(comparisons.candidateRepeat);
  receipt.correctnessPassed=Object.values(comparisons).every(comparisonPassed);
  if(!receipt.baselineStable)receipt.status='FAIL_BASELINE_STABILITY';
  else if(!receipt.candidateStable)receipt.status='FAIL_CANDIDATE_STABILITY';
  else if(!receipt.correctnessPassed)receipt.status='FAIL_EQUIVALENCE';
  else receipt.status=receipt.compilerEvidence.wasmCodeSectionChanged?
    'PASS_CORRECTNESS_CODE_REVIEW_REQUIRED':'PASS_CORRECTNESS_NO_CODE_CHANGE';
}
try {
  for(const variant of variants) {
    write(variant.arithmetic,variant.name==='reference'?pinned:candidate);
    for(const [name,bytes] of dependencyBytes)if(name.endsWith('.hpp'))
      write(resolve(variant.tree,name),bytes);
  }
  write(resolve(out,'experiment-source.json'),JSON.stringify(receipt.experimentCandidate,null,2)+'\n');
  assertSourcesUnchanged();
  for(const command of commands)await runCommand(command);
  receipt.compilerEvidence=compilerEvidence();
  assertSourcesUnchanged();saveReceipt();
  if(options.noRun)receipt.status='BUILT_NOT_RUN';
  else await runOracle();
  assertSourcesUnchanged();
  // Verify shared object files and all experiment sources still match the
  // exact bytes whose provenance was recorded before accepting the result.
  for(const variant of variants) {
    assert(readFileSync(variant.arithmetic).equals(variant.name==='reference'?pinned:candidate));
    for(const [name,bytes] of dependencyBytes)if(name.endsWith('.hpp'))
      assert(readFileSync(resolve(variant.tree,name)).equals(bytes),'Generated header drift: '+name);
  }
  for(const module of receipt.modules) {
    assert.equal(sha(readFileSync(module.wasm)),module.sha256,'WASM changed during execution');
    assert.equal(sha(readFileSync(module.loader)),module.loaderSha256,'Loader changed during execution');
    assert.equal(sha(readFileSync(module.ir)),module.irSha256,'IR evidence changed during execution');
    for(const object of module.linkedObjects)assert.equal(sha(readFileSync(object.path)),object.sha256);
  }
  receipt.finishedAt=new Date().toISOString();saveReceipt();
  console.log(JSON.stringify({
    status:receipt.status,receipt:output,correctnessPassed:receipt.correctnessPassed,
    productionReady:false,recordsPerPass:receipt.coverage?.records??0,
    baselineStable:receipt.baselineStable??null,full16Passed:receipt.full16RepresentationPassed??null,
    wasmCodeSectionChanged:receipt.compilerEvidence.wasmCodeSectionChanged,
    removedBinaryDispatchWrappers:receipt.compilerEvidence.removedBinaryDispatchWrappers,
  },null,2));
  if(!options.noRun&&!receipt.correctnessPassed)process.exitCode=1;
} catch(error) {
  const activeBuild=receipt.builds.find(item=>item.status==='RUNNING');
  if(activeBuild) {activeBuild.status='FAIL';activeBuild.error=errorInfo(error);}
  receipt.status='FAIL';receipt.error=errorInfo(error);receipt.finishedAt=new Date().toISOString();
  receipt.coverage=summarizeCoverage();receipt.correctnessPassed=false;saveReceipt();
  console.error(JSON.stringify({status:receipt.status,receipt:output,error:receipt.error},null,2));
  process.exitCode=1;
}
