import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const output=fileURLToPath(new URL('../../artifacts/sdl3/browser/mobile/',import.meta.url));mkdirSync(output,{recursive:true});
const {server,url}=await presentationServer(),browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']}),context=await browser.newContext({viewport:{width:844,height:390},isMobile:true,hasTouch:true,deviceScaleFactor:2}),page=await context.newPage(),errors=[],states=[];
page.on('pageerror',e=>errors.push(e.stack));page.on('console',e=>{if(e.type()==='error')errors.push(e.text());});
const state=()=>page.evaluate(()=>__th09Runtime.status());
async function tap(){await page.locator('#shoot').tap();}
try{
 await page.goto(url+'/app/th09.html');await page.locator('#start').tap();await page.waitForFunction(()=>window.__th09Runtime,{timeout:120000});await page.waitForFunction(()=>__th09Runtime.status().title[0]>30);assert.equal(await page.locator('#touch').isVisible(),true);await page.screenshot({path:output+'title.png'});
 await tap();await page.waitForFunction(()=>__th09Runtime.status().title[2]===2&&__th09Runtime.status().title[3]===1);await tap();await page.waitForFunction(()=>__th09Runtime.status().title[2]===3&&__th09Runtime.status().title[3]===1);await tap();await page.waitForFunction(()=>!__th09Runtime.status().title[1]);
 await page.evaluate(()=>__th09Runtime.core._th09_key(29,1));await page.waitForFunction(()=>__th09Runtime.status().touch[0]===1);await page.evaluate(()=>__th09Runtime.core._th09_key(29,0));
 await page.waitForFunction(()=>__th09Runtime.status().session[1]>260);await page.screenshot({path:output+'battle.png'});states.push({name:'battle',state:await state()});
 const moved=await page.evaluate(()=>{const r=__th09Runtime,c=r.core;c._th09_loop_pause(1);c._th09_touch_controls(1,1,0,0,0);c._th09_touch(0,71,.35,.7);c._th09_touch(1,71,.2,.65);for(let i=0;i<18;++i)c._th09_game_tick(0);const moving=r.status();c._th09_touch(2,71,.2,.65);const stopped=r.status();c._th09_game_draw();return {moving,stopped};});assert.equal(moved.moving.touch[2],1);assert.equal(moved.stopped.touch[2],0);
 await page.evaluate(()=>__th09Runtime.core._th09_loop_pause(0));await page.locator('#pause').tap();await page.waitForFunction(()=>__th09Runtime.status().touch[0]===0);await page.screenshot({path:output+'pause.png'});
 await page.setViewportSize({width:390,height:844});await page.screenshot({path:output+'portrait.png'});assert.equal(errors.length,0,errors.join('\n'));writeFileSync(output+'report.json',JSON.stringify({passed:true,physicalPhone:false,states,moved,errors},null,2));console.log(JSON.stringify({passed:true,physicalPhone:false,states,moved}));
}catch(e){await page.screenshot({path:output+'failure.png'}).catch(()=>{});console.error(e);writeFileSync(output+'failure.json',JSON.stringify({error:e.stack,states,errors,live:await state().catch(()=>null)},null,2));process.exitCode=1;}
finally{await browser.close();await new Promise(resolve=>server.close(resolve));}
