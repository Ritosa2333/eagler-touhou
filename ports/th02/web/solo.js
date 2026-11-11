import {createSoloAttempt} from './practice-attempt.js';
import {mountPlayer} from './player-ui.js';
import {mountControlSettings,keyboardBits,padBits,isBound,isFormTarget,controlsSummary} from './control-settings.js';
import {mountFocusSettings} from './netplay/focus-display.js';
import {unpackTouch} from './touch-input.js';
import {decodeExternalReplay} from './replay/external-codec.js';
import {mountSoloReplayToolbar} from './replay/external-ui.js';
import {mountPractice} from './practice-ui.js';
import {normalizePractice} from './practice-config.js';

const $=id=>document.getElementById(id),keys=new Set();
const nativeKeys=[['ArrowUp',1],['ArrowDown',2],['ArrowLeft',4],['ArrowRight',8],['KeyX',16],['KeyZ',32],['Escape',128],['Enter',256]];
const residentSignature=new TextEncoder().encode('MIKOConfig\0');
let emulator=null,music=null,patch=null,mailbox=-1,cursor=0,lastKeys=0,focused=document.hasFocus(),editing=false,launching=false,hiddenPause=false;
let recorder=null,recordBlob=null,loadedReplay=null,playback=false,lastAssist='';
let activeAttempt=null,retrySlot=null,retryBlocked=false,warmAfter=0,lastRetry=null,disposed=false;
let committedPractice=null,practiceCompleted=false;
const status=text=>{$('status').textContent=text;};
const host=document.createElement('div');$('game').append(host);
host.hidden=true;
const player=mountPlayer(host,{solo:true,onGesture:()=>{emulator?.audio.context.resume().catch(()=>{});music?.resume();},onChange:()=>syncInput()});
let saveWasPaused=false;
const replayUI=mountSoloReplayToolbar(host,{isPlayback:()=>playback,isSynthetic:()=>false,
  onHold:()=>{saveWasPaused=emulator?.state==='paused';release();emulator?.pauseReplay(true);},
  onRelease:()=>{if(!saveWasPaused&&emulator&&emulator.state!=='exited')emulator.pauseReplay(false);canvas.focus();},
  onAgain:()=>{stopPlayback();startGame(loadedReplay);},onStop:()=>stopPlayback(),onFast:value=>emulator?.fastForwardReplay(value),
});
const screen=document.createElement('div');screen.className='screen solo-screen';
let canvas=document.createElement('canvas');canvas.width=640;canvas.height=400;canvas.tabIndex=0;canvas.setAttribute('aria-label','东方封魔录单人游戏画面');screen.append(canvas);player.stage.prepend(screen);
const practiceUI=mountPractice({host,onRelease:release,onFocus:()=>canvas.focus(),onChange:options=>{if(!playback)emulator?.practiceOptions(options);if(options.preparedRetry===false)discardRetry();if(options.preparedRetry===true)retryBlocked=false;},
  onStart:config=>restartPractice(config),onRetry:()=>{if(committedPractice?.mode==='target')restartPractice({...committedPractice,options:practiceUI.options},true);},onSave:()=>$('record-save').click(),
  onPause:()=>{release();if(readSoloState()?.mode===1){emulator.key('Escape',true);setTimeout(()=>emulator?.key('Escape',false),120);}}});
const practiceOpen=document.createElement('button');practiceOpen.type='button';practiceOpen.id='practice-open';practiceOpen.textContent='特定练习 · F12';$('start').after(practiceOpen);
practiceOpen.onclick=()=>{host.hidden=false;practiceUI.open();};
player.setLabel('单人游戏');
mountControlSettings($('control-settings'),{solo:true,onChange:()=>{$('controls-summary').textContent=controlsSummary('solo');release();},onEditing:value=>{editing=value;release();}});
const pointMask=mountFocusSettings($('control-settings'),{profile:'solo',alwaysPointControl:player.alwaysPointControl});
$('controls-summary').textContent=controlsSummary('solo');
const volume=document.createElement('label');volume.className='player-volume';volume.innerHTML='BGM <input type="range" min="0" max="100" value="70" aria-label="音乐音量">';$('control-settings').append(volume);
volume.querySelector('input').oninput=event=>music?.player.setVolume(Number(event.target.value)/100);
try{
  for(const id of ['language','audio-mode','gamepad']){const saved=localStorage.getItem('th02.solo.'+id);if(saved&&[...$(id).options].some(o=>o.value===saved))$(id).value=saved;}
  $('focus-enabled').checked=localStorage.getItem('th02.solo.focus-enabled')!=='false';
  localStorage.removeItem('th02.solo.focus-percent');
}catch{}
for(const id of ['language','audio-mode','gamepad'])$(id).addEventListener('change',()=>{try{localStorage.setItem('th02.solo.'+id,$(id).value);}catch{};release();});
$('focus-enabled').addEventListener('change',()=>{try{localStorage.setItem('th02.solo.focus-enabled',String($('focus-enabled').checked));}catch{}release();});
const showVolume=()=>{volume.hidden=$('audio-mode').value!=='independent';};$('audio-mode').addEventListener('change',showVolume);showVolume();

function valid(heap,at){
  if(!patch||at<0||at+104>heap.length)return false;
  const sig=patch.signature;
  for(let i=0;i<sig.length;i++)if(heap[at+i]!==sig.charCodeAt(i))return false;
  const ds=heap[at+20]|heap[at+21]<<8;
  const data=at-patch.entries.mailbox+(patch.dataSegment-patch.codeSegment)*16;
  const base=data-ds*16;
  if(ds<=patch.dataSegment||base<0||base+0x100000>heap.length||data<base||data+0x2800>base+0xa0000||heap[at+22]>2)return false;
  const view=new DataView(heap.buffer,heap.byteOffset,heap.byteLength);
  const resident=base+view.getUint16(data+0x27fc,true)+view.getUint16(data+0x27fe,true)*16;
  return resident>=base&&resident+48<=base+0xa0000&&residentSignature.every((b,i)=>heap[resident+i]===b)&&heap[resident+11]<=5;
}
function scan(){
  const h=emulator.module.HEAPU8;
  if(valid(h,mailbox))return;
  mailbox=-1;const end=Math.min(h.length,cursor+2*1024*1024);
  for(let at=h.indexOf(84,cursor);at>=0&&at<end;at=h.indexOf(84,at+1))if(valid(h,at)){mailbox=at;break;}
  cursor=end===h.length?0:end;
}
export function readSoloState(){
  if(!emulator||!valid(emulator.module.HEAPU8,mailbox))return null;
  const h=emulator.module.HEAPU8,v=new DataView(h.buffer),data=mailbox-patch.entries.mailbox+(patch.dataSegment-patch.codeSegment)*16;
  return {mode:h[mailbox+22],generation:v.getUint16(mailbox+24,true),ticks:v.getUint32(mailbox+16,true),x:v.getInt16(data+0x2b86,true),y:v.getInt16(data+0x2b88,true),lives:h[data+0xb2c],bombs:h[data+0xb2d],hit:h[data+0x2b94]|h[data+0xaa7],power:h[data+0xaa6]};
}
export function readSoloAudio(){return {status:music?.player.status||'原版声音',track:music?.player.active?.id||null};}
function applyAssist(input){
  if(!emulator)return;const at=input.address;
  const h=emulator.module.HEAPU8;if(!valid(h,at))return;
  const v=new DataView(h.buffer),base=at-patch.entries.mailbox;
  if(input.generation!==v.getUint16(at+24,true))return;
  v.setUint16(base+patch.entries.currentInput,input.currentInput,true);
  h[at+100]=50;h[at+102]=input.focusEnabled?1:0;h[at+103]=input.points;
  const touch=base+patch.entries.touch;
  if(input.touchAction==='add'){
    const clamp=n=>Math.max(-8192,Math.min(8191,n));
    if(input.begin){v.setInt32(touch+2,0,true);v.setInt32(touch+8,0,true);}
    v.setUint16(touch,input.unlimited?3:1,true);
    v.setInt16(touch+2,clamp(v.getInt16(touch+2,true)+input.x),true);
    v.setInt16(touch+4,clamp(v.getInt16(touch+4,true)+input.y),true);
  }else if(input.touchAction==='clear'){v.setUint16(touch,0,true);v.setInt32(touch+2,0,true);}
}
function syncInput(motion=false){
  if(!emulator||playback)return;
  const p=emulator.practice?.status,enabled=focused&&!editing&&!document.hidden&&emulator.state==='running'&&!p?.preparing&&!p?.outcome&&!document.querySelector('.practice-result')?.open&&($('practice-panel').hidden||!$('practice-panel').classList.contains('practice-full'));
  let bits=enabled?keyboardBits(keys,'solo')|padBits(navigator.getGamepads?.()[Number($('gamepad').value)],'solo'):0;
  if(enabled)bits=player.sample(bits);
  if(p?.options.disableBomb)bits&=~16;if(p?.options.disableShot)bits&=~32;
  if(p?.options.disableFocus)bits&=~64;else if(p?.options.forceFocus)bits|=64;
  if((bits&3)===3)bits&=~3;if((bits&12)===12)bits&=~12;
  for(const [code,bit]of nativeKeys)if((bits&bit)!==(lastKeys&bit))emulator.key(code,!!(bits&bit));
  lastKeys=bits;
  if(mailbox>=0){
    const state=readSoloState(),inGame=enabled&&state?.mode===1&&!state.hit,m=motion?unpackTouch(player.pack(bits)):null;
    const command={address:mailbox,generation:state?.generation,currentInput:enabled&&(bits&64)?0x100:0,focusEnabled:$('focus-enabled').checked&&!p?.options.disableFocus,points:pointMask()|(player.alwaysPoint()||p?.options.alwaysPoint?8:0),
      touchAction:motion?(inGame&&m.active?'add':'clear'):(!inGame||!player.moving()?'clear':'keep'),
      begin:!!m?.begin,unlimited:!!m?.unlimited,x:m?.x||0,y:m?.y||0};
    const encoded=JSON.stringify(command),moving=command.touchAction==='add'&&(command.x||command.y);
    if(!recorder||moving||encoded!==lastAssist)emulator.assist(command);
    lastAssist=encoded;
    if(motion)player.consume();
  }
}
function release(){keys.clear();player.reset();syncInput();}
window.addEventListener('keydown',event=>{
  if(!emulator||editing||isFormTarget(event.target)||event.metaKey||event.ctrlKey||event.altKey)return;
  if(isBound(event.code,'solo')||nativeKeys.some(([code])=>code===event.code)){
    event.preventDefault();event.stopImmediatePropagation();
    if(playback)return;
    if(isBound(event.code,'solo'))keys.add(event.code);syncInput();
  }
},true);
window.addEventListener('keyup',event=>{if(keys.delete(event.code)){event.preventDefault();event.stopImmediatePropagation();syncInput();}},true);
window.addEventListener('blur',()=>{focused=false;release();});window.addEventListener('focus',()=>{focused=true;});
document.addEventListener('focusin',event=>{if(isFormTarget(event.target))release();});
document.addEventListener('visibilitychange',()=>{
  if(document.hidden){release();hiddenPause=emulator?.state==='running';if(hiddenPause)emulator.pause();music?.suspend();}
  else if(hiddenPause){hiddenPause=false;emulator?.run();music?.resume();}
});
function tick(now){
  if(disposed)return;prepareRetry();
  if(emulator?.module.HEAPU8){
    try{scan();const s=readSoloState(),play=s?.mode===1;
      player.setActive(emulator.state==='running');player.setContext({key:`${s?.generation??0}:${play?'game':'menu'}`,play:play&&!s.hit});
      const ps=emulator.practice?.status;practiceUI.update({practice:ps,retryReady:!!retrySlot?.ready});
      if(ps?.outcome&&!playback){if(!practiceCompleted){practiceCompleted=true;release();if(!recorder)emulator.pauseReplay(true);}practiceUI.complete({outcome:ps.outcome,recording:!!recorder,ready:!!recordBlob,failed:recorder?.state.failed});}
      syncInput(true);music?.poll(emulator.module.HEAPU8,now);
      if(music){if(ps&&(ps.preparing||ps.waiting||ps.outcome)||s?.mode===2||emulator.state==='paused')music.suspend();else music.resume();}
    }catch(error){status('运行错误：'+error.message);}
  }
  requestAnimationFrame(tick);
}
requestAnimationFrame(tick);
const replaySources=['../practice-attempt.js','../direct-drag.js','../player-ui.js','../solo.js','../dosbox-adapter.js','../dosbox-disk.js','../frame-presenter.js','../solo-patch.json','../vendor/dosbox-x/wdosbox-x-fast.wasm','../vendor/dosbox-x/emulators.js','../vendor/dosbox-x/dosbox-jspi.mjs',
  '../touch-input.js','../control-settings.js','./external-host.js','./external-loader.js','./external-codec.js','./external-recorder.js','./external-ui.js','./external-lifecycle.js',
  '../practice-config.js','../practice-engine.js','../practice-assets.js','../practice-assets.json','../practice-core.json','../practice-start.com','../practice-cn.exe.gz','../practice-jp.exe.gz','../vendor/dosbox-x/wdosbox-x-practice.wasm',
  '../solo-music.js','../solo-music.json','../solo-music.com','../solo-native-music.js','../netplay/local-bgm.js','../bgm/manifest.json'];
const setReplayStatus=text=>{$('replay-status').textContent=text;};
function clearRecording(){recorder?.dispose();recorder=null;recordBlob=null;$('record-save').disabled=true;$('record-discard').disabled=true;$('record-finish').disabled=true;replayUI.closeSave();}
function retrySettings(){return {language:$('language').value,audioMode:$('audio-mode').value,runtime:'main',recordEnabled:$('record-enabled').checked,volume:Number(volume.querySelector('input').value)/100};}
function retryKey(practice){const {language,audioMode,runtime,recordEnabled}=retrySettings();return JSON.stringify([language,audioMode,runtime,recordEnabled,practice.version,practice.mode,practice.setup]);}
function discardRetry(){const slot=retrySlot;retrySlot=null;slot?.stop();}
function makeAttempt(practice,standby=false,replay=null){
  const target=canvas.cloneNode(false);target.onpointerdown=()=>target.focus();
  return createSoloAttempt({canvas:target,practice,settings:retrySettings(),standby,replay,sources:replaySources,applyReplayInput:applyAssist,callbacks:{
    onStatus:text=>{status(text);},onRecordingStatus:setReplayStatus,onFailure:()=>replayUI.closeSave(),
    onFile:(blob,result,config)=>{recordBlob=blob;$('record-save').disabled=false;$('record-discard').disabled=false;if(config.mode!=='target')replayUI.offerSave(blob,result);},
    onResult:(result,config)=>{$('record-finish').disabled=true;if(config.mode!=='target')replayUI.prepareSave();},
    onError:error=>{music?.suspend();player.setActive(false);const text='游戏停止：'+error.message;status(text);},
    onExit:()=>{music?.suspend();player.setActive(false);const text='游戏已退出，可重新启动。';status(text);},
    onPlaybackEnd:matches=>{const text=matches?'Replay 播放完成，终态内存校验一致。':'Replay 已停止：终态内存校验不一致。';status(text);emulator?.pauseReplay(true);$('replay-pause').disabled=true;replayUI.showEnd(matches);}
  }});
}
function prepareRetry(){
  if(disposed||launching)return;
  const eligible=!playback&&committedPractice?.mode==='target'&&practiceUI.options.preparedRetry;
  if(!eligible){discardRetry();return;}
  if(retrySlot?.cancelled||retrySlot?.error){discardRetry();retryBlocked=true;}
  const key=retryKey(committedPractice);if(retrySlot?.key!==key)discardRetry();
  if(retrySlot||retryBlocked||document.hidden||!activeAttempt||performance.now()<warmAfter)return;
  const ps=emulator?.isWorker?emulator.snapshot.practice:emulator?.practice?.status;if(!ps?.ready||ps.error)return;
  const slot=makeAttempt({...committedPractice,options:practiceUI.options},true);slot.key=key;retrySlot=slot;
  slot.task.catch(()=>{if(retrySlot===slot){retrySlot=null;retryBlocked=true;}});
}
export function readRetryState(){return {state:retrySlot?(retrySlot.ready?'ready':'preparing'):retryBlocked?'unavailable':'idle',last:lastRetry};}
for(const id of ['record-enabled','language','audio-mode'])$(id)?.addEventListener('change',()=>{discardRetry();retryBlocked=false;});
async function startGame(replay=null,practiceConfig=null,cached=null){
  if(launching||emulator||disposed)return;launching=true;
  $('start').disabled=true;$('restart').disabled=true;playback=!!replay;lastAssist='';
  let attempt=cached;const began=performance.now(),cachedReady=!!cached?.ready;let usedCache=!!cached;
  try{
    if(playback){
      $('language').value=replay.header.language;$('audio-mode').value=replay.header.audioMode||'original';$('record-enabled').checked=false;
      clearRecording();discardRetry();
    }
    const practice=normalizePractice(playback?replay.header.practice:practiceConfig||practiceUI.configuration());
    committedPractice=practice;practiceCompleted=false;practiceUI.setRunning(!playback,practice);
    for(const id of ['language','audio-mode'])$(id).disabled=true;
    const text=cachedReady?'正在恢复练习起点…':cached?'正在完成练习起点准备…':'正在加载单人游戏…';status(text);
    attempt??=makeAttempt(practice,false,replay);activeAttempt=attempt;
    try{
      await attempt.task;if(attempt.cancelled||attempt.error)throw attempt.error||Error('预备练习已失效');
      if(cached)await attempt.core.activatePractice(practiceUI.options);
      if(attempt.cancelled||attempt.error)throw attempt.error||Error('预备练习已失效');
    }catch(error){
      if(!cached||disposed)throw error;attempt.stop();usedCache=false;
      attempt=makeAttempt({...practice,options:practiceUI.options});activeAttempt=attempt;await attempt.task;
    }
    if(disposed){attempt.stop();return;}
    emulator=attempt.core;patch=attempt.patch;recorder=attempt.recorder;music=attempt.music;recordBlob=attempt.blob;
    music?.player.setVolume(Number(volume.querySelector('input').value)/100);
    canvas.replaceWith(attempt.canvas);canvas=attempt.canvas;attempt.foreground=true;
    if(!emulator.isWorker&&emulator.practice?.debug.csBase>=0){mailbox=emulator.practice.debug.csBase+patch.entries.mailbox;}
    if(cached)lastRetry={cached:usedCache,cachedReady,milliseconds:Math.round(performance.now()-began)};
    warmAfter=performance.now()+400;retryBlocked=false;focused=true;
    host.hidden=false;$('pause').disabled=playback;$('pause').textContent='暂停运行';
    player.setActive(true);canvas.focus();if(document.hidden)emulator.pause();
    $('record-finish').disabled=!recorder||recorder.state.ending||recorder.state.failed;
    $('record-discard').disabled=!recorder;$('record-save').disabled=!recordBlob;
    setReplayStatus(attempt.recordStatus||(recorder?'等待本次首局；结束或通关时询问保存，首次续关前停止记录。':attempt.recordingError?'Replay 未能启动：'+attempt.recordingError+'；游戏继续。':'本次启动不录制。'));
    $('replay-pause').dataset.paused='false';$('replay-pause').textContent='暂停播放';
    $('replay-fast').disabled=!playback;$('replay-pause').disabled=!playback;$('replay-stop').disabled=!playback;
    {const text=playback?'正在重播浏览器 Replay。':practice.mode==='target'?(usedCache?'练习起点已恢复 · 原版暂停中，按 Enter / Esc 继续。':'正在准备所选练习入口…'):'单人游戏已启动，请在原版菜单选择难度和机体。';status(text);}
  }catch(error){
    attempt?.stop();activeAttempt=null;emulator=null;clearRecording();music?.dispose();music=null;player.setActive(false);playback=false;
    practiceUI.setRunning(false);$('start').disabled=false;
    for(const id of ['language','audio-mode'])$(id).disabled=false;
    const text='启动失败：'+error.message;status(text);
  }finally{launching=false;if(!disposed)$('restart').disabled=false;}
}

$('start').onclick=()=>startGame();
$('pause').onclick=()=>{
  if(emulator?.state==='running'){release();emulator.pause();music?.suspend();$('pause').textContent='继续运行';}
  else if(emulator?.state==='paused'){emulator.run();music?.resume();$('pause').textContent='暂停运行';canvas.focus();}
};
$('restart').onclick=()=>restartPractice(practiceUI.configuration());
async function restartPractice(config,retry=false){
  if(launching||disposed)return;
  let slot=retry&&practiceUI.options.preparedRetry&&retrySlot?.key===retryKey(config)?retrySlot:null;
  if(slot)retrySlot=null;discardRetry();
  release();activeAttempt?.stop();activeAttempt=null;clearRecording();replayUI.reset();practiceUI.setRunning(false);
  music?.dispose();music=null;emulator?.dispose();emulator=null;playback=false;mailbox=-1;cursor=lastKeys=0;
  const began=performance.now();await startGame(null,config,slot);
  if(retry&&!slot)lastRetry={cached:false,cachedReady:false,milliseconds:Math.round(performance.now()-began)};
}
$('record-finish').onclick=()=>recorder?.finish();
$('record-save').onclick=()=>{if(!recordBlob)return;const url=URL.createObjectURL(recordBlob),link=document.createElement('a');link.href=url;link.download='th02-solo.th02r';link.click();setTimeout(()=>URL.revokeObjectURL(url),1000);replayUI.closeSave();setReplayStatus('Replay 已请求保存；重新启动游戏可录制新局。');};
$('record-discard').onclick=()=>{clearRecording();setReplayStatus('Replay 已丢弃，游戏继续；重新启动游戏可录制新局。');};
$('replay-file').onchange=async event=>{
  try{const file=event.target.files[0];if(!file)return;loadedReplay=await decodeExternalReplay(file,'02');$('language').value=loadedReplay.header.language;
    $('replay-play').disabled=false;setReplayStatus(`已载入 ${file.name}，可开始独立重播。`);
  }catch(error){loadedReplay=null;$('replay-play').disabled=true;setReplayStatus('Replay 导入失败：'+error.message);}
};
$('replay-play').onclick=async()=>{
  if(!loadedReplay)return;
  discardRetry();activeAttempt?.stop();activeAttempt=null;
  if(emulator){release();emulator.dispose();emulator=null;}
  music?.dispose();music=null;clearRecording();await startGame(loadedReplay);
};
$('replay-pause').onclick=()=>{if(!emulator||!playback)return;const paused=$('replay-pause').dataset.paused==='true';
  emulator.pauseReplay(!paused);$('replay-pause').dataset.paused=String(!paused);$('replay-pause').textContent=paused?'暂停播放':'继续播放';canvas.focus();};
function stopPlayback(){
  if(!playback)return;discardRetry();activeAttempt?.stop();activeAttempt=null;replayUI.reset();release();emulator?.dispose?.();emulator=null;playback=false;
  const next=canvas.cloneNode(false);canvas.replaceWith(next);canvas=next;canvas.onpointerdown=()=>canvas.focus();
  $('language').disabled=$('audio-mode').disabled=false;$('pause').disabled=true;
  $('start').disabled=false;$('replay-play').disabled=!loadedReplay;
  $('replay-stop').disabled=$('replay-pause').disabled=$('replay-fast').disabled=true;$('replay-pause').textContent='暂停播放';
  $('status').textContent='Replay 已结束，可以再次播放、导入其他录像或开始游戏。';
}
$('replay-stop').onclick=stopPlayback;
window.addEventListener('pagehide',()=>{disposed=true;release();discardRetry();activeAttempt?.stop();recorder?.dispose();music?.dispose();emulator?.dispose();});

window.addEventListener('pageshow',event=>{
  if(!event.persisted)return;disposed=false;launching=false;activeAttempt=null;emulator=null;music=null;
  clearRecording();playback=false;committedPractice=null;practiceCompleted=false;mailbox=-1;cursor=lastKeys=0;
  practiceUI.setRunning(false);$('start').disabled=false;$('restart').disabled=false;
  for(const id of ['language','audio-mode'])$(id).disabled=false;
  $('replay-play').disabled=!loadedReplay;$('replay-stop').disabled=$('replay-pause').disabled=$('replay-fast').disabled=true;
});
