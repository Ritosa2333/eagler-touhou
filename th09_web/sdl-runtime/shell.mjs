import {createBrowserKeyboard,runtimeKeyboardCode} from './directory-keyboard.mjs';
import createModule from '/probe.mjs';
import {Netplay} from './netplay.mjs';
import {exportReplayName,importReplayName} from './motion-replay.mjs';
import {scanCodes} from './keyboard.mjs';
const $=s=>document.querySelector(s),canvas=$('#canvas'),status=$('#status'),progress=$('#progress');
let netplay=null,core=null,launched=false,mounted=false,exited=false,touch=matchMedia('(any-pointer:coarse)').matches,fire=true,focus=false,bombs=0,escapes=0,storageChain=Promise.resolve(),savedRevision=0,healthTime=0,healthFrame=0,maxFrame=0,dialogWasRunning=false;
const sources=new Map(),keys=new Set(),decoder=new TextDecoder();
const values=(fn,n)=>Array.from(core.HEAP32.subarray(fn()/4,fn()/4+n));
const errorText=()=>{const bytes=core.HEAPU8.subarray(core._th09_error());return decoder.decode(bytes.subarray(0,bytes.indexOf(0)));};
function fatal(reason){if(netplay?.active)netplay.close();core?._th09_loop_pause(1);$('#error').hidden=false;$('#error').textContent='游戏运行失败：'+(reason?.message||String(reason));console.error(reason);}
function sync(populate=false){const run=storageChain.then(()=>new Promise((yes,no)=>core.FS.syncfs(populate,e=>e?no(e):yes())));storageChain=run.catch(()=>{});return run;}
function configureTouch(){if(!core||!launched)return;core._th09_touch_controls(+touch,+fire,+focus,bombs,escapes);$('#touch').hidden=!touch;$('#touch-toggle').textContent=touch?'隐藏触控':'显示触控';updateTouchLabels();}
function updateTouchLabels(){if(!launched)return;const active=values(core._th09_touch_state,4)[0]===1,label=active?(fire?'射击 开':'射击 关'):'确认';if($('#shoot').textContent!==label)$('#shoot').textContent=label;}
function setKeys(source,next){if(next.length)sources.set(source,next);else sources.delete(source);const merged=new Set([...sources.values()].flat());for(const key of keys)if(!merged.has(key))core?._th09_key(scanCodes[key],0);for(const key of merged)if(!keys.has(key))core?._th09_key(scanCodes[key],1);keys.clear();for(const key of merged)keys.add(key);}
const keyboard=createBrowserKeyboard({accept:code=>!!scanCodes[code],send:(code,down)=>setKeys('keyboard:'+code,down?[code]:[])});
function resetKeyboard(){keyboard.clear();sources.clear();keys.clear();focus=false;}
function clearKeys(){resetKeyboard();core?._th09_keys_clear();core?._th09_touch_cancel();configureTouch();}
const audioResume=()=>core?.SDL3?.audioContext?.resume().catch(()=>{});
async function checkedResource(f,cache){
 const url=f.url+'?v='+f.sha256;let last;
 for(let attempt=0;attempt<4;++attempt){
  let cached=false;
  try{
   let response=cache?await cache.match(url).catch(()=>null):null;cached=!!response;
   if(!response){response=await fetch(url,{signal:AbortSignal.timeout(120000)});if(!response.ok){const e=Error(f.url+' 下载失败 ('+response.status+')');e.permanent=response.status>=400&&response.status<500&&![408,429].includes(response.status);throw e;}}
   const bytes=new Uint8Array(await response.arrayBuffer());if(bytes.length!==f.bytes)throw Error(f.url+' 文件大小错误');
   const hash=[...new Uint8Array(await crypto.subtle.digest('SHA-256',bytes))].map(b=>b.toString(16).padStart(2,'0')).join('');if(hash!==f.sha256)throw Error(f.url+' 校验失败');
   if(cache&&!cached)await cache.put(url,new Response(bytes,{headers:{'Content-Type':response.headers.get('Content-Type')||'application/octet-stream'}})).catch(()=>{});
   return bytes;
  }catch(e){last=e;if(cached)await cache?.delete(url).catch(()=>{});if(e.permanent||attempt===3)throw e;status.textContent='网络暂时中断，重试资源 '+(attempt+1)+'/3';await new Promise(resolve=>setTimeout(resolve,500*2**attempt));}
 }
 throw last;
}
async function mountAssets(){
 const response=await fetch('/resources.json');if(!response.ok)throw Error('资源清单读取失败');const manifest=await response.json();let cache=null;try{cache=await caches.open('th09-native-resources-v1');}catch{}
 const total=manifest.reduce((n,f)=>n+f.bytes,0);let loaded=0,cursor=0,failure=null;progress.hidden=false;progress.max=total;progress.value=0;
 // Wait for every downloader to settle before permitting a new start attempt;
 // otherwise an old request can write into the newly initialized filesystem.
 await Promise.all(Array.from({length:3},async()=>{try{while(!failure&&cursor<manifest.length){const f=manifest[cursor++];if(!/^\/(?:th09\.dat|fonts\/[a-z0-9_.-]+|music\/[a-z0-9_.-]+\.ogg)$/.test(f.path)||!/^\/[a-zA-Z0-9_./-]+$/.test(f.url))throw Error('资源路径无效');const bytes=await checkedResource(f,cache);if(failure)return;core.FS.mkdirTree(f.path.slice(0,f.path.lastIndexOf('/'))||'/');core.FS.writeFile(f.path,bytes,{canOwn:true});loaded+=bytes.length;progress.value=loaded;status.textContent='加载资源 '+Math.round(loaded/1048576)+' / '+Math.round(total/1048576)+' MB';}}catch(e){failure??=e;}}));
 if(failure)throw failure;
}

async function prepare(){
 if(!core)core=await createModule({canvas,resetBrowserKeyboard:resetKeyboard,printErr:console.error,onNetworkRequest(){openNetwork();},onNetworkResult(){netplay?.result();},onNetworkInput(frame,keys,moving,x,y){netplay?.input(frame,keys,moving,x,y);},onGameFrame(ok,ms){if(!ok){fatal(Error(errorText()));return;}netplay?.frame();updateTouchLabels();maxFrame=Math.max(maxFrame,ms);const now=performance.now();if(now-healthTime>=1000){const count=values(core._th09_title_status,8)[0];$('#health').textContent=((count-healthFrame)*1000/(now-healthTime)).toFixed(1)+' FPS';healthFrame=count;healthTime=now;maxFrame=0;}const revision=core._th09_storage_revision();if(revision!==savedRevision){savedRevision=revision;void sync().catch(fatal);}if(values(core._th09_title_status,8)[7]===0){core._th09_loop_stop();clearKeys();exited=true;void save().then(()=>{$('#welcome').hidden=false;$('#start').disabled=false;$('#start').textContent='重新开始';status.textContent='已保存。可以关闭此页面。';}).catch(fatal);}}});
 if(!mounted){core.FS.mkdirTree('/savesth09');core.FS.mount(core.IDBFS,{},'/savesth09');core.FS.symlink('/savesth09','/save');mounted=true;}await sync(true);await mountAssets();
 if(!core._th09_game_open(Date.now()&65535))throw Error(errorText());launched=true;netplay=new Netplay(core,{sync,onStatus(message){$('#network-status').textContent=message;},onClose(){configureTouch();}});configureTouch();window.__th09Runtime={core,netplay,sync,save,importFile,listFiles,status:()=>({title:values(core._th09_title_status,8),session:values(core._th09_session_status,8),touch:values(core._th09_touch_state,4)})};
}
async function save(){if(core&&launched){if(!core._th09_save_snapshot())throw Error('保存失败');await sync();}}
async function importFile(name,bytes){if(netplay?.socket)throw Error('联机时请先退出房间再导入');if(!core||!launched||!values(core._th09_title_status,8)[1])throw Error('请先返回游戏标题再导入');if(!(bytes instanceof Uint8Array)||bytes.length>16*1024*1024)throw Error('文件过大');name=importReplayName(name.toLowerCase(),bytes,9);const kind=/\.rpy$/i.test(name)?1:/\.cfg$/i.test(name)?2:/\.dat$/i.test(name)?0:-1;if(kind<0)throw Error('请选择花映塚的 .dat、.rpy 或 .cfg 文件');const pointer=core._th09_import_buffer(bytes.length);core.HEAPU8.set(bytes,pointer);if(!core._th09_import_file(kind,bytes.length))throw Error('文件无效或不是花映塚文件');await sync();return listFiles();}
function listFiles(){if(!core)return [];const paths=[];for(const dir of ['/save','/save/replay']){if(!core.FS.analyzePath(dir).exists)continue;for(const name of core.FS.readdir(dir)){if(!/^(?:score\.dat|th09\.cfg|th9_(?:[0-9]{2}|ud[0-9]{4}|udn[0-9]{3})\.rpy)$/.test(name))continue;const path=dir+'/'+name,stat=core.FS.stat(path);if(core.FS.isFile(stat.mode))paths.push({name,path,size:stat.size});}}return paths;}
function renderFiles(){const root=$('#files');root.replaceChildren();for(const f of listFiles()){const button=document.createElement('button');button.textContent='下载 '+f.name+' · '+(f.size/1024).toFixed(1)+' KB';button.onclick=()=>{const url=URL.createObjectURL(new Blob([core.FS.readFile(f.path)],{type:'application/octet-stream'})),link=document.createElement('a');link.href=url;link.download=exportReplayName(f.name,core.FS.readFile(f.path),9);link.click();setTimeout(()=>URL.revokeObjectURL(url),1000);};root.append(button);}}
$('#start').onclick=async()=>{try{$('#start').disabled=true;$('#error').hidden=true;if(!launched){status.textContent='准备游戏……';await prepare();}if(exited){if(!core._th09_game_restart())throw Error(errorText());exited=false;}audioResume();$('#welcome').hidden=true;healthTime=performance.now();healthFrame=values(core._th09_title_status,8)[0];core._th09_loop_start();canvas.focus({preventScroll:true});}catch(e){$('#start').disabled=false;fatal(e);}};
function openNetwork(){if(!launched)return;const allowed=!!values(core._th09_title_status,8)[1]&&!netplay?.socket;$('#network-create').disabled=$('#network-join').disabled=!allowed;if(!allowed&&!netplay?.socket){$('#network-status').textContent='请先返回游戏标题。';}core._th09_loop_pause(1);clearKeys();if(!$('#network').open)$('#network').showModal();}
$('#network-open').onclick=openNetwork;
$('#network-create').onclick=async()=>{try{await netplay.connect();}catch(e){$('#network-status').textContent=e.message;netplay.close(false);}};
$('#network-join').onclick=async()=>{try{await netplay.connect($('#network-code').value);}catch(e){$('#network-status').textContent=e.message;netplay.close(false);}};
$('#network-leave').onclick=()=>{netplay?.close();$('#network-status').textContent='已退出房间。';};
$('#network-close').onclick=()=>$('#network').close();$('#network').addEventListener('close',()=>{if(launched&&(!netplay?.socket||netplay.active))core._th09_loop_pause(+document.hidden);});
$('#fullscreen').onclick=()=>document.fullscreenElement?document.exitFullscreen():document.documentElement.requestFullscreen().catch(()=>{});
$('#touch-toggle').onclick=()=>{touch=!touch;configureTouch();};
$('#storage-open').onclick=async()=>{if(!launched)return;if(netplay?.socket){$('#network-status').textContent='请先退出房间再管理文件';$('#network').showModal();return;}try{core._th09_loop_pause(1);clearKeys();await save();renderFiles();$('#storage-status').textContent='';$('#storage').showModal();dialogWasRunning=true;}catch(e){fatal(e);}};
$('#storage-close').onclick=()=>$('#storage').close();$('#storage').addEventListener('close',()=>{if(dialogWasRunning){dialogWasRunning=false;core._th09_loop_pause(+document.hidden);}});
$('#import').onchange=async e=>{const file=e.target.files[0];if(!file)return;try{await importFile(file.name,new Uint8Array(await file.arrayBuffer()));renderFiles();$('#storage-status').textContent='导入成功。录像请在 Replay 的 User 分类查看。';}catch(e){$('#storage-status').textContent=e.message;}e.target.value='';};
window.addEventListener('keydown',e=>{audioResume();if(!launched||document.hidden||$('#storage').open||$('#network').open||!scanCodes[runtimeKeyboardCode(e)]||['F5','F11','F12'].includes(runtimeKeyboardCode(e)))return;e.preventDefault();keyboard.event(e,true);});window.addEventListener('keyup',e=>keyboard.event(e,false));window.addEventListener('blur',clearKeys);
function hold(id,codes){const button=$(id);button.addEventListener('pointerdown',e=>{e.preventDefault();audioResume();button.setPointerCapture(e.pointerId);button.classList.add('pressed');setKeys('button:'+e.pointerId,codes);});for(const event of ['pointerup','pointercancel','lostpointercapture'])button.addEventListener(event,e=>{button.classList.remove('pressed');setKeys('button:'+e.pointerId,[]);});}
hold('#charge',['KeyZ']);hold('#skip',['ControlLeft']);
$('#focus').onpointerdown=e=>{e.preventDefault();e.target.setPointerCapture(e.pointerId);focus=true;configureTouch();};for(const event of ['pointerup','pointercancel','lostpointercapture'])$('#focus').addEventListener(event,()=>{focus=false;configureTouch();});
$('#bomb').onpointerdown=e=>{e.preventDefault();const context=values(core._th09_touch_state,4)[0];if(context===1){++bombs;configureTouch();}else core._th09_pulse(2);};
$('#pause').onpointerdown=e=>{e.preventDefault();++escapes;configureTouch();};
$('#shoot').onclick=()=>{if(values(core._th09_touch_state,4)[0]===1){fire=!fire;configureTouch();}else core._th09_pulse(1);};
const surface=$('#stage');for(const [name,type] of [['pointerdown',0],['pointermove',1],['pointerup',2],['pointercancel',2]])surface.addEventListener(name,e=>{if(!touch||!launched||e.target.closest('button')||!$('#welcome').hidden||e.pointerType==='mouse')return;e.preventDefault();audioResume();if(type===0)surface.setPointerCapture(e.pointerId);const rect=canvas.getBoundingClientRect();core._th09_touch(type,e.pointerId,(e.clientX-rect.left)/rect.width,(e.clientY-rect.top)/rect.height);});surface.addEventListener('contextmenu',e=>e.preventDefault());
canvas.addEventListener('webglcontextlost',e=>{e.preventDefault();fatal(Error('图形环境已失效，请刷新页面。'));});
document.addEventListener('visibilitychange',()=>{if(!launched)return;clearKeys();core._th09_loop_pause(+(document.hidden||$('#storage').open||($('#network').open&&!netplay?.active)));if(document.hidden)void save().catch(fatal);});window.addEventListener('pagehide',()=>{clearKeys();netplay?.close();if(launched){core._th09_loop_pause(1);void save().catch(console.error);}});
