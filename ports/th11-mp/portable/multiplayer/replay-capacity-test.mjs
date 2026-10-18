import {spawnSync} from 'node:child_process';
import {readFileSync,mkdirSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {WASI} from 'node:wasi';
import assert from 'node:assert/strict';
import {inspectMultiplayerReplay,MULTIPLAYER_REPLAY_MAX_FRAMES} from '../browser/multiplayer-replay.mjs';
const root=resolve(import.meta.dirname,'../..'),common=resolve(root,'third_party/eagler-common');
const sdk=process.env.WASI_SDK_ROOT||'D:/workspace/eagler/toolchains/wasi-sdk-34.0-x86_64-windows';
const out=resolve(root,'artifacts/multiplayer-replay-capacity');mkdirSync(out,{recursive:true});
const wasm=resolve(out,'replay-capacity.wasm');
const source=[...['NetplayProtocol.cpp','InputReplay.cpp'].map(file=>resolve(common,'src/netplay',file)),
 ...['SessionSetup.cpp','InputLanes.cpp','ReplayArchive.cpp'].map(file=>resolve(root,'th11_web/cpp/multiplayer',file)),resolve(import.meta.dirname,'replay-capacity.cpp')];
const build=spawnSync(resolve(sdk,'bin/clang++.exe'),['--target=wasm32-wasip1','-std=c++17','-O1','-g0','-DTH11_MULTIPLAYER=1',
 '-ffp-contract=off','-fno-strict-aliasing','-fno-exceptions','-fno-rtti','-I'+resolve(common,'include'),'-I'+resolve(root,'th11_web/cpp/multiplayer'),...source,
 '-Wl,-z,stack-size=2097152','-Wl,--max-memory=268435456','-o',wasm],{cwd:root,encoding:'utf8',windowsHide:true,maxBuffer:2*1024*1024});
if(build.error)throw build.error;if(build.status)throw Error(build.stdout+build.stderr);
const wasi=new WASI({version:'preview1',args:['replay-capacity','/output/capacity.rpy'],preopens:{'/output':out},returnOnExit:true});
const instance=await WebAssembly.instantiate(await WebAssembly.compile(readFileSync(wasm)),{wasi_snapshot_preview1:wasi.wasiImport});
assert.equal(wasi.start(instance),0);
const core=instance.exports,bytes=new Uint8Array(readFileSync(resolve(out,'capacity.rpy')));
const validate=data=>{const p=core.capacity_allocate(data.length);try{new Uint8Array(core.memory.buffer,p,data.length).set(data);return core.capacity_validate(p,data.length)===1;}finally{core.capacity_free(p);}};
const metadata=inspectMultiplayerReplay(bytes,{validate,buildWords:[0x12345678,0x90abcdef,0x87654321,0xfedcba09]});
assert.equal(metadata.frameCount,MULTIPLAYER_REPLAY_MAX_FRAMES);assert.equal(metadata.frameCount,250000);assert.equal(metadata.completed,false);
assert.equal(metadata.recordedPlayer,2);assert.equal(metadata.prediction,0);assert.equal(metadata.name,'CAPACITY');
writeFileSync(resolve(out,'report.json'),JSON.stringify({passed:true,frameCount:metadata.frameCount,completed:metadata.completed,recordedPlayer:metadata.recordedPlayer,
 scope:'Actual native ReplayArchive and browser metadata only; no 69-minute native gameplay run',overflow:'01-99 and ud0000 occupied -> ud0001; no replacement'},null,2)+'\n');
console.log('PASS browser native-validated metadata: 250000-frame partial Replay, recorded P3, versioned capacity agrees');
