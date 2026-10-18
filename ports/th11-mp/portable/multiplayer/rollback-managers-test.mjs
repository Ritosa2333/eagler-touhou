// Dedicated real-component rollback check. Root agent runs this serially.
import {spawn} from 'node:child_process';
import {readFileSync,readdirSync,existsSync,mkdirSync,writeFileSync} from 'node:fs';
import {resolve,relative} from 'node:path';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'../..');
const sdk=process.env.WASI_SDK_ROOT||'D:/workspace/eagler/toolchains/wasi-sdk-34.0-x86_64-windows';
const compiler=resolve(sdk,'bin/clang++.exe');
const outArg=process.argv.find(s=>s.startsWith('--out='));
const out=resolve(root,outArg?outArg.slice(6):'artifacts/multiplayer-rollback-managers/o2-01');
if(existsSync(resolve(out,'receipt.json')))throw Error('Output receipt already exists; use a new --out= directory');
mkdirSync(out,{recursive:true});
const common=resolve(root,'../../common');
const game=resolve(root,'web/cpp/game');
const names=['BulletManager','ItemManager','BulletAppearance','BulletLaunch','BulletMotion','BulletTransforms','Movement','EnemyState',
    'AnmVm','AnmInterpreter','AnmGeometry','AnmProjection','AnmRenderer','AnmManager','AnmRollback','Interpolation','GraphicsMath','SceneCamera'];
const sources=[...names.map(s=>resolve(game,s+'.cpp')),resolve(common,'src/netplay/RollbackJournal.cpp'),resolve(import.meta.dirname,'rollback-managers-test.cpp')];
const walk=dir=>readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?walk(resolve(dir,e.name)):[resolve(dir,e.name)]);
const headers=[...walk(game).filter(p=>/\.(hpp|inc)$/.test(p)),...walk(resolve(common,'include')).filter(p=>/\.(hpp|h)$/.test(p))];
const inputs=[...new Set([...sources,...headers,resolve(import.meta.filename)])].sort();
const hash=buffer=>createHash('sha256').update(buffer).digest('hex');
const manifest=()=>inputs.map(path=>({path:relative(root,path).replaceAll('\\','/'),sha256:hash(readFileSync(path))}));
const before=manifest();
const baseFlags=['--target=wasm32-wasip1','-std=c++17','-O2','-g0','-ffp-contract=off','-fno-strict-aliasing','-fno-exceptions','-fno-rtti'];
const flags=[...baseFlags,'-DTH11_MULTIPLAYER=1','-I'+resolve(common,'include')];
let log='',commands=[];const started=new Date().toISOString();
const run=args=>new Promise((accept,reject)=>{
    commands.push({program:compiler,args});
    const child=spawn(compiler,args,{cwd:root,windowsHide:true,stdio:['ignore','pipe','pipe']});
    child.stdout.on('data',b=>{log+=b;process.stdout.write(b);});child.stderr.on('data',b=>{log+=b;process.stderr.write(b);});
    child.on('error',reject);child.on('exit',code=>code?reject(Error('Compiler exited '+code)):accept());
});
const receipt={schema:'th11-rollback-managers-component/1',started,flags,inputs:before,commands,scope:'Real BulletManager/ItemManager with synthetic ANM data and real interpreter/renderer/common journal. Not whole-world/network/performance acceptance.'};
let exitCode=1;
try{
    // Explicit ordinary compile regression: observer API is multiplayer-only.
    for(const name of ['BulletManager','ItemManager'])await run([...baseFlags,'-c',resolve(game,name+'.cpp'),'-o',resolve(out,name+'-ordinary.o')]);
    const wasm=resolve(out,'rollback-managers-test.wasm');
    await run([...flags,...sources,'-Wl,--error-limit=0','-Wl,-z,stack-size=2097152','-Wl,--max-memory=536870912','-o',wasm]);
    receipt.wasm={sha256:hash(readFileSync(wasm)),bytes:readFileSync(wasm).length};
    const wasi=new WASI({version:'preview1',args:['rollback-managers-test'],returnOnExit:true});
    const instance=await WebAssembly.instantiate(await WebAssembly.compile(readFileSync(wasm)),{wasi_snapshot_preview1:wasi.wasiImport});
    exitCode=wasi.start(instance);
    receipt.sourcesUnchanged=JSON.stringify(before)===JSON.stringify(manifest());
    if(!receipt.sourcesUnchanged)throw Error('Source inputs changed while compiling/testing');
    if(exitCode)throw Error('Component test exited '+exitCode);
    receipt.ok=true;
}catch(error){receipt.ok=false;receipt.error=String(error?.stack||error);exitCode=1;process.stderr.write(receipt.error+'\n');}
finally{
    receipt.finished=new Date().toISOString();receipt.exitCode=exitCode;
    if(receipt.sourcesUnchanged===undefined)receipt.sourcesUnchanged=JSON.stringify(before)===JSON.stringify(manifest());
    writeFileSync(resolve(out,'compiler.log'),log);
    writeFileSync(resolve(out,'receipt.json'),JSON.stringify(receipt,null,2)+'\n');
}
process.exitCode=exitCode;
