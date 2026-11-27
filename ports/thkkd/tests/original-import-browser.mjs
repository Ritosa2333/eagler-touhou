import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {productionBrowser,nativeState,wait} from './production-driver.mjs';
const args=Object.fromEntries(process.argv.slice(2).map(value=>{const at=value.indexOf('=');return [value.slice(2,at),value.slice(at+1)];}));
if(!args.zip&&!(args.dat&&args.bgm))throw Error('请指定 --zip=原版资源ZIP，或 --dat=dat.dxa --bgm=bgm.dxa');
const url=args.url||'http://127.0.0.1:18180/',output=resolve(args.output||'ports/thkkd/build/original-import');mkdirSync(output,{recursive:true});
const report={schema:'thkkd/original-import-browser/1',checks:[],progress:[]};
const driver=await productionBrowser({url,report,protocolTimeout:1200000,profile:args.profile}),{page,browser,key}=driver;
const coreRequests=[];
page.on('request',request=>{if(/ffmpeg-core.*\.(wasm|js)/.test(request.url()))coreRequests.push(request.url());});
browser.on('targetcreated',async target=>{if(target.type()!=='other')return;try{const session=await target.createCDPSession();await session.send('Network.enable');session.on('Network.requestWillBeSent',event=>{if(/ffmpeg-core.*\.(wasm|js)/.test(event.request.url))coreRequests.push(event.request.url);});}catch{}});
await page.evaluateOnNewDocument(()=>{const NativeWorker=Worker;window.__importWorkers=[];window.Worker=class extends NativeWorker{constructor(...args){super(...args);const row={url:String(args[0]),terminated:false};__importWorkers.push(row);const stop=this.terminate.bind(this);this.terminate=()=>{row.terminated=true;return stop();};}};});
try {
  await driver.library();
  assert.equal(coreRequests.length,0,'资源库正常浏览按需加载视频内核');
  if(args.controlled==='1'){
    await page.evaluate(()=>navigator.serviceWorker.ready);
    await driver.library();
    report.environment=await page.evaluate(()=>({controlled:!!navigator.serviceWorker.controller,isolated:crossOriginIsolated}));
    assert.ok(report.environment.controlled&&report.environment.isolated,'在离线缓存接管且启用隔离的页面中导入');
    report.checks.push('import-after-service-worker-controlled-reload');
  }
  const buttons=await page.$$('button');
  for(const button of buttons)if(await button.evaluate(node=>['Import game resources','导入游戏资源','Start game','开始游戏','启动游戏'].includes(node.textContent.trim()))){await button.click();break;}
  await page.waitForSelector('#gameDataImportWindow',{visible:true,timeout:60000});
  const hint=await page.$eval('#gameDataImportReason',node=>node.textContent);
  assert.match(hint,/dat\.dxa/);assert.match(hint,/bgm\.dxa/);assert.doesNotMatch(hint,/WAV|RAR|7z/);
  await wait(400);await page.screenshot({path:resolve(output,'import-dialog.png')});
  const [chooser]=await Promise.all([page.waitForFileChooser(),page.click('#transferImport')]);
  assert.equal(chooser.isMultiple(),true);await chooser.accept(args.zip?[resolve(args.zip)]:[resolve(args.dat),resolve(args.bgm)]);
  report.input=args.zip?'original-zip':'two-original-dxa';report.checks.push('file-picker-and-format-guidance');
  if(args.interrupt==='1'){
    await page.waitForFunction(()=>/back100\.wmv.*\d+%/.test(document.querySelector('#gameDataImportBusyText')?.textContent||''),{timeout:120000});
    report.interruptedProgress=await page.$eval('#gameDataImportBusyText',node=>node.textContent);
    await driver.library();
    for(const button of await page.$$('button'))if(await button.evaluate(node=>['Import game resources','导入游戏资源'].includes(node.textContent.trim()))){await button.click();break;}
    await page.waitForSelector('#gameDataImportWindow',{visible:true});
    const [retry]=await Promise.all([page.waitForFileChooser(),page.click('#transferImport')]);
    await retry.accept(args.zip?[resolve(args.zip)]:[resolve(args.dat),resolve(args.bgm)]);
    report.checks.push('page-interruption-leaves-no-partial-install-and-retry');
  }
  let last='',finished=false,started=false;const start=Date.now();
  while(Date.now()-start<1200000){
    const state=await page.evaluate(()=>({first:!!document.querySelector('#gameFrame')?.contentWindow?.__thkkdRuntime?.status().first,text:document.querySelector('#gameDataImportBusyText')?.textContent||'',error:document.querySelector('#gameDataImportReason')?.textContent||'',busy:!document.querySelector('#gameDataImportBusy')?.hidden}));
    if(state.first){finished=true;break;}
    if(state.text!==last){last=state.text;report.progress.push({seconds:(Date.now()-start)/1000,text:last});console.log(last);}
    if(Date.now()-start>5000&&!state.busy&&await page.$('#gameDataImportWindow[data-state="open"]'))throw Error(state.error||'资源导入结束但未进入游戏');
    if(!state.busy&&!started){
      for(const button of await page.$$('button'))if(await button.evaluate(node=>['Start game','开始游戏','启动游戏'].includes(node.textContent.trim()))){
        report.installedSeconds=(Date.now()-start)/1000;await button.click();started=true;break;
      }
    }
    await wait(1000);
  }
  assert.ok(finished,'原版资源导入并进入首帧');report.importSeconds=(Date.now()-start)/1000;report.checks.push(report.input+'-through-production-import');
  let frame=page.frames().find(frame=>frame.url().includes('thkkd.html'));report.initial=await nativeState(frame);
  for(let i=0;i<8&&(await nativeState(frame)).outer!==100;i++)await key('z');
  assert.equal((await nativeState(frame)).outer,100);await page.keyboard.down('z');await wait(700);await page.keyboard.up('z');
  report.media=await frame.evaluate(()=>({assets:__thkkdRuntime.platform.assets.size,mp3:[...__thkkdRuntime.platform.assets.values()].filter(x=>x.file.endsWith('.mp3')).length,webm:[...__thkkdRuntime.platform.assets.values()].filter(x=>x.file.endsWith('.webm')).length}));
  assert.equal(report.media.assets,301);assert.equal(report.media.mp3,18);assert.equal(report.media.webm,4);report.checks.push('original-images-mp3-and-converted-video');
  report.videos=await frame.evaluate(async()=>{
    const result=[];
    for(const asset of __thkkdRuntime.platform.assets.values())if(asset.file.endsWith('.webm')){
      const video=document.createElement('video');video.preload='metadata';
      try{await new Promise((resolve,reject)=>{video.onloadedmetadata=resolve;video.onerror=()=>reject(Error(asset.source));video.src=asset.url;});
        video.muted=true;await video.play();await new Promise(resolve=>setTimeout(resolve,250));
        result.push({name:asset.source,width:video.videoWidth,height:video.videoHeight,duration:video.duration,played:video.currentTime>0});video.pause();
      }finally{video.removeAttribute('src');video.load();}
    }return result;
  });
  assert.ok(report.videos.every(v=>v.width===384&&v.height===448&&v.duration>30&&v.played));report.checks.push('four-original-video-dimensions-duration-and-playback');
  report.workers=await page.evaluate(()=>__importWorkers);
  assert.ok(report.workers.some(row=>row.url.startsWith('blob:')&&row.terminated),'转换结束释放 Blob Worker');
  assert.ok(report.progress.some(row=>/back100\.wmv.*\d+%/.test(row.text)),'视频百分比可见');
  const beforeReuse=coreRequests.length;
  await page.screenshot({path:resolve(output,'gameplay.png')});
  await page.reload({waitUntil:'networkidle0'});frame=await driver.start();assert.ok((await nativeState(frame)).outer>=0);report.checks.push('refresh-reuses-installed-resources');
  await page.setOfflineMode(true);await driver.library();frame=await driver.start();assert.ok((await nativeState(frame)).outer>=0);report.checks.push('offline-refresh-reuses-installed-resources');
  assert.equal(coreRequests.length,beforeReuse,'使用已有安装数据不加载视频内核');report.coreRequests=coreRequests;report.checks.push('lazy-load-and-worker-release');
  assert.equal(report.errors.length,0);report.result='passed';console.log(JSON.stringify({result:report.result,seconds:report.importSeconds,checks:report.checks,media:report.media}));
}catch(error){report.result='failed';report.error=String(error.stack||error);process.exitCode=1;console.error(error);await page.screenshot({path:resolve(output,'failure.png')});}
finally{await driver.collect();writeFileSync(resolve(output,'result.json'),JSON.stringify(report,null,2));await browser.close();}
