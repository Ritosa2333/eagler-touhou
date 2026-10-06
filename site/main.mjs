import createCore from './runtime/core.mjs';
import {Renderer} from '/src/renderer.mjs';
import {parseReplay} from '/src/replay.mjs';
import {openStore} from '/src/storage.mjs';
import {installShell} from '/src/shell.mjs';
import {PreviewInput,supportedReplay} from '/src/preview-input.mjs';

const $=id=>document.getElementById(id),canvas=$('screen'),inputState=new PreviewInput();
let core,renderer,store,post=()=>{},ready=false,running=false,paused=false,busy=false,loadingReplay=false,last=0,accumulator=0,generation=0,previous=0,activeReplay=null,replayFrame=0;
let corpus=[],custom=new Map(),renderedFrames=0,healthAt=0,maxGap=0,lastHud=[],log=[],drag=null,hostDrag=null,firstPresented=false,focusLostDuringStart=false,padPauseHeld=false;
const audio={context:null,buffers:new Map(),source:null,enabled:true,index:null,request:0};
const tickMs=1000/60;

function message(text){$('status').textContent=text;}
function overlay(title,description){$('overlay').hidden=false;$('overlay').querySelector('strong').textContent=title;$('overlay').querySelector('span').textContent=description;}
function updateButtons(){
  $('start').disabled=!ready||busy||loadingReplay;
  $('play-replay').disabled=!ready||busy||loadingReplay||!supportedReplay(corpus.find(f=>f.id===$('replay').value));
  $('import').disabled=!store||busy||loadingReplay;
  $('pause').disabled=!running;
}
function stopMusic(){audio.request++;if(audio.source){try{audio.source.stop();}catch{}audio.source=null;}}
function audioPause(value){if(!audio.context)return;const task=value?audio.context.suspend():audio.context.resume();task.catch(e=>console.warn('音频暂停/恢复失败：'+e.message));}
function clear(){inputState.clear();drag=hostDrag=null;previous=0;}
function cancelTouch(){inputState.cancelTouch();drag=hostDrag=null;}
function fail(e){
  generation++;ready=running=paused=busy=loadingReplay=false;clear();stopMusic();audioPause(true);updateButtons();
  canvas.dataset.sessionState='error';
  message('运行失败：'+e.message);overlay('运行中断',e.message);console.error(e);post({event:'error',message:e.message});
}
async function bytes(url){const r=await fetch(url);if(!r.ok)throw Error(`资源 ${url} 加载失败 (${r.status})`);return new Uint8Array(await r.arrayBuffer());}
async function initializeAudio(){
  if(!audio.enabled)return;
  const Audio=window.AudioContext||window.webkitAudioContext;if(!Audio)throw Error('当前浏览器没有可用音频');
  audio.context??=new Audio();audio.context.resume().catch(e=>console.warn('音频尚未获准播放：'+e.message));
  if(!audio.index){const response=await fetch('/assets/music-index.json');audio.index=response.ok?await response.json():[];}
  await Promise.all(['se_tan00.wav','se_enep00.wav','se_graze.wav','se_pldead00.wav','se_item00.wav','se_power0.wav'].map(async name=>{
    if(audio.buffers.has(name))return;try{const b=await bytes('/assets/'+name);audio.buffers.set(name,await audio.context.decodeAudioData(b.buffer));}catch(e){console.warn(e.message);}
  }));
}
function sound(name,volume=.16){
  if(!audio.enabled||paused||!audio.context)return;const buffer=audio.buffers.get(name);if(!buffer)return;
  const source=audio.context.createBufferSource(),gain=audio.context.createGain();source.buffer=buffer;gain.gain.value=volume;source.connect(gain);gain.connect(audio.context.destination);source.start();
}
async function music(token=generation){
  if(token!==generation||!audio.enabled)return;
  stopMusic();const request=audio.request;if(!audio.enabled||!audio.index?.length)return;
  const track=audio.index.find(t=>t.id==='th12_02')||audio.index[0];
  if(!audio.buffers.has(track.id)){const b=await bytes('/assets/music/'+track.file);audio.buffers.set(track.id,await audio.context.decodeAudioData(b.buffer));}
  if(token!==generation||request!==audio.request||!audio.enabled)return;
  const source=audio.context.createBufferSource(),gain=audio.context.createGain();source.buffer=audio.buffers.get(track.id);source.loop=true;source.loopStart=track.loopStart;source.loopEnd=track.loopEnd;source.connect(gain);gain.connect(audio.context.destination);gain.gain.value=.35;source.start();audio.source=source;
}
function pause(value=!paused){
  if(!running||paused===value)return;paused=value;clear();last=performance.now();accumulator=0;
  if(value)overlay('已暂停','按 Esc 或点击继续');else $('overlay').hidden=true;
  $('pause').textContent=value?'继续':'暂停';audioPause(value);
  if(!value&&audio.enabled&&!audio.source){const token=generation;initializeAudio().then(()=>music(token)).catch(e=>message('音频未就绪：'+e.message));}
}
function finish(reason){
  if(!running)return;running=paused=false;clear();accumulator=0;stopMusic();audioPause(true);updateButtons();$('pause').textContent='暂停';$('fps').textContent='已结束';
  const replayEnded=reason==='replay-input-ended',gameOver=reason==='game-over';
  overlay(replayEnded?'第一面录像输入结束':gameOver?'Game Over':'第一面预览结束','可重新开始或选择另一份录像');
  message(replayEnded?'第一面录像输入已结束；行为对照仍待验收':gameOver?'本次试玩结束':'第一面预览已结束；后续关卡尚未接入');
  const token=generation;Promise.resolve(store?.sync()).then(()=>{if(token===generation)post({event:'exit',code:0,status:reason});}).catch(e=>post({event:'error',message:'存档同步失败：'+e.message}));
}
function hud(){return [...new Float32Array(core.HEAPU8.buffer,core._th12_hud(),core._th12_hud_size())];}
function afterTick(){
  lastHud=hud();const e=lastHud[14]|0;
  if(e&1)sound('se_tan00.wav',.04);if(e&2)sound('se_enep00.wav');if(e&16)sound('se_graze.wav',.07);if(e&32)sound('se_pldead00.wav',.3);if(e&64)sound('se_item00.wav',.1);
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
          input=activeReplay.input(1,replayFrame);
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
  if(replay&&!supportedReplay(replay))throw Error('初版仅支持普通难度的第一面，Extra 尚未接入');
  busy=true;updateButtons();message('正在准备音乐与开局…');
  try{
    running=paused=false;updateButtons();canvas.dataset.sessionState='starting';generation++;const token=generation;clear();inputState.clear({resetSerials:true});activeReplay=replay;replayFrame=0;firstPresented=false;focusLostDuringStart=document.hidden;padPauseHeld=false;log=[];stopMusic();
    const c=replay?.character??Number($('character').value),s=replay?.shot??Number($('shot').value),d=replay?.difficulty??Number($('difficulty').value),stage=replay?.stages.find(s=>s.number===1);
    core._th12_start(c,s,d,stage?.seed??Math.floor(Math.random()*65536));
    if(stage){const i=stage.initial;core._th12_set_initial(i.x,i.y,i.power,i.lives,i.bombs,i.score);core._th12_set_replay_economy(i.pointValue,i.lifeFragments,i.bombFragments,...i.ufoColors,i.rank);}
    audio.enabled=$('music').checked;
    try{await initializeAudio();await music(token);}catch(e){stopMusic();console.warn('音频未就绪：'+e.message);post({event:'notice',message:'音频未就绪，可关闭声音后继续试玩'});}
    if(token!==generation)return;
    running=true;paused=false;$('pause').textContent='暂停';$('overlay').hidden=true;
    const now=performance.now();last=healthAt=now;renderedFrames=maxGap=0;accumulator=tickMs;
    message(replay?'原版录像输入预览 · 第一面':'第一面 · 春之凑');canvas.focus({preventScroll:true});
    if(document.hidden||focusLostDuringStart)pause(true);
    post({event:'runtime-info',architecture:'C++/Wasm development core',renderer:'WebGL2',version:'0.1.1'});requestAnimationFrame(t=>loop(t,token));
  }finally{busy=false;updateButtons();}
}
function diagnostic(){const counts=new Uint32Array(core.HEAPU8.buffer,core._th12_unsupported(),1024);return {schema:'th12-development-report/1',version:'0.1.1',scope:'stage-1-prototype',wholeGameDeterminism:false,replay:activeReplay?{name:activeReplay.name,character:activeReplay.character,shot:activeReplay.shot,difficulty:activeReplay.difficulty}:null,replayInputFrame:replayFrame,session:{running,paused},logicalFrame:lastHud[0],state:lastHud,unimplemented:Object.fromEntries([...counts].map((n,i)=>[i,n]).filter(([,n])=>n)),samples:log};}
function replayChanged(){const f=corpus.find(f=>f.id===$('replay').value);$('replay-info').textContent=f?`记录者 ${f.name} · ${f.stages.map(s=>s.number===7?'Extra':s.number+'面').join('、')}${supportedReplay(f)?'':' · 本版尚不支持播放'}`:'';updateButtons();}
function refreshReplays(preferred=$('replay').value){
  const select=$('replay');select.innerHTML='';
  for(const f of corpus){const option=document.createElement('option');option.value=f.id;const supported=supportedReplay(f);option.disabled=!supported;option.textContent=`${f.id} · ${['灵梦','魔理沙','早苗'][f.character]}${f.shot?'B':'A'} · ${['E','N','H','L','EX'][f.difficulty]}${supported?'':' · 尚未支持关卡'}`;select.append(option);}
  select.value=corpus.some(f=>f.id===preferred)?preferred:corpus.find(f=>f.id==='demo2.rpy'&&supportedReplay(f))?.id||corpus.find(supportedReplay)?.id||'';replayChanged();
}
$('replay').onchange=replayChanged;
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
  audio.enabled=$('music').checked;if(!audio.enabled)stopMusic();else if(running&&!paused){const token=generation;initializeAudio().then(()=>music(token)).catch(e=>message('音频未就绪：'+e.message));}
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
  for(const file of ['stage01.ecl','default.ecl']){const b=await bytes('/assets/'+file),p=core._malloc(b.length);try{core.HEAPU8.set(b,p);if(!core._th12_load_ecl(p,b.length))throw Error('关卡脚本格式无效 '+file);}finally{core._free(p);}}
  for(let i=0;i<6;i++){const file=`pl0${Math.floor(i/2)}${i%2?'b':'a'}.sht`,b=await bytes('/assets/'+file),p=core._malloc(b.length);try{core.HEAPU8.set(b,p);if(!core._th12_load_sht(i,p,b.length))throw Error('自机数据格式无效 '+file);}finally{core._free(p);}}
  for(let i=0;i<6;i++){const file=`st01_0${i>>1}${i%2?'b':'a'}.msg`,b=await bytes('/assets/'+file),p=core._malloc(b.length);try{core.HEAPU8.set(b,p);if(!core._th12_load_msg(i,p,b.length))throw Error('对话数据格式无效 '+file);}finally{core._free(p);}}
  for(const [bank,file]of [[0,'bullet'],[1,'enemy'],[2,'stgenm01'],[4,'pl00'],[5,'pl01'],[6,'pl02'],[7,'front'],[8,'text'],[11,'ascii'],[9,'st01logo'],[10,'stage01']]){const b=await bytes('/assets/'+file+'.anm'),p=core._malloc(b.length);try{core.HEAPU8.set(b,p);if(!core._th12_load_anm(bank,p,b.length))throw Error('动画数据格式无效 '+file);}finally{core._free(p);}}
  core._th12_start(2,1,1,0);present();canvas.dataset.sessionState='ready';corpus=(await fetch('/replays/index.json').then(r=>r.json())).fixtures;
  for(const path of await store.list()){if(!path.endsWith('.rpy'))continue;try{const b=await store.read(path),r=parseReplay(b),id=path.split('/').at(-1);custom.set(id,b);corpus.push({...r,data:undefined,id});}catch(e){console.warn(e.message);}}
  ready=true;refreshReplays();message('资源已就绪 · 初版支持第一面');
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
