import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {releaseServer} from '../../scripts/release-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const output=fileURLToPath(new URL('../../artifacts/sdl-release/verification/',import.meta.url));mkdirSync(output,{recursive:true});
const run=await releaseServer({root:fileURLToPath(new URL('../../artifacts/sdl-release/site',import.meta.url)),port:0}),browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']}),context=await browser.newContext({viewport:{width:844,height:390},isMobile:true,hasTouch:true,deviceScaleFactor:2}),page=await context.newPage(),errors=[],states=[];
page.on('pageerror',e=>errors.push(e.stack));page.on('console',e=>{if(e.type()==='error')errors.push(e.text());});
const launch=async()=>{await page.goto(run.url+'/app/th09.html');await page.locator('#start').tap();await page.waitForFunction(()=>window.__th09Runtime,null,{timeout:120000});await page.waitForFunction(()=>__th09Runtime.status().title[0]>30);};
try{
 await launch();assert.equal(await page.locator('#touch').isVisible(),true);await page.evaluate(()=>navigator.serviceWorker.ready);assert.equal(await page.evaluate(()=>typeof __th09Runtime.core._th09_probe_end_round),'undefined');
 await page.evaluate(async()=>{const r=__th09Runtime,cfg=r.core.FS.readFile('/save/th09.cfg');cfg[0xba]=47;await r.importFile('th09.cfg',cfg);await r.save();});await launch();assert.equal(await page.evaluate(()=>__th09Runtime.core.FS.readFile('/save/th09.cfg')[0xba]),47);
 await page.locator('#shoot').tap();await page.waitForFunction(()=>__th09Runtime.status().title[2]===2&&__th09Runtime.status().title[3]===1);await page.locator('#shoot').tap();await page.waitForFunction(()=>__th09Runtime.status().title[2]===3&&__th09Runtime.status().title[3]===1);await page.locator('#shoot').tap();await page.waitForFunction(()=>!__th09Runtime.status().title[1]);
 await page.evaluate(()=>__th09Runtime.core._th09_key(29,1));await page.waitForFunction(()=>__th09Runtime.status().touch[0]===1);await page.evaluate(()=>__th09Runtime.core._th09_key(29,0));await page.waitForFunction(()=>__th09Runtime.status().session[1]>300);
 states.push(await page.evaluate(()=>__th09Runtime.status()));await page.screenshot({path:output+'mobile-battle.png'});
 const closed=await page.evaluate(()=>{const r=__th09Runtime,before=r.status();r.netplay.close();return {before,after:r.status()};});assert.deepEqual(closed.before,closed.after);
 await page.locator('#pause').tap();await page.waitForFunction(()=>__th09Runtime.status().touch[0]===0);await page.screenshot({path:output+'mobile-pause.png'});await page.setViewportSize({width:390,height:844});await page.screenshot({path:output+'portrait.png'});
 await page.locator('#storage-open').tap();await page.waitForFunction(()=>document.querySelector('#storage').open);assert.ok(await page.locator('#files button').count());await page.locator('#storage-close').tap();
 const audio=await page.evaluate(()=>({state:__th09Runtime.core.SDL3?.audioContext?.state,rate:__th09Runtime.core.SDL3?.audioContext?.sampleRate}));assert.equal(audio.state,'running');assert.equal(errors.length,0,errors.join('\n'));writeFileSync(output+'browser.json',JSON.stringify({passed:true,version:run.manifest.version,physicalPhone:false,releaseExports:true,storageReload:true,audio,states,errors},null,2));console.log(JSON.stringify({passed:true,version:run.manifest.version,audio,states}));
}catch(e){await page.screenshot({path:output+'failure.png'}).catch(()=>{});writeFileSync(output+'failure.json',JSON.stringify({error:e.stack,states,errors,state:await page.evaluate(()=>window.__th09Runtime?.status()).catch(()=>null)},null,2));console.error(e);process.exitCode=1;}
finally{await browser.close();run.netplay.close();await new Promise(r=>run.server.close(r));}
