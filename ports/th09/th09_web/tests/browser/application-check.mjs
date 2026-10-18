import assert from 'node:assert/strict';
import {readFileSync,mkdirSync,writeFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const output=fileURLToPath(new URL('../../artifacts/sdl3/browser/application/',import.meta.url));mkdirSync(output,{recursive:true});
const {server,url}=await presentationServer(),browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']}),context=await browser.newContext({viewport:{width:700,height:620}}),page=await context.newPage(),errors=[],states=[];
page.on('pageerror',e=>errors.push(e.stack));page.on('console',e=>{if(e.type()==='error')errors.push(e.text());});
const launch=async()=>{await page.goto(url+'/app/th09.html');await page.locator('#start').click();await page.waitForFunction(()=>window.__th09Runtime,{timeout:120000});await page.waitForFunction(()=>__th09Runtime.status().title[0]>30);};
try{
 await launch();states.push({name:'title',state:await page.evaluate(()=>__th09Runtime.status())});await page.screenshot({path:output+'title.png'});
 const storage=await page.evaluate(async()=>{const r=__th09Runtime;r.core._th09_loop_pause(1);await r.save();const fs=r.core.FS,score=Array.from(fs.readFile('/save/score.dat')),cfg=Array.from(fs.readFile('/save/th09.cfg'));cfg[0xba]=36;cfg[0xbb]=52;await r.importFile('th09.cfg',Uint8Array.from(cfg));await r.importFile('score.dat',Uint8Array.from(score));return {score,cfg};});
 const replay=readFileSync(new URL('../../reference/assets/demorpy0.rpy',import.meta.url));await page.evaluate(bytes=>__th09Runtime.importFile('demo.rpy',Uint8Array.from(bytes)),Array.from(replay));
 const files=await page.evaluate(()=>__th09Runtime.listFiles());assert.ok(files.some(f=>f.name==='th9_ud0001.rpy'));
 const invalid=await page.evaluate(async()=>{try{await __th09Runtime.importFile('bad.rpy',new Uint8Array(512));return false;}catch{return true;}});assert.equal(invalid,true);
 await launch();const restored=await page.evaluate(()=>({score:Array.from(__th09Runtime.core.FS.readFile('/save/score.dat')),cfg:Array.from(__th09Runtime.core.FS.readFile('/save/th09.cfg')),files:__th09Runtime.listFiles()}));assert.deepEqual(restored.cfg,storage.cfg);assert.ok(restored.files.some(f=>f.name==='th9_ud0001.rpy'));assert.deepEqual(restored.score,storage.score);
 await page.keyboard.press('KeyZ',{delay:70});await page.waitForFunction(()=>__th09Runtime.status().title[2]===2);await page.waitForFunction(()=>__th09Runtime.status().title[3]===1);await page.keyboard.press('KeyZ',{delay:70});await page.waitForFunction(()=>__th09Runtime.status().title[2]===3);await page.waitForFunction(()=>__th09Runtime.status().title[3]===1);await page.keyboard.press('KeyZ',{delay:70});await page.waitForFunction(()=>__th09Runtime.status().title[1]===0);
 await page.keyboard.down('ControlLeft');await page.waitForFunction(()=>__th09Runtime.status().touch[0]===1,{timeout:30000});await page.keyboard.up('ControlLeft');await page.screenshot({path:output+'battle.png'});
 const single=await page.evaluate(()=>{const r=__th09Runtime;const before=r.status();r.netplay.close();return {before,after:r.status()};});assert.equal(single.before.title[1],0);assert.deepEqual(single.after,single.before,'Inactive network close must not reset single-player');
 await page.keyboard.press('Escape',{delay:70});await page.waitForFunction(()=>__th09Runtime.status().touch[0]===0);states.push({name:'paused',state:await page.evaluate(()=>__th09Runtime.status())});await page.screenshot({path:output+'pause.png'});
 await page.locator('#storage-open').click();await page.waitForFunction(()=>document.querySelector('#storage').open);await page.screenshot({path:output+'storage.png'});await page.locator('#storage-close').click();
 const cache=await page.evaluate(async()=>({names:await caches.keys(),entries:(await(await caches.open('th09-native-resources-v1')).keys()).length}));assert.equal(cache.entries,23);assert.equal(errors.length,0,errors.join('\n'));
 writeFileSync(output+'report.json',JSON.stringify({passed:true,physicalPhone:false,storageReload:true,invalidReplayRejected:true,files,cache,states,errors},null,2));console.log(JSON.stringify({passed:true,files,cache,states}));
}catch(e){await page.screenshot({path:output+'failure.png'}).catch(()=>{});writeFileSync(output+'failure.json',JSON.stringify({error:e.stack,states,errors,live:await page.evaluate(()=>window.__th09Runtime?.status()).catch(()=>null)},null,2));console.error(e);process.exitCode=1;}
finally{await browser.close();await new Promise(resolve=>server.close(resolve));}
