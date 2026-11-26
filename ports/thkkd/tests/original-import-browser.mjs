import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {productionBrowser,nativeState,wait} from './production-driver.mjs';
const args=Object.fromEntries(process.argv.slice(2).map(value=>{const at=value.indexOf('=');return [value.slice(2,at),value.slice(at+1)];}));
if(!args.zip&&!(args.dat&&args.bgm))throw Error('请指定 --zip=原版资源ZIP，或 --dat=dat.dxa --bgm=bgm.dxa');
const url=args.url||'http://127.0.0.1:18180/',output=resolve(args.output||'ports/thkkd/build/original-import');mkdirSync(output,{recursive:true});
const report={schema:'thkkd/original-import-browser/1',checks:[],progress:[]};
const driver=await productionBrowser({url,report,protocolTimeout:1200000,profile:args.profile}),{page,browser,key}=driver;
try {
  await driver.library();
  const buttons=await page.$$('button');
  for(const button of buttons)if(await button.evaluate(node=>['Import game resources','导入游戏资源','Start game','开始游戏','启动游戏'].includes(node.textContent.trim()))){await button.click();break;}
  await page.waitForSelector('#gameDataImportWindow',{visible:true,timeout:60000});
  const hint=await page.$eval('#gameDataImportReason',node=>node.textContent);
  assert.match(hint,/dat\.dxa/);assert.match(hint,/bgm\.dxa/);assert.doesNotMatch(hint,/WAV|RAR|7z/);
  await wait(400);await page.screenshot({path:resolve(output,'import-dialog.png')});
  const [chooser]=await Promise.all([page.waitForFileChooser(),page.click('#transferImport')]);
  assert.equal(chooser.isMultiple(),true);await chooser.accept(args.zip?[resolve(args.zip)]:[resolve(args.dat),resolve(args.bgm)]);
  report.input=args.zip?'original-zip':'two-original-dxa';report.checks.push('file-picker-and-format-guidance');
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
        result.push({name:asset.source,width:video.videoWidth,height:video.videoHeight,duration:video.duration});
      }finally{video.removeAttribute('src');video.load();}
    }return result;
  });
  assert.ok(report.videos.every(v=>v.width===384&&v.height===448&&v.duration>30));report.checks.push('four-original-video-dimensions-and-duration');
  await page.screenshot({path:resolve(output,'gameplay.png')});
  await page.reload({waitUntil:'networkidle0'});frame=await driver.start();assert.ok((await nativeState(frame)).outer>=0);report.checks.push('refresh-reuses-installed-resources');
  await page.setOfflineMode(true);await driver.library();frame=await driver.start();assert.ok((await nativeState(frame)).outer>=0);report.checks.push('offline-refresh-reuses-installed-resources');
  assert.equal(report.errors.length,0);report.result='passed';console.log(JSON.stringify({result:report.result,seconds:report.importSeconds,checks:report.checks,media:report.media}));
}catch(error){report.result='failed';report.error=String(error.stack||error);process.exitCode=1;console.error(error);await page.screenshot({path:resolve(output,'failure.png')});}
finally{await driver.collect();writeFileSync(resolve(output,'result.json'),JSON.stringify(report,null,2));await browser.close();}
