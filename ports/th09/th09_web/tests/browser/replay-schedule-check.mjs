import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync,readFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const output=fileURLToPath(new URL('../../artifacts/sdl3/browser/replay-schedule/',import.meta.url));mkdirSync(output,{recursive:true});
const run=await presentationServer(),browser=await launchBrowser(),contexts=await Promise.all([browser.newContext(),browser.newContext()]),pages=await Promise.all(contexts.map(c=>c.newPage())),errors=[];
for(const p of pages)p.on('pageerror',e=>errors.push(e.stack));
try{
 const bytes=Array.from(readFileSync(new URL('../../reference/assets/demorpy0.rpy',import.meta.url)));
 for(const p of pages){await p.goto(run.url+'/app/th09.html');await p.locator('#start').click();await p.waitForFunction(()=>window.__th09Runtime,null,{timeout:120000});await p.evaluate(async bytes=>{const r=__th09Runtime,c=r.core;c._th09_loop_pause(1);await r.importFile('demo.rpy',Uint8Array.from(bytes));const tick=(n,k=0)=>{for(let i=0;i<n;++i)if(!c._th09_probe_tick(k))throw Error('Failed step');};const press=k=>{tick(1,k);tick(2);};tick(30);press(32);press(32);press(1);tick(20);press(128);press(1);tick(10);press(1);},bytes);assert.equal((await p.evaluate(()=>__th09Runtime.status())).session[6],1);}
 const accelerated=await pages[0].evaluate(async()=>{const r=__th09Runtime,c=r.core,deltas=[];for(let n=0;n<1000;++n){const before=r.status().session[2];if(!c._th09_game_tick(1))throw Error('Failed scheduled replay');deltas.push(r.status().session[2]-before);if(n%8===7)await new Promise(requestAnimationFrame);}return {ticks:deltas.length,frames:r.status().session[2],hash:c._th09_network_hash()>>>0,deltas:[...new Set(deltas)],state:r.status()};});
 const normal=await pages[1].evaluate(async frames=>{const r=__th09Runtime,c=r.core;while(r.status().session[2]<frames){if(!c._th09_probe_tick(0))throw Error('Failed reference replay');if(r.status().session[2]%8===7)await new Promise(requestAnimationFrame);}return {frames:r.status().session[2],hash:c._th09_network_hash()>>>0,state:r.status()};},accelerated.frames);
 assert.ok(accelerated.deltas.some(n=>n>1),'Original dialogue accelerates replay');assert.ok(accelerated.deltas.every(n=>n>=1&&n<=3));assert.equal(accelerated.hash,normal.hash);assert.equal(accelerated.frames,normal.frames);assert.equal(errors.length,0,errors.join('\n'));writeFileSync(output+'report.json',JSON.stringify({passed:true,accelerated,normal,errors},null,2));console.log(JSON.stringify({passed:true,accelerated,normal}));
}catch(e){writeFileSync(output+'failure.json',JSON.stringify({error:e.stack,errors,states:await Promise.all(pages.map(p=>p.evaluate(()=>window.__th09Runtime?.status()).catch(()=>null)))},null,2));console.error(e);process.exitCode=1;}
finally{await browser.close();run.netplay.close();await new Promise(r=>run.server.close(r));}
