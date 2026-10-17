// Root runs this serially. Component PCM oracle, not a browser/SDL/performance gate.
import {spawn} from 'node:child_process';
import {readFileSync,writeFileSync,mkdirSync,existsSync} from 'node:fs';
import {resolve,relative,dirname,basename,extname} from 'node:path';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'../..'),workspace=resolve(root,'../..');
const option=(name,fallback)=>{const at=process.argv.indexOf(name);if(at<0)return fallback;const value=process.argv[at+1];if(!value||value.startsWith('--'))throw Error('Missing '+name+' value');return value;};
const sha=bytes=>createHash('sha256').update(bytes).digest('hex');
if(process.argv.includes('--run-wasm')){
  const wasm=resolve(option('--run-wasm')),archive=resolve(option('--archive')),bgm=resolve(option('--bgm')),out=resolve(option('--output'));
  const stem=basename(bgm,extname(bgm));
  const wasi=new WASI({version:'preview1',returnOnExit:true,
    args:['audio-pitch-bypass','/archive/'+basename(archive),'/music/'+basename(bgm),stem,'/output'],
    preopens:{'/archive':dirname(archive),'/music':dirname(bgm),'/output':out}});
  const module=await WebAssembly.compile(readFileSync(wasm));
  const instance=await WebAssembly.instantiate(module,{wasi_snapshot_preview1:wasi.wasiImport});
  process.exitCode=wasi.start(instance);
}else{
  const sdk=resolve(process.env.WASI_SDK_ROOT||'D:/workspace/eagler/toolchains/wasi-sdk-34.0-x86_64-windows');
  const compiler=resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++');
  const archive=resolve(option('--archive',process.env.TH11_TEST_ARCHIVE||resolve(workspace,'th11-eagler/[th11] 东方地灵殿 (汉化版+日文版)/th11.dat')));
  const bgm=resolve(option('--bgm',resolve(workspace,'games/web-content/th11/music/th11_01.ogg')));
  const stamp=new Date().toISOString().replaceAll(':','-').replaceAll('.','-');
  const out=resolve(option('--output',resolve(root,'artifacts/rollback-smoothness/audio-pitch-bypass-'+stamp)));
  if(existsSync(resolve(out,'receipt.json')))throw Error('Choose a fresh --output; an earlier receipt exists');
  mkdirSync(out,{recursive:true});
  const receipt={schema:'th11-audio-pitch-bypass/1',startedAt:new Date().toISOString(),completedAt:null,passed:false,
    root,output:out,scope:{pinnedMiniaudio:true,productionArchiveReader:true,retailWavSamples:46,retailSfxIds:56,
      realBgm:true,synthetic48000RateGuard:true,wasiBits:32,optimization:'O2',productionAudioDevice:false,
      browser:false,sdlOutput:false,rollbackPublication:false,fullGame:false,performanceGate:false},
    semantics:{comparisonTolerance:0,sameRateModel:'original has one PCM frame of linear-resampler history; candidate removes it',
      endBoundary:'old nonlooping node does not drain one retained terminal PCM frame',
      differentRateModel:'identical output; required resampling remains enabled',
      filterRemoved:false,denseCaseLooping:true,rawPcmRetained:true},
    runtime:{node:process.execPath,nodeVersion:process.version,platform:process.platform,arch:process.arch},
    toolchain:{sdk,compiler},sourceFiles:{},sourceDigest:null,sourceStable:false,resources:{},commands:[],cases:[]};
  const save=()=>writeFileSync(resolve(out,'receipt.json'),JSON.stringify(receipt,null,2)+'\n');save();
  const observed=new Map();
  const observe=path=>{const digest=sha(readFileSync(path));if(observed.has(path)&&observed.get(path)!==digest)throw Error('Dependency changed: '+path);observed.set(path,digest);return digest;};
  const check=()=>{for(const [path,digest] of observed)if(sha(readFileSync(path))!==digest)throw Error('Dependency changed during oracle: '+path);};
  async function run(label,program,args){
    const command={label,program,args,cwd:root,startedAt:new Date().toISOString(),completedAt:null,exitCode:null,signal:null,
      stdoutFile:label+'.stdout.log',stderrFile:label+'.stderr.log'};receipt.commands.push(command);save();
    let stdout='',stderr='';
    try{
      await new Promise((accept,reject)=>{
        const child=spawn(program,args,{cwd:root,windowsHide:true,stdio:['ignore','pipe','pipe']});
        const timer=setTimeout(()=>{command.timedOut=true;child.kill();},240000);
        child.stdout.on('data',bytes=>{stdout+=bytes.toString();if(stdout.length>8*1024*1024){command.outputExceeded=true;child.kill();}});
        child.stderr.on('data',bytes=>{stderr+=bytes.toString();if(stderr.length>8*1024*1024){command.outputExceeded=true;child.kill();}});
        child.once('error',error=>{clearTimeout(timer);reject(error);});
        child.once('exit',(code,signal)=>{clearTimeout(timer);command.exitCode=code;command.signal=signal;accept();});
      });
    }catch(error){command.error=String(error);}
    finally{command.completedAt=new Date().toISOString();writeFileSync(resolve(out,command.stdoutFile),stdout);writeFileSync(resolve(out,command.stderrFile),stderr);save();}
    if(command.error||command.exitCode!==0||command.timedOut||command.outputExceeded)throw Error(label+' failed; see '+out);
    return {stdout,stderr};
  }
  try{
    const game=resolve(root,'th11_web/cpp/game'),thirdParty=resolve(root,'portable/sdl/third_party');
    const sources=['Archive.cpp','Lzss.cpp','ResourceCrypt.cpp','SoundEffects.cpp'].map(name=>resolve(game,name));
    sources.push(resolve(import.meta.dirname,'audio-pitch-bypass.cpp'));
    // Deliberately observe only this fixture's actual project dependencies.
    // Renderer/Shader/other agents' unrelated headers cannot invalidate this run.
    const dependencies=[...sources,...['Archive.hpp','Lzss.hpp','ResourceCrypt.hpp','SoundEffects.hpp','Types.hpp'].map(name=>resolve(game,name)),
      resolve(thirdParty,'miniaudio.h'),resolve(thirdParty,'stb_vorbis.h'),resolve(import.meta.filename)];
    for(const path of dependencies)observe(path);
    receipt.sourceFiles=Object.fromEntries(dependencies.map(path=>[relative(root,path).replaceAll('\\','/'),observed.get(path)]).sort(([a],[b])=>a.localeCompare(b)));
    receipt.sourceDigest=sha(JSON.stringify(receipt.sourceFiles));
    receipt.resources={archive:{path:archive,sha256:observe(archive)},bgm:{path:bgm,sha256:observe(bgm)}};
    receipt.toolchain.compilerSha256=sha(readFileSync(compiler));
    const config=resolve(sdk,'bin/clang++.cfg');if(existsSync(config))receipt.toolchain.compilerConfigSha256=sha(readFileSync(config));
    receipt.toolchain.version=(await run('compiler-version',compiler,['--version'])).stdout.trim();check();save();
    const wasm=resolve(out,'audio-pitch-bypass.wasm');
    const flags=['--target=wasm32-wasip1','-std=c++17','-O2','-g0','-UNDEBUG','-ffp-contract=off',
      '-fno-strict-aliasing','-fno-exceptions','-fno-rtti',
      '-Wl,-z,stack-size=4194304','-Wl,--max-memory=536870912'];
    await run('compile',compiler,[...flags,...sources,'-o',wasm]);check();
    const bytes=readFileSync(wasm);receipt.wasm={path:wasm,sha256:sha(bytes),bytes:bytes.length};save();
    const result=await run('execute',process.execPath,[resolve(import.meta.filename),'--run-wasm',wasm,'--archive',archive,'--bgm',bgm,'--output',out]);
    for(const line of result.stdout.split(/\r?\n/)){
      if(!line.startsWith('{'))continue;const item=JSON.parse(line);
      if(item.inventory)receipt.inventory=item.inventory;else if(item.case)receipt.cases.push(item);
    }
    const marker=result.stdout.match(/PASS AUDIO NO_PITCH PCM oracle: (\d+) cases, (\d+) checks/);
    if(!marker||Number(marker[1])!==62||receipt.cases.length!==62||receipt.cases.some(value=>!value.passed))
      throw Error('Incomplete PCM oracle or unexplained sample difference');
    if(receipt.inventory?.wavSamples!==46||receipt.inventory?.fixedSfxIds!==56)throw Error('Incomplete retail sound inventory');
    for(const resultCase of receipt.cases){
      resultCase.artifacts={};
      for(const suffix of ['original.f32','no-pitch.f32','differences.jsonl']){
        const path=resolve(out,resultCase.case+'.'+suffix);const bytes=readFileSync(path);
        resultCase.artifacts[suffix]={file:basename(path),sha256:sha(bytes),bytes:bytes.length};
      }
    }
    check();receipt.sourceStable=true;receipt.checks=Number(marker[2]);receipt.passed=true;
    console.log(marker[0]);
    const dense=receipt.cases.find(value=>value.case==='dense-56-looping');
    console.log(JSON.stringify({denseMixMs:{original:dense.originalMixMs,candidate:dense.candidateMixMs},timingIsDiagnosticOnly:true}));
  }catch(error){receipt.error=error?.stack||String(error);process.exitCode=1;console.error(String(error));}
  finally{receipt.completedAt=new Date().toISOString();save();console.log('Audio PCM receipt: '+resolve(out,'receipt.json'));}
}
