import {printPC98Inputs} from '../../../tools/build/pc98-inputs.mjs';
import {adaptPC98Practice} from '../../../common/browser/adapt-pc98-practice.mjs';
import {createHash} from 'node:crypto';
import {readFile,writeFile,mkdir,readdir,copyFile,stat,mkdtemp,rename,rm} from 'node:fs/promises';
import {resolve,relative,dirname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {execFileSync} from 'node:child_process';
import {DATA_SCHEMA,DATA_MAGIC,DATA_LAYOUT_SPEC} from '../web/managed-data.mjs';
const port = resolve(fileURLToPath(new URL('..', import.meta.url))), repo=resolve(port,'../..');
const args = Object.fromEntries(process.argv.slice(2).map(arg => { const at=arg.indexOf('='); return [arg.slice(2,at<0?undefined:at),at<0?true:arg.slice(at+1)]; }));
if(args['print-inputs']) await printPC98Inputs({port,game:'th04'});
const source = resolve(args.source||port), web=resolve(source,'web');
const sourceIdentity=args.source?null:JSON.parse(await readFile(resolve(port,'source-import.json'),'utf8'));
const privateInputs=new Set(sourceIdentity?.resources||[]);
const assets=resolve(args.assets||resolve(port,'.cache/source-resources'));
async function readInput(name){
  try{return await readFile(resolve(privateInputs.has(name)?assets:web,name));}
  catch(error){if(error.code==='ENOENT'&&privateInputs.has(name))throw Error('Missing private resource '+name+'; specify --assets=<standalone web directory>.');throw error;}
}
let revision=sourceIdentity?.commit??null;if(args.source){try{revision=execFileSync('git',['-c','safe.directory='+source.replaceAll('\\','/'),'rev-parse','HEAD'],{cwd:source,encoding:'utf8'}).trim();}catch{}}
const sha = bytes => createHash('sha256').update(bytes).digest('hex');
async function files(directory,prefix='') {
  const result=[];
  for(const item of await readdir(directory,{withFileTypes:true})) {
    if(item.isSymbolicLink()) throw Error('Source symlinks are not accepted');
    if(item.isDirectory()) result.push(...await files(resolve(directory,item.name),prefix+item.name+'/'));
    else if(item.isFile()) result.push(prefix+item.name);
  }
  return result.sort();
}
const retiredInputs=new Set(['local-music-disk.json','local-music-patch.json','netplay/frame-queue.js','netplay/input-wire.js','th04-local-music.hdi.gz']);
const names=[...new Set([...(await files(web)).filter(name=>!/^managed-[^/]+\.(?:mjs|js)$/.test(name)),...privateInputs])].sort().filter(name=>!retiredInputs.has(name)&&name!=='room-title-th05.js'&&!/(?:^|\/)(?:build|reports|baseline|screenshots|backups|tools|test-results|__pycache__)(?:\/|$)/.test(name) && !/\.(?:map|wat|cpuprofile|log|tmp|bak)$/.test(name));
const input=[]; for(const name of names) { const bytes=await readInput(name); input.push({path:name,bytes:bytes.length,sha256:sha(bytes)}); }
if(args['print-package-inputs']) { console.log(JSON.stringify({game:'th04',kind:'managed-import',inputs:input})); process.exit(0); }
const cache=resolve(port,'.cache');await mkdir(cache,{recursive:true});
const staging=await mkdtemp(resolve(cache,'build-')),output=resolve(staging,'build-eagler'),content=resolve(staging,'content');
try {
await mkdir(output,{recursive:true});await mkdir(content,{recursive:true});
const runtime=[], payload=[], entries=[];let offset=0;
for(const item of input) {
  const bytes=await readInput(item.path);
  if(/\.(?:js|mjs|css|html|wasm|json)$/.test(item.path)) {
    const target=resolve(output,item.path);await mkdir(dirname(target),{recursive:true});await writeFile(target,bytes);runtime.push(item.path);
  } else {
    if(!bytes.length) continue;
    entries.push({...item,offset});payload.push(bytes);offset+=bytes.length;
  }
}
function replaceOnce(text,from,to,label) { if(text.split(from).length!==2) throw Error('Upstream changed at '+label); return text.replace(from,to); }
const patch=async(name,edit)=>{const path=resolve(output,name);await writeFile(path,edit(await readFile(path,'utf8')));};
await patch('solo.js',text=>{
  text=replaceOnce(text,'volume:.7','volume:managedMusicMode===\'none\'?0:.7','practice restart music preference');
  text=replaceOnce(text,'function release(){','function release(){globalThis.__eaglerPC98Input.releaseGameplay();','release managed keys');
  text=replaceOnce(text,"base=keyboardBits(keys,'solo');try{base|=padBits(Array.from(navigator.getGamepads?.()||[]).find(p=>p?.connected),'solo');}catch{}","base=globalThis.__eaglerPC98Input.sample({play:game&&!state.flags});",'solo input');
  text=replaceOnce(text,'let bits=enabled?player.sample(base):0;','let bits=enabled?base:0;','main touch owner');
  text=replaceOnce(text,'const touch=unpackTouch(player.pack(bits));','const touch=globalThis.__eaglerPC98Input.motion();','solo motion');
  text=replaceOnce(text,'syntheticInputEvents.add(event);canvas.dispatchEvent(event);','syntheticInputEvents.add(event);event.__eaglerPC98Synthetic=true;canvas.dispatchEvent(event);','native input dispatch');
  text=replaceOnce(text,"if(config.mode!=='target')replayUI.offerSave(blob,result);","void managedSaveReplay(blob);",'managed recording');
  text=replaceOnce(text,"onError:error=>{soloMusic?.suspend();","onError:error=>{globalThis.__eaglerPC98Emit({event:'error',message:error.message});soloMusic?.suspend();",'managed solo error');
  text=replaceOnce(text,"onExit:()=>{soloMusic?.suspend();","onExit:()=>{void managedNaturalExit();soloMusic?.suspend();",'managed solo exit');
  text=replaceOnce(text,"points:(focus()?1:0)|(player.alwaysPoint()||ps?.options.alwaysPoint?8:0)","points:(focus()?1:0)|(globalThis.__eaglerPC98Options().alwaysHitbox||ps?.options.alwaysPoint?8:0)",'managed hitbox preference');
  return text;
});
await adaptPC98Practice({patch,replaceOnce,game:'th04'});
await writeFile(resolve(output,'solo.js'),await readFile(resolve(output,'solo.js'),'utf8')+'\n'+await readFile(resolve(port,'web/managed-solo-api.js'),'utf8'));
await patch('practice-attempt.js',text=>replaceOnce(text,'if(!replay)await installPractice(data,resources,practice);',`if(!replay)await installPractice(data,resources,practice);
      if(!replay&&globalThis.__eaglerPC98Files){const {Fat12}=await import('./solo-fat12.js');const fat=new Fat12(data);for(const [name,bytes]of globalThis.__eaglerPC98Files.memory)if(['GENSOU.SCR','MIKO.CFG'].includes(name))fat.put('GENSO/'+name,bytes);}`,'saved files'));
// Managed Package Store owns installation. The original browser cache is bypassed.
await patch('preload-cache.js',text=>replaceOnce(text,"if(!globalThis.indexedDB){resolve(null);return;}","if(globalThis.__eaglerPC98Content||!globalThis.indexedDB){resolve(null);return;}",'asset cache'));
await patch('site.css',text=>replaceOnce(text,"url('assets/launcher-background.webp')","none",'standalone page artwork'));
await patch('bgm-version.js',text=>text.replace(/export async function loadMidiTracks\(signal\)\{[\s\S]*?\n\}/,"export async function loadMidiTracks(){return {};}"));
await patch('netplay/local-se.js',text=>replaceOnce(text,"new URL('se-manifest.json',base)","new URL('../assets/se-manifest.json',import.meta.url)",'sound manifest URL'));
// Shared lab helpers include optional branches for other games. Specialize
// these imports so the published TH04 generation has a closed dependency set.
await patch('replay/audio.js',text=>{
  text=replaceOnce(text,"if(game==='01'){const {LocalAudio}=await import('../netplay/audio.js');native=new LocalAudio();await native.unlock();context=native.context;await context.suspend();return;}","if(game!=='04')throw Error('TH04 Replay audio only');",'other-title replay audio');
  text=replaceOnce(text,"const [{LocalBgm},{LocalSfx}]=await Promise.all([import('../local-bgm.js'),import('../local-sfx.js')]);bgm=new LocalBgm(context,()=>{});sfx=new LocalSfx(context);","throw Error('TH04 Replay audio only');",'other-title battle audio');
  return replaceOnce(text,"const {LocalSfx}=await import('../netplay/local-sfx.js');sfx=new LocalSfx(context);","throw Error('TH04 Replay audio only');",'other-title sound');
});
await patch('room-title-worker.js',text=>{
  text=replaceOnce(text,"emulator = await (await import('./room-title-th05.js')).createTitleCore({canvas, disk, wasmBinary});","throw Error('TH04 title only');",'other-title preview');
  text=replaceOnce(text,'async function boot({game, canvas, disk, wasmBinary, module})','async function boot({game, canvas, disk, wasmBinary, module, managedTitle=false})','managed title mode');
  // Keep the actual settled title visible throughout an idle main room.
  // No demo/input/save state advances in this disposable preview instance.
  return replaceOnce(text,"send('ready', {frames, pixels, program: m.UTF8ToString(m._emGetRunningProgram())});","send('ready', {frames, pixels, program: m.UTF8ToString(m._emGetRunningProgram())});\n  if(managedTitle)return;",'managed title hold');
});
await patch('replay/external-loader.js',text=>text.replace("new URL('dosbox-jspi.mjs',vendor)","new URL('../vendor/dosbox-x/dosbox-jspi.mjs',import.meta.url)").replace("new URL('wdosbox-x-fast.wasm',vendor)","new URL('../vendor/dosbox-x/wdosbox-x-fast.wasm',import.meta.url)"));
await patch('solo-worker.js',text=>replaceOnce(text,"const {NativePause}=await import('./native-pause.js');bridge=new NativePause(()=>emulator,patch);","throw Error('TH04 solo worker only');",'other-title pause'));
await patch('netplay/room.js',text=>{
  text=replaceOnce(text,"import {createRelay} from './relay.js';","import {createManagedTransport} from '../managed-transport.mjs';\nconst createRelay=(_session,_generation,roles,fail)=>createManagedTransport(globalThis.__eaglerPC98Options(),roles,fail);",'main transport');
  text=replaceOnce(text,"const useRelay=mode.transport==='ws';","const useRelay=true;",'managed channel interface');
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
for(const name of await files(resolve(port,'web'))) if(/^managed-[^/]+\.(?:mjs|js)$/.test(name)&&!name.endsWith('-api.js')) { await copyFile(resolve(port,'web',name),resolve(output,name)); runtime.push(name); }
for(const name of ['directory-keyboard.mjs','keyboard-input-bridge.mjs','keyboard-owners.mjs','direct-input-bridge.mjs','pc98-practice.mjs']) {await copyFile(resolve(repo,'common/browser',name),resolve(output,name));runtime.push(name);}
let html=await readFile(resolve(web,'solo.html'),'utf8');
html=html.replace('<head>','<head><meta name="eagler-data-provider" content="retail-memory">');
html=replaceOnce(html,'src="solo.js"','src="managed-shell.mjs"','managed entry');
html=html.replace('</head>',`<style>body>header,body>nav,main>.intro,main>details,.player-toolbar,.player-actions,.player-movement,.player-note,.touch-surface,[class^="touch-"],[data-layout-control]{display:none!important}html,body{margin:0;background:#000}#game.immersive{position:fixed;inset:0}.player-stage{width:100%;height:100%}</style></head>`);
await writeFile(resolve(output,'th04.html'),html);runtime.push('th04.html');
const escape=text=>text.replaceAll('&','&amp;').replaceAll('<','&lt;').replaceAll('>','&gt;');
let notices='<!doctype html><html lang="en"><meta charset="utf-8"><title>TH04 runtime notices</title><body><h1>TH04 runtime sources and licenses</h1><p>Adapted from <a href="https://github.com/IxinorTyan/th04-coop-lab">th04-coop-lab</a>, revision '+escape(revision??'local working tree')+'.</p>';
for(const name of ['vendor/dosbox-x/NOTICE.md','vendor/dosbox-x/LICENSE','vendor/np2/LICENSE','room-ui-assets/LICENSE.txt'])notices+='<h2>'+escape(name)+'</h2><pre style="white-space:pre-wrap">'+escape(await readFile(resolve(web,name),'utf8'))+'</pre>';
await writeFile(resolve(output,'NOTICE.html'),notices+'</body></html>');runtime.push('NOTICE.html');
const manifestBytes=Buffer.from(JSON.stringify({schema:DATA_SCHEMA,game:'th04',files:entries})),header=Buffer.alloc(12);header.write(DATA_MAGIC);header.writeUInt32LE(manifestBytes.length,8);
await writeFile(resolve(content,'th04.data'),Buffer.concat([header,manifestBytes,...payload]));
const identities={};for(const name of [...new Set(runtime)].sort()){const bytes=await readFile(resolve(output,name));identities[name]={bytes:bytes.length,sha256:sha(bytes)};}
await writeFile(resolve(output,'manifest.json'),JSON.stringify({game:'th04',protocol:'eagler-touhou/1',features:{thprac:false,languages:false,focusHitbox:false},files:identities},null,2)+'\n');
const manifestFile=await readFile(resolve(output,'manifest.json'));identities['manifest.json']={bytes:manifestFile.length,sha256:sha(manifestFile)};
await writeFile(resolve(output,'runtime-files.json'),JSON.stringify({schema:'eagler-touhou/runtime-directory/1',files:identities},null,2)+'\n');
await writeFile(resolve(content,'source.json'),JSON.stringify({schema:'eagler-touhou/pc98-import/1',upstream:'https://github.com/IxinorTyan/th04-coop-lab',revision,sourceInputs:input,dataLayout:'sha256-'+sha(JSON.stringify(DATA_LAYOUT_SPEC)),adaptation:'ports/th04/web'},null,2)+'\n');
const contentBytes=(await stat(resolve(content,'th04.data'))).size;
// A failed patch leaves the last successful export intact. Swap complete
// directories so removed upstream files cannot linger in a later generation.
const swaps=[];
try {
  for(const name of ['build-eagler','content']){
    const target=resolve(port,name),backup=resolve(staging,'previous-'+name);
    let previous=false;
    try{await rename(target,backup);previous=true;}catch(error){if(error.code!=='ENOENT')throw error;}
    const swap={target,backup,previous,installed:false};swaps.push(swap);
    await rename(resolve(staging,name),target);swap.installed=true;
  }
}catch(error){
  for(const swap of swaps.reverse()){
    if(swap.installed)await rename(swap.target,resolve(staging,'failed-'+relative(port,swap.target)));
    if(swap.previous)await rename(swap.backup,swap.target);
  }
  throw error;
}
console.log(JSON.stringify({game:'th04',runtime:'ports/th04/build-eagler',data:'ports/th04/content/th04.data',runtimeFiles:Object.keys(identities).length,contentFiles:entries.length,contentBytes}));
} finally {
  // staging is created by mkdtemp directly inside this port's ignored cache.
  if(dirname(staging)!==cache||!relative(cache,staging).startsWith('build-'))throw Error('Invalid build staging path');
  await rm(staging,{recursive:true,force:true});
}
