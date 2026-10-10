import assert from 'node:assert/strict';
import {launchBrowser} from '../../../../th15-eagler/th10_web/scripts/native/browser-launch.mjs';
const browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
const page=await browser.newPage(),errors=[];page.on('pageerror',e=>{errors.push(String(e));console.error(String(e));});page.on('console',m=>{if(m.type()==='error')console.error(m.text());});
try{
 await page.goto('http://127.0.0.1:8145/');await page.waitForFunction(()=>window.presentationLab);await page.click('#boot');
 await page.waitForFunction(()=>window.presentationLab?.controller,null,{timeout:120000});
 const result=await page.evaluate(async()=>{
  const ctl=presentationLab.controller,c=ctl.runtime.core,driver=ctl.driver;ctl.freeze();
  const blocked=driver.advanceOneTick();if(blocked.status!=='blocked-loading')throw Error('Expected loading receipt '+JSON.stringify(blocked));
  await new Promise(r=>setTimeout(r,1100));
  const state=()=>Array.from(c.HEAP32.subarray(c._th15_probe_state()/4,c._th15_probe_state()/4+16));
  const tick=()=>{const receipt=driver.advanceOneTick();if(receipt.status!=='advanced')throw Error(JSON.stringify(receipt));};
  const menu=n=>{for(let i=0;i<350;i++){tick();if(state()[8]===n&&state()[9]===2)return;}throw Error('Menu '+n+' timed out '+state());};
  const enter=()=>{driver.setInput({code:'Enter',down:true});tick();driver.setInput({code:'Enter',down:false});tick();};
  menu(1);enter();menu(5);enter();menu(6);enter();menu(7);enter();
  for(let i=0;i<300&&c._th15_phase()!==1;i++){const r=driver.advanceOneTick();if(r.status==='blocked-loading'){await new Promise(resolve=>setTimeout(resolve,1100));i--;continue;}if(r.status!=='advanced')throw Error(JSON.stringify(r));}
  // Finish resources without fabricating progress through their wall-clock buffer.
  for(let i=0;i<245;i++){let r=driver.advanceOneTick();if(r.status==='blocked-loading'){await new Promise(resolve=>setTimeout(resolve,1100));i--;continue;}if(r.status!=='advanced')throw Error(JSON.stringify(r));}
  c._th15_probe_protection(100000);driver.setInput({code:'ArrowRight',down:true});tick();tick();driver.setInput({code:'ArrowRight',down:false});
  const before=c._th15_frame(),normal=ctl.sweep(),fault=ctl.sweep({negativeControl:true});
  if(c._th15_frame()!==before)throw Error('Sweep advanced simulation');
  const token=ctl.freezeToken;ctl.play();let rejected=false;try{driver.resume(token);}catch{rejected=true;}
  if(!rejected)throw Error('Stale token accepted');
  return {normal:normal.objects.map(o=>({status:o.status,fields:o.fields})),purity:normal.purityStatus,fault:fault.objects.map(o=>o.status),tick:before,loadingReceipt:blocked};
 });
 assert.equal(result.purity,'unknown');assert.equal(result.normal[0]?.status,'interpolated');assert.notEqual(result.fault[0],'interpolated');assert.deepEqual(errors,[]);console.log(JSON.stringify(result));
 await page.click('#inspect');await page.keyboard.press('F8');await page.waitForFunction(()=>document.getElementById('status').textContent.includes('已锁定并保存'));
 await page.getByRole('button',{name:'限制 60 Hz',exact:true}).click();
 assert.equal(await page.getByRole('button',{name:'限制 60 Hz',exact:true}).getAttribute('class'),'primary');
 await page.getByRole('button',{name:'高刷（默认）',exact:true}).click();
 await page.screenshot({path:new URL('../../th15_web/artifacts/presentation-lab/workbench.png',import.meta.url).pathname.replace(/^\/([A-Za-z]:)/,'$1'),fullPage:true});
}finally{await browser.close();}
