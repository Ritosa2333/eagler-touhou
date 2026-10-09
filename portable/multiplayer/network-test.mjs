import {spawnSync} from 'node:child_process';
import {readFileSync,mkdirSync,writeFileSync,copyFileSync,readdirSync} from 'node:fs';
import {resolve,join} from 'node:path';
import {pathToFileURL} from 'node:url';
import {WASI} from 'node:wasi';
import {webcrypto} from 'node:crypto';
import assert from 'node:assert/strict';
const root=resolve(import.meta.dirname,'../..'),out=resolve(root,'artifacts/multiplayer-network');
const common=resolve(root,'third_party/eagler-common'),native=resolve(root,'th11_web/cpp/multiplayer');
const sdk=process.env.WASI_SDK_ROOT||'D:/workspace/eagler/toolchains/wasi-sdk-34.0-x86_64-windows';
mkdirSync(out,{recursive:true});
const sources=[...readdirSync(resolve(common,'src/netplay')).filter(p=>p.endsWith('.cpp')).map(p=>resolve(common,'src/netplay',p)),
 ...['SessionSetup.cpp','InputLanes.cpp','NetplayRuntime.cpp','ReplayArchive.cpp'].map(p=>join(native,p)),resolve(import.meta.dirname,'network-protocol.cpp')];
const wasm=resolve(out,'network-protocol.wasm');
const build=spawnSync(resolve(sdk,'bin/clang++.exe'),['--target=wasm32-wasip1','-std=c++17','-O1','-g0','-DTH11_MULTIPLAYER=1','-ffp-contract=off','-fno-strict-aliasing','-fno-exceptions','-fno-rtti',
 '-I'+resolve(common,'include'),'-I'+native,...sources,'-Wl,-z,stack-size=2097152','-Wl,--max-memory=268435456','-o',wasm],
 {cwd:root,encoding:'utf8',windowsHide:true,maxBuffer:4*1024*1024});
if(build.error)throw build.error;if(build.status)throw Error(build.stdout+build.stderr);
const wasi=new WASI({version:'preview1',args:['network-protocol','/output/replay-p3.rpy'],preopens:{'/output':out},returnOnExit:true});
const instance=await WebAssembly.instantiate(await WebAssembly.compile(readFileSync(wasm)),{wasi_snapshot_preview1:wasi.wasiImport});
assert.equal(wasi.start(instance),0);
for(const name of ['calibration-retirement','session-channel','adonis-startup']){
 const target=resolve(out,name+'.wasm');
 const result=spawnSync(resolve(sdk,'bin/clang++.exe'),['--target=wasm32-wasip1','-std=c++17','-O1','-g0','-UNDEBUG','-fno-exceptions','-fno-rtti',
  '-I'+resolve(common,'include'),'-I'+resolve(common,'tests/fixtures/include'),
  ...['NetplayProtocol.cpp','NetplayCore.cpp','NetplaySession.cpp','SessionChannel.cpp','RollbackJournal.cpp'].map(file=>resolve(common,'src/netplay',file)),
  resolve(common,'tests',name+'-test.cpp'),'-Wl,-z,stack-size=2097152','-o',target],{cwd:root,encoding:'utf8',windowsHide:true,maxBuffer:4*1024*1024});
 if(result.error)throw result.error;if(result.status)throw Error(result.stdout+result.stderr);
 const owner=new WASI({version:'preview1',args:[name],returnOnExit:true});
 const fixture=await WebAssembly.instantiate(await WebAssembly.compile(readFileSync(target)),{wasi_snapshot_preview1:owner.wasiImport});
 assert.equal(owner.start(fixture),0,name+' shared regression');
}
const tail=spawnSync(process.execPath,[resolve(common,'tests/browser-peer-spectator-tail.test.mjs')],{cwd:root,encoding:'utf8',windowsHide:true});
if(tail.status)throw Error(tail.stdout+tail.stderr);console.log(tail.stdout.trim());
const browser=resolve(out,'browser');mkdirSync(browser,{recursive:true});
for(const [source,name] of [[resolve(root,'th11_web/sdl-runtime/multiplayer.mjs'),'multiplayer.mjs'],
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
assert.equal(first.words.length,22);assert.equal(first.words[2],2);assert.deepEqual(first.words.slice(5,7),other.words.slice(5,7));
assert.notDeepEqual(first.words.slice(5,7),(await multiplayerSetupWords({...base,netplayUrl:base.netplayUrl.replace('run=17','run=18')},buildWords,webcrypto)).words.slice(5,7));
for(const options of [{netplayAdonisMode:2},{netplayPredictionLimit:1},{netplayInputDelayAuto:true},{netplayChallengeMode:true},{thpracEnabled:true},{netplayLoadouts:[{character:2,shot:0}]}])
 assert.throws(()=>validateMultiplayerOptions({...base,...options}));
assert.deepEqual(validateMultiplayerOptions({replayViewer:true}),{replay:true});
assert.equal(multiplayerReplayPath('/savesth11mp/replay/th11_03.rpy'),'replay/th11_03.rpy');
assert.equal(multiplayerReplayPath('/savesth11/replay/th11_03.rpy'),'replay/th11_03.rpy');
for(const path of ['scoreth11.dat','replay/th11mp_03.rpy','replay/th11_03.rpyx','../replay/th11_03.rpy'])assert.throws(()=>multiplayerReplayPath(path));
const core=instance.exports,validate=bytes=>{const pointer=core.network_allocate(bytes.length);try{new Uint8Array(core.memory.buffer,pointer,bytes.length).set(bytes);return core.network_validate(pointer,bytes.length)===1;}finally{core.network_free(pointer);}};
const bytes=new Uint8Array(readFileSync(resolve(out,'replay-p3.rpy'))),metadata=inspectMultiplayerReplay(bytes,{validate,buildWords});
assert.equal(metadata.recordedPlayer,2);assert.equal(metadata.playerCount,3);assert.equal(metadata.frameCount,180);assert.equal(metadata.inputDelay,2);assert.equal(metadata.prediction,0);assert.equal(metadata.score,9999999990);
assert.equal(metadata.name,'P3TEST');assert.deepEqual(metadata.chapters,[{stage:1,frame:0},{stage:2,frame:90}]);
assert.throws(()=>inspectMultiplayerReplay(bytes,{validate,buildWords:[1,2,3,4]}));
const malformed=bytes.slice();malformed[malformed.length-1]^=1;assert.throws(()=>inspectMultiplayerReplay(malformed,{validate,buildWords}));
writeFileSync(resolve(out,'report.json'),JSON.stringify({passed:true,network:{players:[2,3],delays:[0,2,9],prediction:0,undoBytes:0},recordedPlayer:metadata.recordedPlayer,frameCount:metadata.frameCount,score:metadata.score},null,2)+'\n');
console.log('PASS TH11 browser configure/identity rejection, URL admission preservation, native-validated Replay metadata, recorded P3, namespace isolation');
