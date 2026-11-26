import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync,readFileSync,existsSync,unlinkSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {createRequire} from 'node:module';
import {productionBrowser,runtimeCommand,wait} from './production-driver.mjs';
import {replayFixture} from './replay-fixture.mjs';
import {encodeExtended} from '../web/replay.mjs';
const {unzipSync}=createRequire(new URL('../../../launcher/package.json',import.meta.url))('fflate');
const batch=process.argv.includes('--batch');
const output=fileURLToPath(new URL('../build/quality-mp/'+(batch?'files-ui-batch':'files-ui')+'/',import.meta.url));mkdirSync(output,{recursive:true});
const save=new Uint8Array(329*4),view=new DataView(save.buffer);[1,2,3,0,5,4,11,13,6,0,77,80,1,0].forEach((value,index)=>view.setUint32(index*4,value,true));
const replay=replayFixture(),savePath=output+'th_kaikeidou01.dat',replayPath=output+'test.rpy';writeFileSync(savePath,save);writeFileSync(replayPath,replay);
const config=new TextEncoder().encode('01000\n'),configPath=output+'config.txt';writeFileSync(configPath,config);
const extra=replayFixture({count:480,shot:1}),extended=await encodeExtended(replayFixture({count:600}),new Map([[60,{x:100,y:200}]]),true);
mkdirSync(output+'second',{recursive:true});const extraPath=output+'second/test.rpy',extendedPath=output+'test.thkkdr';writeFileSync(extraPath,extra);writeFileSync(extendedPath,extended);
for(const name of ['thkkd-save.zip','thkkd00.rpy'])if(existsSync(output+name))unlinkSync(output+name);
const report={schema:'thkkd/files-ui/1',checks:[]},driver=await productionBrowser({url:process.env.THKKD_SITE_URL||'http://127.0.0.1:8177/',report}),{browser,page}=driver;
const visibleButtons=async()=>page.$$eval('dialog[open] button,[role="dialog"] button',buttons=>buttons.filter(button=>button.getClientRects().length).map(button=>button.textContent.trim()));
async function buttonMatching(pattern){for(const button of await page.$$('dialog[open] button,[role="dialog"] button'))if(await button.evaluate((element,source)=>element.getClientRects().length&&new RegExp(source,'i').test(element.textContent.trim()),pattern.source)){await button.click();return;}throw Error('对话框按钮缺失：'+JSON.stringify(await visibleButtons()));}
async function filesOpen(){if(!await page.$eval('#fileOptions',node=>node.getBoundingClientRect().width>0).catch(()=>false))await page.locator('#gameSettingsOpen').click();await page.waitForSelector('#fileOptions > button',{visible:true});await wait(400);if(await page.$eval('#fileOptions > button',button=>button.getAttribute('aria-expanded'))!=='true')await page.click('#fileOptions > button');}
async function upload(selector,paths){const pending=page.waitForFileChooser();await page.click(selector);const chooser=await pending;if(batch)assert.equal(chooser.isMultiple(),true);await chooser.accept(Array.isArray(paths)?paths:[paths]);}
async function downloaded(name){for(let i=0;i<100;i++){if(existsSync(output+name)&&!existsSync(output+name+'.crdownload'))return readFileSync(output+name);await wait(100);}throw Error('未收到下载：'+name);}
try{
 const cdp=await page.createCDPSession();await cdp.send('Browser.setDownloadBehavior',{behavior:'allow',downloadPath:output});await driver.library();await driver.start();await page.reload({waitUntil:'networkidle0'});await filesOpen();
 await upload('[data-action="import-save"]',batch?[savePath,configPath]:savePath);await page.waitForSelector('#decisionDialog[open]');report.overwritePrompt=await page.$eval('#decisionDialog',element=>element.textContent);assert.match(report.overwritePrompt,/overwrit|replace|覆盖/i);await buttonMatching(/^(Continue import|继续导入)$/);await page.waitForFunction(()=>/imported|导入.*完成|已导入/i.test(document.body.textContent),{timeout:60000});
 await page.click('#libraryBack');let frame=await driver.start();report.importedVolume=await frame.evaluate(()=>__thkkdRuntime.core._thkkd_word(0xa7ebfc));assert.equal(report.importedVolume,77);assert.deepEqual((await runtimeCommand(page,'read',{path:'th_kaikeidou01.dat'})).bytes,[...save]);if(batch)assert.deepEqual((await runtimeCommand(page,'read',{path:'config.txt'})).bytes,[...config]);report.checks.push('actual-save-file-picker-overwrite-prompt-and-native-reload');
 await page.reload({waitUntil:'networkidle0'});await filesOpen();await page.click('[data-action="export-save"]');const archive=unzipSync(await downloaded('thkkd-save.zip'));assert.deepEqual([...archive['th_kaikeidou01.dat']],[...save]);if(batch)assert.deepEqual([...archive['config.txt']],[...config]);report.checks.push('actual-save-download-ZIP-preserves-native-bytes');
 await page.click('[data-action="manage-replay"]');await page.waitForSelector('.replay-file-browser > header button:not([disabled])');const known=await page.$$eval('[data-replay-file]',nodes=>nodes.map(n=>n.getAttribute('data-replay-file')));await upload('.replay-file-browser > header button',batch?[replayPath,extraPath,extendedPath]:replayPath);await page.waitForFunction(known=>[...document.querySelectorAll('[data-replay-file]')].some(n=>!known.includes(n.getAttribute('data-replay-file'))),{timeout:30000},known);report.importedReplays=await page.$$eval('[data-replay-file]',(nodes,known)=>nodes.map(n=>n.getAttribute('data-replay-file')).filter(n=>!known.includes(n)),known);assert.equal(report.importedReplays.length,batch?3:1);report.importedReplay=report.importedReplays.find(path=>path.endsWith('.rpy'))||report.importedReplays[0];
 async function fileMenu(label,path=report.renamedReplay||report.importedReplay){await wait(250);await page.locator('[data-replay-file="'+path+'"] .replay-file-menu-trigger').click();await page.waitForSelector('[role="menuitem"]',{visible:true});for(const item of await page.$$('[role="menuitem"]'))if(await item.evaluate((element,label)=>[label,({Download:'下载',Rename:'重命名',Delete:'删除'})[label]].includes(element.textContent.trim()),label)){await item.click();return;}throw Error('录像操作缺失：'+label);}
 const expected=batch?[replay,extra,extended]:[replay],seen=new Set();for(const path of report.importedReplays){const name=path.split('/').at(-1);if(existsSync(output+name))unlinkSync(output+name);await fileMenu('Download',path);const bytes=await downloaded(name);const index=expected.findIndex(value=>Buffer.from(value).equals(bytes));assert.ok(index>=0);seen.add(index);}assert.equal(seen.size,expected.length);if(batch)report.checks.push('multi-select-save-and-mixed-replay-batches-with-unique-filenames');report.checks.push('actual-replay-workspace-import-and-download-preserve-original-format');
 if(batch){
  await page.evaluate(()=>{window.__fileWorkspacePlayback=null;addEventListener('message',event=>{const m=event.data;if(event.source===document.querySelector('#gameFrame')?.contentWindow&&m?.event==='runtime-info'&&m.playback)__fileWorkspacePlayback=m.playback;});});
  report.played=[];
  for(const path of [report.importedReplay,report.importedReplays.find(path=>path.endsWith('.thkkdr'))]){
   await page.locator('[data-replay-watch="'+path+'"]').click();
   const stage='[data-replay-file="'+path+'"] [data-replay-stage="0"]';await page.waitForSelector(stage,{visible:true});
   await page.evaluate(()=>{__fileWorkspacePlayback=null;});await page.click(stage);
   await page.waitForFunction(path=>__fileWorkspacePlayback?.path===path&&__fileWorkspacePlayback.ended,{timeout:45000},path);
   const ended=await page.evaluate(()=>({frame:__fileWorkspacePlayback.frame,total:__fileWorkspacePlayback.total,ended:__fileWorkspacePlayback.ended}));assert.equal(ended.frame,ended.total);report.played.push({path,...ended});
  }
  report.checks.push('native-and-extended-replays-play-to-end-from-workspace-buttons');
 }
 await fileMenu('Rename');await page.waitForSelector('#decisionDialog[open]');await page.click('#decisionInput',{clickCount:3});await page.keyboard.type('quality.rpy');await page.click('#decisionConfirm');await page.waitForFunction(()=>[...document.querySelectorAll('[data-replay-file]')].some(n=>n.getAttribute('data-replay-file')?.endsWith('quality.rpy')));report.renamedReplay=await page.$$eval('[data-replay-file]',nodes=>nodes.map(n=>n.getAttribute('data-replay-file')).find(n=>n.endsWith('quality.rpy')));report.checks.push('replay-workspace-confirmed-rename');
 await fileMenu('Delete');await page.waitForSelector('#decisionDialog[open]');await page.click('#decisionConfirm');await page.waitForFunction(path=>![...document.querySelectorAll('[data-replay-file]')].some(n=>n.getAttribute('data-replay-file')===path),{},report.renamedReplay);report.checks.push('replay-workspace-confirmed-delete-updates-list');
 await driver.library();let reopened=await driver.start();assert.equal(await reopened.evaluate(()=>__thkkdRuntime.core._thkkd_word(0xa7ebfc)),77);assert.ok(!(await runtimeCommand(page,'list')).files.some(file=>[report.importedReplay,report.renamedReplay].includes(file.path)));report.checks.push('save-and-replay-deletion-persist-after-refresh');
 assert.equal(report.errors.length,0);report.result='passed';console.log(JSON.stringify({result:report.result,checks:report.checks}));
}catch(error){report.result='failed';report.error=error.stack;process.exitCode=1;console.error(error);await page.screenshot({path:output+'failure.png'});}
finally{await driver.collect();writeFileSync(output+'result.json',JSON.stringify(report,null,2));await browser.close();}
