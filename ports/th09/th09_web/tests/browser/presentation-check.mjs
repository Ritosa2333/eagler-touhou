import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const output=fileURLToPath(new URL('../../artifacts/sdl3/browser/',import.meta.url));mkdirSync(output,{recursive:true});
const {server,url}=await presentationServer(),browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']}),page=await browser.newPage({viewport:{width:700,height:600}}),errors=[];
page.on('pageerror',e=>errors.push(e.stack));page.on('console',e=>{if(e.type()==='error')errors.push(e.text());});
try{
 await page.goto(url);await page.evaluate(()=>openProbe(0,1,0,1));
 const states=[];for(const [name,frames,keys] of [['entrance',60,0],['dialogue',180,0],['dialogue-skip',40,257],['dialogue-visible',60,0],['skip-finish',300,257],['battle',420,1],['pause',1,8],['pause-open',40,0]]){
  const state=await page.evaluate(([frames,keys])=>tickMany(frames,keys),[frames,keys]);states.push({name,state});await page.screenshot({path:output+name+'.png'});
 }
 const audio=await page.evaluate(()=>Array.from(core.HEAPU32.subarray(core._th09_probe_audio()/4,core._th09_probe_audio()/4+8)));
 assert.equal(audio[3],0,'audio pump error');assert.equal(errors.length,0,errors.join('\n'));
 const report={passed:true,physicalPhone:false,states,audio,errors};writeFileSync(output+'report.json',JSON.stringify(report,null,2));console.log(JSON.stringify(report));
}catch(e){await page.screenshot({path:output+'failure.png'}).catch(()=>{});writeFileSync(output+'failure.json',JSON.stringify({error:e.stack,errors},null,2));console.error(e);process.exitCode=1;}
finally{await browser.close();await new Promise(resolve=>server.close(resolve));}
