import {createCoopReplayDriver} from './coop-replay.mjs';
import {applyDisplay} from './display.mjs';
import {createSpectator} from './spectator.mjs';
import {createMultiplayer} from './multiplayer.mjs';
import {FrameClock,PresentationClock} from './clock.mjs';
import createModule from './th05.mjs';
import {decodeContent,digest} from './data.mjs';
import {createStore,SAVE_ROOT,userPath,validateUserFile} from './storage.mjs';
import {installManagedShell,protocol} from './managed.mjs';
import {createBrowserKeyboard} from './directory-keyboard.mjs';
import {createDirectInputBridge} from './direct-input-bridge.mjs';
import {createTouchInput,virtualKey} from './input.mjs';
import {hostedPoint,createPointerRouter} from './pointer.mjs';
import {createAudio} from './audio.mjs';
import {createPlayback} from './playback.mjs';
import {decodeMusicNotes} from './music-notes.mjs';
const query=new URLSearchParams(location.search),canvas=document.querySelector('#canvas');
const multiplayer=true;
let netplay,initialized=false,motion=[0,0,0],menuPulse=0;
let module,store,audio,bridge,playback,pointers,launched=false,configured=false,closed=false,options={},language='ja',music='none',raf=0,firstFrame=false,saveTimer=0;
const clock=new FrameClock(),presentation=new PresentationClock();
let lastHealth=0,healthFrames=0,maxGap=0,launchSerial,unloadSync;
const owners=new Map();
const emit=(event,payload={})=>bridge?.emit(event,payload);
function error(value){const message=value?.message??String(value);emit('error',{message,error:message});}
let nativeAborted=false,finishTask;
function fatal(reason){void finish(1,reason);}
function nativeKey(code,down,owner){const key=virtualKey(code);if(!key)return;let keys=owners.get(owner);if(!keys){keys=new Set();owners.set(owner,keys);}const was=[...owners.values()].some(set=>set.has(key));if(down)keys.add(key);else keys.delete(key);const now=[...owners.values()].some(set=>set.has(key));if(was!==now)module?._th05_key(key,+now);}
function keyboardReady(){return launched&&!closed&&!document.hidden&&!(options.replayViewer||options.netplaySpectator);}
const keyboard=createBrowserKeyboard({send:(code,down)=>nativeKey(code,down,'keyboard'),hostContext:{protocol,game:'th05',epoch:Number(query.get('runtimeEpoch')),origin:location.origin,ready:keyboardReady,reset:clearKeyboard}});
const touch=createTouchInput({send:(code,down)=>nativeKey(code,down,'touch'),position:()=>({x:module?._th05_player_x()??224,y:module?._th05_player_y()??376}),options:()=>options,scene:()=>module?._th05_input_scene()??0,move:(x,y,unlimited)=>{if(multiplayer){
 if(options.touchMovementMode?.startsWith('touch'))motion=[(module._th05_player_x()+x-32)*16,(module._th05_player_y()+y-56)*16,2+Number(unlimited)];
 else motion=[x*16,y*16,+unlimited];
 }else module?._th05_touch_motion(x,y,+unlimited);}});
function clearKeyboard(){if(nativeAborted)return;keyboard.clear();owners.delete('keyboard');reconcile();}
function cancelTouch(){if(nativeAborted)return;motion=[0,0,0];touch.cancel();pointers?.cancel();owners.delete('touch');module?._th05_touch_motion(0,0,0);reconcile();}
function reconcile(){module?._th05_clear();for(const keys of owners.values())for(const key of keys)module?._th05_key(key,1);}
function clearAll(){if(nativeAborted)return;motion=[0,0,0];keyboard.clear();touch.cancel();pointers?.cancel();owners.clear();module?._th05_clear();}
function sameOrigin(value){const url=new URL(value,location.href);if(url.origin!==location.origin||!['http:','https:','blob:'].includes(url.protocol))throw Error('Resource must be same-origin');return url;}
function allowedResource(path){
 if(typeof path!=='string'||/[\\\x00-\x1f]/.test(path)||path.split('/').slice(1).some(p=>!p||p==='.'||p==='..'))return false;
 return /^\/(?:music\/[A-Za-z0-9_.-]+\.(?:ogg|opus|json)|data\/(?:chs\.dat|gothic\.ttf)|msgothic\.ttc|unifont\.otf|thcrap\/th05\/chs\.dat)$/.test(path);
}
async function resources(list=[]){
 if(!Array.isArray(list)||list.length>256)throw Error('Invalid resource list');
 const verified=[];
 for(const resource of list){if(!allowedResource(resource?.path))throw Error('Invalid resource path');if(launched&&!resource.path.startsWith('/music/'))throw Error('Runtime resource is immutable after launch');const response=await fetch(sameOrigin(resource.url));if(!response.ok)throw Error('Resource download failed: '+resource.path);const bytes=new Uint8Array(await response.arrayBuffer()),expected=resource.bytes??resource.size;if(bytes.length>64*1024*1024||expected!==undefined&&bytes.length!==expected||resource.sha256&&await digest(bytes)!==resource.sha256.replace(/^sha256-/,''))throw Error('Resource integrity mismatch: '+resource.path);verified.push({path:resource.path,bytes});}
 for(const file of verified){module.FS.mkdirTree(file.path.slice(0,file.path.lastIndexOf('/')));module.FS.writeFile(file.path,file.bytes);audio?.resourceChanged(file.path);emit('transfer',{mode:file.path.startsWith('/music/')?'ogg':'runtime',loaded:file.bytes.length,total:file.bytes.length,path:file.path});}
}
async function languagePack(pack,expectedLanguage){
 if(launched||pack?.language!==expectedLanguage||expectedLanguage!=='lang_zh-hans'||pack.manifest?.schema!=='eagler-touhou/thcrap-static-pack/1'||pack.manifest.game!=='th05'||pack.manifest.language!==expectedLanguage||!Array.isArray(pack.files)||pack.files.length!==1)throw Error('Invalid TH05 language pack');
 sameOrigin(pack.url);const file=pack.files[0],declaration=pack.manifest.files?.find(f=>f.path==='/thcrap/th05/chs.dat');
 const bytes=new Uint8Array(file.bytes);if(file.path!=='/thcrap/th05/chs.dat'||!declaration||bytes.length!==declaration.bytes||!bytes.length||bytes.length>16*1024*1024)throw Error('Invalid TH05 language resource');module.FS.writeFile('/data/chs.dat',bytes);
}
async function sync(){if(launched&&!options.replayViewer)module._th05_save();await store.sync();}
function cstring(value,operation){const bytes=new TextEncoder().encode(value+String.fromCharCode(0)),ptr=module._malloc(bytes.length);try{module.HEAPU8.set(bytes,ptr);return operation(ptr);}finally{module._free(ptr);}}
function openReplay(path,stage){const bytes=store.read(path);validateUserFile(path,bytes);const first=stage??Array.from(bytes.subarray(53,60)).findIndex(flag=>flag!==0);if(first<0||first>6||!cstring(SAVE_ROOT+'/'+userPath(path),ptr=>module._th05_replay_open(ptr,first)))throw Error('Replay could not be opened');}
function roomView(){
 netplay?.close();netplay=null;launched=false;cancelAnimationFrame(raf);clearInterval(saveTimer);clearAll();
 if(!module._th05_coop_room())throw Error('TH05 room title failed');
 module._th05_coop_draw();module._th05_present();firstFrame=false;
 emit('room-menu',{presented:true,launchSerial});
}
function finish(code=0,reason){
 if(finishTask)return finishTask;if(closed)return Promise.resolve();
 closed=true;launched=false;cancelAnimationFrame(raf);clearInterval(saveTimer);
 finishTask=(async()=>{
  try{if(reason&&netplay?.abort)await netplay.abort(reason);else netplay?.close();}catch{}
  if(!nativeAborted){try{clearAll();module?._th05_shutdown();}catch(failure){reason??=failure;code=1;}}
  try{audio?.close();}catch{}
  try{await store?.sync();}catch(failure){reason??=failure;code=1;}
  if(reason)error(reason);
  emit('exit',{code,status:code===0?'success':'error'});
 })();return finishTask;
}
function saveCoopReplay(bytes){
 const occupied=new Set(store.list().map(file=>file.path.toLowerCase()));
 for(let slot=0;slot<=0xffff;slot++){
  const path='replay/th05mp_ud'+slot.toString(16).padStart(4,'0')+'.rpy';
  if(occupied.has(path))continue;
  store.write(path,bytes);
  void store.sync().catch(reason=>emit('runtime-info',{replaySaveError:reason.message}));
  return;
 }
 throw Error('TH05 multiplayer replay slots are full');
}
function frame(time){raf=0;if(closed||!launched)return;
 try{
  if(document.hidden){clock.reset();presentation.reset();raf=requestAnimationFrame(frame);return;}
  const gap=clock.add(time);maxGap=Math.max(maxGap,gap);
  let steps=0;
  if(multiplayer&&!options.replayViewer){
   while(clock.due(1/56)&&netplay?.advance()){clock.consume(1/56);audio.pump();steps++;}
   // Draw UI independently of confirmation waits so a short tap is captured
   // even while a peer is late. Its action travels in the next owned input.
   audio.pump();
   const revision=netplay?.revision??0,fresh=revision!==presentation.worldRevision;
   if(presentation.world(time,revision,module._th05_input_scene()===0,netplay?.recovering)){module._th05_coop_draw();module._th05_present();if(fresh)healthFrames++;}
   if(time-lastHealth>=1000){emit('frame-health',{fps:healthFrames*1000/Math.max(1,time-lastHealth),maxGapMs:maxGap,frameMs:1000/56,scene:module._th05_scene(),ticks:module._th05_ticks(),confirmed:netplay?.confirmed??-1,inputDelay:netplay?.delay??0});lastHealth=time;healthFrames=0;maxGap=0;}
   if(steps&&!firstFrame){firstFrame=true;emit('first-frame',launchSerial===undefined?{}:{launchSerial});}
   raf=requestAnimationFrame(frame);return;
  }
  if(options.replayViewer&&!playback.advancing())clock.reset();
  else if(playback.active()&&playback.rate()===0){for(let n=0;n<32&&playback.advancing();n++){playback.tick();audio.pump();steps++;}}
  else while(clock.consume((multiplayer?1/56:module._th05_frame_seconds())/(playback.active()?playback.rate():1))){if(playback.active()){if(!playback.advancing())break;playback.tick();}else{touch.tick();if(!module._th05_tick(+!clock.due(module._th05_frame_seconds())))throw Error('TH05 game frame failed');}audio.pump();steps++;}
  if(multiplayer&&steps)module._th05_coop_draw();
  playback.update(time);
  if(!module._th05_alive()){void finish();return;}
  if(presentation.fresh(time,options.limitPresentationTo60!==false,steps,module._th05_frame_seconds()/(playback.active()?playback.rate():1))){module._th05_present();healthFrames++;}
  if((steps||options.replayViewer)&&!firstFrame){firstFrame=true;requestAnimationFrame(()=>{if(!closed)emit('first-frame',launchSerial===undefined?{}:{launchSerial});});}
  if(time-lastHealth>=1000){emit('frame-health',{fps:healthFrames*1000/Math.max(1,time-lastHealth),maxGapMs:maxGap,frameMs:module._th05_frame_seconds()*1000,scene:module._th05_scene(),ticks:module._th05_ticks()});emit('audio-health',audio.health());lastHealth=time;healthFrames=0;maxGap=0;}
  raf=requestAnimationFrame(frame);
 }catch(reason){fatal(reason);}
}
const api={multiplayer,saveRoot:multiplayer?"/savesth05mp":SAVE_ROOT,running:()=>launched,viewer:()=>options.replayViewer===true,resources,languagePack,
 get store(){return store;},sync,
 async configure(value){
  if(initialized&&value.language!==language){module._th05_shutdown();initialized=false;}
  options=value.options;module.eaglerOptions=options;music=value.music;language=value.language;
  cstring(language,game=>cstring(options.thpracLocale??(language==='ja'?'ja-JP':'zh-CN'),locale=>module._th05_language_configure(game,locale)));
  if(language!=='ja'&&!module.FS.analyzePath('/data/chs.dat').exists)throw Error('Install the TH05 translation before launch');
  const fs=module.FS;fs.writeFile('/data/config.ini',new TextEncoder().encode(language==='ja'?'':'MOD_FILE = chs.dat\n'));
  const fonts=language==='lang_zh-hans'||options.thpracLocale==='zh-CN'?['/unifont.otf','/data/gothic.ttf','/msgothic.ttc']:['/data/gothic.ttf','/msgothic.ttc','/unifont.otf'];
  const font=fonts.find(path=>fs.analyzePath(path).exists);if(font){if(font!=='/data/gothic.ttf'){if(fs.analyzePath('/data/gothic.ttf').exists)fs.unlink('/data/gothic.ttf');fs.symlink(font,'/data/gothic.ttf');}try{const face=new FontFace('TH05GameFont',fs.readFile(font).buffer);await face.load();document.fonts.add(face);}catch(reason){throw Error('TH05 shared font could not be loaded: '+reason.message);}}
  await audio.configure(music,options);configured=true;
 },
 async prepareRoom(){if(!initialized){
 clearAll();module._th05_viewer(1);if(!module._th05_initialize())throw Error('TH05 preview failed');initialized=true;
 for(let n=0;n<2000&&module._th05_scene()!==2;n++)module._th05_tick(0);
 for(let n=0;n<180&&module._th05_scene()===2;n++)module._th05_tick(0);
 module._th05_viewer(0);
 }
 module._th05_coop_draw();module._th05_present();return {presented:true};},
 async returnRoom(){if(netplay)await netplay.end();return {presented:true};},
 async launch(message){if(!configured||closed)throw Error('Runtime cannot launch');clearAll();cstring(language,game=>cstring(options.thpracLocale??(language==='ja'?'ja-JP':'zh-CN'),locale=>module._th05_language_configure(game,locale)));module._th05_viewer(+!!options.replayViewer);module._th05_practice_configure(+(options.thpracEnabled===true&&!options.replayViewer));if(!initialized){if(!module._th05_initialize())throw Error('TH05 initialization failed');initialized=true;}module._th05_options(+(options.alwaysHitbox===true),+(options.focusHitboxEnabled===true));if(options.replayPath&&!multiplayer)openReplay(options.replayPath);if(multiplayer&&!options.replayViewer){
 if(!options.netplayMode)throw Error('TH05 multiplayer requires a room run');
 netplay=(options.netplaySpectator?createSpectator:createMultiplayer)(options,{module,saveReplay:saveCoopReplay,emit:(event,payload)=>{if(event==='netplay-ended')roomView();else emit(event,payload);},fail:fatal,sample(){touch.tick();const value=[module._th05_sample_buttons()|menuPulse,...motion];motion=[0,0,0];menuPulse=0;return value;}});
 module.eaglerCoopAction=index=>{menuPulse=index===0?4096:index===1?32768:16384;};
 await netplay.start();
 }
 if(multiplayer&&options.replayViewer){
  module.eaglerCoopAction=undefined;
  if(options.replayPath)await playback.command({action:'open',path:options.replayPath});
 }
 launched=true;launchSerial=message.launchSerial;clock.reset();clock.credit=1/56;presentation.reset();lastHealth=performance.now();emit('runtime-info',{renderer:'C++ / WebGL GLES2',architecture:'uth05win-native-cpp',version:'1.0.0',protocol});raf=requestAnimationFrame(frame);saveTimer=setInterval(()=>{unloadSync=sync().catch(error);},3000);},
 spectatorPlayback(message){if(!launched||!options.netplaySpectator)throw Error('Spectator playback is not active');return netplay.command(message);},
 async playback(message){if(!launched||!options.replayViewer)throw Error('Launch the Replay workspace first');return playback.command(message);},
 key(message){if(!options.replayViewer&&!options.netplaySpectator)keyboard.event(message,message.down,'hosted');audio.resume();},clearKeyboard,cancelTouch,
 touch(message){if(!options.replayViewer&&!options.netplaySpectator){audio.resume();touch.controls(message);module.eaglerControls={thpracMobilePanelsEnabled:options.thpracEnabled===true&&message.thpracMobilePanelsEnabled===true,thpracMobilePanelsVisible:message.thpracMobilePanelsVisible===true};}},
 pointer(message){if(!options.replayViewer&&!options.netplaySpectator){audio.resume();const rect=canvas.getBoundingClientRect();pointers.pointer(hostedPoint(message,rect.width,rect.height));}},
 practicePointer(message){if(!options.replayViewer&&!options.netplaySpectator)pointers.pointer({...message,id:message.id??0},true);},
};
try{
 bridge=installManagedShell(api,window);
 module=await createModule({canvas,print:console.log,printErr:console.error,eaglerError:error,onAbort:reason=>{nativeAborted=true;if(module)module.eaglerAborted=true;fatal(Error(String(reason)));}});globalThis.Module=module;
 store=createStore(module,{root:multiplayer?'/savesth05mp':SAVE_ROOT});await store.initialize();if(multiplayer)module.FS.symlink('/savesth05mp',SAVE_ROOT);module.FS.mkdirTree('/data');module.FS.mkdirTree('/music');
 if(query.get('managedData')!=='1'||typeof parent.__eaglerPrepareManagedRuntimeDataV1!=='function')throw Error('TH05 requires the Launcher DATA provider');
 const prepared=await parent.__eaglerPrepareManagedRuntimeDataV1({game:'th05',generation:query.get('gameGeneration'),epoch:bridge.epoch}),files=await decodeContent(prepared.buffer);
 for(const [path,bytes] of files){module.FS.mkdirTree('/data/'+path.split('/').slice(0,-1).join('/'));module.FS.writeFile('/data/'+path,bytes);}module.FS.chdir('/data');
 const noteStream=new Blob([module.FS.readFile('/data/music-notes.bin.gz')]).stream().pipeThrough(new DecompressionStream('gzip'));module.eaglerMusicNotes=decodeMusicNotes(new Uint8Array(await new Response(noteStream).arrayBuffer()));
 audio=createAudio(module,emit);module.eaglerAudio=audio;
 pointers=createPointerRouter(module,touch);
 const coopReplay=multiplayer?createCoopReplayDriver(module,path=>store.read(path),()=>applyDisplay(module,options.th05MultiplayerDisplay)):null;
 playback=createPlayback({emit,read:path=>store.read(path),...(coopReplay??{validate:path=>cstring(SAVE_ROOT+'/'+path,ptr=>module._th05_replay_validate(ptr)),open:(path,stage)=>cstring(SAVE_ROOT+'/'+path,ptr=>module._th05_replay_open(ptr,stage)),step:()=>module._th05_step(),frame:()=>module._th05_replay_frame(),stage:()=>module._th05_replay_stage(),scene:()=>module._th05_scene()}),clear:clearAll,resetClock:()=>clock.reset(),draw:()=>{if(multiplayer)module._th05_coop_draw();else module._th05_draw();module._th05_present();},gate:value=>audio.playback(value),yield:()=>new Promise(resolve=>setTimeout(resolve,0))});
 globalThis.__eaglerDirectInputBridge=createDirectInputBridge({protocol,game:'th05',epoch:bridge.epoch,origin:location.origin,ready:()=>launched&&!document.hidden&&!closed,dispatch(message){switch(message.command){case 'keyboard':api.key(message);break;case 'keyboard-clear':clearKeyboard();break;case 'touch-controls':api.touch(message);break;case 'touch-cancel':cancelTouch();break;case 'direct-touch':api.pointer(message);break;default:return false;}return true;}});
 for(const name of ['keydown','keyup'])addEventListener(name,event=>{
  const down=name==='keydown';
  if(down)audio.resume();
  if(!keyboardReady())return;
  if(keyboard.mapNative(event,down)){event.preventDefault();event.stopImmediatePropagation();return;}
  if(keyboard.event(event,down))event.preventDefault();
 },{capture:true});
 addEventListener('blur',clearAll);
 document.addEventListener('visibilitychange',()=>{if(closed)return;clearAll();clock.reset();presentation.reset();module._th05_focus(+!document.hidden);audio.visibility(document.hidden);if(document.hidden)unloadSync=sync().catch(error);});
 const pointer=event=>{const rect=canvas.getBoundingClientRect(),scale=Math.min(rect.width/640,rect.height/480),left=rect.left+(rect.width-640*scale)/2,top=rect.top+(rect.height-480*scale)/2;return {id:event.pointerId,x:(event.clientX-left)/scale,y:(event.clientY-top)/scale,type:event.type==='pointerdown'?'down':event.type==='pointermove'?'move':'up'};};
 for(const type of ['pointerdown','pointermove','pointerup','pointercancel'])canvas.addEventListener(type,event=>{const message=pointer(event);if(type==='pointerdown')canvas.setPointerCapture(event.pointerId);if(options.replayViewer||options.netplaySpectator)return;pointers.pointer(message,event.pointerType!=='touch'&&event.pointerType!=='pen');audio.resume();event.preventDefault();});
 canvas.addEventListener('webglcontextlost',event=>{event.preventDefault();fatal(Error('TH05 WebGL context lost'));});
 addEventListener('pagehide',()=>{if(closed){bridge.close();return;}netplay?.close();clearAll();cancelAnimationFrame(raf);clearInterval(saveTimer);module._th05_save();unloadSync=store.sync().catch(console.error);audio.close();bridge.close();closed=true;});
 bridge.ready();
}catch(reason){error(reason);}
