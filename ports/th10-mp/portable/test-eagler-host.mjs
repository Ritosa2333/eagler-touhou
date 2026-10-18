import test from 'node:test';
import assert from 'node:assert/strict';
import {existsSync,readFileSync} from 'node:fs';
const game=existsSync(new URL('../th08_web/cpp/game/AnmRenderer.cpp',import.meta.url))?'th08':'th10';
const {normalizeOptions,applyTouchOptions,touchControls,suspendRuntimeAudio,resumeRuntimeAudio,directTouch,resourcePath,ensureSharedFontAlias,installResources,observeMusicWrites,mountManagedData,isSupersededRuntimeError}=await import('../'+game+'_web/sdl-runtime/eagler-host.mjs');
const base='http://localhost/runtime/'+game+'/entry.html';
function filesystem(){
 const files=new Map();return {files,FS:{mkdirTree(){},writeFile(path,bytes){files.set(path,bytes);}}};
}
test('touch snapshots do not reset gesture mode',()=>{
 const calls=[];const core=new Proxy({}, {get:(_,name)=>(...args)=>calls.push([name,...args])});
 const options=normalizeOptions({touchEnabled:true,touchMovementMode:'touch',touchSensitivity:100});
 applyTouchOptions(core,options);calls.length=0;
 for(let i=0;i<60;i++)touchControls(core,options,{touchSensitivity:100,fireEnabled:true,joystickX:0,joystickY:0});
 assert.equal(calls.length,60);assert(calls.every(c=>c[0]==='sdl_touch_controls'));
 touchControls(core,options,{touchSensitivity:150,joystickX:65535,joystickY:-65535});
 assert.equal(calls.at(-2)[0],'sdl_touch_options');assert.deepEqual(calls.at(-1).slice(-2),[32767,-32767]);
 touchControls(core,options,{joystickX:16384,joystickY:NaN});
 assert.deepEqual(calls.at(-1).slice(-2),[16384,0]);
 assert(!calls.some(c=>c[0]==='sdl_touch_mode'));
});
test('foreground audio resumes browser context before Runtime loop',async()=>{
 const calls=[],core={sdl_loop_pause:value=>calls.push(value)};
 const context={state:'suspended',async resume(){this.state='running';}};
 assert.equal(await resumeRuntimeAudio({SDL3:{audioContext:context}},core,()=>true),true);
 assert.deepEqual(calls,[0]);
 calls.length=0;context.state='running';
 assert.equal(await resumeRuntimeAudio({SDL3:{audioContext:context}},core,()=>true),true);
 assert.deepEqual(calls,[0]);
 calls.length=0;context.state='suspended';context.resume=async()=>{throw Error('gesture required');};
 assert.equal(await resumeRuntimeAudio({SDL3:{audioContext:context}},core,()=>true),false);
 assert.deepEqual(calls,[]);
 calls.length=0;let foreground=true;context.state='suspended';context.resume=async()=>{context.state='running';foreground=false;};
 assert.equal(await resumeRuntimeAudio({SDL3:{audioContext:context}},core,()=>foreground),false);
 assert.deepEqual(calls,[]);
});
test('background audio detaches SDL browser output and reconnects before Runtime resumes',async()=>{
 const calls=[],events=[];const destination={};
 const node={disconnect(){events.push('disconnect');},connect(value){assert.equal(value,destination);events.push('connect');}};
 const context={state:'running',destination,async resume(){events.push('resume');}};
 const Module={SDL3:{audioContext:context,audio_playback:{scriptProcessorNode:node}}};
 const core={sdl_loop_pause:value=>calls.push(value)};
 assert.equal(suspendRuntimeAudio(Module,core),true);assert.deepEqual(events,['disconnect']);assert.deepEqual(calls,[1]);
 assert.equal(suspendRuntimeAudio(Module,core),true);assert.deepEqual(events,['disconnect']);assert.deepEqual(calls,[1,1]);
 calls.length=0;assert.equal(await resumeRuntimeAudio(Module,core,()=>true),true);assert.deepEqual(events,['disconnect','connect']);assert.deepEqual(calls,[0]);
});
test('shell startup cannot leave Runtime paused when AudioContext is already running',()=>{
 const shell=readFileSync(new URL('../'+game+'_web/sdl-runtime/shell.mjs',import.meta.url),'utf8').replaceAll('\r','');
 assert.match(shell,/if\(forcePause\)core\.sdl_loop_pause\(1\);\s*return resumeRuntimeAudio/);
 assert.doesNotMatch(shell,/if\(!forcePause&&\(!context\|\|context\.state==='running'\)\)return true/);
});
test('direct touch honors letterbox and viewport updates',()=>{
 const calls=[],core={sdl_touch:(...args)=>calls.push(args)};
 const canvas={getBoundingClientRect:()=>({left:80,top:0,width:640,height:480})};
 directTouch(core,canvas,{type:'down',id:3,x:.5,y:.5},{width:800,height:480});
 assert.deepEqual(calls[0],[0,3,.5,.5]);
 directTouch(core,canvas,{type:'bogus',id:3,x:0,y:0},{width:800,height:480});
 directTouch(core,canvas,{type:'move',id:3,x:NaN,y:0},{width:800,height:480});assert.equal(calls.length,1);
});
test('resource mount restricts game and directory',()=>{
 assert(resourcePath('/bgm-ogg/'+game+'_01.ogg',game));
 if(game==='th10'){
  assert(resourcePath('/msgothic.ttc',game));
  assert(resourcePath('/unifont.otf',game));
 }
 for(const path of ['/bgm-ogg/../save.dat','/fonts/../../shell.mjs','/savesth08/score.dat','/bgm-ogg/th06_01.ogg'])assert(!resourcePath(path,game));
});
test('TH10 shared fonts keep canonical Launcher paths',()=>{
 if(game!=='th10')return;
 const present=new Set(['/msgothic.ttc','/unifont.otf']),links=new Map();
 const Module={FS:{
  mkdirTree(){},
  stat(path){if(!present.has(path)&&!links.has(path))throw Error('missing');return {};},
  unlink(path){links.delete(path);},
  symlink(source,target){links.set(target,source);},
 }};
 assert.deepEqual(ensureSharedFontAlias(Module,'jp'),{source:'/msgothic.ttc',target:'/fonts/msgothic.ttc'});
 assert.equal(links.get('/fonts/msgothic.ttc'),'/msgothic.ttc');
 assert.deepEqual(ensureSharedFontAlias(Module,'chs'),{source:'/unifont.otf',target:'/fonts/simhei.ttf'});
 assert.equal(links.get('/fonts/simhei.ttf'),'/unifont.otf');
});
test('OGG resources install into live FS and notify native retry',async()=>{
 const module=filesystem(),path='/bgm-ogg/'+game+'_01.ogg';let notices=0;
 observeMusicWrites(module,{sdl_music_resource_changed:()=>notices++},game);
 await installResources(module,[{path,url:'./track.ogg',bytes:4}],{game,base,fetcher:async()=>new Response(new Uint8Array([79,103,103,83]))});
 assert.equal(notices,1);assert.equal(module.files.get(path).length,4);
 await assert.rejects(installResources(module,[{path,url:'https://remote.invalid/track.ogg'}],{game,base}),/same-origin/);
 await assert.rejects(installResources(module,[{path,url:'./x',bytes:5}],{game,base,fetcher:async()=>new Response(new Uint8Array(4))}),/size mismatch/);
});
test('managed DATA stays separate from immutable font resources',async()=>{
 const module=filesystem(),buffer=new Uint8Array(32).buffer;let request;
 await mountManagedData(module,{game,base,query:new URLSearchParams('managedData=1&gameGeneration=g1&runtimeEpoch=23'),
 parentWindow:{__eaglerPrepareManagedRuntimeDataV1:async value=>{request=value;return {buffer};}},
 fetcher:async()=>Response.json({schema:'eagler-sdl-resources/1',game,resources:[]})});
 assert.deepEqual(request,{game,generation:'g1',epoch:23});assert.equal(module.files.get('/game/'+game+'.dat').byteLength,32);
 await assert.rejects(mountManagedData(module,{game,base,query:new URLSearchParams('managedData=1&gameGeneration=g1'),
  parentWindow:{__eaglerPrepareManagedRuntimeDataV1:async()=>({buffer})},
  fetcher:async()=>Response.json({schema:'eagler-sdl-resources/1',game,resources:[]})}),/epoch unavailable/);
 const superseded=new DOMException('EAGLER_RUNTIME_SESSION_SUPERSEDED','AbortError');
 assert.equal(isSupersededRuntimeError(superseded),true);
 assert.equal(isSupersededRuntimeError(new DOMException('network aborted','AbortError')),false);
 await assert.rejects(mountManagedData(module,{game,base,query:new URLSearchParams('managedData=1&gameGeneration=g1&runtimeEpoch=23'),
  parentWindow:{__eaglerPrepareManagedRuntimeDataV1:async()=>{throw superseded;}},
  fetcher:async()=>Response.json({schema:'eagler-sdl-resources/1',game,resources:[]})}),error=>isSupersededRuntimeError(error));
 await assert.rejects(mountManagedData(module,{game,query:new URLSearchParams(),parentWindow:{}}),/eagler-touhou/);
});
test('options normalize invalid sensitivity and movement mode',()=>{
 const options=normalizeOptions({touchSensitivity:NaN,touchMovementMode:'invalid'});
 assert.equal(options.touchSensitivity,100);assert.equal(options.touchMovementMode,'touch');
 assert.equal(normalizeOptions({touchSensitivity:50}).touchSensitivity,100);
 assert.equal(normalizeOptions({touchSensitivity:99}).touchSensitivity,100);
 assert.equal(normalizeOptions({touchSensitivity:350}).touchSensitivity,300);
});
