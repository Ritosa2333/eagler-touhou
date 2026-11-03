import {WorkerNP21} from './solo-worker-client.js';
import {SoloMusic} from './solo-music.js';
import {LocalSfx} from './local-sfx.js';
import {FireControl} from './fire-control.js';
import {prepareSoloDisk} from './solo-save.js';
import {Fat12} from './disk.js';
import {loadVerifiedBytes,loadLocalCore} from './startup-assets.js';
import {createSoloAssist} from './solo-assist.js';
import {packTouch} from './touch-input.js';
import {createBrowserSoloRecorder} from './replay/external-recorder.js';
import {decodeExternalReplay,restoreExternalReplayDisk} from './replay/external-codec.js';
import {fingerprint} from './replay/tape.js';
import {userPath} from './managed-storage.mjs';
import {extractOriginalPmd} from './pmd-archives.mjs';
import {PmdPlayer} from './pmd-player.mjs';

let canvas=document.getElementById('canvas');
const replaySources=['../direct-drag.js','../player-ui.js','../solo.js','../solo-worker.js','../solo-worker-client.js','../dosbox-adapter.js','../solo-assist.js','../solo-assist.json',
  '../solo-save.js','../native-pause.js','../touch-input.js','../disks/manifest.json','../vendor/dosbox-x/runtime.json','../vendor/dosbox-x/dosbox-jspi.mjs',
  '../dosbox-disk.js','../frame-presenter.js','../solo-worker-environment.js','../vendor/dosbox-x/emulators.js','../vendor/dosbox-x/wdosbox-x-fast.wasm',
  './external-host.js','./external-loader.js','./external-codec.js','./external-recorder.js','./external-ui.js','./external-lifecycle.js'];
const keys={1:['ArrowUp','ArrowUp',38],2:['ArrowDown','ArrowDown',40],4:['ArrowLeft','ArrowLeft',37],8:['ArrowRight','ArrowRight',39],
  16:['KeyX','x',88],32:['KeyZ','z',90],128:['Escape','Escape',27],256:['Enter','Enter',13]};
let emulator,assist,soloMusic,localSfx,recorder,lang,previous=0,loop=0,playback=false;
const fire=new FireControl(),writes=new Set();
const report=error=>globalThis.__eaglerPC98Emit({event:'error',message:String(error?.message||error)});
async function saveReplay(blob){
  const task=globalThis.__eaglerPC98Files.write(`replay/th03_${Date.now()}_${crypto.randomUUID().slice(0,8)}.th03r`,await blob.arrayBuffer());
  writes.add(task);try{await task;}finally{writes.delete(task);}
}
function send(bits){
  for(const [mask,[code,key,keyCode]] of Object.entries(keys))if(!!(previous&mask)!==!!(bits&mask)){
    emulator.send('key',[{type:bits&mask?'keydown':'keyup',code,key,keyCode,which:keyCode}]);
  }
  previous=bits;
}
function tick(now){
  if(!emulator)return;
  if(emulator.state==='running'&&!playback){
    const state=emulator.snapshot.state,playing=state?.phase===1,input=globalThis.__eaglerPC98Input;
    let bits=input.sample();
    const shot=fire.sample(!!(bits&512),!!(bits&32),now);
    bits=(bits&~544)|(shot?32:0);
    send(bits);
    emulator.setInput({generation:state?.generation,focus:playing&&!!(bits&64)?1:0,
      touch:[playing?packTouch(0,input.motion()):0,0]});
  }
  loop=requestAnimationFrame(tick);
}
async function syncFiles(){
  if(!emulator||playback)return;
  const fat=new Fat12(await emulator.getDiskImage(`3-${lang}.hdi`));
  for(const name of ['YUME.NEM','YUME.CFG']){
    const entry=fat.find('YUMEZIKU/'+name,{optional:true});
    if(entry!==undefined)await globalThis.__eaglerPC98Files.write(name,fat.read(entry));
  }
}
async function naturalExit(){
  soloMusic?.reset();recorder?.finish();
  const deadline=performance.now()+10000;
  while(recorder?.state.ending&&!recorder.state.ready&&!recorder.state.failed&&performance.now()<deadline)await new Promise(resolve=>setTimeout(resolve,50));
  await Promise.allSettled([...writes]);
  try{await syncFiles();}finally{globalThis.__eaglerPC98Emit({event:'exit',code:0});}
}
async function start({language,music,options}){
  lang=language==='ja'?'jp':'cn';playback=!!options.replayPath;
  let replay;
  if(playback){
    const bytes=globalThis.__eaglerPC98Files.memory.get(userPath(options.replayPath));
    if(!bytes)throw Error('录像不存在');
    replay=await decodeExternalReplay(new Blob([bytes]),'03');
    if(replay.header.identity!==await fingerprint(replaySources))throw Error('Replay 与当前游戏或模拟器版本不一致');
    lang=replay.header.language;
  }
  const info=(await(await fetch('disks/manifest.json')).json()).games[`3-${lang}`];
  const base=await loadVerifiedBytes(info.url,info.sha256,{gzip:true});
  const originalTracks=extractOriginalPmd('th03',new Fat12(base));
  const disk=playback?(replay.disk?replay.disk.slice():restoreExternalReplayDisk(replay,base)):prepareSoloDisk(base.slice());
  assist=await createSoloAssist(()=>emulator);
  if(!playback){
    const fat=new Fat12(disk);
    for(const [name,bytes] of globalThis.__eaglerPC98Files.memory)if(['YUME.NEM','YUME.CFG'].includes(name))fat.put('YUMEZIKU/'+name,bytes);
    await assist.install(disk);
    soloMusic=new SoloMusic(report,{independentSfx:true});
    soloMusic.player.pmd={tracks:originalTracks,engine:await PmdPlayer.prepare(soloMusic.context,import.meta.url)};
    localSfx=new LocalSfx(soloMusic.context);
    await Promise.all([soloMusic.prepare('YUMEZIKU'),localSfx.load()]);
    await soloMusic.install(disk,'YUMEZIKU');
  }else if(new Fat12(disk).find('YUMEZIKU/WEBMUSIC.COM',{optional:true})!==undefined){
    const fat=new Fat12(disk),independentSfx=fat.read(fat.find('YUMEZIKU/WEBMUSIC.COM')).length===883;
    soloMusic=new SoloMusic(report,{independentSfx});
    soloMusic.player.pmd={tracks:originalTracks,engine:await PmdPlayer.prepare(soloMusic.context,import.meta.url)};
    if(independentSfx)localSfx=new LocalSfx(soloMusic.context);
    await Promise.all([soloMusic.attachReplay('YUMEZIKU'),localSfx?.load()]);
  }
  if(music==='none')soloMusic?.player.setVolume(0);
  let externalReplay;
  if(playback){
    externalReplay={mode:'playback',boundaryMode:'sleep',clocks:replay.clocks,events:replay.events,endBoundary:replay.header.endBoundary,
      endReason:replay.header.result.endReason,endPhase:replay.header.result.endPhase,timeline:replay.header.result.timeline,
      onEnd:value=>{emulator?.pauseReplay(true);if(value.memoryHash!==replay.header.result.memoryHash||value.memorySha256!==replay.header.result.memorySha256)report(Error('Replay 终态内存校验不一致'));},onError:report};
  }else{
    recorder=createBrowserSoloRecorder({game:'03',language:lang,runtime:'worker',identity:await fingerprint(replaySources),patch:assist.bridge.meta,
      boundaryMode:'sleep',disk:disk.slice(),baseDisk:base,onStatus:()=>{},onFailure:()=>{},onFile:blob=>void saveReplay(blob).catch(report),onResult:()=>{}});
    externalReplay=recorder.config;
  }
  const config={canvas,wasmBinary:await loadLocalCore(),externalReplay,DIPswtch:[0x3e,0xf3,0x7b],ExMemory:7,Latencys:100,
    SampleHz:44100,SNDboard:4,nativeSoloAudio:false,independentSfx:!!localSfx,no_mouse:true,use_menu:false,fontfile:lang==='cn'?'font_cn.bmp':'font.bmp',
    onDiskChange:()=>{},onExit:()=>void naturalExit().catch(report)};
  emulator=await WorkerNP21.create(config,{game:'03',patch:assist.bridge.meta,music:soloMusic?.reader.patch,onError:report});
  recorder?.attach(emulator);
  emulator.onMusic=(ax,hash)=>{if((ax>>>8)===0x0c)localSfx?.command(ax);else soloMusic?.player.command(ax,hash);};
  await emulator.addDiskImage(`3-${lang}.hdi`,disk);await emulator.setHdd(0,`3-${lang}.hdi`);
  emulator.run();canvas.focus();loop=requestAnimationFrame(tick);
}
export const managedAPI={
  start,
  health(){return {frames:emulator?.module.frameUploads??0,audio:{state:emulator?.context?.state,music:soloMusic?.player.active?.id??null,musicStatus:soloMusic?.player.status??null,
    effectsPlayed:localSfx?.played??0,queuedMs:Math.max(0,((emulator?.audioEnd??0)-(emulator?.context?.currentTime??0))*1000)}};},
  async files(message){
    const store=globalThis.__eaglerPC98Files;
    if(message.command==='sync'){await syncFiles();return {};}
    if(message.command==='list'){await syncFiles();return {files:[...store.memory].map(([path,bytes])=>({path,size:bytes.byteLength}))};}
    const path=userPath(message.path);
    if(message.command==='read'){await syncFiles();const bytes=store.memory.get(path);if(!bytes)throw Error('文件不存在');return {bytes:Array.from(bytes)};}
    if(emulator)throw Error('请退出游戏后管理文件');
    if(message.command==='remove'){await store.remove(path);return {};}
    if(message.command==='write'){await store.write(path,new Uint8Array(message.bytes));return {};}
    throw Error('用户文件操作无效');
  },
  playback(message){if(emulator)throw Error('请先结束本局');return start({language:'ja',music:'ogg',options:{replayPath:message.path}});},
  stop(){cancelAnimationFrame(loop);previous=0;fire.reset();recorder?.dispose();localSfx?.dispose();soloMusic?.dispose();emulator?.dispose();emulator=undefined;
    recorder=localSfx=soloMusic=undefined;const next=canvas.cloneNode(false);canvas.replaceWith(next);canvas=next;},
};
