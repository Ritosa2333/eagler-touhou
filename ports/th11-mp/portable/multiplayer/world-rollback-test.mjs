// Full-title O2/WASI32 correctness gate. Only root schedules execution.
// Native graphics fixtures exercise authored Draw but do not emulate a GPU.
import {spawn} from 'node:child_process';
import {readFileSync,writeFileSync,mkdirSync,readdirSync,existsSync} from 'node:fs';
import {resolve,relative,basename,dirname} from 'node:path';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'../..');
const option=(name,fallback)=>{const i=process.argv.indexOf(name);if(i<0)return fallback;if(!process.argv[i+1]||process.argv[i+1].startsWith('--'))throw Error('Missing '+name+' value');return process.argv[i+1];};
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
if(process.argv.includes('--run-wasm')){
  const file=resolve(option('--run-wasm')),archive=resolve(option('--archive'));
  const wasi=new WASI({version:'preview1',args:[basename(file),'/assets/'+basename(archive)],
    returnOnExit:true,preopens:{'/assets':dirname(archive)}});
  const module=await WebAssembly.compile(readFileSync(file));
  const instance=await WebAssembly.instantiate(module,{wasi_snapshot_preview1:wasi.wasiImport});
  process.exitCode=wasi.start(instance);
}else{
  const suite=option('--suite','world');
  if(!['world','native','all'].includes(suite))throw Error('Invalid --suite');
  const sdk=resolve(process.env.WASI_SDK_ROOT||'D:/workspace/eagler/toolchains/wasi-sdk-34.0-x86_64-windows');
  const compiler=resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++');
  const archive=resolve(process.env.TH11_TEST_ARCHIVE||resolve(root,'../../th11-eagler/[th11] 东方地灵殿 (汉化版+日文版)/th11.dat'));
  const stamp=new Date().toISOString().replaceAll(':','-').replaceAll('.','-');
  const out=resolve(option('--output',resolve(root,'artifacts/rollback-smoothness/world-'+stamp)));
  if(existsSync(resolve(out,'receipt.json')))throw Error('Existing receipt; choose a fresh output directory');
  mkdirSync(out,{recursive:true});
  const receipt={schema:'th11-world-rollback-native/1',startedAt:new Date().toISOString(),
    completedAt:null,passed:false,root,output:out,suite,
    scope:{fullTitleSources:true,realRetailArchive:true,realJournal:true,authoredDraw:true,
      wasiBits:32,optimization:'O2',browser:false,actualGpu:false,hostFonts:false,performance:false},
    runtime:{node:process.execPath,nodeVersion:process.version,platform:process.platform},
    toolchain:{sdk,compiler},sourceFiles:{},sourceStable:false,commands:[],suites:[],identity:{}};
  const save=()=>writeFileSync(resolve(out,'receipt.json'),JSON.stringify(receipt,null,2)+'\n');
  save();
  const observed=new Map();
  const walk=dir=>readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?walk(resolve(dir,e.name)):[resolve(dir,e.name)]);
  const observe=path=>observed.set(path,hash(readFileSync(path)));
  const checkSources=()=>{for(const [path,digest] of observed)if(hash(readFileSync(path))!==digest)throw Error('Source changed during validation: '+path);};
  async function run(label,program,args,{required=true}={}){
    const command={label,program,args,cwd:root,startedAt:new Date().toISOString(),completedAt:null,
      exitCode:null,signal:null,stdoutFile:label+'.stdout.log',stderrFile:label+'.stderr.log'};
    receipt.commands.push(command);save();let stdout='',stderr='';
    try{
      await new Promise((accept,reject)=>{
        const child=spawn(program,args,{cwd:root,windowsHide:true,stdio:['ignore','pipe','pipe']});
        child.stdout.on('data',b=>{stdout+=b.toString();if(stdout.length>8*1024*1024){child.kill();reject(Error(label+' exceeded stdout bound'));}});
        child.stderr.on('data',b=>{stderr+=b.toString();if(stderr.length>8*1024*1024){child.kill();reject(Error(label+' exceeded stderr bound'));}});
        child.once('error',reject);
        child.once('exit',(code,signal)=>{command.exitCode=code;command.signal=signal;accept();});
      });
    }catch(error){command.error=String(error);}
    finally{
      command.completedAt=new Date().toISOString();
      writeFileSync(resolve(out,command.stdoutFile),stdout);writeFileSync(resolve(out,command.stderrFile),stderr);save();
    }
    if(required&&(command.error||command.exitCode!==0))throw Error(label+' failed: '+(command.error||'exit '+command.exitCode)+'; see '+out);
    return {stdout,stderr,...command};
  }
  try{
    if(!existsSync(archive))throw Error('TH11_TEST_ARCHIVE must name the existing supplied archive');
    receipt.archive={path:archive,bytes:readFileSync(archive).length,sha256:hash(readFileSync(archive))};
    const game=resolve(root,'web/cpp/game'),common=resolve(root,'../../common');
    const suites=suite==='all'?['world','native']:[suite];
    const entries=Object.fromEntries(suites.map(name=>[name,resolve(import.meta.dirname,name==='world'?'world-rollback.cpp':'native-gameplay.cpp')]));
    const commonSource=resolve(common,'src/netplay/RollbackJournal.cpp');
    const sources=walk(game).filter(p=>p.endsWith('.cpp')).sort().concat(commonSource);
    const headers=[...walk(game),...walk(resolve(root,'portable/sdl')),...walk(resolve(common,'include')),
      ...walk(import.meta.dirname).filter(p=>basename(p).startsWith('native-'))].filter(p=>/\.(h|hpp|inc)$/.test(p));
    for(const path of [...sources,...headers,...Object.values(entries),import.meta.filename])observe(path);
    receipt.sourceFiles=Object.fromEntries([...observed].sort(([a],[b])=>a.localeCompare(b)).map(([p,d])=>[relative(root,p).replaceAll('\\','/'),d]));
    receipt.sourceDigest=hash(JSON.stringify(receipt.sourceFiles));
    receipt.toolchain.compilerSha256=hash(readFileSync(compiler));
    const cfg=resolve(sdk,'bin/clang++.cfg');if(existsSync(cfg))receipt.toolchain.configSha256=hash(readFileSync(cfg));
    receipt.toolchain.version=(await run('compiler-version',compiler,['--version'])).stdout.trim();
    for(const [name,path] of [['game',root],['common',common]])receipt.identity[name]={
      head:(await run(name+'-head','git',['-C',path,'rev-parse','HEAD'],{required:false})).stdout.trim(),
      status:(await run(name+'-status','git',['-C',path,'status','--porcelain=v1','--untracked-files=normal'],{required:false})).stdout};
    const flags=['--target=wasm32-wasip1','-std=c++17','-O2','-g0','-UNDEBUG',
      '-DTH11_MULTIPLAYER=1','-DTH11_DEVELOPMENT_HARNESS=0','-I'+resolve(common,'include'),
      '-ffp-contract=off','-fno-strict-aliasing','-fno-exceptions','-fno-rtti'];
    receipt.compileFlags=flags;
    const headerKey=hash(headers.sort().map(p=>p+' '+observed.get(p)).join('\n'));
    const cache=resolve(root,'artifacts/rollback-smoothness/world-object-cache');mkdirSync(cache,{recursive:true});
    let cursor=0;const objects=new Array(sources.length);let failure=null;
    const worker=async()=>{
      while(cursor<sources.length&&!failure){
        const index=cursor++,file=sources[index];
        const key=hash(JSON.stringify([flags,headerKey,observed.get(file),receipt.toolchain.compilerSha256,receipt.toolchain.configSha256]));
        const object=resolve(cache,basename(file)+'-'+key+'.o');
        objects[index]=object;
        if(existsSync(object)&&existsSync(object+'.sha256')&&readFileSync(object+'.sha256','utf8')===hash(readFileSync(object)))continue;
        try{
          await run('compile-'+String(index).padStart(3,'0')+'-'+basename(file),compiler,[...flags,'-c',file,'-o',object]);
          writeFileSync(object+'.sha256',hash(readFileSync(object)));
        }catch(error){failure=error;}
      }
    };
    // This is one bounded root-owned build, never concurrent test sessions.
    await Promise.all(Array.from({length:4},worker));
    if(failure)throw failure;
    checkSources();
    receipt.objects=objects.map(path=>({path,sha256:hash(readFileSync(path))}));save();
    for(const name of suites){
      const entry=resolve(out,name+'-entry.o'),wasm=resolve(out,name+'-rollback.wasm');
      await run(name+'-entry-compile',compiler,[...flags,'-c',entries[name],'-o',entry]);
      await run(name+'-link',compiler,[...flags,...objects,entry,'-Wl,-z,stack-size=4194304','-Wl,--max-memory=1073741824','-o',wasm]);
      checkSources();
      const item={suite:name,wasm,sha256:hash(readFileSync(wasm)),bytes:readFileSync(wasm).length,passed:false};
      receipt.suites.push(item);save();
      const result=await run(name+'-execute',process.execPath,[import.meta.filename,'--run-wasm',wasm,'--archive',archive]);
      item.passed=result.exitCode===0;save();process.stdout.write(result.stdout);
    }
    checkSources();receipt.sourceStable=true;receipt.passed=receipt.suites.every(s=>s.passed);
  }catch(error){receipt.error=error?.stack||String(error);process.exitCode=1;console.error(String(error));}
  finally{receipt.completedAt=new Date().toISOString();save();console.log('world/native receipt: '+resolve(out,'receipt.json'));}
}
