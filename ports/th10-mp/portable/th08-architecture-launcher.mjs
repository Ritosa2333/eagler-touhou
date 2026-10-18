import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const out='th08_web/artifacts/architecture-candidate/validation';mkdirSync(out,{recursive:true});
const browser=await launchBrowser(),context=await browser.newContext({viewport:{width:844,height:390},isMobile:true,hasTouch:true}),page=await context.newPage(),errors=[],http=[];
page.on('pageerror',e=>errors.push(e.stack));page.on('response',r=>{if(r.status()>=400)http.push([r.status(),r.url()]);});
try{
 await page.goto('http://127.0.0.1:8095/');await page.locator('#changelogClose').click();await page.locator('.game[data-game="th08"]').click();await page.locator('#launch').click();
 await page.waitForFunction(()=>document.querySelector('#gameFrame')?.contentWindow?.__th08Runtime?.app,null,{timeout:120000});const frame=page.frames().find(f=>f.url().includes('/runtime/th08/'));
 await frame.waitForFunction(()=>__th08Runtime.status()[9]>330,null,{timeout:30000});
 const key=async code=>{await page.keyboard.press(code,{delay:80});await page.waitForTimeout(180);};
 for(let i=0;i<10;i++){const cursor=await frame.evaluate(()=>__th08Runtime.core.status(__th08Runtime.app,9));if(cursor===6)break;await key('ArrowDown');}
 await frame.evaluate(()=>{window.measured=[];const original=Module.runtimeFinish;Module.runtimeFinish=(r,ms)=>{measured.push(ms);original(r,ms);};});await key('KeyZ');await page.waitForTimeout(2200);
 const music=await frame.evaluate(()=>({state:__th08Runtime.status(),maxFrameMs:Math.max(...measured),audio:Array.from(new Uint32Array(__th08Runtime.core.memory.buffer,__th08Runtime.core.sdl_audio_stats(),12))}));assert.equal(music.state[0],8);assert.equal(music.state[2],0);await page.screenshot({path:out+'/music-mobile.png'});
 await key('Escape');await page.waitForTimeout(1200);
 const saved=await frame.evaluate(async()=>{const r=__th08Runtime;await Promise.all([r.command({command:'sync'}),r.command({command:'sync'})]);return r.command({command:'read',path:'score.dat'});});assert(saved.bytes.length>100);
 const invalid=await frame.evaluate(async()=>{try{await __th08Runtime.command({command:'write',path:'../game/th08.dat',bytes:[1]});return false;}catch{return true;}});assert(invalid);
 await frame.evaluate(()=>__th08Runtime.stop());await page.reload();await context.setOffline(true);await page.reload();assert(await page.locator('.game[data-game="th08"]').count());
 assert.equal(errors.length,0,errors.join('\n'));assert.equal(http.length,0,JSON.stringify(http));writeFileSync(out+'/launcher.json',JSON.stringify({passed:true,music,saveBytes:saved.bytes.length,offline:true,errors,http},null,2));console.log(JSON.stringify({passed:true,music,saveBytes:saved.bytes.length,offline:true}));
}catch(error){writeFileSync(out+'/launcher-failure.json',JSON.stringify({error:error.stack,errors,http,body:await page.locator('body').innerText()},null,2));await page.screenshot({path:out+'/launcher-failure.png'});throw error;}finally{await browser.close();}
