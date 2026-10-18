// Standalone O2 / WASI32 component gate. Root serializes execution.
// Real exported font fixtures are required; no SDL/GL, browser, device or Runtime executes.
import {spawn} from 'node:child_process';
import {readFileSync,writeFileSync,mkdirSync,readdirSync,existsSync} from 'node:fs';
import {resolve,relative} from 'node:path';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';

const root=resolve(import.meta.dirname,'../..');
const option=(name,fallback)=>{const i=process.argv.indexOf(name);if(i<0)return fallback;if(!process.argv[i+1]||process.argv[i+1].startsWith('--'))throw Error('Missing '+name+' value');return process.argv[i+1];};
const fontFiles=['font0.bin','font1.bin','font2.bin','font3.bin','cp932.bin','blend4444.bin'];
const fontDirectory=resolve(option('--font-dir','D:/workspace/eagler/th11-eagler/build-eagler/fonts'));
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
if(process.argv.includes('--run-wasm')){
  const file=resolve(option('--run-wasm')),lane=option('--lane','unknown');
  const wasi=new WASI({version:'preview1',args:['texture-rollback-test',lane],returnOnExit:true,preopens:{'/fonts':fontDirectory}});
  const module=await WebAssembly.compile(readFileSync(file));
  const instance=await WebAssembly.instantiate(module,{wasi_snapshot_preview1:wasi.wasiImport});
  process.exitCode=wasi.start(instance);
}else{
  const lane=option('--lane','all');
  if(!['all','ordinary','multiplayer'].includes(lane))throw Error('--lane must be all, ordinary or multiplayer');
  const sdk=resolve(process.env.WASI_SDK_ROOT||'D:/workspace/eagler/toolchains/wasi-sdk-34.0-x86_64-windows');
  const compiler=resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++');
  const stamp=new Date().toISOString().replaceAll(':','-').replaceAll('.','-');
  const out=resolve(option('--output',resolve(root,'artifacts/rollback-smoothness/texture-component-'+stamp)));
  // Never overwrite an earlier receipt, including a failed run.
  if(existsSync(resolve(out,'receipt.json')))throw Error('Existing receipt; choose a fresh --output directory');
  mkdirSync(out,{recursive:true});
  const receipt={
    schema:'th11-texture-rollback-component/1',startedAt:new Date().toISOString(),completedAt:null,
    passed:false,root,output:out,requestedLane:lane,
    scope:{realTexturePixels:true,realTextRaster:true,realCommonJournal:true,wasiBits:32,optimization:'O2',
      fullWorld:false,browser:false,performance:false,retailGameData:false,originalFontFixtures:true,
      graphicsDeviceCompiled:false,fontDeviceCompiled:false,sdlGpuUploads:false,localizedTtf:false},
    runtime:{node:process.execPath,nodeVersion:process.version,platform:process.platform,arch:process.arch},
    sourceFiles:{},sourceDigest:null,sourceStable:false,assets:{directory:fontDirectory,files:{},digest:null,stable:false},toolchain:{sdk,compiler},identity:{},commands:[],lanes:[],
  };
  const save=()=>writeFileSync(resolve(out,'receipt.json'),JSON.stringify(receipt,null,2)+'\n');
  save();
  const observed=new Map(),assets=new Map();
  function observe(path){
    const digest=hash(readFileSync(path));
    if(observed.has(path)&&observed.get(path)!==digest)throw Error('Source changed while observing: '+path);
    observed.set(path,digest);
  }
  function checkSources(){
    for(const [path,expected] of assets)if(hash(readFileSync(path))!==expected)throw Error('Font fixture changed during component validation: '+path);
    for(const [path,expected] of observed)if(hash(readFileSync(path))!==expected)
      throw Error('Source changed during component validation: '+path);
  }
  async function run(label,program,args,{required=true}={}){
    const command={label,program,args,cwd:root,startedAt:new Date().toISOString(),completedAt:null,
      exitCode:null,signal:null,stdoutFile:label+'.stdout.log',stderrFile:label+'.stderr.log'};
    receipt.commands.push(command);save();
    let stdout='',stderr='';
    try{
      await new Promise((accept,reject)=>{
        const child=spawn(program,args,{cwd:root,windowsHide:true,stdio:['ignore','pipe','pipe']});
        child.stdout.on('data',b=>{stdout+=b.toString();if(stdout.length>8*1024*1024){child.kill();reject(Error(label+' exceeded bounded stdout'));}});
        child.stderr.on('data',b=>{stderr+=b.toString();if(stderr.length>8*1024*1024){child.kill();reject(Error(label+' exceeded bounded stderr'));}});
        child.once('error',reject);
        child.once('exit',(code,signal)=>{command.exitCode=code;command.signal=signal;accept();});
      });
    }catch(error){command.error=String(error);}
    finally{
      command.completedAt=new Date().toISOString();
      writeFileSync(resolve(out,command.stdoutFile),stdout);
      writeFileSync(resolve(out,command.stderrFile),stderr);save();
    }
    if(required&&(command.error||command.exitCode!==0))throw Error(label+' failed: '+(command.error||'exit '+command.exitCode)+'; see '+out);
    return {stdout,stderr,...command};
  }
  try{
    const game=resolve(root,'th11_web/cpp/game'),common=resolve(root,'third_party/eagler-common');
    const names=['TextRaster.cpp','GlyphAtlas.cpp','ImageResample.cpp','TextureImage.cpp'];
    const sources=names.map(name=>resolve(game,name)).concat(resolve(root,'th11_web/cpp/sdl/TextureRollback.cpp'),resolve(import.meta.dirname,'texture-rollback.cpp'));
    // These hooks are source-identity evidence only: this CPU gate does not
    // compile SDL GraphicsDevice/FontDevice or prove a real GPU re-upload.
    const hooks=['GraphicsDevice.hpp','GraphicsDevice.cpp','FontDevice.hpp','FontDevice.cpp','TextureRollback.hpp'].map(name=>resolve(root,'th11_web/cpp/sdl',name));
    for(const name of fontFiles){const path=resolve(fontDirectory,name);assets.set(path,hash(readFileSync(path)));receipt.assets.files[name]={sha256:assets.get(path),bytes:readFileSync(path).length};}
    receipt.assets.digest=hash(JSON.stringify(receipt.assets.files));
    const walk=dir=>readdirSync(dir,{withFileTypes:true}).flatMap(entry=>entry.isDirectory()?walk(resolve(dir,entry.name)):[resolve(dir,entry.name)]);
    const headers=[...walk(game),...walk(resolve(root,'portable/sdl'))].filter(path=>/\.(h|hpp|inc)$/.test(path));
    for(const path of [...sources,...headers,...hooks,resolve(import.meta.filename)])observe(path);
    if(lane!=='ordinary'){
      for(const path of walk(resolve(common,'include')).filter(path=>/\.(h|hpp|inc)$/.test(path)))observe(path);
      observe(resolve(common,'src/netplay/RollbackJournal.cpp'));
    }
    receipt.sourceFiles=Object.fromEntries([...observed].sort(([a],[b])=>a.localeCompare(b))
      .map(([path,digest])=>[relative(root,path).replaceAll('\\','/'),digest]));
    receipt.sourceDigest=hash(JSON.stringify(receipt.sourceFiles));
    receipt.toolchain.compilerSha256=hash(readFileSync(compiler));
    const compilerConfig=resolve(sdk,'bin/clang++.cfg');
    if(existsSync(compilerConfig))receipt.toolchain.compilerConfigSha256=hash(readFileSync(compilerConfig));
    receipt.toolchain.version=(await run('compiler-version',compiler,['--version'])).stdout.trim();
    for(const [name,path] of [['game',root],['common',common]]){
      receipt.identity[name]={path,
        head:(await run(name+'-head','git',['-C',path,'rev-parse','HEAD'],{required:false})).stdout.trim(),
        status:(await run(name+'-status','git',['-C',path,'status','--porcelain=v1','--untracked-files=normal'],{required:false})).stdout,
      };
    }
    const flags=['--target=wasm32-wasip1','-std=c++17','-O2','-g0','-UNDEBUG','-ffp-contract=off',
      '-fno-strict-aliasing','-fno-exceptions','-fno-rtti','-DTH11_DEVELOPMENT_HARNESS=0'];
    for(const variant of lane==='all'?['ordinary','multiplayer']:[lane]){
      checkSources();
      const wasm=resolve(out,'texture-rollback-test-'+variant+'.wasm');
      const extra=variant==='multiplayer'?['-DTH11_MULTIPLAYER=1','-I'+resolve(common,'include')]:[];
      const compileSources=variant==='multiplayer'?sources.concat(resolve(common,'src/netplay/RollbackJournal.cpp')):sources;
      const args=[...flags,...extra,...compileSources,'-Wl,-z,stack-size=2097152','-Wl,--max-memory=268435456','-o',wasm];
      await run(variant+'-compile',compiler,args);
      checkSources();
      const wasmBytes=readFileSync(wasm),result={lane:variant,wasm,sha256:hash(wasmBytes),bytes:wasmBytes.length,passed:false};
      receipt.lanes.push(result);save();
      const execution=await run(variant+'-execute',process.execPath,[resolve(import.meta.filename),'--run-wasm',wasm,'--lane',variant,'--font-dir',fontDirectory]);
      const expected=variant==='multiplayer'?/PASS texture rollback multiplayer: (\d+) groups, (\d+) checks/:/PASS texture ordinary: (\d+) groups, (\d+) checks/;
      const match=execution.stdout.match(expected);
      if(!match)throw Error('Missing '+variant+' component completion marker');
      result.groups=Number(match[1]);result.checks=Number(match[2]);result.passed=true;save();
      process.stdout.write(execution.stdout);
    }
    checkSources();receipt.sourceStable=true;receipt.assets.stable=true;
    receipt.passed=receipt.lanes.length===(lane==='all'?2:1)&&receipt.lanes.every(item=>item.passed);
  }catch(error){receipt.error=error?.stack||String(error);process.exitCode=1;console.error(String(error));}
  finally{receipt.completedAt=new Date().toISOString();save();console.log('texture component receipt: '+resolve(out,'receipt.json'));}
}
