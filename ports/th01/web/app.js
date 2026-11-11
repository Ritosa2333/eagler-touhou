import {mountHorizontalTouch,mountTouchPreferences} from './th01-touch.js';
import {loadMusicTrack} from './bgm-track.js';
import {onBgmVersion} from './bgm-version.js';
import {populateStages,stageLabel,stageSelection,applyStage} from './stage-select.js';
import {musicRoomTracks} from './music-room.js';
import {createRecorder} from './replay/recorder.js';
import {NativeLocalSimulation,nativeLocalOpening,localExportFacade} from './replay/native-local-core.js';
const $=id=>document.getElementById(id);
const solo=document.body.dataset.page==='solo';
populateStages($('startStage'));
let started=false,configuring=true,sessionConfig;
const preferenceKey=`th01-native-${solo?'solo':'local'}-setup`;
try{
  const saved=JSON.parse(localStorage.getItem(preferenceKey)||'null');
  if(saved){
    for(const id of ['players','orbs','difficulty','startStage'])if([...$(id).options].some(o=>o.value===saved[id]))$(id).value=saved[id];
    for(const id of ['compensation','reducedFlash'])if(typeof saved[id]==='boolean')$(id).checked=saved[id];
  }
}catch{}
function setupSummary(){
  $('setupSummary').textContent=`${solo?1:$('players').value} 人 · ${$('orbs').value} 玉 · ${$('difficulty').selectedOptions[0].textContent} · ${$('compensation').checked?'多玉生命补偿开启':'生命补偿关闭'}。${stageLabel($('startStage').value)}。${stageSelection($('startStage').value).practice?'选关练习，直接入场；成绩不保存到正式排行榜。':solo?'先进入标题菜单，再选择 START。':'直接进入第一关过场。'}`;
}
setupSummary();
const tick=440/24830*1000;
const keys=[['ArrowLeft','ArrowRight','KeyZ','KeyX','KeyC'],['KeyA','KeyD','KeyJ','KeyK','KeyL'],['KeyF','KeyH','KeyT','KeyY','KeyU']];
const held=new Set(),pressed=new Set(),touchOwners=new Map();
let core,paused=false,last=0,accumulator=0,steps=0,logicMs=0,drawMs=0,draws=0,statsAt=performance.now(),lastDraw=-Infinity;
let musicContext,musicBuffer,musicSource,musicGain,musicEnabled=false,musicWanted=true,musicRequest,musicLoop;
let musicTrack=null;
let scoreRevision=0,scoreSaveFailed=false,formalScores;
let nativeRecorder,nativeRecording,nativeRaw,starting=false,nativeReplayFinished=false,nativePromptWasPaused=false;
function recordNativeCommand(command){
  if(!nativeRecording)return;
  try{
    nativeRecording.consumed();const frame=nativeRecording.frame-1;
    nativeRecorder.stage(frame,{inputs:[command],offlineMask:0,proof:JSON.stringify(nativeRecording.checksum()),hash:(frame+1)%120===0?nativeRecording.checksum():null});nativeRecorder.confirm(frame+1,false);
  }catch(error){nativeRecorder?.finish('原生录像停止：'+error.message);nativeRecording=null;}
}
function finishSoloReplay(reason){
  if(!solo||!nativeRecording||nativeReplayFinished)return false;
  nativeReplayFinished=true;nativeRecording=null;nativeRecorder?.finish(reason);
  const dialog=$('solo-replay-dialog');
  if(dialog){
    nativePromptWasPaused=paused;paused=true;release();accumulator=0;nativeRaw.th_set_pause_overlay(1);
    $('solo-replay-title').textContent=reason==='clear'?'路线完成，保存单人 Replay？':'本局结束，保存单人 Replay？';
    $('solo-replay-summary').textContent=reason==='clear'?'录像覆盖到最终关卡完成，不包含之后的菜单操作。':'录像截止于首次全员阵亡，只包含续关前的操作。';
    dialog.showModal();
  }
  return true;
}
function beforeNativeCommand(name){
  if(solo&&name==='th_continue'&&nativeRecording&&!nativeReplayFinished&&nativeRaw.th_get(0,0)===3){
    finishSoloReplay('game-over');return false;
  }
  return true;
}
function closeSoloReplayPrompt(){
  const dialog=$('solo-replay-dialog');if(!dialog)return;
  if(dialog.open)dialog.close();
  nativeRaw.th_set_pause_overlay(nativePromptWasPaused?1:0);paused=nativePromptWasPaused;release();accumulator=0;updatePhase();present();
}
function restoreFormalScores(){if(formalScores){new Uint8Array(core.memory.buffer,core.th_score_data(),formalScores.length).set(formalScores);core.th_score_load(formalScores.length);}}
function loadScores(){
  try{const saved=localStorage.getItem('th01-native-scores-v1');if(saved){const data=Uint8Array.from(atob(saved),c=>c.charCodeAt(0));
    if(data.length===core.th_score_size()){new Uint8Array(core.memory.buffer,core.th_score_data(),data.length).set(data);core.th_score_load(data.length);}}}
  catch(error){console.warn('成绩记录读取失败',error);}
  scoreRevision=core.th_get(110,0);
  formalScores=new Uint8Array(core.memory.buffer,core.th_score_data(),core.th_score_size()).slice();
}
function saveScores(){
  const revision=core.th_get(110,0);if(revision===scoreRevision)return;
  if(stageSelection(sessionConfig?.startStage).practice){scoreRevision=revision;return;}
  formalScores=new Uint8Array(core.memory.buffer,core.th_score_data(),core.th_score_size()).slice();
  try{const data=formalScores;localStorage.setItem('th01-native-scores-v1',btoa(String.fromCharCode(...data)));scoreSaveFailed=false;}
  catch(error){scoreSaveFailed=true;console.warn('成绩记录未能保存到本机',error);}
  scoreRevision=revision;
}
const musicTracks=new Map();
onBgmVersion(()=>{musicTracks.clear();stopMusicSource();musicTrack=null;if(core&&musicWanted)void startMusic();});
function musicCue(){
  const cue=core?.th_get(79,0)??0;
  return cue===2?'silent':cue<2?musicRoomTracks[cue].key:musicRoomTracks[cue-3].key;
}
function stopMusicSource(){musicRequest=null;musicSource?.stop();musicSource?.disconnect();musicSource=null;musicEnabled=false;}
let sfxEnabled=true,sfxReady=false,soundCursor=0,sfxLoading;
const sfxBuffers=new Map(),sfxSources=new Set();
function stopEffects(){for(const source of sfxSources)source.stop();sfxSources.clear();}
async function unlockEffects(){
  if(!sfxEnabled)return;
  musicContext??=new AudioContext();await musicContext.resume();
  sfxLoading??=Promise.all([1,2,3,4,5,8,9,11,14,15,16,17].map(async id=>{
    const response=await fetch(`sfx/${String(id).padStart(2,'0')}.wav`);
    if(!response.ok)throw Error(`音效 ${id} 读取失败`);
    sfxBuffers.set(id,await musicContext.decodeAudioData(await response.arrayBuffer()));
  })).then(()=>{sfxReady=true;});
  try{await sfxLoading;}catch(error){$('sfx').textContent='音效读取失败';console.error(error);}
}
function playEffects(){
  const end=core.th_get(11,0)>>>0;
  if(soundCursor>end)soundCursor=0;
  soundCursor=Math.max(soundCursor,end-128);
  for(;soundCursor<end;++soundCursor){
    const id=core.th_sound_event(soundCursor),buffer=sfxBuffers.get(id);
    if(!sfxEnabled||!sfxReady||!buffer||paused||musicContext.state!=='running')continue;
    // Original MDRV2 replaces an effect sharing PSG channels. One active
    // effect is retained here, so rapid multi-player shots cannot stack loudly.
    stopEffects();const source=musicContext.createBufferSource();source.buffer=buffer;
    const gain=musicContext.createGain();gain.gain.value=.65;source.connect(gain);gain.connect(musicContext.destination);
    source.onended=()=>sfxSources.delete(source);sfxSources.add(source);source.start();
  }
}
const image=new ImageData(640,400),context=$('game').getContext('2d',{alpha:false});
const phases=['等待开球','游戏中','关卡完成','全员阵亡'];
const players=()=>solo?1:Number($('players').value)||2;

let touchMenuBits=0;
const horizontalTouch=mountHorizontalTouch($('game'),{
  allowed:()=>started&&!configuring&&!document.hidden&&!$('solo-replay-dialog')?.open,
  gameplay:()=>!!core&&!core.th_get(50,0)&&core.th_get(0,0)<=1&&!core.th_get(117,0),
  menu:()=>!!core?.th_get(50,0),
  gesture:()=>{void unlockEffects();void startMusic();},
  change:()=>{const bits=horizontalTouch.menuBits;if(bits&&bits!==touchMenuBits&&core?.th_get(50,0))menuInput({1:0,2:1,4:2,8:3}[bits]);touchMenuBits=bits;},
  pulse:bit=>{if(bit===128){setPaused(!paused);return;}if(core?.th_get(50,0)){menuInput(bit===16?5:4);return;}if(bit===256&&core?.th_get(0,0)===2){core.th_continue();updatePhase();}},
});
mountTouchPreferences($('player').querySelector('.toolbar'),horizontalTouch);
function buttons(){
  const result=keys.map(row=>row.reduce((mask,key,i)=>mask|((held.has(key)||pressed.has(key))?1<<i:0),0));
  for(const {slot,bit} of touchOwners.values()) if(slot<players())result[slot]|=bit;
  const slot=Number($('touchSlot').value);result[slot]=horizontalTouch.pack(result[slot]);return result;
}
function release(){horizontalTouch.reset();touchMenuBits=0;held.clear();pressed.clear();touchOwners.clear();for(const b of document.querySelectorAll('[data-bit]'))b.classList.remove('active');}
function previewBoss(stage=5,route=0){
  if(!core)return;
  core.th_stage_command(14,0,stage,route,0,0);paused=false;core.th_set_pause_overlay(0);
  $('pause').textContent='暂停';release();accumulator=0;updatePhase();void startMusic();
}
function setPaused(value){
  if(!value&&core?.th_get(50,0)&&core.th_get(50,0)!==3)return;
  if(value&&core&&!core.th_get(50,0))core.th_menu_open(3,$('compensation').checked?1:0);
  if(!value&&core?.th_get(50,0)===3)core.th_menu_input(5);
  paused=value;core?.th_set_pause_overlay(value?1:0);if(value)stopEffects();release();accumulator=0;$('pause').textContent=paused?'继续':'暂停';updatePhase();
}
function menuInput(key){
  const action=core.th_menu_input(key);
  saveScores();
  if(action===1){
    $('difficulty').value=String(core.th_get(52,0));$('players').value=String(core.th_get(53,0));$('orbs').value=String(core.th_get(54,0));
    $('compensation').checked=!!core.th_get(55,0);$('god').checked=!!core.th_get(56,0);reset();
  }else if(action===2)setPaused(false);
  else if(action===3||action===4){paused=true;stopEffects();release();}
  updatePhase();present();
}
function updatePhase(){
  if(!core)return;
  const phase=core.th_get(0,0),stage=core.th_get(27,0),overlay=$('overlay');
  const menu=core.th_get(50,0);$('menuControls').hidden=!menu;
  if(solo&&!nativeReplayFinished&&(phase===3||stage===20&&(phase===2||menu>=10&&menu<=13)))finishSoloReplay(phase===3?'game-over':'clear');
  $('saveName').hidden=menu!==12;
  if(menu>=10&&menu<=13){
    overlay.hidden=true;$('phase').textContent=menu===10?'结尾演出':menu===11?'成绩评定':menu===12?'姓名登记':'成绩记录';
    $('launch').hidden=true;$('pause').disabled=true;
    $('scoreStatus').textContent=scoreSaveFailed?'成绩仍在本次会话中，本机保存失败':'方向键选择 · Z 确认／逐段快进 · X 删除／跳过结尾 · Enter 保存姓名';return;
  }
  $('pause').disabled=false;$('scoreStatus').textContent='';
  if(menu){
    overlay.hidden=false;$('phase').textContent='菜单';$('phaseLabel').textContent=menu===9?'CONTINUE':menu===8?'MUSIC ROOM':menu===6?'ROUTE':menu===7?'ROUND CLEAR':menu===3?'PAUSE':'TITLE';
    $('phaseTitle').textContent=menu===9?'是否全员续关？':menu===8?musicRoomTracks[core.th_get(76,0)].title:menu===6?'选择下一条路线':menu===7?'第 20 关调试清关':menu===2?'选项':menu===4?'确认退出':menu===3?'暂停':'东方灵异传';
    $('phaseHint').textContent=menu===9?'先按上选择续关／下选择结束，再按 Z 或 Enter 确认。X 不退出；续关从当前关重开，分数清零。':menu===8?'方向键切换并试听，Z / Enter 播放或停止，X / Esc 返回；音乐开关保持有效':menu===6?'方向键选择魔界／地狱，Z 或 Enter 确认并查看结算':menu===7?'调试命令跳过了 Boss 击破流程；确认返回标题':menu===2?'方向键调整；返回标题后 START 应用设置，CONTINUE 恢复当前局':'方向键选择，Z 或 Enter 确认，X 或 Esc 返回';
    $('launch').hidden=false;$('launch').textContent='确定';return;
  }
  if(core.th_get(117,0)&&!paused){
    overlay.hidden=true;$('phase').textContent='关卡过场';$('launch').hidden=true;return;
  }
  $('phase').textContent=paused?'已暂停':phases[phase];
  overlay.hidden=!paused&&phase===1;
  $('phaseLabel').textContent=paused?'PAUSED':`STAGE ${String(stage).padStart(2,'0')}`;
  $('phaseTitle').textContent=paused?'游戏已暂停':phase===0?'准备好了吗？':phase===2?`第 ${stage} 关完成`:'全员阵亡';
  $('phaseHint').textContent=paused?'点击继续或按 Esc 恢复':phase===0?`按 Z，或由任意玩家射击开球${core.th_get(81,0)?'；本 Boss 关暂用第一关占位':''}`:phase===2?`通关奖励 ${core.th_get(28,0)} 分。${core.th_get(60,0)>=0?'按 Enter 跳过结算动画，再按一次进入下一关':stage===20?'按 Enter 完成本路线':'按 Enter 进入下一关，保留残机和 Bomb'}`:'继续从当前关开始，恢复每人的残机与 Bomb';
  $('launch').hidden=false;$('launch').textContent=paused?'继续':phase===0?'开球':phase===2?(stage===5||stage===20?'继续':'下一关'):'全员续关';
}
function reset(){
  if(!core)return;
  release();accumulator=0;last=0;
  core.th_init(players(),Number($('orbs').value),0x12345678,$('compensation').checked?1:0);
  core.th_set_difficulty(Number($('difficulty').value));
  restoreFormalScores();
  applyStage(core,sessionConfig?.startStage);
  soundCursor=0;stopEffects();
  $('touchSlot').value=String(Math.min(Number($('touchSlot').value),players()-1));
  for(const option of $('touchSlot').options)option.disabled=Number(option.value)>=players();
  core.th_debug_invincible($('god').checked?1:0);setPaused(false);present();updatePhase();
}
function present(){
  const start=performance.now();saveScores();core.th_render();
  image.data.set(new Uint8Array(core.memory.buffer,core.th_pixels(),640*400*4));context.putImageData(image,0,0);
  drawMs+=performance.now()-start;++draws;
}
function loop(now){
  if(configuring){last=now;accumulator=0;requestAnimationFrame(loop);return;}
  if(!last)last=now;
  const elapsed=Math.min(250,now-last);last=now;
  if(!paused&&!document.hidden)accumulator+=elapsed;
  let budget=8;
  while(accumulator>=tick&&budget-->0&&!paused){
    const input=buttons(),start=performance.now();core.th_step(...input);horizontalTouch.consume();pressed.clear();logicMs+=performance.now()-start;
    playEffects();
    accumulator-=tick;++steps;
  }
  if(now-lastDraw>=1000/60-.5){present();lastDraw=now;}
  if(musicWanted&&musicContext&&musicTrack!==musicCue())void startMusic();
  if(now-statsAt>=500){
    $('rate').textContent=`${(steps*1000/(now-statsAt)).toFixed(1)} 步/秒`;
    $('compute').textContent=`逻辑 ${(logicMs/Math.max(1,steps)).toFixed(3)} ms`;
    $('draw').textContent=`绘制 ${(drawMs/Math.max(1,draws)).toFixed(2)} ms`;
    $('memory').textContent=`${(core.memory.buffer.byteLength/1048576).toFixed(1)} MiB`;
    $('hash').textContent=(core.th_checksum()>>>0).toString(16).padStart(8,'0');
    updatePhase();steps=logicMs=drawMs=draws=0;statsAt=now;
  }
  requestAnimationFrame(loop);
}
async function startMusic(){
  if(!musicWanted)return;
  const track=musicCue();
  if(musicTrack===track&&(musicSource||musicRequest||track==='silent'))return;
  stopMusicSource();musicTrack=track;
  const request={};musicRequest=request;
  if(track==='silent'){musicRequest=null;return;}
  try{
    musicContext??=new AudioContext();await musicContext.resume();
    if(musicRequest!==request||!musicWanted||musicCue()!==track)return;
    if(!musicTracks.has(track)){
      const pending=loadMusicTrack(musicContext,track);
      musicTracks.set(track,pending);
      void pending.catch(()=>{if(musicTracks.get(track)===pending)musicTracks.delete(track);});
    }
    const [buffer,loop]=await musicTracks.get(track);
    if(musicRequest!==request||!musicWanted||musicCue()!==track)return;
    musicBuffer=buffer;musicLoop=loop;
    musicSource=musicContext.createBufferSource();musicSource.buffer=musicBuffer;musicSource.loop=true;
    musicSource.loopStart=musicLoop.loopStart;musicSource.loopEnd=musicLoop.loopEnd;
    if(!musicGain){musicGain=musicContext.createGain();musicGain.gain.value=.35;musicGain.connect(musicContext.destination);}
    musicSource.connect(musicGain);musicSource.start();musicEnabled=true;$('music').textContent='音乐：开';
  }catch(error){if(musicRequest===request){if(musicWanted)$('music').textContent='音乐读取失败';console.error(error);}}
  finally{if(musicRequest===request)musicRequest=null;}
}
function toggleMusic(){
  musicWanted=!musicWanted;
  if(musicWanted){$('music').textContent='音乐：开（加载中）';void startMusic();}
  else{stopMusicSource();$('music').textContent='音乐：关';}
}
document.addEventListener('keydown',event=>{
  if(!started||configuring||$('solo-replay-dialog')?.open)return;
  if(!event.repeat){void unlockEffects();void startMusic();}
  if(!event.repeat&&event.code==='F10'&&core){event.preventDefault();previewBoss();return;}
  if(core?.th_get(50,0)&&!event.target.matches('select,input')){
    const key={ArrowUp:0,ArrowDown:1,ArrowLeft:2,ArrowRight:3,KeyZ:4,Enter:core.th_get(50,0)===12?6:4,Space:4,KeyX:5,Escape:5}[event.code];
    if(key!==undefined){event.preventDefault();if(!event.repeat)menuInput(key);return;}
    if(['F1','F4','F8'].includes(event.code)){event.preventDefault();return;}
  }
  if(!event.repeat&&event.code==='F1'){event.preventDefault();core.th_menu_open(1,$('compensation').checked?1:0);setPaused(true);return;}
  if(!event.repeat&&event.code==='F8'){event.preventDefault();$('god').checked=!$('god').checked;$('god').dispatchEvent(new Event('change'));return;}
  if(!event.repeat&&event.code==='F4'){event.preventDefault();$('difficulty').value=String((Number($('difficulty').value)+1)%4);reset();return;}
  if(event.target.matches('select,input'))return;
  if(!event.repeat&&(event.code==='KeyZ'||event.code==='Enter')&&(paused||core?.th_get(0,0)===2)){
    event.preventDefault();if(paused)setPaused(false);else{core.th_continue();core.th_debug_invincible($('god').checked?1:0);release();updatePhase();}return;
  }
  if(keys.some(row=>row.includes(event.code))){event.preventDefault();held.add(event.code);if(!event.repeat)pressed.add(event.code);}
  if(!event.repeat&&(event.code==='Escape'||event.code==='Space')){event.preventDefault();setPaused(!paused);}
  if(!event.repeat&&event.code==='Enter'&&core?.th_get(0,0)>=2){core.th_continue();core.th_debug_invincible($('god').checked?1:0);updatePhase();}
});
document.addEventListener('keyup',event=>held.delete(event.code));
window.addEventListener('blur',()=>{if(core&&started&&!configuring)setPaused(true);});
document.addEventListener('visibilitychange',()=>{if(document.hidden&&core&&started&&!configuring)setPaused(true);});
for(const button of document.querySelectorAll('[data-bit]')){
  button.addEventListener('pointerdown',event=>{event.preventDefault();if(core?.th_get(50,0)){const key={1:2,2:3,4:4,8:5}[button.dataset.bit];if(key!==undefined)menuInput(key);return;}if(Number(button.dataset.bit)===4&&(paused||core?.th_get(0,0)===2)){if(paused)setPaused(false);else{core.th_continue();updatePhase();}return;}button.setPointerCapture(event.pointerId);touchOwners.set(event.pointerId,{slot:Number($('touchSlot').value),bit:Number(button.dataset.bit)});button.classList.add('active');});
  for(const type of ['pointerup','pointercancel','lostpointercapture'])button.addEventListener(type,event=>{touchOwners.delete(event.pointerId);if(![...touchOwners.values()].some(v=>v.bit===Number(button.dataset.bit)))button.classList.remove('active');});
}
document.addEventListener('pointerdown',()=>{if(started&&!configuring){void unlockEffects();void startMusic();}},{passive:true});
$('sfx').onclick=()=>{sfxEnabled=!sfxEnabled;$('sfx').textContent=`音效：${sfxEnabled?'开':'关'}`;if(sfxEnabled)void unlockEffects();else stopEffects();};
$('restart').onclick=reset;$('pause').onclick=()=>setPaused(!paused);$('music').onclick=toggleMusic;
$('title').onclick=()=>{core.th_menu_open(1,$('compensation').checked?1:0);setPaused(true);};
$('scores').onclick=()=>{core.th_menu_open(13,$('compensation').checked?1:0);paused=false;release();updatePhase();present();};
$('saveName').onclick=()=>menuInput(6);
for(const button of document.querySelectorAll('[data-ending]'))button.onclick=()=>{
  const variant=Number(button.dataset.ending);core.th_stage_command(29,0,variant>>1,variant&1,0,0);
  paused=false;release();updatePhase();present();$('game').focus({preventScroll:true});
};
for(const button of document.querySelectorAll('[data-menu]'))button.onclick=()=>menuInput(Number(button.dataset.menu));
for(const id of ['players','orbs','compensation','difficulty','startStage'])$(id).onchange=setupSummary;
$('god').onchange=()=>core?.th_debug_invincible($('god').checked?1:0);
$('reducedFlash').onchange=()=>{if(core&&!configuring){core.th_set_bomb_reduced_flash($('reducedFlash').checked?1:0);present();}};
$('touchSlot').onchange=release;
$('touchToggle').onclick=()=>$('touch').classList.toggle('visible');
$('hit').onclick=()=>core?.th_debug_hit(0,0);
$('ghost').onclick=()=>{if(players()>1)core?.th_debug_hit(1,1);};
$('fullscreen').onclick=async()=>{try{if(document.fullscreenElement)await document.exitFullscreen();else await $('stage').requestFullscreen();}catch(error){console.error(error);}};
$('launch').onclick=()=>{if(!core)return;if(core.th_get(50,0))menuInput(4);else if(paused)setPaused(false);else if(core.th_get(0,0)>=2){core.th_continue();core.th_debug_invincible($('god').checked?1:0);}else core.th_step(4,0,0);updatePhase();$('game').focus({preventScroll:true});};
if($('solo-replay-dialog')){
  $('solo-replay-save').onclick=async()=>{
    const button=$('solo-replay-save');button.disabled=true;
    try{await nativeRecorder.exportSoloReplay();$('scoreStatus').textContent='Replay 已保存；续关后的操作不会进入该文件。';closeSoloReplayPrompt();}
    catch(error){$('solo-replay-summary').textContent=`Replay 保存失败：${error.message}。可以重试保存或丢弃。`;}
    finally{button.disabled=false;}
  };
  $('solo-replay-discard').onclick=()=>{closeSoloReplayPrompt();$('scoreStatus').textContent='Replay 已丢弃；续关后的操作不会被记录。';};
  $('solo-replay-dialog').addEventListener('cancel',event=>event.preventDefault());
}
$('bossPreview').onclick=()=>{previewBoss();$('game').focus({preventScroll:true});};
$('mimaPreview').onclick=()=>{previewBoss(10,1);$('game').focus({preventScroll:true});};
$('sarielPreview').onclick=()=>{previewBoss(20,0);$('game').focus({preventScroll:true});};
$('konngaraPreview').onclick=()=>{previewBoss(20,1);$('game').focus({preventScroll:true});};
$('elisPreview').onclick=()=>{previewBoss(15,0);$('game').focus({preventScroll:true});};
$('kikuriPreview').onclick=()=>{previewBoss(15,1);$('game').focus({preventScroll:true});};
$('yuugenPreview').onclick=()=>{previewBoss(10,0);$('game').focus({preventScroll:true});};
if(matchMedia('(pointer:coarse)').matches)$('touch').classList.add('visible');
async function begin(){
  if(!core||starting)return;starting=true;
    try{
  const saved={};for(const id of ['players','orbs','difficulty','startStage'])saved[id]=$(id).value;
  for(const id of ['compensation','reducedFlash'])saved[id]=$(id).checked;
  sessionConfig=saved;
  if(!nativeRecorder){
      try{
    nativeRecorder=createRecorder(document.querySelector('#player .toolbar'));globalThis.th01LocalReplay=nativeRecorder;
    const settings={players:players(),count:Number(saved.orbs),rank:Number(saved.difficulty),lifeCompensation:!!saved.compensation,startStage:saved.startStage};
    await nativeRecorder.boot({adapter:'native-local/1',settings,players:settings.players,hostSlot:0,tickMs:tick,opening:nativeLocalOpening(nativeRaw)});
    if(!nativeRecorder.error){nativeReplayFinished=false;nativeRecording=new NativeLocalSimulation(nativeRaw,settings,0);}
      }catch(error){nativeRecording=null;try{nativeRecorder?.finish('原生录像未启动：'+error.message);}catch{}console.warn('原生录像未启动',error);}
  }
  try{localStorage.setItem(preferenceKey,JSON.stringify(saved));}catch{}
  started=true;configuring=false;$('setup').hidden=true;$('player').hidden=false;
  document.querySelector('.diagnostics').hidden=false;
  reset();core.th_set_bomb_reduced_flash($('reducedFlash').checked?1:0);
  if(solo&&!stageSelection(saved.startStage).practice){core.th_menu_open(1,$('compensation').checked?1:0);setPaused(true);}
  $('sessionSettings').textContent=$('setupSummary').textContent;
  updatePhase();present();$('game').focus({preventScroll:true});void unlockEffects();void startMusic();
    }finally{starting=false;}
}
$('start').onclick=begin;
$('settings').onclick=()=>{setPaused(true);configuring=true;$('player').hidden=true;document.querySelector('.diagnostics').hidden=true;$('setup').hidden=false;$('cancelSetup').hidden=false;setupSummary();$('start').focus();};
$('cancelSetup').onclick=()=>{
  for(const id of ['players','orbs','difficulty','startStage'])$(id).value=sessionConfig[id];
  for(const id of ['compensation','reducedFlash'])$(id).checked=sessionConfig[id];
  setupSummary();configuring=false;$('setup').hidden=true;$('player').hidden=false;document.querySelector('.diagnostics').hidden=false;setPaused(false);$('game').focus({preventScroll:true});
};
try{
  const response=await fetch('game.wasm',{cache:'no-store'});if(!response.ok)throw Error('缺少 game.wasm，请先运行构建脚本');
  const binary=await response.arrayBuffer();const module=await WebAssembly.compile(binary);
  if(WebAssembly.Module.imports(module).length)throw Error('核心不应依赖外部导入');
  const instance=await WebAssembly.instantiate(module,{});nativeRaw=instance.exports;core=nativeRaw;loadScores();core=localExportFacade(nativeRaw,recordNativeCommand,beforeNativeCommand);
  // Read-only diagnostics and deterministic commands for local QA.
  window.th01Native={core,buttons,reset,pause:setPaused,audio:()=>({ready:sfxReady,enabled:sfxEnabled,buffers:sfxBuffers.size,cursor:soundCursor,musicWanted,musicTrack,musicPlaying:musicEnabled&&musicContext?.state==='running',musicLoop:musicSource?[musicSource.loopStart,musicSource.loopEnd]:null})};
  $('start').disabled=false;$('start').textContent=solo?'开始单人游戏 →':'开始本地合作 →';requestAnimationFrame(loop);
}catch(error){$('setupSummary').textContent=`加载失败：${error.message}`;$('start').textContent='游戏资源加载失败';console.error(error);}
addEventListener('pagehide',()=>{nativeRecorder?.finish('原生本地页面关闭');nativeRecorder?.close();});
