import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const output=fileURLToPath(new URL('../../artifacts/sdl3/browser/replay-save/',import.meta.url));mkdirSync(output,{recursive:true});
const {server,url,netplay}=await presentationServer(),browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']}),page=await browser.newPage({viewport:{width:700,height:620}}),errors=[],states=[];
page.on('pageerror',e=>errors.push(e.stack));page.on('console',e=>{if(e.type()==='error')errors.push(e.text());});
try{
 await page.goto(url);await page.evaluate(()=>openProbe(0,1,2,1));
 const tick=async(n,k=0)=>page.evaluate(([n,k])=>{for(let i=0;i<n;++i)step(k);return {game:probeStatus(),session:sessionStatus(),title:titleStatus()};},[n,k]);const press=async k=>{await tick(1,k);await tick(2);};
 await tick(300,257);await page.evaluate(()=>core._th09_probe_end_round(0));await tick(350,257);await page.evaluate(()=>core._th09_probe_end_round(0));await tick(350,257);assert.equal((await tick(1)).session[0],3);await tick(30);await press(32);await press(32);await press(1);await tick(50);states.push(await tick(1));assert.equal(states.at(-1).title[2],15);await tick(20);await press(1);await tick(20);assert.equal((await tick(1)).title[3],4);await press(1);await tick(20);
 const saved=await page.evaluate(()=>Array.from(core.FS.readFile('/save/replay/th9_01.rpy')));assert.ok(saved.length>192);writeFileSync(output+'th9_01.rpy',Uint8Array.from(saved));
 // Re-enter the actual replay list and choose the saved match, then pause it.
 for(let n=0;n<2;++n)await press(32);await press(1);await tick(20);assert.equal((await tick(1)).title[2],11);await press(1);await tick(10);await press(1);await tick(60);assert.equal((await tick(1)).session[6],1);await press(8);await tick(30);assert.equal((await tick(1)).game[9],1);await page.screenshot({path:output+'saved-replay.png'});assert.equal(errors.length,0,errors.join('\n'));writeFileSync(output+'report.json',JSON.stringify({passed:true,forcedRoundEnds:true,bytes:saved.length,states,errors},null,2));console.log(JSON.stringify({passed:true,bytes:saved.length,states}));
}catch(e){await page.screenshot({path:output+'failure.png'}).catch(()=>{});writeFileSync(output+'failure.json',JSON.stringify({error:e.stack,states,errors,status:await page.evaluate(()=>({game:probeStatus(),session:sessionStatus(),title:titleStatus()})).catch(()=>null)},null,2));console.error(e);process.exitCode=1;}
finally{await browser.close();netplay.close();await new Promise(resolve=>server.close(resolve));}
