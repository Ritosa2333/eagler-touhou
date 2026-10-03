import {execFileSync} from 'node:child_process';
import {existsSync,mkdirSync,readFileSync,writeFileSync,openSync,closeSync,rmSync} from 'node:fs';
import {resolve,relative,dirname,basename,isAbsolute} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {cpus} from 'node:os';
import {WASI} from 'node:wasi';
// Optional native bullet-owner diagnostic. Nothing is extracted or embedded in WASM.
// CPU-only fixture timings are informational, never pass/fail performance thresholds.
// Usage: WASI_SDK_PATH=... TH10_DATA_PATH=/path/to/th10.dat node portable/check-bullet-owner.mjs
// Optional TH10_TEST_UBSAN=1 recompiles with trapping undefined-behavior checks.
const root=resolve(dirname(fileURLToPath(import.meta.url)),'..');
const sdk=process.env.WASI_SDK_PATH,input=process.env.TH10_DATA_PATH;
if(!sdk||!input)throw Error('Set WASI_SDK_PATH and TH10_DATA_PATH to locally supplied tools/data.');
if(basename(input)!=='th10.dat')throw Error('TH10_DATA_PATH must name th10.dat.');
const compiler=resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++');
if(!existsSync(compiler)||!existsSync(input))throw Error('Compiler or supplied data not found.');
const sanitized=process.env.TH10_TEST_UBSAN==='1';
const out=resolve(root,'artifacts/bullet-owner-check'+(sanitized?'-ubsan':''));mkdirSync(out,{recursive:true});
const metadataPath=resolve(out,'build.json');rmSync(metadataPath,{force:true});
const sha256=data=>createHash('sha256').update(data).digest('hex');
const inputBytes=readFileSync(input),inputHash=sha256(inputBytes);
const common=resolve(root,'third_party/eagler-common');
const flags=['--target=wasm32-wasip1','-O2','-std=c++17','-Wall','-Wextra','-Werror','-DTH_NATIVE_PLATFORM=1','-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1','-fno-exceptions','-fno-rtti','-ffp-contract=off','-fno-strict-aliasing','-I'+resolve(common,'include')];
if(sanitized)flags.push('-fsanitize=undefined','-fsanitize-trap=undefined');
const names=['ResourceArchive','ArchiveLifecycle','ResourceCodec','ResourceFiles','AnmResources','AnmTexture','TexturePlatform','AnmFile','AnmScript','AnmVm','AnmInterpreter','AnmVariables','AnmRenderer','BulletEmitter','BulletCommands','BulletFeatures','BulletFrame','Arithmetic','GameMath','Timer','Interpolation','Rng'];
const sources=['tests/bullet-owner-check.cpp','third_party/eagler-common/src/netplay/RollbackJournal.cpp','th10_web/cpp/platform/FileSystem.cpp',...names.map(n=>`th10_web/cpp/game/${n}.cpp`)];
const softfloatFlags=['--target=wasm32-wasip1','-O2','-x','c','-std=c11','-DSOFTFLOAT_FAST_INT64','-DINLINE_LEVEL=5'];
const linkFlags=['--target=wasm32-wasip1','-Wl,-z,stack-size=1048576'];
const dependencies=new Set([resolve(root,'portable/check-bullet-owner.mjs')]);
function compile(source,compileFlags=flags){
  const object=resolve(out,source.replace(/[^A-Za-z0-9_.-]/g,'_')+'.o'),depfile=object+'.d';
  execFileSync(compiler,[...compileFlags,'-MMD','-MF',depfile,'-MT','source','-c',resolve(root,source),'-o',object],{cwd:root,stdio:'inherit'});
  // SDK system headers and libraries are represented by the toolchain version.
  const make=readFileSync(depfile,'utf8').replace(/\\\r?\n/g,'').replace(/^source:\s*/,'');
  for(const token of make.match(/(?:\\.|[^\s])+/g)??[]){
    const path=resolve(root,token.replace(/\\(.)/g,'$1')),name=relative(root,path);
    if(name.startsWith('..')||isAbsolute(name))throw Error(`Project dependency outside checkout: ${path}`);
    dependencies.add(path);
  }
  return object;
}
const softfloat=compile('th10_web/cpp/rebuild/third_party/softfloat.c',softfloatFlags);
const wasm=resolve(out,'bullet-owner-check.wasm');
execFileSync(compiler,[...linkFlags,...sources.map(source=>compile(source)),softfloat,'-o',wasm],{cwd:root,stdio:'inherit'});
// The guest host implementation permits only read-only opens of th10.dat.
const log=resolve(out,'results.jsonl'),stdout=openSync(log,'w',0o600);
try{
  const wasi=new WASI({version:'preview1',args:['bullet-owner-check'],env:{},preopens:{'/input':dirname(resolve(input))},stdout,returnOnExit:true});
  const {instance}=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});
  const code=wasi.start(instance);if(code!==0)throw Error(`Resource diagnostic failed: ${code}`);
}finally{closeSync(stdout);process.stdout.write(readFileSync(log,'utf8'));}
const results=readFileSync(log,'utf8').trim().split('\n').map(line=>JSON.parse(line));
if(!results.some(row=>row.suite==='result'&&row.passed))throw Error('Missing successful test result.');
if(sha256(readFileSync(input))!==inputHash)throw Error('Input data changed during the test.');
writeFileSync(metadataPath,JSON.stringify({passed:true,runtime:'wasm32-wasip1',node:process.version,platform:process.platform,arch:process.arch,cpu:cpus()[0]?.model,
  compiler:execFileSync(compiler,['--version'],{encoding:'utf8'}).trim(),compileFlags:flags,softfloatFlags,linkFlags,sanitized,
  dependencyScope:'Clang -MMD non-system source/header closure plus this driver; SDK system headers/libraries excluded',
  input:{name:basename(input),bytes:inputBytes.length,sha256:inputHash,unchanged:true},
  sources:Object.fromEntries([...dependencies].sort().map(path=>[relative(root,path).replaceAll('\\','/'),sha256(readFileSync(path))])),
  fixtureBuild:sanitized?'optimized trapping-UBSAN fixture':'optimized unsanitized fixture',
  native_bullet_owner:true,gpu:false,browser:false,mobile:false,full_game_fps:false},null,2)+'\n');
