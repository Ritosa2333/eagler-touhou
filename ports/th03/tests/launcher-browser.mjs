// Full Launcher, Package Store and native TH03 runtime (requires scripts/dev.mjs).
import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {findChromiumExecutable} from '../../../launcher/lib/chromium-executable.mjs';
const require=createRequire(new URL('../../../launcher/package.json',import.meta.url)),puppeteer=require('puppeteer-core');
const base=process.env.PC98_LAUNCHER_URL||'http://127.0.0.1:18330/';
const out=resolve(import.meta.dirname,'../.cache/launcher-browser');await mkdir(out,{recursive:true});
const browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling']});
const page=await browser.newPage();await page.setViewport({width:1360,height:900});
const errors=[],events=[];page.on('pageerror',e=>{errors.push(e.message);console.error('PAGE',e.message);});
page.on('console',m=>{if(m.type()==='error')console.error('CONSOLE',m.text());});
await page.exposeFunction('recordRuntimeEvent',m=>events.push(m));
await page.evaluateOnNewDocument(()=>{
  addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1')window.recordRuntimeEvent(e.data);});
  window.__keyProbe=[];
  const post=Worker.prototype.postMessage;
  Worker.prototype.postMessage=function(message,...rest){
    if(message?.method==='key')window.__keyProbe.push(message.args[0]);
    return post.call(this,message,...rest);
  };
});
try{
  await page.goto(base+'?test=th03');
  await page.waitForFunction(()=>window.__eaglerBoot?.done===true,{timeout:45000});
  await page.waitForSelector('.game[data-game="th03"]',{timeout:45000});
  await page.evaluate(()=>document.querySelector('#firstUseNoticeDialog')?.close());
  console.log('Launcher TH03 card ready');
  await page.click('.game[data-game="th03"]:not([data-product])');
  await page.click('.game[data-game="th03"]:not([data-product])');
  await page.waitForSelector('.tools.is-open',{visible:true});
  await page.$eval('.tools.is-open',async panel=>{await Promise.all(panel.getAnimations().map(animation=>animation.finished.catch(()=>{})));});
  await page.waitForSelector('#musicSelect');
  console.log('music',await page.$$eval('#musicSelect option',nodes=>nodes.map(n=>({value:n.value,disabled:n.disabled}))));
  const music=await page.$$eval('#musicSelect option',nodes=>nodes.find(n=>!n.disabled&&n.value!=='none')?.value);
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
  await new Promise(resolve=>setTimeout(resolve,1500));
  assert.equal((await frame.evaluate(()=>window.__keyProbe)).filter(event=>event.code==='KeyZ'&&event.type==='keydown').length,0,'startup must not press Z');
  await page.keyboard.down('ArrowDown');
  await new Promise(resolve=>setTimeout(resolve,500));
  await page.keyboard.up('ArrowDown');
  assert((await frame.evaluate(()=>window.__keyProbe)).some(event=>event.code==='ArrowDown'&&event.type==='keydown'),'game must receive keyboard input');
  const musicDeadline=Date.now()+45000;
  while(Date.now()<musicDeadline&&!events.some(event=>event.event==='audio-health'&&event.music))await new Promise(resolve=>setTimeout(resolve,250));
  assert(events.some(event=>event.event==='audio-health'&&event.musicStatus?.startsWith('原版 FM：')),'original PMD must play while recording');
  assert(events.filter(event=>event.event==='audio-health'&&event.state==='running').every(event=>event.queuedMs<250),'native sound queue must remain short');
  assert.equal((await frame.evaluate(()=>window.__keyProbe)).filter(event=>event.code==='KeyZ'&&event.type==='keydown').length,0,'title must not auto-confirm');
  for(let i=0;i<8&&!events.some(event=>event.event==='audio-health'&&event.effectsPlayed>0);i++){
    await page.keyboard.down('KeyZ');await new Promise(resolve=>setTimeout(resolve,200));await page.keyboard.up('KeyZ');
    await new Promise(resolve=>setTimeout(resolve,1500));
  }
  assert(events.some(event=>event.event==='audio-health'&&event.effectsPlayed>0),'game effects must play through the independent audio path');
  await page.screenshot({path:resolve(out,'solo.png')});
  assert.deepEqual(errors,[]);
  console.log('PASS full Launcher Package Store installation and TH03 solo presentation');
}catch(error){await page.screenshot({path:resolve(out,'failure.png')});console.error(await page.evaluate(()=>document.body.innerText.slice(-3500)));throw error;}
finally{await writeFile(resolve(out,'result.json'),JSON.stringify({errors,events},null,2));await browser.close();}
