import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync,readFileSync,existsSync,unlinkSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {createRequire} from 'node:module';
import {productionBrowser,runtimeCommand,wait} from './production-driver.mjs';
import {replayFixture} from './replay-fixture.mjs';
const {unzipSync}=createRequire(new URL('../../../launcher/package.json',import.meta.url))('fflate');
const output=fileURLToPath(new URL('../build/quality-mp/files-ui/',import.meta.url));mkdirSync(output,{recursive:true});
const save=new Uint8Array(329*4),view=new DataView(save.buffer);[1,2,3,0,5,4,11,13,6,0,77,80,1,0].forEach((value,index)=>view.setUint32(index*4,value,true));
const replay=replayFixture(),savePath=output+'th_kaikeidou01.dat',replayPath=output+'test.rpy';writeFileSync(savePath,save);writeFileSync(replayPath,replay);
for(const name of ['thkkd-save.zip','thkkd00.rpy'])if(existsSync(output+name))unlinkSync(output+name);
const report={schema:'thkkd/files-ui/1',checks:[]},driver=await productionBrowser({url:process.env.THKKD_SITE_URL||'http://127.0.0.1:8177/',report}),{browser,page}=driver;
const visibleButtons=async()=>page.$$eval('dialog[open] button,[role="dialog"] button',buttons=>buttons.filter(button=>button.getClientRects().length).map(button=>button.textContent.trim()));
async function buttonMatching(pattern){for(const button of await page.$$('dialog[open] button,[role="dialog"] button'))if(await button.evaluate((element,source)=>element.getClientRects().length&&new RegExp(source,'i').test(element.textContent.trim()),pattern.source)){await button.click();return;}throw Error('对话框按钮缺失：'+JSON.stringify(await visibleButtons()));}
async function filesOpen(){await page.waitForSelector('#fileOptions > button');if(await page.$eval('#fileOptions > button',button=>button.getAttribute('aria-expanded'))!=='true')await page.click('#fileOptions > button');}
async function upload(selector,path){const chooser=page.waitForFileChooser();await page.click(selector);await(await chooser).accept([path]);}
async function downloaded(name){for(let i=0;i<100;i++){if(existsSync(output+name)&&!existsSync(output+name+'.crdownload'))return readFileSync(output+name);await wait(100);}throw Error('未收到下载：'+name);}
try{
 const cdp=await page.createCDPSession();await cdp.send('Browser.setDownloadBehavior',{behavior:'allow',downloadPath:output});await driver.library();await driver.start();await page.reload({waitUntil:'networkidle0'});await filesOpen();
 await upload('[data-action="import-save"]',savePath);await page.waitForSelector('#decisionDialog[open]');report.overwritePrompt=await page.$eval('#decisionDialog',element=>element.textContent);assert.match(report.overwritePrompt,/overwrit|replace|覆盖/i);await buttonMatching(/^(Continue import|继续导入)$/);await page.waitForFunction(()=>/imported|导入.*完成|已导入/i.test(document.body.textContent),{timeout:60000});
 let frame=await driver.start();report.importedVolume=await frame.evaluate(()=>__thkkdRuntime.core._thkkd_word(0xa7ebfc));assert.equal(report.importedVolume,77);assert.deepEqual((await runtimeCommand(page,'read',{path:'th_kaikeidou01.dat'})).bytes,[...save]);report.checks.push('actual-save-file-picker-overwrite-prompt-and-native-reload');
 await page.reload({waitUntil:'networkidle0'});await filesOpen();await page.click('[data-action="export-save"]');const archive=unzipSync(await downloaded('thkkd-save.zip'));assert.deepEqual([...archive['th_kaikeidou01.dat']],[...save]);report.checks.push('actual-save-download-ZIP-preserves-native-bytes');
 await page.click('[data-action="manage-replay"]');await page.waitForSelector('.replay-file-browser > header button:not([disabled])');await upload('.replay-file-browser > header button',replayPath);await page.waitForSelector('[data-replay-file]',{timeout:30000});report.importedReplay=await page.$eval('[data-replay-file]',element=>element.getAttribute('data-replay-file'));
 async function fileMenu(label){await wait(250);await page.locator('[data-replay-file] .replay-file-menu-trigger').click();await page.waitForSelector('[role="menuitem"]',{visible:true});for(const item of await page.$$('[role="menuitem"]'))if(await item.evaluate((element,label)=>element.textContent.trim()===label,label)){await item.click();return;}throw Error('录像操作缺失：'+label);}
 await fileMenu('Download');const replayName=report.importedReplay.split('/').at(-1);assert.deepEqual([...await downloaded(replayName)],[...replay]);report.checks.push('actual-replay-workspace-import-and-download-preserve-original-format');
 await fileMenu('Rename');await page.waitForSelector('#decisionDialog[open]');await page.click('#decisionInput',{clickCount:3});await page.keyboard.type('quality.rpy');await page.click('#decisionConfirm');await page.waitForFunction(()=>document.querySelector('[data-replay-file]')?.getAttribute('data-replay-file')?.endsWith('quality.rpy'));report.renamedReplay=await page.$eval('[data-replay-file]',element=>element.getAttribute('data-replay-file'));report.checks.push('replay-workspace-confirmed-rename');
 await fileMenu('Delete');await page.waitForSelector('#decisionDialog[open]');await page.click('#decisionConfirm');await page.waitForFunction(()=>!document.querySelector('[data-replay-file]'));report.checks.push('replay-workspace-confirmed-delete-updates-list');
 await driver.library();let reopened=await driver.start();assert.equal(await reopened.evaluate(()=>__thkkdRuntime.core._thkkd_word(0xa7ebfc)),77);assert.ok(!(await runtimeCommand(page,'list')).files.some(file=>[report.importedReplay,report.renamedReplay].includes(file.path)));report.checks.push('save-and-replay-deletion-persist-after-refresh');
 assert.equal(report.errors.length,0);report.result='passed';console.log(JSON.stringify({result:report.result,checks:report.checks}));
}catch(error){report.result='failed';report.error=error.stack;process.exitCode=1;console.error(error);await page.screenshot({path:output+'failure.png'});}
finally{await driver.collect();writeFileSync(output+'result.json',JSON.stringify(report,null,2));await browser.close();}
