import {WorkerNP21,workerAssistBridge,mountWorkerChoice} from './solo-worker-client.js';
import {mountSoloView} from './solo-view.js';
import {SoloMusic} from './solo-music.js';
import {FireControl} from './fire-control.js';
import {prepareSoloDisk} from './solo-save.js';
import {Fat12} from './disk.js';
import {unlockDiskScores} from './scores.js';
import {NP21} from './dosbox-adapter.js';
import {loadVerifiedBytes,loadLocalCore} from './startup-assets.js';
import {mountPlayer} from './player-ui.js';
import {unpackTouch} from './touch-input.js';
import {createSoloAssist} from './solo-assist.js';
import {mountSoloPerformance} from './solo-performance.js';
import {createBrowserSoloRecorder} from './replay/external-recorder.js';
import {decodeExternalReplay,restoreExternalReplayDisk} from './replay/external-codec.js';
import {fingerprint} from './replay/tape.js';
import {mountSoloReplayToolbar} from './replay/external-ui.js';

const $=id=>document.getElementById(id);
const audioMode=$('audio-mode');
const runtimeMode=mountWorkerChoice(audioMode.closest('label').parentElement);
let soloMusic;
window.addEventListener('pagehide',event=>{if(event.persisted)soloMusic?.suspend();else soloMusic?.dispose();});
window.addEventListener('pageshow',event=>{if(event.persisted)soloMusic?.resume();});
try{const saved=localStorage.getItem('solo-audio-mode-v2');if(['independent','original','buffered'].includes(saved))audioMode.value=saved;}catch{}
audioMode.onchange=()=>{try{localStorage.setItem('solo-audio-mode-v2',audioMode.value);}catch{}};

let emulator,lang,meta,database,dirty=0,saved=0,saveTask,timer,starting=false,assist;
let recorder=null,recordBlob=null,loadedReplay=null,playback=false;
const status=text=>{$('status').textContent=text;player.note(text);if(!cover.hidden)cover.textContent=text;};
const report=error=>{console.error(error);status(`错误：${error.message}`);};
const replaySources=['../direct-drag.js','../player-ui.js','../solo.js','../solo-worker.js','../solo-worker-client.js','../dosbox-adapter.js','../solo-assist.js','../solo-assist.json',
  '../solo-save.js','../native-pause.js','../touch-input.js','../disks/manifest.json','../vendor/dosbox-x/runtime.json','../vendor/dosbox-x/dosbox-jspi.mjs',
  '../dosbox-disk.js','../frame-presenter.js','../solo-worker-environment.js','../vendor/dosbox-x/emulators.js','../vendor/dosbox-x/wdosbox-x-fast.wasm',
  './external-host.js','./external-loader.js','./external-codec.js','./external-recorder.js','./external-ui.js','./external-lifecycle.js'];
const setReplayStatus=text=>{$('replay-status').textContent=text;};
function clearRecording(){recorder?.dispose();recorder=null;recordBlob=null;$('record-save').disabled=true;$('record-discard').disabled=true;$('record-finish').disabled=true;replayUI.closeSave();}
function applyReplayAssist(value){
  const bridge=assist?.bridge,h=emulator?.module.HEAPU8;
  const at=Number.isInteger(value.address)?value.address:bridge?.live;
  if(!h||!bridge?.valid(h,at)||new DataView(h.buffer).getUint16(at+bridge.meta.fields.generation,true)!==value.generation)return;
  const previous=bridge.live;bridge.live=at;
  try{bridge.setFocus(value.focus,0);bridge.setTouch(value.touch||[0,0]);}finally{bridge.live=previous;}
}
let canvas=$('canvas');const cover=$('cover');
const player=mountPlayer($('screen'),{solo:true,pure:true,onGesture:()=>{canvas.focus();soloMusic?.resume();emulator?.module.SDL2?.audioContext?.resume();}});
let saveWasPaused=false;
const replayUI=mountSoloReplayToolbar($('screen'),{isPlayback:()=>playback,isSynthetic:event=>synthetic.has(event),
  onHold:()=>{saveWasPaused=emulator?.state==='paused';release();emulator?.pauseReplay(true);},
  onRelease:()=>{if(!saveWasPaused&&emulator&&emulator.state!=='exited')emulator.pauseReplay(false);canvas.focus();},
  onAgain:()=>{stopPlayback();startGame(loadedReplay);},onStop:()=>stopPlayback(),onFast:value=>emulator?.fastForwardReplay(value),
});
$('screen').classList.remove('screen');
mountSoloView($('screen'));
const viewport=document.createElement('div');viewport.className='screen';viewport.append(canvas);player.stage.prepend(viewport);player.stage.append(cover);
player.setLabel('原版单人');player.setContext({key:'original:menu',play:false});
// Launch-page and in-game switches share the player's existing saved settings.
const launchFocus=$('solo-focus-enabled'),launchPoints=$('solo-focus-points');
const focusControl=$('screen').querySelector('[data-focus-enabled]');
const pointsControl=$('screen').querySelector('[data-focus-points]');
function syncLaunchAssists(){
  launchFocus.checked=focusControl.checked;launchFocus.disabled=false;
  launchPoints.checked=pointsControl.checked;launchPoints.disabled=!focusControl.checked;
}
for(const [launch,control]of [[launchFocus,focusControl],[launchPoints,pointsControl]]){
  launch.addEventListener('change',()=>{
    control.checked=launch.checked;control.dispatchEvent(new Event('change'));
  });
  control.addEventListener('change',syncLaunchAssists);
}
syncLaunchAssists();
const markerLayer=document.createElement('div');markerLayer.className='hit-points';
const marker=document.createElement('i');marker.hidden=true;markerLayer.append(marker);viewport.append(markerLayer);
mountSoloPerformance({host:$('screen'),game:'03',getEmulator:()=>emulator,readState:()=>{
  if(emulator?.isWorker){const s=emulator.snapshot.state;return s?{playing:s.phase===1,generation:s.generation,ticks:s.ticks}:null;}
  const bridge=assist?.bridge,h=emulator?.module.HEAPU8;
  if(!h||!bridge.valid(h,bridge.live))return null;
  const at=bridge.live,f=bridge.meta.fields,v=new DataView(h.buffer);
  return {playing:h[at+f.phase]===1,generation:v.getUint16(at+f.generation,true),ticks:v.getUint32(at+f.ticks,true)};
}});

async function metadata(language){
  const response=await fetch('disks/manifest.json');if(!response.ok)throw Error('镜像清单加载失败');
  return (await response.json()).games[`3-${language}`];
}
async function storage(language,mode,value){
  database ||= await new Promise((resolve,reject)=>{
    const request=indexedDB.open('th03-fast-original-solo',1);
    request.onupgradeneeded=()=>request.result.createObjectStore('disks');
    request.onsuccess=()=>resolve(request.result);request.onerror=()=>reject(request.error);
    request.onblocked=()=>reject(Error('原版存档数据库被占用'));
  });
  return new Promise((resolve,reject)=>{
    const tx=database.transaction('disks',mode),store=tx.objectStore('disks');
    const request=mode==='readonly'?store.get(language):store.put(value,language);
    tx.oncomplete=()=>resolve(request.result);tx.onerror=tx.onabort=()=>reject(tx.error||request.error);
  });
}
function validate(data,info){
  if(data.length!==info.size||new TextDecoder().decode(data.subarray(info.languageMarkerOffset,info.languageMarkerOffset+8))!==info.languageMarker)throw Error('存档大小或语言不匹配');
  // Reject multiplayer saves rather than silently booting their injected launcher.
  const signature=new TextEncoder().encode('TH03LOCALPAUSE1!');
  for(let at=data.indexOf(signature[0]);at>=0;at=data.indexOf(signature[0],at+1))
    if(signature.every((b,i)=>data[at+i]===b))throw Error('这是双人补丁存档，请导入原版单人存档');
}
async function loadDisk(){
  meta=await metadata(lang);
  try{const disk=await storage(lang,'readonly');if(disk){const data=new Uint8Array(disk);validate(data,meta);return data;}}
  catch(error){$('save-status').textContent=`存档读取失败：${error.message}`;}
  return loadVerifiedBytes(meta.url,meta.sha256,{gzip:true});
}
async function loadReplayDisk(replay){
  if(replay.disk)return replay.disk.slice();
  const info=await metadata(replay.header.language),base=await loadVerifiedBytes(info.url,info.sha256,{gzip:true});
  return restoreExternalReplayDisk(replay,base);
}
async function exportDisk(){let disk=assist.exportDisk(await emulator.getDiskImage(`3-${lang}.hdi`));if(soloMusic)disk=soloMusic.exportDisk(disk);unlockDiskScores(new Fat12(disk));return disk;}
async function save(){
  if(saveTask)return saveTask;
  if(!emulator||dirty===saved)return;
  saveTask=(async()=>{try{while(saved!==dirty){const version=dirty;
    await storage(lang,'readwrite',await exportDisk());saved=version;}
    $('save-status').textContent='原版进度已保存';
  }catch(error){$('save-status').textContent=`保存失败：${error.message}`;}finally{saveTask=null;}})();
  return saveTask;
}
$('start').onclick=()=>startGame();
async function startGame(replay=null){
  if(starting||emulator)return;starting=true;$('start').disabled=true;audioMode.disabled=true;runtimeMode.disabled=true;
  for(const id of ['language','clock','import'])$(id).disabled=true;
  playback=!!replay;
  try{
    if(replay){
      if(!['main','worker'].includes(replay.header.runtime)||replay.header.boundaryMode!=='sleep')throw Error('Replay 运行器版本不匹配');
      runtimeMode.value=replay.header.runtime;$('language').value=replay.header.language;audioMode.value='original';clearRecording();
      const identity=await fingerprint(replaySources);if(identity!==replay.header.identity)throw Error('Replay 与当前游戏或模拟器版本不一致');
    }
    lang=$('language').value;status(playback?'正在载入 Replay':'正在加载原版镜像');
    if(!playback&&audioMode.value==='independent'&&!$('record-enabled').checked)soloMusic=new SoloMusic(status);
    const [source,preparedAssist,wasmBinary]=await Promise.all([
      playback?loadReplayDisk(replay):loadDisk(),createSoloAssist(()=>emulator),loadLocalCore(),soloMusic?.prepare('YUMEZIKU'),
    ]);
    const disk=playback?source:prepareSoloDisk(source);
    assist=preparedAssist;
    if(!playback){
      try{await storage(lang,'readwrite',disk);$('save-status').textContent='全解锁存档已保存';}
      catch(error){$('save-status').textContent=`全解锁已生效，存档写入失败：${error.message}`;}
      await assist.install(disk);if(soloMusic)await soloMusic.install(disk,'YUMEZIKU');
    }
    let externalReplay=null;
    if(playback)externalReplay={mode:'playback',boundaryMode:'sleep',clocks:replay.clocks,events:replay.events,endBoundary:replay.header.endBoundary,endReason:replay.header.result.endReason,endPhase:replay.header.result.endPhase,timeline:replay.header.result.timeline,
      onEnd:value=>{
        const matches=value.memoryHash===replay.header.result.memoryHash&&value.memorySha256===replay.header.result.memorySha256;
        status(matches?'Replay 播放完成，终态内存校验一致。':'Replay 已停止：终态内存校验不一致。');
        emulator?.pauseReplay(true);$('replay-pause').disabled=true;replayUI.showEnd(matches);
      },onError:error=>{setReplayStatus('Replay 播放失败：'+error.message);emulator?.pauseReplay(true);}};
    else if($('record-enabled').checked){
      try{
        const identity=await fingerprint(replaySources);
        const baseDisk=await loadVerifiedBytes(meta.url,meta.sha256,{gzip:true});
        recorder=createBrowserSoloRecorder({game:'03',language:lang,runtime:runtimeMode.value,identity,patch:assist.bridge.meta,boundaryMode:'sleep',disk:disk.slice(),baseDisk,
          onStatus:setReplayStatus,onFailure:()=>replayUI.closeSave(),onFile:(blob,value)=>{recordBlob=blob;$('record-save').disabled=false;$('record-discard').disabled=false;replayUI.offerSave(blob,value);},
          onResult:()=>{$('record-finish').disabled=true;replayUI.prepareSave();}});
        externalReplay=recorder.config;$('record-finish').disabled=$('record-discard').disabled=false;
        setReplayStatus('等待本次首局；结束或通关时询问保存，首次续关前停止记录。');
      }catch(error){clearRecording();setReplayStatus('Replay 未能启动：'+error.message+'；游戏继续。');}
    }else setReplayStatus('本次启动不录制。');
    const config={canvas,wasmBinary,externalReplay,applyReplayInput:applyReplayAssist,
      // Independent BGM uses its own audio clock; native PCM retains sound effects.
      DIPswtch:[0x3e,0xf3,0x7b],ExMemory:7,Latencys:100,SampleHz:44100,SNDboard:4,nativeSoloAudio:!externalReplay&&audioMode.value!=='original',
      no_mouse:true,use_menu:false,fontfile:lang==='cn'?'font_cn.bmp':'font.bmp',
      onDiskChange:()=>{if(playback)return;dirty++;clearTimeout(timer);timer=setTimeout(save,1500);},
      onExit:()=>{soloMusic?.reset();release();if(!playback){recorder?.finish();save();}status('游戏已退出，可点击重启');}};
    emulator=runtimeMode.value==='worker'?await WorkerNP21.create(config,{game:'03',patch:assist.bridge.meta,music:soloMusic?.reader.patch,onError:error=>{soloMusic?.suspend();player.exit();report(error);}}):await NP21.create(config);
    recorder?.attach(emulator);
    if(emulator.isWorker){assist.bridge=workerAssistBridge(emulator);emulator.onMusic=(ax,hash)=>soloMusic?.player.command(ax,hash);}
    await emulator.addDiskImage(`3-${lang}.hdi`,disk);await emulator.setHdd(0,`3-${lang}.hdi`);
    emulator.run();cover.hidden=true;$('reset').disabled=!!(playback||recorder);$('export').disabled=playback;
    $('replay-fast').disabled=!playback;$('replay-pause').dataset.paused='false';$('replay-pause').textContent='暂停播放';$('replay-pause').disabled=!playback;$('replay-stop').disabled=!playback;
    await player.enter({native:false});canvas.focus();emulator.module.SDL2?.audioContext?.resume();status(playback?'正在重播浏览器 Replay。':'原版运行中');
  }catch(error){emulator?.dispose?.();emulator=null;recorder?.dispose();recorder=null;soloMusic?.dispose();soloMusic=null;player.exit();playback=false;report(error);$('start').textContent='刷新重试';$('start').disabled=false;$('start').onclick=()=>location.reload();}
  finally{starting=false;}
}
$('reset').onclick=async()=>{if(recorder||playback||!confirm('重启原版游戏？当前对局会结束。'))return;release();emulator.pause();await save();assist.bridge.reset();soloMusic?.reset();await emulator.reset();emulator.run();canvas.focus();status('原版运行中');};
$('export').onclick=async()=>{try{const url=URL.createObjectURL(new Blob([await exportDisk()]));
  const a=document.createElement('a');a.href=url;a.download=`th03-original-${lang}.hdi`;a.click();setTimeout(()=>URL.revokeObjectURL(url),10000);}catch(error){report(error);}};
$('import').onchange=async event=>{try{const file=event.target.files[0];if(!file||emulator||starting)return;
  const language=$('language').value,data=new Uint8Array(await file.arrayBuffer());validate(data,await metadata(language));
  if(confirm('覆盖当前语言的原版单人进度？')){await storage(language,'readwrite',prepareSoloDisk(data));status('全解锁存档已导入，默认 Lunatic');}
}catch(error){report(error);}finally{event.target.value='';}};
$('record-finish').onclick=()=>recorder?.finish();
$('record-save').onclick=()=>{if(!recordBlob)return;const url=URL.createObjectURL(recordBlob),a=document.createElement('a');a.href=url;a.download='th03-solo.th03r';a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);replayUI.closeSave();setReplayStatus('Replay 已请求保存；重新启动游戏可录制新局。');};
$('record-discard').onclick=()=>{clearRecording();$('reset').disabled=false;setReplayStatus('Replay 已丢弃，游戏继续；重新启动游戏可录制新局。');};
$('replay-file').onchange=async event=>{
  try{const file=event.target.files[0];if(!file)return;loadedReplay=await decodeExternalReplay(file,'03');
    $('replay-play').disabled=!!(emulator||starting);setReplayStatus(`已载入 ${file.name}，可开始独立重播。`);
  }catch(error){loadedReplay=null;$('replay-play').disabled=true;setReplayStatus('Replay 导入失败：'+error.message);}
};
$('replay-play').onclick=()=>{if(loadedReplay&&!emulator&&!starting)startGame(loadedReplay);};
$('replay-pause').onclick=()=>{if(!emulator||!playback)return;const paused=$('replay-pause').dataset.paused==='true';
  emulator.pauseReplay(!paused);$('replay-pause').dataset.paused=String(!paused);$('replay-pause').textContent=paused?'暂停播放':'继续播放';canvas.focus();};
function stopPlayback(){
  if(!playback)return;replayUI.reset();release();emulator?.dispose?.();emulator=null;playback=false;
  const next=canvas.cloneNode(false);canvas.replaceWith(next);canvas=next;canvas.onpointerdown=()=>canvas.focus();
  starting=false;audioMode.disabled=runtimeMode.disabled=false;for(const id of ['language','clock','import'])$(id).disabled=false;
  $('start').disabled=false;$('replay-play').disabled=!loadedReplay;
  $('replay-stop').disabled=$('replay-pause').disabled=$('replay-fast').disabled=true;$('replay-pause').textContent='暂停播放';
  $('status').textContent='Replay 已结束，可以再次播放、导入其他录像或开始游戏。';
}
$('replay-stop').onclick=stopPlayback;
window.addEventListener('pagehide',()=>{recorder?.dispose();emulator?.dispose?.();});

// Standard dragging uses stock keys; unlimited dragging is consumed by the native movement hook.
const keys={1:['ArrowUp','ArrowUp',38],2:['ArrowDown','ArrowDown',40],4:['ArrowLeft','ArrowLeft',37],8:['ArrowRight','ArrowRight',39],
  16:['KeyX','x',88],32:['KeyZ','z',90],128:['Escape','Escape',27],256:['Enter','Enter',13]};
let previous=0,dx=0,dy=0,last=performance.now(),hiddenPaused=false;
const focusKeys=new Set(),physical=new Set(),tapped=new Set(),synthetic=new WeakSet(),fire=new FireControl();
const physicalActions={KeyZ:512,KeyJ:512,ShiftLeft:32,ShiftRight:32,KeyK:32,KeyX:16,KeyL:16};
for(const type of ['keydown','keyup'])window.addEventListener(type,event=>{
  if(synthetic.has(event))return;
  const focus=event.code==='Space';
  if(!focus&&!physicalActions[event.code])return;
  event.stopImmediatePropagation();
  if(type==='keyup'){focusKeys.delete(event.code);physical.delete(event.code);return;}
  if(!emulator||player.isEditing()||event.target.closest?.('input,select,textarea,[contenteditable="true"]'))return;
  event.preventDefault();
  if(focus)focusKeys.add(event.code);
  else if(!event.repeat){physical.add(event.code);tapped.add(event.code);}
},true);
function send(bits){for(const [bit,[code,key,keyCode]] of Object.entries(keys))if(!!(previous&bit)!==!!(bits&bit)){
  const event=new KeyboardEvent(bits&bit?'keydown':'keyup',{code,key,keyCode,which:keyCode,bubbles:true});
  synthetic.add(event);canvas.dispatchEvent(event);
}previous=bits;}
function release(){emulator?.clearInput?.();send(0);dx=dy=0;focusKeys.clear();physical.clear();tapped.clear();fire.reset();player.reset();
  const bridge=assist?.bridge,state=bridge?.update(),focus=0,touch=[0,0];
  if(emulator?.isWorker){bridge?.setFocus(focus);bridge?.setTouch(touch);}
  else emulator?.assist({address:state?.at,generation:state?.generation,focus,touch});
}
function poll(now){
  const running=emulator?.state==='running'&&!document.hidden;player.setActive(running);
  const bridge=assist?.bridge,state=bridge?.update(now);
  const playing=running&&state?.phase===1;
  if(running&&!emulator.isWorker)soloMusic?.poll(emulator.module.HEAPU8,now);
  player.setContext({key:playing?`round:${state.generation}`:'original:menu',play:playing});
  let bits=0;
  if(running&&!player.isEditing()&&!playback){
    bits=player.sample();for(const code of [...physical,...tapped])bits|=physicalActions[code]||0;tapped.clear();
    const shot=fire.sample(!!(bits&512),!!(bits&32),now);bits=(bits&~544)|(shot?32:0);
    const packed=player.pack(0),touch=unpackTouch(packed);player.consume();
    const focusValue=playing&&player.focusEnabled()&&((bits&64)||focusKeys.size)?1:0;
    const touchValue=[playing?packed:0,0];
    if(emulator.isWorker){bridge?.setFocus(focusValue);bridge?.setTouch(touchValue);}
    else emulator.assist({address:state?.at,generation:state?.generation,focus:focusValue,touch:touchValue});

  }else{dx=dy=0;physical.clear();tapped.clear();fire.reset();if(!playback){if(emulator?.isWorker){bridge?.setFocus(0,0);bridge?.setTouch([0,0]);}else if(emulator)emulator.assist({address:state?.at,generation:state?.generation,focus:0,touch:[0,0]});}}
  marker.hidden=true;
  if(playing)for(const point of bridge.markers())if(point.seat===0&&(player.alwaysPoint()||point.focused&&player.focusPoints())){
    marker.hidden=false;marker.style.left=`${point.x/640*100}%`;marker.style.top=`${point.y/400*100}%`;
  }
  send(bits);last=now;requestAnimationFrame(poll);
}
requestAnimationFrame(poll);
window.addEventListener('blur',release);
document.addEventListener('visibilitychange',()=>{release();if(document.hidden){soloMusic?.suspend();hiddenPaused=emulator?.state==='running';emulator?.pause();save();}
  else if(hiddenPaused){hiddenPaused=false;emulator?.run();soloMusic?.resume();}});
window.addEventListener('beforeunload',event=>{if(dirty!==saved){event.preventDefault();event.returnValue='';}});
canvas.addEventListener('pointerdown',()=>canvas.focus());
