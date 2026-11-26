import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {productionBrowser,nativeState,observeCadence,wait} from './production-driver.mjs';
const args=Object.fromEntries(process.argv.slice(2).map(value=>{const at=value.indexOf('=');return [value.slice(2,at),value.slice(at+1)];}));
const url=args.url||'http://127.0.0.1:8177/',output=fileURLToPath(new URL('../build/quality-mp/'+(args.label||'candidate-default')+'/',import.meta.url));mkdirSync(output,{recursive:true});
const report={schema:'thkkd/quality-browser/2',url,checks:[],cadences:[]};
const driver=await productionBrowser({url,report,unlimited:!!args.unlimited,headed:!!args.headed,nativeViewport:args['native-window']==='1',launchArgs:args['window-position']?[`--window-position=${args['window-position']}`,'--window-size=1200,900']:[]}),{browser,page,key}=driver;
try{
 await driver.library();
 const openSettings=async()=>{
   await page.click('#gameSettingsOpen');
   await page.waitForSelector('#displayOptions > button');
   if(await page.$eval('#displayOptions > button',element=>element.getAttribute('aria-expanded'))!=='true')await page.click('#displayOptions > button');
   await page.waitForSelector('#frameLimitToggle');
 };
 await openSettings();
 const toggle=()=>page.$eval('#frameLimitToggle',element=>element.getAttribute('aria-checked'));
 report.defaultHighRefresh=await toggle();assert.equal(report.defaultHighRefresh,'false');
 if(args.high){
   if(await page.$eval('#displayOptions > button',element=>element.getAttribute('aria-expanded'))!=='true')await page.click('#displayOptions > button');
   await page.click('#frameLimitToggle');assert.equal(await toggle(),'true');
   await page.reload({waitUntil:'networkidle0'});await openSettings();assert.equal(await toggle(),'true');report.checks.push('high-refresh-setting-survives-reload');
 }
 await page.click('#libraryBack');
 await page.waitForSelector('#gameSettingsOpen',{visible:true});
 const frame=await driver.start();await wait(1100);report.initial=await nativeState(frame);
 const cadence=async scene=>{const result={scene,...await observeCadence(frame)};report.cadences.push(result);console.log(JSON.stringify({scene,presentHz:result.presentHz,logicHz:result.logicHz,renderedFPS:result.renderedFPS}));return result;};
 await cadence('title');report.z=[];
 for(let i=0;i<8;i++){
   const before=await nativeState(frame);await key('z');const after=await nativeState(frame);report.z.push({before,after});
   if(i===0)await cadence('difficulty');if(after.outer===100)break;
 }
 assert.equal((await nativeState(frame)).outer,100,'默认 Z 连续确认进入游戏');
 await page.keyboard.down('z');await wait(400);report.shooting=await frame.evaluate(()=>{const w=address=>__thkkdRuntime.core._thkkd_word(address);return {active:Array.from({length:20},(_,i)=>w(0xa87be0+i*24)===1).filter(Boolean).length+Array.from({length:80},(_,i)=>w(0xa79090+i*56)===1).filter(Boolean).length};});await page.keyboard.up('z');assert.ok(report.shooting.active>0,'物理 Z 生成原生子弹');report.checks.push('default-Z-menu-and-actual-shots');
 await cadence('gameplay');await key('Escape');assert.equal((await nativeState(frame)).gameScene,1);await cadence('pause');await key('z');assert.equal((await nativeState(frame)).gameScene,0,'暂停菜单 Z 恢复');report.checks.push('Z-pause-confirm');
 await page.screenshot({path:output+'gameplay.png'});
 for(const sample of report.cadences){assert.ok(sample.logicHz>54&&sample.logicHz<66,`${sample.scene} 逻辑约 60 Hz`);if(!args.high)assert.ok(sample.presentHz>54&&sample.presentHz<66,`${sample.scene} 呈现约 60 Hz`);if(args.high&&args.unlimited)assert.ok(sample.presentHz>75,`${sample.scene} 实际额外呈现`);assert.ok(Math.abs(parseFloat(sample.renderedFPS)-sample.presentHz)<Math.max(8,sample.presentHz*.15),`${sample.scene} 屏幕 FPS 对应实际呈现`);}
 assert.equal(report.errors.length,0);report.result='passed';console.log(JSON.stringify({result:report.result,checks:report.checks,shooting:report.shooting,runtime:report.runtime.url}));
}catch(error){report.result='failed';report.error=error.stack;process.exitCode=1;console.error(error);await page.screenshot({path:output+'failure.png'});}
finally{await driver.collect();writeFileSync(output+'result.json',JSON.stringify(report,null,2));await browser.close();}
