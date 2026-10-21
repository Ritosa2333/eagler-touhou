// O2 real Runtime/core regression. Root serializes execution; fresh receipts retain failures.
import {spawnSync} from 'node:child_process';
import {readFileSync,mkdirSync,writeFileSync,copyFileSync,readdirSync,existsSync} from 'node:fs';
import {resolve,join,relative} from 'node:path';
import {pathToFileURL} from 'node:url';
import {WASI} from 'node:wasi';
import {webcrypto,createHash} from 'node:crypto';
import assert from 'node:assert/strict';
const root=resolve(import.meta.dirname,'../..');
const option=(name,fallback)=>{const i=process.argv.indexOf(name);if(i<0)return fallback;if(!process.argv[i+1]||process.argv[i+1].startsWith('--'))throw Error('Missing '+name);return process.argv[i+1];};
const nativeOnly=process.argv.includes('--native-only');
const out=resolve(option('--output',resolve(root,'artifacts/rollback-smoothness/network-'+new Date().toISOString().replaceAll(':','-').replaceAll('.','-'))));
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const commands=[];
function run(label,program,args,options){
 const item={label,program,args,startedAt:new Date().toISOString()};
 const result=spawnSync(program,args,options);
 Object.assign(item,{completedAt:new Date().toISOString(),exitCode:result.status,signal:result.signal,error:result.error?String(result.error):null,
  stdoutFile:label+'.stdout.log',stderrFile:label+'.stderr.log'});
 writeFileSync(resolve(out,item.stdoutFile),result.stdout||'');writeFileSync(resolve(out,item.stderrFile),result.stderr||'');
 commands.push(item);writeFileSync(resolve(out,process.argv.includes('--run-suite')?'suite-commands.json':'commands.json'),JSON.stringify(commands,null,2)+'\n');return result;
}
async function suite(){
const root=resolve(import.meta.dirname,'../..');
const common=resolve(root,'../../common'),native=resolve(root,'web/cpp/multiplayer');
const sdk=process.env.WASI_SDK_ROOT||'D:/workspace/eagler/toolchains/wasi-sdk-34.0-x86_64-windows';
const compiler=resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++');
mkdirSync(out,{recursive:true});
const sources=[...readdirSync(resolve(common,'src/netplay')).filter(p=>p.endsWith('.cpp')).map(p=>resolve(common,'src/netplay',p)),
 ...['SessionSetup.cpp','InputLanes.cpp','NetplayRuntime.cpp','ReplayArchive.cpp'].map(p=>join(native,p)),resolve(import.meta.dirname,'network-protocol.cpp'),resolve(import.meta.dirname,'network-rollback.cpp'),resolve(root,'web/cpp/game/GameInput.cpp')];
const wasm=resolve(out,'network-protocol.wasm');
const build=run('network-compile',compiler,['--target=wasm32-wasip1','-std=c++17','-O2','-g0','-DTH11_MULTIPLAYER=1','-ffp-contract=off','-fno-strict-aliasing','-fno-exceptions','-fno-rtti',
 '-I'+resolve(common,'include'),'-I'+native,...sources,'-Wl,-z,stack-size=2097152','-Wl,--max-memory=268435456','-o',wasm],
 {cwd:root,encoding:'utf8',windowsHide:true,maxBuffer:4*1024*1024});
if(build.error)throw build.error;if(build.status)throw Error(build.stdout+build.stderr);
const wasi=new WASI({version:'preview1',args:['network-protocol','/output/replay-p3.rpy','/output/replay-challenge.rpy','/output/replay-direct.rpy','/output/replay-hybrid.rpy'],preopens:{'/output':out},returnOnExit:true});
const instance=await WebAssembly.instantiate(await WebAssembly.compile(readFileSync(wasm)),{wasi_snapshot_preview1:wasi.wasiImport});
assert.equal(wasi.start(instance),0);
for(const name of ['calibration-retirement','session-channel','adonis-startup']){
 const target=resolve(out,name+'.wasm');
 const result=run(name+'-compile',compiler,['--target=wasm32-wasip1','-std=c++17','-O2','-g0','-UNDEBUG','-fno-exceptions','-fno-rtti',
  '-I'+resolve(common,'include'),'-I'+resolve(common,'tests/fixtures/include'),
  ...['NetplayProtocol.cpp','NetplayCore.cpp','NetplaySession.cpp','SessionChannel.cpp','RollbackJournal.cpp'].map(file=>resolve(common,'src/netplay',file)),
  resolve(common,'tests',name+'-test.cpp'),'-Wl,-z,stack-size=2097152','-o',target],{cwd:root,encoding:'utf8',windowsHide:true,maxBuffer:4*1024*1024});
 if(result.error)throw result.error;if(result.status)throw Error(result.stdout+result.stderr);
 const owner=new WASI({version:'preview1',args:[name],returnOnExit:true});
 const fixture=await WebAssembly.instantiate(await WebAssembly.compile(readFileSync(target)),{wasi_snapshot_preview1:owner.wasiImport});
 assert.equal(owner.start(fixture),0,name+' shared regression');
}
const tail=run('spectator-tail',process.execPath,[resolve(common,'tests/browser-peer-spectator-tail.test.mjs')],{cwd:root,encoding:'utf8',windowsHide:true});
if(tail.status)throw Error(tail.stdout+tail.stderr);console.log(tail.stdout.trim());
if(nativeOnly){console.log('PASS TH11 native network suite (browser capability checks intentionally pending)');return;}
const browser=resolve(out,'browser');mkdirSync(browser,{recursive:true});
for(const [source,name] of [[resolve(root,'web/sdl-runtime/multiplayer.mjs'),'multiplayer.mjs'],
 [resolve(root,'portable/browser/multiplayer-replay.mjs'),'multiplayer-replay.mjs'],[resolve(common,'browser/adonis-calibration.mjs'),'adonis-calibration.mjs']])copyFileSync(source,resolve(browser,name));
const {multiplayerBuildWords,validateMultiplayerOptions,multiplayerSetupWords}=await import(pathToFileURL(resolve(browser,'multiplayer.mjs')));
const {inspectMultiplayerReplay,multiplayerReplayPath}=await import(pathToFileURL(resolve(browser,'multiplayer-replay.mjs')));
const buildWords=[0x12345678,0x90abcdef,0x87654321,0xfedcba09];
const manifest={game:'th11',product:'th11mp',variant:'multiplayer',profile:'multiplayer',features:{multiplayer:true},
 execution:{sha256:buildWords.map(n=>n.toString(16).padStart(8,'0')).join('')+'0'.repeat(32)}};
assert.deepEqual(multiplayerBuildWords(manifest),buildWords);
assert.throws(()=>multiplayerBuildWords({...manifest,product:'th11'}));
const base={netplayMode:'lan',netplayUrl:'wss://relay.invalid/relay?room=th11mp-abcd&run=17&member=private',
 netplayPlayerCount:3,netplayPlayer:2,netplaySeed:417,netplayDifficulty:1,netplayLoadouts:[{character:0,shot:0},{character:1,shot:1},{character:1,shot:2}],
 netplayAdonisMode:1,netplayInputDelayAuto:false,netplayInputDelay:2,netplayPredictionReserve:2,netplayPredictionLimit:0};
const first=await multiplayerSetupWords(base,buildWords,webcrypto),other=await multiplayerSetupWords({...base,netplayPlayer:0,netplayUrl:base.netplayUrl.replace('private','other')},buildWords,webcrypto);
assert.equal(first.words.length,22);assert.equal(first.words[0],6);assert.equal(first.words[14],1);assert.equal(first.words[2],2);assert.deepEqual(first.words.slice(5,7),other.words.slice(5,7));
assert.notDeepEqual(first.words.slice(5,7),(await multiplayerSetupWords({...base,netplayUrl:base.netplayUrl.replace('run=17','run=18')},buildWords,webcrypto)).words.slice(5,7));
for(const options of [{netplayAdonisMode:2},{netplayPredictionLimit:1},{netplayInputDelayAuto:true},{netplayChallengeMode:1},{netplayChallengeMode:'true'},{thpracEnabled:true},{netplayLoadouts:[{character:2,shot:0}]}])
 assert.throws(()=>validateMultiplayerOptions({...base,...options}));
const modes=new Map();
for(const mode of [0,1,2]){
 const options={...base,netplayAdonisMode:mode,netplayInputDelay:mode?2:0,netplayPredictionLimit:mode===1?0:12};
 const configured=await multiplayerSetupWords(options,buildWords,webcrypto);modes.set(mode,configured);
 assert.equal(configured.config.timingMode,mode);assert.equal(configured.config.prediction,mode===1?0:12);
 assert.equal(configured.words.length,22);assert.equal(configured.words[0],6);assert.equal(configured.words[14],mode);
 assert.equal(configured.words[7],mode?2:0);assert.equal(configured.words[15],0);
 assert.equal(validateMultiplayerOptions({...options,netplayPredictionLimit:undefined}).prediction,mode===1?0:12);
 for(const limit of mode===1?[1,12]:[0,1,11,13])assert.throws(()=>validateMultiplayerOptions({...options,netplayPredictionLimit:limit}));
 if(mode){
  const automatic=await multiplayerSetupWords({...options,netplayInputDelayAuto:true,netplayInputDelay:0},buildWords,webcrypto);
  assert.equal(automatic.words[7],0);assert.equal(automatic.words[15],1);
  const manual=await multiplayerSetupWords({...options,netplayInputDelayAuto:false,netplayInputDelay:0},buildWords,webcrypto);
  assert.equal(manual.words[7],0);assert.equal(manual.words[15],0); // explicit manual D0 remains authoritative
 }else{
  assert.throws(()=>validateMultiplayerOptions({...options,netplayInputDelay:1}));
  assert.throws(()=>validateMultiplayerOptions({...options,netplayInputDelayAuto:true}));
 }
 const observer=await multiplayerSetupWords({...options,netplaySpectator:true,netplaySpectatorId:'observer'},buildWords,webcrypto);
 assert.equal(observer.words[2],0);assert.equal(observer.config.spectator,true);assert.deepEqual(observer.words.slice(5,7),configured.words.slice(5,7));
 assert.throws(()=>validateMultiplayerOptions({...options,netplaySpectator:true}));
}
for(const mode of [-1,3,'0',null])assert.throws(()=>validateMultiplayerOptions({...base,netplayAdonisMode:mode}));
for(const challenge of [false,true]){const configured=await multiplayerSetupWords({...base,netplayChallengeMode:challenge},buildWords,webcrypto);assert.equal(configured.words[21],+challenge);assert.equal(configured.config.challenge,challenge);assert.deepEqual(configured.words.slice(0,21),first.words.slice(0,21));}
assert.deepEqual(validateMultiplayerOptions({replayViewer:true}),{replay:true});
assert.equal(multiplayerReplayPath('/savesth11mp/replay/th11_03.rpy'),'replay/th11_03.rpy');
assert.equal(multiplayerReplayPath('/savesth11/replay/th11_03.rpy'),'replay/th11_03.rpy');
for(const path of ['scoreth11.dat','replay/th11mp_03.rpy','replay/th11_03.rpyx','../replay/th11_03.rpy'])assert.throws(()=>multiplayerReplayPath(path));
const core=instance.exports,validate=bytes=>{const pointer=core.network_allocate(bytes.length);try{new Uint8Array(core.memory.buffer,pointer,bytes.length).set(bytes);return core.network_validate(pointer,bytes.length)===1;}finally{core.network_free(pointer);}};
const bytes=new Uint8Array(readFileSync(resolve(out,'replay-p3.rpy'))),metadata=inspectMultiplayerReplay(bytes,{validate,buildWords});
assert.equal(metadata.recordedPlayer,2);assert.equal(metadata.playerCount,3);assert.equal(metadata.frameCount,180);assert.equal(metadata.inputDelay,2);assert.equal(metadata.prediction,0);assert.equal(metadata.score,9999999990);
assert.equal(metadata.name,'P3TEST');assert.deepEqual(metadata.chapters,[{stage:1,frame:0},{stage:2,frame:90}]);
assert.equal(metadata.challenge,false);
assert.equal(new DataView(bytes.buffer).getUint32(80,true),5);assert.equal(metadata.timingMode,1); // genuine legacy fixture
const checkedModes=[];
const checksum=bytes=>{let h=2166136261;for(let i=0;i<bytes.length;i++)if(i<36||i>=40)h=Math.imul((h^bytes[i])>>>0,16777619)>>>0;new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength).setUint32(36,h,true);};
for(const mode of [0,1,2]){
 const original=mode===1?bytes:new Uint8Array(readFileSync(resolve(out,mode===0?'replay-direct.rpy':'replay-hybrid.rpy')));
 const meta=inspectMultiplayerReplay(original,{validate,buildWords});
 assert.equal(meta.timingMode,mode);assert.equal(meta.prediction,mode===1?0:12);assert.equal(meta.inputDelay,mode?2:0);
 assert.equal(meta.recordedPlayer,2);assert.equal(meta.playerCount,3);assert.equal(meta.frameCount,mode===1?180:4);
 if(mode!==1){assert.equal(meta.name,'RBTEST');assert.equal(meta.score,1234560);assert.equal(new DataView(original.buffer).getUint32(80,true),6);}
 // The real native decoder checks the exact setup words emitted by the JS bridge.
 const bridged=original.slice(),view=new DataView(bridged.buffer);
 modes.get(mode).words.forEach((word,i)=>view.setUint32(80+i*4,word,true));checksum(bridged);
 assert.equal(validate(bridged),true,'JS/native setup contract mode '+mode);
 assert.equal(inspectMultiplayerReplay(bridged,{validate,buildWords}).timingMode,mode);checkedModes.push(mode);
 if(mode!==1){const legacy=original.slice();new DataView(legacy.buffer).setUint32(80,5,true);checksum(legacy);assert.equal(validate(legacy),false);assert.throws(()=>inspectMultiplayerReplay(legacy,{validate,buildWords}));}
 const wrong=original.slice();new DataView(wrong.buffer).setUint32(136,3,true);checksum(wrong);assert.equal(validate(wrong),false);assert.throws(()=>inspectMultiplayerReplay(wrong,{validate,buildWords}));
}
const challengeBytes=new Uint8Array(readFileSync(resolve(out,'replay-challenge.rpy'))),challengeMetadata=inspectMultiplayerReplay(challengeBytes,{validate,buildWords});
assert.equal(challengeMetadata.challenge,true);assert.equal(challengeMetadata.prediction,0);assert.equal(challengeMetadata.frameCount,180);
assert.throws(()=>inspectMultiplayerReplay(challengeBytes,{validate,buildWords:[1,2,3,4]}));
for(const oldRules of [1,2]){
 const old=bytes.slice();new DataView(old.buffer).setUint32(48,oldRules,true);checksum(old);
 assert.equal(validate(old),false);assert.throws(()=>inspectMultiplayerReplay(old,{validate,buildWords}));
}
const invalidChallenge=challengeBytes.slice();new DataView(invalidChallenge.buffer).setUint32(164,2,true);assert.throws(()=>inspectMultiplayerReplay(invalidChallenge,{validate,buildWords}));
assert.throws(()=>inspectMultiplayerReplay(bytes,{validate,buildWords:[1,2,3,4]}));
const malformed=bytes.slice();malformed[malformed.length-1]^=1;assert.throws(()=>inspectMultiplayerReplay(malformed,{validate,buildWords}));
writeFileSync(resolve(out,'report.json'),JSON.stringify({passed:true,network:{players:[2,3],delays:[0,2,9],modes:checkedModes,legacyPrediction:0,rollbackPredictionLimit:12,worldUndoTested:false},recordedPlayer:metadata.recordedPlayer,frameCount:metadata.frameCount,score:metadata.score},null,2)+'\n');
console.log('PASS TH11 browser configure/identity rejection, URL admission preservation, native-validated Replay metadata, recorded P3, namespace isolation');
}
if(process.argv.includes('--run-suite')){
 await suite();
}else{
 if(existsSync(resolve(out,'receipt.json')))throw Error('Existing receipt; choose a fresh --output directory');
 mkdirSync(out,{recursive:true});
 const receipt={schema:'th11-runtime-network/2',startedAt:new Date().toISOString(),completedAt:null,passed:false,root,output:out,
  scope:{nativeRuntime:true,realCommonCore:true,realGameInputEdges:true,players:[2,3],legacyDelays:[0,2,9],predictionLimit:12,optimization:'O2',browserOptionChecks:!nativeOnly,
   browserSession:false,worldRestore:false,performance:false,retailAssets:false},
  runtime:{node:process.execPath,nodeVersion:process.version,platform:process.platform,arch:process.arch},sourceFiles:{},sourceStable:false,identity:{},commands:[]};
 const save=()=>writeFileSync(resolve(out,'receipt.json'),JSON.stringify(receipt,null,2)+'\n');
 save();
 try{
  const common=resolve(root,'../../common'),native=resolve(root,'web/cpp/multiplayer'),game=resolve(root,'web/cpp/game');
  const walk=dir=>readdirSync(dir,{withFileTypes:true}).flatMap(e=>e.isDirectory()?walk(resolve(dir,e.name)):[resolve(dir,e.name)]);
  const files=[...walk(resolve(common,'include')),...walk(resolve(common,'src/netplay')),
   ...walk(resolve(common,'tests/fixtures/include')),...['calibration-retirement-test.cpp','session-channel-test.cpp','adonis-startup-test.cpp','browser-peer-spectator-tail.test.mjs'].map(p=>resolve(common,'tests',p)),
   ...['SessionSetup.cpp','SessionSetup.hpp','InputLanes.cpp','InputLanes.hpp','NetplayRuntime.cpp','NetplayRuntime.hpp','ReplayArchive.cpp','ReplayArchive.hpp'].map(p=>resolve(native,p)),
   ...walk(game).filter(p=>/\.(h|hpp|inc)$/.test(p)),resolve(game,'GameInput.cpp'),
   resolve(import.meta.filename),resolve(import.meta.dirname,'network-protocol.cpp'),resolve(import.meta.dirname,'network-rollback.cpp')];
  if(!nativeOnly)files.push(resolve(root,'web/sdl-runtime/multiplayer.mjs'),resolve(root,'portable/browser/multiplayer-replay.mjs'),resolve(common,'browser/adonis-calibration.mjs'));
  const observed=new Map([...new Set(files)].map(path=>[path,hash(readFileSync(path))]));
  receipt.sourceFiles=Object.fromEntries([...observed].sort(([a],[b])=>a.localeCompare(b)).map(([path,digest])=>[relative(root,path).replaceAll('\\','/'),digest]));
  receipt.sourceDigest=hash(JSON.stringify(receipt.sourceFiles));
  const sdk=resolve(process.env.WASI_SDK_ROOT||'D:/workspace/eagler/toolchains/wasi-sdk-34.0-x86_64-windows'),compiler=resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++');
  receipt.toolchain={sdk,compiler,sha256:hash(readFileSync(compiler))};
  receipt.toolchain.version=run('compiler-version',compiler,['--version'],{cwd:root,encoding:'utf8',windowsHide:true}).stdout;
  for(const [name,path] of [['game',root],['common',common]]){
   receipt.identity[name]={head:run(name+'-head','git',['-C',path,'rev-parse','HEAD'],{cwd:root,encoding:'utf8',windowsHide:true}).stdout.trim(),
    status:run(name+'-status','git',['-C',path,'status','--porcelain=v1','--untracked-files=normal'],{cwd:root,encoding:'utf8',windowsHide:true}).stdout};
  }
  save();
  const args=[resolve(import.meta.filename),'--run-suite','--output',out,...(nativeOnly?['--native-only']:[])];
  const result=run('suite',process.execPath,args,{cwd:root,encoding:'utf8',windowsHide:true,maxBuffer:16*1024*1024});
  receipt.commands=JSON.parse(readFileSync(resolve(out,'commands.json'),'utf8'));
  // The child records compiler invocations separately from this orchestration process.
  if(existsSync(resolve(out,'suite-commands.json')))receipt.commands.push(...JSON.parse(readFileSync(resolve(out,'suite-commands.json'),'utf8')));
  if(result.stdout)process.stdout.write(result.stdout);if(result.stderr)process.stderr.write(result.stderr);
  if(result.error||result.status!==0)throw Error('Runtime network suite failed; see suite logs');
  for(const [path,digest] of observed)if(hash(readFileSync(path))!==digest)throw Error('Source changed during validation: '+path);
  receipt.sourceStable=true;
  receipt.artifacts={};
  for(const name of readdirSync(out).filter(p=>/\.(wasm|rpy)$/.test(p)))receipt.artifacts[name]={sha256:hash(readFileSync(resolve(out,name))),bytes:readFileSync(resolve(out,name)).length};
  const marker=result.stdout.match(/PASS TH11 rollback network: (\d+) groups, (\d+) checks/);
  if(!marker)throw Error('Missing rollback behavior completion marker');receipt.rollback={groups:Number(marker[1]),checks:Number(marker[2])};receipt.passed=true;
 }catch(error){receipt.error=error?.stack||String(error);process.exitCode=1;console.error(String(error));}
 finally{receipt.completedAt=new Date().toISOString();save();console.log('network receipt: '+resolve(out,'receipt.json'));}
}
