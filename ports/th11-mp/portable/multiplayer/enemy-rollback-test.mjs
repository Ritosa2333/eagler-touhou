// Root runs this dedicated real Enemy/ECL component check serially.
import {spawn} from 'node:child_process';
import {readFileSync,readdirSync,existsSync,mkdirSync,writeFileSync,openSync,closeSync} from 'node:fs';
import {resolve,relative} from 'node:path';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'../..');
const sdk=process.env.WASI_SDK_ROOT||'D:/workspace/eagler/toolchains/wasi-sdk-34.0-x86_64-windows';
const compiler=resolve(sdk,'bin/clang++.exe');
const outArg=process.argv.find(s=>s.startsWith('--out='));
const out=resolve(root,outArg?outArg.slice(6):'artifacts/multiplayer-enemy-rollback/o2-01');
if(existsSync(resolve(out,'receipt.json')))throw Error('Output receipt exists; choose a new --out= directory');
mkdirSync(out,{recursive:true});
const common=resolve(root,'../../common'),game=resolve(root,'web/cpp/game');
const names=['EnemyManager','EnemyRollback','EnemyScript','EnemyFrame','EnemyState','EnemyPhase','EnemyAnimations',
 'EnemyCommands','EnemyStateCommands','EnemyEmitterCommands','EnemyLaserCommands',
 'EclContext','EclInterpreter','EclOwner','EclResource',
 'BulletManager','BulletAppearance','BulletLaunch','BulletMotion','BulletTransforms',
 'LaserManager','LaserState','LaserLine','LaserInfinite','LaserCut','LaserInfiniteCut',
 'AnmVm','AnmInterpreter','AnmGeometry','AnmProjection','AnmRenderer','AnmManager','AnmRollback',
 'Movement','Interpolation','GraphicsMath','SceneCamera','SpellController','GameEconomy',
 'AsciiText','ScreenShake','StageScript','StageResource','FogInterpolation'];
const sources=[...names.map(n=>resolve(game,n+'.cpp')),resolve(common,'src/netplay/RollbackJournal.cpp'),resolve(import.meta.dirname,'enemy-rollback-test.cpp')];
const walk=dir=>readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?walk(resolve(dir,e.name)):[resolve(dir,e.name)]);
const headers=[...walk(game),...walk(resolve(root,'portable/sdl')),...walk(resolve(common,'include'))].filter(p=>/\.(hpp|h|inc)$/.test(p));
const inputs=[...new Set([...sources,...headers,resolve(import.meta.filename)])].sort();
const hash=b=>createHash('sha256').update(b).digest('hex');
const manifest=()=>inputs.map(path=>({path:relative(root,path).replaceAll('\\','/'),sha256:hash(readFileSync(path))}));
const before=manifest();
const baseFlags=['--target=wasm32-wasip1','-std=c++17','-O2','-g0','-ffp-contract=off','-fno-strict-aliasing','-fno-exceptions','-fno-rtti','-I'+resolve(common,'include')];
let log='',commands=[];const started=new Date().toISOString();
const run=args=>new Promise((accept,reject)=>{
 commands.push({program:compiler,args});const child=spawn(compiler,args,{cwd:root,windowsHide:true,stdio:['ignore','pipe','pipe']});
 child.stdout.on('data',b=>{log+=b;process.stdout.write(b);});child.stderr.on('data',b=>{log+=b;process.stderr.write(b);});
 child.on('error',reject);child.on('exit',code=>code?reject(Error('Compiler exited '+code)):accept());
});
const receipt={schema:'th11-enemy-rollback-component/1',started,inputs:before,commands,variants:[],
 scope:'Real EnemyManager/EnemyScriptServices/ECL interpreter; synthetic decoded ECL and empty real ANM owner; fixture explicitly captures external environment/commands/target fields. Excludes complete world, deformation, renderer, network and performance acceptance.'};
let exitCode=1;
try{
 for(const mode of ['ordinary','multiplayer']){
  const flags=[...baseFlags,...(mode==='multiplayer'?['-DTH11_MULTIPLAYER=1']:[])];
  const wasm=resolve(out,'enemy-rollback-'+mode+'.wasm');
  await run([...flags,...sources,'-Wl,--error-limit=0','-Wl,-z,stack-size=2097152','-Wl,--max-memory=536870912','-o',wasm]);
  const stdout=resolve(out,mode+'.stdout.log'),stderr=resolve(out,mode+'.stderr.log');
  const outFd=openSync(stdout,'w'),errFd=openSync(stderr,'w');let code;
  try{
   const wasi=new WASI({version:'preview1',args:['enemy-rollback-test'],returnOnExit:true,stdout:outFd,stderr:errFd});
   const instance=await WebAssembly.instantiate(await WebAssembly.compile(readFileSync(wasm)),{wasi_snapshot_preview1:wasi.wasiImport});
   code=wasi.start(instance);
  }finally{closeSync(outFd);closeSync(errFd);}
  const output=readFileSync(stdout,'utf8'),errorOutput=readFileSync(stderr,'utf8');
  process.stdout.write(mode+': '+output);if(errorOutput)process.stderr.write(errorOutput);
  receipt.variants.push({mode,flags,wasm:{sha256:hash(readFileSync(wasm)),bytes:readFileSync(wasm).length},exitCode:code,stdout:output,stderr:errorOutput});
  if(code)throw Error(mode+' component test exited '+code);
 }
 receipt.sourcesUnchanged=JSON.stringify(before)===JSON.stringify(manifest());
 if(!receipt.sourcesUnchanged)throw Error('Source inputs changed while compiling/testing');
 receipt.ok=true;exitCode=0;
}catch(error){receipt.ok=false;receipt.error=String(error?.stack||error);process.stderr.write(receipt.error+'\n');}
finally{
 receipt.finished=new Date().toISOString();receipt.exitCode=exitCode;
 if(receipt.sourcesUnchanged===undefined)receipt.sourcesUnchanged=JSON.stringify(before)===JSON.stringify(manifest());
 writeFileSync(resolve(out,'compiler.log'),log);writeFileSync(resolve(out,'receipt.json'),JSON.stringify(receipt,null,2)+'\n');
}
process.exitCode=exitCode;
