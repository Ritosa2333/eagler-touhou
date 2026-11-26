import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {productionBrowser,observeCadence,runtimeCommand,wait} from './production-driver.mjs';
import {replayFixture} from './replay-fixture.mjs';
const high=process.argv.includes('--high'),url=process.env.THKKD_SITE_URL||'http://127.0.0.1:8177/',output=fileURLToPath(new URL('../build/quality-mp/playback-cadence-'+(high?'high':'60')+'/',import.meta.url));mkdirSync(output,{recursive:true});
const report={schema:'thkkd/playback-cadence/1',high},driver=await productionBrowser({url,report,unlimited:high}),{browser,page}=driver;
try{
 await driver.library();if(high){if(await page.$eval('#displayOptions > button',element=>element.getAttribute('aria-expanded'))!=='true')await page.click('#displayOptions > button');await page.click('#frameLimitToggle');}
 const frame=await driver.start();await runtimeCommand(page,'write',{path:'replay/cadence.rpy',bytes:[...replayFixture({count:1200,bombFrame:-1})]});await runtimeCommand(page,'playback',{action:'open',path:'replay/cadence.rpy'});await runtimeCommand(page,'playback',{action:'pause'});await runtimeCommand(page,'playback',{action:'seek',value:70});await frame.waitForFunction(()=>__thkkdRuntime.core._thkkd_word(0xb43110)===70);await wait(1100);
 report.paused=await observeCadence(frame);assert.equal(report.paused.steps,0);assert.equal(report.paused.after.cursor,70);
 report.seeking=await observeCadence(frame,1600,()=>runtimeCommand(page,'playback',{action:'seek',value:1000}));assert.ok(report.seeking.steps>0);await frame.waitForFunction(()=>__thkkdRuntime.core._thkkd_word(0xb43110)===1000);report.seekFinal=1000;
 for(const sample of [report.paused,report.seeking]){if(high)assert.ok(sample.presentHz>75);else assert.ok(sample.presentHz>54&&sample.presentHz<66);assert.ok(Math.abs(parseFloat(sample.renderedFPS)-sample.presentHz)<Math.max(10,sample.presentHz*.2));}
 const telemetry=await driver.collect();assert.equal(telemetry.filter(event=>event.event==='error'||event.event==='audio-health'&&['blocked','error'].includes(event.state)).length,0);assert.equal(report.errors.length,0);await page.screenshot({path:output+'paused.png'});report.result='passed';console.log(JSON.stringify({result:report.result,high,paused:report.paused.presentHz,seeking:report.seeking.presentHz,seekSteps:report.seeking.steps}));
}catch(error){report.result='failed';report.error=error.stack;process.exitCode=1;console.error(error);await page.screenshot({path:output+'failure.png'});}
finally{await driver.collect();writeFileSync(output+'result.json',JSON.stringify(report,null,2));await browser.close();}
