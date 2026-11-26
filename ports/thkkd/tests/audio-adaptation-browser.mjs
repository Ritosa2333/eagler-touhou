import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync,readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {productionBrowser,wait} from './production-driver.mjs';

const args=Object.fromEntries(process.argv.slice(2).map(argument=>{const i=argument.indexOf('=');return [argument.slice(2,i),argument.slice(i+1)];}));
const output=resolve(args.output||'ports/thkkd/build/adaptation-20261127/audio');mkdirSync(output,{recursive:true});
const report={schema:'thkkd/audio-adaptation-browser/1',checks:[]};
const mp3=readFileSync(new URL('../assets/bgm/bgm00.mp3',import.meta.url));
const driver=await productionBrowser({url:args.url||'http://127.0.0.1:19027/',profile:args.profile,report});const {browser,page}=driver;
try{
  await page.setBypassServiceWorker(true);await page.setCacheEnabled(false);await driver.library();
  const frame=await driver.start({expectedBuildId:JSON.parse(readFileSync(new URL('../runtime/manifest.json',import.meta.url),'utf8')).buildId});
  let delayedRequests=0;
  await page.setRequestInterception(true);
  page.on('request',request=>{
    if(request.url().endsWith('/__thkkd-delayed-bgm.mp3')){delayedRequests++;void wait(1500).then(()=>request.respond({status:200,contentType:'audio/mpeg',body:mp3})).catch(()=>{});}
    else void request.continue();
  });
  report.latency=await frame.evaluate(async()=>{
    const p=__thkkdRuntime.platform,asset=p.assets.get('bgm/bgm00.mp3'),name='bgm/adaptation-delayed.mp3';
    p.assets.set(name,{...asset,url:location.origin+'/__thkkd-delayed-bgm.mp3'});
    const handle=p.call(0x4434d0,[],[name],new Uint8Array()),sound=p.sounds.get(handle);
    const effect=[...p.sounds.values()].find(value=>!value.bgm&&value.buffer?.duration>.05);
    if(!effect)throw Error('No native sound effect loaded');
    const analyser=p.context.createAnalyser();analyser.fftSize=2048;p.sfxGain.connect(analyser);await p.resume();
    p.playSound(sound,2,1);const musicReadyBefore=sound.audio.readyState,started=performance.now();p.playSound(effect,0,1);
    const samples=new Float32Array(analyser.fftSize);let peak=0,delay;
    for(let i=0;i<100;i++){analyser.getFloatTimeDomainData(samples);peak=Math.max(peak,...samples.map(Math.abs));if(peak>1e-6){delay=performance.now()-started;break;}await new Promise(resolve=>setTimeout(resolve,10));}
    const musicReadyAtEffect=sound.audio.readyState;
    for(let i=0;i<400&&sound.audio.paused;i++)await new Promise(resolve=>setTimeout(resolve,10));
    const resumed=!sound.audio.paused;
    p.sfxGain.disconnect(analyser);analyser.disconnect();p.call(0x43e010,[handle],[],new Uint8Array());p.assets.delete(name);
    return {delay,peak,musicReadyBefore,musicReadyAtEffect,resumed};
  });
  assert.equal(delayedRequests,1);assert.equal(report.latency.musicReadyBefore,0);assert.equal(report.latency.musicReadyAtEffect,0);
  assert.ok(report.latency.peak>0&&report.latency.delay<250);assert.equal(report.latency.resumed,true);
  report.checks.push('AUDIO-07-native-SFX-produces-PCM-while-real-MP3-request-is-pending');
  report.errorsAndRecovery=await frame.evaluate(async()=>{
    const r=__thkkdRuntime,p=r.platform,asset=p.assets.get('bgm/bgm00.mp3'),name='bgm/adaptation-invalid.mp3';
    const badURL=URL.createObjectURL(new Blob([new Uint8Array(256)],{type:'audio/mpeg'}));p.assets.set(name,{...asset,url:badURL});
    const handle=p.call(0x4434d0,[],[name],new Uint8Array()),sound=p.sounds.get(handle);p.playSound(sound,2,1);
    for(let i=0;i<200&&!sound.failure;i++)await new Promise(resolve=>setTimeout(resolve,10));
    const failed=p.audioHealth();await new Promise(resolve=>setTimeout(resolve,2200));const retained=p.audioHealth();
    sound.audio.src=asset.url;sound.audio.load();p.playSound(sound,2,1);
    for(let i=0;i<400&&(sound.audio.paused||sound.failure);i++)await new Promise(resolve=>setTimeout(resolve,10));
    const recovered=!sound.audio.paused&&!sound.failure;
    p.call(0x43e010,[handle],[],new Uint8Array());p.assets.delete(name);URL.revokeObjectURL(badURL);
    const missing=p.call(0x4434d0,[],['bgm/adaptation-missing.mp3'],new Uint8Array()),missingHealth=p.audioHealth();
    p.music=false;const silentHealth=p.audioHealth();p.music=true;p.missingMusic.delete('bgm/adaptation-missing.mp3');
    return {failed,retained,recovered,missing,missingHealth,silentHealth,launched:r.status().launched,nativeError:document.querySelector('#error').textContent,assets:p.assets.size};
  });
  const error=report.errorsAndRecovery;assert.equal(error.failed.state,'error');assert.equal(error.retained.state,'error');assert.ok(error.recovered);
  assert.equal(error.missing,-1);assert.equal(error.missingHealth.state,'error');assert.equal(error.silentHealth.state,'running');assert.equal(error.launched,true);assert.equal(error.nativeError,'');assert.equal(error.assets,301);
  report.checks.push('AUDIO-05-corrupt-MP3-diagnostic-persists-through-periodic-health','AUDIO-05-MP3-playback-recovers-after-resource-restoration','AUDIO-05-missing-music-keeps-game-running-and-supports-no-music');
  await driver.collect();assert.equal(report.errors.length,0);report.result='passed';console.log(JSON.stringify({result:report.result,checks:report.checks,latency:report.latency}));
}catch(error){report.result='failed';report.error=String(error.stack||error);process.exitCode=1;console.error(error);await page.screenshot({path:resolve(output,'failure.png')});}
finally{writeFileSync(resolve(output,'result.json'),JSON.stringify(report,null,2));await browser.close();}
