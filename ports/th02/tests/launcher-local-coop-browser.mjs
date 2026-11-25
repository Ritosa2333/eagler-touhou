// Run against Eagler with native TH02 DATA. Pass --boss-gauge to reach the Stage 1 midboss.
import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {findChromiumExecutable} from '../../../launcher/lib/chromium-executable.mjs';

const require=createRequire(new URL('../../../launcher/package.json',import.meta.url));
const {launch}=require('puppeteer-core');
const url=new URL(process.env.PC98_LAUNCHER_URL || 'http://localhost:18324/?test=th02');
const browser=await launch({executablePath:await findChromiumExecutable(),headless:true});
try {
  const page=await browser.newPage();
  await page.setViewport({width:1360,height:900});
  const errors=[];
  page.on('pageerror',error=>errors.push(error.message));
  const failedNavigations=[];
  page.on('requestfailed',request=>{
    if(request.isNavigationRequest())failedNavigations.push({url:request.url(),error:request.failure()?.errorText});
  });
  await page.evaluateOnNewDocument(()=>{
    localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1');
    window.runtimeEvents=[];
    addEventListener('message',event=>{
      if(event.origin===location.origin&&event.data?.protocol==='eagler-touhou/1'&&event.data.game==='th02')runtimeEvents.push(event.data);
    });
  });
  await page.goto(url.href);
  await page.waitForFunction(()=>window.__eaglerBoot?.done===true,{timeout:45000});
  if(await page.$('meta[name="eagler-react-app-shell"]')){
    // A warm App Shell enforces its embedder policy on live Runtime HTML.
    await page.waitForFunction(async()=>(
      await navigator.serviceWorker.getRegistration()
    )?.active?.state==='activated',{timeout:45000});
    assert((await page.reload()).fromServiceWorker(),'Expected the cached App Shell after reload');
    await page.waitForFunction(()=>window.__eaglerBoot?.done===true,{timeout:45000});
  }
  if(url.searchParams.get('game')!=='th02')await page.locator('a[data-book-product="th02"]').click();
  for(const count of [2,3]){
    if(!await page.$('#th02LocalCoop[open]'))await page.locator('#th02LocalCoop summary').click();
    await page.evaluate(()=>{runtimeEvents.length=0;});
    await page.select('#th02LocalCoop [name="players"]',String(count));
    await page.locator('#th02LocalCoopStart').click();
    await page.waitForFunction(()=>{
      document.querySelector('#decisionDialog[open] #decisionConfirm')?.click();
      document.querySelector('#touchHelpClose')?.click();
      return runtimeEvents.some(e=>e.event==='first-frame'||e.event==='error');
    },{timeout:90000,polling:150}).catch(async error=>{
      const state=await page.evaluate(()=>({events:runtimeEvents,ui:document.body.innerText.slice(-1000)}));
      throw Error(JSON.stringify({state,errors,failedNavigations,frames:page.frames().map(frame=>frame.url())}),{cause:error});
    });
    const events=await page.evaluate(()=>runtimeEvents);
    assert.deepEqual(events.filter(e=>e.event==='error'),[]);
    assert(events.some(e=>e.event==='runtime-info'&&e.renderer==='SoEW Native'));
    const frame=page.frames().find(f=>f.url().includes('localCoop='));
    assert.equal(new URL(frame.url()).searchParams.get('localCoop'),[count,0,1,2,3,0,1,2].join(','));
    await frame.click('#canvas');
    await frame.waitForFunction(()=>/一時停止|Esc: Pause/u.test(document.title));
    await frame.waitForFunction(count=>{
      const canvas=document.querySelector('canvas'),ctx=canvas.getContext('2d');
      const hasLimit=(x,y)=>{
        const scale=canvas.width/640,ys=canvas.height/400;
        const pixels=ctx.getImageData(Math.floor(x*scale),Math.floor(y*ys),Math.ceil(9*scale),Math.ceil(7*ys)).data;
        return pixels.some((r,i)=>i%4===0&&r===192&&pixels[i+1]===192&&pixels[i+2]===192);
      };
      for(let seat=0;seat<count;seat++){
        const y=149+seat*48;
        for(const x of [472,536])if(!hasLimit(x,y))return false;
      }
      return true;
    },{timeout:5000},count);

    const positions=()=>frame.evaluate(()=>{
      const c=document.querySelector('canvas'),ctx=c.getContext('2d'),scale=c.width/640;
      const left=Math.round(32*scale),top=Math.round(336*c.height/400),width=Math.round(384*scale);
      const pixels=ctx.getImageData(left,top,width,Math.round(30*c.height/400)).data;
      const colors=[new Set([0x2244aa,0x4488ff,0x66bbff]),new Set([0x117744,0x33cc77,0x55dd99])];
      return colors.map(set=>{
        let sum=0,seen=0;
        for(let i=0;i<pixels.length;i+=4)if(set.has(pixels[i]*65536+pixels[i+1]*256+pixels[i+2])){
          sum+=(left+(i/4)%width)/scale;seen++;
        }
        return seen?sum/seen:null;
      });
    });
    let original;
    for(let i=0;i<40;i++){
      original=await positions();
      if(original[0]>240&&(count===2||original[1]>208))break;
      await new Promise(done=>setTimeout(done,100));
    }
    assert(original[0]>240,JSON.stringify(original));
    if(count===3)assert(original[1]>208&&original[0]-original[1]>70,JSON.stringify(original));
    else assert.equal(original[1],null);
    await page.keyboard.down('KeyD');
    if(count===3)await page.keyboard.down('Numpad4');
    await new Promise(done=>setTimeout(done,600));
    await page.keyboard.up('KeyD');
    if(count===3)await page.keyboard.up('Numpad4');
    const moved=await positions();
    assert(moved[0]>original[0]+26,JSON.stringify({original,moved}));
    if(count===3)assert(moved[1]!==null&&moved[1]<original[1]-20,JSON.stringify({original,moved}));
    if(count===3&&process.argv.includes('--boss-gauge')){
      await frame.waitForFunction(()=>{
        const canvas=document.querySelector('canvas'),ctx=canvas.getContext('2d');
        const sx=canvas.width/640,sy=canvas.height/400;
        const pixels=ctx.getImageData(Math.round(448*sx),Math.round(320*sy),Math.round(128*sx),Math.round(16*sy)).data;
        let lit=0;
        for(let i=0;i<pixels.length;i+=4)if(pixels[i]||pixels[i+1]||pixels[i+2])lit++;
        return lit>pixels.length/8;
      },{timeout:110000,polling:250});
      console.log('PASS Eagler TH02 Stage 1 midboss gauge appears in the original multiplayer rows');
    }
    await page.keyboard.press('Escape',{delay:100});
    await page.keyboard.press('KeyQ',{delay:100});
    await page.waitForFunction(()=>runtimeEvents.some(e=>e.event==='exit'&&e.code===0),{timeout:12000});
    await page.waitForFunction(()=>![...document.querySelectorAll('iframe')].some(el=>el.src.includes('localCoop=')));
    assert.deepEqual(await page.evaluate(()=>runtimeEvents.filter(e=>e.event==='error')),[]);
    console.log('PASS Eagler TH02 Native local co-op, '+count+' seats, independent movement and exit');
  }
  assert.deepEqual(errors,[]);
} finally {
  await browser.close();
}
