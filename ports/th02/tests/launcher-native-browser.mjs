// Run against the Eagler dev server with --native-build DATA and Runtime.
import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {findChromiumExecutable} from '../../../launcher/lib/chromium-executable.mjs';

const require=createRequire(new URL('../../../launcher/package.json',import.meta.url));
const {launch}=require('puppeteer-core');
const url=new URL(process.env.PC98_LAUNCHER_URL || 'http://127.0.0.1:18320/');
url.searchParams.set('test','th02');
const browser=await launch({executablePath:await findChromiumExecutable(),headless:true});
try {
  const page=await browser.newPage();
  await page.setViewport({width:1360,height:900});
  const errors=[];
  page.on('pageerror',error=>errors.push(error.message));
  await page.evaluateOnNewDocument(()=>{
    localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1');
    window.runtimeEvents=[];
    addEventListener('message',event=>{
      if(event.origin===location.origin&&event.data?.protocol==='eagler-touhou/1'&&event.data.game==='th02')runtimeEvents.push(event.data);
    });
  });
  await page.goto(url.href);
  await page.waitForFunction(()=>window.__eaglerBoot?.done===true,{timeout:45000});
  await page.locator('a[data-book-product="th02"]').click();
  await page.waitForFunction(()=>document.querySelector('#launch')?.disabled===false);
  await page.locator('#launch').click();
  await page.waitForFunction(()=>{
    document.querySelector('#decisionDialog[open] #decisionConfirm')?.click();
    document.querySelector('#touchHelpClose')?.click();
    const error=runtimeEvents.find(e=>e.event==='error');if(error)throw Error(error.error);
    return runtimeEvents.some(e=>e.event==='first-frame');
  },{timeout:90000,polling:150}).catch(async error=>{
    const state=await page.evaluate(()=>({events:runtimeEvents,body:document.body.innerText.slice(-2400),iframe:document.querySelector('#gameFrame')?.src}));
    throw Error(JSON.stringify({state,errors}),{cause:error});
  });
  const events=()=>page.evaluate(()=>window.runtimeEvents);
  assert((await events()).some(e=>e.event==='runtime-info'&&e.renderer==='SoEW Native'));
  const frame=page.frames().find(f=>/th02\.html.*runtimeEpoch=/.test(f.url()));
  assert(frame);
  await frame.click('#canvas');
  const press=(key,delay=45)=>page.keyboard.press(key,{delay});
  for(let i=0;i<5&&!/ショット選択|choose shot/u.test(await frame.title());i++) {
    await press('Enter',130);
    await new Promise(done=>setTimeout(done,400));
  }
  await frame.waitForFunction(()=>/ショット選択|choose shot/u.test(document.title));
  await new Promise(done=>setTimeout(done,700));
  await press('Enter');
  await frame.waitForFunction(()=>/一時停止|Esc: Pause/u.test(document.title));
  await frame.waitForFunction(()=>{
    const c=document.querySelector('canvas'),ctx=c.getContext('2d');
    const pixels=(x,y,w,h)=>ctx.getImageData(x*c.width/640,y*c.height/400,w*c.width/640,h*c.height/400).data;
    const colored=(red,i,rgba)=>i%4===0&&(red!==rgba[i+1]||red!==rgba[i+2]);
    return pixels(550,375,82,17).some((v,i)=>i%4!==3&&v>240)&&
      pixels(32,32,380,200).some(colored)&&pixels(432,32,144,200).some(colored);
  });
  await press('ArrowLeft',200);
  await page.waitForFunction(()=>runtimeEvents.some(e=>e.event==='audio-health'&&e.state==='running'));
  await press('Escape');
  await new Promise(done=>setTimeout(done,120));
  await press('KeyQ');
  await frame.waitForFunction(()=>!(/一時停止|Esc: Pause/u.test(document.title)));
  await new Promise(done=>setTimeout(done,700));
  await press('ArrowUp');
  await press('Enter');
  await page.waitForFunction(()=>runtimeEvents.some(e=>e.event==='exit'&&e.code===0));
  await page.waitForFunction(()=>![...document.querySelectorAll('iframe')].some(el=>el.src.includes('runtimeEpoch=')));
  assert.deepEqual((await events()).filter(e=>e.event==='error'),[]);
  assert.deepEqual(errors,[]);
  console.log('PASS Eagler TH02 install, SoEW Native title, Stage 1, audio and return');
} finally {
  await browser.close();
}
