import {mountPauseMenu} from './pc98-pause-menu.mjs';
import {createSoloAttempt} from './practice-attempt.js';
import {mountPractice} from './practice-ui.js';
import {normalizePractice,encodeNativeOptions,decodeNativeOptions} from './practice-config.js';
import {mountManagedPractice} from './pc98-practice.mjs';
import {Fat12} from './solo-fat12.js';
import {decodeExternalReplay} from './replay/external-codec.js';
import {userPath} from './managed-storage.mjs';

const host=document.getElementById('game'),input=globalThis.__eaglerPC98Input;
const nativeKeys=[['ArrowUp',1],['ArrowDown',2],['ArrowLeft',4],['ArrowRight',8],['KeyX',16],['KeyZ',32],['Escape',128],['Enter',256]];
const residentSignature=new TextEncoder().encode('MIKOConfig\0');
const sources=['../managed-gameover.mjs','../managed-rapid-fire.mjs','../practice-attempt.js','../direct-drag.js','../player-ui.js','../solo.js','../dosbox-adapter.js','../dosbox-disk.js','../frame-presenter.js','../solo-patch.json','../vendor/dosbox-x/wdosbox-x-fast.wasm','../vendor/dosbox-x/emulators.js','../vendor/dosbox-x/dosbox-jspi.mjs',
  '../touch-input.js','../control-settings.js','./external-host.js','./external-loader.js','./external-codec.js','./external-recorder.js','./external-ui.js','./external-lifecycle.js',
  '../practice-config.js','../practice-engine.js','../practice-assets.js','../practice-assets.json','../practice-core.json','../practice-start.com','../practice-cn.exe.gz','../practice-jp.exe.gz','../vendor/dosbox-x/wdosbox-x-practice.wasm',
  '../solo-music.js','../solo-music.json','../solo-music.com','../solo-native-music.js','../netplay/local-bgm.js','../bgm/manifest.json'];
let canvas=document.getElementById('canvas'),attempt,retrySlot,retryBlocked=false,lastRetry=null,emulator,music,recorder,patch,mailbox=-1,cursor=0,lastKeys=0,loop=0,playback=false,options={},language='jp',musicMode='ogg',recordBlob,lastPractice,currentSettings,suppressedLoad=null;
const report=error=>globalThis.__eaglerPC98Emit({event:'error',message:String(error?.message||error)});
function valid(heap,at){
  if(!patch||at<0||at+104>heap.length)return false;
  for(let i=0;i<patch.signature.length;i++)if(heap[at+i]!==patch.signature.charCodeAt(i))return false;
  const ds=heap[at+20]|heap[at+21]<<8,data=at-patch.entries.mailbox+(patch.dataSegment-patch.codeSegment)*16,base=data-ds*16;
  if(ds<=patch.dataSegment||base<0||base+0x100000>heap.length||data<base||data+0x2800>base+0xa0000||heap[at+22]>2)return false;
  const view=new DataView(heap.buffer,heap.byteOffset,heap.byteLength),resident=base+view.getUint16(data+0x27fc,true)+view.getUint16(data+0x27fe,true)*16;
  return resident>=base&&resident+48<=base+0xa0000&&residentSignature.every((b,i)=>heap[resident+i]===b)&&heap[resident+11]<=5&&heap[resident+39]===0;
}
function readState(){
  if(!emulator)return null;
  if(suppressedLoad!==null){if(emulator.practice?.loadCount===suppressedLoad)return null;suppressedLoad=null;}
  const heap=emulator.module.HEAPU8;
  if(!valid(heap,mailbox)){
    mailbox=-1;const end=Math.min(heap.length,cursor+2*1024*1024);
    for(let at=heap.indexOf(84,cursor);at>=0&&at<end;at=heap.indexOf(84,at+1))if(valid(heap,at)){mailbox=at;break;}
    cursor=end===heap.length?0:end;
  }
  if(mailbox<0)return null;
  const view=new DataView(heap.buffer),data=mailbox-patch.entries.mailbox+(patch.dataSegment-patch.codeSegment)*16;
  return {mode:heap[mailbox+22],generation:view.getUint16(mailbox+24,true),ticks:view.getUint32(mailbox+16,true),x:view.getInt16(data+0x2b86,true),y:view.getInt16(data+0x2b88,true),lives:heap[data+0xb2c],bombs:heap[data+0xb2d],hit:heap[data+0x2b94]|heap[data+0xaa7],power:heap[data+0xaa6]};
}
function applyAssist(value){
  if(!emulator)return;const at=value.address,heap=emulator.module.HEAPU8;if(!valid(heap,at))return;
  const view=new DataView(heap.buffer),base=at-patch.entries.mailbox;if(value.generation!==view.getUint16(at+24,true))return;
  const e=patch.practice?.entries;
  if(value.formOpen){const f=value.formOpen;if(!e||heap[base+e.resultVisible])return;if(f!==true){if(!Array.isArray(f.values)||f.values.length!==10||!Array.isArray(f.settings)||f.settings.length!==11||!Number.isInteger(f.hp)||f.hp<0||f.hp>8500||f.values.some(n=>!Number.isInteger(n)||n<0||n>255)||f.settings.some((n,i)=>!Number.isInteger(n)||n<0||n>(i===10?5:1)))return;heap.set(f.values,base+e.formValues);heap.set(f.settings,base+e.formOptions);view.setUint16(base+e.formHp,f.hp,true);}heap[base+e.formRequest]=0;heap[base+e.formOpen]=1;return;}
  if(Number.isInteger(value.formInput)){if(e&&(heap[base+e.formOpen]||value.formInput===64))view.setUint16(base+e.formInput,value.formInput&127,true);return;}
  if(Number.isInteger(value.pauseInput)){if(e){if(value.openPause&&!heap[base+e.resultVisible])heap[base+e.pauseOpen]=1;if(heap[base+e.pauseOpen])view.setUint16(base+e.pauseInput,value.pauseInput&15,true);}return;}
  if(Number.isInteger(value.pauseAck)){if(e&&heap[base+e.pauseOpen]&&heap[base+e.pauseRequest])heap[base+e.pauseAck]=value.pauseAck;return;}
  if(Number.isInteger(value.resultInput)){if(e&&heap[base+e.resultVisible])view.setUint16(base+e.resultInput,value.resultInput&31,true);return;}
  if(Number.isInteger(value.resultAck)){if(e&&heap[base+e.resultVisible]&&heap[base+e.resultRequest])heap[base+e.resultAck]=value.resultAck;return;}
  if(value.resultRetry===true){if(e&&heap[base+e.resultVisible]&&!heap[base+e.resultRequest])heap[base+e.resultRequest]=1;return;}
  if(value.quit||value.restart){
    const data=base+(patch.dataSegment-patch.codeSegment)*16,ds=view.getUint16(at+20,true),ram=data-ds*16;
    if(ram<0||ram+0x100000>heap.length||data<ram||data+0x2800>ram+0xa0000)return;
    if(value.restart){
      // Let MAIN perform its normal cleanup, then load a fresh MAIN.EXE in
      // the same DOS session instead of reusing the current stage's objects.
      const resident=ram+view.getUint16(data+0x27fc,true)+view.getUint16(data+0x27fe,true)*16,branch=base+0x10e;
      if(resident<ram||resident+48>ram+0xa0000||!residentSignature.every((b,i)=>heap[resident+i]===b)||heap[branch]!==0xe8||heap[branch+1]!==0xae||heap[branch+2]!==0x10||heap[base+0x13b]!==0x1e||heap[base+0x13c]!==0x68||view.getUint16(base+0x13d,true)!==0x2da)return;
      const config=value.practiceConfig?normalizePractice(value.practiceConfig):emulator.config.practice,stage=config?.mode==='target'?config.setup.stage:heap[resident+22]===4?5:0;
      if(!Number.isInteger(e?.restartPending)||!Number.isInteger(e?.restartValues))return;
      const s=config.setup;heap.set([stage,s.difficulty,s.character,s.lives,s.bombs,value.practiceConfig?1:0],base+e.restartValues);heap[base+e.restartPending]=1;
      if(value.practiceConfig)emulator.config.practice=config;
      emulator.restartPractice(true);
    }else{
      // Skip MAIN's game-over/continue prompt and let it execute OP directly.
      const branch=base+0x10e;
      if(heap[branch]!==0xe8||heap[branch+1]!==0xae||heap[branch+2]!==0x10)return;
      heap.set([0xe9,0x09,0x00],branch);emulator.practice?.exitPractice();
    }
    if(!value.restart)heap[data+0x2b97]=1;return;
  }
  view.setUint16(base+patch.entries.currentInput,value.currentInput,true);heap[at+100]=50;heap[at+102]=value.focusEnabled?1:0;heap[at+103]=value.points;
  const touch=base+patch.entries.touch;
  if(value.touchAction==='add'){
    const clamp=n=>Math.max(-8192,Math.min(8191,n));if(value.begin){view.setInt32(touch+2,0,true);view.setInt32(touch+8,0,true);}
    view.setUint16(touch,value.unlimited?3:1,true);view.setInt16(touch+2,clamp(view.getInt16(touch+2,true)+value.x),true);view.setInt16(touch+4,clamp(view.getInt16(touch+4,true)+value.y),true);
  }else if(value.touchAction==='clear'){view.setUint16(touch,0,true);view.setInt32(touch+2,0,true);}
}
function release(){input.releaseGameplay();if(emulator)for(const [code,bit] of nativeKeys)if(lastKeys&bit)emulator.key(code,false);lastKeys=0;}
let replayResult=null;
let nativeResultBusy=false,nativeResultHandled=0;
let nativePauseBusy=false,nativePauseHandled=0;
let nativeTransitionHolding=false;
let pausePrevious=0;
const pauseMenu=mountPauseMenu(host.querySelector('.solo-screen'),{
  resume(){pauseMenu.close();input.clear();pausePrevious=0;emulator?.run();music?.resume();void attempt?.sfx?.resume();canvas.focus();},
  exit(){transition(false);},
  save(){transition(true);},
  discard(){transition(false);},
  restart(){transition(false,true);},
});
function transition(save,restarting=false){const current=emulator;void returnToTitle(save,restarting,current).catch(error=>{if(emulator===current){pauseMenu.open();report(error);}});}
const savedReplays=new WeakMap();
async function saveReplay(blob){recordBlob=blob;if(savedReplays.has(blob))return savedReplays.get(blob);const task=(async()=>{const name=`replay/th02_${Date.now()}_${crypto.randomUUID().slice(0,8)}.th02r`;await globalThis.__eaglerPC98Files.write(name,await blob.arrayBuffer());return name;})();savedReplays.set(blob,task);return task;}
async function saveRecording(){if(!recorder?.state.ready)return null;return saveReplay(await recorder.save());}
async function syncFiles(){if(!emulator||playback)return;const fat=new Fat12(await emulator.getDiskImage('th02-solo.hdi'));
  for(const name of ['HUUHI.DAT','HUUMA.CFG']){const entry=fat.find('HUMAROKU/'+name,{optional:true});if(entry!==undefined)await globalThis.__eaglerPC98Files.write(name,fat.read(entry));}}
async function returnToTitle(save,restarting=false,current=emulator,practiceConfig=null){
  if(practiceConfig&&nativeTransitionHolding)return;
  if(practiceConfig){nativeTransitionHolding=true;current?.externalHost?.pause(true);}
  try{
  pauseMenu.close();
  const recording=recorder;if(recording?.state.failed&&!save){recording.dispose();recorder=undefined;if(attempt)attempt.recorder=null;}else if(recording){recording.finish();const until=performance.now()+10000;while(emulator===current&&!recording.state.ready&&!recording.state.failed&&performance.now()<until)await new Promise(resolve=>setTimeout(resolve,25));if(emulator!==current)return;if(!recording.state.ready)throw Error('录像未能完成');if(save)await saveRecording();if(emulator!==current)return;await recording.rearm(!!practiceConfig);}
  if(emulator!==current)return;const state=readState();if(!state||state.mode!==1||!valid(emulator.module.HEAPU8,mailbox))throw Error('当前对局无法退出');
  const branch=mailbox-patch.entries.mailbox+0x10e,expected=[0xe8,0xae,0x10];
  if(!expected.every((b,i)=>emulator.module.HEAPU8[branch+i]===b))throw Error('当前对局无法退出');
  discardRetry();if(!restarting){practiceUI.setRunning(false);lastPractice=normalizePractice(practiceUI.configuration());}
  await syncFiles();if(emulator!==current)return;emulator.assist({address:mailbox,generation:state.generation,...(restarting?{restart:true,...(practiceConfig?{practiceConfig}:{} )}:{quit:true})});if(practiceConfig){lastPractice=practiceConfig;practiceUI.setRunning(true,lastPractice);}if(!restarting){suppressedLoad=emulator.practice?.loadCount??null;mailbox=-1;cursor=0;}input.clear();pausePrevious=0;emulator.run();music?.resume();void attempt?.sfx?.resume().catch(()=>{});canvas.focus();
  }finally{if(practiceConfig){nativeTransitionHolding=false;current?.externalHost?.pause(false);}}
}
async function naturalExit(){await syncFiles();globalThis.__eaglerPC98Emit({event:'exit',code:0});}
const practiceUI=mountPractice({host,onNativeOpen:openNativeForm,onRelease:release,onFocus:()=>canvas.focus(),onChange:value=>{emulator?.practiceOptions(value);},
  onStart:value=>void startPractice(value).catch(report),onRetry:()=>void startPractice({...lastPractice,options:practiceUI.options}).catch(report),onSave:()=>void saveRecording().catch(report),
  onPause:()=>{if(emulator&&!pauseMenu.visible){emulator.key('Escape',true);setTimeout(()=>emulator?.key('Escape',false),120);}}});
const managedPractice=mountManagedPractice(practiceUI,input,{release,nativeOpen:openNativeForm,nativeRetry:()=>{const state=readState();if(state)emulator.assist({address:mailbox,generation:state.generation,formInput:64});},nativeKey:(code,down)=>emulator?.key(code,down),readState:()=>({...readState(),practice:emulator?.practice?.status})});
async function startPractice(value){const config=normalizePractice(value),state=readState(),ps=emulator?.practice?.status;if(!state||playback)return restart(config);if(state.mode!==1||ps?.nativeForm?.visible)return;if(ps?.nativeResult?.visible){emulator.assist({address:mailbox,generation:state.generation,resultRetry:true});return;}await returnToTitle(false,true,emulator,config);}
function openNativeForm(){
  const state=readState(),e=patch?.practice?.entries;if(!emulator||playback)return;
  if(!state||state.mode!==1||!valid(emulator.module.HEAPU8,mailbox)||!Number.isInteger(e?.formOpen))return;
  const h=emulator.module.HEAPU8,base=mailbox-patch.entries.mailbox;
  if(h[base+e.resultVisible])return;
  emulator.assist({address:mailbox,generation:state.generation,formOpen:true});release();
}
function completeNativeForm(ps){
  const form=ps?.nativeForm,e=patch?.practice?.entries;if(!form?.request||!emulator||!valid(emulator.module.HEAPU8,mailbox))return;
  emulator.module.HEAPU8[mailbox-patch.entries.mailbox+e.formRequest]=0;
  try{const options=decodeNativeOptions(form.settings,ps.options);emulator.practiceOptions(options);lastPractice={...lastPractice,options};if(form.request===2)return;
  const [stage,character,difficulty,entry,phase,lives,bombs,power,,rank]=form.values;
  const config=normalizePractice({...lastPractice,mode:'target',setup:{...lastPractice.setup,stage,character,difficulty,entry:['stage','mid','boss'][entry],phase,lives,bombs,power,rank:rank-6,hp:form.hp},options});void returnToTitle(false,true,emulator,config).catch(report);}catch(error){report(error);}
}
async function handleNativeResult(ps,state){
  const request=ps?.nativeResult?.request||0;if(!request){nativeResultHandled=0;return;}
  if(nativeResultBusy||nativeResultHandled===request||!state)return;
  nativeResultBusy=true;nativeResultHandled=request;const current=emulator,at=mailbox,generation=state.generation;
  const acknowledge=ack=>{if(emulator===current)current.assist({address:at,generation,resultAck:ack});};
  try{
    if(request===1){await returnToTitle(false,true,current,normalizePractice({...lastPractice,options:ps.options}));acknowledge(1);}
    else if(request===2){nativeTransitionHolding=true;current.externalHost?.pause(true);try{recorder?.finish();const until=performance.now()+10000;while(recorder&&!recorder.state.ready&&!recorder.state.failed&&performance.now()<until)await new Promise(resolve=>setTimeout(resolve,25));if(!await saveRecording())throw Error('录像尚未就绪');await recorder.rearm(true);acknowledge(1);}finally{nativeTransitionHolding=false;current.externalHost?.pause(false);}}
    else acknowledge(2);
  }catch(error){acknowledge(2);report(error);}finally{nativeResultBusy=false;}
}
async function handleNativePause(ps,state){
  const request=ps?.nativePause?.request||0;if(!request){nativePauseHandled=0;return;}
  if(nativePauseBusy||nativePauseHandled===request||!state)return;
  nativePauseBusy=true;nativePauseHandled=request;const current=emulator,at=mailbox,generation=state.generation;
  const ack=value=>{if(emulator===current)current.assist({address:at,generation,pauseAck:value});};
  try{await returnToTitle(request===1,request===3,current);ack(1);}catch(error){ack(2);report(error);}finally{nativePauseBusy=false;}
}
function tick(now){
  if(!emulator)return;const frameState=readState(),sample=input.sample({play:!pauseMenu.visible&&frameState?.mode===1&&!frameState.hit}),edge=sample&~pausePrevious;pausePrevious=sample;
  if(pauseMenu.visible){if(edge&1)pauseMenu.move(-1);if(edge&2)pauseMenu.move(1);if(edge&(32|256))pauseMenu.confirm();else if(edge&(128|16))pauseMenu.confirmResume();loop=requestAnimationFrame(tick);return;}
  const state=frameState,ps=emulator.practice?.status;practiceUI.update({practice:ps,retryReady:!!retrySlot?.ready});prepareRetry(ps);
  if(!playback&&ps?.mode==='target'&&ps.setup&&(lastPractice?.mode!=='target'||JSON.stringify(lastPractice.setup)!==JSON.stringify(ps.setup))){lastPractice=normalizePractice({...lastPractice,mode:'target',setup:ps.setup,options:ps.options});practiceUI.setRunning(true,lastPractice);}
  if(state&&valid(emulator.module.HEAPU8,mailbox)&&Number.isInteger(patch.practice?.entries.pauseOpen)){
    const openPause=!!((edge&128)&&state.mode===1&&!ps?.nativePause?.visible&&!ps?.nativeResult?.visible&&!ps?.nativeForm?.visible&&!playback);
    if(openPause||ps?.nativePause?.visible)emulator.assist({address:mailbox,generation:state.generation,openPause,pauseInput:(sample&1?1:0)|(sample&2?2:0)|(sample&(32|256)?4:0)|(sample&(128|16)?8:0)});
  }
  if(state&&ps?.nativeForm?.visible&&valid(emulator.module.HEAPU8,mailbox)){
    const e=patch.practice.entries,base=mailbox-patch.entries.mailbox;
    emulator.assist({address:mailbox,generation:state.generation,formInput:(sample&1?1:0)|(sample&2?2:0)|(sample&4?4:0)|(sample&8?8:0)|(sample&(32|256)?16:0)|(sample&(128|16)?32:0)});
  }
  if(ps?.nativeForm?.request){completeNativeForm(ps);loop=requestAnimationFrame(tick);return;}
  if(state?.mode===1&&ps?.nativePause?.visible){void handleNativePause(ps,state);}
  if(!nativeTransitionHolding&&ps?.nativeResult?.visible&&emulator.state==='running'&&emulator.externalHost?.status.held)emulator.externalHost.pause(false);
  if(ps?.nativeResult?.visible&&state&&valid(emulator.module.HEAPU8,mailbox)){
    const resultInput=(sample&1?1:0)|(sample&2?2:0)|(sample&32?4:0)|(sample&(128|16)?8:0)|(sample&256?16:0);
    emulator.assist({address:mailbox,generation:state.generation,resultInput});
  }

  if(ps?.outcome&&!playback){if(ps.nativeResult){void handleNativeResult(ps,state);}else practiceUI.complete({outcome:ps.outcome,recording:!!recorder,ready:!!recorder?.state.ready,failed:recorder?.state.failed});}
  if((edge&128)&&state?.mode===1&&!Number.isInteger(patch.practice?.entries.pauseOpen)&&!document.documentElement.dataset.browserPracticeOpen?.includes('true')&&!playback){release();emulator.pause();music?.suspend();void attempt?.sfx?.context.suspend();input.clear();pausePrevious=0;pauseMenu.open();loop=requestAnimationFrame(tick);return;}
  if(emulator.state==='running'&&!playback){
    const playing=state?.mode===1&&!state.hit&&!ps?.nativeForm?.visible;let bits=ps?.nativeForm?.visible?0:sample;
    if(playing){
      if(Number.isInteger(patch.practice?.entries.pauseOpen))bits&=~128;
      if(ps?.options.disableBomb)bits&=~16;if(ps?.options.disableShot)bits&=~32;if(ps?.options.disableFocus)bits&=~64;else if(ps?.options.forceFocus)bits|=64;
    }
    if((bits&3)===3)bits&=~3;if((bits&12)===12)bits&=~12;
    for(const [code,bit] of nativeKeys)if(!!(bits&bit)!==!!(lastKeys&bit))emulator.key(code,!!(bits&bit));lastKeys=bits;
    if(state){const motion=input.motion();emulator.assist({address:mailbox,generation:state.generation,currentInput:bits&64?0x100:0,focusEnabled:!ps?.options.disableFocus,points:1|(options.alwaysHitbox||ps?.options.alwaysPoint?8:0),
      touchAction:playing&&motion.active?'add':'clear',begin:!!motion.begin,unlimited:!!motion.unlimited,x:motion.x||0,y:motion.y||0});}
    music?.poll(emulator.module.HEAPU8,now);
  }
  if(music){if(document.hidden||emulator.state!=='running'||state?.mode===2||ps&&(ps.preparing||ps.waiting||ps.outcome))music.suspend();else music.resume();}
  loop=requestAnimationFrame(tick);
}
function discardRetry(){retrySlot?.stop();retrySlot=undefined;}
function makeAttempt(target,practice,settings,replay=null,standby=false,effects=null){return createSoloAttempt({canvas:target,practice,settings,replay,standby,effects,deferReplay:true,sources,applyReplayInput:applyAssist,callbacks:{onError:report,onExit:()=>void naturalExit().catch(report),onPlaybackEnd:matches=>{replayResult=matches;if(!matches)report(Error('Replay 终态内存校验不一致'));}}});}
function prepareRetry(){discardRetry();}
async function launch(config,replay=null,cached=null,effects=null){
  suppressedLoad=null;playback=!!replay;replayResult=null;lastPractice=normalizePractice(replay?.header.practice||config);practiceUI.setRunning(!playback,lastPractice);
  const settings={language:replay?.header.language||language,audioMode:replay?.header.audioMode||'independent',runtime:'main',memoryScope:'pc98-guest-ram/1',exitOnGameQuit:true,recordEnabled:!playback,volume:musicMode==='none'?0:.7};
  currentSettings=settings;attempt=cached||makeAttempt(canvas,lastPractice,settings,replay,false,effects);
  const started=performance.now(),cachedReady=!!cached?.ready;await attempt.task;if(cached)await attempt.core.activatePractice(practiceUI.options);
  if(cached){canvas.replaceWith(attempt.canvas);canvas=attempt.canvas;lastRetry={cached:true,cachedReady,milliseconds:Math.round(performance.now()-started)};}
  emulator=attempt.core;patch=attempt.patch;music=attempt.music;recorder=attempt.recorder;attempt.foreground=true;
  if(emulator.practice?.debug.csBase>=0)mailbox=emulator.practice.debug.csBase+patch.entries.mailbox;
  canvas.focus();managedPractice.start({readOnly:playback});loop=requestAnimationFrame(tick);
}
async function restart(config,retry=false){const cached=retry?retrySlot:undefined;if(cached)retrySlot=undefined;discardRetry();release();cancelAnimationFrame(loop);const effects=!cached&&!playback?attempt?.takeEffects():null;const stopped=attempt?.stop();emulator=music=recorder=undefined;mailbox=-1;cursor=0;suppressedLoad=null;recordBlob=undefined;retryBlocked=false;await stopped;await launch(config,null,cached,effects);}
export const managedAPI={
  async start(value){options=value.options;language=value.language==='ja'?'jp':'cn';musicMode=value.music;let replay;
    if(options.replayPath){const bytes=globalThis.__eaglerPC98Files.memory.get(userPath(options.replayPath));if(!bytes)throw Error('录像不存在');replay=await decodeExternalReplay(new Blob([bytes]),'02');}
    await launch(practiceUI.configuration(),replay);},
  health(){return {replayResult,gameFrame:readState()?.ticks,frames:emulator?.module.renderMetrics?.drawn??emulator?.module.framePresenter?.info().drawn??0,audio:{state:emulator?.audio.context.state,musicStatus:music?.player.status??null,effectsPeak:attempt?.sfx?.peak??0}};},
  practiceState(){return {...managedPractice.state(),loading:attempt?.timings,recordingReady:!!recorder?.state.ready,retry:{state:retrySlot?(retrySlot.ready?'ready':'preparing'):retryBlocked?'unavailable':'idle',last:lastRetry}};},
  finishRecording(){recorder?.finish();},
  saveRecording,
  async files(message){const store=globalThis.__eaglerPC98Files;if(message.command==='sync'){await syncFiles();return {};}
    if(message.command==='list'){await syncFiles();return {files:[...store.memory].map(([path,bytes])=>({path,size:bytes.length}))};}
    const path=userPath(message.path);if(message.command==='read'){await syncFiles();const bytes=store.memory.get(path);if(!bytes)throw Error('文件不存在');return {bytes:Array.from(bytes)};}
    if(emulator)throw Error('请退出游戏后管理文件');if(message.command==='remove'){await store.remove(path);return {};}
    if(message.command==='write'){const bytes=new Uint8Array(message.bytes);if(path.endsWith('.th02r'))await decodeExternalReplay(new Blob([bytes]),'02');await store.write(path,bytes);return {};}
    throw Error('用户文件操作无效');},
  async playback(message){if(message.action!=='open'||emulator)throw Error('请从启动器打开录像');const bytes=globalThis.__eaglerPC98Files.memory.get(userPath(message.path));if(!bytes)throw Error('录像不存在');await launch(practiceUI.configuration(),await decodeExternalReplay(new Blob([bytes]),'02'));return {};},
  stop(){pauseMenu.close();pausePrevious=0;managedPractice.stop();discardRetry();release();cancelAnimationFrame(loop);attempt?.stop();emulator=music=recorder=undefined;mailbox=-1;suppressedLoad=null;},
};
