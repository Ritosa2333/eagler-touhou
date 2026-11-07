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
import {downloadReplay} from './replay/download.js';

let canvas=document.getElementById('canvas');
const replaySources=['../direct-drag.js','../player-ui.js','../solo.js','../solo-worker.js','../solo-worker-client.js','../dosbox-adapter.js','../solo-assist.js','../solo-assist.json',
  '../solo-save.js','../native-pause.js','../touch-input.js','../disks/manifest.json','../vendor/dosbox-x/runtime.json','../vendor/dosbox-x/dosbox-jspi.mjs',
  '../dosbox-disk.js','../frame-presenter.js','../solo-worker-environment.js','../vendor/dosbox-x/emulators.js','../vendor/dosbox-x/wdosbox-x-fast.wasm',
  './external-host.js','./external-loader.js','./external-codec.js','./external-recorder.js','./external-ui.js','./external-lifecycle.js'];
const keys={1:['ArrowUp','ArrowUp',38],2:['ArrowDown','ArrowDown',40],4:['ArrowLeft','ArrowLeft',37],8:['ArrowRight','ArrowRight',39],
  16:['KeyX','x',88],32:['KeyZ','z',90],128:['Escape','Escape',27],256:['Enter','Enter',13]};
const versusKeys=[{1:'KeyT',2:'KeyB',4:'KeyF',8:'KeyH',16:'KeyX',32:'KeyZ'},
  {1:'Numpad8',2:'Numpad2',4:'Numpad4',8:'Numpad6',16:'ArrowRight',32:'ArrowLeft'}];
let emulator,assist,soloMusic,localSfx,recorder,battleReplay,viewerSettings,playbackPath,playbackDescription,lang,previous=0,loop=0,playback=false,pauseState=null,pauseTransition=false,guestPaused=false,pauseRequestHandled=false,scoreOffer=null,lastReplayStatus=0,lastReplayKey='';
const fire=new FireControl(),secondFire=new FireControl();let versusHeld=new Set(),lastVersus=false;
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
  downloadReplay(blob,`th03-${Date.now()}.th03r`);
}
function scoreSaved(){
  const recording=recorder,current=emulator;
  if(!recording||scoreOffer||!recording.state.ready)return;
  scoreOffer=(async()=>{
    if(window.confirm('是否保存本局 Replay？'))await saveReplay(await recording.save());
    if(emulator===current&&recorder===recording)await recording.rearm();
  })().catch(report).finally(()=>{scoreOffer=null;});
}
function send(bits){
  for(const [mask,[code,key,keyCode]] of Object.entries(keys))if(!!(previous&mask)!==!!(bits&mask)){
    emulator.send('key',[{type:bits&mask?'keydown':'keyup',code,key,keyCode,which:keyCode}]);
  }
  previous=bits;
}
function releaseInput(){
  globalThis.__eaglerPC98Input.clear();send(0);for(const code of versusHeld)emulator?.send('key',[{type:'keyup',code}]);versusHeld.clear();emulator?.clearInput();fire.reset();secondFire.reset();
}
function onVisibilityChange(){if(document.hidden)releaseInput();}
function transition(save,restart=false){
  if(pauseTransition)return;pauseTransition=true;const current=emulator;
  void returnToTitle(save,restart,current).catch(async error=>{
    if(emulator===current){await current.call('soloPauseCancel',[pauseState?.generation,pauseState?.at]).catch(()=>{});report(error);}
  }).finally(()=>{pauseTransition=false;});
}
function tick(now){
  if(!emulator)return;
  const state=emulator.snapshot.state,paused=state?.phase===2;
  const versus=state?.phase===3||emulator.snapshot.selecting;if(versus!==lastVersus){releaseInput();lastVersus=versus;}
  if(!paused||!state.command)pauseRequestHandled=false;
  if(paused!==guestPaused){guestPaused=paused;releaseInput();if(paused){soloMusic?.suspend();void localSfx?.context.suspend();}else{soloMusic?.resume();void localSfx?.resume().catch(()=>{});}}
  const sample=globalThis.__eaglerPC98Input.sample({play:state?.phase===1});
  if(!playback&&paused&&state.command&&!pauseTransition&&!pauseRequestHandled){pauseRequestHandled=true;pauseState=state;send(0);transition(state.command===1,state.command===3);}
  if(emulator.state==='running'&&!playback){
    const playing=state?.phase===1,input=globalThis.__eaglerPC98Input;
    let bits=sample;
    const shot=fire.sample(!!(bits&512),!!(bits&32),now);
    bits=(bits&~544)|(shot?32:0);
    let focus=playing&&!!(bits&64)?1:0;
    if(versus){
      send(state?.phase===3?bits&128:0);let second=input.localSecond();if(bits&64)focus|=1;if(second&64)focus|=2;second=(second&~544)|(secondFire.sample(!!(second&512),!!(second&32),now)?32:0);
      const next=new Set();for(const [slot,value] of [bits,second].entries())for(const [mask,code] of Object.entries(versusKeys[slot]))if(value&mask)next.add(code);
      for(const code of new Set([...versusHeld,...next]))if(versusHeld.has(code)!==next.has(code))emulator.send('key',[{type:next.has(code)?'keydown':'keyup',code}]);
      versusHeld=next;
    }else send(bits);
    const settings=globalThis.__eaglerPC98Options();if(settings.th03FocusEnabled===false)focus=0;
    emulator.setInput({generation:state?.generation,focus,
      touch:[playing||state?.phase===3?packTouch(0,{...(playing?input.motion():{}),alwaysPoint:!!settings.alwaysHitbox}):0,0]});
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
  const recording=recorder;if(recording?.state.failed){recording.dispose();recorder=undefined;}else if(recording){recording.finish();const until=performance.now()+10000;while(emulator===current&&!recording.state.ready&&!recording.state.failed&&performance.now()<until)await new Promise(resolve=>setTimeout(resolve,25));if(emulator!==current)return;if(recording.state.failed){recording.dispose();recorder=undefined;}else if(!recording.state.ready)throw Error('录像未能完成');else if(!save)await recording.rearm();}
  if(emulator!==current)return;const generation=pauseState?.generation;
  await syncFiles();if(emulator!==current)return;if(!await emulator.call('soloQuit',[generation,restart,pauseState?.at,save&&!restart]))throw Error('当前对局无法退出');if(emulator!==current)return;
  globalThis.__eaglerPC98Input.clear();emulator.run();canvas.focus();
}
async function naturalExit(){
  soloMusic?.reset();
  if(scoreOffer)await scoreOffer;
  try{await syncFiles();}finally{globalThis.__eaglerPC98Emit({event:'exit',code:0});}
}
async function start({language,music,options}){
  if(options.replayViewer&&!options.replayPath){viewerSettings={language,music};playback=true;return;}
  lang=language==='ja'?'jp':'cn';playback=!!options.replayPath;
  let replay;
  if(playback){
    const bytes=globalThis.__eaglerPC98Files.memory.get(userPath(options.replayPath));
    if(!bytes)throw Error('录像不存在');
    if(new TextDecoder().decode(bytes.subarray(0,8))==='P98TAPE1'){
      const [{decodeTape},{startBattleReplay}]=await Promise.all([import('./replay/tape.js'),import('./managed-battle-replay.mjs')]);
      const tape=await decodeTape(new Blob([bytes]));if(tape.header.game!=='03'||tape.header.players!==2)throw Error('录像作品或模式不匹配');
      battleReplay=await startBattleReplay({tape,canvas,music,onError:report,onStatus:state=>publishBattleStatus(state)});return;
    }
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
  const config={canvas,managedKeyboard:true,exitOnGameQuit:true,wasmBinary:await loadLocalCore(),externalReplay,DIPswtch:[0x3e,0xf3,0x7b],ExMemory:7,Latencys:100,
    SampleHz:44100,SNDboard:4,nativeSoloAudio:false,independentSfx:!!localSfx,no_mouse:true,use_menu:false,fontfile:lang==='cn'?'font_cn.bmp':'font.bmp',
    onDiskChange:()=>{},onExit:()=>void naturalExit().catch(report)};
  emulator=await WorkerNP21.create(config,{game:'03',patch:assist.bridge.meta,music:soloMusic?.reader.patch,onError:report});
  emulator.onScoreSaved=scoreSaved;
  globalThis.__eaglerPC98Input.menuPointer=pausePointer;canvas.addEventListener('pointerup',canvasPausePointer);window.addEventListener('blur',releaseInput);document.addEventListener('visibilitychange',onVisibilityChange);
  recorder?.attach(emulator);
  emulator.onMusic=(ax,hash)=>{if((ax>>>8)===0x0c)localSfx?.command(ax);else soloMusic?.player.command(ax,hash);};
  await emulator.addDiskImage(`3-${lang}.hdi`,disk);await emulator.setHdd(0,`3-${lang}.hdi`);
  emulator.run();canvas.focus();loop=requestAnimationFrame(tick);
}
export const managedAPI={
  start,
  health(){if(battleReplay)return battleReplay.health();if(viewerSettings&&!emulator)return {frames:1,audio:{state:'running'}};return {gameFrame:emulator?.snapshot?.state?.ticks,frames:emulator?.module.frameUploads??0,audio:{state:emulator?.context?.state,music:soloMusic?.player.active?.id??null,musicStatus:soloMusic?.player.status??null,
    effectsPlayed:localSfx?.played??0,effectsPeak:localSfx?.peak??0,queuedMs:Math.max(0,((emulator?.audioEnd??0)-(emulator?.context?.currentTime??0))*1000)}};},
  async files(message){
    const store=globalThis.__eaglerPC98Files;
    if(message.command==='sync'){await syncFiles();return {};}
    if(message.command==='list'){await syncFiles();return {files:[...store.memory].map(([path,bytes])=>({path,size:bytes.byteLength}))};}
    const path=userPath(message.path);
    if(message.command==='read'){await syncFiles();const bytes=store.memory.get(path);if(!bytes)throw Error('文件不存在');return {bytes:Array.from(bytes)};}
    if(emulator||battleReplay)throw Error('请退出游戏后管理文件');
    if(message.command==='remove'){await store.remove(path);return {};}
    if(message.command==='write'){await store.write(path,new Uint8Array(message.bytes));return {};}
    throw Error('用户文件操作无效');
  },
  async saveRecording(){if(!recorder?.state.ready)return null;return saveReplay(await recorder.save());},
  finishRecording(){recorder?.finish();},
  async playback(message){
    if(!viewerSettings)throw Error('请先打开录像工作台');
    if(message.action==='inspect'||message.action==='open'){
      const path=userPath(message.path),bytes=globalThis.__eaglerPC98Files.memory.get(path);if(!bytes)throw Error('录像不存在');
      const blob=new Blob([bytes]),battle=new TextDecoder().decode(bytes.subarray(0,8))==='P98TAPE1';
      const value=battle?await (await import('./replay/tape.js')).decodeTape(blob):await decodeExternalReplay(blob,'03');
      if(value.header.game!=='03')throw Error('录像作品不匹配');
      const difficulty=battle&&Number.isInteger(Number(value.header.settings.difficulty))?Number(value.header.settings.difficulty):-1;
      const description={name:'',character:-1,difficulty,stages:[{id:0,label:'Replay',score:null,...(battle?{frames:value.rows.length-value.matchStart}:{})}]};
      if(message.action==='inspect')return {description};
      const settings=viewerSettings;managedAPI.stop();viewerSettings=settings;playbackPath=path;playbackDescription=description;
      await start({...settings,options:{replayPath:path}});
      if(battleReplay)publishBattleStatus(battleReplay.status(),true);
      return {};
    }
    if(battleReplay){battleReplay.control(message.action,message.value);return {};}
    if(emulator&&['pause','play'].includes(message.action)){emulator.pauseReplay(message.action==='pause');return {};}
    throw Error('录像操作无效');
  },
  stop(){cancelAnimationFrame(loop);releaseInput();previous=0;lastVersus=false;pauseState=null;pauseTransition=guestPaused=pauseRequestHandled=false;delete globalThis.__eaglerPC98Input.menuPointer;canvas.removeEventListener('pointerup',canvasPausePointer);window.removeEventListener('blur',releaseInput);document.removeEventListener('visibilitychange',onVisibilityChange);fire.reset();recorder?.dispose();localSfx?.dispose();soloMusic?.dispose();emulator?.dispose();emulator=undefined;
    battleReplay?.stop();battleReplay=undefined;viewerSettings=playbackPath=playbackDescription=undefined;lastReplayStatus=0;lastReplayKey='';recorder=localSfx=soloMusic=undefined;const next=canvas.cloneNode(false);canvas.replaceWith(next);canvas=next;},
};
function publishBattleStatus(state,force=false){
  if(!playbackPath)return;
  const now=performance.now(),key=[state.paused,state.seeking,state.target,state.rate,state.ended].join(':');
  if(!force&&key===lastReplayKey&&now-lastReplayStatus<250)return;lastReplayStatus=now;lastReplayKey=key;
  globalThis.__eaglerPC98Emit({event:'runtime-info',playback:{kind:'replay',path:playbackPath,description:playbackDescription,difficulty:playbackDescription.difficulty,left:-1,right:-1,stage:0,...state}});
}
