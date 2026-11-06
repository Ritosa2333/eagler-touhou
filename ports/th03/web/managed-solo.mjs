import {WorkerNP21} from './solo-worker-client.js';
import {SoloMusic} from './solo-music.js';
import {OriginalSfx} from './original-sfx.mjs';
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
let emulator,assist,soloMusic,localSfx,recorder,lang,previous=0,loop=0,playback=false,pauseState=null,pauseTransition=false,guestPaused=false,pauseRequestHandled=false;
const fire=new FireControl(),writes=new Set();
const report=error=>globalThis.__eaglerPC98Emit({event:'error',message:String(error?.message||error)});
function pausePointer(message){
  const state=emulator?.snapshot.state;if(state?.phase!==2)return false;
  if(message.type==='up'&&message.x>=176/640&&message.x<464/640){
    const row=Math.floor((message.y*400-112)/46),inside=(message.y*400-112)-row*46;
    if(row>=0&&row<4&&inside>=0&&inside<40)emulator.send('soloPauseSelect',[state.generation,state.at,row]);
  }
  return true;
}
function canvasPausePointer(event){
  if(event.pointerType!=='mouse')return;
  const box=canvas.getBoundingClientRect();pausePointer({type:'up',x:(event.clientX-box.left)/box.width,y:(event.clientY-box.top)/box.height});
}
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
function transition(save,restart=false){
  if(pauseTransition)return;pauseTransition=true;const current=emulator;
  void returnToTitle(save,restart,current).catch(async error=>{
    if(emulator===current){await current.call('soloPauseCancel',[pauseState?.generation,pauseState?.at]).catch(()=>{});report(error);}
  }).finally(()=>{pauseTransition=false;});
}
function tick(now){
  if(!emulator)return;
  const state=emulator.snapshot.state,paused=state?.phase===2;
  if(!paused||!state.command)pauseRequestHandled=false;
  const sample=globalThis.__eaglerPC98Input.sample({play:state?.phase===1});
  if(paused!==guestPaused){guestPaused=paused;if(paused){soloMusic?.suspend();void localSfx?.context.suspend();}else{soloMusic?.resume();void localSfx?.resume().catch(()=>{});}}
  if(paused&&state.command&&!pauseTransition&&!pauseRequestHandled){pauseRequestHandled=true;pauseState=state;send(0);transition(state.command===1,state.command===3);}
  if(emulator.state==='running'&&!playback){
    const playing=state?.phase===1,input=globalThis.__eaglerPC98Input;
    let bits=sample;
    const shot=fire.sample(!!(bits&512),!!(bits&32),now);
    bits=(bits&~544)|(shot?32:0);
    send(bits);
    emulator.setInput({generation:state?.generation,focus:playing&&!!(bits&64)?1:0,
      touch:[playing?packTouch(0,{...input.motion(),alwaysPoint:!!globalThis.__eaglerPC98Options().alwaysHitbox}):0,0]});
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
async function returnToTitle(save,restart=false,current=emulator){
  const recording=recorder;if(recording?.state.failed&&!save){recording.dispose();recorder=undefined;}else if(recording){recording.finish();const until=performance.now()+10000;while(emulator===current&&!recording.state.ready&&!recording.state.failed&&performance.now()<until)await new Promise(resolve=>setTimeout(resolve,25));if(emulator!==current)return;if(!recording.state.ready)throw Error('录像未能完成');if(save)await managedAPI.saveRecording();if(emulator!==current)return;await recording.rearm();}
  if(emulator!==current)return;const generation=pauseState?.generation;
  await syncFiles();if(emulator!==current)return;if(!await emulator.call('soloQuit',[generation,restart,pauseState?.at]))throw Error('当前对局无法退出');if(emulator!==current)return;
  globalThis.__eaglerPC98Input.clear();emulator.run();canvas.focus();
}
async function naturalExit(){
  soloMusic?.reset();
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
  const disk=playback?(replay.disk?replay.disk.slice():await restoreExternalReplayDisk(replay,base)):prepareSoloDisk(base.slice());
  assist=await createSoloAssist(()=>emulator);
  if(!playback){
    const fat=new Fat12(disk);
    for(const [name,bytes] of globalThis.__eaglerPC98Files.memory)if(['YUME.NEM','YUME.CFG'].includes(name))fat.put('YUMEZIKU/'+name,bytes);
    await assist.install(disk);
    soloMusic=new SoloMusic(report,{independentSfx:true});
    soloMusic.player.pmd={tracks:originalTracks,engine:await PmdPlayer.prepare(soloMusic.context,import.meta.url)};
    [localSfx]=await Promise.all([OriginalSfx.create('th03',new Fat12(base)),soloMusic.prepare('YUMEZIKU')]);
    await soloMusic.install(disk,'YUMEZIKU');
  }else if(new Fat12(disk).find('YUMEZIKU/WEBMUSIC.COM',{optional:true})!==undefined){
    const fat=new Fat12(disk),independentSfx=fat.read(fat.find('YUMEZIKU/WEBMUSIC.COM')).length===883;
    soloMusic=new SoloMusic(report,{independentSfx});
    soloMusic.player.pmd={tracks:originalTracks,engine:await PmdPlayer.prepare(soloMusic.context,import.meta.url)};
    if(independentSfx)localSfx=await OriginalSfx.create('th03',new Fat12(base));
    await soloMusic.attachReplay('YUMEZIKU');
  }
  if(music==='none')soloMusic?.player.setVolume(0);
  let externalReplay;
  if(playback){
    externalReplay={mode:'playback',boundaryMode:'sleep',clocks:replay.clocks,events:replay.events,endBoundary:replay.header.endBoundary,
      endReason:replay.header.result.endReason,endPhase:replay.header.result.endPhase,timeline:replay.header.result.timeline,memoryScope:replay.header.result.memoryScope,
      onEnd:value=>{emulator?.pauseReplay(true);if(value.memoryHash!==replay.header.result.memoryHash||value.memorySha256!==replay.header.result.memorySha256)report(Error('Replay 终态内存校验不一致'));},onError:report};
  }else{
    recorder=createBrowserSoloRecorder({game:'03',language:lang,runtime:'worker',identity:await fingerprint(replaySources),patch:assist.bridge.meta,memoryScope:'pc98-guest-ram/1',reuse:true,
      boundaryMode:'sleep',disk:disk.slice(),baseDisk:base,deferEncoding:true,onStatus:()=>{},onFailure:report,onResult:()=>{}});
    externalReplay=recorder.config;
  }
  const config={canvas,exitOnGameQuit:true,wasmBinary:await loadLocalCore(),externalReplay,DIPswtch:[0x3e,0xf3,0x7b],ExMemory:7,Latencys:100,
    SampleHz:44100,SNDboard:4,nativeSoloAudio:false,independentSfx:!!localSfx,no_mouse:true,use_menu:false,fontfile:lang==='cn'?'font_cn.bmp':'font.bmp',
    onDiskChange:()=>{},onExit:()=>void naturalExit().catch(report)};
  emulator=await WorkerNP21.create(config,{game:'03',patch:assist.bridge.meta,music:soloMusic?.reader.patch,onError:report});
  globalThis.__eaglerPC98Input.menuPointer=pausePointer;canvas.addEventListener('pointerup',canvasPausePointer);
  recorder?.attach(emulator);
  emulator.onMusic=(ax,hash)=>{if((ax>>>8)===0x0c)localSfx?.command(ax);else soloMusic?.player.command(ax,hash);};
  await emulator.addDiskImage(`3-${lang}.hdi`,disk);await emulator.setHdd(0,`3-${lang}.hdi`);
  emulator.run();canvas.focus();loop=requestAnimationFrame(tick);
}
export const managedAPI={
  start,
  health(){return {gameFrame:emulator?.snapshot?.state?.ticks,frames:emulator?.module.frameUploads??0,audio:{state:emulator?.context?.state,music:soloMusic?.player.active?.id??null,musicStatus:soloMusic?.player.status??null,
    effectsPlayed:localSfx?.played??0,effectsPeak:localSfx?.peak??0,queuedMs:Math.max(0,((emulator?.audioEnd??0)-(emulator?.context?.currentTime??0))*1000)}};},
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
  async saveRecording(){if(!recorder?.state.ready)return null;return saveReplay(await recorder.save());},
  finishRecording(){recorder?.finish();},
  playback(message){if(emulator)throw Error('请先结束本局');return start({language:'ja',music:'ogg',options:{replayPath:message.path}});},
  stop(){cancelAnimationFrame(loop);previous=0;pauseState=null;pauseTransition=guestPaused=pauseRequestHandled=false;delete globalThis.__eaglerPC98Input.menuPointer;canvas.removeEventListener('pointerup',canvasPausePointer);fire.reset();recorder?.dispose();localSfx?.dispose();soloMusic?.dispose();emulator?.dispose();emulator=undefined;
    recorder=localSfx=soloMusic=undefined;const next=canvas.cloneNode(false);canvas.replaceWith(next);canvas=next;},
};
