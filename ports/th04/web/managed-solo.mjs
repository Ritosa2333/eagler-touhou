import {mountPauseMenu} from './pc98-pause-menu.mjs';
import {createSoloAttempt} from './practice-attempt.js';
import {mountPractice} from './practice-ui.js';
import {normalizePractice,encodeNativeOptions,decodeNativeOptions} from './practice-config.js';
import {mountManagedPractice} from './pc98-practice.mjs';
import {Fat12} from './solo-fat12.js';
import {decodeExternalReplay} from './replay/external-codec.js';
import {downloadReplay} from './replay/download.js';
import {userPath} from './managed-storage.mjs';

const host=document.getElementById('game'),input=globalThis.__eaglerPC98Input;
const nativeKeys=[['ArrowUp',1,38],['ArrowDown',2,40],['ArrowLeft',4,37],['ArrowRight',8,39],['KeyX',16,88],['KeyZ',32,90],['ShiftLeft',64,16],['Escape',128,27],['Enter',256,13]];
const sources=['../managed-gameover.mjs','../practice-attempt.js','../direct-drag.js','../player-ui.js','../solo.js','../solo-worker.js','../solo-worker-client.js','../dosbox-adapter.js','../game-version.js','../solo-native-music.js',
  '../touch-input.js','../vendor/dosbox-x/runtime.json','../vendor/dosbox-x/dosbox-jspi.mjs','../vendor/dosbox-x/wdosbox-x-fast.wasm',
  '../dosbox-disk.js','../frame-presenter.js','../solo-worker-environment.js','../solo-patch.json','../solo-patch-jp.json','../vendor/dosbox-x/emulators.js',
  './external-host.js','./external-loader.js','./external-codec.js','./external-recorder.js','./external-ui.js','./external-lifecycle.js',
  '../practice-config.js','../practice-engine.js','../practice-assets.js','../practice-assets.json','../practice-core.json','../practice-start.com','../practice-cn.exe.gz','../practice-jp.exe.gz','../vendor/dosbox-x/wdosbox-x-practice.wasm',
  '../solo-music.js','../solo-music.json','../solo-music.com','../bgm/manifest.json','../netplay/local-bgm.js'];
let canvas=document.getElementById('canvas'),attempt,retrySlot,retryBlocked=false,lastRetry=null,emulator,music,recorder,lastKeys=0,loop=0,playback=false,options={},language='jp',musicMode='ogg',recordBlob,lastPractice,currentSettings;
const report=error=>globalThis.__eaglerPC98Emit({event:'error',message:String(error?.message||error)});
const readState=()=>emulator?.snapshot.state??null;
function key(code,number,down){emulator.send('key',[{type:down?'keydown':'keyup',code,key:code==='KeyZ'?'z':code==='KeyX'?'x':code,keyCode:number,which:number}]);}
function release(){input.releaseGameplay();if(emulator)for(const [code,bit,number] of nativeKeys)if(lastKeys&bit)key(code,number,false);lastKeys=0;emulator?.clearInput();}
let autoSaveTask=null;
let replayResult=null,viewerSettings=null,playbackPath=null,playbackDescription=null,playbackPaused=false,playbackEnded=false,playbackRate=1,playbackFrame=0,playbackTotal=0,playbackTarget=null,playbackPoll=null,playbackPolling=false;
function publishPlayback(){if(playbackPath)globalThis.__eaglerPC98Emit({event:'runtime-info',playback:{kind:'replay',path:playbackPath,description:playbackDescription,difficulty:playbackDescription.difficulty,left:-1,right:-1,stage:0,frame:playbackFrame,total:playbackTotal,seeking:playbackTarget!==null,target:playbackTarget,rate:playbackRate,seekable:true,rateControl:true,fileRelocation:1,paused:playbackPaused,ended:playbackEnded}});}
async function pollPlayback(){
  if(!playback||!emulator||playbackPolling)return;playbackPolling=true;const current=emulator;
  try{const status=await current.call('externalReplayStatus');if(emulator!==current||!status)return;playbackFrame=status.boundary;
    if(playbackTarget!==null&&status.seekBoundary===null&&status.held){playbackTarget=null;current.pauseReplay(true);playbackPaused=true;music?.suspend();void attempt?.sfx?.context?.suspend();}
    publishPlayback();
  }catch(error){if(emulator===current)report(error);}finally{playbackPolling=false;}
}
let nativeResultBusy=false,nativeResultHandled=0;
let nativePauseBusy=false,nativePauseHandled=0,nativeTransitionHolding=false;
let pausePrevious=0,pauseState=null;
const pauseMenu=mountPauseMenu(host.querySelector('.solo-screen'),{
  resume(){pauseMenu.close();input.clear();pausePrevious=0;emulator?.run();music?.resume();void attempt?.sfx?.resume();canvas.focus();},
  exit(){transition(false);},
  save(){transition(true);},
  discard(){transition(false);},
  restart(){transition(false,true);},
});
function transition(save,restarting=false){const current=emulator;void returnToTitle(save,restarting,current).catch(error=>{if(emulator===current){pauseMenu.open();report(error);}});}
const savedReplays=new WeakMap();
async function saveReplay(blob){recordBlob=blob;if(savedReplays.has(blob))return savedReplays.get(blob);const task=(async()=>{const name=`replay/th04_${Date.now()}_${crypto.randomUUID().slice(0,8)}.th04r`;await globalThis.__eaglerPC98Files.write(name,await blob.arrayBuffer());return name;})();savedReplays.set(blob,task);return task;}
async function saveRecording({download=true}={}){if(!recorder?.state.ready)return null;const blob=await recorder.save(),path=await saveReplay(blob);if(download)downloadReplay(blob,path.slice(path.lastIndexOf('/')+1));return path;}
async function syncFiles(){if(!emulator||playback)return;const fat=new Fat12(await emulator.getDiskImage('th04-solo.hdi'));
  for(const name of ['GENSOU.SCR','MIKO.CFG']){const entry=fat.find('GENSO/'+name,{optional:true});if(entry!==undefined)await globalThis.__eaglerPC98Files.write(name,fat.read(entry));}}
async function returnToTitle(save,restarting=false,current=emulator,practiceConfig=null){
  if(practiceConfig&&nativeTransitionHolding)return;
  if(practiceConfig)nativeTransitionHolding=true;
  try{
  if(practiceConfig)await current.call('externalReplayHold',[true]);
  pauseMenu.close();
  const recording=recorder;if(recording?.state.failed&&!save){recording.dispose();recorder=undefined;if(attempt)attempt.recorder=null;}else if(recording){recording.finish();const until=performance.now()+10000;while(emulator===current&&!recording.state.ready&&!recording.state.failed&&performance.now()<until)await new Promise(resolve=>setTimeout(resolve,25));if(emulator!==current)return;if(!recording.state.ready)throw Error('录像未能完成');if(save)await saveRecording();if(autoSaveTask)await autoSaveTask;if(emulator!==current)return;await recording.rearm(!!practiceConfig);}
  if(emulator!==current)return;await syncFiles();if(emulator!==current)return;if(!await emulator.call('soloQuit',[pauseState?.generation,restarting,pauseState?.at,practiceConfig]))throw Error('当前对局无法退出');if(emulator!==current)return;
  if(practiceConfig){lastPractice=practiceConfig;practiceUI.setRunning(true,lastPractice);}
  discardRetry();if(!restarting){practiceUI.setRunning(false);lastPractice=normalizePractice(practiceUI.configuration());}
  input.clear();pausePrevious=0;emulator.run();music?.resume();void attempt?.sfx?.resume().catch(()=>{});canvas.focus();
  }finally{if(practiceConfig){nativeTransitionHolding=false;if(emulator===current)await current.call('externalReplayHold',[false]);}}
}
async function naturalExit(){if(playback)return;if(autoSaveTask)await autoSaveTask;await syncFiles();globalThis.__eaglerPC98Emit({event:'exit',code:0});}
const practiceUI=mountPractice({host,onNativeOpen:openNativeForm,onRelease:release,onFocus:()=>canvas.focus(),onChange:value=>{emulator?.practiceOptions(value);},
  onStart:value=>void startPractice(value).catch(report),onRetry:()=>void startPractice({...lastPractice,options:practiceUI.options}).catch(report),onSave:()=>void saveRecording().catch(report),
  onPause:()=>{if(emulator&&!pauseMenu.visible){key('Escape',27,true);setTimeout(()=>emulator&&key('Escape',27,false),120);}}});
const managedPractice=mountManagedPractice(practiceUI,input,{release,nativeOpen:openNativeForm,nativeRetry:()=>{const state=readState();if(state)emulator.setInput({address:state.at,generation:state.generation,formInput:64});},nativeKey:(code,down)=>{const number={Backspace:8,F1:112,F2:113,F3:114,F4:115,F5:116,KeyU:85}[code];if(emulator)key(code,number,down);},readState:()=>({...readState(),practice:emulator?.snapshot.practice})});
async function startPractice(value){const config=normalizePractice(value),state=readState(),ps=emulator?.snapshot.practice;if(!state||playback)return restart(config);if(state.mode!==1||ps?.nativeForm?.visible)return;if(ps?.nativeResult?.visible){await emulator.call('practiceResultRetry',[state.at,state.generation]);return;}pauseState=state;await returnToTitle(false,true,emulator,config);}
async function openNativeForm(){
  const state=readState();if(!emulator||playback)return;
  if(!state||state.mode!==1)return;
  release();await emulator.call('practiceFormOpen',[state.at,state.generation,true]);
}
let formBusy=false;
async function completeNativeForm(ps,state){
  if(formBusy||!ps?.nativeForm?.request||!state)return;formBusy=true;
  try{const current=emulator,values=ps.nativeForm.values;if(!await current.call('practiceFormTake',[state.at,state.generation,ps.nativeForm.request]))return;
    const options=decodeNativeOptions(ps.nativeForm.settings,ps.options);current.practiceOptions(options);lastPractice={...lastPractice,options};if(ps.nativeForm.request===2)return;
    const [stage,character,difficulty,entry,phase,lives,bombs,power,branch,rank]=values;
    const config=normalizePractice({...lastPractice,mode:'target',setup:{...lastPractice.setup,stage,character,difficulty,entry:['stage','mid','boss'][entry],phase,lives,bombs,power,branch:branch?'gengetsu':'auto',rank:rank+4,hp:ps.nativeForm.hp},options});pauseState=state;await returnToTitle(false,true,current,config);
  }catch(error){report(error);}finally{formBusy=false;}
}
async function handleNativeResult(ps,state){
  const request=ps?.nativeResult?.request||0;if(!request){nativeResultHandled=0;return;}
  if(nativeResultBusy||nativeResultHandled===request||!state)return;
  nativeResultBusy=true;nativeResultHandled=request;const current=emulator,at=state.at,generation=state.generation;
  const acknowledge=async ack=>{if(emulator===current)await current.call('practiceResultAck',[at,generation,ack]);};
  try{
    if(request===1){pauseState=state;await returnToTitle(false,true,current,normalizePractice({...lastPractice,options:ps.options}));await acknowledge(1);}
    else if(request===2){await current.call('externalReplayHold',[true]);try{recorder?.finish();const until=performance.now()+10000;while(recorder&&!recorder.state.ready&&!recorder.state.failed&&performance.now()<until)await new Promise(resolve=>setTimeout(resolve,25));if(!await saveRecording())throw Error('录像尚未就绪');await recorder.rearm(true);await acknowledge(1);}finally{if(emulator===current)await current.call('externalReplayHold',[false]);}}
    else await acknowledge(2);
  }catch(error){await acknowledge(2);report(error);}finally{nativeResultBusy=false;}
}
async function handleNativePause(ps,state){
  const request=ps?.nativePause?.request||0;if(!request){nativePauseHandled=0;return;}
  if(nativePauseBusy||nativePauseHandled===request||!state)return;
  nativePauseBusy=true;nativePauseHandled=request;const current=emulator,at=state.at,generation=state.generation;
  const ack=async value=>{if(emulator===current)await current.call('practicePauseAck',[at,generation,value]);};
  try{if(request===3){pauseState=state;await returnToTitle(false,true,current);await ack(1);}else{pauseState=state;await returnToTitle(request===1,false,current);await ack(1);}}catch(error){await ack(2);report(error);}finally{nativePauseBusy=false;}
}
function tick(){
  if(!emulator)return;const frameState=readState(),sample=input.sample({play:!pauseMenu.visible&&frameState?.mode===1&&!frameState.flags}),edge=sample&~pausePrevious;pausePrevious=sample;
  if(pauseMenu.visible){if(edge&1)pauseMenu.move(-1);if(edge&2)pauseMenu.move(1);if(edge&(32|256))pauseMenu.confirm();else if(edge&(128|16))pauseMenu.confirmResume();loop=requestAnimationFrame(tick);return;}
  const state=frameState,ps=emulator.snapshot.practice;practiceUI.update({practice:ps,retryReady:!!retrySlot?.ready});prepareRetry(ps);
  if(!playback&&ps?.mode==='target'&&ps.setup&&(lastPractice?.mode!=='target'||JSON.stringify(lastPractice.setup)!==JSON.stringify(ps.setup))){lastPractice=normalizePractice({...lastPractice,mode:'target',setup:ps.setup,options:ps.options});practiceUI.setRunning(true,lastPractice);}
  if(state&&!playback&&(ps?.nativePause?.visible||((edge&128)&&state.mode===1&&!ps?.nativeResult?.visible&&!ps?.nativeForm?.visible))){
    const pauseInput=(sample&1?1:0)|(sample&2?2:0)|(sample&(32|256)?4:0)|(sample&(128|16)?8:0);
    emulator.setInput({address:state.at,generation:state.generation,pauseInput,openPause:!!((edge&128)&&!ps?.nativePause?.visible)});
  }
  if(!playback&&state&&ps?.nativeForm?.visible){const formInput=(sample&1?1:0)|(sample&2?2:0)|(sample&4?4:0)|(sample&8?8:0)|(sample&(32|256)?16:0)|(sample&(128|16)?32:0);emulator.setInput({address:state.at,generation:state.generation,formInput});}
  if(!playback&&ps?.nativeForm?.request){void completeNativeForm(ps,state);loop=requestAnimationFrame(tick);return;}
  if(!playback&&ps?.nativePause?.visible){void handleNativePause(ps,state);}
  if(!playback&&ps?.nativeResult?.visible&&state){const resultInput=(sample&1?1:0)|(sample&2?2:0)|(sample&32?4:0)|(sample&(128|16)?8:0)|(sample&256?16:0);emulator.setInput({address:state.at,generation:state.generation,resultInput});}
  if(ps?.outcome&&!playback){if(ps.nativeResult){void handleNativeResult(ps,state);}else practiceUI.complete({outcome:ps.outcome,recording:!!recorder,ready:!!recorder?.state.ready,failed:recorder?.state.failed});}
  if(emulator.state==='running'&&!playback){
    const playing=state?.mode===1&&!state.flags&&!ps?.nativeForm?.visible;let bits=ps?.nativeForm?.visible?0:sample;
    if(playing){
      bits&=~128;
      if(ps?.options.disableBomb)bits&=~16;if(ps?.options.disableShot)bits&=~32;if(ps?.options.disableFocus)bits&=~64;else if(ps?.options.forceFocus)bits|=64;
    }
    if((bits&3)===3)bits&=~3;if((bits&12)===12)bits&=~12;
    for(const [code,bit,number] of nativeKeys)if(!!(bits&bit)!==!!(lastKeys&bit))key(code,number,!!(bits&bit));lastKeys=bits;
    const motion=input.motion();if(!ps?.nativeResult?.visible&&!ps?.nativePause?.visible&&!(edge&128))emulator.setInput({address:state?.at,generation:state?.generation,points:(bits&64?1:0)|(options.alwaysHitbox||ps?.options.alwaysPoint?8:0),touch:playing&&motion.active?motion:null});
  }
  if(music){if(playbackTarget!==null||document.hidden||emulator.state!=='running'||state?.mode===2||lastPractice?.mode==='target'&&(!ps?.ready||ps.waiting||ps.outcome))music.suspend();else music.resume();}
  loop=requestAnimationFrame(tick);
}
function discardRetry(){retrySlot?.stop();retrySlot=undefined;}
function makeAttempt(target,practice,settings,replay=null,standby=false,effects=null){return createSoloAttempt({canvas:target,practice,settings,replay,standby,effects,deferReplay:true,sources,callbacks:{onError:report,onResult:result=>{if(lastPractice?.mode==='full'&&['game-over','clear'].includes(result.timeline?.reason)){autoSaveTask=saveRecording({download:false}).catch(report).finally(()=>{autoSaveTask=null;});}},onExit:()=>void naturalExit().catch(report),onPlaybackEnd:matches=>{replayResult=matches;playbackFrame=playbackTotal;playbackTarget=null;playbackEnded=true;playbackPaused=true;emulator?.pauseReplay(true);music?.suspend();void attempt?.sfx?.context?.suspend();publishPlayback();if(!matches)report(Error('Replay 终态内存校验不一致'));}}});}
function prepareRetry(){discardRetry();}
async function launch(config,replay=null,cached=null,effects=null){
  playback=!!replay;replayResult=null;lastPractice=normalizePractice(replay?.header.practice||config);practiceUI.setRunning(!playback,lastPractice);
  const settings={language:replay?.header.language||language,audioMode:replay?.header.audioMode||'independent',runtime:'worker',memoryScope:'pc98-guest-ram/1',exitOnGameQuit:true,recordEnabled:!playback,volume:musicMode==='none'?0:.7};
  currentSettings=settings;attempt=cached||makeAttempt(canvas,lastPractice,settings,replay,false,effects);
  const started=performance.now(),cachedReady=!!cached?.ready;await attempt.task;if(cached)await attempt.core.activatePractice(practiceUI.options);
  if(cached){canvas.replaceWith(attempt.canvas);canvas=attempt.canvas;lastRetry={cached:true,cachedReady,milliseconds:Math.round(performance.now()-started)};}
  emulator=attempt.core;music=attempt.music;recorder=attempt.recorder;attempt.foreground=true;
  canvas.focus();managedPractice.start({readOnly:playback});loop=requestAnimationFrame(tick);
}
async function restart(config,retry=false){const cached=retry?retrySlot:undefined;if(cached)retrySlot=undefined;discardRetry();release();cancelAnimationFrame(loop);const effects=!cached&&!playback?attempt?.takeEffects():null;attempt?.stop();emulator=music=recorder=undefined;recordBlob=undefined;retryBlocked=false;const next=canvas.cloneNode(false);canvas.replaceWith(next);canvas=next;await launch(config,null,cached,effects);}
async function stopPlayback(){clearInterval(playbackPoll);playbackPoll=null;if(autoSaveTask)await autoSaveTask;pauseMenu.close();pausePrevious=0;managedPractice.stop();discardRetry();release();cancelAnimationFrame(loop);const previous=attempt;attempt=undefined;emulator=music=recorder=undefined;await previous?.stop();const next=canvas.cloneNode(false);canvas.replaceWith(next);canvas=next;}
export const managedAPI={
  async start(value){options=value.options||{};language=value.language==='ja'?'jp':'cn';musicMode=value.music;let replay;if(options.replayViewer&&!options.replayPath){viewerSettings={language:value.language,music:value.music,options};playback=true;return;}
    if(options.replayPath){const bytes=globalThis.__eaglerPC98Files.memory.get(userPath(options.replayPath));if(!bytes)throw Error('录像不存在');replay=await decodeExternalReplay(new Blob([bytes]),'04');}
    await launch(practiceUI.configuration(),replay);},
  health(){if(viewerSettings&&!emulator)return {frames:1,audio:{state:'running'}};return {replayResult,gameFrame:readState()?.ticks,frames:emulator?.module.frameUploads??emulator?.module.renderMetrics?.drawn??0,audio:{state:emulator?.context?.state,musicStatus:music?.player.status??null,effectsPeak:attempt?.sfx?.peak??0}};},
  practiceState(){return {...managedPractice.state(),loading:attempt?.timings,recordingReady:!!recorder?.state.ready,retry:{state:retrySlot?(retrySlot.ready?'ready':'preparing'):retryBlocked?'unavailable':'idle',last:lastRetry}};},
  finishRecording(){recorder?.finish();},
  saveRecording,
  async files(message){const store=globalThis.__eaglerPC98Files;if(message.command==='sync'){await syncFiles();return {};}
    if(message.command==='list'){await syncFiles();return {files:[...store.memory].map(([path,bytes])=>({path,size:bytes.length}))};}
    const path=userPath(message.path);if(message.command==='read'){await syncFiles();const bytes=store.memory.get(path);if(!bytes)throw Error('文件不存在');return {bytes:Array.from(bytes)};}
    if(emulator&&!(playback&&path.startsWith('replay/')))throw Error('请退出游戏后管理文件');if(message.command==='remove'){await store.remove(path);return {};}
    if(message.command==='write'){const bytes=new Uint8Array(message.bytes);if(path.endsWith('.th04r'))await decodeExternalReplay(new Blob([bytes]),'04');await store.write(path,bytes);return {};}
    throw Error('用户文件操作无效');},
  async playback(message){
    if(message.action==='inspect'||message.action==='open'){
      const path=userPath(message.path),bytes=globalThis.__eaglerPC98Files.memory.get(path);if(!bytes)throw Error('录像不存在');
      const replay=await decodeExternalReplay(new Blob([bytes]),'04');
      const setup=replay.header.practice?.mode==='target'?replay.header.practice.setup:null;
      const description={name:'',character:setup?.character??-1,difficulty:setup?.difficulty??-1,stages:[{id:0,label:'Replay',score:null}]};
      if(message.action==='inspect')return {description};
      if(message.stage!==undefined&&message.stage!==0)throw Error('录像关卡无效');
      if(emulator&&!playback)throw Error('请先退出当前游戏');
      if(emulator)await stopPlayback();
      playbackPath=path;playbackDescription=description;playbackPaused=playbackEnded=false;playbackFrame=0;playbackTarget=null;playbackRate=1;playbackTotal=replay.header.endBoundary;
      await launch(practiceUI.configuration(),replay);playbackPoll=setInterval(()=>void pollPlayback(),250);publishPlayback();return {};
    }
    if(message.action==='relocate'&&playbackPath&&userPath(message.path)===playbackPath){const target=userPath(message.target);if(!target.startsWith('replay/')||!globalThis.__eaglerPC98Files.memory.has(target))throw Error('录像不存在');playbackPath=target;publishPlayback();return {};}
    if(playback&&emulator&&message.action==='rate'){if(![0,.5,1,2,4,8].includes(message.value))throw Error('录像速度无效');await emulator.call('externalReplayRate',[message.value]);playbackRate=message.value;publishPlayback();return {};}
    if(playback&&emulator&&message.action==='play'&&playbackEnded){const path=playbackPath;return managedAPI.playback({action:'open',path});}
    if(playback&&emulator&&message.action==='seek'){
      if(!Number.isSafeInteger(message.value)||message.value<0||message.value>playbackTotal)throw Error('录像位置无效');
      const target=Math.min(message.value,Math.max(0,playbackTotal-1)),path=playbackPath,rate=playbackRate;
      await pollPlayback();if(target<=playbackFrame||playbackEnded){await managedAPI.playback({action:'open',path});if(rate!==1)await managedAPI.playback({action:'rate',value:rate});}
      if(target===0){emulator.pauseReplay(true);playbackPaused=true;publishPlayback();return {};}
      playbackTarget=target;playbackPaused=false;emulator.pauseReplay(false);music?.suspend();void attempt?.sfx?.context?.suspend();await emulator.call('externalReplaySeek',[target]);publishPlayback();return {};
    }
    if(playback&&emulator&&['pause','play'].includes(message.action)){
      playbackPaused=message.action==='pause';emulator.pauseReplay(playbackPaused);
      if(playbackPaused){music?.suspend();await attempt?.sfx?.context?.suspend();}else{music?.resume();await attempt?.sfx?.resume();}
      publishPlayback();return {};
    }
    throw Error('录像操作无效');
  },
  async stop(){await stopPlayback();viewerSettings=playbackPath=playbackDescription=null;playback=false;playbackPaused=playbackEnded=false;},
};
