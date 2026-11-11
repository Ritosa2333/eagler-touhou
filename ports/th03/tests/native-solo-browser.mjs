// Full Launcher, Package Store and native TH03 runtime (requires scripts/dev.mjs).
import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {findChromiumExecutable} from '../../../launcher/lib/chromium-executable.mjs';
const require=createRequire(new URL('../../../launcher/package.json',import.meta.url)),puppeteer=require('puppeteer-core');
const replayQa=process.env.TH03_REPLAY_QA==='1',focusQa=process.env.TH03_FOCUS_QA==='1';
const base=process.env.PC98_LAUNCHER_URL||'http://127.0.0.1:18330/';
const out=resolve(import.meta.dirname,'../.cache/launcher-browser');await mkdir(out,{recursive:true});
const browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling']});
const page=await browser.newPage();await page.setViewport({width:1360,height:900});
const errors=[],events=[];page.on('pageerror',e=>{errors.push(e.message);console.error('PAGE',e.message);});
page.on('console',m=>{if(m.type()==='error')console.error('CONSOLE',m.text());});
await page.exposeFunction('recordRuntimeEvent',m=>events.push(m));
await page.evaluateOnNewDocument(replayQa=>{
  if(replayQa&&location.pathname.endsWith('/th03.html')){const nativeSetInterval=window.setInterval;window.setInterval=(fn,delay,...args)=>nativeSetInterval(fn,delay>17&&delay<18?1:delay,...args);}
  addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1')window.recordRuntimeEvent(e.data);});
  window.__keyProbe=[];
  const post=Worker.prototype.postMessage;
  Worker.prototype.postMessage=function(message,...rest){
    if(message?.method==='key')window.__keyProbe.push(message.args[0]);
    return post.call(this,message,...rest);
  };
},replayQa||focusQa);
try{
  await page.goto(base+'?test=th03');
  await page.waitForFunction(()=>window.__eaglerBoot?.done===true,{timeout:45000});
  await page.waitForSelector('.book-game-link[data-game="th03"]',{timeout:45000});
  await page.evaluate(()=>document.querySelector('#firstUseNoticeDialog')?.close());
  console.log('Launcher TH03 card ready');
  await page.click('.book-game-link[data-game="th03"]');
  await page.waitForSelector('.tools.is-open',{visible:true});
  await page.$eval('.tools.is-open',async panel=>{await Promise.all(panel.getAnimations().map(animation=>animation.finished.catch(()=>{})));});
  await page.waitForSelector('#musicSelect');
  console.log('music',await page.$$eval('#musicSelect option',nodes=>nodes.map(n=>({value:n.value,disabled:n.disabled}))));
  const music=process.env.TH03_AKYU_QA==='1'?'ogg-akyu':await page.$$eval('#musicSelect option',nodes=>nodes.find(n=>!n.disabled&&n.value!=='none')?.value);
  await page.select('#musicSelect',music);
  await page.waitForFunction(()=>document.querySelector('#launch')?.disabled===false);
  await page.click('#launch');
  const deadline=Date.now()+120000;let lastLog=0;
  while(Date.now()<deadline&&!events.some(e=>e.event==='first-frame')){
    await page.evaluate(()=>{document.querySelector('#decisionDialog[open] #decisionConfirm')?.click();document.querySelector('#touchHelpClose')?.click();});
    if(events.some(e=>e.event==='error'))throw Error(JSON.stringify(events.filter(e=>e.event==='error')));
    if(Date.now()-lastLog>10000){console.log('Launcher status',await page.evaluate(()=>document.body.innerText.slice(-1200)));lastLog=Date.now();}
    await new Promise(r=>setTimeout(r,200));
  }
  assert(events.some(e=>e.event==='first-frame'),'full Launcher must present a real frame');
  const frame=page.frames().find(frame=>frame.url().includes('runtimeEpoch='));
  assert(frame,'runtime frame must exist');
  assert.equal(await frame.$('.masthead'),null,'runtime must not contain the standalone page');
  const info=events.find(e=>e.event==='runtime-info');assert.equal(info.renderer,'Native TH03');
  const state=await frame.evaluate(async()=>(await import('./managed-native-solo.mjs')).managedAPI.health());assert(state.running&&state.gameFrame>0);
  const canvas=await frame.$('canvas');await canvas.screenshot({path:resolve(out,'native-solo.png')});
  await page.keyboard.press('KeyZ',{delay:150});await new Promise(r=>setTimeout(r,1000));
  assert((await frame.evaluate(async()=>(await import('./managed-native-solo.mjs')).managedAPI.health().gameFrame))>state.gameFrame);
  assert(!events.some(e=>e.event==='error'));assert.deepEqual(errors,[]);console.log('PASS main launcher runs native TH03 solo');
  if(process.env.TH03_AKYU_QA==='1'){
   console.log('music config',await frame.evaluate(async()=>({options:__eaglerPC98Options(),health:(await import('./managed-native-solo.mjs')).managedAPI.health()})));await frame.waitForFunction(async()=>(await import('./managed-native-solo.mjs')).managedAPI.health().audio.music!==null,{timeout:15000});
   await page.keyboard.press('KeyZ',{delay:150});await frame.waitForFunction(async()=>(await import('./managed-native-solo.mjs')).managedAPI.health().audio.musicPlaying,{timeout:15000});const musicTrack=await frame.evaluate(async()=>(await import('./managed-native-solo.mjs')).managedAPI.health().audio.music);assert(musicTrack);console.log('PASS selectable Akyu track',musicTrack);
  }
  if(focusQa){
   await frame.evaluate(()=>{__eaglerPC98Options().th03RapidFire=false;const input=__eaglerPC98Input;let n=0;input.sample=()=>{n++;return n>300&&n%100<8?512:0;};});
   await frame.waitForFunction(async()=>(await import('./managed-native-solo.mjs')).managedAPI.health().player?.x>=0,{timeout:60000});
   await frame.evaluate(()=>{__eaglerPC98Input.sample=()=>64;__eaglerPC98Options().th03FocusEnabled=true;__eaglerPC98Options().focusHitboxEnabled=true;});
   await frame.waitForFunction(async()=>(await import('./managed-native-solo.mjs')).managedAPI.health().player?.focused,{timeout:10000});await new Promise(r=>setTimeout(r,200));
   const marked=await frame.evaluate(async()=>{const p=(await import('./managed-native-solo.mjs')).managedAPI.health().player;const ctx=document.querySelector('#canvas').getContext('2d');return {p,pixels:Array.from(ctx.getImageData(p.x-4,p.y-4,13,13).data)};});
   await canvas.screenshot({path:resolve(out,'native-focus-point.png')});
   await frame.evaluate(()=>__eaglerPC98Options().focusHitboxEnabled=false);await new Promise(r=>setTimeout(r,150));
   const hidden=await frame.evaluate(async()=>{const p=(await import('./managed-native-solo.mjs')).managedAPI.health().player;return Array.from(document.querySelector('#canvas').getContext('2d').getImageData(p.x-4,p.y-4,13,13).data);});
   assert.notDeepEqual(marked.pixels,hidden,'focus point switch removes marker');
   await frame.evaluate(()=>{__eaglerPC98Options().alwaysHitbox=true;__eaglerPC98Options().th03FocusEnabled=false;});await new Promise(r=>setTimeout(r,150));
   const always=await frame.evaluate(async()=>{const p=(await import('./managed-native-solo.mjs')).managedAPI.health().player;return {p,pixels:Array.from(document.querySelector('#canvas').getContext('2d').getImageData(p.x-4,p.y-4,13,13).data)};});
   assert.equal(always.p.focused,false);assert.notDeepEqual(always.pixels,hidden,'always point overrides focus switch');console.log('PASS native focus point switches and marker');
  }
  if(replayQa){
   await frame.evaluate(()=>{__eaglerPC98Options().th03RapidFire=false;const input=__eaglerPC98Input;let n=0;input.sample=()=>{n++;return n>300&&n%100<8?512:0;};});
   let recordedReady=false;for(let n=0;n<24;n++){await new Promise(r=>setTimeout(r,5000));const probe=await frame.evaluate(async()=>({state:(await import('./managed-native-solo.mjs')).managedAPI.health(),files:[...__eaglerPC98Files.memory.keys()]}));if(n%3===0)console.log('solo replay progress',probe);if(probe.files.some(k=>k.endsWith('.th03r'))){recordedReady=true;break;}}assert(recordedReady,'solo replay recorded');
   const recorded=await frame.evaluate(async()=>{const path=[...__eaglerPC98Files.memory.keys()].find(k=>k.endsWith('.th03r')),data=await(await import('./native-replay.mjs')).decodeReplay(__eaglerPC98Files.memory.get(path));return {path,mode:data.mode,frames:data.rows.length,native:data.native.length};});
   assert.equal(recorded.mode,'solo');assert(recorded.frames>1000&&recorded.native>0);
   await frame.evaluate(async path=>{const api=(await import('./managed-native-solo.mjs')).managedAPI;api.stop();const options={...__eaglerPC98Options(),replayViewer:true};await api.start({options,music:'none'});await api.playback({action:'open',path,stage:0});await api.playback({action:'rate',value:8});},recorded.path);
   await frame.waitForFunction(async()=>(await import('./managed-native-solo.mjs')).managedAPI.health().gameFrame>300,{timeout:30000});
   await frame.evaluate(async()=>{const api=(await import('./managed-native-solo.mjs')).managedAPI;await api.playback({action:'pause'});await api.playback({action:'seek',value:10});});
   await frame.waitForFunction(async()=>(await import('./managed-native-solo.mjs')).managedAPI.health().gameFrame===10,{timeout:30000});
   console.log('PASS native solo recording, playback, speed and seek');
  }
}catch(error){await page.screenshot({path:resolve(out,'failure.png')});console.error(await page.evaluate(()=>document.body.innerText.slice(-3500)));throw error;}
finally{await writeFile(resolve(out,'result.json'),JSON.stringify({errors,events},null,2));await browser.close();}
