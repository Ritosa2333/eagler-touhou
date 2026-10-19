import createModule from './th075.mjs';
import {createBrowserKeyboard} from './directory-keyboard.mjs';
import {scanCodes} from './keyboard.mjs';
import {attachDisplay} from './display.mjs';
import {parseContentBundle} from './content-bundle.mjs';
import {createSaveSync} from './save-sync.mjs';

const game='th075',protocol='eagler-touhou/1',query=new URLSearchParams(location.search);
const epoch=Number(query.get('runtimeEpoch')),validEpoch=Number.isSafeInteger(epoch)&&epoch>0;
const canvas=document.querySelector('canvas'),$=s=>document.querySelector(s);
const build=/*TH075_EAGLER_BUILD*/{version:'development',completeGame:false};
let net,replayPrompt,showReplaySavePrompt,replayBaseline=new Set();
let core,bundle,launched=false,stopping=false,busy=false,loopId=0,options={},music='none',commands=Promise.resolve(),roomPreview=false;
let escapeSerial=0,stick={x:0,y:0},direct=null,actionButtons=0;
const keyboard=createBrowserKeyboard({accept:code=>!!scanCodes[code],send:(code,down)=>core?._th075_key(scanCodes[code],+down)});
const emit=(event,fields={})=>parent.postMessage({protocol,game,epoch,event,...fields},location.origin);
// The common Launcher fits other games to the available 4:3 player area.
// Keep integer scaling only when an older host explicitly requests it.
const updateDisplay=attachDisplay(canvas,()=>options.displayScaleMode==='integer'?'integer':'fit');
const error=()=>core.UTF8ToString(core._th075_error());
const call=(name,types=[],args=[])=>core.ccall('th075_'+name,'number',types,args,{async:true});
const sync=createSaveSync(()=>core);
function clearKeyboard(){keyboard.clear();core?._th075_keys_clear();}
function cancelTouch(){direct=null;stick={x:0,y:0};actionButtons=0;core?._th075_touch_cancel();}
function inputKey(event,down,source){
  if(replayPrompt)return false;
  if(net?.spectator)return true;
  if(event.code==='KeyR'&&down&&!event.repeat&&core._th075_phase()===2){if(net)core._th075_mp_restart();else core._th075_restart();return true;}
  return keyboard.event(event,down,source);
}
const fatal=e=>{
  if(net){globalThis.__eaglerNetplayFailed=true;globalThis.__eaglerNetplayError=e?.message||String(e);globalThis.__eaglerNetplayLanActive=false;}
  ++loopId;
  const message=e?.message||String(e),inspect=name=>{try{return core?.[name]?.()??null;}catch{return null;}};
  const report={game,version:build.version,time:new Date().toISOString(),browser:navigator.userAgent,
    error:message,stack:String(e?.stack||'').slice(0,32768),scene:inspect('_th075_scene'),frame:inspect('_th075_frame'),
    phase:inspect('_th075_phase'),language:core?.th075Language||'ja',music};
  // Preserve the original failure even if pausing a failed Wasm instance throws.
  globalThis.__th075LastError=report;console.error('TH075 runtime failure',report);
  const panel=$('#error');panel.textContent=message;
  const details=document.createElement('details'),summary=document.createElement('summary'),trace=document.createElement('pre');
  summary.textContent='错误详情 / Error details';trace.textContent=report.stack||message;
  trace.style.cssText='max-height:35vh;overflow:auto;white-space:pre-wrap;text-align:left;font-size:12px;user-select:text';
  details.append(summary,trace);panel.append(details);
  const download=document.createElement('a');download.textContent='下载错误报告 / Download error report';
  download.download='th075-error-'+report.time.replace(/[:.]/g,'-')+'.json';
  download.href=URL.createObjectURL(new Blob([JSON.stringify(report,null,2)],{type:'application/json'}));
  download.style.color='inherit';panel.append(download);
  try{core?._th075_loop_pause(1);}catch{}
  emit('error',{error:message,stack:report.stack});
};
async function idle(){while(busy)await new Promise(resolve=>setTimeout(resolve,5));}
async function save(){await idle();if(launched&&!core._th075_save_scores())throw Error(error());await sync();}
function closeAudio(){const context=core.SDL3?.audioContext,borrowed=parent!==window&&context===parent.__touhouAudioContext;if(borrowed)core.SDL3.audioContext=undefined;try{core._th075_audio_close();}finally{if(borrowed)core.SDL3.audioContext=context;}}
async function stop(){if(stopping)return;stopping=true;++loopId;try{await idle();core._th075_loop_pause(1);clearKeyboard();cancelTouch();if(net&&!net.spectator&&!core._th075_mp_save_replay())throw Error(error()||'Unable to save TH075 Replay');await save();net?.close();net=null;replayPrompt?.dispose();replayPrompt=null;closeAudio();launched=false;emit('exit',{code:0,status:'success'});}finally{stopping=false;}}
async function completeMultiplayer(){
  if(stopping||replayPrompt)return;
  const session=net;if(!session||session.spectator)return stop();
  ++loopId;await idle();if(stopping||net!==session)return;
  core._th075_loop_pause(1);clearKeyboard();cancelTouch();
  if(!core._th075_mp_save_replay())throw Error(error()||'Unable to save TH075 Replay');
  await save();if(stopping||net!==session)return;
  session.close();net=null;
  replayPrompt=showReplaySavePrompt({core,baseline:replayBaseline,onReturn:()=>void stop().catch(fatal)});
}

export function userPath(value){
  if(typeof value!=='string'||value.includes('\\'))throw Error('Invalid save path');
  const path=value.replace(/^\/savesth075(?:mp)?\//,'').replace(/^\//,'');
  if(!/^(?:score\.dat|config\.dat|replay\/[a-zA-Z0-9_(). -]+\.rep)$/.test(path)||path.includes('..')||path.length>240)throw Error('Invalid save path');
  return path;
}
async function resource(r){
  if(!r||!/^\/music\/[a-zA-Z0-9_-]+\.ogg$/.test(r.path))throw Error('Invalid music resource path');
  const url=new URL(r.url,location.href);
  if(url.origin!==location.origin||!['http:','https:','blob:'].includes(url.protocol))throw Error('Invalid resource origin');
  const response=await fetch(url);if(!response.ok)throw Error('Music resource HTTP '+response.status);
  const bytes=new Uint8Array(await response.arrayBuffer()),expected=r.bytes??r.size;
  if(!bytes.length||bytes.length>64*1024*1024||(expected!=null&&bytes.length!==expected))throw Error('Invalid music resource size');
  core.FS.mkdirTree('/music');core.FS.writeFile(r.path,bytes,{canOwn:true});
  core.resourceFiles.add(r.path.slice(1));emit('transfer',{mode:'ogg',path:r.path,loaded:bytes.length,total:bytes.length});
}
function applyTouch(){
  const bits=options.touchEnabled?actionButtons:0;
  core._th075_touch_controls(+!!options.touchEnabled,bits&1,bits&2,bits&4,bits&8,0);
  let x=stick.x,y=stick.y;
  if(direct){const scale=Math.min(3,Math.max(1,Number(options.touchSensitivity||100)/100));x=Math.max(-32767,Math.min(32767,(direct.x-direct.startX)*32767*scale/.08));y=Math.max(-32767,Math.min(32767,(direct.y-direct.startY)*32767*scale/.08));}
  core._th075_touch_stick(x,y);
}
function touch(m){
  const sensitivity=(m.controls||m).touchSensitivity;
  if(sensitivity!=null&&!(Number.isFinite(sensitivity)&&sensitivity>=100&&sensitivity<=300))throw Error('Invalid touch sensitivity');
  const t=m.controls||m;actionButtons=Number.isInteger(t.actionButtons)?t.actionButtons&15:0;
  stick={x:Number(t.joystickX)||0,y:Number(t.joystickY)||0};
  if(Number.isFinite(t.touchSensitivity))options.touchSensitivity=Math.max(100,Math.min(300,t.touchSensitivity));
  const serial=t.escapeSerial>>>0;
  if(options.touchEnabled&&serial!==escapeSerial){escapeSerial=serial;keyboard.event({code:'Escape'},true,'touch-pause');setTimeout(()=>keyboard.event({code:'Escape'},false,'touch-pause'),80);}
  applyTouch();
}
function directTouch(m){
  if(m.type==='cancel'){cancelTouch();return;}
  if(!options.touchEnabled||!['down','move','up'].includes(m.type)||![m.id,m.x,m.y].every(Number.isFinite))return;
  if(m.type==='down'&&!direct)direct={id:m.id,startX:m.x,startY:m.y,x:m.x,y:m.y};
  else if(direct?.id===m.id){if(m.type==='up')direct=null;else{direct.x=m.x;direct.y=m.y;}}
  applyTouch();
}
function loop(){
  const id=++loopId;let first=true,last=performance.now(),presents=0,ticks=core._th075_frame(),maximum=0;
  const frame=async time=>{
    if((!launched&&!roomPreview)||id!==loopId)return;
    if(document.hidden){requestAnimationFrame(frame);return;}
    busy=true;const start=performance.now();
    try{
      if(!await call('tick',['number'],[time])){if(core._th075_phase()===4){busy=false;await stop();return;}throw Error(error());}
      // The C++ tick commits the shared renderer before returning. Do not use ready as presentation proof.
      if(first){first=false;emit('first-frame');}net?.frame();
      presents++;maximum=Math.max(maximum,performance.now()-start);
      const now=performance.now();
      if(now-last>=1000){const current=core._th075_frame(),pointer=core._th075_audio_statistics();
        if(pointer){const a=core.HEAPU32.subarray(pointer>>>2,(pointer>>>2)+5);emit('audio-health',{ready:!!a[0],queuedFrames:a[1],queuedMs:a[1]*1000/44100,track:a[2],suspended:!!a[3],musicEnabled:!!a[4],backend:'SDL3'});}
        emit('frame-health',{fps:presents*1000/(now-last),simulationFps:(current-ticks)*1000/(now-last),maxGapMs:maximum});last=now;ticks=current;presents=0;maximum=0;}
    }catch(e){fatal(e);return;}finally{busy=false;}
    requestAnimationFrame(frame);
  };requestAnimationFrame(frame);
}

// Share one input owner across the same-origin fast path and postMessage fallback.
function dispatchInput(m){if(net?.spectator&&['keyboard','keyboard-clear','touch-controls','touch-cancel','direct-touch'].includes(m.command))return true;switch(m.command){
  case 'keyboard':if(launched&&!document.hidden&&!stopping)inputKey(m,!!m.down,'hosted');return true;
  case 'keyboard-clear':clearKeyboard();return true;
  case 'touch-controls':if(launched&&!document.hidden&&!stopping)touch(m);return true;
  case 'touch-cancel':cancelTouch();return true;
  case 'direct-touch':if(launched&&!document.hidden&&!stopping)directTouch(m);return true;
  default:return false;
}}
if(validEpoch)Object.defineProperty(window,'__eaglerDirectInputBridge',{configurable:true,value:Object.freeze({
  schema:'eagler-touhou/direct-input/1',protocol,game,epoch,origin:location.origin,
  submit(m){
    if(!core||!launched||stopping||!m||m.protocol!==protocol||m.game!==game||m.epoch!==epoch||m.request!=null)return false;
    return dispatchInput(m);
  },
})});
async function command(m){if(dispatchInput(m))return {};switch(m.command){
  case 'configure':
    if(launched)throw Error('Cannot configure a running game');
    if(!['ogg','none'].includes(m.music))throw Error('Unsupported TH075 music mode');
    if(m.runtimePack){
      const pack=m.runtimePack,manifest=pack.manifest;
      if(!manifest||manifest.schema!=='eagler-touhou/thcrap-static-pack/1'||manifest.game!==game||manifest.language!==pack.language||!/^lang_[a-z0-9]+(?:-[a-z0-9]+)*$/i.test(pack.language)||!Array.isArray(pack.files)||pack.files.length>256||manifest.files.length!==pack.files.length)throw Error('Invalid TH075 language pack');
      const expected=new Map(manifest.files.map(f=>[f.path,f.bytes])),staged=new Map();
      for(const f of pack.files){
        if(!/^\/thcrap\/th075\/(?:(?:images|text)\/(?:data\/(?:system|character)\/[a-zA-Z0-9_/]+\.(?:dat|sce)|musicroom\.dat)\.bin|fonts\/(?:localized-glyphs\.bin|LICENSE\.txt)|localization\/options\.json)$/.test(f.path)||f.path.includes('..')||staged.has(f.path))throw Error('Unsupported TH075 language path');
        const bytes=new Uint8Array(f.bytes);if(!bytes.length||bytes.length>128*1024*1024||expected.get(f.path)!==bytes.length)throw Error('Invalid TH075 language file size');staged.set(f.path,bytes);
      }
      if(!staged.has('/thcrap/th075/fonts/localized-glyphs.bin')||!staged.has('/thcrap/th075/localization/options.json'))throw Error('Incomplete TH075 language pack');
      for(const [path,bytes]of staged){core.FS.mkdirTree(path.slice(0,path.lastIndexOf('/')));core.FS.writeFile(path,bytes);}
      core.th075Language=pack.language;
    }else{if(m.language&&m.language!=='ja')throw Error('Missing selected TH075 language pack');core.th075Language=null;}
    options={...(m.options||{})};music=m.music;core.gameLanguage='ja';
    for(const r of [...(m.sharedResources||[]),...(m.runtimeResources||[]),...(m.resources||[])])await resource(r);
    core._th075_music_enabled(+(music==='ogg'));cancelTouch();clearKeyboard();updateDisplay();return {};
  case 'resources':for(const r of m.resources||[])await resource(r);return {};
  case 'prepare-room':
    if(!build.multiplayer||launched)throw Error('Invalid room preview owner');
    if(!roomPreview){
      if(!await call('initialize',['number'],[options.renderScale===1?1:2])||!await call('room_preview',['number'],[1]))throw Error(error());
      roomPreview=true;$('#loading').textContent='';core._th075_loop_pause(0);loop();
    }
    {const deadline=performance.now()+60000;while(core._th075_scene()!==2){if(stopping||performance.now()>deadline)throw Error('Native title preview timeout');await new Promise(requestAnimationFrame);}}return {presented:true};
  case 'launch':
    if(launched)return {};
    if(build.multiplayer&&options.netplayMode!=='lan'&&!options.replayViewer)throw Error('Launch TH075 multiplayer from its shared room');
    if(roomPreview){++loopId;core._th075_loop_pause(1);while(busy)await new Promise(requestAnimationFrame);if(!await call('room_preview',['number'],[0]))throw Error(error());roomPreview=false;}
    core._th075_music_enabled(+(music==='ogg'));
    if(!await call('initialize',['number'],[options.renderScale===1?1:2]))throw Error(error());
    if(build.multiplayer&&options.netplayMode==='lan'){const multiplayer=await import('./multiplayer.mjs');showReplaySavePrompt=multiplayer.showReplaySavePrompt;replayBaseline=new Set(core.FS.readdir('/save/replay'));net=new multiplayer.Multiplayer({core,options,build,emit,onError:fatal,onFinished:()=>void completeMultiplayer().catch(fatal),isBusy:()=>busy});await net.connect();}
    else if(build.multiplayer&&options.replayViewer){if(!await call('mp_replay_viewer'))throw Error(error());}
    launched=true;$('#loading').textContent='';updateDisplay();core._th075_loop_pause(0);
    emit('runtime-info',{renderer:'SDL3 / WebGL2 / C++',...build});loop();return {};
  case 'sync':await save();return {};
  case 'exit':await stop();return {};
  case 'list':{const files=[];for(const directory of ['','/replay'])for(const name of core.FS.readdir('/save'+directory)){const path=(directory+'/'+name).slice(1);try{userPath(path);}catch{continue;}const stat=core.FS.stat('/save/'+path);if(core.FS.isFile(stat.mode))files.push({path,size:stat.size});}return {files};}
  case 'read':return {bytes:Array.from(core.FS.readFile('/save/'+userPath(m.path)))};
  case 'write':{
    if(launched)throw Error('Exit the game before importing saves');
    const path=userPath(m.path),bytes=new Uint8Array(m.bytes||[]);
    if(!bytes.length||bytes.length>64*1024*1024)throw Error('Invalid user file size');
    const pointer=core._malloc(bytes.length);if(!pointer)throw Error('File allocation failed');
    let valid;try{core.HEAPU8.set(bytes,pointer);valid=core._th075_validate_file(path==='score.dat'?0:path==='config.dat'?2:1,pointer,bytes.length);}finally{core._free(pointer);}
    if(!valid)throw Error('Invalid TH075 save/config/Replay');
    core.FS.writeFile('/save/'+path,bytes);await sync();return {};
  }
  case 'remove':if(launched)throw Error('Exit the game before removing saves');core.FS.unlink('/save/'+userPath(m.path));await sync();return {};
  default:throw Error('Unsupported Runtime command');
}}
window.addEventListener('message',event=>{
  const m=event.data;
  if(event.source!==parent||event.origin!==location.origin||m?.protocol!==protocol||m.game!==game||!validEpoch||m.epoch!==epoch||typeof m.command!=='string')return;
  commands=commands.then(async()=>{if(await initialized===false)return;try{const result=await command(m);if(typeof m.request==='string')emit(undefined,{request:m.request,ok:true,...result});}catch(e){if(typeof m.request==='string')emit(undefined,{request:m.request,ok:false,error:String(e),errno:e?.errno});else fatal(e);}}).catch(fatal);
});
for(const type of ['keydown','keyup'])window.addEventListener(type,event=>{if(!launched||event.altKey||event.metaKey||document.hidden)return;if(inputKey(event,type==='keydown','native')){event.preventDefault();event.stopPropagation();}},{capture:true});
window.addEventListener('blur',()=>{clearKeyboard();cancelTouch();});
document.addEventListener('visibilitychange',()=>{clearKeyboard();cancelTouch();if(launched){core._th075_loop_pause(+document.hidden);if(document.hidden)void save().catch(fatal);}});
window.addEventListener('pagehide',()=>{++loopId;clearKeyboard();cancelTouch();core?._th075_loop_pause(1);if(core)void save().catch(()=>{});});
for(const type of ['pointerdown','keydown'])window.addEventListener(type,()=>core?.SDL3?.audioContext?.resume().catch(()=>{}),{capture:true});
canvas.addEventListener('webglcontextlost',event=>{event.preventDefault();fatal(Error('WebGL context lost; restart the game.'));});
setInterval(()=>{if(launched&&!busy&&!document.hidden)void save().catch(fatal);},30000);
const initialized=(async()=>{
  if(!validEpoch||query.get('managedData')!=='1'||parent===window||typeof parent.__eaglerPrepareManagedRuntimeDataV1!=='function')throw Error('Launch TH075 through eagler-touhou');
  const {buffer}=await parent.__eaglerPrepareManagedRuntimeDataV1({game,generation:query.get('gameGeneration'),epoch});
  bundle=parseContentBundle(buffer);
  core=await createModule({canvas,resourceFiles:new Set([...bundle.paths.map(path=>path.slice(1))]),
    async readAsset(path){if(path.startsWith('/music/')&&path.endsWith('.ogg')){try{return core.FS.readFile(path).buffer;}catch{throw Error('Music must be installed through Package Store');}}return (await bundle.read(path)).buffer;},
    printErr:console.error,onAssetLoading(path,on){$('#loading').textContent=on?'正在准备场景…':'';}});
  core.th075ContentIdentity=bundle.identity;
  core.SDL3=core.SDL3||{};if(parent.__touhouAudioContext)core.SDL3.audioContext=parent.__touhouAudioContext;
  window.Module=core;window.FS=core.FS;core.resetBrowserKeyboard=()=>keyboard.clear();
  const saveRoot=build.multiplayer?'/savesth075mp':'/savesth075';
  core.FS.mkdirTree(saveRoot);core.FS.mount(core.IDBFS,{},saveRoot);await sync(true);
  core.FS.mkdirTree(saveRoot+'/replay');core.FS.symlink(saveRoot,'/save');
  for(const path of ['/fonts/original-glyphs.bin','/music/catalog.bin']){core.FS.mkdirTree(path.slice(0,path.lastIndexOf('/')));core.FS.writeFile(path,await bundle.read(path));}
  emit('ready',{roomPreview:build.multiplayer===true});
})().catch(e=>{if(e?.name==='AbortError'&&e?.message==='EAGLER_RUNTIME_SESSION_SUPERSEDED')return false;fatal(e);throw e;});
