// Full Launcher, Package Store and native TH01 runtime (requires scripts/dev.mjs).
import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {findChromiumExecutable} from '../../../launcher/lib/chromium-executable.mjs';
const require=createRequire(new URL('../../../launcher/package.json',import.meta.url)),puppeteer=require('puppeteer-core');
const base=process.env.PC98_LAUNCHER_URL||'http://127.0.0.1:18310/';
const out=resolve(import.meta.dirname,'../.cache/launcher-browser');await mkdir(out,{recursive:true});
const browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling']});
const page=await browser.newPage();await page.setViewport({width:1360,height:900});
const errors=[],events=[];page.on('pageerror',e=>{errors.push(e.message);console.error('PAGE',e.message);});
page.on('console',m=>{if(m.type()==='error')console.error('CONSOLE',m.text());});
await page.exposeFunction('recordRuntimeEvent',m=>events.push(m));
await page.evaluateOnNewDocument(()=>{addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1')window.recordRuntimeEvent(e.data);});});
try{
  await page.goto(base+'?test=th01');
  await page.waitForFunction(()=>window.__eaglerBoot?.done===true,{timeout:45000});
  await page.waitForSelector('.game[data-game="th01"]',{timeout:45000});
  await page.evaluate(()=>document.querySelector('#firstUseNoticeDialog')?.close());
  console.log('Launcher TH01 card ready');
  await page.click('.game[data-game="th01"]:not([data-product])');
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
  const frame=page.frames().find(f=>/th01.html.*runtimeEpoch=/.test(f.url()));
  const health=()=>frame.evaluate(async()=>(await import('./app.js')).managedAPI.health());
  await new Promise(resolve=>setTimeout(resolve,2000));
  assert.equal((await health()).menu,1,'title remains open without input');
  async function checkRatio(){
    const box=await frame.$eval('#game',node=>{const r=node.getBoundingClientRect();return {x:r.x,y:r.y,width:r.width,height:r.height,viewportWidth:innerWidth,viewportHeight:innerHeight};});
    assert(Math.abs(box.width/box.height-1.6)<.01,'game canvas retains 640:400 aspect ratio');
    assert(box.x>=-1&&box.y>=-1&&box.x+box.width<=box.viewportWidth+1&&box.y+box.height<=box.viewportHeight+1,'game canvas fits the viewport');
  }
  await checkRatio();
  await page.setViewport({width:480,height:800});await checkRatio();
  await page.setViewport({width:1360,height:900});
  if(!await page.evaluate(()=>!!document.fullscreenElement))await page.click('#fullscreenToggle');
  await page.waitForFunction(()=>!!document.fullscreenElement,{timeout:5000});
  await checkRatio();await page.screenshot({path:resolve(out,'solo.png')});
  await frame.$eval('#game',node=>node.focus());await page.keyboard.press('KeyZ',{delay:100});
  await frame.waitForFunction(async()=> (await import('./app.js')).managedAPI.health().menu===0,{timeout:5000});
  assert.deepEqual(errors,[]);
  console.log('PASS full Launcher Package Store installation and TH01 solo presentation');
}catch(error){await page.screenshot({path:resolve(out,'failure.png')});console.error(await page.evaluate(()=>document.body.innerText.slice(-3500)));throw error;}
finally{await writeFile(resolve(out,'result.json'),JSON.stringify({errors,events},null,2));await browser.close();}

