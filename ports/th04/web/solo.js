import {createSoloAttempt} from './practice-attempt.js';
import {mountWorkerChoice} from './solo-worker-client.js';
import {mountSoloView} from './solo-view.js';
import {startFrameLoop} from './frame-limit.js';
import {mountSoloPerformance} from './solo-performance.js';
import {mountSoloFault} from './solo-fault.js';
import {mountSoloTestTools,readSoloDiagnostics,writeSoloTestCommand} from './solo-test-tools.js';
import {gameVersion,rememberLanguage} from './game-version.js';
// Standalone original game. No cooperative runtime, launcher or netplay session.
import {mountPlayer} from './player-ui.js';
import {mountFocusSettings} from './focus-settings.js';
import {mountControlSettings,keyboardBits,padBits,isBound,isFormTarget,controlsSummary,syntheticInputEvents} from './control-settings.js';
import {unpackTouch} from './touch-input.js';
import {decodeExternalReplay} from './replay/external-codec.js';
import {mountSoloReplayToolbar} from './replay/external-ui.js';
import {mountPractice} from './practice-ui.js';
import {normalizePractice} from './practice-config.js';
const $=id=>document.getElementById(id);
const audioMode=$('audio-mode');
const runtimeMode=mountWorkerChoice(audioMode.closest('label').parentElement);
let soloMusic;
window.addEventListener('pagehide',event=>{if(event.persisted)soloMusic?.suspend();else soloMusic?.dispose();});
window.addEventListener('pageshow',event=>{if(event.persisted)soloMusic?.resume();});
try{const saved=localStorage.getItem('solo-audio-mode-v2');if(['independent','original','buffered'].includes(saved))audioMode.value=saved;}catch{}
audioMode.onchange=()=>{try{localStorage.setItem('solo-audio-mode-v2',audioMode.value);}catch{}};

rememberLanguage($('language'),'solo');
let emulator,patch,mailbox=-1,scanCursor=0,oldBits=0,focused=true,started=false,menuBits=0,menuUntil=0;
let recorder=null,recordBlob=null,loadedReplay=null,playback=false,lastAssist='';
let activeAttempt=null,retrySlot=null,retryBlocked=false,warmAfter=0,lastRetry=null,disposed=false,launching=false;
let committedPractice=null,practiceCompleted=false,nativePauseTimer=null;
const keys=new Set(),seen=new Map();
const signature=new TextEncoder().encode('TH04SOLOINPUTv1!');
const replaySources=['../practice-attempt.js','../direct-drag.js','../player-ui.js','../solo.js','../solo-worker.js','../solo-worker-client.js','../dosbox-adapter.js','../game-version.js','../solo-native-music.js',
  '../touch-input.js','../vendor/dosbox-x/runtime.json','../vendor/dosbox-x/dosbox-jspi.mjs','../vendor/dosbox-x/wdosbox-x-fast.wasm',
  '../dosbox-disk.js','../frame-presenter.js','../solo-worker-environment.js','../solo-patch.json','../solo-patch-jp.json','../vendor/dosbox-x/emulators.js',
  './external-host.js','./external-loader.js','./external-codec.js','./external-recorder.js','./external-ui.js','./external-lifecycle.js',
  '../practice-config.js','../practice-engine.js','../practice-assets.js','../practice-assets.json','../practice-core.json','../practice-start.com','../practice-cn.exe.gz','../practice-jp.exe.gz','../vendor/dosbox-x/wdosbox-x-practice.wasm',
  '../solo-music.js','../solo-music.json','../solo-music.com','../bgm/manifest.json','../netplay/local-bgm.js'];
const setReplayStatus=text=>{$('replay-status').textContent=text;};
function clearRecording(){recorder?.dispose();recorder=null;recordBlob=null;$('record-save').disabled=true;$('record-discard').disabled=true;$('record-finish').disabled=true;replayUI.closeSave();}
const player=mountPlayer($('game'),{solo:true,onGesture:async()=>{soloMusic?.resume();await emulator?.module.SDL2?.audioContext?.resume();}});
let saveWasPaused=false;
const replayUI=mountSoloReplayToolbar($('game'),{isPlayback:()=>playback,isSynthetic:event=>syntheticInputEvents.has(event),
  onHold:()=>{saveWasPaused=emulator?.state==='paused';release();emulator?.pauseReplay(true);},
  onRelease:()=>{if(!saveWasPaused&&emulator&&emulator.state!=='exited')emulator.pauseReplay(false);canvas.focus();},
  onAgain:()=>{stopPlayback();startGame(loadedReplay);},onStop:()=>stopPlayback(),onFast:value=>emulator?.fastForwardReplay(value),
});
mountSoloView($('game'));
mountSoloFault({host:$('game'),getEmulator:()=>emulator,readState:readSoloState,readContext:()=>({language:$('language').value,audio:audioMode.value,status:$('status').textContent})});
mountSoloPerformance({host:$('game'),game:'04',getEmulator:()=>emulator,readState:()=>{
  const s=readSoloState();return s?{...s,playing:s.mode===1&&!s.flags}:null;
}});
const testTools=mountSoloTestTools({host:$('game'),readState:readSoloState,command:async value=>{
  if(playback||recorder)throw Error('Replay 录制或播放期间不能使用测试命令。请先丢弃录制。');
  const state=readSoloState();if(!state)throw Error('请先进入单机关卡。');
  const command={...value,generation:state.generation};
  if(emulator.isWorker)await emulator.call('soloTest',[command]);
  else writeSoloTestCommand(emulator.module.HEAPU8,mailbox,patch,command);
}});
const screen=document.createElement('div');screen.className='screen solo-screen';
let canvas=document.createElement('canvas');canvas.id='canvas';canvas.width=640;canvas.height=400;canvas.tabIndex=0;screen.append(canvas);player.stage.append(screen);
const practiceUI=mountPractice({host:$('game'),isSynthetic:e=>syntheticInputEvents.has(e),onRelease:release,onFocus:()=>canvas.focus(),
  onChange:options=>{if(!playback)emulator?.practiceOptions(options);if(options.preparedRetry===false)discardRetry();if(options.preparedRetry===true)retryBlocked=false;},onStart:config=>restartPractice(config),
  onRetry:()=>{if(committedPractice?.mode==='target')restartPractice({...committedPractice,options:practiceUI.options},true);},
  onSave:()=>$('record-save').click(),onPause:()=>{release();if(readSoloState()?.mode===1){nativeKey('Escape',true);const until=performance.now()+3000;const check=()=>{if(readSoloState()?.mode===2||performance.now()>=until){nativeKey('Escape',false);nativePauseTimer=null;}else nativePauseTimer=setTimeout(check,30);};nativePauseTimer=setTimeout(check,30);}},
});
const practiceOpen=document.createElement('button');practiceOpen.type='button';practiceOpen.id='practice-open';practiceOpen.textContent='特定练习 · F12';$('start').after(practiceOpen);
practiceOpen.onclick=()=>{player.enter();practiceUI.open();};
// Return keyboard control after using the solo test/settings panel.
$('game').addEventListener('click',event=>{
  if(event.target.closest('[data-close],[data-save]')&&$('game').querySelector('.touch-layout-editor').hidden)canvas.focus();
});
const focus=mountFocusSettings($('control-settings'),{alwaysPointControl:player.alwaysPointControl,profile:'solo'});
const editor=mountControlSettings($('control-settings'),{solo:true,onEditing:()=>release(),onChange:()=>{release();summary();}});
function summary(){$('controls-summary').textContent=controlsSummary('solo');}summary();
const nativeActions=[['ArrowUp',1],['ArrowDown',2],['ArrowLeft',4],['ArrowRight',8],['KeyX',16],['KeyZ',32],['ShiftLeft',64],['Escape',128],['Enter',256]];
function nativeKey(code,down){
  if(!emulator)return;
  const keyCodes={KeyZ:90,KeyX:88,ArrowUp:38,ArrowDown:40,ArrowLeft:37,ArrowRight:39,Escape:27,ShiftLeft:16,Enter:13};
  const event=new KeyboardEvent(down?'keydown':'keyup',{code,key:({KeyZ:'z',KeyX:'x',ShiftLeft:'Shift'})[code]||code,keyCode:keyCodes[code],which:keyCodes[code],bubbles:true});
  syntheticInputEvents.add(event);canvas.dispatchEvent(event);
}
function send(bits){for(const [code,bit] of nativeActions)if((bits&bit)!==(oldBits&bit))nativeKey(code,!!(bits&bit));oldBits=bits;}
function applySoloAssist(value){
  if(!emulator)return;const at=Number.isInteger(value.address)?value.address:mailbox,h=emulator.module.HEAPU8;
  if(at<0||at+36>h.length||!signature.every((v,i)=>h[at+i]===v))return;
  const v=new DataView(h.buffer);
  if(Number.isInteger(value.generation)&&value.generation!==v.getUint16(at+24,true))return;
  h[at+26]=value.points||0;const t=value.touch;
  if(emulator.state==='running'&&h[at+22]===1&&!h[at+23]&&t?.active){
    if(t.begin)v.setInt32(at+30,0,true);
    v.setUint16(at+28,t.unlimited?3:1,true);
    for(const [offset,delta]of [[30,t.x],[32,t.y]])v.setInt16(at+offset,Math.max(-8192,Math.min(8191,v.getInt16(at+offset,true)+delta)),true);
  }else h.fill(0,at+28,at+36);
}
function clearNative(at=mailbox){
  if(playback||at<0||!emulator)return;
  if(emulator.isWorker){emulator.clearInput();return;}
  if(emulator.externalHost){const h=emulator.module.HEAPU8;emulator.assist({address:at,generation:new DataView(h.buffer).getUint16(at+24,true),points:0,touch:null});return;}
  const h=emulator.module.HEAPU8;if(signature.every((v,i)=>h[at+i]===v))h.fill(0,at+28,at+36);
}
function release(){if(nativePauseTimer!==null){clearTimeout(nativePauseTimer);nativePauseTimer=null;nativeKey('Escape',false);}menuBits=0;menuUntil=0;keys.clear();player.reset();send(0);clearNative();}
function scan(now){
  if(emulator.isWorker){mailbox=emulator.snapshot.state?.at??-1;return emulator.snapshot.state;}
  const h=emulator.module.HEAPU8,view=new DataView(h.buffer);
  const previous=mailbox;mailbox=-1;
  for(const [at,state] of seen){
    if(!signature.every((v,i)=>h[at+i]===v)){seen.delete(at);continue;}
    const ticks=view.getUint32(at+16,true);
    if(ticks!==state.ticks){state.ticks=ticks;state.changed=now;}
    // State 0 is explicitly published on world exit; a stale DOS allocation
    // must not turn title/ending touches into writes to the departed game.
    if((now-state.changed<200||h[at+22]===2)&&h[at+22]!==0)mailbox=at;
  }
  // Reuse validated live addresses. Search only while no live mailbox exists,
  // and bound each slice to 1 MiB so discovery cannot scan the whole WASM heap
  // in one animation frame. Overlap the boundary to catch split signatures.
  if(mailbox<0){
    if(scanCursor>=h.length)scanCursor=0;
    const end=Math.min(scanCursor+1024*1024,h.length);
    const slice=h.subarray(scanCursor,Math.min(end+35,h.length));
    for(let offset=slice.indexOf(signature[0]);offset>=0&&scanCursor+offset<end;offset=slice.indexOf(signature[0],offset+1)){
      const at=scanCursor+offset;
      if(at+(patch.mailbox_size||36)>h.length||!signature.every((v,i)=>h[at+i]===v))continue;
      if(view.getUint32(at+16,true)&&view.getUint16(at+20,true)&&!seen.has(at))seen.set(at,{ticks:0,changed:-Infinity});
    }
    scanCursor=end;
  }
  if(previous!==mailbox)clearNative(previous);
  emulator.module.sidebarPaletteAddress=mailbox<0?-1:mailbox-patch.mailbox_cs_offset+(patch.data_segment-patch.code_segment)*16+0x2a82;
  return mailbox<0?null:{at:mailbox,mode:h[mailbox+22],flags:h[mailbox+23],generation:view.getUint16(mailbox+24,true)};
}
function tick(now){
  if(disposed)return;prepareRetry();
  testTools.update(now);
  const ps=emulator?.isWorker?emulator.snapshot.practice:emulator?.practice?.status;
  practiceUI.update({practice:ps,retryReady:!!retrySlot?.ready});
  const muted=readSoloState()?.mode===2||ps?.mode==='target'&&(!ps.ready||ps.waiting||ps.outcome);
  if(muted||emulator?.state!=='running')soloMusic?.suspend();else if(!document.hidden)soloMusic?.resume();
  if(ps?.outcome&&!playback){
    if(!practiceCompleted){practiceCompleted=true;release();if(!recorder)emulator.pauseReplay(true);}
    practiceUI.complete({outcome:ps.outcome,recording:!!recorder,ready:!!recordBlob,failed:recorder?.state.failed});
  }
  if(emulator?.state==='running'){
    if(!emulator.isWorker)soloMusic?.poll(emulator.module.HEAPU8,now);
    const state=scan(now),game=state?.mode===1;
    const enabled=!playback&&focused&&!document.hidden&&!editor.isEditing()&&($('practice-panel').hidden||!$('practice-panel').classList.contains('practice-full'))&&!ps?.outcome;
    player.setContext({key:`${mailbox}:${state?.generation||0}:${game?'game':'menu'}`,play:enabled&&game&&!state.flags});
    let base=0;
    if(enabled){base=keyboardBits(keys,'solo');try{base|=padBits(Array.from(navigator.getGamepads?.()||[]).find(p=>p?.connected),'solo');}catch{}}
    let bits=enabled?player.sample(base):0;
    if((bits&3)===3)bits&=~3;if((bits&12)===12)bits&=~12;
    // The DOS title/options menus poll on their own cadence. Keep a touch
    // button edge down long enough to be sensed, without enabling gameplay fire.
    if(!game&&enabled){
      const touchButtons=bits&~base;
      if(touchButtons){menuBits=touchButtons;menuUntil=now+100;}
      if(now<menuUntil)bits|=menuBits;
      else menuBits=0;
    }else{menuBits=0;menuUntil=0;}
    send(bits);
    const touch=unpackTouch(player.pack(bits));
    const assistInput={address:state?.at??mailbox,generation:state?.generation,points:(focus()?1:0)|(player.alwaysPoint()||ps?.options.alwaysPoint?8:0),
      touch:enabled&&game&&!state.flags?touch:null};
    if(!playback){
      const encoded=JSON.stringify(assistInput),moving=!!assistInput.touch?.active&&(assistInput.touch.x||assistInput.touch.y);
      if(emulator.isWorker)emulator.setInput(assistInput);
      else if(!recorder||moving||encoded!==lastAssist)emulator.assist(assistInput);
      lastAssist=encoded;
    }
    player.consume();
  }
}
startFrameLoop(tick);
for(const type of ['keydown','keyup'])window.addEventListener(type,e=>{
  if(syntheticInputEvents.has(e))return;
  e.stopImmediatePropagation();
  if(type==='keyup'){if(keys.delete(e.code))e.preventDefault();return;}
  if(!emulator||editor.isEditing()||isFormTarget(e.target)||e.metaKey)return;
  if(isBound(e.code,'solo')){e.preventDefault();keys.add(e.code);}
},true);
window.addEventListener('keypress',e=>{if(!syntheticInputEvents.has(e)&&!isFormTarget(e.target)){e.preventDefault();e.stopImmediatePropagation();}},true);
window.addEventListener('blur',()=>{focused=false;release();});window.addEventListener('focus',()=>{focused=true;});
let hiddenPaused=false;
document.addEventListener('visibilitychange',()=>{if(document.hidden){release();soloMusic?.suspend();hiddenPaused=emulator?.state==='running';emulator?.pause();}
  else if(hiddenPaused){hiddenPaused=false;emulator?.run();soloMusic?.resume();}});
document.addEventListener('focusin',e=>{if(isFormTarget(e.target))release();});
canvas.onpointerdown=()=>canvas.focus();
$('restart').onclick=()=>location.reload();
$('start').onclick=async()=>{
  startGame();
};
function retrySettings(){return {language:$('language').value,audioMode:$('audio-mode').value,runtime:$('runtime-mode').value,recordEnabled:$('record-enabled').checked,volume:.7};}
function retryKey(practice){const {language,audioMode,runtime,recordEnabled}=retrySettings();return JSON.stringify([language,audioMode,runtime,recordEnabled,practice.version,practice.mode,practice.setup]);}
function discardRetry(){const slot=retrySlot;retrySlot=null;slot?.stop();}
function makeAttempt(practice,standby=false,replay=null){
  const target=canvas.cloneNode(false);target.onpointerdown=()=>target.focus();
  return createSoloAttempt({canvas:target,practice,settings:retrySettings(),standby,replay,sources:replaySources,applyReplayInput:applySoloAssist,callbacks:{
    onStatus:text=>{$('status').textContent=text;player.note(text);player.diagnostics(text);},onRecordingStatus:setReplayStatus,onFailure:()=>replayUI.closeSave(),
    onFile:(blob,result,config)=>{recordBlob=blob;$('record-save').disabled=false;$('record-discard').disabled=false;if(config.mode!=='target')replayUI.offerSave(blob,result);},
    onResult:(result,config)=>{$('record-finish').disabled=true;if(config.mode!=='target')replayUI.prepareSave();},
    onError:error=>{soloMusic?.suspend();player.setActive(false);const text='游戏停止：'+error.message;$('status').textContent=text;player.note(text);player.diagnostics(text);},
    onExit:()=>{soloMusic?.suspend();player.setActive(false);const text='游戏已退出，可重新启动。';$('status').textContent=text;player.note(text);player.diagnostics(text);},
    onPlaybackEnd:matches=>{const text=matches?'Replay 播放完成，终态内存校验一致。':'Replay 已停止：终态内存校验不一致。';$('status').textContent=text;player.note(text);player.diagnostics(text);emulator?.pauseReplay(true);$('replay-pause').disabled=true;replayUI.showEnd(matches);}
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
for(const id of ['record-enabled','language','audio-mode','runtime-mode'])$(id)?.addEventListener('change',()=>{discardRetry();retryBlocked=false;});
async function startGame(replay=null,practiceConfig=null,cached=null){
  if(launching||emulator||disposed)return;launching=true;started=true;player.enter();
  $('start').disabled=true;$('restart').disabled=true;playback=!!replay;lastAssist='';
  let attempt=cached;const began=performance.now(),cachedReady=!!cached?.ready;let usedCache=!!cached;
  try{
    if(playback){
      $('language').value=replay.header.language;$('audio-mode').value=replay.header.audioMode||'original';$('runtime-mode').value=replay.header.runtime;
      clearRecording();discardRetry();
    }
    const practice=normalizePractice(playback?replay.header.practice:practiceConfig||practiceUI.configuration());
    committedPractice=practice;practiceCompleted=false;practiceUI.setRunning(!playback,practice);
    for(const id of ['language','audio-mode','runtime-mode'])$(id).disabled=true;
    const text=cachedReady?'正在恢复练习起点…':cached?'正在完成练习起点准备…':'正在加载单人游戏…';$('status').textContent=text;player.note(text);player.diagnostics(text);
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
    emulator=attempt.core;patch=attempt.patch;recorder=attempt.recorder;soloMusic=attempt.music;recordBlob=attempt.blob;
    canvas.replaceWith(attempt.canvas);canvas=attempt.canvas;attempt.foreground=true;
    if(!emulator.isWorker&&emulator.practice?.debug.csBase>=0){mailbox=emulator.practice.debug.csBase+patch.mailbox_cs_offset;seen.set(mailbox,{ticks:0,changed:performance.now()});}
    if(cached)lastRetry={cached:usedCache,cachedReady,milliseconds:Math.round(performance.now()-began)};
    warmAfter=performance.now()+400;retryBlocked=false;focused=true;
    player.enter();
    player.setActive(true);canvas.focus();if(document.hidden)emulator.pause();
    $('record-finish').disabled=!recorder||recorder.state.ending||recorder.state.failed;
    $('record-discard').disabled=!recorder;$('record-save').disabled=!recordBlob;
    setReplayStatus(attempt.recordStatus||(recorder?'等待本次首局；结束或通关时询问保存，首次续关前停止记录。':attempt.recordingError?'Replay 未能启动：'+attempt.recordingError+'；游戏继续。':'本次启动不录制。'));
    $('replay-pause').dataset.paused='false';$('replay-pause').textContent='暂停播放';
    $('replay-fast').disabled=!playback;$('replay-pause').disabled=!playback;$('replay-stop').disabled=!playback;$('replay-play').disabled=true;
    {const text=playback?'正在重播浏览器 Replay。':practice.mode==='target'?(usedCache?'练习起点已恢复 · 原版暂停中，按 Enter / Esc 继续。':'正在准备所选练习入口…'):'单人游戏已启动，请在原版菜单选择难度和机体。';$('status').textContent=text;player.note(text);player.diagnostics(text);}
  }catch(error){
    attempt?.stop();activeAttempt=null;emulator=null;clearRecording();soloMusic?.dispose();soloMusic=null;player.setActive(false);playback=false;started=false;player.exit();
    practiceUI.setRunning(false);$('start').disabled=false;
    for(const id of ['language','audio-mode','runtime-mode'])$(id).disabled=false;
    const text='启动失败：'+error.message;$('status').textContent=text;player.note(text);player.diagnostics(text);
  }finally{launching=false;if(!disposed)$('restart').disabled=false;}
}

$('record-finish').onclick=()=>recorder?.finish();
$('record-save').onclick=()=>{if(!recordBlob)return;const url=URL.createObjectURL(recordBlob),a=document.createElement('a');a.href=url;a.download='th04-solo.th04r';a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);replayUI.closeSave();setReplayStatus('Replay 已请求保存；重新启动游戏可录制新局。');};
$('record-discard').onclick=()=>{clearRecording();setReplayStatus('Replay 已丢弃，游戏继续；重新启动游戏可录制新局。');};
$('replay-file').onchange=async event=>{
  try{const file=event.target.files[0];if(!file)return;loadedReplay=await decodeExternalReplay(file,'04');
    $('replay-play').disabled=!!(emulator||started);setReplayStatus(`已载入 ${file.name}，可开始独立重播。`);
  }catch(error){loadedReplay=null;$('replay-play').disabled=true;setReplayStatus('Replay 导入失败：'+error.message);}
};
$('replay-play').onclick=()=>{if(loadedReplay&&!emulator&&!started)startGame(loadedReplay);};
$('replay-pause').onclick=()=>{if(!emulator||!playback)return;const paused=$('replay-pause').dataset.paused==='true';
  emulator.pauseReplay(!paused);$('replay-pause').dataset.paused=String(!paused);$('replay-pause').textContent=paused?'暂停播放':'继续播放';canvas.focus();};
function stopPlayback(){
  if(!playback)return;discardRetry();activeAttempt?.stop();activeAttempt=null;replayUI.reset();release();emulator?.dispose?.();emulator=null;playback=false;
  practiceUI.setRunning(false);committedPractice=null;practiceCompleted=false;
  soloMusic?.dispose();soloMusic=null;
  const next=canvas.cloneNode(false);canvas.replaceWith(next);canvas=next;canvas.onpointerdown=()=>canvas.focus();
  started=false;mailbox=-1;scanCursor=0;seen.clear();audioMode.disabled=runtimeMode.disabled=$('language').disabled=false;
  $('start').disabled=false;$('replay-play').disabled=!loadedReplay;
  $('replay-stop').disabled=$('replay-pause').disabled=$('replay-fast').disabled=true;$('replay-pause').textContent='暂停播放';
  $('status').textContent='Replay 已结束，可以再次播放、导入其他录像或开始游戏。';
}
$('replay-stop').onclick=stopPlayback;
async function restartPractice(config,retry=false){
  if(launching||disposed)return;
  let slot=retry&&practiceUI.options.preparedRetry&&retrySlot?.key===retryKey(config)?retrySlot:null;
  if(slot)retrySlot=null;discardRetry();
  release();activeAttempt?.stop();activeAttempt=null;clearRecording();replayUI.reset();practiceUI.setRunning(false);
  soloMusic?.dispose();soloMusic=null;emulator?.dispose();emulator=null;playback=false;started=false;mailbox=-1;scanCursor=oldBits=menuBits=menuUntil=0;seen.clear();clearTimeout(nativePauseTimer);nativePauseTimer=null;
  const began=performance.now();await startGame(null,config,slot);
  if(retry&&!slot)lastRetry={cached:false,cachedReady:false,milliseconds:Math.round(performance.now()-began)};
}
window.addEventListener('pagehide',()=>{disposed=true;discardRetry();activeAttempt?.stop();recorder?.dispose();emulator?.dispose?.();});

// Read-only diagnostics used by the standalone browser smoke test.
export function readFrameStats(){return {limit:60,callbacks:emulator?.module.localFrameCount||0};}
export function readSoloState(){
  if(emulator?.isWorker)return emulator.snapshot.state;
  if(!emulator||mailbox<0)return null;
  const h=emulator.module.HEAPU8,v=new DataView(h.buffer);
  const data=mailbox-patch.mailbox_cs_offset+(patch.data_segment-patch.code_segment)*16;
  return {at:mailbox,mode:h[mailbox+22],flags:h[mailbox+23],ticks:v.getUint32(mailbox+16,true),
    generation:v.getUint16(mailbox+24,true),pointOptions:h[mailbox+26],
    x:v.getInt16(data+0x464e,true)/16,y:v.getInt16(data+0x4650,true)/16,
    stage:h[data+0x5394],focus:h[data+0x3976],touchFlags:v.getUint16(mailbox+28,true),
    ...readSoloDiagnostics(h,mailbox,patch)};
}

window.addEventListener('pageshow',event=>{
  if(!event.persisted)return;disposed=false;launching=false;activeAttempt=null;emulator=null;soloMusic=null;
  clearRecording();playback=false;committedPractice=null;practiceCompleted=false;started=false;mailbox=-1;scanCursor=oldBits=menuBits=menuUntil=0;seen.clear();
  practiceUI.setRunning(false);$('start').disabled=false;$('restart').disabled=false;
  for(const id of ['language','audio-mode','runtime-mode'])$(id).disabled=false;
  $('replay-play').disabled=!loadedReplay;$('replay-stop').disabled=$('replay-pause').disabled=$('replay-fast').disabled=true;
});
