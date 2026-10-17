import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const output=fileURLToPath(new URL('../../artifacts/sdl3/browser/performance/',import.meta.url));mkdirSync(output,{recursive:true});
const {server,url,netplay}=await presentationServer(),browser=await launchBrowser(),page=await browser.newPage({viewport:{width:700,height:620}}),errors=[];
page.on('pageerror',e=>errors.push(e.stack));
try{
 await page.goto(url);await page.evaluate(()=>openProbe(0,1,0,1));
 const metrics=await page.evaluate(async()=>{const timing=[],metrics=()=>{const p=core._th09_game_metrics()/4;return Array.from(core.HEAPU32.subarray(p,p+10));};const before=metrics();for(let stage=0;stage<9;++stage){let maximum=0,transition=0,total=0;for(let n=0;n<600;++n){const t=performance.now();step((n%8<4?1:0)|(n<220?256:0));const dt=performance.now()-t;total+=dt;maximum=Math.max(maximum,dt);if(n%8===7)await new Promise(requestAnimationFrame);}core._th09_probe_end_round(0);for(let n=0;n<1500&&sessionStatus()[5]===stage&&sessionStatus()[0]===1;++n){const t=performance.now();step(257);transition=Math.max(transition,performance.now()-t);if(n%8===7)await new Promise(requestAnimationFrame);}timing.push({stage,mean:total/600,maximum,transition});}return {before,after:metrics(),timing,memory:core.HEAPU8.length};});
 assert.equal(metrics.after[2],0,'No GPU pixel readbacks');assert.equal(metrics.after[6],0,'No buffer sub updates');assert.ok(metrics.after[5]>0,'Streaming buffer replacement');assert.equal(errors.length,0,errors.join('\n'));writeFileSync(output+'report.json',JSON.stringify({passed:true,physicalPhone:false,gpu:process.env.NATIVE_GPU==='1'?'host hardware':'software WebGL',metrics,errors},null,2));console.log(JSON.stringify({passed:true,metrics}));
}catch(e){console.error(e);process.exitCode=1;}
finally{await browser.close();netplay.close();await new Promise(resolve=>server.close(resolve));}
