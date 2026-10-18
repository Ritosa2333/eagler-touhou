import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const output=fileURLToPath(new URL('../../artifacts/sdl3/browser/title/',import.meta.url));mkdirSync(output,{recursive:true});
const {server,url}=await presentationServer(),browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']}),page=await browser.newPage({viewport:{width:700,height:620}}),errors=[],states=[];
page.on('pageerror',e=>errors.push(e.stack));page.on('console',e=>{if(e.type()==='error')errors.push(e.text());});
try{
 await page.goto(url);await page.evaluate(()=>openProbe(0,1,0,1,true));
 const tick=async(frames,keys=0)=>page.evaluate(([n,k])=>{tickMany(n,k);return titleStatus();},[frames,keys]);
 const press=async(keys)=>{await tick(1,keys);await tick(2);};
 const image=async name=>{states.push({name,title:await page.evaluate(()=>titleStatus()),game:await page.evaluate(()=>probeStatus())});await page.screenshot({path:output+name+'.png'});};
 await tick(40);await image('main');
 for(let n=0;n<4;++n)await press(32);await press(1);await tick(35);assert.equal((await tick(1))[2],12);await image('music');
 await press(32);await press(1);await tick(20);await image('music-locked');await press(8);await tick(35);
 await press(16);await press(1);await tick(35);await image('rankings');await press(8);await tick(35);
 for(let n=0;n<3;++n)await press(16);await press(1);await tick(20);await image('difficulty');await press(1);await tick(20);await image('characters');await press(1);await tick(60);await image('story-start');
 assert.equal((await page.evaluate(()=>titleStatus()))[1],0);assert.equal((await page.evaluate(()=>probeStatus()))[4],0);
 await tick(480,257);await image('battle');await press(8);await tick(35);await image('pause');
 assert.equal(errors.length,0,errors.join('\n'));writeFileSync(output+'report.json',JSON.stringify({passed:true,physicalPhone:false,states,errors},null,2));console.log(JSON.stringify({passed:true,states,errors}));
}catch(e){await page.screenshot({path:output+'failure.png'}).catch(()=>{});writeFileSync(output+'failure.json',JSON.stringify({error:e.stack,states,errors},null,2));console.error(e);process.exitCode=1;}
finally{await browser.close();await new Promise(resolve=>server.close(resolve));}
