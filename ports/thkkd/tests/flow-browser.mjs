import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {productionBrowser,nativeState,runtimeCommand,wait} from './production-driver.mjs';
import {nativeReplay} from '../web/replay.mjs';
const observe=process.argv.includes('--observe-persistence'),url=process.env.THKKD_SITE_URL||'http://127.0.0.1:8177/',output=fileURLToPath(new URL('../build/quality-mp/native-flow'+(observe?'-before':'')+'/',import.meta.url));mkdirSync(output,{recursive:true});
const report={schema:'thkkd/native-flow/1',checks:[]},driver=await productionBrowser({url,report}),{browser,page,key}=driver;
try{
 await driver.library();let frame=await driver.start();
 await frame.evaluate(()=>{window.__qualitySyncTimes=[];const fs=__thkkdRuntime.core.FS,sync=fs.syncfs;fs.syncfs=(populate,callback)=>{if(!populate)__qualitySyncTimes.push(performance.now());return sync.call(fs,populate,callback);};});
 for(let i=0;i<8&&(await nativeState(frame)).outer!==100;i++)await key('z');assert.equal((await nativeState(frame)).outer,100);
 await page.keyboard.down('z');await key('ArrowRight',250,30);await key('x',80,30);await page.keyboard.up('z');
 report.beforeRestart=await nativeState(frame);await key('Escape');await key('r');report.restarted=await nativeState(frame);assert.equal(report.restarted.gameScene,0);assert.ok(report.restarted.gameFrame<report.beforeRestart.gameFrame);report.checks.push('native-pause-R-restarts-current-run');
 await page.keyboard.down('z');await wait(500);await page.keyboard.up('z');await key('Escape');await key('ArrowDown');await key('z');
 const replayMenu=()=>frame.evaluate(()=>{const w=address=>__thkkdRuntime.core._thkkd_word(address);return {state:w(0xb41215),selected:w(0xa87f40),nameCursor:w(0xb43046),phase:w(0xa87f48),outer:w(0xa8ce58)};});
 report.saveMenus=[await replayMenu()];assert.equal(report.saveMenus.at(-1).state,3,'原生保存录像询问');
 await key('z');report.saveMenus.push(await replayMenu());assert.equal(report.saveMenus.at(-1).state,1,'原生录像槽选择');
 await key('z');report.saveMenus.push(await replayMenu());assert.equal(report.saveMenus.at(-1).state,2,'原生署名');
 await key('z');assert.equal((await replayMenu()).nameCursor,1,'Z 输入署名字符');await key('ArrowLeft');assert.equal((await replayMenu()).selected,85);
 await runtimeCommand(page,'sync');const beforeSaveSyncs=await frame.evaluate(()=>__qualitySyncTimes.length);await key('z',80,60);report.saveSyncDelta=await frame.evaluate(before=>__qualitySyncTimes.length-before,beforeSaveSyncs);if(!observe)assert.ok(report.saveSyncDelta>0,'原生文件关闭触发独立同步');
 const listed=await runtimeCommand(page,'list'),saved=listed.files.find(file=>file.path.endsWith('.rpy'));assert.ok(saved,'原生保存器实际生成录像');const exported=(await runtimeCommand(page,'read',{path:saved.path})).bytes,decoded=nativeReplay(Uint8Array.from(exported));assert.equal(decoded.description.name,'A');report.saved={path:saved.path,bytes:exported.length,count:decoded.count,description:decoded.description};report.checks.push('Z-save-prompt-slot-name-and-native-replay-export');
 const persisted=await frame.evaluate(path=>new Promise((resolve,reject)=>{const request=indexedDB.open('/savesthkkd');request.onerror=()=>reject(request.error);request.onsuccess=()=>{const db=request.result,transaction=db.transaction('FILE_DATA','readonly'),get=transaction.objectStore('FILE_DATA').get('/savesthkkd/'+path);get.onsuccess=()=>resolve(get.result?.contents?[...get.result.contents]:null);get.onerror=()=>reject(get.error);transaction.oncomplete=()=>db.close();};}),saved.path);report.saveImmediatelyPersisted=JSON.stringify(persisted)===JSON.stringify(exported);if(!observe)assert.ok(report.saveImmediatelyPersisted,'原生保存结束立即进入独立 IndexedDB 持久化记录');
 await runtimeCommand(page,'write',{path:'replay/imported.rpy',bytes:exported});await runtimeCommand(page,'playback',{action:'open',path:'replay/imported.rpy'});await runtimeCommand(page,'playback',{action:'pause'});await runtimeCommand(page,'playback',{action:'seek',value:Math.min(20,decoded.count-2)});await frame.waitForFunction(target=>__thkkdRuntime.core._thkkd_word(0xb43110)===target,{},Math.min(20,decoded.count-2));report.playback=await nativeState(frame);
 await page.keyboard.down('ArrowRight');await page.keyboard.down('x');await wait(160);assert.equal((await nativeState(frame)).cursor,report.playback.cursor);await page.keyboard.up('ArrowRight');await page.keyboard.up('x');report.checks.push('native-replay-import-seek-and-live-input-isolation');
 const invalid=Uint8Array.from(exported);new DataView(invalid.buffer,invalid.length-62,62).setUint32(12,8,true);await runtimeCommand(page,'write',{path:'replay/invalid.rpy',bytes:[...invalid]});await assert.rejects(()=>runtimeCommand(page,'playback',{action:'inspect',path:'replay/invalid.rpy'}),/元数据无效/);assert.equal((await nativeState(frame)).cursor,report.playback.cursor);report.checks.push('invalid-replay-inspection-reports-error-and-keeps-current-playback');
 await page.reload({waitUntil:'networkidle0'});frame=await driver.start();assert.deepEqual((await runtimeCommand(page,'read',{path:saved.path})).bytes,exported);report.checks.push('refresh-retains-original-replay-bytes');
 assert.equal(report.errors.length,0);report.result=observe?'observed':'passed';console.log(JSON.stringify({result:report.result,checks:report.checks,saved:report.saved,saveImmediatelyPersisted:report.saveImmediatelyPersisted}));
}catch(error){report.result='failed';report.error=error.stack;process.exitCode=1;console.error(error);await page.screenshot({path:output+'failure.png'});}
finally{await driver.collect();writeFileSync(output+'result.json',JSON.stringify(report,null,2));await browser.close();}
