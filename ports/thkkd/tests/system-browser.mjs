import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {productionBrowser,nativeState,runtimeCommand,wait} from './production-driver.mjs';
import {replayFixture} from './replay-fixture.mjs';
const args=Object.fromEntries(process.argv.slice(2).map(value=>{const at=value.indexOf('=');return [value.slice(2,at),value.slice(at+1)];}));
const url=args.url||'http://127.0.0.1:8177/',output=fileURLToPath(new URL('../build/quality-mp/'+(args.label||'system-audit')+'/',import.meta.url));mkdirSync(output,{recursive:true});
const report={schema:'thkkd/system-browser/1',url,checks:[]},driver=await productionBrowser({url,report}),{browser,page,key}=driver;
try{
 await page.evaluateOnNewDocument(()=>{window.__auditPads=[];Object.defineProperty(navigator,'getGamepads',{value:()=>__auditPads});});
 await driver.library();let frame=await driver.start();
 for(let i=0;i<8&&(await nativeState(frame)).outer!==100;i++)await key('z');assert.equal((await nativeState(frame)).outer,100);
 await wait(600);
 const media=()=>frame.evaluate(()=>{const p=__thkkdRuntime.platform;return {audioState:p.context.state,videos:[...p.graphs.values()].filter(g=>g.video).map(g=>({time:g.video.currentTime,paused:g.video.paused,ready:g.video.readyState,texture:!!g.texture})),bgm:[...p.sounds.values()].filter(s=>s.bgm).map(s=>({time:s.audio.currentTime,paused:s.audio.paused})),textures:p.renderer.textures.size,deletedTextures:[...p.renderer.textures].filter(t=>!p.renderer.gl.isTexture(t.texture)).length};});
 report.running=await media();assert.ok(report.running.videos.some(v=>v.time>0&&v.texture&&!v.paused));
 report.pcm=await frame.evaluate(async()=>{const p=__thkkdRuntime.platform,analyser=p.context.createAnalyser(),samples=new Float32Array(2048);p.bgmGain.connect(analyser);analyser.fftSize=samples.length;let peak=0;for(let i=0;i<30&&!peak;i++){await new Promise(resolve=>setTimeout(resolve,60));analyser.getFloatTimeDomainData(samples);peak=Math.max(...samples.map(Math.abs));}p.bgmGain.disconnect(analyser);analyser.disconnect();return peak;});assert.ok(report.pcm>0);
 await key('Escape');report.paused=await media();await wait(250);report.pauseHeld=await media();assert.ok(report.pauseHeld.videos.every((v,i)=>v.paused&&Math.abs(v.time-report.paused.videos[i].time)<.03));assert.ok(report.pauseHeld.bgm.every(s=>s.paused));await key('z');report.resumed=await media();assert.ok(report.resumed.videos.some(v=>!v.paused));report.checks.push('production-video-texture-BGM-PCM-native-pause-resume');
 report.replayTransitions=[];
 for(const stage of [0,1,0]){
   await runtimeCommand(page,'write',{path:'replay/system-fixture.rpy',bytes:[...replayFixture({stage,practice:true})]});await runtimeCommand(page,'playback',{action:'open',path:'replay/system-fixture.rpy'});await runtimeCommand(page,'playback',{action:'pause'});await runtimeCommand(page,'playback',{action:'seek',value:100});await wait(250);
   report.replayTransitions.push(await media());
 }
 report.resourceLeak=report.replayTransitions.at(-1).deletedTextures;console.log(JSON.stringify({resourceLeak:report.resourceLeak,transitions:report.replayTransitions}));
 report.readOnlyPresentation=await frame.evaluate(async()=>{
   const r=__thkkdRuntime,fs=r.core.FS,write=fs.writeFile,play=r.platform.playSound;let writes=0,sounds=0;
   fs.writeFile=(...args)=>{writes++;return write.apply(fs,args);};r.platform.playSound=(...args)=>{sounds++;return play.apply(r.platform,args);};
   const before=r.core.HEAPU8.slice(),seed=r.platform.seed,commands=JSON.stringify(r.platform.renderer.commands);
   try{for(let frame=0;frame<100;frame++)r.platform.renderer.present(frame%10/10);}finally{fs.writeFile=write;r.platform.playSound=play;}
   const after=r.core.HEAPU8.slice(),commandsAfter=JSON.stringify(r.platform.renderer.commands),sha=async bytes=>[...new Uint8Array(await crypto.subtle.digest('SHA-256',bytes))].map(value=>value.toString(16).padStart(2,'0')).join('');
   return {frames:100,before:await sha(before),after:await sha(after),writes,sounds,seedSame:seed===r.platform.seed,commandsSame:commands===commandsAfter};
 });assert.equal(report.readOnlyPresentation.before,report.readOnlyPresentation.after);assert.equal(report.readOnlyPresentation.writes,0);assert.equal(report.readOnlyPresentation.sounds,0);assert.ok(report.readOnlyPresentation.seedSame&&report.readOnlyPresentation.commandsSame);report.checks.push('100-extra-presentations-preserve-complete-WASM-memory-commands-RNG-files-and-audio');
 const openFixture=async(options,target)=>{await runtimeCommand(page,'write',{path:'replay/system-fixture.rpy',bytes:[...replayFixture(options)]});await runtimeCommand(page,'playback',{action:'open',path:'replay/system-fixture.rpy'});await runtimeCommand(page,'playback',{action:'pause'});await runtimeCommand(page,'playback',{action:'seek',value:target});await frame.waitForFunction(target=>__thkkdRuntime.core._thkkd_word(0xb43110)===target,{},target);};
 report.characters=[];
 for(let character=0;character<3;character++)for(let shot=0;shot<3;shot++){
   await openFixture({character,shot},70);
   const before=await frame.evaluate(()=>{const w=address=>__thkkdRuntime.core._thkkd_word(address),active=(base,count,stride)=>Array.from({length:count},(_,index)=>w(base+index*stride)===1).filter(Boolean).length;return {character:w(0xa74c0c),shot:w(0xa7e614),bombs:w(0xa8cd80),shots:active(0xa87be0,20,24)+active(0xa79090,80,56)};});assert.equal(before.character,character);assert.equal(before.shot,shot);assert.ok(before.shots>0);
   await runtimeCommand(page,'playback',{action:'seek',value:110});await frame.waitForFunction(()=>__thkkdRuntime.core._thkkd_word(0xb43110)===110);
   const after=await frame.evaluate(()=>({bombs:__thkkdRuntime.core._thkkd_word(0xa8cd80),timer:__thkkdRuntime.core._thkkd_word(0xa8cd94)}));assert.equal(after.bombs,before.bombs-1);assert.ok(after.timer>0);await page.screenshot({path:output+`character-${character}-${shot}.png`});report.characters.push({character,shot,before,after});
 }
 report.checks.push('all-nine-loadouts-native-shots-and-Bombs-in-production-runtime');report.stages=[];
 for(let stage=0;stage<8;stage++){
   const difficulty=stage<6?stage%4:stage===6?4:5;await openFixture({stage,difficulty,practice:stage<6,bombFrame:-1},150);
   const state=await frame.evaluate(()=>{const r=__thkkdRuntime,w=address=>r.core._thkkd_word(address);return {stage:w(0xb3e2ec),difficulty:w(0xa7e610),commands:r.platform.renderer.commands.length,error:document.querySelector('#error').textContent};});assert.equal(state.stage,stage);assert.equal(state.difficulty,difficulty);assert.ok(state.commands>0);assert.equal(state.error,'');await page.screenshot({path:output+`stage-${stage}.png`});report.stages.push(state);
 }
 report.checks.push('six-stages-EX-and-Omniverse-resources-render-in-production-runtime');
 await page.reload({waitUntil:'networkidle0'});frame=await driver.start();await wait(500);
 await page.evaluate(()=>{
   for(const element of document.querySelectorAll('*')){let fiber=element[Object.keys(element).find(key=>key.startsWith('__reactFiber$'))];while(fiber){const session=fiber.memoizedProps?.session;if(session?.gamepad){window.__auditSession=session;break;}fiber=fiber.return;}if(window.__auditSession)break;}
   if(!window.__auditSession)throw Error('启动器会话不存在');
   __auditPads=[{id:'System audit standard controller',index:0,connected:true,mapping:'standard',buttons:Array.from({length:17},()=>({pressed:false,value:0})),axes:[0,0,0,0]}];
   const model=__auditSession.gamepad;model.refresh();const device=model.getSnapshot().devices[0];model.select(0,device.key);model.setEnabled(true);model.bind(device.key,'confirm',[{kind:'button',index:3}]);
 });
 await key('ArrowDown');await key('ArrowDown');await key('ArrowDown');await key('ArrowDown');assert.equal((await nativeState(frame)).choice,6);await key('z');assert.equal((await nativeState(frame)).outer,15);
 await key('z');assert.equal((await nativeState(frame)).phase,2);
 await page.evaluate(()=>{__auditPads[0].buttons[1]={pressed:true,value:1};});await wait(200);report.keyConfig=await nativeState(frame);report.padSnapshot=await frame.evaluate(()=>({bits:__eaglerGamepadInputBridge.sample(0),native:__thkkdRuntime.input.padState?.()}));await page.evaluate(()=>{__auditPads[0].buttons[1]={pressed:false,value:0};});assert.equal(report.padSnapshot.bits&2,2);
 report.nativeKeyConfigWorks=report.keyConfig.phase===0;console.log(JSON.stringify({nativeKeyConfigWorks:report.nativeKeyConfigWorks,state:report.keyConfig}));
 if(!args.observe){assert.equal(report.resourceLeak,0,'重开后没有已删除纹理残留在资源集合');assert.ok(report.nativeKeyConfigWorks,'原生手柄设置接收实际映射按键');}
 await page.screenshot({path:output+'native-key-config.png'});const telemetry=await driver.collect();assert.equal(telemetry.filter(event=>event.event==='error'||event.event==='audio-health'&&['blocked','error'].includes(event.state)).length,0,'正式 Runtime 无意外的播放或平台错误');assert.equal(report.errors.length,0);report.result=args.observe?'observed':'passed';
}catch(error){report.result='failed';report.error=error.stack;process.exitCode=1;console.error(error);await page.screenshot({path:output+'failure.png'});}
finally{await driver.collect();writeFileSync(output+'result.json',JSON.stringify(report,null,2));await browser.close();}
