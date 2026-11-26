import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {productionBrowser,nativeState,runtimeCommand,wait} from './production-driver.mjs';
const observe=process.argv.includes('--observe'),touch=process.argv.includes('--touch'),url=process.env.THKKD_SITE_URL||'http://127.0.0.1:8177/',output=fileURLToPath(new URL('../build/quality-mp/dialogue-'+(touch?'touch':observe?'before':'fixed')+'/',import.meta.url));mkdirSync(output,{recursive:true});
const report={schema:'thkkd/dialogue-browser/1',touch,samples:[]},driver=await productionBrowser({url,report,touch}),{browser,page,key}=driver;
try{
 await driver.library();if(touch){await page.click('#gameSettingsOpen');if(await page.$eval('#mobileOptionsToggle',element=>element.getAttribute('aria-expanded'))!=='true')await page.click('#mobileOptionsToggle');if(await page.$eval('#touchToggle',element=>element.getAttribute('aria-checked'))!=='true')await page.click('#touchToggle');await page.click('#libraryBack');}await driver.start();if(touch&&await page.$eval('#touchHelpClose',element=>element.getClientRects().length>0).catch(()=>false))await page.click('#touchHelpClose');
 const savedata=new Uint8Array(329*4),view=new DataView(savedata.buffer);[1,2,3,0,5,4,11,13,6,0,100,80,1,0].forEach((value,index)=>view.setUint32(index*4,value,true));for(let index=14;index<329;index++)view.setUint32(index*4,2,true);
 await runtimeCommand(page,'write',{path:'th_kaikeidou01.dat',bytes:[...savedata]});await page.reload({waitUntil:'networkidle0'});const frame=await driver.start();if(touch&&await page.$eval('#touchHelpClose',element=>element.getClientRects().length>0).catch(()=>false))await page.click('#touchHelpClose');
 for(let i=0;i<8&&(await nativeState(frame)).choice!==3;i++)await key('ArrowDown');await key('z');await key('ArrowUp');for(let i=0;i<8&&(await nativeState(frame)).outer!==100;i++)await key('z');assert.equal((await nativeState(frame)).outer,100);
 report.mode='原生 Practice，一面 Easy，独立测试进度文件';
 await page.keyboard.down('z');let direction=null,lastBomb=0;const start=Date.now();
 while(Date.now()-start<210000){
   const state=await nativeState(frame);if(state.dialogue){report.entered=state;break;}
   if(state.gameScene===2)throw Error('固定输入路径在对话前进入 Game Over');assert.equal(state.error,'');
   const elapsed=Date.now()-start,next=Math.floor(elapsed/900)%2?'ArrowLeft':'ArrowRight';if(next!==direction){if(direction)await page.keyboard.up(direction);await page.keyboard.down(next);direction=next;}
   if(elapsed-lastBomb>=13000){await key('x',65,0);lastBomb=elapsed;report.samples.push(state);}
   await wait(100);
 }
 assert.ok(report.entered,'经普通关卡正常进入 Boss 对话');if(direction)await page.keyboard.up(direction);await page.keyboard.up('z');await wait(1800);
 report.before=await nativeState(frame);await key('z',80,350);report.afterZ=await nativeState(frame);report.zWorks=report.afterZ.dialogue!==report.before.dialogue;
 if(!observe){report.recorded=await frame.evaluate(({from,to})=>{const w=address=>__thkkdRuntime.core._thkkd_word(address),buffer=w(0xb43100);return Array.from({length:to-from},(_,index)=>w(buffer+from+index)&255);},{from:report.before.cursor,to:report.afterZ.cursor});assert.ok(report.recorded.some(value=>value&64),'Z 的对话确认记入原生 Replay 的推进位');}
 if(observe&&!report.zWorks){await key('Control',80,350);report.afterControl=await nativeState(frame);}
 if(touch){
   await frame.waitForFunction(()=>__thkkdRuntime.core._thkkd_word(0xa73060)>2&&__thkkdRuntime.core._thkkd_word(0xa73064)===0);report.beforeTap=await nativeState(frame);
   const point=await page.$eval('#touchDirectSurface',element=>{const rect=element.getBoundingClientRect();return {x:rect.x+rect.width*.45,y:rect.y+rect.height*.6,id:9};}),cdp=await page.createCDPSession();
   await cdp.send('Input.dispatchTouchEvent',{type:'touchStart',touchPoints:[point]});await cdp.send('Input.dispatchTouchEvent',{type:'touchEnd',touchPoints:[]});await wait(350);report.afterTap=await nativeState(frame);assert.notEqual(report.afterTap.dialogue,report.beforeTap.dialogue,'原生对话中的触控确认');
   await frame.evaluate(()=>{const r=__thkkdRuntime,i=r.input,sample=i.sample.bind(i);window.__longTouchSamples=[];i.sample=keys=>{sample(keys);__longTouchSamples.push({present:r.platform.presented,routing:i.dialogueRouting,advance:i.dialogueAdvance,pad:i.padState(),ctrl:keys[29],z:keys[44],pointers:[...i.pointers.values()].map(p=>({age:performance.now()-p.time,gesture:p.gesture}))});if(__longTouchSamples.length>160)__longTouchSamples.shift();};});
   report.beforeLong=await nativeState(frame);await cdp.send('Input.dispatchTouchEvent',{type:'touchStart',touchPoints:[{...point,id:10}]});await wait(1500);report.afterLong=await nativeState(frame);await cdp.send('Input.dispatchTouchEvent',{type:'touchEnd',touchPoints:[]});report.longSamples=await frame.evaluate(()=>__longTouchSamples);assert.notEqual(report.afterLong.dialogue,report.beforeLong.dialogue,'原生对话中的长按跳过');
 }
 if(!observe)assert.ok(report.zWorks,'普通 Z 推进原生对话脚本');await page.screenshot({path:output+'dialogue.png'});assert.equal(report.errors.length,0);report.result=observe?'observed':'passed';console.log(JSON.stringify({result:report.result,entered:report.entered,before:report.before,afterZ:report.afterZ,afterControl:report.afterControl}));
}catch(error){report.result='failed';report.error=error.stack;process.exitCode=1;console.error(error);await page.screenshot({path:output+'failure.png'});}
finally{await driver.collect();writeFileSync(output+'result.json',JSON.stringify(report,null,2));await browser.close();}
