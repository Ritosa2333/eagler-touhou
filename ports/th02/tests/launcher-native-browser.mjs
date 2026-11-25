// Run against the Eagler dev server with --native-build DATA and Runtime.
// Pass --practice-seed for the fixed-seed input flow.
// Pass --boss-gauge to reach the Stage 1 boss directly in solo Practice.
// Pass --about to check the disclaimer on the About page.
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
  async function openTh02() {
    await page.goto(url.href);
    await page.waitForFunction(()=>window.__eaglerBoot?.done===true,{timeout:45000});
    await page.locator('a[data-book-product="th02"]').click();
    await page.waitForFunction(()=>document.querySelector('#launch')?.disabled===false);
  }
  async function launchSoew() {
    const eventCount=await page.evaluate(()=>runtimeEvents.length);
    await page.locator('#launch').click();
    await page.waitForFunction(eventCount=>{
      document.querySelector('#decisionDialog[open] #decisionConfirm')?.click();
      document.querySelector('#touchHelpClose')?.click();
      const recent=runtimeEvents.slice(eventCount);
      const error=recent.find(e=>e.event==='error');if(error)throw Error(error.error);
      return recent.some(e=>e.event==='first-frame');
    },{timeout:90000,polling:150},eventCount).catch(async error=>{
      const state=await page.evaluate(()=>({events:runtimeEvents,body:document.body.innerText.slice(-2400),iframe:document.querySelector('#gameFrame')?.src}));
      throw Error(JSON.stringify({state,errors}),{cause:error});
    });
    const frame=page.frames().find(f=>/th02\.html.*runtimeEpoch=/.test(f.url()));
    assert(frame);
    await frame.click('#canvas');
    return frame;
  }
  const press=(key,delay=45)=>page.keyboard.press(key,{delay});
  const wait=ms=>new Promise(done=>setTimeout(done,ms));
  async function openStoryShots(frame) {
    for(let i=0;i<5&&!/ショット選択|choose shot/u.test(await frame.title());i++) {
      await press('Enter',130);
      await wait(400);
    }
    await frame.waitForFunction(()=>/ショット選択|choose shot/u.test(document.title));
    await wait(700);
  }
  await openTh02();
  const frame=await launchSoew();
  const events=()=>page.evaluate(()=>window.runtimeEvents);
  assert((await events()).some(e=>e.event==='runtime-info'&&e.renderer==='SoEW Native'));
  await openStoryShots(frame);
  const practiceSeed=process.argv.includes('--practice-seed');
  const about=process.argv.includes('--about');
  const bossGauge=process.argv.includes('--boss-gauge');
  if(about){
    await press('Escape');
    await wait(1050);
    await press('ArrowUp'); // Story → Quit → About.
    await press('ArrowUp');
    await press('Enter');
    await frame.waitForFunction(()=>{
      const c=document.querySelector('canvas'),ctx=c.getContext('2d');
      const sx=c.width/640,sy=c.height/400;
      const line=ctx.getImageData(Math.round(32*sx),Math.round(96*sy),Math.round(190*sx),Math.round(20*sy)).data;
      return line.some((v,i)=>i%4===0&&v===255&&line[i+1]===255&&line[i+2]===255);
    },{timeout:5000});
  } else if(practiceSeed||bossGauge) {
    await press('Escape'); // Story shot select → title.
    await wait(1050);
    await press('ArrowDown'); // Practice.
    await press('Enter');
    await wait(1050);
    await press('Enter'); // Shot → stage list.
    await press('Enter'); // Stage → practice setup.
    if(bossGauge){
      await press('Enter'); // Start at the selected stage's boss.
      await frame.waitForFunction(()=>{
        const c=document.querySelector('canvas'),ctx=c.getContext('2d');
        const sx=c.width/640,sy=c.height/400;
        const has=(x,y,w,h,red,green,blue)=>{
          const pixels=ctx.getImageData(Math.round(x*sx),Math.round(y*sy),Math.round(w*sx),Math.round(h*sy)).data;
          return pixels.some((v,i)=>i%4===0&&v===red&&pixels[i+1]===green&&pixels[i+2]===blue);
        };
        return has(448,112,90,10,180,220,255)&&has(546,112,30,10,180,220,255)&&
          has(448,122,8,1,0,216,255)&&has(568,131,8,1,0,216,255)&&
          has(448,133,128,10,180,220,255);
      },{timeout:15000,polling:150}).catch(async cause=>{
        throw Error(`Solo practice boss HUD not detected: ${await frame.title()}`,{cause});
      });
    }else{
      await press('ArrowUp');
      await press('ArrowUp'); // Start → New Seed → Enter Seed.
      for(const digit of '8675309')await press(digit);
      await press('Enter');
      await press('ArrowDown');
      await press('ArrowDown');
      await press('Enter');
      await frame.waitForFunction(()=>/シード 8675309|Seed 8675309/u.test(document.title),{timeout:10000}).catch(async cause=>{
        throw Error(`Practice did not launch with 8675309: ${await frame.title()}`,{cause});
      });
    }
  } else {
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
    await wait(120);
    await press('KeyQ');
    await frame.waitForFunction(()=>!(/一時停止|Esc: Pause/u.test(document.title)));
    await wait(700);
    await press('ArrowUp');
    await press('Enter');
    await page.waitForFunction(()=>runtimeEvents.some(e=>e.event==='exit'&&e.code===0));
    await page.waitForFunction(()=>![...document.querySelectorAll('iframe')].some(el=>el.src.includes('runtimeEpoch=')));
  }
  assert.deepEqual((await events()).filter(e=>e.event==='error'),[]);
  assert.deepEqual(errors,[]);
  console.log(about?'PASS Eagler TH02 renders the unofficial-project line on About':
    bossGauge?'PASS Eagler TH02 solo Practice shows the full-width cyan bar and clock':
    practiceSeed?'PASS Eagler TH02 fixed-seed practice launch':'PASS Eagler TH02 install, SoEW Native title, Stage 1, audio and return');
} finally {
  await browser.close();
}
