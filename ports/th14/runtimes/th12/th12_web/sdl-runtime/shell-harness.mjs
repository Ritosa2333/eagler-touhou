import createModule from '/th11-harness.mjs';
import {FrameClock,keyboardBits} from './frame-clock.mjs';
const $=s=>document.querySelector(s),canvas=$('#canvas'),status=$('#status'),progress=$('#progress');
const keys=new Set(),touchKeys=new Map(),clock=new FrameClock();
const runtimeTitle=Number(document.body.dataset.portTitle)||12;
const scanCodes=runtimeTitle===14?(await import('./keyboard.mjs')).scanCodes:{};
const publishedScans=new Set();
function syncPlatformKeys(){const all=new Set(keys);for(const values of touchKeys.values())for(const code of values)all.add(code);const next=new Set([...all].map(code=>scanCodes[code]).filter(Boolean));for(const scan of publishedScans)if(!next.has(scan))core._th11_key(scan,0);for(const scan of next)if(!publishedScans.has(scan))core._th11_key(scan,1);publishedScans.clear();for(const scan of next)publishedScans.add(scan);}
// Standalone local transition proof uses only volatile module files. It must
// never persist artificial completion records into the playable save store.
const transitionProof=runtimeTitle===14&&document.body.dataset.transitionProof==='1';
let core=null,running=false,touch=matchMedia('(any-pointer:coarse)').matches,paused=false,frames=0,healthAt=0,dragPointer=null,dragOrigin=null;
let saveStore=null,prepared=false;
let proofPausedStage=0;
function saveProgress(){return saveStore?.save().catch(e=>{fail(Error('存档保存失败：'+e.message));})||Promise.resolve();}
// Verification switch, off unless a proof turns it on: the run's policy cannot dodge, so a browser proof of the
// campaign needs the player kept alive while the stage machinery is what is being watched.
let keepInvincible=false;
window.__th12_keep_invincibility=v=>{keepInvincible=!!v;return keepInvincible;};
// A read-only diagnostic hook: the health line cannot say whether a frozen stage frame is a pause, a menu or a
// session that left its stage, and those are three different failures with the same picture.
window.__th12_probe=()=>({running,paused,phase:core?core._th11_phase():null,stage:core?core._th11_stage():null,stageFrame:core?core._th11_stage_frame():null,error:core?coreError():null,held:[...keys],
  title:core&&core._th11_title?[0,1,2,3,4].map(k=>core._th11_title(k)):null,
  input:core&&core._th11_input?[0,1,2,3].map(k=>core._th11_input(k)):null});
function coreError(){const p=core._th11_error(),end=core.HEAPU8.indexOf(0,p);return new TextDecoder().decode(core.HEAPU8.subarray(p,end<0?p+256:end));}
function fail(e){running=false;clearInput();$('#health').textContent='运行已停止';$('#error').hidden=false;$('#error').textContent='游戏运行失败：'+(e?.message||e);console.error(e);}
function clearInput(){keys.clear();touchKeys.clear();dragPointer=null;dragOrigin=null;if(core)for(const scan of publishedScans)core._th11_key(scan,0);publishedScans.clear();}
function syncPause(){paused=core?._th11_phase()===2;$('#pause-panel').hidden=!paused;clock.reset(performance.now());frames=0;healthAt=performance.now();}
function setPaused(value){
 if(!core||!running)return;
 const phase=core._th11_phase();
 if(value&&phase===1)core._th11_pause();
 else if(!value&&phase===2)core._th11_resume();
 clearInput();syncPause();
 if(value)void saveProgress();
}
async function prepare(){
 // Each step names itself in #status before it suspends. The line is not decoration: when a load stops making
 // progress, the page is idle in an await, and #status then says which await it is - the only readout that can
 // distinguish "still downloading the archive" from "engine initialization never returned".
 status.textContent='加载运行时…';
 if(!core)core=await createModule({canvas,printErr:console.error,...(runtimeTitle===14?{resetBrowserKeyboard:()=>clearInput()}:{})});
 status.textContent='下载资源包…';
 const response=await fetch('/th11.dat');if(!response.ok)throw Error('th11.dat 下载失败 ('+response.status+')');
 const bytes=new Uint8Array(await response.arrayBuffer());progress.hidden=false;progress.max=bytes.length;progress.value=bytes.length;status.textContent='加载资源 '+(bytes.length/1048576).toFixed(1)+' MB';
 core.FS.writeFile('/th11.dat',bytes,{canOwn:true});core.FS.mkdirTree('/music');
 status.textContent='读取音乐清单…';
 const list=await fetch('/music-index.json').then(r=>{if(!r.ok)throw Error('音乐清单加载失败');return r.json();});
 let tracks=0;
 await Promise.all(list.map(async entry=>{if(!/^[a-z0-9_]+\.(?:ogg|flac)$/.test(entry.file))throw Error('音乐文件名无效');const r=await fetch('/music/'+entry.file);if(!r.ok)throw Error('音乐加载失败');core.FS.writeFile('/music/'+entry.file,new Uint8Array(await r.arrayBuffer()),{canOwn:true});status.textContent='加载音乐 '+String(++tracks)+'/'+String(list.length);}));
 core.FS.mkdirTree('/fonts');status.textContent='加载字形…';const fonts=await fetch('/fonts-index.json').then(r=>r.json());
 for(const entry of fonts.files){if(!/^[a-z0-9_]+\.bin$/.test(entry.file))throw Error('字形文件名无效');const r=await fetch('/fonts/'+entry.file);if(!r.ok)throw Error('字形加载失败');core.FS.writeFile('/fonts/'+entry.file,new Uint8Array(await r.arrayBuffer()));}
 if(runtimeTitle===14&&!saveStore&&!transitionProof){
  status.textContent='读取本机存档…';
  const {openBrowserSaveStore}=await import('./browser-save-store.mjs');
  saveStore=await openBrowserSaveStore(core,runtimeTitle);
 }
 status.textContent='初始化引擎…';
 if(!core._th11_initialize())throw Error(coreError());
 if(saveStore){saveStore.activate();await saveStore.save();}
 prepared=true;
}
function heldBits(){const all=new Set(keys);for(const values of touchKeys.values())for(const code of values)all.add(code);return keyboardBits(all,runtimeTitle);}
function frame(now){
 if(!running)return;
 try{
 if(paused)clock.reset(now);
 else for(let i=0,n=clock.advance(now);i<n;++i){if(keepInvincible)core._th11_probe_invincibility(600);let ok;if(runtimeTitle===14){syncPlatformKeys();ok=core._th11_probe_platform_tick();}else ok=core._th11_tick(heldBits());if(!ok){fail(Error(coreError()));return;}++frames;}
 if(now-healthAt>=500){$('#health').textContent=(paused?'已暂停':(frames*1000/(now-healthAt)).toFixed(0)+' FPS')+' stage='+core._th11_stage()+' frame='+core._th11_stage_frame()+' heapMB='+(core._th11_heap_bytes()/1048576).toFixed(1)+' allocs='+core._th11_dynamic_allocs();frames=0;healthAt=now;}
 if(transitionProof){
  const stage=core._th11_stage(),stageFrame=core._th11_stage_frame();
  if($('#proof-auto-pause')?.checked&&!paused&&core._th11_phase()===1&&stage>0&&stage!==proofPausedStage&&stageFrame>=300){proofPausedStage=stage;setPaused(true);console.info(`[transition-proof14] pause stage=${stage} frame=${stageFrame}`);}
  const button=$('#proof-complete');button.disabled=![1,2].includes(core._th11_phase())||stage>=7||stageFrame<180;
  if(core._th11_title(0)===10)$('#proof-status').textContent=`记录页键盘检查：序列 ${core._th11_title(5)}/9，等待 ${core._th11_title(6)} 帧。`;
  else if($('#proof-status').textContent.startsWith('记录页键盘检查：'))$('#proof-status').textContent='';
 }
 requestAnimationFrame(frame);
 }catch(e){fail(e);}
}
$('#start').onclick=async()=>{
 try{
  $('#start').disabled=true;$('#error').hidden=true;
  if(!prepared)await prepare();else if(!core._th11_return_title())throw Error(coreError());
  clearInput();$('#welcome').hidden=true;running=true;paused=false;proofPausedStage=0;$('#pause-panel').hidden=true;
  const now=performance.now();clock.reset(now);frames=0;healthAt=now;canvas.focus({preventScroll:true});requestAnimationFrame(frame);
 }catch(e){$('#start').disabled=false;fail(e);}
};
const recognized=new Set(['KeyZ','KeyX','KeyC','ShiftLeft','ShiftRight','ControlLeft','ControlRight','ArrowUp','ArrowDown','ArrowLeft','ArrowRight','Enter','NumpadEnter']);
window.addEventListener('keydown',e=>{
 if(!running)return;
 if(e.code==='Escape'){e.preventDefault();if(!e.repeat)setPaused(!paused);return;}
 if(!(runtimeTitle===14?scanCodes[e.code]:recognized.has(e.code)))return;
 if(transitionProof&&/^Key[A-Y]$/.test(e.code))console.info(`[keyboard14] ${e.code} scan=${scanCodes[e.code]}`);
 e.preventDefault();if(!paused)keys.add(e.code);
});
window.addEventListener('keyup',e=>keys.delete(e.code));
window.addEventListener('blur',()=>{clearInput();setPaused(true);});
document.addEventListener('visibilitychange',()=>{if(document.hidden){clearInput();setPaused(true);}});
window.addEventListener('pagehide',()=>{void saveProgress();});
// Save during play because a browser can terminate a hidden page before its
// final IndexedDB transaction completes. The store serializes all flushes.
setInterval(()=>{if(core&&running&&!document.hidden)void saveProgress();},15000);
$('#fullscreen').onclick=()=>document.fullscreenElement?document.exitFullscreen():document.documentElement.requestFullscreen().catch(()=>{});
$('#touch').hidden=!touch;
$('#touch-toggle').onclick=()=>{touch=!touch;$('#touch').hidden=!touch;touchKeys.clear();dragPointer=null;};
$('#resume').onclick=()=>{setPaused(false);canvas.focus({preventScroll:true});};
$('#return-title').onclick=async()=>{
 if(core?._th11_return_title()!==1){fail(Error(coreError()));return;}
 running=false;paused=false;clearInput();$('#pause-panel').hidden=true;$('#welcome').hidden=false;$('#start').disabled=false;$('#start').textContent='重新开始';
 await saveProgress();
};
for(const [id,code] of [['shoot','KeyZ'],['focus','ShiftLeft'],['bomb','KeyX']]){
 const b=$('#'+id);
 b.onpointerdown=e=>{if(!running||paused)return;e.preventDefault();touchKeys.set(e.pointerId,new Set([code]));b.setPointerCapture(e.pointerId);};
 for(const event of ['pointerup','pointercancel','lostpointercapture'])b.addEventListener(event,e=>touchKeys.delete(e.pointerId));
}
$('#pause').onclick=()=>setPaused(!paused);
if(transitionProof)$('#proof-complete').onclick=()=>{
 if(!core||![1,2].includes(core._th11_phase()))return;
 if(paused)setPaused(false);
 const stage=core._th11_stage(),frame=core._th11_stage_frame();
 if(core._th11_probe_complete()){const message=`人工触发第 ${stage} 关结算（帧 ${frame}），等待真实加载任务与背景过渡。`;$('#proof-status').textContent=message;console.info(`[transition-proof14] complete stage=${stage} frame=${frame}`);}
 else $('#proof-status').textContent='过渡尚未完成，稍后重试。';
 canvas.focus({preventScroll:true});
};
canvas.addEventListener('pointerdown',e=>{
 if(!touch||e.pointerType==='mouse'||!running||paused||dragPointer!==null)return;
 e.preventDefault();dragPointer=e.pointerId;dragOrigin=[e.clientX,e.clientY];canvas.setPointerCapture(e.pointerId);
});
canvas.addEventListener('pointermove',e=>{
 if(e.pointerId!==dragPointer||!dragOrigin)return;e.preventDefault();
 const dx=e.clientX-dragOrigin[0],dy=e.clientY-dragOrigin[1],codes=new Set();
 if(Math.abs(dx)>8)codes.add(dx<0?'ArrowLeft':'ArrowRight');if(Math.abs(dy)>8)codes.add(dy<0?'ArrowUp':'ArrowDown');touchKeys.set(e.pointerId,codes);
});
for(const event of ['pointerup','pointercancel','lostpointercapture'])canvas.addEventListener(event,e=>{touchKeys.delete(e.pointerId);if(e.pointerId===dragPointer){dragPointer=null;dragOrigin=null;}});
