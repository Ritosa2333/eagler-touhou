import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {productionBrowser,observeCadence,runtimeCommand,wait} from './production-driver.mjs';
const source=process.argv[2];if(!source)throw Error('请提供已有授权录像文件');const target=Number(process.argv[3]||20000),bytes=readFileSync(source),url=process.env.THKKD_SITE_URL||'http://127.0.0.1:8177/',output=fileURLToPath(new URL('../build/quality-mp/performance/',import.meta.url));mkdirSync(output,{recursive:true});
const report={schema:'thkkd/performance/1',input:{sha256:createHash('sha256').update(bytes).digest('hex'),bytes:bytes.length},target},driver=await productionBrowser({url,report,protocolTimeout:660000}),{browser,page}=driver;
try{
 await driver.library();const frame=await driver.start();await runtimeCommand(page,'write',{path:'replay/performance.rpy',bytes:[...bytes]});await runtimeCommand(page,'playback',{action:'open',path:'replay/performance.rpy'});await runtimeCommand(page,'playback',{action:'pause'});await runtimeCommand(page,'playback',{action:'seek',value:target});
 await frame.waitForFunction(target=>__thkkdRuntime.core._thkkd_word(0xb43110)===target,{timeout:600000},target);
 await runtimeCommand(page,'playback',{action:'play'});await wait(1000);
 await frame.waitForFunction(()=>{const w=address=>__thkkdRuntime.core._thkkd_word(address);let active=0;for(let i=0;i<3000;i++)if(w(0xa8e660+i*240))active++;return active>=200;},{timeout:30000,polling:100});
 report.scene=await frame.evaluate(()=>{const r=__thkkdRuntime,w=address=>r.core._thkkd_word(address),gl=r.platform.renderer.gl,debug=gl.getExtension('WEBGL_debug_renderer_info');return {stage:w(0xb3e2ec),difficulty:w(0xa7e610),activeProjectiles:Array.from({length:3000},(_,index)=>w(0xa8e660+index*240)!==0).filter(Boolean).length,commands:r.platform.renderer.commands.length,renderer:debug?gl.getParameter(debug.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER),memory:r.core.HEAPU8.byteLength};});
 await frame.evaluate(()=>{const r=__thkkdRuntime,step=r.core._thkkd_step;window.__qualityStepTimes=[];r.core._thkkd_step=(...args)=>{const start=performance.now(),result=step(...args);__qualityStepTimes.push(performance.now()-start);return result;};});
 report.cadence=await observeCadence(frame,3000);await runtimeCommand(page,'playback',{action:'pause'});report.stepTime=await frame.evaluate(()=>{const values=__qualityStepTimes.sort((a,b)=>a-b);return {count:values.length,mean:values.reduce((a,b)=>a+b,0)/values.length,p95:values[Math.floor((values.length-1)*.95)],max:values.at(-1)};});
 report.finalState=await frameState(page);assert.equal(report.finalState.scene,0,'测量期间保持原生游戏场景');
 assert.ok(report.cadence.logicHz>54&&report.cadence.logicHz<66,'密集片段保持实际 60 Hz 逻辑');assert.ok(report.cadence.presentHz>54&&report.cadence.presentHz<66,'密集片段保持实际 60 Hz 呈现');assert.equal(report.errors.length,0);await page.screenshot({path:output+'scene.png'});report.result='passed';console.log(JSON.stringify({result:report.result,scene:report.scene,logicHz:report.cadence.logicHz,presentHz:report.cadence.presentHz,stepTime:report.stepTime}));
}catch(error){report.result='failed';report.error=error.stack;report.failedState=await frameState(page).catch(()=>null);process.exitCode=1;console.error(error);await page.screenshot({path:output+'failure.png'});}
finally{await driver.collect();writeFileSync(output+'result.json',JSON.stringify(report,null,2));await browser.close();}
function frameState(page){return page.evaluate(()=>{const w=address=>document.querySelector('#gameFrame').contentWindow.__thkkdRuntime.core._thkkd_word(address);let active=0;for(let i=0;i<3000;i++)if(w(0xa8e660+i*240))active++;return {cursor:w(0xb43110),scene:w(0xa78e2c),frame:w(0xb3e2e8),active};});}
