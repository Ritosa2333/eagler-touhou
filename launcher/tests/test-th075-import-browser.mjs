import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {mkdir,writeFile} from 'node:fs/promises';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../lib/chromium-executable.mjs';
const base=process.env.EAGLER_TH075_TEST_URL||'http://127.0.0.1:18175/',zip=resolve(process.env.EAGLER_TH075_IMPORT_ZIP||'../游戏包/th075（东方萃梦想）.zip'),output=resolve('artifacts/th075-import');await mkdir(output,{recursive:true});
const browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--no-sandbox','--autoplay-policy=no-user-gesture-required']});
try {
 const context=await browser.createBrowserContext(),page=await context.newPage();await page.setViewport({width:1280,height:900});
 const dataRequests=[];await page.setRequestInterception(true);page.on('request',r=>{if(new URL(r.url()).pathname.startsWith('/games/th075/')){dataRequests.push(r.url());void r.abort();}else void r.continue();});
 await page.goto(base+'?test=th075-import');await page.waitForFunction(()=>window.__eaglerBoot?.done===true);
 await page.evaluate(()=>document.querySelector('#firstUseNoticeDialog')?.close());
 await page.$eval('.game[data-game=th075]',e=>{e.click();e.click();});await page.waitForSelector('#gamePackageImport');await page.$eval('#gamePackageImport',e=>e.click()); console.log('Import requested');
 await page.waitForFunction(()=>document.querySelector('#gameDataImportWindow')?.hidden===false);
 await (await page.$('#gameDataImportInput')).uploadFile(zip);
 if(process.env.EAGLER_EXPECT_IMPORT_SIZE_ERROR==='1'){
  await page.waitForFunction(()=>document.querySelector('#gameDataImportReason')?.textContent.includes('游戏数据包大小无效'));
  console.log('REPRODUCED:',await page.$eval('#gameDataImportReason',e=>e.textContent));
 }else{
  await page.waitForFunction(()=>document.querySelector('#gameDataImportWindow')?.hidden===true,{timeout:120000});console.log('Actual ZIP imported through UI');
  await page.select('#musicSelect','ogg-stream');
  for(let i=0;i<2;i++){
   await page.waitForFunction(()=>!document.querySelector('#launch')?.disabled);await page.$eval('#launch',e=>e.click()); console.log('Launch requested',i);
   await page.waitForFunction(()=>{document.querySelector('#decisionConfirm')?.click();document.querySelector('#touchHelpClose')?.click();return document.querySelector('#gameFrame')?.contentWindow?.Module?._th075_frame?.()>90;},{timeout:120000});
   assert.deepEqual(dataRequests,[],'Imported data/music must launch without remote content');
   await page.screenshot({path:resolve(output,'imported-launch.png')});
   if(i===0){await page.reload();await page.waitForFunction(()=>window.__eaglerBoot?.done===true);await page.$eval('.game[data-game=th075]',e=>{e.click();e.click();});}
  }
  const report={url:base,zip,uiImport:true,launch:true,reloadLaunch:true,remoteGameContentRequests:0};await writeFile(resolve(output,'report.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify(report));
 }
 await context.close();
}finally{await browser.close();}

