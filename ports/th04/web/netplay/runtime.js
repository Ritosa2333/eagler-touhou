import {gameVersion} from '../game-version.js';
import {NP21} from '../dosbox-adapter.js';
import {writeNativeTouch,touchPlayer} from '../native-touch.js';
import {encodeConfig,installConfig} from '../launch-config.js';
import {sha256} from './sha256.js';
import {isBound,keyboardBits} from './controls.js';
import {useControlSnapshot,isFormTarget,controlsSummary} from '../control-settings.js';
import {PauseMenu} from './pause.js';
import {createPersonalHud} from './personal-hud.js';
import {readSoloFault} from '../solo-fault.js';
import {LocalSe,readNativeSe} from './local-se.js';
import {assetBytes} from '../preload-cache.js';
const workerHost=globalThis.th04WorkerHost;
const canvas=workerHost?.canvas??document.getElementById('canvas'),details=workerHost?{}:document.getElementById('details');
const nativeCanvas=workerHost?.canvas??document.createElement('canvas');
nativeCanvas.width=640;nativeCanvas.height=400;
if(!workerHost){nativeCanvas.hidden=true;nativeCanvas.style.display='none';document.body.append(nativeCanvas);}
function notifyParent(message){if(workerHost)workerHost.notify(message);else if(typeof globalThis.th04RuntimeNotify==='function')globalThis.th04RuntimeNotify(message);}
const presentPersonalHud=createPersonalHud(canvas,nativeCanvas,{overlay:!!workerHost});
let presentationHud=null;
let rawPresentation=false,presentationCount=0,presentationPending=false;
let pointPreferences={focus:true,always:false};
const signature=new TextEncoder().encode('TH04COOPLABv001!');
const keyTable=[['ArrowUp','ArrowUp',38,1],['ArrowDown','ArrowDown',40,2],['ArrowLeft','ArrowLeft',37,4],['ArrowRight','ArrowRight',39,8],['KeyX','x',88,16],['KeyZ','z',90,32],['ShiftLeft','Shift',16,64],['Escape','Escape',27,128],['Enter','Enter',13,256]];
let emulator,patch,soundPolicy,bgm,musicTask,closed=false,bootPromise,oldP1=0,ticks=0,inputListener=()=>{},keys=new Set(),enabled=false,mailbox=-1;
let guestMemoryBase=-1,residentAddress=-1,gameplaySeen=false;
const residentSignature=new TextEncoder().encode('HUMAConfig\0');
let musicStatus='浏览器音乐：等待游戏开始后加载',musicError=false,lastMusicSequence=null;
let pendingMusic=[],lastMusicName='',musicSettings,musicReady=false,stoppedMusic=null;
let pauseMenu,localSlot,menuAction=()=>{},blocked=[0,0],emulatedTicks=0,playerCount=2;
const rollbackStates=new Map(),effects=new Map();
let confirmedPrefix=0,replaying=false,currentEffect;
let checkpointReplays=0;
let runtimeFailure=null;
let localSe,seError='';
function reportRuntimeFailure(kind,error){
  if(runtimeFailure||closed)return;
  const message=String(error?.message||error);
  runtimeFailure={kind,message,tick:ticks,emulatedTicks};
  stoppedMusic??=musicInfo();enabled=false;closed=true;bgm?.dispose();localSe?.dispose();details.textContent=message;
  notifyParent({protocol:'th04-rollback/4',event:'runtime-error',message,kind});
}
let controlsBlocked=false;
function localButtons(){
  return keyboardBits(keys);
}
function clearKeys(){keys.clear();inputListener(0);}
if(!workerHost){
window.addEventListener('keydown',event=>{
  if(controlsBlocked||isFormTarget(event.target)||event.metaKey||!isBound(event.code))return;
  event.preventDefault();event.stopImmediatePropagation();keys.add(event.code);inputListener(localButtons());
},true);
window.addEventListener('keyup',event=>{keys.delete(event.code);inputListener(localButtons());},true);
window.addEventListener('blur',clearKeys);
document.addEventListener('focusin',event=>{if(isFormTarget(event.target))clearKeys();});
document.addEventListener('visibilitychange',()=>{if(document.hidden){clearKeys();emulator?.module.netFlushAudio?.();localSe?.stop();}});
canvas.addEventListener('pointerdown',()=>canvas.focus());
}
function nativePauseAddress(){
  return !mailboxOwned(mailbox,guestMemoryBase,residentAddress)?-1:mailbox-patch.mailbox_cs_offset+patch.native_pause_cs_offset;
}
function nativePauseActive(){
  const at=nativePauseAddress();
  return at>=0&&emulator.module.HEAPU8[at+3]!==0;
}
function predictionBlockReason(){
  if(pauseMenu.exited)return '退出同步';
  if(pauseMenu.paused)return '同步暂停菜单';
  if(nativePauseActive())return '原生暂停菜单尚未退出';
  return '';
}
if(!workerHost){
document.getElementById('bgm-volume').addEventListener('input',event=>bgm?.setVolume(Number(event.target.value)/100));
window.addEventListener('pagehide',()=>{closed=true;bgm?.dispose();localSe?.dispose();});
document.getElementById('sound').onclick=async()=>{
  try{
    const context=emulator?.module.SDL2?.audioContext;
    if(!context)throw Error('模拟器音频尚未就绪');
    await context.resume();document.getElementById('sound').textContent='本机声音已启用';canvas.focus();
  }catch(error){details.textContent=error.message;}
};
}
async function get(path,type='json'){
  const response=await fetch(new URL(path,import.meta.url),{cache:'no-cache'});if(!response.ok)throw Error(`资源读取失败：${path}`);
  if(type==='arrayBuffer'&&response.body){
    const total=Number(response.headers.get('content-length'))||0,reader=response.body.getReader(),chunks=[];
    let received=0,last=0;
    while(true){
      const {done,value}=await reader.read();if(done)break;
      chunks.push(value);received+=value.length;
      if(performance.now()-last>200){
        last=performance.now();
        await bootProgress(`下载游戏磁盘：${(received/1048576).toFixed(2)} MiB${total?` / ${(total/1048576).toFixed(2)} MiB（${Math.min(100,Math.floor(received/total*100))}%）`:''}`,1);
      }
    }
    const bytes=new Uint8Array(received);let at=0;
    for(const chunk of chunks){bytes.set(chunk,at);at+=chunk.length;}
    return bytes.buffer;
  }
  return response[type]();
}
async function bootProgress(message,step){
  details.textContent=`启动进度：${message}`;
  notifyParent({protocol:'th04-rollback/4',event:'boot-progress',message,step});
  // Give the parent page a chance to display the stage before expensive work.
  await new Promise(resolve=>setTimeout(resolve,0));
}
async function boot(config){
  const selected=gameVersion(config.language);
  await bootProgress('下载游戏磁盘与版本清单',1);
  if(typeof WebAssembly==='undefined')throw Error('浏览器不支持 WebAssembly，请使用最新版 Chrome / Edge / Safari');
  if(typeof DecompressionStream==='undefined')throw Error('浏览器缺少 gzip 解压支持，请更新浏览器');
  const [meta,manifest,version]=await Promise.all([
    get('../'+selected.coopMeta),get('../'+selected.coopPatch),get('./runtime.json')]);
  patch=manifest;
  await bootProgress('解压游戏磁盘',2);
  const disk=await assetBytes(selected.coopDisk,{hash:meta.sha256,size:meta.size,gzip:true});
  await bootProgress('校验游戏磁盘（局域网 HTTP 下可能需要更长时间）',3);
  if(disk.length!==meta.size||await sha256(disk)!==meta.sha256)throw Error('游戏磁盘校验失败，请刷新页面');
  installConfig(disk,patch,encodeConfig(config));
  await bootProgress('安装本局配置和声音补丁',4);
  // BGM OFF / FM SE arbitration ON; driver play calls use browser samples.
  // Keep the original launcher, PMD driver, shared disk and single-player intact.
  const sound=soundPolicy=version.external_sound_by_language?.[selected.language];
  if(sound?.disk_sha256!==meta.sha256||sound.bgm!==0||sound.se!==1||sound.schema!==3||sound.observer?.se_capacity!==8||!version.local_se_sha256||!sound.edits?.length)
    throw Error('联机声音配置版本不匹配，请重启服务并刷新所有玩家页面');
  for(const edit of sound.edits){
    if(edit.targets.length!==edit.expected.length||edit.targets.length!==edit.values.length)throw Error('声音配置长度不匹配');
    for(const [i,at]of edit.targets.entries()){
      if(!Number.isInteger(at)||at<0||at>=disk.length||disk[at]!==edit.expected[i])throw Error('游戏声音配置位置不匹配');
    }
  }
  for(const edit of sound.edits)edit.targets.forEach((at,i)=>{disk[at]=edit.values[i];});
  await bootProgress('下载、编译并初始化 DOSBox-X WASM 与音频设备',5);
  const tracked=config.rollback!==false&&WebAssembly.validate(new Uint8Array([0,97,115,109,1,0,0,0,5,5,2,0,1,0,1]));
  const core=tracked?'wdosbox-x-rollback-tracked.wasm':'wdosbox-x-rollback.wasm';
  const wasmHash=tracked?version.write_journal?.wasm_sha256:version.rollback_wasm?.sha256;
  const wasmBinary=await assetBytes(`vendor/dosbox-x/${core}`,{hash:wasmHash});
  emulator=await NP21.create({canvas:nativeCanvas,artPatch:patch,wasmBinary,lockstep:true,externalEffects:true,
    ...(workerHost?{workerRuntime:true,audioContext:workerHost.audioContext}:{}),rollbackEnabled:config.rollback!==false,lockstepEpoch:version.epoch,
    ExMemory:7,Latencys:40,SampleHz:44100,SNDboard:4,
    no_mouse:true,use_menu:false,fontfile:'font.bmp',
    onError:error=>reportRuntimeFailure('emulator-error',Error('模拟器异常：'+String(error?.message||error))),
    onExit:()=>reportRuntimeFailure('emulator-exit',Error('游戏模拟器已经退出'))});
  emulator.module.projectileArt.view(localSlot);
  if(workerHost){
    emulator.module.getPersonalHud=()=>presentationHud;
    emulator.module.getHitPoints=()=>presentationHud?.points||[];
  }
  if(closed){emulator.pause();return;}
  localSe=workerHost?workerHost.se:new LocalSe(emulator.module.SDL2.audioContext);
  const seLoading=localSe.load().catch(error=>{if(!closed)seError='浏览器音效加载失败：'+error.message;});
  await bootProgress('挂载游戏磁盘',6);
  emulator.addDiskImage('th04-sync.hdi',disk);await emulator.setHdd(0,'th04-sync.hdi');
  // The private DOS launcher restores 30000 cycles before MAIN. The initial
  // 1000-cycle budget avoids wasting interpreted work on firmware/driver waits.
  // All peers run the same fixed sequence until the first gameplay mailbox.
  emulator.module.warmingBoot=true;emulator.run();
  try{
    const {runBoot}=await import('./boot-runner.js');
    const ready=await runBoot({step:()=>{
      emulator.step();
      // Before MAIN is running the mailbox cannot be live. Avoid repeatedly
      // scanning the entire WASM heap during BIOS and DOS driver waits.
      if(emulator.tick%15===0&&emulator.module.UTF8ToString(emulator.module._emGetRunningProgram()).toUpperCase()==='MAIN')findMailbox();
    },
      ready:()=>mailbox>=0,tick:()=>emulator.tick,cancelled:()=>closed,
      progress:message=>bootProgress(message,6)});
    if(!ready)return;
  }finally{emulator.module.warmingBoot=false;emulator.pause();}
  await seLoading;
  if(closed){localSe.dispose();return;}
  emulator.module.bootCPUMetrics=emulator.module.UTF8ToString(emulator.module._getCPUMetrics());
  if(emulator.module.bootCPUMetrics.split('|')[0]!=='30000')
    throw Error('启动后联机算力未恢复，请刷新页面并检查引擎版本');
  if(emulator.module.SDL2?.audioContext?.sampleRate!==44100)throw Error('此浏览器未提供统一的 44100 Hz 音频，请改用新版 Chrome 或 Edge');
  // No music imports/downloads/decoding until an actual game frame is observed.
  musicSettings=config;
  await bootProgress('原游戏磁盘和引擎已就绪，等待全员同步开局',7);
  return {inputTransport:version.input_transport_sha256,worker:version.network_worker_sha256,inputLatch:version.input_latch_sha256,localSe:version.local_se_sha256,projectileArt:version.projectile_art_sha256,presentation:version.presentation_sha256,disk:meta.sha256,runtime:version.generated_js_sha256,wasm:version.wasm_sha256,mobileWasm:version.mobile_wasm_sha256,clock:version.clock_sha256,cpuCycles:version.cpu_cycles,audio:version.audio_output_sha256,nativeSound:sound,queue:version.rollback_queue_sha256,snapshots:version.native_snapshots_sha256,controls:version.controls_sha256,pause:version.pause_sha256,game:version.game_adapter_sha256,room:version.room_sha256,membership:version.membership_sha256,startup:version.startup_sha256,hostSlot:pauseMenu.hostSlot,adapter:version.adapter,dosboxDisk:version.dosbox_disk_sha256,memoryDiff:version.dosbox_memory_diff_sha256,snapshotDiff:version.snapshot_diff_wasm_sha256,fault:version.fault_sha256,trackedWasm:version.write_journal?.wasm_sha256,trackedGlue:version.write_journal?.glue_sha256,trackedDiff:version.write_journal?.diff_sha256};
}
function mailboxOwned(at,base,resident){
  const heap=emulator?.module.HEAPU8;
  return !!heap&&Number.isInteger(base)&&base>=0&&base+0xa0000<=heap.length&&
    Number.isInteger(at)&&at>=base&&at+675<base+0xa0000&&
    Number.isInteger(resident)&&resident>=base&&resident+74<=base+0xa0000&&
    heap[at+25]===1&&signature.every((v,i)=>heap[at+i]===v)&&
    residentSignature.every((v,i)=>heap[resident+i]===v)&&heap[resident+48]===0x37;
}
function findMailbox(){
  const heap=emulator.module.HEAPU8,view=new DataView(heap.buffer);
  if(mailbox>=0)return;
  // MAIN's original exit sets end_sequence before freeing its code/heap.
  // MAINE reuses that memory; a surviving signature alone is not ownership.
  if(residentAddress>=0&&residentSignature.every((v,i)=>heap[residentAddress+i]===v)&&heap[residentAddress+48]!==0x37)return;
  const candidates=[];
  const limit=guestMemoryBase<0?heap.length:guestMemoryBase+0xa0000;
  for(let at=heap.indexOf(signature[0],Math.max(0,guestMemoryBase));at>=0&&at<limit;at=heap.indexOf(signature[0],at+1)){
    if(at+675>=heap.length||!signature.every((v,i)=>heap[at+i]===v)||heap[at+25]!==1||!view.getUint32(at+16,true)||!view.getUint16(at+20,true))continue;
    const data=at-patch.mailbox_cs_offset+(patch.data_segment-patch.code_segment)*16;
    const base=data-view.getUint16(at+20,true)*16;
    if(base<0||base+0xa0000>heap.length||data<base||data+0xba8a>base+0xa0000)continue;
    if(guestMemoryBase>=0&&base!==guestMemoryBase)continue;
    const resident=base+view.getUint16(data+0xba86,true)+view.getUint16(data+0xba88,true)*16;
    if(!mailboxOwned(at,base,resident))continue;
    candidates.push({at,base,resident});
  }
  if(candidates.length===1){
    const {at,base,resident}=candidates[0];mailbox=at;guestMemoryBase=base;residentAddress=resident;gameplaySeen=true;
  }
  emulator.module.projectileArt?.bind(mailbox);
}
function refreshMailbox(){
  if(mailbox<0)return;
  if(mailboxOwned(mailbox,guestMemoryBase,residentAddress))return;
  mailbox=-1;emulator.module.projectileArt?.bind(-1);
}
function applyP1(bits){
  for(const [code,key,keyCode,bit]of keyTable){
    if((bits&bit)===(oldP1&bit))continue;
    emulator.module.netKey(bits&bit?'keydown':'keyup',{code,key,keyCode,which:keyCode,charCode:0,
      location:code==='ShiftLeft'?1:0,timeStamp:emulatedTicks*1000/60,shiftKey:!!(bits&64),ctrlKey:false,altKey:false,metaKey:false,repeat:false,preventDefault(){}});
  }
  oldP1=bits;
}
function stepInner(inputs){
  // Browser-written input lives in the guest's conventional-memory window.
  // Native writes and host glue writes are journaled by the tracked core.
  if(guestMemoryBase>=0)emulator.module.thDirtyMark?.(guestMemoryBase,1048576);
  const touchInputs=inputs;
  if(!enabled)throw Error(runtimeFailure?.message||'本机游戏没有运行');
  // There is no native stage pause before the first gameplay mailbox exists.
  if(mailbox<0&&emulatedTicks%15===0)findMailbox();
  if(mailbox>=0){
    emulator.module.HEAPU8[mailbox-patch.mailbox_cs_offset+patch.focus_visible_mask_cs_offset]=
      0; // All peers disable native markers; browser preferences only affect final pixels.
  }
  inputs=inputs.map(value=>(value&4095)&~2048);
  const wasPaused=pauseMenu.paused,result=mailbox>=0?pauseMenu.step(inputs):'play';
  ticks++;
  if(result==='exit')return 'exit';
  if(result==='paused'&&!wasPaused)currentEffect.flushAudio=true;
  if(result==='resume')blocked=[...inputs];
  inputs=inputs.map((value,i)=>{blocked[i]&=value;return value&~blocked[i]&~(128|512|1024);});
  const pauseAt=nativePauseAddress(),active=nativePauseActive();
  if(pauseAt>=0){
    const heap=emulator.module.HEAPU8;
    heap[pauseAt]=1;heap[pauseAt+1]=pauseMenu.paused?1:0;heap[pauseAt+2]=pauseMenu.selection;
    if(pauseMenu.paused||active||result==='resume'){
      inputs=Array(playerCount).fill(0);
      // Any seat can request the original P1 pause entry. Release Esc as soon
      // as native code acknowledges it; native menu commands use the bridge.
      if(pauseMenu.paused&&!active)inputs[0]=128;
    }
  }
  applyP1(inputs[0]);
  if(mailbox>=0){
    const heap=emulator.module.HEAPU8;
    for(let slot=1;slot<playerCount;slot++){
      const at=slot===1?mailbox+22:mailbox-patch.mailbox_cs_offset+patch.p3_input_cs_offset;
      heap[at]=inputs[slot]&63;heap[at+1]=0;heap[at+2]=(inputs[slot]>>>6)&1;
    }
  }
  writeNativeTouch(emulator.module.HEAPU8,mailbox,patch,touchInputs,pauseMenu.paused||active||result==='resume');
  emulator.step();emulatedTicks++;
  refreshMailbox();
  if(mailbox>=0&&!seError){
    try{currentEffect.se=readNativeSe(emulator.module.HEAPU8,gameDataBase(),soundPolicy.observer);}
    catch(error){currentEffect.seError=error.message;}
  }
  if(mailbox>=0&&!musicError){
    try{currentEffect.music=readMusic();}
    catch(error){currentEffect.musicError=error.message;}
  }
  if(!gameplaySeen&&emulatedTicks>=10800)throw Error('模拟启动超过 180 秒仍未进入游戏，请记录两边的画面');
  if(!replaying&&ticks%30===0){
    const s=snapshot();
    const info=emulator.module.netInfo();
    details.textContent=`本机同步步 ${ticks} · 游戏帧 ${s?.gameFrame??'启动中'} · 音频断供 ${info.audioUnderruns??0}\n声音待播 ${(info.audioQueuedMs??0).toFixed(1)} ms · 设备延迟 ${info.audioDeviceMs==null?'浏览器未提供':info.audioDeviceMs.toFixed(1)+' ms'} · 清理积压 ${info.audioResets??0} 次`;
    details.textContent+='\n'+(seError||localSe?.status||'浏览器音效：等待');
    if(s)details.textContent+='\n'+s.players.map((xy,i)=>`P${i+1} (${xy.join(', ')})`).join(' · ');
    else details.textContent+=`\n启动进度：正在执行 DOS / 游戏启动，尚未检测到游戏首帧（模拟时间 ${(emulatedTicks/60).toFixed(1)} 秒）。`;
    details.textContent+=`\n${musicError?musicStatus:(bgm?.status??musicStatus)}`;
    if(mailbox>=0){const d=gameDataBase(),h=emulator.module.HEAPU8;details.textContent+=` · 游戏实际 BGM=${h[d+soundPolicy.observer.bgm_mode]} / SE=${h[d+soundPolicy.observer.se_mode]}`;}
  }
}
function step(inputs,{frame=ticks,replay=false,offlineMask=0}={}){
  const saved=rollbackStates.get(frame);
  if(saved)saved.execution={inputs:inputs.slice(),offlineMask};
  replaying=replay;currentEffect={};
  emulator.module.netBeginFrame(frame,replay);
  try{
    refreshMailbox();
    if(mailbox>=0)emulator.module.HEAPU8[mailbox-patch.mailbox_cs_offset+patch.offline_mask_cs_offset]=offlineMask;
    currentEffect.result=stepInner(inputs);effects.set(frame,currentEffect);
    if(!replay)presentationPending=true;
    return currentEffect.result;
  }finally{emulator.module.netEndFrame();replaying=false;currentEffect=null;}
}
function present(){
  if(!enabled||!presentationPending)return false;
  // Called once per completed room batch, synchronously after all simulation.
  // Resolve overlays from this simulation boundary before the Worker GPU draw.
  const live=mailboxOwned(mailbox,guestMemoryBase,residentAddress);
  const hud={raw:rawPresentation,heap:emulator.module.HEAPU8,mailbox:live?mailbox:-1,patch,slot:localSlot,
    guestBase:live?guestBase():0,points:{...pointPreferences,players:playerCount}};
  if(workerHost)presentationHud=presentPersonalHud(hud);
  if(!emulator.module.netPresent())return false;
  if(!workerHost)presentPersonalHud(hud);
  presentationPending=false;presentationCount++;return true;
}
function capture(frame,restored=false){
  if(rollbackStates.has(frame))throw Error('游戏回滚快照重复');
  // Two adjacent speculative ticks share a first-write set. The JS state and
  // original input of each tick remain available for exact odd-tick restore.
  const previous=rollbackStates.get(frame-1);
  const nativeFrame=previous?.nativeFrame===frame-1?frame-1:frame;
  if(nativeFrame===frame)emulator.module.netCapture(frame,restored);
  rollbackStates.set(frame,{nativeFrame,ticks,emulatedTicks,oldP1,mailbox,guestMemoryBase,residentAddress,gameplaySeen,blocked:blocked.slice(),
    pause:{previous:pauseMenu.previous.slice(),paused:pauseMenu.paused,selection:pauseMenu.selection,exited:pauseMenu.exited}});
}
function restore(frame){
  presentationPending=false;
  const saved=rollbackStates.get(frame);
  if(!saved||frame<confirmedPrefix)throw Error('游戏回滚状态已过期');
  const from=saved.nativeFrame,checkpoint=rollbackStates.get(from);
  if(!checkpoint||(from!==frame&&!checkpoint.execution))throw Error('游戏回滚检查点缺失');
  emulator.module.netRestore(from);
  ({ticks,emulatedTicks,oldP1,mailbox,guestMemoryBase,residentAddress,gameplaySeen}=checkpoint);blocked=checkpoint.blocked.slice();
  Object.assign(pauseMenu,checkpoint.pause,{previous:checkpoint.pause.previous.slice()});
  for(const key of rollbackStates.keys())if(key>=from)rollbackStates.delete(key);
  for(const key of effects.keys())if(key>=from)effects.delete(key);
  if(from!==frame){
    // Keep the rebuilt checkpoint available if another late input corrects
    // this same pair. The prior tick's original input is immutable here:
    // RollbackQueue has already selected the earliest changed input.
    capture(from,true);
    step(checkpoint.execution.inputs,{frame:from,replay:true,offlineMask:checkpoint.execution.offlineMask});
    checkpointReplays++;
    if(from<confirmedPrefix){
      // Its effects were already delivered. Reconstruct simulation only.
      effects.delete(from);emulator.module.netDiscardFrame(from);
    }
  }
}
function retainedCheckpoint(prefix){
  const pending=rollbackStates.get(prefix);
  if(pending)return pending.nativeFrame;
  const previous=rollbackStates.get(prefix-1);
  return previous?.nativeFrame===prefix-1?prefix-1:prefix;
}
function confirm(prefix){
  let exit=false;
  for(let frame=confirmedPrefix;frame<prefix;frame++){
    const effect=effects.get(frame);if(!effect)throw Error('确认帧缺少模拟结果');
    if(effect.flushAudio){emulator.module.netFlushAudio();localSe?.stop();}
    emulator.module.netConfirm(frame+1,retainedCheckpoint(frame+1));
    if(effect.seError)seError='浏览器音效错误：'+effect.seError;
    if(effect.se&&!seError){try{localSe.confirm(effect.se);}catch(error){seError='浏览器音效错误：'+error.message;localSe.stop();}}
    if(effect.musicError){musicError=true;musicStatus=`浏览器音乐错误（游戏继续）：${effect.musicError}`;}
    if(effect.music&&!musicError){try{observeMusic(effect.music);}catch(error){musicError=true;musicStatus=`浏览器音乐错误（游戏继续）：${error.message}`;}}
    if(effect.result==='exit')exit=true;
    effects.delete(frame);
  }
  confirmedPrefix=prefix;
  const retain=retainedCheckpoint(prefix);
  for(const frame of rollbackStates.keys())if(frame<retain)rollbackStates.delete(frame);
  return exit?'exit':undefined;
}
function snapshot(){
  if(!mailboxOwned(mailbox,guestMemoryBase,residentAddress))return null;
  const v=new DataView(emulator.module.HEAPU8.buffer),m=mailbox;
  const players=[[v.getInt16(m+36,true)/16,v.getInt16(m+38,true)/16],[v.getInt16(m+46,true)/16,v.getInt16(m+48,true)/16]];
  if(playerCount===3){const at=m-patch.mailbox_cs_offset+patch.p3_motion_cs_offset;players.push([v.getInt16(at+2,true)/16,v.getInt16(at+4,true)/16]);}
  return {gameFrame:v.getUint32(m+16,true),stageFrame:v.getUint16(m+40,true),players,p1:players[0],p2:players[1]};
}
function deliverMusic(ax,name){
  if(workerHost){workerHost.music({ax,name});return;}
  if(musicReady)bgm.commandTrack(ax,name);
  else{pendingMusic.push([ax,name]);if(pendingMusic.length>64)pendingMusic.shift();}
}
function readMusic(){
  const heap=emulator.module.HEAPU8,view=new DataView(heap.buffer),d=gameDataBase(),o=soundPolicy.observer;
  let raw='';
  for(let i=0;i<13&&heap[d+o.filename+i];i++)raw+=String.fromCharCode(heap[d+o.filename+i]);
  const name=/^(st\d\d[bc]?|end[12]|staff|name|op|logo)\./.exec(raw.toLowerCase())?.[1];
  const sequence=view.getUint16(d+o.sequence,true),ax=view.getUint16(d+o.command,true);
  return {name,sequence,ax,mode:heap[d+o.bgm_mode]};
}
function observeMusic({name,sequence,ax,mode}){
  if(mode!==0)throw Error('游戏内 BGM 未关闭，暂不叠加浏览器音乐');
  const changed=lastMusicSequence!==sequence;
  if(name){
    // The current filename remains observable even with native BGM OFF.
    // Start can be followed by volume/fade within one emulated step. The
    // observer retains only the last command, so a new filename must start
    // its song before that command is applied, even if the sequence wrapped.
    if(name!==lastMusicName){deliverMusic(0,name);if(changed&&(ax>>>8)!==0)deliverMusic(ax,name);}
    else if(changed)deliverMusic(ax,name);
    lastMusicName=name;
  }else if(changed&&lastMusicName)deliverMusic(ax,lastMusicName);
  lastMusicSequence=sequence;
  if(!workerHost&&!musicTask){
    musicStatus='浏览器音乐：游戏已运行，正在后台加载';
    musicTask=new Promise(resolve=>setTimeout(resolve,0)).then(async()=>{
      if(closed)return;
      const {LocalBgm}=await import('./local-bgm.js');
      if(closed)return;
      bgm=new LocalBgm(emulator.module.SDL2.audioContext,()=>{});
      bgm.setVolume(Number(document.getElementById('bgm-volume').value)/100);
      await bgm.load({...musicSettings,warmOpening:false});
      if(closed)return;
      musicReady=true;
      for(const [command,track]of pendingMusic.splice(0))bgm.commandTrack(command,track);
    }).catch(error=>{if(!closed){musicError=true;musicStatus=`浏览器音乐加载失败（游戏继续）：${error.message}`;}});
  }
}
function gameDataBase(){return mailbox-patch.mailbox_cs_offset+(patch.data_segment-patch.code_segment)*16;}
function guestBase(){
  const heap=emulator.module.HEAPU8,base=guestMemoryBase;
  if(!Number.isInteger(base)||base<0||base+0xa0000>heap.length)throw Error('游戏内存布局不匹配');
  return base;
}
function checksum(){
  let h=0x811c9dc5;
  for(const value of [...pauseMenu.signature(),...blocked,emulatedTicks])h=Math.imul(h^value,0x01000193);
  if(guestMemoryBase<0)return {tick:ticks,hash:(h>>>0).toString(16).padStart(8,'0'),gameFrame:0};
  const heap=emulator.module.HEAPU8,v=new DataView(heap.buffer),base=guestBase();
  for(let at=base;at<base+0xa0000;at++)h=Math.imul(h^heap[at],0x01000193);
  return {tick:ticks,hash:(h>>>0).toString(16).padStart(8,'0'),gameFrame:mailboxOwned(mailbox,guestMemoryBase,residentAddress)?v.getUint32(mailbox+16,true):0};
}

// Portable captures belong exclusively to a verified replay Worker, never a player.
let portableReplay,portableBase;
function portableBoundary(){
 if(!workerHost?.replayOnly||effects.size||confirmedPrefix!==ticks||emulator.snapshots)throw Error('完整检查点仅允许在独立旁观实例的确认边界');
}
async function replaySave(identity){
 portableBoundary();const {createPortableCheckpoint,encodeCheckpoint}=await import('../replay/portable.js');
 portableReplay??=createPortableCheckpoint(emulator.module,emulator.ci);
 const host={identity,frame:ticks,ticks,emulatedTicks,oldP1,mailbox,guestMemoryBase,residentAddress,gameplaySeen,blocked,pause:{previous:pauseMenu.previous,paused:pauseMenu.paused,selection:pauseMenu.selection,exited:pauseMenu.exited},lastMusicSequence,lastMusicName,pendingMusic,seError,musicError,musicStatus,emulator:{tick:emulator.tick,keys:[...emulator.keys],state:emulator.state},audio:workerHost.captureAudioState?.(),proof:sync.replayProof(),check:checksum()};
 const state=portableReplay.capture(host),bytes=await encodeCheckpoint(state,ticks?portableBase:null);
 if(!ticks)portableBase=state;
 return {frame:ticks,bytes,proof:host.proof,check:host.check};
}
async function replayLoad(bytes,baseBytes,identity){
 portableBoundary();const {createPortableCheckpoint,decodeCheckpoint}=await import('../replay/portable.js');
 portableReplay??=createPortableCheckpoint(emulator.module,emulator.ci);
 const base=baseBytes?await decodeCheckpoint(baseBytes):null,state=await decodeCheckpoint(bytes,base),h=state.host;
 if(h?.identity!==identity||!Number.isSafeInteger(h.frame)||h.frame<0||h.frame>216000||h.frame!==h.ticks)throw Error('完整检查点身份或帧号无效');
 portableReplay.restore(state);({ticks,emulatedTicks,oldP1,mailbox,guestMemoryBase,residentAddress,gameplaySeen,lastMusicSequence,lastMusicName,pendingMusic,seError,musicError,musicStatus}=h);blocked=h.blocked.slice();Object.assign(pauseMenu,h.pause,{previous:h.pause.previous.slice()});
 emulator.tick=h.emulator.tick;emulator.keys=new Set(h.emulator.keys);emulator.state=h.emulator.state;emulator.outputs.clear();
  confirmedPrefix=ticks;rollbackStates.clear();effects.clear();presentationPending=true;
 if(sync.replayProof()!==h.proof||JSON.stringify(checksum())!==JSON.stringify(h.check))throw Error('完整检查点恢复校验不一致');
  workerHost.restoreAudioState?.(h.audio);
 return {frame:ticks,proof:h.proof,check:h.check};
}

function musicInfo(){return {observedTrack:lastMusicName,sequence:lastMusicSequence,error:musicError?musicStatus:null,status:bgm?.status??musicStatus,track:bgm?.active?.id,requested:bgm?.requested?.id};}
const sync={
  faultReport({cpu=true}={}){return {settings:musicSettings,game:snapshot(),runtimeFailure,
    sound:{...localSe?.info(),error:seError},
    music:stoppedMusic??musicInfo(),
    emulator:emulator?{state:emulator.state,error:emulator.failedError?.message??null,logs:emulator.logs??[]}:null,
    pause:pauseMenu?{paused:pauseMenu.paused,selection:pauseMenu.selection,exited:pauseMenu.exited,hostSlot:pauseMenu.hostSlot}:null,
    cpu:cpu&&emulator?readSoloFault(emulator.module.HEAPU8,emulator.module):null,diagnostics:details.textContent};},
  setPointPreferences({focus,always}){pointPreferences={focus:!!focus,always:!!always};},
  diagnosticText:()=>{
    const r=emulator?.module.framePresenter?.info(),ms=value=>value==null?'—':value.toFixed(1);
    return `${details.textContent}\n画面提交 ${presentationCount} · ${rawPresentation?"原始画面":"个人 HUD"}`
      +(r?`\n核心画面更新 ${r.submitted} · WebGL 绘制 ${r.drawn} · 重算画面更新 ${r.replaySubmitted}\n最近绘制间隔 P95 ${ms(r.intervals.p95Ms)} / P99 ${ms(r.intervals.p99Ms)} ms`:'');
  },
  soundInfo:()=>({...localSe?.info(),error:seError}),
  renderInfo:()=>({canvasSubmitted:presentationCount,...emulator?.module.framePresenter?.info()}),
  setRawPresentation(value){rawPresentation=!!value;presentationPending=true;},
  setMusicVolume(value){if(!workerHost)document.getElementById('bgm-volume').value=value;bgm?.setVolume(Number(value)/100);},
  touchContext(){
    if(!enabled||!mailboxOwned(mailbox,guestMemoryBase,residentAddress))return {key:'boot',play:false};
    const heap=emulator.module.HEAPU8,p=touchPlayer(heap,mailbox,patch,localSlot);
    const stage=new DataView(heap.buffer).getUint16(mailbox+26,true),paused=pauseMenu.paused||nativePauseActive();
    return {key:`${stage}:${paused?'menu':'game'}`,play:!paused&&!p.miss&&!p.entry&&!p.out};
  },
  async unlockAudio(){
    const context=emulator?.module.SDL2?.audioContext;
    if(context&&context.state!=='running')await context.resume();
    return context?.state==='running';
  },
  updateControls(value,editing=false){clearKeys();useControlSnapshot(value);controlsBlocked=editing;if(!workerHost)document.getElementById('controls-summary').textContent=controlsSummary('online');},
  configure(host,local,players=2){
    if(![2,3].includes(players)||![host,local].every(n=>Number.isInteger(n)&&n>=0&&n<players))throw Error('无效暂停菜单座位');
    playerCount=players;blocked=Array(players).fill(0);pauseMenu=new PauseMenu(host,players);localSlot=local;
    emulator?.module.projectileArt?.view(localSlot);
  },
  onMenuAction(callback){menuAction=callback;},
  load(config){if(!bootPromise)bootPromise=boot(config);return bootPromise;},
  onInput(callback){inputListener=callback;},
  start(){emulator.run();enabled=true;details.textContent='启动进度：全员已同步开局，正在启动 DOS 和游戏。';canvas.focus?.();},
  stop(){stoppedMusic??=musicInfo();enabled=false;closed=true;clearKeys();bgm?.dispose();localSe?.dispose();if(emulator){emulator.module.netFlushAudio();emulator.pause();}},
  replayProof:()=>JSON.stringify([ticks,emulatedTicks,oldP1,mailbox,guestMemoryBase,residentAddress,gameplaySeen,blocked,pauseMenu.previous,pauseMenu.paused,pauseMenu.selection,pauseMenu.exited,snapshot()]),
  replayScene(){
    if(!mailboxOwned(mailbox,guestMemoryBase,residentAddress))return null;
    const stage=emulator.module.HEAPU8[gameDataBase()+0x5394];
    return stage<=6?{kind:'stage',stage}:null;
  },
  replaySave,replayLoad,
  step,present,checksum,snapshot,capture,restore,confirm,
  readyForRollback:()=>gameplaySeen&&!!emulator?.module.netCapture,
  canPredict:()=>!predictionBlockReason(),predictionBlockReason,
  rollbackInfo:()=>({...emulator?.module.netSnapshotInfo(),checkpointFrames:2,checkpointReplays})
};

if(workerHost)workerHost.runtime=sync;else window.th04Sync=sync;
