import createCore from './runtime/core.mjs';
import {Renderer} from '../src/renderer.mjs';
import {TitleScenes} from '../src/title-scenes.mjs';
import {TitlePresentation} from '../src/title-presentation.mjs';
import {validateMenuResources,selectEnding} from '../src/menu-resources.mjs';
import {EndingTimeline} from '../src/ending-timeline.mjs';
import {GameProfile} from '../src/game-profile.mjs';
import {openStore} from '../src/storage.mjs';
import {StageLoader,stageMusic,bossMusic} from '../src/stage-loader.mjs';
import {MusicPlayer} from '../src/music.mjs';
import {StreamingMusicPlayer} from '../src/audio/streaming-music.mjs';
import {guard as practiceMusicGuard,reset as resetPracticeMusic} from '../src/audio/practice-music.mjs';
import {SoundPlayer,nativeMusicGain} from '../src/sound-player.mjs';
import {readCoreSoundEvents} from '../src/sound-events.mjs';
import {parseReplay,replayInputEnded} from '../src/replay.mjs';
import {startReplayStage} from '../src/replay-session.mjs';
import {ReplayWorkspace} from '../src/replay-workspace.mjs';
import {ReplayRecorder,defaultReplayConfig} from '../src/replay-recorder.mjs';
import {createFrameRateMeter} from '../src/fps-presentation.mjs';
import {decodeSpellTitle} from '../src/spell-presentation.mjs';
import {LoadingPresentation} from '../src/loading-presentation.mjs';
import {RuntimeAssets} from '../src/managed-data.mjs';
import {installManagedShell} from '../src/managed-shell.mjs';
import {ManagedInput,keyboardCode} from '../src/managed-input.mjs';
import {TouchInput} from '../src/touch-input.mjs';
import {encodeNativeConfig} from '../src/native-profile.mjs';
import {installRuntimeFonts} from '../src/runtime-fonts.mjs';
import {prepareLocalization,isLocalizationResourcePath} from '../src/localization/loader.mjs';
import {PracticeBridge,createPracticeMetadata} from '../src/practice/bridge.mjs';
import {PracticeOverlay} from '../src/practice/overlay.mjs';
import {DamageSourceBridge} from '../src/practice/damage-sources.mjs';

const $=id=>document.getElementById(id),canvas=$('screen'),keys=new Set(),hostKeys=new Set(),meter=createFrameRateMeter(),TICK=1000/60;
const assets=new RuntimeAssets(),keyDown=code=>keys.has(code)||hostKeys.has(code);
const managedInput=new ManagedInput(),touch=new TouchInput();
let previousPresentation=null,currentPresentation=null;
let replayWorkspace=null,viewerMusic={id:null,ticks:0};
let practiceBridge=null,practiceUI=null,damageSources=null,practiceCaptured=false,logicalStep=TICK,captureThisTick=true,localization=null;
let emit=()=>{},launched=false,terminated=false,runtimeMusic='ogg',runtimeOptions={limitPresentationTo60:false},firstFrame=false,lastPresentation=0,renderedFrames=0,healthAt=0,maxFrameGap=0,backgrounded=document.hidden;
let core,renderer,loader,profile,store,resources,model,presentation,loadingScreen,corpus=[],musicIndex=[],soundIndex=[],session=null,ending=null,ready=false,busy=false,fatalError=false,last=0,accumulator=0,previousUI=0,previousPadUI=0,pendingKeys=0,pendingPressed=0,choiceSerial=-1,lastReplay=null,pendingResult=null,joyCaptureRelease=false;
const audio={context:null,music:null,track:null,pendingTrack:null,musicPaused:false,browserPaused:false,request:0,effects:null,effectsReady:null,musicSerial:-1};
const decoder=new TextDecoder();
const readJSON=name=>{const p=core['_th12_'+name](),size=core['_th12_'+name+'_size']();return JSON.parse(decoder.decode(core.HEAPU8.subarray(p,p+size)));};
const hud=()=>[...new Float32Array(core.HEAPU8.buffer,core._th12_hud(),core._th12_hud_size())];
async function bytes(url){const r=await assets.fetch(url);if(!r.ok)throw Error('资源加载失败 '+url);return new Uint8Array(await r.arrayBuffer());}
function notice(text=''){$('notice').textContent=text;}
function fail(error){if(fatalError)return;fatalError=true;busy=true;audio.musicPaused=true;audio.pendingTrack=null;++audio.request;audio.music?.stop();audio.effects?.stop();clearKeys();notice('运行中断：'+error.message);console.error(error);emit({event:'error',message:error.message});}
function clearKeys({touchToo=true,practiceToo=true}={}){keys.clear();hostKeys.clear();if(assets.managed)managedInput.clear();if(touchToo)touch.cancel();if(practiceToo)practiceUI?.cancel();model?.clearKeys();previousUI=previousPadUI=pendingKeys=pendingPressed=0;if(session)session.previous=0;accumulator=0;last=performance.now();meter.reset(last);resetGamePresentation();}
function resetGamePresentation(){previousPresentation=currentPresentation=null;renderer?.resetPresentation();}
function gamePresentationOptions(){return {currentFps:meter.value,hudAge:session?.hudAge??0,scoreLowDigit:session?.continues??0,alwaysHitbox:!!runtimeOptions.alwaysHitbox,disableMaster:!!(core._th12_practice_ui_behavior?.()&1),stageClear:readJSON('stage_flow').status==='stage-clear',practice:session?.mode==='practice'||session?.replay?.practice,recordedFps:session?.replay?.recordedFps(session.stage,session.cursor)??null};}
function captureGamePresentation(){if(!session||!ready)return;previousPresentation=currentPresentation;currentPresentation=renderer.capturePresentation(core,gamePresentationOptions());}
function drawGamePresentation(interpolate=false){if(!currentPresentation)captureGamePresentation();if(currentPresentation)renderer.renderPresentation(previousPresentation,currentPresentation,interpolate&&!runtimeOptions.limitPresentationTo60?Math.min(1,accumulator/logicalStep):1);}
function updatePracticeUI(){
  if(!practiceUI||!model)return;
  practiceUI.update({scene:model.state.scene,replay:!!session?.replay,menu:model.state.scene==='practice'?model.practiceMenu:null,menuReady:model.ready()});
  if(practiceCaptured!==practiceUI.capturesGameplay){practiceCaptured=practiceUI.capturesGameplay;clearKeys({practiceToo:false});}
}
function practicePointer(message){if(runtimeOptions.replayViewer)return false;const consumed=practiceUI?.pointer(message)??false;if(consumed)updatePracticeUI();return consumed;}
function checkDamageEntry(){const state=damageSources.state();if(state.error&&state.entryResult===-1)throw Error(state.error);}
function initializeReplayStage(replay,part){
  practiceBridge.setReplay(true);
  const practice=replay.extension?.practice;
  if(practice?.params.mode){
    if(!runtimeOptions.thpracEnabled)throw Error('这份录像使用了自定义练习，请在启动器启用 THPrac');
    practiceBridge.configure(practice.params,{replay:true,difficulty:replay.difficulty});
    if(core._th12_practice_prepare_replay(replay.difficulty)!==1)throw Error(practiceBridge.snapshot().error||'练习录像脚本准备失败');
  }
  startReplayStage(core,replay,part);core._th12_set_full_game_mode(replay.practice?1:0);checkDamageEntry();
}
function touchState(){
  const scene=model?.state.scene,playing=ready&&!busy&&session&&scene==='playing';
  const h=playing?hud():null,message=playing?readJSON('message'):null;
  const kind=playing?(session.replay?'replay':message?.active&&(message.flags&1||message.wait>0)?'dialogue':'game'):'menu';
  const instance=playing?`${session.stage}:${core._th12_player_instance()}`:'';
  return {key:playing?`${kind}:${instance}`:`menu:${model?.state.serial??0}`,kind,instance,ready:!!playing&&core._th12_player_state()===1,
    x:h?Math.round(h[1]*128):0,y:h?Math.round(h[2]*128):0,
    fast:playing?core._th12_player_speed(0):0,slow:playing?core._th12_player_speed(1):0,keyFocused:ready&&keyDown(profile.settings.keyMap.focus)};
}
function pointerPosition(x,y){const rect=canvas.getBoundingClientRect();return {x:(x-rect.left)*640/rect.width,y:(y-rect.top)*480/rect.height};}
function hostPointer(message){if(!ready||busy||runtimeOptions.replayViewer)return;const x=message.x*innerWidth,y=message.y*innerHeight,id='host:'+message.id;if(practicePointer({...message,x,y,id}))return;const point=pointerPosition(x,y);touch.pointer({...message,...point,id},touchState());}

function input(){
  const map=profile.settings.keyMap,held=(name,defaults=[])=>keyDown(map[name])||defaults.some(keyDown);
  let native=0;
  if(held('up'))native|=16;if(held('down'))native|=32;if(held('left'))native|=64;if(held('right'))native|=128;
  if(held('shoot'))native|=1;if(held('bomb'))native|=2;if(held('focus',['ShiftRight']))native|=8;if(held('skip',['ControlRight']))native|=512;
  native|=pendingKeys&0x2ff;
  let ui=native|pendingKeys;if(keyDown('Escape')||held('pause'))ui|=256;if(keyDown('Enter'))ui|=0x80000;if(keyDown('KeyR'))ui|=0x200000;
  const pressed=(ui&~previousUI)|pendingPressed;previousUI=ui;
  pendingKeys=pendingPressed=0;
  return {held:native,ui,pressed};
}
function padInput(){
  const p=Array.from(navigator.getGamepads?.()??[]).find(p=>p?.connected);
  const buttons=Array.from(p?.buttons??[]).flatMap((button,index)=>index<32&&button.pressed?[index]:[]);
  //Wait for the confirmation button to be released before capturing the
  //next button. Its remaining held frames must not confirm the menu again.
  if(joyCaptureRelease){if(!buttons.length)joyCaptureRelease=false;return {value:0,capturing:true};}
  if(model.state.captureJoy){
    if(!model.state.captureReady){if(!buttons.length)model.state.captureReady=true;}
    else if(buttons.length&&model.captureGamepad(buttons[0])){joyCaptureRelease=true;clearKeys();}
    return {value:0,capturing:true};
  }
  const mapped=assets.managed?managedInput.pad():null;if(mapped!==null)return {value:mapped,capturing:false};
  if(!p)return {value:0,capturing:false};
  let value=0;if(p.axes[0]<-.3||p.buttons[14]?.pressed)value|=64;if(p.axes[0]>.3||p.buttons[15]?.pressed)value|=128;
  if(p.axes[1]<-.3||p.buttons[12]?.pressed)value|=16;if(p.axes[1]>.3||p.buttons[13]?.pressed)value|=32;
  for(const [name,bit]of Object.entries({shoot:1,bomb:2,focus:8,pause:256,skip:512}))if(p.buttons[profile.settings.joyButtons[name]]?.pressed)value|=bit;
  return {value,capturing:false};
}
async function audioReady(){
  const Audio=window.AudioContext??window.webkitAudioContext;if(!Audio)return;
  audio.context??=(assets.managed&&window.parent.__touhouAudioContext)||new Audio();
  const Player=runtimeOptions.oggDecodeMode==='stream'?StreamingMusicPlayer:MusicPlayer;
  audio.music??=new Player({context:audio.context,readBytes:file=>bytes('/assets/music/'+file),onError:fail});
  audio.effects??=new SoundPlayer({context:audio.context,index:soundIndex,readBytes:file=>bytes('/assets/'+file),volume:profile.settings.seVolume});
  audio.music.setVolume(runtimeMusic==='none'?0:nativeMusicGain(profile.settings.bgmVolume));
  audio.effects.setVolume(profile.settings.seVolume);
  // One shared load prevents repeated key presses from decoding the whole bank
  // again. Events arriving during decode retain their channel until it is ready.
  audio.effectsReady??=audio.effects.preload().catch(error=>{audio.effectsReady=null;throw error;});
  await Promise.all([audio.context.resume(),audio.effectsReady]);
  if(!audio.browserPaused&&!audio.musicPaused&&!audio.pendingTrack&&(!runtimeOptions.replayViewer||replayWorkspace?.audible)){
    if(audio.music.paused)await resumeMusicTransport();
    else if(audio.track&&!audio.music.source)await playMusic(audio.track,{authorized:true});
  }
}
function suppressMusic(command,{retain=false,track=null}={}){return practiceMusicGuard(command,{enabled:!!core?._th12_practice_everlasting_bgm?.(),retain,track});}
async function playMusic(id,{offsetSeconds=0,authorized=false,viewerTransport=false}={}){
  if(runtimeOptions.replayViewer&&!viewerTransport){
    viewerMusic={id,ticks:0};audio.track=id;
    if(!replayWorkspace?.audible){pauseAudio();return;}
  }
  if(!authorized&&suppressMusic('play',{track:id}))return;
  audio.track=id;audio.pendingTrack=null;audio.musicPaused=false;const request=++audio.request;
  if(session&&!session.replay)profile.unlockMusic(id);
  if(runtimeMusic==='none'||audio.browserPaused||backgrounded||!audio.context||!audio.music)return;
  const track=musicIndex.find(t=>t.id===id);if(!track)throw Error('音乐不存在 '+id);
  audio.music.setVolume(runtimeMusic==='none'?0:nativeMusicGain(profile.settings.bgmVolume));
  audio.pendingTrack=id;
  try{await audio.music.play(track,()=>request===audio.request,{offsetSeconds});}
  finally{if(request===audio.request)audio.pendingTrack=null;}
}
function effect(event){if(!runtimeOptions.replayViewer||replayWorkspace?.audible)audio.effects?.play(event);}
function stopEffects(){audio.effects?.stop();}
function stopMusic({retain=false}={}){if(suppressMusic('stop',{retain}))return;audio.musicPaused=true;audio.pendingTrack=null;++audio.request;audio.music?.stop();}
function fadeMusic(seconds){if(!suppressMusic('default'))audio.music?.fade(seconds);}
function pauseAudio(){audio.effects?.pause();if(suppressMusic('pause'))return;audio.musicPaused=true;audio.pendingTrack=null;++audio.request;audio.music?.pause();}
async function resumeAudio(){
  if(!audio.browserPaused)audio.effects?.resume();if(suppressMusic('resume'))return;audio.musicPaused=false;
  if(audio.browserPaused||!audio.context||!audio.music)return;
  await audio.context.resume();
  await resumeMusicTransport();
}
async function resumeMusicTransport(){
  if(!audio.context||!audio.music)return;
  const request=++audio.request;audio.pendingTrack=audio.track;
  try{if(audio.music.paused)await audio.music.resume(()=>request===audio.request);else if(!audio.music.source&&audio.track)await playMusic(audio.track,{authorized:true});}
  finally{if(request===audio.request)audio.pendingTrack=null;}
}
function suspendBrowserAudio(){
  if(audio.browserPaused)return;audio.browserPaused=true;audio.pendingTrack=null;++audio.request;
  audio.music?.pause();audio.effects?.pause();
}
async function restoreBrowserAudio(){
  if(!audio.browserPaused)return;audio.browserPaused=false;
  if(terminated||fatalError||!launched)return;
  // Keep the native pause intent (and F7 state) separate from browser transport.
  // AudioContext.resume remains in the user-gesture audioReady/resume path.
  if(!audio.musicPaused&&(!runtimeOptions.replayViewer||replayWorkspace?.audible))await resumeMusicTransport();
  if(ready&&(!runtimeOptions.replayViewer||replayWorkspace?.audible)&&!['pause','pause-confirm','continue','result-menu'].includes(model.state.scene))audio.effects?.resume();
}
function viewerTransport(){
  if(!replayWorkspace?.audible||backgrounded){pauseAudio();return;}
  audio.effects?.resume();
  if(viewerMusic.id)playMusic(viewerMusic.id,{offsetSeconds:viewerMusic.ticks/60,authorized:true,viewerTransport:true}).catch(fail);
}
function loadSpellHistory(){const words=profile.spellHistoryWords(),p=core._malloc(words.length*4);try{new Int32Array(core.HEAPU8.buffer,p,words.length).set(words);if(core._th12_load_spell_history(p,words.length)!==1)throw Error('符卡记录载入失败');}finally{core._free(p);}}
function saveProfile(){profile.updateSpellHistory(readJSON('spell_history').records);return profile.save().catch(error=>notice('记录保存失败：'+error.message));}
async function prepareStage(stage,{demo=false}={}){await renderer.prepareStage(stage);if(!await loader.load(stage))throw Error('关卡准备被取消');if(audio.context){const tracks=demo||runtimeMusic==='none'?[]:musicIndex.filter(t=>[stageMusic[stage],bossMusic[stage]].includes(t.id));await Promise.all([audio.music.preload(tracks),audio.effectsReady]);}}
function recordedHeader(flow){const h=flow.recordHeader;if(!h)throw Error('缺少关卡开始记录');return {number:flow.stage,seed:h.seed,initial:{x:h.x,y:h.y,power:h.power,lives:h.lives,bombs:h.bombs,score:h.scoreStored,pointValue:h.pointValueStored,lifeFragments:h.lifeFragments,bombFragments:h.bombFragments,ufoColors:h.ufoColors,rank:h.rank,nativeGlobals:h.nativeGlobals}};}
function recordStage(){const flow=readJSON('stage_flow');session.recorder?.recordStage(recordedHeader(flow));session.resultKey=null;session.spellCount=0;}
function recordingConfig(){
  return encodeNativeConfig(profile.settings,profile.nativeConfig);
}

async function begin(settings,replay=null,id=null){
  //A fresh story run (including every Retry path) starts at stage1.
  //Only Practice retains its selected stage; Extra always starts stage7.
  settings={...settings,stage:replay?settings.stage??1:settings.mode==='practice'?settings.stage??1:settings.mode==='extra'?7:1};
  busy=true;loadingScreen.startResources();
  if(session&&!session.replay)await saveProfile();
  clearKeys();stopEffects();ending=null;pendingResult=null;
  try{
    const stage=settings.stage??1;
    localization?.setContext({stage,character:settings.character,shot:settings.shot});
    await prepareStage(stage,{demo:settings.mode==='demo'});loadSpellHistory();
    if(replay?.extension?.damage&&!runtimeOptions.thpracEnabled)throw Error('这份录像带有 THPrac 跨关状态，请在启动器启用 THPrac');
    practiceBridge.reset();practiceBridge.setReplay(!!replay);
    damageSources.begin({replay:!!replay,practice:settings.mode==='practice'||!!replay?.practice,character:settings.character,shot:settings.shot,metadata:replay?.extension?.damage});
    session={...settings,stage,replay,id,cursor:0,previous:0,clock:0,playTicks:0,hudAge:0,resultKey:null,spellCount:0,continues:0,transitioning:false,clearRecorded:false,resultRecorded:false,unlimitedMovement:!!replay?.extension?.unlimited,assisted:!!replay?.extension?.practice?.assisted};
    if(replay){const part=replay.stages.find(s=>s.number===stage);if(!part)throw Error('录像不包含关卡');initializeReplayStage(replay,part);}
    else{
      const seed=crypto.getRandomValues(new Uint16Array(1))[0];
      if(settings.practice&&runtimeOptions.thpracEnabled){practiceBridge.configure(settings.practice,{difficulty:settings.difficulty});practiceBridge.start({...settings,seed});session.practiceMetadata=createPracticeMetadata(settings.practice,{assisted:!!settings.practice.mode});}
      else if(core._th12_start_full_game(settings.character,settings.shot,settings.difficulty,seed,settings.mode==='practice'?1:0)!==1)throw Error('完整游戏初始化失败');
      checkDamageEntry();
      session.recorder=new ReplayRecorder({character:settings.character,shot:settings.shot,difficulty:settings.difficulty,practice:settings.mode==='practice',configBytes:recordingConfig()});recordStage();
      profile.recordPlayStart(settings.character,settings.shot,settings.difficulty);
      profile.recordStageEntry(settings.character,settings.shot,settings.difficulty,stage);
    }
    model.run={...settings,...(replay?{replayId:id}:{})};audio.musicSerial=core._th12_music_serial();
    // Native4221bd and422928 gate the road/Boss slots on Demo flag20.
    // The title stream continues at its current playhead throughout the Demo.
    if(settings.mode!=='demo'){
      const track=core._th12_practice_boss_music()?bossMusic[stage]:musicIndex[core._th12_music_track()]?.id??stageMusic[stage];if(!replay)profile.unlockMusic(track);await playMusic(track);
    }
    canvas.focus({preventScroll:true});
  }finally{if(!fatalError){loadingScreen.completeResources();busy=false;}last=performance.now();meter.reset(last);accumulator=0;}
}
async function playReplay(id,stage,mode='replay'){
  loadingScreen.startResources();
  const b=mode==='demo'?await bytes('/assets/'+encodeURIComponent(id)):(await store.read('/savesth12/replay/'+id)??await bytes('/replays/'+encodeURIComponent(id)));
  const replay=parseReplay(b),part=replay.stages.find(s=>s.number===stage)??replay.stages[0];
  await begin({mode,character:replay.character,shot:replay.shot,difficulty:replay.difficulty,stage:part.number},replay,id);
}
async function viewerStage(stage){
  busy=true;loadingScreen.startResources();stopEffects();pauseAudio();
  try{
    if(core._th12_practice_damage_transition(session.stage,stage)!==1)throw Error(damageSources.state().error||'录像跨关状态保存失败');
    localization?.setContext({stage,character:session.character,shot:session.shot});
    await prepareStage(stage);resetGamePresentation();session.stage=stage;session.cursor=0;session.previous=0;session.clock=0;
    initializeReplayStage(session.replay,session.replay.stages.find(part=>part.number===stage));
    audio.musicSerial=core._th12_music_serial();await playMusic(musicIndex[core._th12_music_track()]?.id??stageMusic[stage]);
  }finally{if(!fatalError){loadingScreen.completeResources();busy=false;}last=performance.now();accumulator=0;}
}
async function nextStage(){
  if(session?.mode==='demo'){model.returnTitle();return;}
  if(!session||session.transitioning)return;session.transitioning=true;busy=true;
  try{
    const flow=readJSON('stage_flow');session.recorder?.finishStage();
    if(!session.replay&&session.mode!=='practice')profile.recordStageClear(session.character,session.shot,session.difficulty,session.stage,core._th12_score_stored()*10);
    const stage=session.replay?session.replay.stages.find(s=>s.number>session.stage)?.number:flow.nextStage;
    if(!stage){completeRun(true);return;}
    if(session.replay&&core._th12_practice_damage_transition(session.stage,stage)!==1)throw Error(damageSources.state().error||'录像跨关状态保存失败');
    const heldPresentation=currentPresentation??renderer.capturePresentation(core,gamePresentationOptions());
    loadingScreen.startResources({renderBase:()=>{
      renderer.renderPresentation(null,heldPresentation,1);
      renderer.renderMenu([],[],null,{overlay:true,shade:1,shadeBounds:[32,16,416,464]});
      practiceUI?.render();
    }});
    localization?.setContext({stage,character:session.character,shot:session.shot});
    await prepareStage(stage);resetGamePresentation();touch.cancel();session.stage=stage;session.cursor=0;session.previous=0;session.resultKey=null;session.spellCount=0;
    if(session.replay){initializeReplayStage(session.replay,session.replay.stages.find(s=>s.number===stage));}
    else{if(core._th12_start_next_stage()!==1)throw Error('跨关资源继承失败');recordStage();profile.recordStageEntry(session.character,session.shot,session.difficulty,stage);await saveProfile();}
    audio.musicSerial=core._th12_music_serial();const track=musicIndex[core._th12_music_track()]?.id??stageMusic[stage];if(!session.replay)profile.unlockMusic(track);await playMusic(track);
  }finally{if(session)session.transitioning=false;if(!fatalError){loadingScreen.completeResources();busy=false;}last=performance.now();meter.reset(last);accumulator=0;}
}
function resultData(clear){const flow=readJSON('stage_flow'),scoreStored=core._th12_score_stored(),timestamp=Math.floor(Date.now()/1000);session.assisted||=!!practiceBridge?.snapshot().assisted;const assisted=session.unlimitedMovement||session.assisted;return {mode:session.mode,practice:session.mode==='practice',character:session.character,shot:session.shot,difficulty:session.difficulty,stage:session.stage,scoreStored,score:scoreStored*10+flow.continueCount,timestamp,date:new Date(timestamp*1000).toISOString(),playSeconds:session.playTicks/60,clear,continueCount:flow.continueCount,continues:flow.continueCount,canContinue:flow.canContinue,canSaveReplay:!session.replay&&flow.continueCount===0,ranked:flow.continueCount===0&&!assisted,slowdown:assisted?100:0};}
function recordClear(result){
  if(!session||session.replay||session.mode==='practice'||session.clearRecorded||!result.clear)return;
  session.clearRecorded=true;profile.recordStageClear(session.character,session.shot,session.difficulty,session.stage,result.scoreStored*10);profile.recordClear(session.character,session.shot,session.difficulty,{continues:result.continueCount,stage:session.stage});
  //Unlock flags are committed at the clear, independently of name entry.
  //A continued story clear still counts, but does not unlock Extra.
  if(session.stage===7||session.stage===6&&result.continueCount===0)profile.data.clears[`${session.character*2+session.shot}:${session.difficulty}`]=true;
}
async function recordRunOnce(result,name=profile.data.lastName){
  if(!session||session.replay||session.resultRecorded)return;
  profile.recordRun({...result,name});session.resultRecorded=true;await profile.save();
}
function completeRun(clear){
  if(!session)return;
  pendingResult=resultData(clear);recordClear(pendingResult);session.recorder?.finishStage({end:true});stopEffects();saveProfile();
  model.finish({kind:clear?'clear':'gameover',...pendingResult});clearKeys();
}
function beginEnding(){
  pendingResult=resultData(true);recordClear(pendingResult);session.recorder?.finishStage({end:true});stopEffects();saveProfile();
  const id=selectEnding(session.character,session.shot,pendingResult.continueCount>0);
  model.enter('ending',0,{readyAt:0});ending=new EndingTimeline({resources,atlases:renderer.atlases,ending:resources.endings[id],difficulty:session.difficulty,seen:profile.data.endingsSeen.includes(id),onMusic:id=>playMusic(id).catch(fail),onFadeMusic:()=>fadeMusic(3),onSound:id=>effect({id})});clearKeys();
}
function returnTitle(){if(session&&session.mode!=='demo')stopMusic();loadingScreen.startResources();session=null;ending=null;stopEffects();clearKeys();practiceBridge?.reset();damageSources?.reset();saveProfile();}

function gameTick(input){
  if(!session)return;
  let held=input.held,pressed=(held&~session.previous)|(input.pressed??0),motion=input.motion;
  if(session.replay){
    const recorded=session.replay.input(session.stage,session.cursor);
    if(replayInputEnded(recorded)){if(session.mode==='demo')model.returnTitle();else nextStage().catch(fail);return;}
    ({held,pressed}=recorded);const delta=session.replay.motion?.(session.stage,session.cursor);motion=delta?{mode:4,x:delta[0],y:delta[1]}:null;session.cursor++;
  }
  core._th12_motion(motion?.mode??0,motion?.x??0,motion?.y??0);
  core._th12_tick(held,pressed);session.clock++;session.hudAge++;
  if(!session.replay){const resolved=new Int32Array(core.HEAPU8.buffer,core._th12_practice_input(),2);held=resolved[0];pressed=resolved[1];session.recorder?.pushInput({held,pressed,released:session.previous&~held},meter.value||60);}
  session.previous=held;
  if(runtimeOptions.replayViewer)viewerMusic.ticks++;
  if(!session.replay){
    const resolved=new Int32Array(core.HEAPU8.buffer,core._th12_motion_result(),4);
    session.unlimitedMovement||=!!resolved[3];session.recorder?.recordMotion(resolved[0],resolved[1],session.unlimitedMovement);
    core._th12_set_wall_time(performance.now());
  }
  const h=hud(),flow=readJSON('stage_flow');session.continues=flow.continueCount;
  readCoreSoundEvents(core)?.forEach(effect);
  const serial=core._th12_music_serial();if(serial!==audio.musicSerial){audio.musicSerial=serial;if(session.mode!=='demo'){const track=musicIndex[core._th12_music_track()]?.id;if(track){if(!session.replay)profile.unlockMusic(track);playMusic(track).catch(fail);}}}
  if(!session.replay){
    session.playTicks++;profile.data.playSeconds+=1/60;profile.recordPlayFrame(session.character,session.shot,session.difficulty);
    const spell=readJSON('spell'),r=spell.lastResult,key=`${r.cardId}:${r.kind}:${r.elapsedTicks}:${r.bonusDisplayed}`;
    if((spell.flags&1)&&spell.titleHex&&spell.titleHex!==session.lastCardTitleHex){
      session.lastCardTitleHex=spell.titleHex;const id=`${spell.cardId}:${session.character*2+session.shot}`;
      profile.data.spellRecords[id]={...profile.data.spellRecords[id],title:decodeSpellTitle(spell.titleHex)};
    }
    if(r.kind&&key!==session.resultKey){
      session.resultKey=key;if(session.spellCount<20)session.recorder?.setSpellTime(session.spellCount++,r.realTimeEncoded);
      const recordKey=`${r.cardId}:${session.character*2+session.shot}`,record=profile.data.spellRecords[recordKey]??{},title=decodeSpellTitle(r.titleHex??'');
      // Display metadata only. The C++ history owns attempts/captures; using
      // recordSpell here would count the same retail start/result twice.
      if(title)record.title=title;
      if(r.kind===1){const time=r.elapsedTicks/60;if(time>0)record.bestTime=record.bestTime>0?Math.min(record.bestTime,time):time;record.bestBonus=Math.max(record.bestBonus??0,r.bonusDisplayed);}
      profile.data.spellRecords[recordKey]=record;
    }
  }
  // The workspace's recorded route owns EOF and transitions. It must not
  // enter live name/result/ending menus or add unrecorded gameplay ticks.
  if(runtimeOptions.replayViewer)return;
  if(flow.status==='next-stage'){
    // Retail stage headers include the clear transition's final recorded
    // inputs. Keep consuming that stream until its actual stage boundary.
    const part=session.replay?.stages.find(s=>s.number===session.stage);
    if(!part||session.cursor>=part.frames||replayInputEnded(session.replay.input(session.stage,session.cursor)))nextStage().catch(fail);
  }else if(flow.status==='ending')beginEnding();
  else if(flow.status==='result'){
    const part=session.replay?.stages.find(s=>s.number===session.stage);
    if(!part||session.cursor>=part.frames||replayInputEnded(session.replay.input(session.stage,session.cursor)))completeRun(true);
  }
  else if(flow.status==='game-over'){
    session.recorder?.finishStage({end:true});
    if(session.replay)model.finish({mode:session.mode,kind:'replay'});
    else if(flow.canContinue)model.openContinue(resultData(false));
    else completeRun(false);
    clearKeys();
  }
}
function replayMetadata(replay,extra={}){
  const {data,input,recordedFps,motion,extension,...header}=replay;
  const timestamp=header.timestamp,date=Number.isSafeInteger(timestamp)&&timestamp>0?new Date(timestamp*1000):null;
  return {...extra,...header,webMotion:!!extension,unlimitedMovement:!!extension?.unlimited,score:header.scoreStored*10+(header.continues??0),clear:header.finalStage===8,
    ...(date&&Number.isFinite(date.getTime())?{date:date.toISOString()}:{} )};
}
async function saveReplay(action){
  if(!session?.recorder||!action.result?.canSaveReplay)return;
  const state=practiceBridge.snapshot(),practice=session.practiceMetadata?{...session.practiceMetadata,assisted:session.practiceMetadata.assisted||state.assisted}:state.assisted?createPracticeMetadata({...state.params,mode:0,stage:session.stage-1},{assisted:true}):null;
  session.recorder.setWebExtensions({practice,damage:damageSources.metadata()});
  const id='th12_'+String(action.slot).padStart(2,'0')+'.rpy',b=session.recorder.result({name:action.name,timestamp:action.result.timestamp,scoreStored:action.result.scoreStored??Math.trunc(action.result.score/10),continues:action.result.continues,cleared:action.result.clear,finalStage:action.result.clear?8:action.result.stage,slowdown:action.result.slowdown??0});
  await store.write('/savesth12/replay/'+id,b);await store.sync();lastReplay={id,bytes:b};$('export-replay').hidden=false;
  const r=parseReplay(b);corpus=corpus.filter(f=>f.id!==id);corpus.push(replayMetadata(r,{id}));model.setCorpus(corpus);notice('录像已保存在此浏览器，可导出 .rpy');
}
async function actions(){
  for(const action of model.drainActions()){
    if(action.type==='start')await begin(action);
    else if(action.type==='playReplay')await playReplay(action.id,action.stage);
    else if(action.type==='demo')await playReplay(action.id,undefined,'demo');
    else if(action.type==='returnTitle')returnTitle();
    else if(action.type==='loadingIndicator')loadingScreen.showIndicator();
    else if(action.type==='menuMusic'){
      if(action.musicRoom){profile.unlockMusic(action.track);await profile.save();}
      if(action.restart||action.musicRoom||action.track!==audio.track||audio.musicPaused||audio.context&&!audio.music?.source&&audio.pendingTrack!==action.track)await playMusic(action.track);
    }
    else if(action.type==='musicWarning')stopMusic();
    else if(action.type==='pause')pauseAudio();
    else if(action.type==='stopEffects')stopEffects();
    else if(action.type==='sound')effect({id:action.id});
    else if(action.type==='titleLogo')presentation.titleLogo(action.interrupt);
    else if(action.type==='fadeMusic')fadeMusic(action.seconds);
    else if(action.type==='resume'){clearKeys();await resumeAudio();}
    else if(action.type==='settings'){await profile.saveSettings(action.settings);audio.music?.setVolume(runtimeMusic==='none'?0:nativeMusicGain(profile.settings.bgmVolume));audio.effects?.setVolume(profile.settings.seVolume);if(!audio.music?.source&&audio.track&&!audio.musicPaused&&!audio.pendingTrack)await playMusic(audio.track,{authorized:true});}
    else if(action.type==='practiceReset'){practiceBridge.reset();damageSources.reset();}
    else if(action.type==='saveName')await recordRunOnce(action.result,action.name);
    else if(action.type==='saveReplay')await saveReplay(action);
    else if(action.type==='saveExit'){stopMusic();completeRun(false);}
    else if(action.type==='result'){
      if(action.needsName===false)await recordRunOnce(action.result);
    }else if(action.type==='continue'){
      stopMusic({retain:true});stopEffects();
      if(core._th12_continue_game()!==1)throw Error('续关失败');
      session.previous=0;session.clock=0;session.cursor=0;session.recorder=null;session.resultRecorded=false;session.continues=readJSON('stage_flow').continueCount;audio.musicSerial=core._th12_music_serial();clearKeys();await playMusic(stageMusic[session.stage]);
    }else if(action.type==='retry'){
      const stage=action.mode==='practice'?action.stage:action.mode==='extra'?7:1;
      stopMusic({retain:stage===session?.stage});await begin({mode:action.mode,character:action.character,shot:action.shot,difficulty:action.difficulty,stage:action.stage,practice:action.practice});
    }else if(action.type==='quit'){audio.musicPaused=true;audio.track=audio.pendingTrack=null;++audio.request;audio.music?.stop();resetPracticeMusic();stopEffects();if(assets.managed){await syncRuntime();ready=launched=false;terminated=true;emit({event:'exit',code:0,status:'success'});}else notice('已结束。按 Z 返回标题。');}
  }
  if(model.state.scene==='title'&&!loadingScreen.startup&&loadingScreen.waiting)loadingScreen.completeResources();
}
function choices(){
  const state=model.state;if(choiceSerial===state.serial&&$('choices').dataset.selection===String(state.selection))return;choiceSerial=state.serial;
  $('choices').dataset.selection=state.selection;$('choices').replaceChildren(...state.choices.map((choice,index)=>{const option=document.createElement('div');option.textContent=choice.label;option.setAttribute('role','option');option.setAttribute('aria-selected',String(index===state.selection));option.setAttribute('aria-disabled',String(!!choice.disabled));return option;}));
}
function present(){
  const scene=model.state.scene;canvas.dataset.scene=scene;canvas.dataset.stage=String(session?.stage??0);canvas.dataset.replayCursor=String(session?.cursor??0);
  if(['playing','demo'].includes(scene)&&session){drawGamePresentation(!runtimeOptions.replayViewer||!replayWorkspace.paused&&!replayWorkspace.seeking&&replayWorkspace.rate>0);if(scene==='demo'&&model.state.age>=3540)renderer.renderMenu([],[],null,{overlay:true,shade:Math.min(1,(model.state.age-3540)/60)});}
  else if(ending){ending.render(renderer,meter.value);}
  else{
    if(session&&(['pause','pause-confirm','continue','result-menu','result','name'].includes(scene)||scene==='replay-save'&&model.state.nameContext==='pause'))drawGamePresentation();
    presentation.render(model.state,meter.value);
  }
  if(session&&!session.replay)core._th12_set_wall_time(performance.now());choices();practiceUI?.render();
}
function tick(){
  updatePracticeUI();
  const sampled=input(),controller=padInput(),pad=controller.value;
  // Outside-window menu gestures must reach THPrac before its keyboard
  // capture blocks the title menu. Widget-owned pointers never enter TouchInput.
  const touched=touch.sample(touchState(),{arrows:!!((sampled.held|pad)&0xf0)});
  if(practiceUI?.tick(pad|sampled.ui|touched.held)){presentation.tick(model.state);updatePracticeUI();return;}
  updatePracticeUI();
  if(controller.capturing){previousPadUI=0;model.tick({held:0,pressed:0});presentation.tick(model.state);return;}
  if(practiceCaptured){if(['playing','demo'].includes(model.state.scene)){gameTick({held:0});if(captureThisTick)captureGamePresentation();}else{model.tick({held:0,pressed:0});presentation.tick(model.state);}updatePracticeUI();return;}
  const ui=sampled.ui|pad|touched.held,pressed=sampled.pressed|(pad&~previousPadUI)|touched.pressed;previousPadUI=pad;
  if(ending){ending.tick({held:ui,pressed});if(ending.finished){profile.recordEndingSeen(ending.message.id);profile.save().catch(fail);ending=null;model.finish({kind:'clear',...pendingResult});}return;}
  if(model.state.scene==='playing'&&(pressed&(256|0x200000))){model.openPause(resultData(false));clearKeys();if(pressed&0x200000)model.tick({held:0x200000,pressed:0x200000});presentation.tick(model.state);return;}
  model.tick({held:ui,pressed});presentation.tick(model.state);
  // A scene may queue start/continue/retry/resume before the async action
  // worker has replaced or reinitialised the session. Never tick that old
  // session (its recorder may already have finished) during this UI tick.
  if(model.actions.some(a=>['start','playReplay','demo','continue','retry','resume','returnTitle','saveExit'].includes(a.type)))return;
  if(['playing','demo'].includes(model.state.scene)){gameTick({held:(sampled.held|pad|touched.held)&0x2ff&~256,pressed:touched.pressed&0x2ff&~256,motion:touched.motion});if(!busy&&captureThisTick)captureGamePresentation();}updatePracticeUI();
}
function frame(now){
  if(terminated)return;
  if(backgrounded||fatalError){last=now;accumulator=0;requestAnimationFrame(frame);return;}
  if(runtimeOptions.limitPresentationTo60&&now-lastPresentation<TICK-.5){requestAnimationFrame(frame);return;}
  lastPresentation=now;
  const elapsed=Math.max(0,Math.min(250,now-last));last=now;meter.frame(now);
  try{if(!fatalError){
    loadingScreen?.advance(elapsed);
    if(loadingScreen?.startup||loadingScreen?.waiting){loadingScreen.renderBase(meter.value);}
    else if(ready&&!busy&&runtimeOptions.replayViewer){
      replayWorkspace?.update(elapsed,now);logicalStep=TICK;accumulator=(replayWorkspace?.credit??0)*TICK;
      if(loadingScreen.waiting)loadingScreen.renderBase(meter.value);else{present();loadingScreen.renderOverlay();}
    }else if(ready&&!busy){
      const toolSpeed=runtimeOptions.thpracEnabled&&session&&model.state.scene==='playing',fast=keyDown(profile.settings.keyMap.skip)||keyDown('ControlRight')||!!(previousPadUI&512),shoot=keyDown(profile.settings.keyMap.shoot)||!!(previousPadUI&1),slow=keyDown(profile.settings.keyMap.focus)||keyDown('ShiftRight')||!!(previousPadUI&8);
      const rate=!toolSpeed&&session?.replay&&model.state.scene==='playing'&&fast&&!shoot?4:1;
      logicalStep=toolSpeed?1000*practiceBridge.interval({replay:!!session.replay,fast,slow,debug:keyDown('Space')}):session?.replay&&model.state.scene==='playing'&&slow?TICK*4:TICK;
      if(session?.replay&&model.state.scene==='playing'&&shoot)logicalStep=Math.min(logicalStep,TICK/4);accumulator+=elapsed*rate;
      for(let count=0;accumulator>=logicalStep&&count<Math.min(240,Math.max(8,Math.ceil(TICK/logicalStep)*8)*rate)&&!busy;count++){accumulator-=logicalStep;captureThisTick=accumulator<2*logicalStep;tick();if(model.actions.length){busy=true;actions().catch(fail).finally(()=>{if(!fatalError)busy=false;last=performance.now();accumulator=0;});break;}}
      if(loadingScreen.waiting)loadingScreen.renderBase(meter.value);else{present();loadingScreen.renderOverlay();}
    }else if(ready){present();loadingScreen.renderOverlay();}
  }}catch(error){fail(error);}
  if(ready&&launched){
    renderedFrames++;maxFrameGap=Math.max(maxFrameGap,elapsed);
    if(!firstFrame){firstFrame=true;emit({event:'first-frame'});healthAt=now;renderedFrames=0;maxFrameGap=0;}
    if(now-healthAt>=1000){emit({event:'frame-health',fps:renderedFrames*1000/(now-healthAt),maxGapMs:maxFrameGap});emit({event:'audio-health',backend:'WebAudio',contextState:audio.context?.state??'uninitialized',musicMode:runtimeMusic,voices:audio.effects?.voices.size??0});healthAt=now;renderedFrames=0;maxFrameGap=0;}
  }
  requestAnimationFrame(frame);
}
function eventKey(code,down,source='local'){
  if(!ready)return;
  if(runtimeOptions.replayViewer){if(down)audioReady().catch(error=>notice('音频：'+error.message));return;}
  if(practiceUI?.key(code,down,{repeat:down&&practiceUI.keys.has(code)})){updatePracticeUI();return;}
  const sourceKeys=source==='host'?hostKeys:keys,wasHeld=keyDown(code);
  if(down&&wasHeld){sourceKeys.add(code);return;}
  if(model.state.captureJoy){
    model.key(code,down);if(!down)sourceKeys.delete(code);
    if(!model.state.captureJoy){joyCaptureRelease=true;clearKeys();}
    if(down)audioReady().catch(error=>notice('音频：'+error.message));return;
  }
  if(down){sourceKeys.add(code);let pressed=0;const bits={up:16,down:32,left:64,right:128,shoot:1,bomb:2,focus:8,pause:256,skip:512};for(const [name,bit]of Object.entries(bits))if(profile.settings.keyMap[name]===code)pressed|=bit;if(code==='Escape')pressed|=256;if(code==='Enter')pressed|=0x80000;if(code==='KeyR')pressed|=0x200000;pendingKeys|=pressed;pendingPressed|=pressed;}
  else {sourceKeys.delete(code);if(keyDown(code))return;}
  if(model.state.captureJoy||code==='F1')model.key(code,down);
  if(down)audioReady().catch(error=>notice('音频：'+error.message));
}
window.addEventListener('keydown',e=>{if(['INPUT','SELECT','TEXTAREA'].includes(e.target?.tagName))return;if(assets.managed&&managedInput.key(e,true,'physical',eventKey)){e.preventDefault();return;}const code=keyboardCode(e);if(code==='F1'||['ArrowUp','ArrowDown','ArrowLeft','ArrowRight','Space','Enter','Escape','Tab','Backspace','F12'].includes(code))e.preventDefault();if(code)eventKey(code,true);});
window.addEventListener('keyup',e=>{if(assets.managed&&managedInput.key(e,false,'physical',eventKey)){e.preventDefault();return;}const code=keyboardCode(e);if(code)eventKey(code,false);});
for(const type of ['pointerdown','pointermove','pointerup','pointercancel'])window.addEventListener(type,event=>{
  if(!ready||busy||runtimeOptions.replayViewer)return;
  if(type==='pointerdown')try{canvas.setPointerCapture(event.pointerId);}catch{}
  if(type==='pointerdown')audioReady().catch(error=>notice('音频：'+error.message));
  if(practicePointer({id:'local:'+event.pointerId,type:type==='pointercancel'?'cancel':type.slice(7),x:event.clientX,y:event.clientY,button:event.button,pointerType:event.pointerType})){event.preventDefault();return;}
  if(event.pointerType==='mouse'||!touch.options.touchEnabled)return;
  event.preventDefault();
  const point=pointerPosition(event.clientX,event.clientY);touch.pointer({id:'local:'+event.pointerId,type:type==='pointercancel'?'cancel':type.slice(7),...point},touchState());
},{passive:false});
canvas.addEventListener('lostpointercapture',event=>{const id='local:'+event.pointerId;if(touch.points.has(id))touch.release(id,true);practiceUI?.pointer({type:'cancel',id});});
window.addEventListener('wheel',event=>{if(practiceUI?.wheel(event))event.preventDefault();},{passive:false});
window.addEventListener('blur',()=>{clearKeys();if(assets.managed)syncRuntime().catch(fail);});
document.addEventListener('visibilitychange',()=>{backgrounded=document.hidden;clearKeys();if(backgrounded){if(runtimeOptions.replayViewer)replayWorkspace?.background();else if(ready&&model.state.scene==='playing'){model.openPause(resultData(false));pauseAudio();}suspendBrowserAudio();syncRuntime().catch(fail);}else restoreBrowserAudio().catch(fail);});
window.addEventListener('pagehide',()=>{backgrounded=true;clearKeys();replayWorkspace?.background();suspendBrowserAudio();syncRuntime().catch(console.error);});
window.addEventListener('pageshow',()=>{backgrounded=document.hidden;if(!backgrounded)restoreBrowserAudio().catch(fail);});
canvas.addEventListener('webglcontextlost',event=>{event.preventDefault();clearKeys();audio.music?.stop();stopEffects();fail(Error('图形环境已失效，请退出并重新启动游戏'));});
$('fullscreen').onclick=()=>{if(document.fullscreenElement)document.exitFullscreen();else document.querySelector('.viewport').requestFullscreen();};
$('manual').onclick=()=>{if(runtimeOptions.replayViewer)return;if(session&&model.state.scene==='playing'){model.openPause(resultData(false));}model.key('F1',true);};
$('import').onchange=async()=>{try{const file=$('import').files[0];if(!file)return;if(file.size>64*1024*1024)throw Error('录像过大');const b=new Uint8Array(await file.arrayBuffer()),r=parseReplay(b),id='th12_import_'+Date.now()+'.rpy';await store.write('/savesth12/replay/'+id,b);corpus.push(replayMetadata(r,{id}));model.setCorpus(corpus);notice('录像已导入，可从 Replay 选择');}catch(error){notice(error.message);}finally{$('import').value='';}};
$('export-replay').onclick=()=>{if(!lastReplay)return;const url=URL.createObjectURL(new Blob([lastReplay.bytes],{type:'application/octet-stream'})),a=document.createElement('a');a.href=url;a.download=lastReplay.id;a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);};

async function initialize(){
  //The first visible page is the original sig.anm splash. Bootstrap only
  //needs its small resource manifest, not the runtime or all stage assets.
  const bootResponse=await assets.fetch('/assets/loading-resources.json');if(!bootResponse.ok)throw Error('原版加载画面资源缺失，请更新本地菜单资源');
  const boot=await bootResponse.json();
  if(boot.schema!=='th12-loading-resources/1'||!boot.atlases?.sig||!boot.atlases?.ascii)throw Error('原版加载画面资源无效');
  renderer=new Renderer(canvas,boot.atlases,{fetcher:assets.fetch});loadingScreen=new LoadingPresentation(renderer);
  await renderer.loadAtlases(['sig']);loadingScreen.beginStartup();last=performance.now();meter.reset(last);requestAnimationFrame(frame);
  await renderer.loadAtlases(['ascii']);loadingScreen.showIndicator();
  store??=await openStore();profile=await new GameProfile(store).load();
  const [atlases,menus,index,music,sounds]=await Promise.all(['/assets/atlases.json','/assets/menu-resources.json','/replays/index.json','/assets/music-index.json','/assets/sound-index.json'].map(async url=>{if(assets.managed&&url==='/replays/index.json')return {fixtures:[]};const r=url.startsWith('/assets/')?await assets.fetch(url):await fetch(url);if(!r.ok)throw Error('资源清单加载失败');return r.json();}));
  resources=validateMenuResources(localization?.applyMenuResources(menus)??menus);musicIndex=music;soundIndex=sounds;Object.assign(atlases,resources.atlases);
  Object.assign(renderer.atlases,atlases);await renderer.prepare();await renderer.loadAtlases(['title','title_v','staff',...resources.endings.map(e=>e.bank)]);
  core=await createCore();if(!core._th12_start_full_game||!core._th12_load_spell_history)throw Error('请更新完整游戏核心');loader=new StageLoader(core,atlases,file=>bytes('/assets/'+file));
  corpus=await Promise.all(index.fixtures.filter(r=>!/^demo/.test(r.id)).map(async fixture=>{
    //The local fixture index describes inputs. Its menu metadata must come
    //from the actual retail header, as it does for imported/saved replays.
    try{return replayMetadata(parseReplay(await bytes('/replays/'+encodeURIComponent(fixture.id))),fixture);}
    catch(error){console.warn('录像信息读取失败 '+fixture.id+': '+error.message);return fixture;}
  }));
  for(const path of await store.list())if(/^\/savesth12\/replay\/[^/]+\.rpy$/i.test(path))try{const b=await store.read(path),r=parseReplay(b),id=path.split('/').at(-1);corpus=corpus.filter(f=>f.id!==id);corpus.push(replayMetadata(r,{id}));}catch(error){console.warn(error.message);}
  practiceBridge=new PracticeBridge(core);damageSources=new DamageSourceBridge(core);practiceUI=new PracticeOverlay({bridge:practiceBridge,canvas,onError:fail,onMenuAction:action=>model.practiceAction(action),onRetry:()=>{model.openPause(resultData(false));model.state.readyAt=0;model.quickRetry();}});
  if(runtimeOptions.replayViewer)runtimeOptions={...runtimeOptions,thpracEnabled:true};
  practiceUI.configure({enabled:!!runtimeOptions.thpracEnabled&&!runtimeOptions.replayViewer,coreEnabled:!!runtimeOptions.thpracEnabled,locale:runtimeOptions.thpracLocale??'ja-JP'});
  await practiceUI.initialize(assets.fonts);
  if(runtimeOptions.thpracEnabled&&!practiceBridge.snapshot().enabled)throw Error('当前构建未包含 THPrac，请重新生成完整适配版');
  model=new TitleScenes({profile,corpus,settings:profile.settings,musicTracks:resources.musicComments,practiceEnabled:!!runtimeOptions.thpracEnabled,practiceLocale:runtimeOptions.thpracLocale??'ja-JP'});presentation=new TitlePresentation(renderer,resources,profile);
  if(runtimeOptions.replayViewer)replayWorkspace=new ReplayWorkspace({
    read:path=>store.read(path),state:()=>({stage:session?.stage??0,cursor:session?.cursor??0}),busy:()=>busy||backgrounded||terminated||fatalError,
    open:async(replay,stage)=>{await begin({mode:'replay',character:replay.character,shot:replay.shot,difficulty:replay.difficulty,stage},replay);model.enter('playing',0,{readyAt:0});captureGamePresentation();},
    transition:viewerStage,step:()=>{gameTick({held:0});captureGamePresentation();},
    resetTime:()=>{last=performance.now();accumulator=0;},silence:()=>{stopEffects();pauseAudio();},transport:viewerTransport,
    report:playback=>emit({event:'runtime-info',playback}),error:fail,
  });
  // Start the title transport during initialization, including hosts that do
  // not publish a shared AudioContext. Later pointer/key gestures can resume
  // a context suspended by browser autoplay policy without entering a menu.
  audioReady().catch(error=>notice('音频：'+error.message));
  await loadingScreen.waitForStartup();await actions();loadingScreen.endStartup();ready=true;last=performance.now();meter.reset(last);accumulator=0;
  if(!assets.managed)window.__th12Game={ready:()=>ready,state:()=>({scene:model.view(),session:session?{mode:session.mode,stage:session.stage,cursor:session.cursor}:null,flow:session?readJSON('stage_flow'):null}),profile:()=>profile.data,start:settings=>{model.run={...settings};model.enter('playing',0,{readyAt:0});return begin(settings);},model:()=>model,present};
}
async function syncRuntime(){if(launched&&profile){if(core&&!session?.replay&&!runtimeOptions.replayViewer)await saveProfile();else await profile.save();}await store?.sync();}
async function launchRuntime(){
  if(terminated)throw Error('此运行会话已结束，请从启动器重新打开');
  if(launched)throw Error('游戏已启动');launched=true;firstFrame=false;
  try{localization??=await prepareLocalization();localization.activate();assets.localization=localization;await initialize();if(runtimeOptions.replayPath){if(runtimeOptions.replayViewer)await replayWorkspace.command({action:'open',path:runtimeOptions.replayPath.startsWith('/savesth12/')?runtimeOptions.replayPath:'/savesth12/'+runtimeOptions.replayPath});else{const id=runtimeOptions.replayPath.split('/').at(-1);await playReplay(id);model.enter('playing',0,{readyAt:0});}}emit({event:'runtime-info',renderer:'WebGL2',architecture:'C++20 / WebAssembly',version:'0.3.0-cpp',integration:'manual-acceptance-pending',language:localization.language,completeAdapter:false});}
  catch(error){launched=false;throw error;}
}
async function bootstrap(){
  if(!assets.managed){await launchRuntime();return;}
  document.documentElement.dataset.managed='true';
  emit=installManagedShell({
    get store(){return store;},running:()=>launched,replayViewer:()=>!!runtimeOptions.replayViewer,configure:async settings=>{
      const installed=new Map(),pack=settings.runtimePack;
      if(pack){
        if(!Array.isArray(pack.files)||pack.files.length>256)throw Error('语言包安装列表无效');
        let total=0;
        for(const file of pack.files){
          if(!file||!isLocalizationResourcePath(file.path)||installed.has(file.path)||!ArrayBuffer.isView(file.bytes))throw Error('语言包只能安装受支持的文字和贴图');
          const data=new Uint8Array(file.bytes.buffer,file.bytes.byteOffset,file.bytes.byteLength);total+=data.byteLength;
          if(!data.length||total>96*1024*1024)throw Error('语言包安装大小无效');installed.set(file.path,data);
        }
      }
      if(settings.language!=='ja'&&!assets.fonts.has('/unifont.otf'))throw Error('语言字体尚未安装');
      const provider=await prepareLocalization({language:settings.language,manifest:pack?.manifest,readFile:path=>installed.get(path),fontReady:installRuntimeFonts(assets.fonts)});
      localization?.deactivate();localization=provider;assets.localization=provider;runtimeOptions=settings.options;runtimeMusic=settings.music;touch.configure(runtimeOptions);
    },
    resources:async items=>{await assets.install(items,message=>emit(message));await installRuntimeFonts(assets.fonts);},launch:launchRuntime,
    playback:message=>{if(!ready||!runtimeOptions.replayViewer||!replayWorkspace)throw Error('请先打开录像工作区');return replayWorkspace.command(message);},
    key:(code,down,message)=>{if(!managedInput.key(message,down,message.inputSource==='physical'?'physical':'virtual',eventKey))eventKey(keyboardCode(message),down,'host');},clearKeyboard:()=>{clearKeys({touchToo:false,practiceToo:false});practiceUI?.clearKeyboard();},cancelTouch:()=>{touch.cancel();practiceUI?.cancel();},
    touch:message=>{touch.controls(message,ready&&!busy?touchState():{key:'inactive',kind:'menu',instance:'',ready:false});practiceUI?.mobilePanels(message);updatePracticeUI();},pointer:hostPointer,practicePointer,sync:syncRuntime,
  },window,{announceReady:false});
  store=await openStore();await assets.prepare();emit.ready();
}
bootstrap().catch(fail);
