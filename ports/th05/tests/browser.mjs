import {createServer} from 'node:http';
import {readFile,mkdir} from 'node:fs/promises';
import {resolve,extname} from 'node:path';
import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {inspectReplay} from '../web/replay.mjs';
import {ORIGINAL_BGM,AKYU_BGM,musicFileName} from '../web/audio-layout.mjs';
const {unzipSync}=createRequire(new URL('../../../launcher/package.json',import.meta.url))('fflate');
import {launchBrowser} from '../../../tools/browser/launch.mjs';
const root=resolve(import.meta.dirname,'..'),runtime=resolve(process.env.TH05_TEST_RUNTIME??resolve(root,process.env.TH05_BROWSER_TESTS==='1'?'artifacts/test-runtime':'artifacts/runtime')),content=resolve(process.env.TH05_TEST_CONTENT??resolve(root,'artifacts/content/th05.data'));
const font=resolve(process.env.TH05_TEST_FONT??resolve(root,'../../dependencies/unifont-15.1.05/unifont-15.1.05.otf'));
const fixture=`<!doctype html><html><body style="margin:0"><script>
window.events=[];window.responses=new Map();window.serial=0;
window.__eaglerPrepareManagedRuntimeDataV1=async request=>{if(request.game!=='th05'||request.epoch!==7)throw Error('identity');return {buffer:await (await fetch('/th05.data')).arrayBuffer()};};
addEventListener('message',e=>{if(e.origin!==location.origin)return;events.push(e.data);if(e.data.request&&responses.has(e.data.request)){responses.get(e.data.request)(e.data);responses.delete(e.data.request);}});
window.command=(command,payload={})=>new Promise(resolve=>{const request='r'+(++serial);responses.set(request,resolve);document.querySelector('iframe').contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th05',epoch:7,command,request,...payload},location.origin);});
</script><iframe style="border:0;width:640px;height:480px" src="/runtime/th05.html?runtimeEpoch=7&managedData=1"></iframe></body></html>`;
const server=createServer(async(req,res)=>{try{let body,type;if(req.url==='/'){body=fixture;type='text/html';}else if(req.url==='/th05.data'){body=await readFile(content);type='application/octet-stream';}else if(req.url==='/language.json'){const zip=await readFile(resolve(process.env.TH05_TEST_ASSETS??resolve(root,'artifacts'),'languages/language/lang_zh-hans.zip')),files=unzipSync(zip);body=JSON.stringify({language:'lang_zh-hans',url:'/language.zip',bytes:zip.length,manifest:JSON.parse(new TextDecoder().decode(files['manifest.json'])),files:[{path:'/thcrap/th05/chs.dat',bytes:Array.from(files['thcrap/th05/chs.dat'])}]});type='application/json';}else if(req.url.startsWith('/music/')){const name=req.url.slice(7);if(!/^[a-z0-9._-]+$/.test(name))throw Error('path');body=await readFile(resolve(process.env.TH05_TEST_ASSETS??resolve(root,'artifacts'),'content/music',name));type='audio/ogg';}else if(req.url==='/font.ttf'){body=await readFile(font);type='font/ttf';}else{const name=decodeURIComponent(new URL(req.url,'http://test').pathname).replace(/^\/runtime\//,'');if(name.includes('..')||name.startsWith('/'))throw Error('path');body=await readFile(resolve(runtime,name));type=({'.mjs':'text/javascript','.html':'text/html','.wasm':'application/wasm','.json':'application/json'})[extname(name)]??'application/octet-stream';}res.writeHead(200,{'Content-Type':type});res.end(body);}catch(error){res.writeHead(404);res.end(error.message);}});
await new Promise(resolve=>server.listen(0,'127.0.0.1',resolve));
const browser=await launchBrowser();const logs=[],report={};
try{
 report.runtime=JSON.parse(await readFile(resolve(runtime,'build.json'),'utf8'));report.runtime={wasmSha256:report.runtime.sha256,builtAt:report.runtime.builtAt,diagnostic:report.runtime.diagnostic};
 const page=await browser.newPage({viewport:{width:660,height:500}});page.on('pageerror',e=>logs.push('pageerror: '+e.message));page.on('console',e=>{if(e.type()==='error')logs.push(e.text());});
 async function launch(enabled,locale='ja-JP',language='ja',audioMode='none',source='original',viewer=false){await page.goto('http://127.0.0.1:'+server.address().port);await page.waitForFunction(()=>events.some(e=>e.event==='ready'),null,{timeout:30000});
  const config=await page.evaluate(async({enabled,locale,language,audioMode,source,viewer})=>{const pack=language==='lang_zh-hans'?await (await fetch('/language.json')).json():null;if(pack)for(const file of pack.files)file.bytes=new Uint8Array(file.bytes);return command('configure',{music:audioMode==='none'?'none':'ogg',language,...(pack?{runtimePack:pack}:{}),sharedResources:[{path:'/data/gothic.ttf',url:'/font.ttf'}],options:{replayViewer:viewer,musicSource:source,oggDecodeMode:audioMode==='full'?'full':'stream',thpracEnabled:enabled,thpracLocale:locale,alwaysHitbox:true,touchEnabled:true,touchSensitivity:150,touchMovementMode:'touch-unlimited'}});},{enabled,locale,language,audioMode,source,viewer});assert.equal(config.ok,true,JSON.stringify(config));
  const result=await page.evaluate(()=>command('launch'));assert.equal(result.ok,true,JSON.stringify(result));await page.waitForFunction(()=>events.some(e=>e.event==='first-frame'),null,{timeout:15000});return page.frames().find(f=>f.url().includes('/runtime/'));}
 let frame=await launch(false);
 report.vanilla=await frame.evaluate(()=>({enabled:Module._th05_practice_enabled(),scene:Module._th05_scene(),ticks:Module._th05_ticks()}));assert.equal(report.vanilla.enabled,0);
 await frame.evaluate(()=>{Module._th05_key(112,1);Module._th05_step();Module._th05_key(112,0);});assert.equal(await frame.evaluate(()=>Module._th05_practice_tool(0)),0);
 // Advance the real opening, title and character scenes through their input.
 await frame.evaluate(()=>{for(let i=0;i<220&&Module._th05_scene()===1;i++)Module._th05_step();for(let i=0;i<250;i++)Module._th05_step();});
 await page.screenshot({path:resolve(root,'artifacts/title-vanilla.png')});
 const press=async(key,frames=2)=>frame.evaluate(({key,frames})=>{Module._th05_key(key,1);for(let i=0;i<frames;i++)Module._th05_step();Module._th05_key(key,0);Module._th05_step();},{key,frames});
 await press(90);await frame.evaluate(()=>{for(let i=0;i<90;i++)Module._th05_step();});await press(90);await frame.evaluate(()=>{for(let i=0;i<110;i++)Module._th05_step();});
 report.gameplay=await frame.evaluate(()=>({scene:Module._th05_scene(),x:Module._th05_player_x(),y:Module._th05_player_y(),ticks:Module._th05_ticks()}));assert.equal(report.gameplay.scene,4,JSON.stringify(report.gameplay));
 await frame.evaluate(()=>{for(let i=0;i<500&&Module._th05_replay_frame()<30;i++)Module._th05_step();});
 await press(27);await frame.evaluate(()=>{for(let i=0;i<20;i++)Module._th05_step();});assert.equal(await frame.evaluate(()=>Module._th05_input_scene()),0);await press(82);await frame.evaluate(()=>{for(let i=0;i<100;i++)Module._th05_step();});assert.equal(await frame.evaluate(()=>Module._th05_scene()),4);assert.equal(await frame.evaluate(()=>Module._th05_input_scene()),4);report.pauseRestart=true;
 await frame.evaluate(()=>{for(let i=0;i<500&&Module._th05_replay_frame()<30;i++)Module._th05_step();});const before=await frame.evaluate(()=>Module._th05_player_x());await frame.evaluate(()=>{Module._th05_touch_motion(20,0,1);Module._th05_step();});assert.ok(await frame.evaluate(()=>Module._th05_player_x())>before+10);

 const gamepad=await frame.evaluate(()=>{const x=Module._th05_player_x();globalThis.__eaglerGamepadInputBridge={owned:()=>true,sample:()=>128};Module._th05_step();const moved=Module._th05_player_x();globalThis.__eaglerGamepadInputBridge.sample=()=>0;Module._th05_step();delete globalThis.__eaglerGamepadInputBridge;return {x,moved};});assert.ok(gamepad.moved>gamepad.x);report.gamepad=true;
 const recorded=await frame.evaluate(()=>{const history=[];for(let n=0;n<24;n++){Module._th05_touch_motion(n%2?1:-2,-.5,1);Module._th05_step();history.push({frame:Module._th05_replay_frame(),x:Module._th05_player_x(),y:Module._th05_player_y()});}const text=new TextEncoder().encode('/savesth05/replay/replay00.rpy'+String.fromCharCode(0)),ptr=Module._malloc(text.length);Module.HEAPU8.set(text,ptr);if(!Module._th05_replay_save(ptr))throw Error('Replay save failed');const bytes=Array.from(Module.FS.readFile('/savesth05/replay/replay00.rpy'));if(!Module._th05_replay_open(ptr,0))throw Error('Replay open failed');Module._free(ptr);const actual=[];for(let n=0;n<2000;n++){Module._th05_step();const index=Module._th05_replay_frame();if(history.some(h=>h.frame===index))actual.push({frame:index,x:Module._th05_player_x(),y:Module._th05_player_y()});if(index>=history.at(-1).frame)break;}return {history,actual,bytes};});
 assert.deepEqual(recorded.actual,recorded.history);assert.equal(inspectReplay(new Uint8Array(recorded.bytes)).touch,true);report.touchReplayFrames=recorded.actual.length;
 await press(90,60);await page.screenshot({path:resolve(root,'artifacts/gameplay-vanilla.png')});
 const list=await page.evaluate(()=>command('list'));assert.equal(list.ok,true);assert.ok(list.files.some(f=>f.path==='game.cfg'));
 frame=await launch(true,'en-US');const persisted=await page.evaluate(()=>command('read',{path:'replay/replay00.rpy'}));assert.equal(persisted.ok,true);assert.deepEqual(persisted.bytes,recorded.bytes);report.replayDurability=true;assert.equal(await frame.evaluate(()=>Module._th05_practice_enabled()),1);await frame.evaluate(()=>{Module._th05_key(112,1);Module._th05_step();Module._th05_key(112,0);});assert.equal(await frame.evaluate(()=>Module._th05_practice_tool(0)),1);report.thprac=true;
 const nativeLabel=()=>{const ptr=Module._th05_practice_label(),end=Module.HEAPU8.indexOf(0,ptr);return new TextDecoder().decode(Module.HEAPU8.subarray(ptr,end));};assert.equal(await frame.evaluate(nativeLabel),'Invincibility');
 frame=await launch(false,'zh-CN','lang_zh-hans');assert.equal(await frame.evaluate(()=>Module._th05_codepage()),936);assert.equal(await frame.evaluate(nativeLabel),'无敌');report.languages=['ja','lang_zh-hans'];report.practiceLanguages=['ja-JP','en-US','zh-CN'];
 

 frame=await launch(false,'ja-JP','ja','none','original',true);
 const send=async(command,payload={})=>{const response=await page.evaluate(({command,payload})=>window.command(command,payload),{command,payload});assert.equal(response.ok,true,JSON.stringify(response));return response;};
 await send('write',{path:'replay/replay00.rpy',bytes:recorded.bytes});
 const description=(await send('playback',{action:'inspect',path:'replay/replay00.rpy'})).description;assert.equal(description.stages[0].id,1);
 await send('playback',{action:'open',path:'replay/replay00.rpy',stage:1});await send('playback',{action:'pause'});
 const target=recorded.history[10];await send('playback',{action:'seek',value:target.frame});assert.equal(await frame.evaluate(()=>Module._th05_replay_frame()),target.frame);assert.equal(await frame.evaluate(()=>Module._th05_player_x()),target.x);assert.equal(await frame.evaluate(()=>Module._th05_player_y()),target.y);
 await send('playback',{action:'rate',value:2});await send('playback',{action:'play'});await page.waitForFunction(()=>events.some(e=>e.playback?.ended),null,{timeout:15000});report.replayWorkspace={inspect:true,seek:true,ended:true};
 report.audio=[];
 for(const [mode,source] of [['none','original'],['stream','original'],['full','original'],['stream','akyu']]){
  frame=await launch(false,'ja-JP','ja',mode,source);await page.mouse.click(30,30);
  const def=(source==='akyu'?AKYU_BGM:ORIGINAL_BGM).tracks['op-m86'],file=musicFileName(def.url);
  if(mode!=='none')await send('resources',{resources:[{path:'/music/'+file,url:'/music/'+file,bytes:def.bytes,sha256:def.audioSha256}]});
  await frame.evaluate(()=>{Module.eaglerAudio.select('OP.M2');Module.eaglerAudio.action(1);Module.eaglerAudio.resume();});
  if(mode!=='none')await frame.waitForFunction(()=>Module.eaglerAudio.health().playing,null,{timeout:15000});
  await frame.evaluate(()=>{Module.eaglerAudio.action(2);Module.eaglerAudio.sound(1);Module.eaglerAudio.pump();});
  const health=await frame.evaluate(()=>Module.eaglerAudio.health());assert.ok(health.sfx.played>0,JSON.stringify(health));assert.equal(health.playing,false);assert.equal(health.musicSource,source);
  if(mode!=='none'){assert.equal(health.path,'/music/'+file);assert.equal(health.loopStart,def.loopStart);assert.equal(health.loopEnd,def.loopEnd);await frame.evaluate(()=>Module.eaglerAudio.action(3));await frame.waitForFunction(()=>Module.eaglerAudio.health().playing);}
  assert.deepEqual(await page.evaluate(()=>events.filter(e=>e.event==='music-error')),[]);
  report.audio.push({mode,source,prIdentity:true,sfx:health.sfx.backend,loopStart:health.loopStart,loopEnd:health.loopEnd});
 }
 const invalid=await page.evaluate(()=>command('read',{path:'../score.dat'}));assert.equal(invalid.ok,false);
 if(process.env.TH05_BROWSER_TESTS==='1'){
  report.nativeStageCases=await frame.evaluate(()=>{
   const onError=Module.eaglerError;
   let failure;
   Module.eaglerError=message=>{failure=message;onError?.(message);};
   try{
    const cases=Module._th05_native_stage_matrix();
    if(failure)throw Error(failure);
    return cases;
   }finally{Module.eaglerError=onError;}
  });
  assert.equal(report.nativeStageCases,84);
 }
 const runtimeErrors=await page.evaluate(()=>events.filter(e=>e.event==='error'||e.event==='music-error'));assert.deepEqual(runtimeErrors,[]);
 assert.deepEqual(logs.filter(e=>!e.includes('404')&&!e.includes('GL Driver Message')),[]);
 report.logs=logs;report.events=await page.evaluate(()=>events.map(e=>e.event).filter(Boolean));report.protocol='eagler-touhou/1';console.log(JSON.stringify(report,null,2));
 await mkdir(resolve(root,'artifacts'),{recursive:true});const {writeFile}=await import('node:fs/promises');await writeFile(resolve(root,'artifacts/browser-validation.json'),JSON.stringify(report,null,2)+'\n');await writeFile(resolve(root,'artifacts/browser-'+(process.env.TH05_BROWSER_TESTS==='1'?'native':'production')+'-validation.json'),JSON.stringify(report,null,2)+'\n');
}catch(error){console.error('Browser logs:',logs);console.error('Report:',report);throw error;}finally{await browser.close();await new Promise(resolve=>server.close(resolve));}
