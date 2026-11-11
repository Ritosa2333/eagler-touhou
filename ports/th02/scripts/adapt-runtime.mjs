import {adaptPC98Practice} from '../../../common/browser/adapt-pc98-practice.mjs';
import {readFile,writeFile,copyFile,mkdir} from 'node:fs/promises';
import {resolve} from 'node:path';
export async function adaptRuntime({patch,replaceOnce,output,port,web,runtime,repo,files}){
await patch('solo.js',text=>{
  text=replaceOnce(text,'function release(){','function release(){globalThis.__eaglerPC98Input.releaseGameplay();','release managed keys');
  text=replaceOnce(text,"let bits=enabled?keyboardBits(keys,'solo')|padBits(navigator.getGamepads?.()[Number($('gamepad').value)],'solo'):0;",'let bits=enabled?globalThis.__eaglerPC98Input.sample({play:readSoloState()?.mode===1&&!readSoloState()?.hit}):0;','solo input');
  text=replaceOnce(text,'if(enabled)bits=player.sample(bits);','', 'legacy touch input');
  text=replaceOnce(text,'m=motion?unpackTouch(player.pack(bits)):null','m=motion?globalThis.__eaglerPC98Input.motion():null','solo motion');
  text=replaceOnce(text,'player.alwaysPoint()||p?.options.alwaysPoint','globalThis.__eaglerPC98Options().alwaysHitbox||p?.options.alwaysPoint','hitbox');
  text=replaceOnce(text,"if(config.mode!=='target')replayUI.offerSave(blob,result);","void managedSaveReplay(blob);",'managed recording');
  text=replaceOnce(text,"onError:error=>{music?.suspend();","onError:error=>{globalThis.__eaglerPC98Emit({event:'error',message:error.message});music?.suspend();",'managed solo error');
  text=replaceOnce(text,"onExit:()=>{music?.suspend();","onExit:()=>{void managedNaturalExit();music?.suspend();",'managed solo exit');
  return text;
});
await adaptPC98Practice({patch,replaceOnce,game:'th02'});
await writeFile(resolve(output,'solo.js'),await readFile(resolve(output,'solo.js'),'utf8')+'\n'+await readFile(resolve(port,'web/managed-solo-api.js'),'utf8'));
await patch('practice-attempt.js',text=>replaceOnce(text,'if(!replay)await installPractice(data,resources,practice);',`if(!replay)await installPractice(data,resources,practice);
      if(!replay&&globalThis.__eaglerPC98Files){const {Fat12}=await import('./solo-fat12.js');const fat=new Fat12(data);for(const [name,bytes]of globalThis.__eaglerPC98Files.memory)if(['HUUHI.DAT','HUUMA.CFG'].includes(name))fat.put('HUMAROKU/'+name,bytes);}`,'saved files'));
// Managed Package Store owns installation. The original browser cache is bypassed.
await patch('preload-cache.js',text=>replaceOnce(text,"if(!globalThis.indexedDB){resolve(null);return;}","if(globalThis.__eaglerPC98Content||!globalThis.indexedDB){resolve(null);return;}",'asset cache'));
await patch('site.css',text=>replaceOnce(text,"url('assets/launcher-background.webp')","none",'standalone page artwork'));
await patch('bgm-version.js',text=>text.replace(/export async function loadMidiTracks\(signal\)\{[\s\S]*?\n\}/,"export async function loadMidiTracks(){return {};}"));
await patch('room-title-worker.js',text=>{
  text=replaceOnce(text,"emulator = await (await import('./room-title-th05.js')).createTitleCore({canvas, disk, wasmBinary});","throw Error('TH02 title only');",'other-title preview');
  text=replaceOnce(text,'async function boot({game, canvas, disk, wasmBinary, module})','async function boot({game, canvas, disk, wasmBinary, module, managedTitle=false})','managed title mode');
  // Keep the actual settled title visible throughout an idle main room.
  // No demo/input/save state advances in this disposable preview instance.
  return replaceOnce(text,"send('ready', {frames, pixels, program: m.UTF8ToString(m._emGetRunningProgram())});","send('ready', {frames, pixels, program: m.UTF8ToString(m._emGetRunningProgram())});\n  if(managedTitle)return;",'managed title hold');
});
await patch('replay/external-loader.js',text=>text.replace("new URL('dosbox-jspi.mjs',vendor)","new URL('../vendor/dosbox-x/dosbox-jspi.mjs',import.meta.url)").replace("new URL('wdosbox-x-fast.wasm',vendor)","new URL('../vendor/dosbox-x/wdosbox-x-fast.wasm',import.meta.url)"));
await patch('netplay/room.js',text=>{
  text=replaceOnce(text,"import {createRelay} from './relay.js';","import {createManagedTransport} from '../managed-transport.mjs';\nconst createRelay=(_session,_generation,roles,fail)=>createManagedTransport(globalThis.__eaglerPC98Options(),roles,fail);",'main transport');
  text=replaceOnce(text,"const attempt=++connectionAttempt,relayMode=useRelay;","const attempt=++connectionAttempt,relayMode=true;",'managed channel interface');
  text=replaceOnce(text,"async function request(action,data={}){","async function request(action,data={}){\n  throw Error('房间成员与状态由主项目管理：'+action);",'legacy API guard');
  text=replaceOnce(text,"async function poll(){","async function poll(){return;",'legacy polling');
  text=replaceOnce(text,"async function returnToRoom(reason='本局已结束，可重新准备。',snapshot=null){","async function returnToRoom(reason='本局已结束，可重新准备。',snapshot=null){\n  return managedAPI.end(reason);",'managed room return');
  text=replaceOnce(text,'function fail(error){','function fail(error){\n  if(!stopped){managedAPI.stop();globalThis.__eaglerPC98Emit({event:"error",message:String(error.message||error)});}return;','managed fatal');
  text=replaceOnce(text,"if(session){\n    const q=new URLSearchParams", "if(false){\n    const q=new URLSearchParams",'legacy progress');
  text=replaceOnce(text,"if(session.role==='host')replayRecorder.connectNetwork", "if(false)replayRecorder.connectNetwork",'legacy replay service');
  text=replaceOnce(text,"if(session)navigator.sendBeacon", "if(false)navigator.sendBeacon",'legacy leave beacon');
  text=replaceOnce(text,'normalize(player.sample(keyboardBits(keys)|childBits|gamepadBits())|pendingAction)','normalize(globalThis.__eaglerPC98Input.sample({play:runtime?.touchContext()?.play===true})|pendingAction)','host input');
  text=replaceOnce(text,'packed=player.pack(bits|pendingAction,{includePointPreference:false});','packed=packTouch(bits|pendingAction,{...globalThis.__eaglerPC98Input.motion(),alwaysPoint:!!globalThis.__eaglerPC98Options().alwaysHitbox});','host direct touch');
  text="import {packTouch} from '../touch-input.js';\n"+text;
  return text;
});
await writeFile(resolve(output,'netplay/room.js'),await readFile(resolve(output,'netplay/room.js'),'utf8')+'\n'+await readFile(resolve(port,'web/managed-room-api.js'),'utf8'));
await patch('netplay/runtime.html',text=>replaceOnce(text,'<script>','<script type="module">\nimport "../managed-child.mjs";','managed child resources'));
// Source UI is retained as the engine's DOM contract. The Launcher supplies the
// lobby; standalone entry scripts and their secondary room service never boot.
let roomHtml=await readFile(resolve(web,'lan.html'),'utf8');
roomHtml=roomHtml.replace(/<script\b[^>]*>[\s\S]*?<\/script>/g,'');
await writeFile(resolve(output,'managed-room.html'),roomHtml);runtime.push('managed-room.html');
for(const name of await files(resolve(port,'web'))) if(!name.endsWith('-api.js')) { await copyFile(resolve(port,'web',name),resolve(output,name)); runtime.push(name); }
for(const name of ['directory-keyboard.mjs','keyboard-input-bridge.mjs','keyboard-owners.mjs','direct-input-bridge.mjs','pc98-practice.mjs']) {await copyFile(resolve(repo,'common/browser',name),resolve(output,name));runtime.push(name);}
for(const name of ['assets/option-0.png','assets/option-1.png','assets/option-2.png']) {await mkdir(resolve(output,'assets'),{recursive:true});await copyFile(resolve(web,name),resolve(output,name));runtime.push(name);}
let html=await readFile(resolve(web,'solo.html'),'utf8');
html=html.replace('<head>','<head><meta name="eagler-data-provider" content="retail-memory">');
html=replaceOnce(html,'src="solo.js"','src="managed-shell.mjs"','managed entry');
html=html.replace('</head>',`<style>body>header,body>nav,main>.intro,main>details,.player-toolbar,.player-actions,.player-movement,.player-note,.touch-surface,[class^="touch-"],[data-layout-control]{display:none!important}html,body{margin:0;background:#000}#game.immersive{position:fixed;inset:0}.player-stage{width:100%;height:100%}</style></head>`);
await writeFile(resolve(output,'th02.html'),html);runtime.push('th02.html');

await patch('replay/audio.js',text=>text
  .replace("if(game==='01'){const {LocalAudio}=await import('../netplay/audio.js');native=new LocalAudio();await native.unlock();context=native.context;await context.suspend();return;}","if(game!=='02')throw Error('TH02 Replay only');")
  .replace("const [{LocalBgm},{LocalSfx}]=await Promise.all([import('../local-bgm.js'),import('../local-sfx.js')]);bgm=new LocalBgm(context,()=>{});sfx=new LocalSfx(context);","throw Error('TH02 Replay only');")
  .replace("const {LocalSe}=await import('../netplay/local-se.js');sfx=new LocalSe(context);","throw Error('TH02 Replay only');"));

}
