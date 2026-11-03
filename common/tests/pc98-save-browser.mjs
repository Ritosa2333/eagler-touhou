// Real Launcher UI + native PC-98 runtimes. Each case gets an empty browser
// context; exported native scores supply the legal import fixture.
import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {mkdir, readFile, writeFile} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {resolve} from 'node:path';
import {PRODUCT_GAMES} from '../contracts/product-catalog.mjs';
import {findChromiumExecutable} from '../../launcher/lib/chromium-executable.mjs';

const root=resolve(import.meta.dirname,'../..');
const require=createRequire(new URL('../../launcher/package.json',import.meta.url));
const puppeteer=require('puppeteer-core');
const base=process.env.PC98_LAUNCHER_URL||'http://127.0.0.1:18330/';
const games=(process.env.PC98_SAVE_GAMES||'th01,th02,th03,th04').split(',');
const output=resolve(root,'launcher/.cache/pc98-save-browser',process.env.PC98_SAVE_RUN||new Date().toISOString().replace(/[:.]/g,'-'));
await mkdir(output,{recursive:true});
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
const pause=ms=>new Promise(resolve=>setTimeout(resolve,ms));
const browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,
  args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling']});
const cdp=await browser.target().createCDPSession(),results=[];

try {
  for(const game of games){
    assert(/^th0[1-5]$/.test(game),'Unknown PC-98 game');
    const out=resolve(output,game),downloads=resolve(out,'downloads');
    await mkdir(downloads,{recursive:true});
    const context=await browser.createBrowserContext(),page=await context.newPage();
    await page.setViewport({width:1360,height:900});
    const errors=[],events=[];
    page.on('pageerror',error=>errors.push(error.message));
    await page.exposeFunction('recordSaveEvent',event=>events.push(event));
    await page.evaluateOnNewDocument(()=>addEventListener('message',event=>{
      if(event.source===document.querySelector('#gameFrame')?.contentWindow&&event.data?.protocol==='eagler-touhou/1')void window.recordSaveEvent(event.data);
    }));
    await cdp.send('Browser.setDownloadBehavior',{behavior:'allowAndName',downloadPath:downloads,browserContextId:context.id,eventsEnabled:true});
    const scoreFile=PRODUCT_GAMES[game].storage.scoreFile;
    let step='open',record={game,status:'FAIL'};
    async function wait(check,label,timeout=120000){
      const end=Date.now()+timeout;
      while(Date.now()<end){
        const fatal=events.find(event=>event.event==='error'||event.event==='fatal');
        if(fatal)throw Error(JSON.stringify(fatal));
        assert.deepEqual(errors,[],'browser exceptions');
        if(await check())return;
        await pause(100);
      }
      throw Error(label+' timeout: '+await page.evaluate(()=>document.body.innerText.slice(-1500)));
    }
    async function open(){
      await page.goto(base+'?test='+game);
      await page.waitForFunction(()=>window.__eaglerBoot?.done===true,{timeout:45000});
      await page.waitForSelector(`.game[data-game="${game}"]:not([data-product])`,{timeout:45000});
      await page.evaluate(()=>document.querySelector('#firstUseNoticeDialog')?.close());
      await page.click(`.game[data-game="${game}"]:not([data-product])`);
      // In an all-games development catalog the first click centers/selects a
      // carousel card; the next click opens the regular settings surface.
      await page.waitForFunction(()=>!!document.querySelector('.tools.is-open'),{timeout:1500}).catch(async()=>{
        await page.click(`.game[data-game="${game}"]:not([data-product])`);
      });
      await page.waitForSelector('.tools.is-open',{visible:true});
      await page.$eval('.tools.is-open',async panel=>{await Promise.all(panel.getAnimations().map(a=>a.finished.catch(()=>{})));});
      await page.waitForFunction(()=>document.querySelector('#launch')?.disabled===false,{timeout:60000});
    }
    async function launch(){
      const mark=events.length;
      await page.$eval('#launch',button=>button.click());
      await wait(async()=>{
        await page.evaluate(()=>{document.querySelector('#decisionDialog[open] #decisionConfirm')?.click();document.querySelector('#touchHelpClose')?.click();});
        return events.slice(mark).some(event=>event.event==='first-frame');
      },'native first frame',180000);
    }
    async function command(command,fields={}){
      return page.evaluate(({game,command,fields})=>{
        const frame=document.querySelector('#gameFrame'),target=frame.contentWindow;
        const epoch=Number(new URL(frame.src,location.href).searchParams.get('runtimeEpoch')),request=crypto.randomUUID();
        return new Promise((yes,no)=>{
          const timer=setTimeout(()=>{removeEventListener('message',receive);no(Error(command+' timeout'));},30000);
          function receive(event){
            const message=event.data;
            if(event.source!==target||event.origin!==location.origin||message?.protocol!=='eagler-touhou/1'||message.game!==game||message.epoch!==epoch||message.request!==request)return;
            clearTimeout(timer);removeEventListener('message',receive);message.ok?yes(message):no(Error(message.error));
          }
          addEventListener('message',receive);target.postMessage({protocol:'eagler-touhou/1',game,epoch,request,command,...fields},location.origin);
        });
      },{game,command,fields});
    }
    async function stored(){
      return Buffer.from(await page.evaluate(async({game,scoreFile})=>{
        const root='/saves'+game;
        const db=await new Promise((yes,no)=>{const request=indexedDB.open(root,21);request.onsuccess=()=>yes(request.result);request.onerror=()=>no(request.error);});
        try{return await new Promise((yes,no)=>{const request=db.transaction('FILE_DATA').objectStore('FILE_DATA').get(root+'/'+scoreFile);request.onsuccess=()=>yes(Array.from(request.result?.contents||[]));request.onerror=()=>no(request.error);});}
        finally{db.close();}
      },{game,scoreFile}));
    }
    async function download(){
      let entry,finished,error;
      const begin=event=>{entry=event;},progress=event=>{if(entry?.guid===event.guid){if(event.state==='completed')finished=true;if(event.state==='canceled')error=Error('Download canceled');}};
      cdp.on('Browser.downloadWillBegin',begin);cdp.on('Browser.downloadProgress',progress);
      try{
        await page.$eval('#saveFileTool [data-action="export-save"]',button=>button.click());
        await wait(async()=>{if(error)throw error;return !!finished;},'Launcher save download',60000);
        assert.equal(entry.suggestedFilename,scoreFile,'native download filename');
        return await readFile(resolve(downloads,entry.guid));
      }finally{cdp.off('Browser.downloadWillBegin',begin);cdp.off('Browser.downloadProgress',progress);}
    }
    async function retired(){await page.waitForFunction(()=>!document.querySelector('#gameFrame')?.hasAttribute('src'),{timeout:30000});}
    try{
      await open();step='native export';await launch();
      const original=Buffer.from((await command('read',{path:scoreFile})).bytes);
      assert(original.length>0,'native game must generate a score file');
      assert.deepEqual(await download(),original,'running-game download equals native bytes');
      const fixture=resolve(out,scoreFile);await writeFile(fixture,original);
      console.log(game+': native score downloaded ('+original.length+' bytes)');

      step='Launcher upload over running game';
      await page.$eval('#saveFileTool [data-action="import-save"]',button=>button.click());
      await page.waitForFunction(()=>document.querySelector('#decisionDialog')?.open);
      const chooserPromise=page.waitForFileChooser();
      await page.click('#decisionConfirm');const chooser=await chooserPromise;await chooser.accept([fixture]);
      await page.waitForFunction(()=>document.querySelector('#status')?.textContent.includes('已导入 1 个文件'),{timeout:120000});
      await retired();assert.deepEqual(await stored(),original,'import commits bytes before retiring Runtime');
      console.log(game+': uploaded through Launcher and verified after file Runtime restart');

      step='native reload';await open();await launch();
      assert.deepEqual(Buffer.from((await command('read',{path:scoreFile})).bytes),original,'native runtime exports the restored save after full page reload');
      await page.screenshot({path:resolve(out,'restored.png')});
      await command('sync');
      // A full document reload retires the gameplay Runtime and flushes the
      // browser-backed store before checking the temporary file Runtime path.
      await page.goto(base+'?test='+game);await page.waitForFunction(()=>window.__eaglerBoot?.done===true,{timeout:45000});
      assert.deepEqual(await stored(),original,'closing gameplay preserves save');
      console.log(game+': native startup restored uploaded score');

      step='temporary download';await open();
      assert.deepEqual(await download(),original,'temporary file Runtime download after gameplay teardown');await retired();
      assert.deepEqual(await stored(),original,'temporary Runtime teardown preserves save');
      step='launch after temporary Runtime';await launch();
      assert.deepEqual(Buffer.from((await command('read',{path:scoreFile})).bytes),original,'native restore after temporary download');
      record={game,status:'PASS',scoreFile,bytes:original.length,sha256:hash(original),checks:['native-export','Launcher-download','Launcher-upload-while-running','file-runtime-restart','page-reload-native-restore','gameplay-close','temporary-download','temporary-retirement','native-restore-after-download']};
      console.log('PASS '+game+' save round trip');
    }catch(error){
      record={...record,step,error:error.stack};
      await page.screenshot({path:resolve(out,'failure.png')}).catch(()=>{});
      console.error('FAIL '+game+' '+step+': '+error.message);
    }finally{
      record.errors=errors;record.runtimeErrors=events.filter(event=>event.event==='error'||event.event==='fatal');
      results.push(record);await writeFile(resolve(output,'report.json'),JSON.stringify(results,null,2)+'\n');
      await writeFile(resolve(out,'events.json'),JSON.stringify(events,null,2)+'\n');await context.close();
    }
  }
}finally{await browser.close();}
console.log('Report: '+resolve(output,'report.json'));
if(results.some(result=>result.status!=='PASS'))process.exitCode=1;
