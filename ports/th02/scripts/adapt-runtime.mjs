import {adaptPC98Practice} from '../../../common/browser/adapt-pc98-practice.mjs';
import {readFile,writeFile,copyFile,mkdir} from 'node:fs/promises';
import {resolve} from 'node:path';
import {installPc98Pmd} from '../../../tools/build/pc98-pmd.mjs';
import {createHash} from 'node:crypto';
export async function adaptRuntime({patch,replaceOnce,output,port,web,runtime,repo,files,readInput}){
await installPc98Pmd(repo,output,runtime);
await patch('solo.js',text=>{
  text=replaceOnce(text,'function release(){','function release(){globalThis.__eaglerPC98Input.releaseGameplay();','release managed keys');
  text=replaceOnce(text,"let bits=enabled?keyboardBits(keys,'solo')|padBits(navigator.getGamepads?.()[Number($('gamepad').value)],'solo'):0;",'let bits=enabled?globalThis.__eaglerPC98Input.sample({play:readSoloState()?.mode===1&&!readSoloState()?.hit}):0;','solo input');
  text=replaceOnce(text,'if(enabled)bits=player.sample(bits);','', 'legacy touch input');
  text=replaceOnce(text,'m=motion?unpackTouch(player.pack(bits)):null','m=motion?globalThis.__eaglerPC98Input.motion():null','solo motion');
  text=replaceOnce(text,'player.alwaysPoint()||p?.options.alwaysPoint','globalThis.__eaglerPC98Options().alwaysHitbox||p?.options.alwaysPoint','hitbox');
  text=replaceOnce(text,"if(config.mode!=='target')replayUI.offerSave(blob,result);","void managedSaveReplay(blob);",'managed recording');
  text=replaceOnce(text,"onError:error=>{music?.suspend();","onError:error=>{globalThis.__eaglerPC98Emit({event:'error',message:error.message});music?.suspend();",'managed solo error');
  text=replaceOnce(text,"onExit:()=>{music?.suspend();","onExit:()=>{void managedNaturalExit();music?.suspend();",'managed solo exit');
  text=replaceOnce(text,"const replaySources=['../practice-attempt.js'","const replaySources=['../managed-rapid-fire.mjs','../practice-attempt.js'",'rapid fire replay identity');
  return text;
});
await adaptPC98Practice({patch,replaceOnce,game:'th02'});
await writeFile(resolve(output,'solo.js'),await readFile(resolve(output,'solo.js'),'utf8')+'\n'+await readFile(resolve(port,'web/managed-solo-api.js'),'utf8'));
await patch('practice-attempt.js',text=>replaceOnce(text,'if(!replay)await installPractice(data,resources,practice);',`if(!replay)await installPractice(data,resources,practice);
      if(!replay&&globalThis.__eaglerPC98Options().th02RapidFire!==false){const {Fat12}=await import('./solo-fat12.js');const {installRapidFire}=await import('./managed-rapid-fire.mjs');const fat=new Fat12(data),entry=fat.find('HUMAROKU/MAIN.EXE'),exe=fat.read(entry);installRapidFire(exe,original,false);fat.replace(entry,exe);}
      if(!replay&&globalThis.__eaglerPC98Files){const {Fat12}=await import('./solo-fat12.js');const fat=new Fat12(data);for(const [name,bytes]of globalThis.__eaglerPC98Files.memory)if(['HUUHI.DAT','HUUMA.CFG'].includes(name))fat.put('HUMAROKU/'+name,bytes);}`,'saved files'));
await patch('netplay/runtime.js',text=>replaceOnce(text,'const at=patch.headerSize+patch.codeSegment*16+patch.entries.run_config;',`const {installRapidFire}=await import('../managed-rapid-fire.mjs');installRapidFire(exe,patch,true);
 const at=patch.headerSize+patch.codeSegment*16+patch.entries.run_config;`,'rapid fire native hook'));
await patch('netplay/th02-state.js',text=>replaceOnce(text,'((value&256)>>1);','((value&256)>>1)|((value&1024)>>1);','rapid fire input flag'));
await patch('netplay/rollback-queue.js',text=>replaceOnce(text,'const HELD=32|64|2048,DIRECTIONS=15;','const HELD=32|64|1024|2048,DIRECTIONS=15;','rapid fire prediction'));
// Managed Package Store owns installation. The original browser cache is bypassed.
await patch('preload-cache.js',text=>replaceOnce(text,"if(!globalThis.indexedDB){resolve(null);return;}","if(globalThis.__eaglerPC98Content||!globalThis.indexedDB){resolve(null);return;}",'asset cache'));
await patch('site.css',text=>replaceOnce(text,"url('assets/launcher-background.webp')","none",'standalone page artwork'));
await patch('bgm-version.js',text=>text.replace(/export async function loadMidiTracks\(signal\)\{[\s\S]*?\n\}/,"export async function loadMidiTracks(){return {};}"));
await patch('room-title-worker.js',text=>{
  text=replaceOnce(text,"const settle = ['02','04'].includes(game) ? 240 : 720;","const settle = 720;",'settled TH02 title');
  text=replaceOnce(text,"emulator = await (await import('./room-title-th05.js')).createTitleCore({canvas, disk, wasmBinary});","throw Error('TH02 title only');",'other-title preview');
  text=replaceOnce(text,'async function boot({game, canvas, disk, wasmBinary, module})','async function boot({game, canvas, disk, wasmBinary, module, managedTitle=false})','managed title mode');
  return text;
});
await patch('replay/external-loader.js',text=>text.replace("new URL('dosbox-jspi.mjs',vendor)","new URL('../vendor/dosbox-x/dosbox-jspi.mjs',import.meta.url)").replace("new URL('wdosbox-x-fast.wasm',vendor)","new URL('../vendor/dosbox-x/wdosbox-x-fast.wasm',import.meta.url)"));
// Give the managed room its own canvas and worker client without a nested page.
{
  let client=await readFile(resolve(port,'web/netplay/runtime-worker-client.js'),'utf8');
  const imports=client.lastIndexOf("import {LocalBgm} from './local-bgm.js';")+"import {LocalBgm} from './local-bgm.js';".length;
  client=client.slice(0,imports)+"\nexport function createManagedRuntime(canvas,notify){\n"+client.slice(imports);
  client=client.replace("const PROTOCOL='th02-rollback/6',canvas=document.getElementById('canvas');","const PROTOCOL='th02-rollback/6';");
  client=client.replace("window.parent.postMessage({protocol:PROTOCOL,event:'boot-error',message},location.origin)","notify({protocol:PROTOCOL,event:'boot-error',message})");
  client=client.replace("window.parent.postMessage(data.message,location.origin)","notify(data.message)");
  client=client.replace("document.getElementById('controls-summary').textContent=controlsSummary('online');",'');
  const listeners=client.indexOf("window.addEventListener('keydown'");
  if(listeners<0)throw Error('TH02 worker client listeners changed');
  client=client.slice(0,listeners)+"canvas.addEventListener('contextlost',()=>fatal('游戏画面上下文已丢失'));\nreturn sync;\n}\n";
  await writeFile(resolve(output,'netplay/managed-runtime.js'),client);runtime.push('netplay/managed-runtime.js');
}
await patch('netplay/room.js',text=>{
  const begin=text.indexOf("  iframe=document.createElement('iframe');iframe.title='本机游戏模拟器'");
  const end=text.indexOf("  runtime.setMusicVolume(",begin);
  if(begin<0||end<0)throw Error('TH02 managed runtime entry changed');
  text=text.slice(0,begin)+`  iframe=document.createElement('div');iframe.className='managed-runtime-screen';
  const canvas=document.createElement('canvas');canvas.width=640;canvas.height=400;canvas.tabIndex=0;
  canvas.style.cssText='width:100%;height:100%;object-fit:contain;image-rendering:pixelated;touch-action:none';
  iframe.style.cssText='width:100%;height:100%';iframe.append(canvas);player.stage.append(iframe);
  const {createManagedRuntime}=await import('./managed-runtime.js');
  runtime=createManagedRuntime(canvas,message=>{
    if(stopped)return;
    if(message.event==='boot-error')fail(Error(message.message));
    else if(message.event==='boot-progress'){bootStage=String(message.message).slice(0,300);bootStep=message.step;showStartup();}
  });
`+text.slice(end);
  text=replaceOnce(text,"  iframe.contentWindow.addEventListener('blur',()=>player.reset());",'', 'direct runtime input');
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
  text=replaceOnce(text,'packed=packTouch(bits|pendingAction,{...globalThis.__eaglerPC98Input.motion(),alwaysPoint:!!globalThis.__eaglerPC98Options().alwaysHitbox});','packed=packTouch(bits|pendingAction|((bits&32)&&globalThis.__eaglerPC98Options().th02RapidFire!==false?1024:0),{...globalThis.__eaglerPC98Input.motion(),alwaysPoint:!!globalThis.__eaglerPC98Options().alwaysHitbox});','rapid fire local preference');
  text="import {packTouch} from '../touch-input.js';\n"+text;
  return text;
});
await writeFile(resolve(output,'netplay/room.js'),await readFile(resolve(output,'netplay/room.js'),'utf8')+'\n'+await readFile(resolve(port,'web/managed-room-api.js'),'utf8'));
await patch('netplay/runtime.html',text=>replaceOnce(text,'<script>','<script type="module">\nimport "../managed-child.mjs";','managed child resources'));
for(const name of await files(resolve(port,'web'))) if(/^managed-[^/]+\.(?:mjs|js)$/.test(name)&&!name.endsWith('-api.js')) { await copyFile(resolve(port,'web',name),resolve(output,name)); runtime.push(name); }
{
  const path=resolve(output,'netplay/runtime.json'), manifest=JSON.parse(await readFile(path,'utf8'));
  manifest.assets['managed-rapid-fire.mjs']=createHash('sha256').update(await readFile(resolve(output,'managed-rapid-fire.mjs'))).digest('hex');
  await writeFile(path,JSON.stringify(manifest,null,2)+'\n');
}
for(const name of ['directory-keyboard.mjs','keyboard-input-bridge.mjs','keyboard-owners.mjs','direct-input-bridge.mjs','pc98-game-fps.mjs','pc98-practice.mjs','pc98-practice-theme.css','pc98-pause-menu.mjs']) {await copyFile(resolve(repo,'common/browser',name),resolve(output,name));runtime.push(name);}
for(const name of ['assets/option-0.png','assets/option-1.png','assets/option-2.png']) {await mkdir(resolve(output,'assets'),{recursive:true});await writeFile(resolve(output,name),await readInput(name));runtime.push(name);}
const html=`<!doctype html><html><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><style>html,body,#game,.player-stage{width:100%;height:100%;margin:0;background:#000;overflow:hidden}.player-stage{display:flex;align-items:center;justify-content:center}.solo-screen{position:relative;width:min(100vw,160vh);height:min(62.5vw,100vh);aspect-ratio:8/5;flex:none}.player-toolbar{display:none}#canvas{outline:0;width:100%;height:100%;image-rendering:pixelated}</style><script type="module" src="managed-shell.mjs"></script></head><body class="shell"><div id="game" class="th02-player"><div class="player-toolbar"></div><div class="player-stage"><div class="solo-screen"><canvas id="canvas" width="640" height="400" tabindex="0"></canvas></div></div></div></body></html>`;
await writeFile(resolve(output,'th02.html'),html);runtime.push('th02.html');

await patch('replay/audio.js',text=>text
  .replace("if(game==='01'){const {LocalAudio}=await import('../netplay/audio.js');native=new LocalAudio();await native.unlock();context=native.context;await context.suspend();return;}","if(game!=='02')throw Error('TH02 Replay only');")
  .replace("const [{LocalBgm},{LocalSfx}]=await Promise.all([import('../local-bgm.js'),import('../local-sfx.js')]);bgm=new LocalBgm(context,()=>{});sfx=new LocalSfx(context);","throw Error('TH02 Replay only');")
  .replace("const {LocalSe}=await import('../netplay/local-se.js');sfx=new LocalSe(context);","throw Error('TH02 Replay only');"));

}
