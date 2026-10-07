import createCore from './runtime/core.mjs';
import {Renderer} from '/src/renderer.mjs';
import {parseReplay} from '/src/replay.mjs';
import {openStore} from '/src/storage.mjs';
import {installShell} from '/src/shell.mjs';
import {PreviewInput,supportedReplay} from '/src/preview-input.mjs';
import {StageLoader,stageMusic,bossMusic} from '/src/stage-loader.mjs';
import {MusicPlayer} from '/src/music.mjs';
import {readCoreSoundEvents} from '/src/sound-events.mjs';

const $=id=>document.getElementById(id),canvas=$('screen'),inputState=new PreviewInput();
let core,renderer,store,stageLoader,currentStage=1,post=()=>{},ready=false,running=false,paused=false,busy=false,loadingReplay=false,last=0,accumulator=0,generation=0,previous=0,activeReplay=null,replayFrame=0;
let corpus=[],custom=new Map(),renderedFrames=0,healthAt=0,maxGap=0,lastHud=[],log=[],drag=null,hostDrag=null,firstPresented=false,focusLostDuringStart=false,padPauseHeld=false;
const audio={context:null,buffers:new Map(),player:null,musicSerial:-1,enabled:true,index:null,sounds:null,effects:new Map()};
const tickMs=1000/60;

function message(text){$('status').textContent=text;}
function overlay(title,description){$('overlay').hidden=false;$('overlay').querySelector('strong').textContent=title;$('overlay').querySelector('span').textContent=description;}
function updateButtons(){
  $('start').disabled=!ready||busy||loadingReplay;
  $('play-replay').disabled=!ready||busy||loadingReplay||!supportedReplay(corpus.find(f=>f.id===$('replay').value));
  $('import').disabled=!store||busy||loadingReplay;
  $('pause').disabled=!running;
  for(const id of ['character','shot','difficulty','stage','replay','replay-stage'])$(id).disabled=busy||loadingReplay;
}
function stopMusic(){audio.player?.stop();}
function stopEffects(){for(const source of audio.effects.values()){try{source.stop();}catch{}}audio.effects.clear();}
function audioPause(value){if(!audio.context)return;const task=value?audio.context.suspend():audio.context.resume();task.catch(e=>console.warn('音频暂停/恢复失败：'+e.message));}
function clear(){inputState.clear();drag=hostDrag=null;previous=0;}
function cancelTouch(){inputState.cancelTouch();drag=hostDrag=null;}
function fail(e){
  generation++;ready=running=paused=busy=loadingReplay=false;clear();stopMusic();stopEffects();audioPause(true);updateButtons();
  canvas.dataset.sessionState='error';
  message('运行失败：'+e.message);overlay('运行中断',e.message);console.error(e);post({event:'error',message:e.message});
}
async function bytes(url){const r=await fetch(url);if(!r.ok)throw Error(`资源 ${url} 加载失败 (${r.status})`);return new Uint8Array(await r.arrayBuffer());}
async function initializeAudio(){
  if(!audio.enabled)return;
  const Audio=window.AudioContext||window.webkitAudioContext;if(!Audio)throw Error('当前浏览器没有可用音频');
  audio.context??=new Audio();audio.context.resume().catch(e=>console.warn('音频尚未获准播放：'+e.message));
  if(!audio.index){const response=await fetch('/assets/music-index.json');audio.index=response.ok?await response.json():[];}
  audio.player??=new MusicPlayer({context:audio.context,readBytes:file=>bytes('/assets/music/'+file)});
  // Native4221d1/4221e1 load both slots before the stage starts.
  const tracks=[stageMusic[currentStage],bossMusic[currentStage]].map(id=>audio.index.find(t=>t.id===id));
  if(tracks.some(t=>!t))throw Error('本关的道中或 Boss 音乐尚未准备好');
  await audio.player.preload(tracks);
  if(!audio.sounds)audio.sounds=await fetch('/assets/sound-index.json').then(r=>{if(!r.ok)throw Error('音效索引加载失败');return r.json();});
  await Promise.all([...new Set(audio.sounds.map(s=>s.file))].map(async name=>{
    if(audio.buffers.has(name))return;try{const b=await bytes('/assets/'+name);audio.buffers.set(name,await audio.context.decodeAudioData(b.buffer));}catch(e){console.warn(e.message);}
  }));
}
function sound(name,volume=.16){
  if(!audio.enabled||paused||!audio.context)return;const buffer=audio.buffers.get(name);if(!buffer)return;
  const source=audio.context.createBufferSource(),gain=audio.context.createGain();source.buffer=buffer;gain.gain.value=volume;source.connect(gain);gain.connect(audio.context.destination);source.start();
}
function nativeSound(event){
  const old=audio.effects.get(event.id);if(old){try{old.stop();}catch{}audio.effects.delete(event.id);}
  if(event.stop||!audio.enabled||paused||!audio.context)return;
  const record=audio.sounds?.find(s=>s.id===event.id),buffer=audio.buffers.get(record?.file);if(!buffer)return;
  const source=audio.context.createBufferSource(),gain=audio.context.createGain(),pan=audio.context.createStereoPanner();
  source.buffer=buffer;source.loop=!!record.loop;gain.gain.value=Math.pow(10,record.volume/2000);pan.pan.value=Math.max(-1,Math.min(1,event.pan/10000));
  source.connect(gain);gain.connect(pan);pan.connect(audio.context.destination);audio.effects.set(event.id,source);
  source.onended=()=>{if(audio.effects.get(event.id)===source)audio.effects.delete(event.id);};source.start();
}
async function music(token=generation){
  if(token!==generation||!audio.enabled)return;
  if(!audio.index?.length||!audio.player)return;
  const track=core._th12_music_track?audio.index[core._th12_music_track()]:audio.index.find(t=>t.id===stageMusic[currentStage]);
  if(!track)throw Error('当前音乐曲目无效');
  audio.musicSerial=core._th12_music_serial?.()??0;
  return audio.player.play(track,()=>token===generation&&audio.enabled&&(running||busy));
}
function pause(value=!paused){
  if(!running||paused===value)return;paused=value;clear();last=performance.now();accumulator=0;
  if(value)overlay('已暂停','按 Esc 或点击继续');else $('overlay').hidden=true;
  $('pause').textContent=value?'继续':'暂停';audioPause(value);
  if(!value&&audio.enabled&&!audio.player?.source){const token=generation;initializeAudio().then(()=>music(token)).catch(e=>message('音频未就绪：'+e.message));}
}
function finish(reason){
  if(!running)return;running=paused=false;clear();accumulator=0;stopMusic();stopEffects();audioPause(true);updateButtons();$('pause').textContent='暂停';$('fps').textContent='已结束';
  const replayEnded=reason==='replay-input-ended',gameOver=reason==='game-over';
  overlay(replayEnded?'录像输入结束':gameOver?'Game Over':'本关结束','可重新开始或选择另一关');
  message(replayEnded?'本关录像输入已结束':gameOver?'本次试玩结束':'本关试玩已结束');
  const token=generation;Promise.resolve(store?.sync()).then(()=>{if(token===generation)post({event:'exit',code:0,status:reason});}).catch(e=>post({event:'error',message:'存档同步失败：'+e.message}));
}
function hud(){return [...new Float32Array(core.HEAPU8.buffer,core._th12_hud(),core._th12_hud_size())];}
function afterTick(){
  lastHud=hud();const e=lastHud[14]|0;
  const musicSerial=core._th12_music_serial?.()??0;
  if(audio.enabled&&musicSerial!==audio.musicSerial){
    const token=generation;music(token).catch(error=>{if(token===generation)message('音乐切换失败：'+error.message);});
  }
  const events=readCoreSoundEvents(core);
  if(events)events.forEach(nativeSound);
  else {if(e&1)sound('se_tan00.wav',.04);if(e&2)sound('se_enep00.wav');if(e&16)sound('se_graze.wav',.07);if(e&32)sound('se_pldead00.wav',.3);if(e&64)sound('se_item00.wav',.1);}
  if(lastHud[0]%60===0)log.push({frame:lastHud[0],player:{x:lastHud[1],y:lastHud[2]},rng:{seed:lastHud[8],calls:lastHud[9]},enemies:lastHud[10],bullets:lastHud[11],score:lastHud[3]});
  if(lastHud[13])finish(lastHud[13]===2?'game-over':'stage-ended');
}
function present(){
  lastHud=renderer.render(core);canvas.dataset.logicalFrame=String(lastHud[0]);canvas.dataset.sessionState=running?(paused?'paused':'running'):'ended';
  for(const [name,index] of [['playerX',1],['playerY',2],['rngSeed',8],['rngCalls',9],['enemies',10],['bullets',11]])canvas.dataset[name]=String(lastHud[index]);
  $('score').textContent=Math.round(lastHud[3]).toString().padStart(9,'0');
  $('life').textContent='★'.repeat(Math.max(0,Math.min(20,Math.trunc(lastHud[5]))));$('bombs').textContent='★'.repeat(Math.max(0,Math.min(20,Math.trunc(lastHud[6]))));
  $('power').textContent=(lastHud[4]/100).toFixed(2);$('graze').textContent=String(lastHud[7]);
}
function pollPadPause(pad){const held=!!pad?.buttons[9]?.pressed;if(held&&!padPauseHeld)pause();padPauseHeld=held;}
function loop(now,token){
  if(token!==generation||!running)return;
  try{
    const elapsed=Math.max(0,now-last);last=now;maxGap=Math.max(maxGap,elapsed);
    const pad=Array.from(navigator.getGamepads?.()||[]).find(p=>p?.connected);pollPadPause(pad);
    if(!paused){
      accumulator+=elapsed;let budget=0;
      while(running&&accumulator>=tickMs&&budget++<8){
        let input;
        if(activeReplay){
          input=activeReplay.input(currentStage,replayFrame);
          if(!input||input.held===65535){finish('replay-input-ended');break;}
          replayFrame++;
        }else input=inputState.sample(previous,pad);
        core._th12_tick(input.held,input.pressed);previous=input.held;accumulator-=tickMs;afterTick();
      }
    }else accumulator=0;
    present();renderedFrames++;
    if(!firstPresented){firstPresented=true;requestAnimationFrame(()=>{if(token===generation)post({event:'first-frame'});});}
    if(now-healthAt>1000&&running){
      const fps=renderedFrames*1000/(now-healthAt);$('fps').textContent=paused?'已暂停':fps.toFixed(0)+' FPS';
      post({event:'frame-health',fps,maxGapMs:maxGap});post({event:'audio-health',backend:audio.enabled?'web-audio':'none',robust:false});renderedFrames=0;maxGap=0;healthAt=now;
    }
  }catch(e){fail(e);return;}
  if(running)requestAnimationFrame(t=>loop(t,token));
}
async function start(replay=null){
  if(!ready)throw Error('资源尚未就绪');if(busy)throw Error('正在准备开局');
  if(replay&&!supportedReplay(replay,Number($('replay-stage').value)))throw Error('录像中没有选择的关卡');
  busy=true;updateButtons();message('正在准备音乐与开局…');
  try{
    running=paused=false;updateButtons();canvas.dataset.sessionState='starting';generation++;const token=generation;clear();inputState.clear({resetSerials:true});activeReplay=replay;replayFrame=0;firstPresented=false;focusLostDuringStart=document.hidden;padPauseHeld=false;log=[];stopMusic();stopEffects();
    currentStage=Number($(replay?'replay-stage':'stage').value);
    const c=replay?.character??Number($('character').value),s=replay?.shot??Number($('shot').value),d=replay?.difficulty??(currentStage===7?4:Number($('difficulty').value)),stage=replay?.stages.find(s=>s.number===currentStage);
    await renderer.prepareStage(currentStage);
    if(!await stageLoader.load(currentStage,()=>token===generation))return;
    core._th12_start(c,s,d,stage?.seed??Math.floor(Math.random()*65536));
    if(stage){const i=stage.initial;core._th12_set_initial(i.x,i.y,i.power,i.lives,i.bombs,i.score);core._th12_set_replay_economy(i.pointValue,i.lifeFragments,i.bombFragments,...i.ufoColors,i.rank);}
    audio.enabled=$('music').checked;
    try{await initializeAudio();await music(token);}catch(e){stopMusic();console.warn('音频未就绪：'+e.message);post({event:'notice',message:'音频未就绪，可关闭声音后继续试玩'});}
    if(token!==generation)return;
    running=true;paused=false;$('pause').textContent='暂停';$('overlay').hidden=true;
    const now=performance.now();last=healthAt=now;renderedFrames=maxGap=0;accumulator=tickMs;
    message((replay?'原版录像 · ':'')+(currentStage===7?'Extra':'第 '+currentStage+' 面'));canvas.focus({preventScroll:true});
    if(document.hidden||focusLostDuringStart)pause(true);
    post({event:'runtime-info',architecture:'C++/Wasm development core',renderer:'WebGL2',version:'0.3.0-cpp'});requestAnimationFrame(t=>loop(t,token));
  }finally{busy=false;updateButtons();}
}
function diagnostic(){const counts=new Uint32Array(core.HEAPU8.buffer,core._th12_unsupported(),1024);return {schema:'th12-development-report/1',version:'0.3.0-cpp',scope:'all-stage-preview',stage:currentStage,wholeGameDeterminism:false,replay:activeReplay?{name:activeReplay.name,character:activeReplay.character,shot:activeReplay.shot,difficulty:activeReplay.difficulty}:null,replayInputFrame:replayFrame,session:{running,paused},logicalFrame:lastHud[0],state:lastHud,unimplemented:Object.fromEntries([...counts].map((n,i)=>[i,n]).filter(([,n])=>n)),samples:log};}
function replayChanged(){const f=corpus.find(f=>f.id===$('replay').value),select=$('replay-stage'),prior=Number(select.value);select.replaceChildren();for(const stage of f?.stages??[]){if(!supportedReplay(f,stage.number))continue;const option=document.createElement('option');option.value=stage.number;option.textContent=stage.number===7?'Extra':'第 '+stage.number+' 面';select.append(option);}if(f?.stages.some(s=>s.number===prior))select.value=prior;$('replay-info').textContent=f?`记录者 ${f.name} · ${f.stages.map(s=>s.number===7?'Extra':s.number+'面').join('、')}`:'';updateButtons();}
function refreshReplays(preferred=$('replay').value){
  const select=$('replay');select.innerHTML='';
  for(const f of corpus){const option=document.createElement('option');option.value=f.id;const supported=supportedReplay(f);option.disabled=!supported;option.textContent=`${f.id} · ${['灵梦','魔理沙','早苗'][f.character]}${f.shot?'B':'A'} · ${['E','N','H','L','EX'][f.difficulty]}${supported?'':' · 尚未支持关卡'}`;select.append(option);}
  select.value=corpus.some(f=>f.id===preferred)?preferred:corpus.find(f=>f.id==='demo2.rpy'&&supportedReplay(f))?.id||corpus.find(supportedReplay)?.id||'';replayChanged();
}
$('replay').onchange=replayChanged;
$('stage').onchange=()=>{if($('stage').value==='7')$('difficulty').value='4';else if($('difficulty').value==='4')$('difficulty').value='1';};
$('difficulty').onchange=()=>{if($('difficulty').value==='4')$('stage').value='7';else if($('stage').value==='7')$('stage').value='1';};
$('start').onclick=()=>start().catch(fail);
$('play-replay').onclick=async()=>{
  if(busy||loadingReplay)return;const id=$('replay').value;if(!supportedReplay(corpus.find(f=>f.id===id)))return;
  loadingReplay=true;updateButtons();
  try{const b=custom.get(id)||await bytes('/replays/'+encodeURIComponent(id)),replay=parseReplay(b);loadingReplay=false;await start(replay);}
  catch(e){message('录像播放失败：'+e.message);}finally{loadingReplay=false;updateButtons();}
};
$('pause').onclick=()=>pause();
$('fullscreen').onclick=()=>{const task=document.fullscreenElement?document.exitFullscreen():$('screen').parentElement.requestFullscreen?.();Promise.resolve(task).catch(e=>message(e.message));};
$('music').onchange=()=>{
  audio.enabled=$('music').checked;if(!audio.enabled){stopMusic();stopEffects();}else if(running&&!paused){const token=generation;initializeAudio().then(()=>music(token)).catch(e=>message('音频未就绪：'+e.message));}
};
function key(code,down,source='local'){
  if(code==='Escape'){const keys=source==='host'?inputState.hostKeys:inputState.localKeys;if(down&&!keys.has(code))pause();down?keys.add(code):keys.delete(code);return;}
  if(down&&(!running||paused||activeReplay))return;inputState.key(code,down,source);
}
// Ignore repeated Escape while paused and preserve browser controls outside the game surface.
window.addEventListener('keydown',e=>{
  if(!running||e.target.closest?.('input,select,textarea'))return;
  if(['Escape','ArrowUp','ArrowDown','ArrowLeft','ArrowRight','KeyZ','KeyX','ShiftLeft','ShiftRight'].includes(e.code)){e.preventDefault();if(!e.repeat)key(e.code,true);}
});
window.addEventListener('keyup',e=>key(e.code,false));
function loseFocus(){focusLostDuringStart=true;clear();pause(true);Promise.resolve(store?.sync()).catch(e=>console.warn(e.message));}
window.addEventListener('blur',loseFocus);document.addEventListener('visibilitychange',()=>{if(document.hidden)loseFocus();});window.addEventListener('pagehide',loseFocus);
canvas.addEventListener('webglcontextlost',e=>{e.preventDefault();fail(Error('画面上下文已中断，请重新打开游戏'));});
for(const button of document.querySelectorAll('#touch button')){
  button.onpointerdown=e=>{if(!running||paused||activeReplay)return;e.preventDefault();inputState.touches.set(e.pointerId,Number(button.dataset.bit));button.setPointerCapture(e.pointerId);};
  for(const type of ['pointerup','pointercancel','lostpointercapture'])button.addEventListener(type,e=>inputState.touches.delete(e.pointerId));
}
canvas.onpointerdown=e=>{if(e.pointerType==='mouse'||!running||paused||activeReplay||drag)return;e.preventDefault();drag={id:e.pointerId,x:e.clientX,y:e.clientY};canvas.setPointerCapture(e.pointerId);};
canvas.onpointermove=e=>{if(drag?.id!==e.pointerId)return;const dx=e.clientX-drag.x,dy=e.clientY-drag.y;inputState.touches.set(e.pointerId,(Math.abs(dx)>8?(dx<0?64:128):0)|(Math.abs(dy)>8?(dy<0?16:32):0));};
for(const type of ['pointerup','pointercancel','lostpointercapture'])canvas.addEventListener(type,e=>{inputState.touches.delete(e.pointerId);if(drag?.id===e.pointerId)drag=null;});
$('import').onchange=async()=>{
  try{
    const file=$('import').files[0];if(!file)return;if(file.size>64*1024*1024)throw Error('录像文件过大');
    const b=new Uint8Array(await file.arrayBuffer()),r=parseReplay(b),id='th12_import_'+Date.now()+'_'+crypto.randomUUID()+'.rpy';
    await store.write('/savesth12/replay/'+id,b);custom.set(id,b);corpus.push({...r,data:undefined,id});refreshReplays(id);
    message(supportedReplay(r)?'录像已导入并保存在当前浏览器':'录像已保存；本版尚不支持该难度或关卡');
  }catch(e){message('导入失败：'+e.message);}finally{$('import').value='';}
};
function download(value,name){const a=document.createElement('a'),u=URL.createObjectURL(new Blob([JSON.stringify(value,null,2)],{type:'application/json'}));a.href=u;a.download=name;a.click();setTimeout(()=>URL.revokeObjectURL(u),1000);}
$('report').onclick=()=>{if(core)download(diagnostic(),'th12-development-report.json');};
async function initialize(){
  store=await openStore();const atlases=await fetch('/assets/atlases.json').then(r=>r.json());renderer=new Renderer(canvas,atlases);await renderer.prepare();core=await createCore();
  stageLoader=new StageLoader(core,atlases,file=>bytes('/assets/'+file));await stageLoader.load(1);
  core._th12_start(2,1,1,0);present();canvas.dataset.sessionState='ready';corpus=(await fetch('/replays/index.json').then(r=>r.json())).fixtures;
  for(const path of await store.list()){if(!path.endsWith('.rpy'))continue;try{const b=await store.read(path),r=parseReplay(b),id=path.split('/').at(-1);custom.set(id,b);corpus.push({...r,data:undefined,id});}catch(e){console.warn(e.message);}}
  ready=true;refreshReplays();message('资源已就绪 · 可选择关卡');
  post=installShell({store,running:()=>running||busy,music:on=>{$('music').checked=on;audio.enabled=on;if(!on)stopMusic();},start:()=>start(),
    key:(code,down)=>key(code,down,'host'),keyboardClear:()=>inputState.clearKeyboard(),touchCancel:cancelTouch,
    touch:m=>{const escape=inputState.touch(m,running&&!paused&&!activeReplay);if(escape)pause();},
    pointer:m=>{
      if(m.type==='up'){if(hostDrag?.id===m.id){inputState.touches.delete('host-pointer');hostDrag=null;}return;}
      if(!running||paused||activeReplay)return;
      if(m.type==='down'){if(!hostDrag)hostDrag={id:m.id,x:m.x,y:m.y};}
      else if(hostDrag?.id===m.id){const dx=m.x-hostDrag.x,dy=m.y-hostDrag.y;inputState.touches.set('host-pointer',(Math.abs(dx)>8?(dx<0?64:128):0)|(Math.abs(dy)>8?(dy<0?16:32):0));}
    }});
  window.__th12Preview={state:()=>[...lastHud],report:diagnostic,ready:()=>ready,paused:()=>paused};
}
initialize().catch(fail);
