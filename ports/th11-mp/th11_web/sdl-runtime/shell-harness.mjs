import createModule from '/th11-harness.mjs';
import {FrameClock,keyboardBits} from './frame-clock.mjs';
const $=s=>document.querySelector(s),canvas=$('#canvas'),status=$('#status'),progress=$('#progress');
const keys=new Set(),touchKeys=new Map(),clock=new FrameClock();
let core=null,running=false,touch=matchMedia('(any-pointer:coarse)').matches,paused=false,frames=0,healthAt=0,dragPointer=null,dragOrigin=null;
function coreError(){const p=core._th11_error(),end=core.HEAPU8.indexOf(0,p);return new TextDecoder().decode(core.HEAPU8.subarray(p,end<0?p+256:end));}
function fail(e){running=false;$('#error').hidden=false;$('#error').textContent='游戏运行失败：'+(e?.message||e);console.error(e);}
function clearInput(){keys.clear();touchKeys.clear();dragPointer=null;dragOrigin=null;}
function syncPause(){paused=core?._th11_phase()===2;$('#pause-panel').hidden=!paused;clock.reset(performance.now());frames=0;healthAt=performance.now();}
function setPaused(value){
 if(!core||!running)return;
 const phase=core._th11_phase();
 if(value&&phase===1)core._th11_pause();
 else if(!value&&phase===2)core._th11_resume();
 clearInput();syncPause();
}
async function prepare(){
 core=await createModule({canvas,printErr:console.error});
 const response=await fetch('/th11.dat');if(!response.ok)throw Error('th11.dat 下载失败 ('+response.status+')');
 const bytes=new Uint8Array(await response.arrayBuffer());progress.hidden=false;progress.max=bytes.length;progress.value=bytes.length;status.textContent='加载资源 '+(bytes.length/1048576).toFixed(1)+' MB';
 core.FS.writeFile('/th11.dat',bytes,{canOwn:true});core.FS.mkdirTree('/music');
 const list=await fetch('/music-index.json').then(r=>{if(!r.ok)throw Error('音乐清单加载失败');return r.json();});
 await Promise.all(list.map(async entry=>{if(!/^[a-z0-9_]+\.ogg$/.test(entry.file))throw Error('音乐文件名无效');const r=await fetch('/music/'+entry.file);if(!r.ok)throw Error('音乐加载失败');core.FS.writeFile('/music/'+entry.file,new Uint8Array(await r.arrayBuffer()));}));
 core.FS.mkdirTree('/fonts');const fonts=await fetch('/fonts-index.json').then(r=>r.json());
 for(const entry of fonts.files){if(!/^[a-z0-9_]+\.bin$/.test(entry.file))throw Error('字形文件名无效');const r=await fetch('/fonts/'+entry.file);if(!r.ok)throw Error('字形加载失败');core.FS.writeFile('/fonts/'+entry.file,new Uint8Array(await r.arrayBuffer()));}
 if(!core._th11_initialize())throw Error(coreError());
}
function heldBits(){const all=new Set(keys);for(const values of touchKeys.values())for(const code of values)all.add(code);return keyboardBits(all);}
function frame(now){
 if(!running)return;
 if(paused)clock.reset(now);
 else for(let i=0,n=clock.advance(now);i<n;++i){if(!core._th11_tick(heldBits())){fail(Error(coreError()));return;}++frames;}
 if(now-healthAt>=500){$('#health').textContent=paused?'已暂停':(frames*1000/(now-healthAt)).toFixed(0)+' FPS';frames=0;healthAt=now;}
 requestAnimationFrame(frame);
}
$('#start').onclick=async()=>{
 try{
  $('#start').disabled=true;$('#error').hidden=true;
  if(!core)await prepare();else if(!core._th11_restart())throw Error(coreError());
  clearInput();$('#welcome').hidden=true;running=true;paused=false;$('#pause-panel').hidden=true;
  const now=performance.now();clock.reset(now);frames=0;healthAt=now;canvas.focus({preventScroll:true});requestAnimationFrame(frame);
 }catch(e){$('#start').disabled=false;fail(e);}
};
const recognized=new Set(['KeyZ','KeyX','KeyC','ShiftLeft','ShiftRight','ControlLeft','ControlRight','ArrowUp','ArrowDown','ArrowLeft','ArrowRight','Enter','NumpadEnter']);
window.addEventListener('keydown',e=>{
 if(!running)return;
 if(e.code==='Escape'){e.preventDefault();if(!e.repeat)setPaused(!paused);return;}
 if(!recognized.has(e.code))return;
 e.preventDefault();if(!paused)keys.add(e.code);
});
window.addEventListener('keyup',e=>keys.delete(e.code));
window.addEventListener('blur',()=>{clearInput();setPaused(true);});
document.addEventListener('visibilitychange',()=>{if(document.hidden){clearInput();setPaused(true);}});
$('#fullscreen').onclick=()=>document.fullscreenElement?document.exitFullscreen():document.documentElement.requestFullscreen().catch(()=>{});
$('#touch').hidden=!touch;
$('#touch-toggle').onclick=()=>{touch=!touch;$('#touch').hidden=!touch;touchKeys.clear();dragPointer=null;};
$('#resume').onclick=()=>{setPaused(false);canvas.focus({preventScroll:true});};
$('#return-title').onclick=()=>{
 if(core?._th11_return_title()!==1){fail(Error(coreError()));return;}
 running=false;paused=false;clearInput();$('#pause-panel').hidden=true;$('#welcome').hidden=false;$('#start').disabled=false;$('#start').textContent='重新开始';
};
for(const [id,code] of [['shoot','KeyZ'],['focus','ShiftLeft'],['bomb','KeyX']]){
 const b=$('#'+id);
 b.onpointerdown=e=>{if(!running||paused)return;e.preventDefault();touchKeys.set(e.pointerId,new Set([code]));b.setPointerCapture(e.pointerId);};
 for(const event of ['pointerup','pointercancel','lostpointercapture'])b.addEventListener(event,e=>touchKeys.delete(e.pointerId));
}
$('#pause').onclick=()=>setPaused(!paused);
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
