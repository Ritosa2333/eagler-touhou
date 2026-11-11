import {DOSBoxPC98,supportsWriteJournal} from '../dosbox-adapter.js';
import {Fat12,sha256} from '../solo-fat12.js';
import {keyboardBits,isBound,useControlSnapshot,isFormTarget,controlsSummary} from '../control-settings.js';
import {nativeBits,locateMailbox,guestBase,mailboxActive,nativePaused,writeInputs,readState} from './th02-state.js';
import {configureLauncher,launchConfig,launchDefaults} from './launch-config.js';
import {runBoot} from './boot-runner.js';
import {LocalSfx} from './local-sfx.js';
import {assetBytes} from '../preload-cache.js';
import {hashNativeMemory} from './native-hash.js';
const workerHost=globalThis.th02WorkerHost;
const PROTOCOL='th02-rollback/6',canvas=workerHost?.canvas??document.getElementById('canvas'),details=workerHost?{}:document.getElementById('details');
const nativeCanvas=workerHost?.canvas??document.createElement('canvas');nativeCanvas.width=640;nativeCanvas.height=400;
if(!workerHost){nativeCanvas.hidden=true;document.body.append(nativeCanvas);}
const ctx=workerHost?null:canvas.getContext('2d');
const nativeKeys=[['ArrowUp',1],['ArrowDown',2],['ArrowLeft',4],['ArrowRight',8],['KeyZ',16],['KeyX',32],['Escape',64],['Enter',128]];
let emulator,patch,version,config,bootPromise,mailbox=-1,oldP1=0,ticks=0,emulatedTicks=0,localSlot=0,players=2;
let enabled=false,closed=false,keys=new Set(),inputListener=()=>{},editing=false,pending=false,raw=false,points={focus:true,always:false};
let previousInputs=[0,0,0],confirmedPrefix=0,checkpointReplays=0,presentationCount=0;
const states=new Map(),effects=new Map();
let soundPolicy,bgm,musicTask,musicError='',musicVolume=.7,musicSequence=-1,musicName='',playedName='',pendingMusic=[];
let pendingNative=[0,0,0],consumedNative=[0,0,0];
let sfx,sfxSequence=0;
let guestMemoryBase=-1,residentAddress=-1,gameplaySeen=false;
const residentSignature=new TextEncoder().encode('MIKOConfig\0');
function refreshMailbox(scan=false){
 const h=emulator.module.HEAPU8,v=new DataView(h.buffer);
 const residentValid=at=>at>=guestMemoryBase&&at+48<=guestMemoryBase+0xa0000&&residentSignature.every((b,i)=>h[at+i]===b)&&h[at+11]<=5;
 // MAIN's exit hook clears +22 before freeing its resources. Mima also
 // publishes resident.stage=0x7f before executing the ending program.
 // A surviving signature alone does not mean MAIN still owns that memory.
 if(mailbox>=0&&(!mailboxActive(h,patch,mailbox)||!residentValid(residentAddress)))mailbox=-1;
 if(mailbox<0&&scan){
  if(!(residentAddress>=0&&residentSignature.every((b,i)=>h[residentAddress+i]===b)&&h[residentAddress+11]>5)){
   const candidate=locateMailbox(h,patch);
   if(candidate>=0){
    const base=guestBase(h,candidate,patch),d=candidate-patch.entries.mailbox+(patch.dataSegment-patch.codeSegment)*16;
    if((guestMemoryBase<0||base===guestMemoryBase)&&d>=base&&d+0x2800<=base+0xa0000){
     const resident=base+v.getUint16(d+0x27fc,true)+v.getUint16(d+0x27fe,true)*16;
     const previousBase=guestMemoryBase;guestMemoryBase=base;
     if(residentValid(resident)){
      mailbox=candidate;residentAddress=resident;gameplaySeen=true;
      pendingNative.fill(0);consumedNative=[0,1,2].map(i=>v.getUint16(mailbox+138+i*2,true));
      sfxSequence=readSfxSequence();musicSequence=-1;musicName='';
     }else guestMemoryBase=previousBase;
    }
   }
  }
 }
 emulator.module.playerArt?.bind(mailbox);
}
const state=()=>emulator?readState(emulator.module.HEAPU8,mailbox,patch):null;
function clearKeys(){keys.clear();inputListener(0);}
if(!workerHost){
window.addEventListener('keydown',e=>{if(editing||isFormTarget(e.target)||e.metaKey||e.ctrlKey||e.altKey||!isBound(e.code))return;e.preventDefault();e.stopImmediatePropagation();keys.add(e.code);inputListener(keyboardBits(keys));},true);
window.addEventListener('keyup',e=>{if(keys.delete(e.code)){e.preventDefault();e.stopImmediatePropagation();inputListener(keyboardBits(keys));}},true);
window.addEventListener('blur',clearKeys);
document.addEventListener('visibilitychange',()=>{if(document.hidden){clearKeys();sfx?.stop();emulator?.module.netFlushAudio();}});
canvas.addEventListener('pointerdown',()=>canvas.focus());
}
function notifyParent(message){if(workerHost)workerHost.notify(message);else window.parent.postMessage(message,location.origin);}
async function progress(message,step){details.textContent=message;notifyParent({protocol:PROTOCOL,event:'boot-progress',message,step});await new Promise(r=>setTimeout(r,0));}
async function get(url,type='json'){
 const address=new URL(url,import.meta.url),root=new URL('../',import.meta.url),path=decodeURIComponent(address.pathname.slice(root.pathname.length));
 const hash=version?.assets?.[path];
 if(hash){const bytes=await assetBytes(path,{hash});return type==='json'?JSON.parse(new TextDecoder().decode(bytes)):bytes.buffer;}
 const r=await fetch(address,{cache:'no-store'});if(!r.ok)throw Error(`资源读取失败：${url}（${r.status}）`);return r[type]();
}
async function boot(settings){
 config={...launchDefaults,...settings};launchConfig(config);if(config.players!==players)throw Error('TH02 开局人数不匹配');await progress('读取 TH02 游戏磁盘和版本清单',1);
 [patch,version]=await Promise.all([get('../local-patch.json'),get('./runtime.json')]);
 if(version.protocol!==PROTOCOL)throw Error('联机清单版本不匹配');
 // Verify every adapter, imported module and core before declaring ready. The
 // handshake compares actual bytes, so a stale tab cannot claim a new build.
 let verified=0,nextAsset=0;const assets=Object.entries(version.assets);
 await Promise.all(Array.from({length:4},async()=>{
  while(nextAsset<assets.length){
   const [path,expected]=assets[nextAsset++],bytes=new Uint8Array(await get('../'+path,'arrayBuffer'));
   if(await sha256(bytes)!==expected)throw Error(`联机资源版本不匹配：${path}，请所有玩家强制刷新`);
   verified++;if(verified%10===0)await progress(`校验联机资源 ${verified}/${assets.length}`,2);
  }
 }));
 const entry=patch.disks[config.language];if(!entry)throw Error('游戏语言无效');
 await progress('解压并校验 TH02 合作磁盘',3);
 const disk=await assetBytes(entry.file,{hash:entry.sha256,size:entry.size,gzip:true});
 if(disk.length!==entry.size||await sha256(disk)!==entry.sha256)throw Error('TH02 游戏磁盘校验失败');
 const fat=new Fat12(disk),main=fat.find('HUMAROKU/MAIN.EXE'),exe=fat.read(main);
 if(await sha256(exe)!==entry.mainSha256)throw Error('TH02 主程序校验失败');
 // Patch only this session's verified MAIN; shipped disks and art layouts stay pinned.
 const swap=await get('./state-swap-patch.json'),swapBase=patch.headerSize+patch.codeSegment*16;
 const swapBytes=hex=>Uint8Array.from(hex.match(/../g)||[],n=>parseInt(n,16));
 const swapEntry=swapBytes(swap.entryExpected),swapJump=swapBytes(swap.entryReplacement),swapCode=swapBytes(swap.replacement);
 if(swap.schema!==1||swap.codeSegment!==patch.codeSegment||swap.bankBytes!==62||swap.sourceMainSha256[config.language]!==entry.mainSha256||
    swap.entry<patch.entries.select_player||swap.entry+swapEntry.length>patch.entries.player_input||
    swap.target!==0x8660||swapJump.length!==3||swapEntry.length!==3||swapCode.length!==swap.bytes||swap.target+swap.bytes>0x8960||
    !swapEntry.every((b,i)=>exe[swapBase+swap.entry+i]===b)||
    !exe.subarray(swapBase+swap.target,swapBase+swap.target+swap.bytes).every(b=>b===0))throw Error('TH02 玩家状态交换补丁版本不匹配');
 exe.set(swapJump,swapBase+swap.entry);exe.set(swapCode,swapBase+swap.target);
 const rowCopy=await get('./row-copy-patch.json'),rowEdits=rowCopy.edits?.[config.language];
 if(rowCopy.schema!==1||rowCopy.codeSegment!==patch.codeSegment||rowCopy.entryImageOffset!==0x442a||
    rowCopy.targetCodeOffset!==0x8760||rowCopy.bytes!==169||rowCopy.targetCodeOffset+rowCopy.bytes>0x8960||
    rowCopy.rows!==16||rowCopy.stride!==80||!rowCopy.wordAccessOrderPreserved||
    rowCopy.sourceMainSha256[config.language]!==entry.mainSha256||!Array.isArray(rowEdits)||rowEdits.length!==4)throw Error('TH02 图形复制补丁版本不匹配');
 for(const edit of rowEdits){
  const expected=swapBytes(edit.expected),replacement=swapBytes(edit.replacement);
  if(!Number.isInteger(edit.fileOffset)||edit.fileOffset<0||edit.fileOffset+replacement.length>exe.length||
     expected.length!==replacement.length||!expected.every((b,i)=>exe[edit.fileOffset+i]===b))throw Error('TH02 图形复制原程序不匹配');
 }
 for(const edit of rowEdits)exe.set(swapBytes(edit.replacement),edit.fileOffset);
 const bulletBank=await get('./bullet-bank-patch.json');
 if(bulletBank.schema!==1||bulletBank.codeSegment!==patch.codeSegment||bulletBank.entry!==patch.entries.collision_0||
    bulletBank.target!==0x8810||bulletBank.bytes<=0||bulletBank.target+bulletBank.bytes>0x8960||
    !bulletBank.ordinaryOnly||!bulletBank.activeNonzeroFallback||!bulletBank.mailboxInactiveFallback||
    bulletBank.sourceMainSha256[config.language]!==entry.mainSha256)throw Error('TH02 普通弹碰撞补丁版本不匹配');
 const bulletExpected=swapBytes(bulletBank.entryExpected),bulletBody=swapBytes(bulletBank.replacement),bulletJump=swapBytes(bulletBank.entryReplacement);
 if(bulletExpected.length!==5||bulletJump.length!==5||bulletBody.length!==bulletBank.bytes||
    !bulletExpected.every((b,i)=>exe[swapBase+bulletBank.entry+i]===b)||
    !exe.subarray(swapBase+bulletBank.target,swapBase+bulletBank.target+bulletBank.bytes).every(b=>b===0))throw Error('TH02 普通弹碰撞原程序不匹配');
 exe.set(bulletJump,swapBase+bulletBank.entry);exe.set(bulletBody,swapBase+bulletBank.target);
 soundPolicy=await get('./sound-patch.json');
 for(const edit of soundPolicy.edits){
  const offset=patch.headerSize+edit.imageOffset,expected=edit.expected.match(/../g).map(n=>parseInt(n,16));
  if(!expected.every((b,i)=>exe[offset+i]===b))throw Error('TH02 音乐分离补丁版本不匹配');
 }
 for(const edit of soundPolicy.edits)exe.set(edit.replacement.match(/../g).map(n=>parseInt(n,16)),patch.headerSize+edit.imageOffset);
 const at=patch.headerSize+patch.codeSegment*16+patch.entries.run_config;
 exe[at]=config.p2;exe[at+1]=config.p3;exe[at+2]=players;fat.replace(main,exe);
 const core=config.rollback&&supportsWriteJournal()?'tracked':'plain',coreEntry=version.cores[core];
 await progress('下载并校验本局 WASM 核心',4);
 const wasmBinary=await assetBytes(coreEntry.file,{hash:coreEntry.sha256});
 if(await sha256(wasmBinary)!==coreEntry.sha256)throw Error('WASM 核心版本不匹配，请重新构建并刷新');
 const launcher=new Uint8Array(await get('./th02-launch.com','arrayBuffer'));
 if(await sha256(launcher)!==version.assets['netplay/th02-launch.com'])throw Error('TH02 网页启动器校验失败');
 await progress('初始化确定性 DOSBox-X 核心与音频',5);
 emulator=await DOSBoxPC98.create({canvas:nativeCanvas,artPatch:patch,lockstep:true,independentEffects:true,rollbackEnabled:config.rollback,lockstepEpoch:version.epoch,wasmBinary,
  ...(workerHost?{workerRuntime:true,audioContext:workerHost.audioContext}:{}),
  launchFiles:[{path:'game/HUMAROKU/WEBSTART.COM',contents:configureLauncher(launcher,config)}],
  onError:error=>notifyParent({protocol:PROTOCOL,event:'boot-error',message:error.message}),
  onExit:()=>notifyParent({protocol:PROTOCOL,event:'runtime-exit'})});
 emulator.module.playerArt.view(localSlot);
 if(workerHost)emulator.module.getHitPoints=hitPoints;
 if(closed){emulator.dispose();return;}
 if(emulator.audio.context.sampleRate!==44100)throw Error('浏览器未提供统一的 44100 Hz 音频，请使用新版 Chrome / Edge');
 sfx=workerHost?workerHost.sfx:new LocalSfx(emulator.audio.context);await sfx.load();
 if(closed){sfx.dispose();return;}
 await progress('挂载本局私有磁盘',6);
 emulator.addDiskImage('th02-sync.hdi',disk);await emulator.setHdd(0,'th02-sync.hdi');
 emulator.module.warmingBoot=true;emulator.run();const started=performance.now();
 try{
  const ready=await runBoot({step:()=>{
   emulator.step();
   refreshMailbox(emulator.tick%15===0);
   if(emulator.state==='failed'||emulator.state==='exited')throw Error('TH02 启动失败，请检查加载详情');
  },ready:()=>!!state(),tick:()=>emulator.tick,cancelled:()=>closed,progress:message=>progress(message,6)});
  if(!ready)return;
 }finally{emulator.module.warmingBoot=false;emulator.pause();}
 emulator.module.bootCPUMetrics=emulator.module.UTF8ToString(emulator.module._getCPUMetrics());
 if(emulator.module.bootCPUMetrics.split('|')[0]!=='30000')throw Error('TH02 联机周期未恢复为 30000');
 const initial=state();if(initial.stage!==config.stage||initial.players.length!==players||initial.players.some((p,i)=>p.lives!==config.lives||p.bombs!==config.bombs||p.shot!==config[`p${i+1}`]))throw Error('TH02 原生开局配置未生效');
 sfxSequence=readSfxSequence();
 writeInputs(emulator.module.HEAPU8,mailbox,patch,Array(players).fill(0),players,config);
 const view=new DataView(emulator.module.HEAPU8.buffer);consumedNative=consumedNative.map((_,i)=>view.getUint16(mailbox+138+i*2,true));
 config.boot={steps:emulator.tick,wallMs:Math.round(performance.now()-started)};
 await progress('关卡已就绪，等待全员同步开始',7);
 return {protocol:PROTOCOL,build:version.build,disk:entry.sha256,epoch:version.epoch,cycles:30000,bootSteps:emulator.tick,initial:checksum()};
}
function applyKeys(bits){
 for(const [code,bit]of nativeKeys)if((bits&bit)!==(oldP1&bit))emulator.key(code,!!(bits&bit));
 oldP1=bits;
}
function nativeInput(inputs){
 if(mailbox<0){pendingNative.fill(0);consumedNative.fill(0);return inputs;}
 const heap=emulator.module.HEAPU8,v=new DataView(heap.buffer),paused=nativePaused(heap,mailbox,patch);
 return inputs.map((value,i)=>{
  const ack=v.getUint16(mailbox+138+i*2,true);
  if(ack!==consumedNative[i]||paused||heap[mailbox+23]&(1<<i))pendingNative[i]=0;
  consumedNative[i]=ack;
  if(!paused){// Retain each unacknowledged axis independently until MAIN consumes it.
   if(value&3)pendingNative[i]=(pendingNative[i]&~3)|(value&3);
   if(value&12)pendingNative[i]=(pendingNative[i]&~12)|(value&12);
   pendingNative[i]|=value&112;}
  return value-(value&127)+((value&127)|pendingNative[i]);
 });
}
function readMusic(){
 const heap=emulator.module.HEAPU8,d=mailbox-patch.entries.mailbox+(patch.dataSegment-patch.codeSegment)*16,v=new DataView(heap.buffer);
 const sequence=v.getUint16(d+soundPolicy.sequence,true);let name='';
 for(let i=0;i<13&&heap[d+soundPolicy.filename+i];i++)name+=String.fromCharCode(heap[d+soundPolicy.filename+i]);
 name=/^(stage[0-5]|boss[1-5]|mima|op|end1|ending)\.m$/i.test(name)?name.slice(0,-2).toLowerCase():'';
 if(sequence===musicSequence&&name===musicName)return null;
 musicSequence=sequence;musicName=name;
 return {sequence,name,ax:v.getUint16(d+soundPolicy.command,true)};
}
function readSfxSequence(){
 const m=emulator.module,d=mailbox-patch.entries.mailbox+(patch.dataSegment-patch.codeSegment)*16;
 return new DataView(m.HEAPU8.buffer).getUint16(d+soundPolicy.effects.sequence,true);
}
function readSfx(){
 const sequence=readSfxSequence(),count=(sequence-sfxSequence)&65535,p=soundPolicy.effects;
 if(count>p.capacity)throw Error('TH02 单步音效事件越界');
 const h=emulator.module.HEAPU8,d=mailbox-patch.entries.mailbox+(patch.dataSegment-patch.codeSegment)*16,events=[];
 for(let i=0;i<count;i++){
  const id=h[d+p.ring+((sfxSequence+i)%p.capacity)];
  if(id>=p.count)throw Error(`TH02 原生音效编号无效：${id}`);events.push(id);
 }
 sfxSequence=sequence;return events;
}
function confirmedMusic(event){
 if(workerHost){workerHost.music(event);return;}
 if(event.name){
  if(playedName!==event.name){pendingMusic.push([0,event.name]);if((event.ax>>>8)!==0)pendingMusic.push([event.ax,event.name]);}
  else pendingMusic.push([event.ax,event.name]);
  playedName=event.name;
 }else if(playedName)pendingMusic.push([event.ax,playedName]);
 if(pendingMusic.length>64)pendingMusic.splice(0,pendingMusic.length-64);
 if(bgm?.manifest){for(const [ax,name]of pendingMusic.splice(0))bgm.commandTrack(ax,name);return;}
 if(!musicTask&&!musicError)musicTask=new Promise(resolve=>setTimeout(resolve,0)).then(async()=>{
  const {LocalBgm}=await import('./local-bgm.js');if(closed)return;
  bgm=new LocalBgm(emulator.audio.context,()=>{});bgm.setVolume(musicVolume);
  await bgm.load({warmOpening:false});if(closed)return;
  for(const [ax,name]of pendingMusic.splice(0))bgm.commandTrack(ax,name);
 }).catch(error=>{if(!closed)musicError=error.message;});
}
function step(inputs,{frame=ticks,replay=false,offlineMask=0}={}){
 if(!enabled)throw Error('TH02 尚未同步启动');
 if(!Number.isInteger(offlineMask)||offlineMask<0||offlineMask>=(1<<players))throw Error('无效离线席位');
 inputs=inputs.map((bits,i)=>offlineMask&(1<<i)?0:bits);
 const saved=states.get(frame);if(saved)saved.execution={inputs:inputs.slice(),offlineMask};
 const m=emulator.module;
 m.netBeginFrame(frame,replay);
 try{
  // MAIN is not resident throughout DOS boot. A full heap signature scan on
  // every step spent most startup work searching memory that cannot match.
  refreshMailbox(emulatedTicks%15===0);
  if(mailbox>=0){
   const offlineAt=mailbox-patch.entries.mailbox+patch.entries.offline_mask;
   m.thDirtyMark?.(offlineAt,1);m.HEAPU8[offlineAt]=offlineMask;
   m.thDirtyMark?.(mailbox,patch.players.mailboxSize);
   m.thDirtyMark?.(mailbox-patch.entries.mailbox+patch.entries.touch_state,2*patch.touch.stride+6);
  }
 writeInputs(m.HEAPU8,mailbox,patch,nativeInput(inputs),players,config);
  // Before MAIN and inside native pause, P1 operates original menus. Every
  // seat can request pause/confirm; all key events carry the virtual timestamp.
  const mapped=inputs.map(nativeBits);
  applyKeys(mapped[0]|mapped.slice(1).reduce((v,n)=>v|(n&192),0));
  previousInputs=inputs.slice();
  const nativeStarted=workerHost?performance.now():0;
  const pausedBefore=nativePaused(m.HEAPU8,mailbox,patch),previousMailbox=mailbox;
  emulator.step();
  workerHost?.recordNative?.(performance.now()-nativeStarted,frame,replay);
  ticks++;emulatedTicks++;
  refreshMailbox();
  if(emulator.state==='failed'||emulator.state==='exited')throw Error('TH02 模拟器停止运行');
  if(mailbox>=0&&!m.bootCPUMetrics){
   m.bootCPUMetrics=m.UTF8ToString(m._getCPUMetrics());
   if(m.bootCPUMetrics.split('|')[0]!=='30000')throw Error('TH02 联机周期未恢复为 30000');
  }
  // The original pause routine publishes its confirmed quit result in MAIN's
  // shared quit byte. Do not treat MAIN -> MAINE ending ownership as an exit.
  const quitAt=previousMailbox-patch.entries.mailbox+(patch.dataSegment-patch.codeSegment)*16+0x2b97;
  const exit=pausedBefore&&!nativePaused(m.HEAPU8,mailbox,patch)&&m.HEAPU8[quitAt]===1;
  effects.set(frame,{exit,music:mailbox>=0?readMusic():null,sounds:mailbox>=0?readSfx():[]});if(!replay)pending=true;
 }finally{m.netEndFrame();}
}
function capture(frame,restored=false){
 if(states.has(frame))throw Error('TH02 重复检查点');
 const previous=states.get(frame-1),nativeFrame=previous?.nativeFrame===frame-1?frame-1:frame;
 if(nativeFrame===frame)emulator.module.netCapture(frame,restored);
 states.set(frame,{nativeFrame,ticks,emulatedTicks,oldP1,mailbox,guestMemoryBase,residentAddress,gameplaySeen,previousInputs:previousInputs.slice(),pendingNative:pendingNative.slice(),consumedNative:consumedNative.slice(),musicSequence,musicName,sfxSequence});
}
function restore(frame){
 pending=false;const saved=states.get(frame);
 if(!saved||frame<confirmedPrefix)throw Error('TH02 检查点已过期');
 const from=saved.nativeFrame,checkpoint=states.get(from);
 if(!checkpoint||(from!==frame&&!checkpoint.execution))throw Error('TH02 检查点历史缺失');
 emulator.module.netRestore(from);
 ({ticks,emulatedTicks,oldP1,mailbox,guestMemoryBase,residentAddress,gameplaySeen}=checkpoint);previousInputs=checkpoint.previousInputs.slice();
 pendingNative=checkpoint.pendingNative.slice();consumedNative=checkpoint.consumedNative.slice();({musicSequence,musicName,sfxSequence}=checkpoint);
 for(const at of states.keys())if(at>=from)states.delete(at);
 for(const at of effects.keys())if(at>=from)effects.delete(at);
 if(from!==frame){
  capture(from,true);step(checkpoint.execution.inputs,{frame:from,replay:true,offlineMask:checkpoint.execution.offlineMask});checkpointReplays++;
  if(from<confirmedPrefix){effects.delete(from);emulator.module.netDiscardFrame(from);}
 }
}
function retained(prefix){const next=states.get(prefix);if(next)return next.nativeFrame;const previous=states.get(prefix-1);return previous?.nativeFrame===prefix-1?prefix-1:prefix;}
function confirm(prefix){
 let sound=null,exit=false;
 for(let frame=confirmedPrefix;frame<prefix;frame++){
  if(!effects.has(frame))throw Error('TH02 确认步缺少模拟结果');
  const effect=effects.get(frame);
  if(effect.exit)exit=true;
  emulator.module.netConfirm(frame+1,retained(frame+1));if(effect.music)confirmedMusic(effect.music);
  if(effect.sounds.length)sound=effect.sounds.at(-1);effects.delete(frame);
 }
 confirmedPrefix=prefix;const keep=retained(prefix);for(const frame of states.keys())if(frame<keep)states.delete(frame);
 // PMD has one SE voice. A confirmation burst starts only its newest event,
 // never a queue of obsolete sounds. Playback thereafter uses device time.
 if(sound!==null&&(workerHost||!document.hidden))sfx.play(sound);
 return exit?'exit':undefined;
}
function checksum(){
 let h=0x811c9dc5;const hash=n=>{h=Math.imul(h^n,0x01000193);};
 hash(ticks);hash(emulatedTicks);hash(oldP1);previousInputs.forEach(n=>{hash(n%4294967296);hash(Math.floor(n/4294967296));});
 pendingNative.forEach(hash);consumedNative.forEach(hash);hash(musicSequence);hash(sfxSequence);for(const c of musicName)hash(c.charCodeAt(0));
 if(guestMemoryBase>=0)h=hashNativeMemory(emulator.module.nativeExports().memory,guestMemoryBase,guestMemoryBase+0xa0000,h);
 return {tick:ticks,hash:(h>>>0).toString(16).padStart(8,'0'),gameFrame:state()?.gameFrame||0};
}
function hitPoints(){
 const s=state(),p=s?.players[localSlot];
 if(!raw&&p&&!p.hit&&!p.out&&!nativePaused(emulator.module.HEAPU8,mailbox,patch)&&(points.always||(points.focus&&(previousInputs[localSlot]&64)))){
  // Native rendering adds 0x28d4 hardware scroll; scanout removes it. The
  // published player coordinates are already the visible screen position.
  return [[p.x+16,p.y+21]];
 }
 return [];
}
function present(){
 if(!enabled||!pending||!emulator.module.netPresent())return false;
 // Like TH05, the Worker transfers its rendered WebGL canvas directly.
 // The compatibility backend retains its page-owned 2D canvas.
 if(ctx){ctx.drawImage(nativeCanvas,0,0,640,400);for(const [x,y]of hitPoints()){
  ctx.fillStyle='#000';ctx.fillRect(x-2,y-2,5,5);ctx.fillStyle='#fff';ctx.fillRect(x-1,y-1,3,3);
 }}
 pending=false;presentationCount++;return true;
}
function predictionBlockReason(){return !state()?'原版菜单或关卡加载':nativePaused(emulator.module.HEAPU8,mailbox,patch)?'原生暂停菜单':'';}
function diagnosticText(){const s=state(),info=sfx?.info();return `TH02 · 同步步 ${ticks} · 游戏帧 ${s?.gameFrame??'菜单'} · 画面提交 ${presentationCount}\n团队分数 ${s?.players.map(p=>`P${p.id} ${p.score}`).join(' / ')||'等待入关'}\n本地完整音效 ${info?.played??0} 次 · 核心固定 30000 cycles · ${version?.th02IdleOptimization?'TH02 等价忙等优化':'通用核心'}\n浏览器 BGM：${musicError||bgm?.status||'等待确认关卡'} · 浏览器独立音效：${info?.ready?'就绪':'加载中'}`;}

// Portable captures belong exclusively to a verified replay Worker, never a player.
let portableReplay,portableBase;
function portableBoundary(){
 if(!workerHost?.replayOnly||effects.size||confirmedPrefix!==ticks||emulator.snapshots)throw Error('完整检查点仅允许在独立旁观实例的确认边界');
}
async function replaySave(identity){
 portableBoundary();const {createPortableCheckpoint,encodeCheckpoint}=await import('../replay/portable.js');
 portableReplay??=createPortableCheckpoint(emulator.module,emulator.ci);
 const host={identity,frame:ticks,ticks,emulatedTicks,oldP1,mailbox,guestMemoryBase,residentAddress,gameplaySeen,previousInputs,pendingNative,consumedNative,musicSequence,musicName,sfxSequence,playedName,pendingMusic,musicError,emulator:{tick:emulator.tick,keys:[...emulator.keys],state:emulator.state},audio:workerHost.captureAudioState?.(),proof:sync.replayProof(),check:checksum()};
 const state=portableReplay.capture(host),bytes=await encodeCheckpoint(state,ticks?portableBase:null);
 if(!ticks)portableBase=state;
 return {frame:ticks,bytes,proof:host.proof,check:host.check};
}
async function replayLoad(bytes,baseBytes,identity){
 portableBoundary();const {createPortableCheckpoint,decodeCheckpoint}=await import('../replay/portable.js');
 portableReplay??=createPortableCheckpoint(emulator.module,emulator.ci);
 const base=baseBytes?await decodeCheckpoint(baseBytes):null,state=await decodeCheckpoint(bytes,base),h=state.host;
 if(h?.identity!==identity||!Number.isSafeInteger(h.frame)||h.frame<0||h.frame>216000||h.frame!==h.ticks)throw Error('完整检查点身份或帧号无效');
 portableReplay.restore(state);({ticks,emulatedTicks,oldP1,mailbox,guestMemoryBase,residentAddress,gameplaySeen,previousInputs,pendingNative,consumedNative,musicSequence,musicName,sfxSequence,playedName,pendingMusic,musicError}=h);
 emulator.tick=h.emulator.tick;emulator.keys=new Set(h.emulator.keys);emulator.state=h.emulator.state;emulator.outputs.clear();
  confirmedPrefix=ticks;states.clear();effects.clear();pending=true;
 if(sync.replayProof()!==h.proof||JSON.stringify(checksum())!==JSON.stringify(h.check))throw Error('完整检查点恢复校验不一致');
  workerHost.restoreAudioState?.(h.audio);
 return {frame:ticks,proof:h.proof,check:h.check};
}

const sync={
 configure(host,local,count=2){if(![2,3].includes(count)||![host,local].every(n=>Number.isInteger(n)&&n>=0&&n<count))throw Error('无效联机座位');localSlot=local;players=count;emulator?.module.playerArt?.view(localSlot);},
 load(settings){return bootPromise??=boot(settings);},
 start(){if(closed)return;emulator.run();enabled=true;canvas.focus?.();},
 stop(){enabled=false;closed=true;clearKeys();bgm?.dispose();sfx?.dispose();emulator?.module.netFlushAudio();emulator?.pause();},
 onInput(callback){inputListener=callback;},onMenuAction(){},setMusicVolume(value){musicVolume=Math.max(0,Math.min(1,Number(value)||0));bgm?.setVolume(musicVolume);},
 updateControls(value,blocked=false){clearKeys();useControlSnapshot(value);editing=blocked;if(!workerHost)document.getElementById('controls-summary').textContent=controlsSummary('online');},
 setPointPreferences(value){points={...value};},setRawPresentation(value){raw=!!value;pending=true;},
 async unlockAudio(){const context=emulator?.audio.context;if(context?.state!=='running')await context?.resume();return context?.state==='running';},
 touchContext(){const s=state(),p=s?.players[localSlot],paused=emulator&&nativePaused(emulator.module.HEAPU8,mailbox,patch);return {key:`${s?.generation??0}:${!s||paused?'menu':'game'}`,play:!!p&&!p.hit&&!p.out&&!paused};},
 replayProof:()=>JSON.stringify([ticks,emulatedTicks,oldP1,mailbox,guestMemoryBase,residentAddress,gameplaySeen,previousInputs,pendingNative,consumedNative,musicSequence,musicName,sfxSequence,state()]),
 replayScene(){const s=state();return s?{kind:'stage',stage:s.stage}:null;},
 replaySave,replayLoad,
 snapshot:state,checksum,step,present,capture,restore,confirm,
 readyForRollback:()=>gameplaySeen&&!!emulator?.module.netCapture,
 canPredict:()=>!predictionBlockReason(),predictionBlockReason,
 rollbackInfo:()=>({...emulator?.module.netSnapshotInfo(),checkpointFrames:2,checkpointReplays}),
 diagnosticText,renderInfo:()=>emulator?.module.framePresenter.info(),
 faultReport:()=>({settings:config,game:state(),diagnostics:diagnosticText(),music:{error:musicError,status:bgm?.status,track:bgm?.active?.id,sequence:musicSequence,nativeBgm:false,nativeEffects:false},sound:{...sfx?.info(),sequence:sfxSequence},input:{pendingNative,consumedNative},logs:emulator?.logs,build:version?.build})
};
if(workerHost)workerHost.runtime=sync;
else{
window.th02Sync=sync;
document.getElementById('sound').onclick=()=>window.th02Sync.unlockAudio();
window.addEventListener('pagehide',()=>window.th02Sync.stop());
}
