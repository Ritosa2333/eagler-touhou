import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {presentationServer} from '../../scripts/presentation-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const output=fileURLToPath(new URL('../../artifacts/sdl3/browser/campaign/',import.meta.url));mkdirSync(output,{recursive:true});
const {server,url}=await presentationServer(),browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']}),page=await browser.newPage({viewport:{width:700,height:620}}),errors=[],states=[];
page.on('pageerror',e=>errors.push(e.stack));page.on('console',e=>{if(e.type()==='error')errors.push(e.text());});
try{
 await page.goto(url);await page.evaluate(()=>openProbe(0,1,0,1));
 for(let stage=0;stage<9;++stage){
  const state=await page.evaluate(expected=>{let budget=0;while(probeStatus()[3]>=0||probeStatus()[5]===5){step(257);if(++budget>2500)throw Error('Dialogue did not finish');}if(sessionStatus()[5]!==expected)throw Error('Wrong stage');core._th09_probe_end_round(0);const old=sessionStatus()[5];for(let n=0;n<2200&&sessionStatus()[0]===1&&sessionStatus()[5]===old;++n)step(257);return {game:probeStatus(),session:sessionStatus()};},stage);
  states.push({stage,...state});await page.screenshot({path:output+'after-stage-'+(stage+1)+'.png'});assert.equal(state.session[0],stage===8?4:1);if(stage<8)assert.equal(state.session[5],stage+1);
 }
 for(const [name,frames,keys] of [['ending-text',150,0],['ending-skip',150,257],['staff',2000,257]]){const state=await page.evaluate(([n,k])=>{for(let i=0;i<n;++i){step(k);if(titleStatus()[1])break;}return {title:titleStatus(),session:sessionStatus()};},[frames,keys]);states.push({name,...state});await page.screenshot({path:output+name+'.png'});}
 assert.equal((await page.evaluate(()=>titleStatus()))[1],1,'Return from complete ending to results');assert.equal(errors.length,0,errors.join('\n'));
 writeFileSync(output+'report.json',JSON.stringify({passed:true,physicalPhone:false,forcedVictories:true,states,errors},null,2));console.log(JSON.stringify({passed:true,states,errors}));
}catch(e){await page.screenshot({path:output+'failure.png'}).catch(()=>{});writeFileSync(output+'failure.json',JSON.stringify({error:e.stack,states,errors},null,2));console.error(e);process.exitCode=1;}
finally{await browser.close();await new Promise(resolve=>server.close(resolve));}
