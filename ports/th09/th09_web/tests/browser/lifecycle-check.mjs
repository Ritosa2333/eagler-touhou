import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const output=fileURLToPath(new URL('../../artifacts/sdl3/browser/lifecycle/',import.meta.url));mkdirSync(output,{recursive:true});
const {server,url}=await presentationServer(),browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']}),context=await browser.newContext({viewport:{width:700,height:620}}),page=await context.newPage(),errors=[],states=[];
page.on('pageerror',e=>errors.push(e.stack));
try{
 let failures=0;await page.route('**/resources.json',async route=>{if(!failures++){await route.fulfill({status:503,body:'Injected resource failure'});return;}await route.continue();});
 let transientFailures=0;await page.route('**/music/th09_01.ogg*',async route=>{if(transientFailures++<2){await route.fulfill({status:502,body:'Injected transient tunnel failure'});return;}await route.continue();});
 await page.goto(url+'/app/th09.html');await page.locator('#start').click();await page.waitForFunction(()=>!document.querySelector('#error').hidden);assert.equal(await page.locator('#start').isEnabled(),true);
 await page.locator('#start').click();await page.waitForFunction(()=>window.__th09Runtime,{timeout:120000});await page.waitForFunction(()=>__th09Runtime.status().title[0]>30);assert.equal(await page.locator('#error').isVisible(),false);
 await page.evaluate(()=>{const c=__th09Runtime.core;c._th09_loop_pause(1);for(let n=0;n<1600&&__th09Runtime.status().title[1];++n)if(!c._th09_probe_tick(0))throw Error('demo start failed');});
 assert.equal((await page.evaluate(()=>__th09Runtime.status())).session[6],1);
 const interrupted=await page.evaluate(()=>{const c=__th09Runtime.core;if(!c._th09_probe_tick(16))throw Error('demo exit failed');for(let n=0;n<24;++n)c._th09_probe_tick(0);return __th09Runtime.status();});assert.equal(interrupted.title[1],1);assert.equal(interrupted.title[2],1);states.push({name:'demoInterrupted',state:interrupted});
 const muted=await page.evaluate(async()=>{const r=__th09Runtime,c=r.core,config=c.FS.readFile('/save/th09.cfg');config[0xae]=0;await r.importFile('th09.cfg',config);c._th09_probe_tick(8);c._th09_probe_tick(0);c._th09_probe_tick(1);for(let n=0;n<61;++n)c._th09_probe_tick(0);const p=c._th09_probe_audio()/4;return {audio:Array.from(c.HEAPU32.subarray(p,p+8)),title:r.status().title};});assert.equal(muted.audio[7],0);assert.equal(muted.title[7],0);
 await page.evaluate(()=>__th09Runtime.core._th09_loop_pause(0));await page.waitForFunction(()=>!document.querySelector('#welcome').hidden);await page.locator('#start').click();await page.waitForFunction(()=>__th09Runtime.status().title[1]&&__th09Runtime.status().title[3]===1&&__th09Runtime.status().title[4]===0);assert.equal(await page.locator('#error').isVisible(),false);assert.equal(errors.length,0,errors.join('\n'));assert.ok(transientFailures>=3);
 await page.screenshot({path:output+'restart.png'});writeFileSync(output+'report.json',JSON.stringify({passed:true,assetRetry:true,transientFailures,demoInterrupt:true,exitRestart:true,muted,states,errors},null,2));console.log(JSON.stringify({passed:true,muted,states}));
}catch(e){await page.screenshot({path:output+'failure.png'}).catch(()=>{});writeFileSync(output+'failure.json',JSON.stringify({error:e.stack,errors,state:await page.evaluate(()=>window.__th09Runtime?.status()).catch(()=>null)},null,2));console.error(e);process.exitCode=1;}
finally{await browser.close();await new Promise(resolve=>server.close(resolve));}
