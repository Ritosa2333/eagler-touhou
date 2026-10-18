import createCore from './runtime/core.mjs';
import {Renderer} from '/src/renderer.mjs';
import {TitleScenes} from '/src/title-scenes.mjs';
import {TitlePresentation} from '/src/title-presentation.mjs';
import {validateMenuResources,selectEnding} from '/src/menu-resources.mjs';
import {EndingTimeline} from '/src/ending-timeline.mjs';
import {GameProfile} from '/src/game-profile.mjs';
import {openStore} from '/src/storage.mjs';
import {StageLoader,stageMusic,bossMusic} from '/src/stage-loader.mjs';
import {MusicPlayer} from '/src/music.mjs';
import {SoundPlayer,nativeMusicGain} from '/src/sound-player.mjs';
import {readCoreSoundEvents} from '/src/sound-events.mjs';
import {parseReplay,replayInputEnded} from '/src/replay.mjs';
import {startReplayStage} from '/src/replay-session.mjs';
import {ReplayRecorder,defaultReplayConfig} from '/src/replay-recorder.mjs';
import {createFrameRateMeter} from '/src/fps-presentation.mjs';
import {decodeSpellTitle} from '/src/spell-presentation.mjs';
import {LoadingPresentation} from '/src/loading-presentation.mjs';

const $=id=>document.getElementById(id),canvas=$('screen'),keys=new Set(),meter=createFrameRateMeter(),TICK=1000/60;
let core,renderer,loader,profile,store,resources,model,presentation,loadingScreen,corpus=[],musicIndex=[],soundIndex=[],session=null,ending=null,ready=false,busy=false,fatalError=false,last=0,accumulator=0,previousUI=0,previousPadUI=0,pendingKeys=0,pendingPressed=0,choiceSerial=-1,lastReplay=null,pendingResult=null,joyCaptureRelease=false;
const audio={context:null,music:null,track:null,pendingTrack:null,musicPaused:false,request:0,effects:null,effectsReady:null,musicSerial:-1};
const decoder=new TextDecoder();
const readJSON=name=>{const p=core['_th12_'+name](),size=core['_th12_'+name+'_size']();return JSON.parse(decoder.decode(core.HEAPU8.subarray(p,p+size)));};
const hud=()=>[...new Float32Array(core.HEAPU8.buffer,core._th12_hud(),core._th12_hud_size())];
async function bytes(url){const r=await fetch(url);if(!r.ok)throw Error('资源加载失败 '+url);return new Uint8Array(await r.arrayBuffer());}
function notice(text=''){$('notice').textContent=text;}
function fail(error){fatalError=true;busy=true;notice('运行中断：'+error.message);console.error(error);}
function clearKeys(){keys.clear();model?.clearKeys();previousUI=previousPadUI=pendingKeys=pendingPressed=0;if(session)session.previous=0;accumulator=0;last=performance.now();meter.reset(last);}

function input(){
  const map=profile.settings.keyMap,held=(name,defaults=[])=>keys.has(map[name])||defaults.some(code=>keys.has(code));
  let native=0;
  if(held('up'))native|=16;if(held('down'))native|=32;if(held('left'))native|=64;if(held('right'))native|=128;
  if(held('shoot'))native|=1;if(held('bomb'))native|=2;if(held('focus',['ShiftRight']))native|=8;if(held('skip',['ControlRight']))native|=512;
  native|=pendingKeys&0x2ff;
  let ui=native|pendingKeys;if(keys.has('Escape')||held('pause'))ui|=256;if(keys.has('Enter'))ui|=0x80000;if(keys.has('KeyR'))ui|=0x200000;
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
  if(!p)return {value:0,capturing:false};
  let value=0;if(p.axes[0]<-.3||p.buttons[14]?.pressed)value|=64;if(p.axes[0]>.3||p.buttons[15]?.pressed)value|=128;
  if(p.axes[1]<-.3||p.buttons[12]?.pressed)value|=16;if(p.axes[1]>.3||p.buttons[13]?.pressed)value|=32;
  for(const [name,bit]of Object.entries({shoot:1,bomb:2,focus:8,pause:256,skip:512}))if(p.buttons[profile.settings.joyButtons[name]]?.pressed)value|=bit;
  return {value,capturing:false};
}
async function audioReady(){
  const Audio=window.AudioContext??window.webkitAudioContext;if(!Audio)return;
  audio.context??=new Audio();
  audio.music??=new MusicPlayer({context:audio.context,readBytes:file=>bytes('/assets/music/'+file)});
  audio.effects??=new SoundPlayer({context:audio.context,index:soundIndex,readBytes:file=>bytes('/assets/'+file),volume:profile.settings.seVolume});
  audio.music.setVolume(nativeMusicGain(profile.settings.bgmVolume));
  audio.effects.setVolume(profile.settings.seVolume);
  // One shared load prevents repeated key presses from decoding the whole bank
  // again. Events arriving during decode retain their channel until it is ready.
  audio.effectsReady??=audio.effects.preload().catch(error=>{audio.effectsReady=null;throw error;});
  await Promise.all([audio.context.resume(),audio.effectsReady]);
  if(audio.track&&!audio.music.source&&!audio.musicPaused&&!audio.pendingTrack)await playMusic(audio.track);
}
async function playMusic(id,{offsetSeconds=0}={}){
  audio.track=id;audio.pendingTrack=null;audio.musicPaused=false;const request=++audio.request;
  if(session&&!session.replay)profile.unlockMusic(id);
  if(!audio.context||!audio.music)return;
  const track=musicIndex.find(t=>t.id===id);if(!track)throw Error('音乐不存在 '+id);
  audio.music.setVolume(nativeMusicGain(profile.settings.bgmVolume));
  audio.pendingTrack=id;
  try{await audio.music.play(track,()=>request===audio.request,{offsetSeconds});}
  finally{if(request===audio.request)audio.pendingTrack=null;}
}
function effect(event){audio.effects?.play(event);}
function stopEffects(){audio.effects?.stop();}
function pauseAudio(){audio.musicPaused=true;audio.pendingTrack=null;++audio.request;audio.music?.pause();audio.effects?.pause();}
async function resumeAudio(){
  audio.musicPaused=false;audio.effects?.resume();
  if(!audio.context||!audio.music)return;
  await audio.context.resume();
  const request=++audio.request;audio.pendingTrack=audio.track;
  try{if(audio.music.paused)await audio.music.resume(()=>request===audio.request);else if(!audio.music.source&&audio.track)await playMusic(audio.track);}
  finally{if(request===audio.request)audio.pendingTrack=null;}
}
function loadSpellHistory(){const words=profile.spellHistoryWords(),p=core._malloc(words.length*4);try{new Int32Array(core.HEAPU8.buffer,p,words.length).set(words);if(core._th12_load_spell_history(p,words.length)!==1)throw Error('符卡记录载入失败');}finally{core._free(p);}}
function saveProfile(){profile.updateSpellHistory(readJSON('spell_history').records);return profile.save().catch(error=>notice('记录保存失败：'+error.message));}
async function prepareStage(stage,{demo=false}={}){await renderer.prepareStage(stage);if(!await loader.load(stage))throw Error('关卡准备被取消');if(audio.context){const tracks=demo?[]:musicIndex.filter(t=>[stageMusic[stage],bossMusic[stage]].includes(t.id));await Promise.all([audio.music.preload(tracks),audio.effectsReady]);}}
function recordedHeader(flow){const h=flow.recordHeader;if(!h)throw Error('缺少关卡开始记录');return {number:flow.stage,seed:h.seed,initial:{x:h.x,y:h.y,power:h.power,lives:h.lives,bombs:h.bombs,score:h.scoreStored,pointValue:h.pointValueStored,lifeFragments:h.lifeFragments,bombFragments:h.bombFragments,ufoColors:h.ufoColors,rank:h.rank,nativeGlobals:h.nativeGlobals}};}
function recordStage(){const flow=readJSON('stage_flow');session.recorder?.recordStage(recordedHeader(flow));session.resultKey=null;session.spellCount=0;}
function recordingConfig(){
  const bytes=defaultReplayConfig(),view=new DataView(bytes.buffer),buttons=profile.settings.joyButtons;
  for(const [i,key]of ['shoot','bomb','focus','pause'].entries())view.setUint16(4+i*2,buttons[key],true);
  view.setUint16(20,buttons.skip,true);bytes[0x20]=profile.settings.bgmVolume;bytes[0x21]=profile.settings.seVolume;return bytes;
}

async function begin(settings,replay=null,id=null){
  busy=true;loadingScreen.startResources();
  if(session&&!session.replay)await saveProfile();
  clearKeys();stopEffects();ending=null;pendingResult=null;
  try{
    const stage=settings.stage??1;
    await prepareStage(stage,{demo:settings.mode==='demo'});loadSpellHistory();
    session={...settings,stage,replay,id,cursor:0,previous:0,clock:0,playTicks:0,resultKey:null,spellCount:0,continues:0,transitioning:false,clearRecorded:false,resultRecorded:false};
    if(replay){const part=replay.stages.find(s=>s.number===stage);if(!part)throw Error('录像不包含关卡');startReplayStage(core,replay,part);core._th12_set_full_game_mode(replay.practice?1:0);}
    else{
      const seed=crypto.getRandomValues(new Uint16Array(1))[0];
      if(core._th12_start_full_game(settings.character,settings.shot,settings.difficulty,seed,settings.mode==='practice'?1:0)!==1)throw Error('完整游戏初始化失败');
      session.recorder=new ReplayRecorder({character:settings.character,shot:settings.shot,difficulty:settings.difficulty,practice:settings.mode==='practice',configBytes:recordingConfig()});recordStage();
      profile.recordPlayStart(settings.character,settings.shot,settings.difficulty);
      profile.recordStageEntry(settings.character,settings.shot,settings.difficulty,stage);
    }
    model.run={...settings};audio.musicSerial=core._th12_music_serial();
    // Native4221bd and422928 gate the road/Boss slots on Demo flag20.
    // The title stream continues at its current playhead throughout the Demo.
    if(settings.mode!=='demo'){
      const track=musicIndex[core._th12_music_track()]?.id??stageMusic[stage];if(!replay)profile.unlockMusic(track);await playMusic(track);
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
async function nextStage(){
  if(session?.mode==='demo'){model.returnTitle();return;}
  if(!session||session.transitioning)return;session.transitioning=true;busy=true;
  try{
    const flow=readJSON('stage_flow');session.recorder?.finishStage();
    const stage=session.replay?session.replay.stages.find(s=>s.number>session.stage)?.number:flow.nextStage;
    if(!stage){completeRun(true);return;}
    loadingScreen.startResources();
    await prepareStage(stage);session.stage=stage;session.cursor=0;session.previous=0;session.resultKey=null;session.spellCount=0;
    if(session.replay){startReplayStage(core,session.replay,session.replay.stages.find(s=>s.number===stage));core._th12_set_full_game_mode(session.replay.practice?1:0);}
    else{if(core._th12_start_next_stage()!==1)throw Error('跨关资源继承失败');recordStage();profile.recordStageEntry(session.character,session.shot,session.difficulty,stage);await saveProfile();}
    audio.musicSerial=core._th12_music_serial();const track=musicIndex[core._th12_music_track()]?.id??stageMusic[stage];if(!session.replay)profile.unlockMusic(track);await playMusic(track);
  }finally{if(session)session.transitioning=false;if(!fatalError){loadingScreen.completeResources();busy=false;}last=performance.now();meter.reset(last);accumulator=0;}
}
function resultData(clear){const flow=readJSON('stage_flow'),scoreStored=core._th12_score_stored(),timestamp=Math.floor(Date.now()/1000);return {mode:session.mode,practice:session.mode==='practice',character:session.character,shot:session.shot,difficulty:session.difficulty,stage:session.stage,scoreStored,score:scoreStored*10+flow.continueCount,timestamp,date:new Date(timestamp*1000).toISOString(),playSeconds:session.playTicks/60,clear,continueCount:flow.continueCount,continues:flow.continueCount,canContinue:flow.canContinue,canSaveReplay:!session.replay&&flow.continueCount===0,ranked:flow.continueCount===0};}
function recordClear(result){
  if(!session||session.replay||session.mode==='practice'||session.clearRecorded||!result.clear)return;
  session.clearRecorded=true;profile.recordClear(session.character,session.shot,session.difficulty);
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
  model.enter('ending',0,{readyAt:0});ending=new EndingTimeline({resources,atlases:renderer.atlases,ending:resources.endings[id],difficulty:session.difficulty,seen:profile.data.endingsSeen.includes(id),onMusic:id=>playMusic(id).catch(fail),onFadeMusic:()=>audio.music?.fade(3),onSound:id=>effect({id})});clearKeys();
}
function returnTitle(){loadingScreen.startResources();session=null;ending=null;stopEffects();clearKeys();saveProfile();}

function gameTick(input){
  if(!session)return;
  let held=input.held,pressed=held&~session.previous;
  if(session.replay){
    const recorded=session.replay.input(session.stage,session.cursor);
    if(replayInputEnded(recorded)){if(session.mode==='demo')model.returnTitle();else nextStage().catch(fail);return;}
    ({held,pressed}=recorded);session.cursor++;
  }else session.recorder?.pushInput({held,pressed,released:session.previous&~held},meter.value||60);
  session.previous=held;core._th12_tick(held,pressed);session.clock++;if(!session.replay)core._th12_set_wall_time(performance.now());
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
  const {data,input,recordedFps,...header}=replay;
  const timestamp=header.timestamp,date=Number.isSafeInteger(timestamp)&&timestamp>0?new Date(timestamp*1000):null;
  return {...extra,...header,score:header.scoreStored*10+(header.continues??0),clear:header.finalStage===8,
    ...(date&&Number.isFinite(date.getTime())?{date:date.toISOString()}:{} )};
}
async function saveReplay(action){
  if(!session?.recorder||!action.result?.canSaveReplay)return;
  const id='th12_'+String(action.slot).padStart(2,'0')+'.rpy',b=session.recorder.result({name:action.name,timestamp:action.result.timestamp,scoreStored:action.result.scoreStored??Math.trunc(action.result.score/10),continues:action.result.continues,cleared:action.result.clear,finalStage:action.result.clear?8:action.result.stage});
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
    else if(action.type==='musicWarning'){audio.musicPaused=true;audio.pendingTrack=null;++audio.request;audio.music?.stop();}
    else if(action.type==='pause')pauseAudio();
    else if(action.type==='stopEffects')stopEffects();
    else if(action.type==='sound')effect({id:action.id});
    else if(action.type==='titleLogo')presentation.titleLogo(action.interrupt);
    else if(action.type==='fadeMusic')audio.music?.fade(action.seconds);
    else if(action.type==='resume'){clearKeys();await resumeAudio();}
    else if(action.type==='settings'){await profile.saveSettings(action.settings);audio.music?.setVolume(nativeMusicGain(profile.settings.bgmVolume));audio.effects?.setVolume(profile.settings.seVolume);if(!audio.music?.source&&audio.track&&!audio.musicPaused&&!audio.pendingTrack)await playMusic(audio.track);}
    else if(action.type==='saveName')await recordRunOnce(action.result,action.name);
    else if(action.type==='saveReplay')await saveReplay(action);
    else if(action.type==='saveExit')completeRun(false);
    else if(action.type==='result'){
      if(action.needsName===false)await recordRunOnce(action.result);
    }else if(action.type==='continue'){
      stopEffects();
      if(core._th12_continue_game()!==1)throw Error('续关失败');
      session.previous=0;session.clock=0;session.cursor=0;session.recorder=null;session.resultRecorded=false;session.continues=readJSON('stage_flow').continueCount;audio.musicSerial=core._th12_music_serial();clearKeys();await playMusic(stageMusic[session.stage]);
    }else if(action.type==='retry')await begin({mode:session?.mode??action.mode,character:session?.character??action.character,shot:session?.shot??action.shot,difficulty:session?.difficulty??action.difficulty,stage:session?.mode==='practice'?session.stage:session?.mode==='extra'?7:1});
    else if(action.type==='quit'){audio.music?.stop();notice('已结束。按 Z 返回标题。');}
  }
  if(model.state.scene==='title'&&!loadingScreen.startup&&loadingScreen.waiting)loadingScreen.completeResources();
}
function choices(){
  const state=model.state;if(choiceSerial===state.serial&&$('choices').dataset.selection===String(state.selection))return;choiceSerial=state.serial;
  $('choices').dataset.selection=state.selection;$('choices').replaceChildren(...state.choices.map((choice,index)=>{const button=document.createElement('button');button.textContent=choice.label;button.disabled=choice.disabled;button.setAttribute('aria-current',index===state.selection?'true':'false');button.onclick=()=>{audioReady().catch(console.warn);model.choose(index);};return button;}));
}
function present(){
  const scene=model.state.scene;canvas.dataset.scene=scene;canvas.dataset.stage=String(session?.stage??0);canvas.dataset.replayCursor=String(session?.cursor??0);
  if(['playing','demo'].includes(scene)&&session){renderer.render(core,{currentFps:meter.value,scoreLowDigit:session.continues,stageClear:readJSON('stage_flow').status==='stage-clear',practice:session.mode==='practice'||session.replay?.practice,recordedFps:session.replay?.recordedFps(session.stage,session.cursor)??null});if(scene==='demo'&&model.state.age>=3540)renderer.renderMenu([],[],null,{overlay:true,shade:Math.min(1,(model.state.age-3540)/60)});}
  else if(ending){ending.render(renderer,meter.value);}
  else{
    if(session&&(['pause','pause-confirm','continue','result-menu','result','name'].includes(scene)||scene==='replay-save'&&model.state.nameContext==='pause'))renderer.render(core,{currentFps:meter.value,recordedFps:session.replay?.recordedFps(session.stage,session.cursor)??null});
    presentation.render(model.state,meter.value);
  }
  if(session&&!session.replay)core._th12_set_wall_time(performance.now());choices();
}
function tick(){
  const sampled=input(),controller=padInput(),pad=controller.value;
  if(controller.capturing){previousPadUI=0;model.tick({held:0,pressed:0});presentation.tick(model.state);return;}
  const ui=sampled.ui|pad,pressed=sampled.pressed|(pad&~previousPadUI);previousPadUI=pad;
  if(ending){ending.tick({held:ui,pressed});if(ending.finished){profile.recordEndingSeen(ending.message.id);profile.save().catch(fail);ending=null;model.finish({kind:'clear',...pendingResult});}return;}
  if(model.state.scene==='playing'&&(pressed&256)){model.openPause(resultData(false));clearKeys();presentation.tick(model.state);return;}
  model.tick({held:ui,pressed});presentation.tick(model.state);
  // A scene may queue start/continue/retry/resume before the async action
  // worker has replaced or reinitialised the session. Never tick that old
  // session (its recorder may already have finished) during this UI tick.
  if(model.actions.some(a=>['start','playReplay','demo','continue','retry','resume','returnTitle','saveExit'].includes(a.type)))return;
  if(['playing','demo'].includes(model.state.scene))gameTick({held:sampled.held|(pad&~256)});
}
function frame(now){
  const elapsed=Math.max(0,Math.min(250,now-last));last=now;meter.frame(now);
  try{if(!fatalError){
    loadingScreen?.advance(elapsed);
    if(loadingScreen?.startup||loadingScreen?.waiting){loadingScreen.renderBase(meter.value);}
    else if(ready&&!busy){
      const rate=session?.replay&&model.state.scene==='playing'&&(keys.has(profile.settings.keyMap.skip)||keys.has('ControlRight')||(previousPadUI&512))?4:1;accumulator+=elapsed*rate;
      for(let count=0;accumulator>=TICK&&count<8*rate&&!busy;count++){accumulator-=TICK;tick();if(model.actions.length){busy=true;actions().catch(fail).finally(()=>{if(!fatalError)busy=false;last=performance.now();accumulator=0;});break;}}
      if(loadingScreen.waiting)loadingScreen.renderBase(meter.value);else{present();loadingScreen.renderOverlay();}
    }else if(ready){present();loadingScreen.renderOverlay();}
  }}catch(error){fail(error);}
  requestAnimationFrame(frame);
}
function eventKey(code,down){
  if(!ready)return;
  if(model.state.captureJoy){
    model.key(code,down);if(!down)keys.delete(code);
    if(!model.state.captureJoy){joyCaptureRelease=true;clearKeys();}
    if(down)audioReady().catch(error=>notice('音频：'+error.message));return;
  }
  if(down){keys.add(code);let pressed=0;const bits={up:16,down:32,left:64,right:128,shoot:1,bomb:2,focus:8,pause:256,skip:512};for(const [name,bit]of Object.entries(bits))if(profile.settings.keyMap[name]===code)pressed|=bit;if(code==='Escape')pressed|=256;if(code==='Enter')pressed|=0x80000;if(code==='KeyR')pressed|=0x200000;pendingKeys|=pressed;pendingPressed|=pressed;}
  else keys.delete(code);
  if(model.state.captureJoy||code==='F1')model.key(code,down);
  if(down)audioReady().catch(error=>notice('音频：'+error.message));
}
window.addEventListener('keydown',e=>{if(['INPUT','SELECT','TEXTAREA'].includes(e.target?.tagName))return;if(e.code==='F1'||['ArrowUp','ArrowDown','ArrowLeft','ArrowRight','Space','Enter','Escape'].includes(e.code))e.preventDefault();if(!e.repeat)eventKey(e.code,true);});
window.addEventListener('keyup',e=>eventKey(e.code,false));
window.addEventListener('blur',()=>{if(ready&&model.state.scene==='playing'){model.openPause(resultData(false));}clearKeys();});
document.addEventListener('visibilitychange',()=>{if(document.hidden)clearKeys();});
canvas.addEventListener('pointerdown',e=>{
  if(!ready)return;audioReady().catch(console.warn);canvas.focus();
  const x=(e.clientX-canvas.getBoundingClientRect().left)*640/canvas.clientWidth,y=(e.clientY-canvas.getBoundingClientRect().top)*480/canvas.clientHeight,s=model.state;
  if(!['playing','demo','ending'].includes(s.scene)&&s.choices.length){const row=presentation.hitTest(s,x,y);if(row>=0&&row<s.choices.length)model.choose(row);}
});
for(const button of document.querySelectorAll('#touch button')){button.onpointerdown=e=>{e.preventDefault();button.setPointerCapture(e.pointerId);eventKey(button.dataset.key,true);};for(const type of ['pointerup','pointercancel','lostpointercapture'])button.addEventListener(type,()=>eventKey(button.dataset.key,false));}
$('fullscreen').onclick=()=>{if(document.fullscreenElement)document.exitFullscreen();else document.querySelector('.viewport').requestFullscreen();};
$('manual').onclick=()=>{if(session&&model.state.scene==='playing'){model.openPause(resultData(false));}model.key('F1',true);};
$('import').onchange=async()=>{try{const file=$('import').files[0];if(!file)return;if(file.size>64*1024*1024)throw Error('录像过大');const b=new Uint8Array(await file.arrayBuffer()),r=parseReplay(b),id='th12_import_'+Date.now()+'.rpy';await store.write('/savesth12/replay/'+id,b);corpus.push(replayMetadata(r,{id}));model.setCorpus(corpus);notice('录像已导入，可从 Replay 选择');}catch(error){notice(error.message);}finally{$('import').value='';}};
$('export-replay').onclick=()=>{if(!lastReplay)return;const url=URL.createObjectURL(new Blob([lastReplay.bytes],{type:'application/octet-stream'})),a=document.createElement('a');a.href=url;a.download=lastReplay.id;a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);};

async function initialize(){
  //The first visible page is the original sig.anm splash. Bootstrap only
  //needs its small resource manifest, not the runtime or all stage assets.
  const bootResponse=await fetch('/assets/loading-resources.json');if(!bootResponse.ok)throw Error('原版加载画面资源缺失，请更新本地菜单资源');
  const boot=await bootResponse.json();
  if(boot.schema!=='th12-loading-resources/1'||!boot.atlases?.sig||!boot.atlases?.ascii)throw Error('原版加载画面资源无效');
  renderer=new Renderer(canvas,boot.atlases);loadingScreen=new LoadingPresentation(renderer);
  await renderer.loadAtlases(['sig']);loadingScreen.beginStartup();last=performance.now();meter.reset(last);requestAnimationFrame(frame);
  await renderer.loadAtlases(['ascii']);loadingScreen.showIndicator();
  store=await openStore();profile=await new GameProfile(store).load();
  const [atlases,menus,index,music,sounds]=await Promise.all(['/assets/atlases.json','/assets/menu-resources.json','/replays/index.json','/assets/music-index.json','/assets/sound-index.json'].map(async url=>{const r=await fetch(url);if(!r.ok)throw Error('资源清单加载失败');return r.json();}));
  resources=validateMenuResources(menus);musicIndex=music;soundIndex=sounds;Object.assign(atlases,resources.atlases);
  Object.assign(renderer.atlases,atlases);await renderer.prepare();await renderer.loadAtlases(['title','title_v','staff',...resources.endings.map(e=>e.bank)]);
  core=await createCore();if(!core._th12_start_full_game||!core._th12_load_spell_history)throw Error('请更新完整游戏核心');loader=new StageLoader(core,atlases,file=>bytes('/assets/'+file));
  corpus=await Promise.all(index.fixtures.filter(r=>!/^demo/.test(r.id)).map(async fixture=>{
    //The local fixture index describes inputs. Its menu metadata must come
    //from the actual retail header, as it does for imported/saved replays.
    try{return replayMetadata(parseReplay(await bytes('/replays/'+encodeURIComponent(fixture.id))),fixture);}
    catch(error){console.warn('录像信息读取失败 '+fixture.id+': '+error.message);return fixture;}
  }));
  for(const path of await store.list())if(path.endsWith('.rpy'))try{const b=await store.read(path),r=parseReplay(b),id=path.split('/').at(-1);corpus=corpus.filter(f=>f.id!==id);corpus.push(replayMetadata(r,{id}));}catch(error){console.warn(error.message);}
  model=new TitleScenes({profile,corpus,settings:profile.settings,musicTracks:resources.musicComments});presentation=new TitlePresentation(renderer,resources,profile);
  await loadingScreen.waitForStartup();await actions();loadingScreen.endStartup();ready=true;last=performance.now();meter.reset(last);accumulator=0;
  window.__th12Game={ready:()=>ready,state:()=>({scene:model.view(),session:session?{mode:session.mode,stage:session.stage,cursor:session.cursor}:null,flow:session?readJSON('stage_flow'):null}),profile:()=>profile.data,start:settings=>{model.run={...settings};model.enter('playing',0,{readyAt:0});return begin(settings);},model:()=>model,present};
}
initialize().catch(fail);
