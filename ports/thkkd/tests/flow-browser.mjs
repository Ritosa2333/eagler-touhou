import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync,readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {productionBrowser,nativeState,runtimeCommand,wait} from './production-driver.mjs';
import {nativeReplay,decodeReplay} from '../web/replay.mjs';
const freeTouch=process.argv.includes('--free-touch'),gamepad=process.argv.includes('--gamepad'),observe=process.argv.includes('--observe-persistence'),url=process.env.THKKD_SITE_URL||'http://127.0.0.1:8177/',output=resolve(process.env.THKKD_TEST_OUTPUT||fileURLToPath(new URL('../build/quality-mp/native-flow'+(observe?'-before':'')+'/',import.meta.url)))+'/';mkdirSync(output,{recursive:true});
const report={schema:'thkkd/native-flow/1',checks:[]},driver=await productionBrowser({url,report,touch:freeTouch}),{browser,page,key}=driver;
const expectedBuildId=gamepad||freeTouch?JSON.parse(readFileSync(new URL('../runtime/manifest.json',import.meta.url),'utf8')).buildId:undefined;
const confirm=async()=>{if(!freeTouch){await key('z');return;}const box=await page.$eval('#touchDirectSurface',element=>{const r=element.getBoundingClientRect();return {x:r.x+r.width*.6,y:r.y+r.height*.5};});await page.touchscreen.tap(box.x,box.y);await wait(450);};
try{
 if(gamepad||freeTouch){await page.setBypassServiceWorker(true);await page.setCacheEnabled(false);}
 if(gamepad)await page.evaluateOnNewDocument(()=>{window.testPads=[];Object.defineProperty(navigator,'getGamepads',{value:()=>window.testPads});});
 await driver.library();
 if(gamepad){
   await page.click('#gameSettingsOpen');await page.waitForSelector('#libraryBack',{visible:true});
   await page.evaluate(()=>{for(const element of document.querySelectorAll('*')){let fiber=element[Object.keys(element).find(k=>k.startsWith('__reactFiber$'))];while(fiber){const session=fiber.memoizedProps?.session;if(session?.gamepad){window.testSession=session;session.settings.setOption('touchEnabled',false);const pad={id:'THKKD recording controller',index:0,connected:true,mapping:'standard',buttons:Array.from({length:17},()=>({pressed:false,value:0})),axes:[0,0,0,0]};testPads=[pad];session.gamepad.refresh();session.gamepad.select(0,session.gamepad.getSnapshot().devices[0].key);session.gamepad.setEnabled(true);return;}fiber=fiber.return;}}throw Error('Gamepad session missing');});
   await page.click('#libraryBack');
 }
 if(freeTouch){
   await page.click('#gameSettingsOpen');await page.waitForSelector('#libraryBack',{visible:true});
   await page.evaluate(()=>{for(const element of document.querySelectorAll('*')){let fiber=element[Object.keys(element).find(k=>k.startsWith('__reactFiber$'))];while(fiber){const session=fiber.memoizedProps?.session;if(session?.settings){window.testSession=session;session.settings.setOption('touchEnabled',true);session.settings.setOption('touchMovementMode','touch-unlimited');session.settings.setOption('doubleTapBombEnabled',false);return;}fiber=fiber.return;}}throw Error('Touch session missing');});
   await page.click('#libraryBack');
 }
 let frame=await driver.start({expectedBuildId});if(freeTouch)await page.evaluate(()=>testSession.closeTouchHelp());
 await frame.evaluate(()=>{window.__qualitySyncTimes=[];const fs=__thkkdRuntime.core.FS,sync=fs.syncfs;fs.syncfs=(populate,callback)=>{if(!populate)__qualitySyncTimes.push(performance.now());return sync.call(fs,populate,callback);};});
 for(let i=0;i<8&&(await nativeState(frame)).outer!==100;i++)await key('z');assert.equal((await nativeState(frame)).outer,100);
 await page.keyboard.down('z');await key('ArrowRight',250,30);await key('x',80,30);await page.keyboard.up('z');
 report.beforeRestart=await nativeState(frame);await key('Escape');await key('r');report.restarted=await nativeState(frame);assert.equal(report.restarted.gameScene,0);assert.ok(report.restarted.gameFrame<report.beforeRestart.gameFrame);report.checks.push('native-pause-R-restarts-current-run');
 if(gamepad){await page.evaluate(()=>{testPads[0].buttons[0]={pressed:true,value:1};testPads[0].buttons[4]={pressed:true,value:1};testPads[0].axes[0]=1;});await wait(500);await page.evaluate(()=>{for(const button of testPads[0].buttons){button.pressed=false;button.value=0;}testPads[0].axes[0]=0;});await wait(80);}
 else if(freeTouch){
   const box=await page.$eval('#touchDirectSurface',element=>{const r=element.getBoundingClientRect();return {x:r.x+r.width*.5,y:r.y+r.height*.6};}),cdp=await page.createCDPSession();
   const point=x=>({x,y:box.y,id:21,radiusX:4,radiusY:4,force:1});await cdp.send('Input.dispatchTouchEvent',{type:'touchStart',touchPoints:[point(box.x)]});await cdp.send('Input.dispatchTouchEvent',{type:'touchMove',touchPoints:[point(box.x+85)]});await wait(160);await cdp.send('Input.dispatchTouchEvent',{type:'touchEnd',touchPoints:[]});
   report.touchRecorded=await frame.evaluate(()=>({unlimited:__thkkdRuntime.input.unlimitedUsed,coordinates:__thkkdRuntime.input.recordedMotion.size,marker:__thkkdRuntime.platform.renderer.commands.some(command=>command.texture?.text?.includes('处理落率 100%'))}));assert.ok(report.touchRecorded.unlimited&&report.touchRecorded.coordinates>0&&report.touchRecorded.marker);
 }else{await page.keyboard.down('z');await wait(500);await page.keyboard.up('z');}
 await key('Escape');await key('ArrowDown');await key('z');
 const replayMenu=()=>frame.evaluate(()=>{const w=address=>__thkkdRuntime.core._thkkd_word(address);return {state:w(0xb41215),selected:w(0xa87f40),nameCursor:w(0xb43046),phase:w(0xa87f48),outer:w(0xa8ce58)};});
 report.saveMenus=[await replayMenu()];assert.equal(report.saveMenus.at(-1).state,3,'原生保存录像询问');
 await confirm();report.saveMenus.push(await replayMenu());assert.equal(report.saveMenus.at(-1).state,1,'原生录像槽选择');
 await confirm();report.saveMenus.push(await replayMenu());assert.equal(report.saveMenus.at(-1).state,2,'原生署名');
 await confirm();assert.equal((await replayMenu()).nameCursor,1,'Z 输入署名字符');await key('ArrowLeft');assert.equal((await replayMenu()).selected,85);
 await runtimeCommand(page,'sync');const beforeSaveSyncs=await frame.evaluate(()=>__qualitySyncTimes.length),recordedCount=await frame.evaluate(()=>__thkkdRuntime.core._thkkd_word(0xb43110));await confirm();report.saveSyncDelta=await frame.evaluate(before=>__qualitySyncTimes.length-before,beforeSaveSyncs);if(!observe)assert.ok(report.saveSyncDelta>0,'原生文件关闭触发独立同步');
 const savedPath=`replay/thkkd${String(report.saveMenus[1].selected).padStart(2,'0')}.${freeTouch?'thkkdr':'rpy'}`,listed=await runtimeCommand(page,'list'),saved=listed.files.find(file=>file.path===savedPath);assert.ok(saved,'原生保存器实际生成所选槽位的录像');const exported=(await runtimeCommand(page,'read',{path:saved.path})).bytes,decodedFile=await decodeReplay(Uint8Array.from(exported)),decoded=nativeReplay(decodedFile.native);assert.equal(decoded.count,recordedCount,'读取本轮刚录制的录像');assert.equal(decoded.description.name,'A');report.saved={path:saved.path,bytes:exported.length,count:decoded.count,description:decodedFile.description};report.checks.push('Z-save-prompt-slot-name-and-native-replay-export');
 if(freeTouch){assert.equal(decodedFile.description.slowdown,100);assert.ok(decodedFile.motion.size>0);report.checks.push('TOUCH-10-unlimited-motion-saved-with-100-percent-marker','TOUCH-13-touch-confirms-native-result-slot-and-name');}
 if(gamepad){const inputs=exported.slice(0,decoded.count);report.recordedGamepadFrames=inputs.filter(value=>(value&4)&&(value&32)&&(value&128)).length;assert.ok(report.recordedGamepadFrames>0,'原生录像记录手柄方向、射击与低速的实际输入');report.checks.push('REPLAY-03-production-gamepad-input-records-in-native-rpy');}
 const persisted=await frame.evaluate(path=>new Promise((resolve,reject)=>{const request=indexedDB.open('/savesthkkd');request.onerror=()=>reject(request.error);request.onsuccess=()=>{const db=request.result,transaction=db.transaction('FILE_DATA','readonly'),get=transaction.objectStore('FILE_DATA').get('/savesthkkd/'+path);get.onsuccess=()=>resolve(get.result?.contents?[...get.result.contents]:null);get.onerror=()=>reject(get.error);transaction.oncomplete=()=>db.close();};}),saved.path);report.saveImmediatelyPersisted=JSON.stringify(persisted)===JSON.stringify(exported);if(!observe)assert.ok(report.saveImmediatelyPersisted,'原生保存结束立即进入独立 IndexedDB 持久化记录');
 const importedPath=`replay/imported.${freeTouch?'thkkdr':'rpy'}`;await runtimeCommand(page,'write',{path:importedPath,bytes:exported});await runtimeCommand(page,'playback',{action:'open',path:importedPath});await runtimeCommand(page,'playback',{action:'pause'});await runtimeCommand(page,'playback',{action:'seek',value:Math.min(20,decoded.count-2)});await frame.waitForFunction(target=>__thkkdRuntime.core._thkkd_word(0xb43110)===target,{},Math.min(20,decoded.count-2));report.playback=await nativeState(frame);
 await page.keyboard.down('ArrowRight');await page.keyboard.down('x');await wait(160);assert.equal((await nativeState(frame)).cursor,report.playback.cursor);await page.keyboard.up('ArrowRight');await page.keyboard.up('x');report.checks.push('native-replay-import-seek-and-live-input-isolation');
 const invalid=decodedFile.native.slice();new DataView(invalid.buffer,invalid.length-62,62).setUint32(12,8,true);await runtimeCommand(page,'write',{path:'replay/invalid.rpy',bytes:[...invalid]});await assert.rejects(()=>runtimeCommand(page,'playback',{action:'inspect',path:'replay/invalid.rpy'}),/元数据无效/);assert.equal((await nativeState(frame)).cursor,report.playback.cursor);report.checks.push('invalid-replay-inspection-reports-error-and-keeps-current-playback');
 await page.reload({waitUntil:'networkidle0'});frame=await driver.start({expectedBuildId});assert.deepEqual((await runtimeCommand(page,'read',{path:saved.path})).bytes,exported);report.checks.push('refresh-retains-original-replay-bytes');
 assert.equal(report.errors.length,0);report.result=observe?'observed':'passed';console.log(JSON.stringify({result:report.result,checks:report.checks,saved:report.saved,saveImmediatelyPersisted:report.saveImmediatelyPersisted}));
}catch(error){report.result='failed';report.error=error.stack;process.exitCode=1;console.error(error);await page.screenshot({path:output+'failure.png'});}
finally{await driver.collect();writeFileSync(output+'result.json',JSON.stringify(report,null,2));await browser.close();}
