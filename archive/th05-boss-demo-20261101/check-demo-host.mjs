import assert from 'node:assert/strict';
import {readFile,writeFile} from 'node:fs/promises';
import {resolve,extname} from 'node:path';
import {launchBrowser} from '../../tools/browser/launch.mjs';
const ops=import.meta.dirname,staged=process.env.TH05_PUBLIC!=='1',origin='https://test.touhou.vip',browser=await launchBrowser();
const musicRoom=true;
const profile=process.env.TH05_PHONE??'android',ios=profile==='ios',label=(staged?'preflight':'public')+'-'+profile+(musicRoom?'-music-room':'');
const page=await browser.newPage({viewport:{width:844,height:390},hasTouch:true,isMobile:true,deviceScaleFactor:1,userAgent:ios?'Mozilla/5.0 (iPhone; CPU iPhone OS 18_0 like Mac OS X) AppleWebKit/605.1.15 Version/18.0 Mobile/15E148 Safari/604.1':'Mozilla/5.0 (Linux; Android 14; Pixel 8) AppleWebKit/537.36 Chrome/130.0.0.0 Mobile Safari/537.36',serviceWorkers:'block'}),errors=[];
page.setDefaultTimeout(30000);
page.on('pageerror',error=>errors.push(error.message));
const mime={'.html':'text/html','.mjs':'text/javascript','.js':'text/javascript','.json':'application/json','.wasm':'application/wasm','.css':'text/css','.ogg':'audio/ogg','.webp':'image/webp','.svg':'image/svg+xml','.png':'image/png','.woff2':'font/woff2'};
let frame;
try{
 await page.addInitScript(()=>{localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1');window.__th05Events=[];addEventListener('message',event=>{if(event.data?.game==='th05')window.__th05Events.push(event.data);});});
 if(staged)await page.route(origin+'/**',async route=>{const path=decodeURIComponent(new URL(route.request().url()).pathname);try{const bytes=await readFile(resolve(ops,'payload-v2/site-files',path==='/'?'index.html':path.slice(1)));await route.fulfill({status:200,body:bytes,contentType:mime[extname(path)]??(path==='/'?'text/html':'application/octet-stream')});}catch(error){if(error.code!=='ENOENT'&&error.code!=='EISDIR')throw error;await route.continue();}});
 await page.goto(origin+'/',{waitUntil:'domcontentloaded'});const card=page.locator('[data-shelf=singleplayer] a[data-game=th05]');await card.click();await page.waitForTimeout(400);if(!await page.locator('#advancedOptions > summary').isVisible())await card.click();
 const toggle=async(id,value)=>{const node=page.locator('#'+id);if((await node.getAttribute('aria-checked')==='true')!==value)await node.click();};
 await toggle('touchToggle',true);await page.locator('#touchLayoutEdit').click();await toggle('thpracTouchControlsToggle',true);await page.locator('#touchLayoutSave').click();await page.locator('#touchLayoutExit').click();await page.locator('#languageSelect').evaluate(node=>{node.value='lang_zh-hans';node.dispatchEvent(new Event('change',{bubbles:true}));});
 if(!await page.locator('#advancedOptions').evaluate(node=>node.open))await page.locator('#advancedOptions > summary').click();
 await page.locator('#musicOption .mizuki-select-trigger').click();await page.locator('[role=option][data-value='+'ogg-stream'+']:visible').click();await toggle('thpracToggle',true);
 await page.locator('#launch').click();if(!musicRoom)await page.getByRole('button',{name:'仍要启动',exact:true}).click();await page.waitForFunction(()=>document.querySelector('#gameFrame')?.contentWindow?.Module?._th05_ticks()>10,null,{timeout:90000});
 frame=page.frames().find(frame=>frame.url().includes('/runtime/th05/'));assert.ok(frame);if(await page.locator('#touchHelpClose').isVisible())await page.locator('#touchHelpClose').tap();
 assert.equal(await page.locator('#touchDirectSurface').isVisible(),true,'native touch surface');
 const advance=async count=>frame.evaluate(count=>{for(let n=0;n<count;n++)Module._th05_step();},count);
 const pointer=async(type,id,x,y)=>page.locator('#touchDirectSurface').evaluate((surface,{type,id,x,y,ios})=>{
  const r=document.querySelector('#gameFrame').getBoundingClientRect(),scale=Math.min(r.width/640,r.height/480),clientX=r.left+(r.width-640*scale)/2+x*scale,clientY=r.top+(r.height-480*scale)/2+y*scale;
  if(ios){const contact=new Touch({identifier:id,target:surface,clientX,clientY});surface.dispatchEvent(new TouchEvent({down:'touchstart',move:'touchmove',up:'touchend'}[type],{bubbles:true,cancelable:true,touches:type==='up'?[]:[contact],changedTouches:[contact]}));}
  else surface.dispatchEvent(new PointerEvent({down:'pointerdown',move:'pointermove',up:'pointerup'}[type],{pointerId:id,pointerType:'touch',clientX,clientY,bubbles:true,cancelable:true,buttons:type==='up'?0:1}));
 },{type,id,x,y,ios});
 const tap=async(x,y)=>{await pointer('down',17,x,y);await advance(3);await pointer('up',17,x,y);await page.waitForTimeout(90);await advance(3);};
 console.log('launched',profile);await advance(450);assert.equal(await frame.evaluate(()=>Module._th05_scene()),2);
 await page.waitForTimeout(12000);const demo=await frame.evaluate(()=>({scene:Module._th05_scene(),input:Module._th05_input_scene(),ticks:Module._th05_ticks()}));assert.equal(demo.input,14,JSON.stringify(demo));await page.waitForTimeout(5000);const continued=await frame.evaluate(()=>({scene:Module._th05_scene(),input:Module._th05_input_scene(),ticks:Module._th05_ticks()}));assert.equal(continued.input,14);const audio=await frame.evaluate(()=>Module.eaglerAudio.health());assert.equal(audio.mode,'ogg');assert.equal(audio.decodeMode,'stream');assert.equal(audio.playing,true);await tap(350,300);await advance(100);assert.equal(await frame.evaluate(()=>Module._th05_scene()),2);const result={passed:true,staged,profile,demo,continued,audio,errors};assert.deepEqual(errors,[]);await writeFile(resolve(ops,'public-cold-demo.json'),JSON.stringify(result,null,2)+'\n');console.log(JSON.stringify(result));
}catch(error){console.error(error);throw error;}finally{await browser.close();}
