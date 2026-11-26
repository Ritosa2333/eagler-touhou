import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {productionBrowser,runtimeCommand} from './production-driver.mjs';

const args=Object.fromEntries(process.argv.slice(2).map(argument=>{const i=argument.indexOf('=');return [argument.slice(2,i),argument.slice(i+1)];}));
const output=resolve(args.output||'ports/thkkd/build/adaptation-20261127/progress');mkdirSync(output,{recursive:true});
const report={schema:'thkkd/local-progress-browser/1',checks:[]};
const driver=await productionBrowser({url:args.url||'http://127.0.0.1:19027/',profile:args.profile,report});
const {page,browser}=driver;
try{
  await page.setBypassServiceWorker(true);await page.setCacheEnabled(false);await driver.library();
  await page.waitForSelector('[data-statistics-game="thkkd"][data-statistics-source="local"]');
  const source=new Uint8Array(1316),words=new DataView(source.buffer);
  [1,2,3,0,5,4,11,13,6,0,77,80,1,0].forEach((value,index)=>words.setUint32(index*4,value,true));
  words.setUint32(76,1,true);words.setUint32(56+(63+21+14+5)*4,2,true);words.setUint32(56+(252+42+14+6)*4,2,true);
  await driver.start();await runtimeCommand(page,'write',{path:'th_kaikeidou01.dat',bytes:[...source]});
  await driver.library();
  const selector='[data-clear-game="thkkd"]';
  await page.waitForFunction(selector=>document.querySelectorAll(selector+' tbody tr[data-character]').length===9,{},selector);
  report.cells=await page.$$eval(selector+' tbody tr[data-character]',rows=>rows.map(row=>({
    character:Number(row.dataset.character),shot:Number(row.dataset.shot),
    cells:[...row.querySelectorAll('td')].map(cell=>({difficulty:cell.dataset.difficulty,status:cell.querySelector('[data-status]')?.getAttribute('data-status')})),
  })));
  const status=(character,shot,difficulty)=>report.cells.find(row=>row.character===character&&row.shot===shot).cells.find(cell=>cell.difficulty===difficulty).status;
  assert.equal(status(0,0,'easy'),'cleared');assert.equal(status(1,2,'normal'),'one-credit');assert.equal(status(2,2,'extra'),'one-credit');assert.equal(status(0,1,'normal'),'uncleared');
  assert.equal(await page.$eval('[data-statistics-game="thkkd"]',element=>element.dataset.cloudRecord),'false');
  assert.match(await page.$eval('[data-statistics-game="thkkd"]',element=>element.textContent),/不续关通关/);
  assert.equal(report.cells.reduce((sum,row)=>sum+row.cells.length,0),45);
  await page.screenshot({path:resolve(output,'local-progress.png'),fullPage:true});
  report.checks.push('native-save-read-on-detail-with-nine-loadouts-and-five-difficulties','continued-clear-and-one-credit-clear-distinguished','local-statistics-without-cloud-save');
  await driver.start();assert.deepEqual((await runtimeCommand(page,'read',{path:'th_kaikeidou01.dat'})).bytes,[...source]);
  report.checks.push('statistics-read-preserves-native-save-bytes');
  await driver.collect();assert.equal(report.errors.length,0);report.result='passed';console.log(JSON.stringify({result:report.result,checks:report.checks}));
}catch(error){report.result='failed';report.error=String(error.stack||error);process.exitCode=1;console.error(error);await page.screenshot({path:resolve(output,'failure.png')});}
finally{writeFileSync(resolve(output,'result.json'),JSON.stringify(report,null,2));await browser.close();}
