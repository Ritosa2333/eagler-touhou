import {mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {productionBrowser,nativeState,runtimeCommand,wait} from './production-driver.mjs';

const args=Object.fromEntries(process.argv.slice(2).map(a=>{const at=a.indexOf('=');return [a.slice(2,at),a.slice(at+1)];}));
if(!args.replay)throw Error('需要 --replay=已有授权 EX 录像');
const output=resolve(args.output||fileURLToPath(new URL('../build/performance-profile',import.meta.url)));mkdirSync(output,{recursive:true});
const report={schema:'thkkd/cpu-profile/1',samples:[]},d=await productionBrowser({url:args.url||'http://127.0.0.1:18201/',report,protocolTimeout:700000});
let frame;
try{
 await d.library();frame=await d.start();const cdp=await d.page.createCDPSession();await cdp.send('Profiler.enable');await cdp.send('Profiler.setSamplingInterval',{interval:100});
 async function profile(name,rate,trigger){
  await d.page.emulateCPUThrottling(rate);
  await frame.evaluate(()=>{const r=__thkkdRuntime;window.__nativeProfile={steps:0,presents:0,stepMs:0,presentMs:0};if(window.__profileWrapped)return;window.__profileWrapped=true;
   const step=r.core._thkkd_step;r.core._thkkd_step=(...a)=>{const began=performance.now(),result=step(...a);__nativeProfile.stepMs+=performance.now()-began;__nativeProfile.steps++;return result;};
   const present=r.platform.renderer.present.bind(r.platform.renderer);r.platform.renderer.present=(...a)=>{const began=performance.now(),result=present(...a);__nativeProfile.presentMs+=performance.now()-began;__nativeProfile.presents++;return result;};
  });
  const before=await nativeState(frame);await cdp.send('Profiler.start');if(trigger)await trigger();await wait(4000);const {profile}=await cdp.send('Profiler.stop');
  const after=await nativeState(frame),timing=await frame.evaluate(()=>__nativeProfile);writeFileSync(resolve(output,name+'.cpuprofile'),JSON.stringify(profile));
  const row={name,rate,before,after,timing,samples:profile.samples?.length||0};report.samples.push(row);writeFileSync(resolve(output,'progress.json'),JSON.stringify(row,null,2));console.log(JSON.stringify(row));
 }
 await frame.click('canvas');for(let i=0;i<8&&(await nativeState(frame)).outer!==100;i++)await d.key('z');
 for(const rate of [1,4]){
  await d.page.emulateCPUThrottling(1);await d.page.keyboard.up('z');await d.key('Escape');await d.key('r',50,0);await frame.waitForFunction(()=>__thkkdRuntime.core._thkkd_word(0xb43110)<100&&__thkkdRuntime.core._thkkd_word(0xa78e2c)===0);await d.page.keyboard.down('z');await frame.waitForFunction(()=>__thkkdRuntime.core._thkkd_word(0xb43110)>=120);await profile('live-cpu-'+rate,rate);await d.page.keyboard.up('z');
 }
 await d.page.emulateCPUThrottling(1);const replay=readFileSync(args.replay);await runtimeCommand(d.page,'write',{path:'replay/performance-profile.rpy',bytes:[...replay]});await runtimeCommand(d.page,'playback',{action:'open',path:'replay/performance-profile.rpy'});await runtimeCommand(d.page,'playback',{action:'pause'});await runtimeCommand(d.page,'playback',{action:'seek',value:50120});await frame.waitForFunction(()=>__thkkdRuntime.core._thkkd_word(0xb43110)===50120,{timeout:650000,polling:500});
 await profile('dense-cpu-4',4,()=>runtimeCommand(d.page,'playback',{action:'play'}));await runtimeCommand(d.page,'playback',{action:'pause'});report.result='profiled';
}catch(error){report.result='failed';report.error=error.stack;console.error(error);process.exitCode=1;}
finally{await d.page.emulateCPUThrottling(1).catch(()=>{});await d.collect();writeFileSync(resolve(output,'result.json'),JSON.stringify(report,null,2));await d.browser.close();}
