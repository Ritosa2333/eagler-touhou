// Current frozen AudioDevice vs the upstream post-track refresh change.
// Real OGG decode/miniaudio PCM, with the SDL device boundary held in memory.
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';
import {fullApplicationRecordingPlatform} from './th14-full-application-recording-platform-uniform2-nov13.mjs';
const R=resolve(import.meta.dirname,'..'),O=resolve(R,'artifacts/th14/current-concurrent-mixer-nov14/track-volume-nov17');
const read=p=>JSON.parse(readFileSync(p)),sha=b=>createHash('sha256').update(b).digest('hex');
const build=read(resolve(O,'build.json')),archivePath=read(resolve(R,'runtimes/th14/target.json')).dataArchives['th14.dat'].path;
const music=read(resolve(R,'artifacts/th14/published-vorbis-cache-nov13/prepared.json')),musicDirectory=resolve(R,music.musicDirectory);
for(const [n,h] of Object.entries(music.sha256))assert.equal(sha(readFileSync(resolve(musicDirectory,n))),h);
async function run(mode){
 const spec=build.modes[mode],bytes=readFileSync(resolve(O,spec.directory,'test.wasm'));assert.equal(sha(bytes),spec.wasmSha256);
 const module=new WebAssembly.Module(bytes),memory=new WebAssembly.Memory({initial:1024,maximum:16384,shared:true});const errors=[];
 const platform=fullApplicationRecordingPlatform(module,memory,{clockInput:1000000,archivePath,fontDirectory:resolve(R,'artifacts/th14/stable-stage-close-nov07/runtime-ready/fonts'),saveDirectory:resolve(O,spec.directory),onAsyncError:e=>errors.push(String(e))});
 let ns=0n;platform.env.SDL_GetTicksNS=()=>ns;
 const wasi=new WASI({version:'preview1',args:[],env:{},preopens:{'/music':musicDirectory}});
 const instance=new WebAssembly.Instance(module,{env:platform.env,wasi_snapshot_preview1:wasi.wasiImport,wasi:{'thread-spawn':()=>{throw Error('Unexpected worker in isolated audio test');}}});const c=instance.exports;platform.attach(c);wasi.initialize(instance);
 const error=()=>{const a=new Uint8Array(memory.buffer),p=c.error();return new TextDecoder().decode(a.subarray(p,a.indexOf(0,p)));};
 const rows=[];let mixedFrames=0;
 function mix(frames){const hash=createHash('sha256');let energy=0,peak=0;
  while(frames){const n=Math.min(1024,frames),p=c.mix(n);assert.ok(p,error());const pcm=new Float32Array(memory.buffer,p,n*2);hash.update(new Uint8Array(pcm.buffer,pcm.byteOffset,pcm.byteLength));for(const v of pcm){assert.ok(Number.isFinite(v)&&Math.abs(v)<=1);energy+=v*v;peak=Math.max(peak,Math.abs(v));}ns+=BigInt(n)*1000000000n/44100n;mixedFrames+=n;frames-=n;}
  return {sha256:hash.digest('hex'),energy,peak};}
 try{
  const dat=readFileSync(archivePath),p=c.allocate(dat.length);assert.ok(p);new Uint8Array(memory.buffer,p,dat.length).set(dat);assert.equal(c.open_archive(p,dat.length),1,error());assert.equal(platform.summary().audibleOutput,false);
  for(const level of [0,37,100])for(const target of [1,17])for(const state of ['clean','partial-fade','finished-fade','paused'])for(const muted of [false,true]){
   c.mute(0);c.master(level);assert.equal(c.music(0),1,error());const cleanGain=c.audio_stat(7);
   if(state==='partial-fade'){c.fade(10);for(let i=0;i<4;i++)c.update();if(level>0)assert.ok(c.audio_stat(7)<cleanGain,'prime a real attenuated voice');}
   if(state==='finished-fade'){c.fade(2);c.update();c.update();assert.equal(c.audio_stat(6),0);}
   if(state==='paused'){c.pause(1);assert.equal(c.audio_stat(8),1);}
   c.mute(+muted);assert.equal(c.music(target),1,error());
   const gain=c.audio_stat(7);assert.equal(gain,muted?0:cleanGain,'new voice restores configured level');assert.equal(c.audio_stat(5),target);assert.equal(c.audio_stat(6),0);assert.equal(c.audio_stat(8),0);
   const pcm=mix(131072);if(muted)assert.equal(pcm.energy,0);else assert.ok(pcm.energy>0,'real PCM including native near-silence floor at level zero');
   rows.push({level,target,state,muted,gain,pcm});
  }
  assert.deepEqual(errors,[]);
 }finally{c.audio_close();await platform.closeServices();}
 const result={mode,rows,mixedFrames,errors,hardwareDeviceOpened:false,platformAudibleOutput:platform.summary().audibleOutput};writeFileSync(resolve(O,mode+'-result.json'),JSON.stringify(result,null,2)+'\n');return result;
}
const a=await run('current'),b=await run('upstream');assert.deepEqual(a.rows,b.rows,'all post-switch gain and full generated PCM bytes');
const q={passed:true,casesPerMode:a.rows.length,allPostSwitchGainsAndPcmHashesEqual:true,mixedFrames:a.mixedFrames+b.mixedFrames,currentSourceSha256:build.modes.current.audioSourceSha256,upstreamSourceSha256:build.modes.upstream.audioSourceSha256,sourceWasmSha256:Object.fromEntries(Object.entries(build.modes).map(([k,v])=>[k,v.wasmSha256])),hardwareDeviceOpened:false,gameplayOrBrowserAudioAccepted:false,newAssetsCopied:false,upstreamPatchAppliedToProduction:false,completeGame:false};writeFileSync(resolve(O,'qualified.json'),JSON.stringify(q,null,2)+'\n');console.log(JSON.stringify(q));
