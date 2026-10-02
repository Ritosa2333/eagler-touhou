import {execFileSync} from 'node:child_process';
import {existsSync, mkdirSync, readFileSync, writeFileSync} from 'node:fs';
import {resolve, relative, dirname, isAbsolute} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';

// Always compile current sources. No SDL/Emscripten objects or retail assets.
// Usage: WASI_SDK_PATH=/path/to/wasi-sdk node portable/check-anm-dense-engine.mjs
// Pass --reverse to run batched captures before per-slot captures as an order control.
// Timing is informational: host noise is not a correctness regression gate.
const root=resolve(dirname(fileURLToPath(import.meta.url)), '..');
const sdk=process.env.WASI_SDK_PATH;
if(!sdk)throw new Error('Set WASI_SDK_PATH to an installed WASI SDK directory.');
const compiler=resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++');
if(!existsSync(compiler))throw new Error(`WASI compiler not found: ${compiler}`);
const out=resolve(root,'artifacts/anm-dense-engine');mkdirSync(out,{recursive:true});
const common=resolve(root,'third_party/eagler-common');
const compileFlags=['--target=wasm32-wasip1','-O2','-std=c++17','-Wall','-Wextra','-Werror',
  '-DTH_NATIVE_PLATFORM=1','-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1','-fno-exceptions','-fno-rtti',
  '-ffp-contract=off','-fno-strict-aliasing','-I'+resolve(common,'include')];
const softfloatFlags=['--target=wasm32-wasip1','-O2','-x','c','-std=c11',
  '-DSOFTFLOAT_FAST_INT64','-DINLINE_LEVEL=5'];
const linkFlags=['--target=wasm32-wasip1','-Wl,-z,stack-size=1048576'];
const dependencies=new Set([resolve(root,'portable/check-anm-dense-engine.mjs')]);
const objects=new Map();
function compile(source,flags=compileFlags){
  if(objects.has(source))return objects.get(source);
  const object=resolve(out,source.replace(/[^A-Za-z0-9_.-]/g,'_')+'.o');
  const depfile=object+'.d';
  execFileSync(compiler,[...flags,'-MMD','-MF',depfile,'-MT','source','-c',resolve(root,source),'-o',object],
    {cwd:root,stdio:'inherit',windowsHide:true});
  // Clang supplies the complete non-system source/header closure. Toolchain
  // system headers/libraries are represented by the SDK version, not hashed.
  const make=readFileSync(depfile,'utf8').replace(/\\\r?\n/g,'').replace(/^source:\s*/,'');
  for(const token of make.match(/(?:\\.|[^\s])+/g)??[]){
    const path=resolve(root,token.replace(/\\(.)/g,'$1'));
    const name=relative(root,path);
    if(name.startsWith('..')||isAbsolute(name))throw new Error(`Project dependency outside checkout: ${path}`);
    dependencies.add(path);
  }
  objects.set(source,object);return object;
}
const softfloat=compile('th10_web/cpp/rebuild/third_party/softfloat.c',softfloatFlags);
const gameSources=['AnmRenderer','Arithmetic','GameMath'].map(name=>`th10_web/cpp/game/${name}.cpp`);
const engineSources=['AnmVm','AnmInterpreter','AnmVariables','AnmFile','Timer','Interpolation','Rng']
  .map(name=>`th10_web/cpp/game/${name}.cpp`);
for(const [name,sources] of [
  ['subpixel-check',['portable/subpixel-check.cpp',...gameSources]],
  ['anm-dense-engine',['tests/anm-dense-engine-benchmark.cpp',...gameSources,...engineSources,
    'third_party/eagler-common/src/netplay/RollbackJournal.cpp']],
]){
  const wasm=resolve(out,name+'.wasm');
  execFileSync(compiler,[...linkFlags,...sources.map(source=>compile(source)),softfloat,'-o',wasm],
    {cwd:root,stdio:'inherit',windowsHide:true});
  const wasi=new WASI({version:'preview1',args:[name,...(process.argv.includes('--reverse')?['--reverse']:[])],env:{},returnOnExit:true});
  const {instance}=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});
  const code=wasi.start(instance);if(code!==0)throw new Error(`${name} failed: ${code}`);
  console.log(`${name}: PASS`);
}
// Keep reproducibility metadata inside the already-ignored artifacts directory.
const metadata={node:process.version,platform:process.platform,arch:process.arch,
  captureOrder:process.argv.includes('--reverse')?'runs-first':'per-slot-first',
  compiler:execFileSync(compiler,['--version'],{encoding:'utf8'}).trim(),compileFlags,softfloatFlags,linkFlags,
  dependencyScope:'Clang -MMD non-system source/header closure plus this driver; SDK system headers/libraries excluded',
  sources:Object.fromEntries([...dependencies].sort().map(path=>[relative(root,path).replaceAll('\\','/'),
    createHash('sha256').update(readFileSync(path)).digest('hex')]))};
writeFileSync(resolve(out,'build.json'),JSON.stringify(metadata,null,2)+'\n');
