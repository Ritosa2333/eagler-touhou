import assert from 'node:assert/strict';
import {readFileSync,mkdirSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {productionBrowser,nativeState} from './production-driver.mjs';

const args=Object.fromEntries(process.argv.slice(2).map(value=>{const at=value.indexOf('=');return [value.slice(2,at),value.slice(at+1)];}));
assert.ok(args.profile,'指定旧版专用测试配置的副本');
const output=resolve(args.output||'ports/thkkd/build/installed-resource');mkdirSync(output,{recursive:true});
const report={schema:'thkkd/installed-resource/1',reference:args.reference||'previous-install',checks:[]};
const driver=await productionBrowser({url:args.url,profile:args.profile,report}),{page,browser}=driver;
await page.evaluateOnNewDocument(()=>{const NativeWorker=Worker;window.__loadedVideoWorkers=[];window.Worker=class extends NativeWorker{constructor(...args){super(...args);const send=this.postMessage.bind(this);this.postMessage=(message,...rest)=>{if(message?.type==='LOAD'&&message.data?.coreURL)__loadedVideoWorkers.push(message.data);return send(message,...rest);};}};});
try{
  await driver.library();
  const expectedBuildId=JSON.parse(readFileSync(new URL('../runtime/manifest.json',import.meta.url),'utf8')).buildId;
  const frame=await driver.start({expectedBuildId});
  report.state=await nativeState(frame);assert.equal(report.state.error,'');
  report.media=await frame.evaluate(()=>({assets:__thkkdRuntime.platform.assets.size,videos:[...__thkkdRuntime.platform.assets.values()].filter(x=>x.file.endsWith('.webm')).length}));
  assert.deepEqual(report.media,{assets:301,videos:4});
  report.videoWorkers=await page.evaluate(()=>__loadedVideoWorkers);assert.deepEqual(report.videoWorkers,[]);
  await driver.library();await driver.start({expectedBuildId});assert.deepEqual(await page.evaluate(()=>__loadedVideoWorkers),[]);
  assert.deepEqual(report.errors,[]);report.checks.push('previous-install-starts-with-current-runtime','301-existing-assets-and-four-webm','refresh-without-video-core-load');report.result='passed';
}catch(error){report.result='failed';report.error=String(error.stack||error);process.exitCode=1;console.error(error);}
finally{await driver.collect();writeFileSync(resolve(output,'result.json'),JSON.stringify(report,null,2));await browser.close();}
