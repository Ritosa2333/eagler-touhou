import assert from 'node:assert/strict';
import {resolve} from 'node:path';
import {launchBrowser} from '../th10_web/scripts/native/browser-launch.mjs';
const browser=await launchBrowser();
try{
 const page=await browser.newPage({viewport:{width:1280,height:900}}),errors=[];
 page.on('pageerror',e=>errors.push(String(e)));
 await page.goto('http://127.0.0.1:8148/?game=th15');
 await page.waitForFunction(()=>window.__eaglerBoot?.done===true);
 await page.evaluate(()=>document.querySelector('#firstUseNoticeDialog')?.close());
 await page.evaluate(()=>{const card=document.querySelector('.game[data-game=th15]');card.click();card.click();});
 await page.waitForFunction(()=>!document.querySelector('#thpracToggle').disabled);
 await page.evaluate(()=>{const b=document.querySelector('#thpracToggle');if(b.getAttribute('aria-checked')!=='true')b.click();});
 await page.waitForFunction(()=>document.querySelector('#thpracToggle').getAttribute('aria-checked')==='true');
 assert.equal(await page.locator('#thpracToggle').getAttribute('aria-checked'),'true');
 await page.selectOption('#musicSelect','none',{force:true});
 await page.evaluate(()=>document.querySelector('#launch').click());
 let runtime;
 for(let i=0;i<240;i++){
  await page.evaluate(()=>{const d=document.querySelector('#decisionDialog[open]');if(d&&!d.classList.contains('closing'))document.querySelector('#decisionConfirm')?.click();});
  runtime=page.frames().find(f=>f.url().includes('/th15.html'));
  const error=await runtime?.evaluate(()=>document.querySelector('#error')?.textContent).catch(()=>'');
  if(error)throw Error(error);
  if(runtime&&await runtime.evaluate(()=>globalThis.Module?._th15_frame()>160).catch(()=>false))break;
  if(i===239)throw Error('Launch timed out: '+await page.locator('#playerStatus').textContent());
  await new Promise(r=>setTimeout(r,250));
 }
 assert.equal(await runtime.evaluate(()=>Module.eaglerOptions.thpracEnabled),true);
 assert.equal(await runtime.evaluate(()=>Module.FS.stat('/unifont.otf').size),5105508);
 await page.evaluate(()=>{window.previewEvents=[];window.addEventListener('message',e=>{if(e.data?.protocol==='eagler-touhou/1')previewEvents.push(e.data);});});
 const rpc=async(command,fields)=>page.evaluate(({command,fields})=>new Promise((resolve,reject)=>{
  const frame=document.querySelector('#gameFrame'),epoch=Number(new URL(frame.src).searchParams.get('runtimeEpoch')),request=crypto.randomUUID();
  const timer=setTimeout(()=>reject(Error('RPC timeout '+command)),10000);
  const listen=e=>{if(e.source!==frame.contentWindow||e.data?.request!==request)return;window.removeEventListener('message',listen);clearTimeout(timer);e.data.ok?resolve(e.data):reject(Error(e.data.error));};
  window.addEventListener('message',listen);frame.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th15',epoch,command,request,...fields},location.origin);
 }),{command,fields});
 await rpc('keyboard',{code:'F12',down:true});await new Promise(r=>setTimeout(r,100));await rpc('keyboard',{code:'F12',down:false});
 await new Promise(r=>setTimeout(r,200));
 await page.screenshot({path:resolve(import.meta.dirname,'../th15_web/artifacts/thprac-purple-browser-check/launcher-protocol-f12.png')});
 const key=async code=>{await rpc('keyboard',{code,down:true});await new Promise(r=>setTimeout(r,100));await rpc('keyboard',{code,down:false});await new Promise(r=>setTimeout(r,900));};
 await key('F12');
 for(let i=0;i<6&&await runtime.evaluate(()=>Module._th15_phase()===0);i++)await key('KeyZ');
 assert.equal(await runtime.evaluate(()=>Module._th15_phase()),1);
 await key('F12');
 const tap=async(x,y)=>{
  await runtime.evaluate(({x,y})=>Module._th15_thprac_mouse(1,x,y),{x,y});
  await new Promise(r=>setTimeout(r,100));
  await runtime.evaluate(({x,y})=>Module._th15_thprac_mouse(2,x,y),{x,y});
  await new Promise(r=>setTimeout(r,150));
 };
 await tap(100,73);await tap(18,174);
 await key('F12');
 await rpc('keyboard',{code:'KeyZ',down:true});await rpc('keyboard',{code:'ArrowRight',down:true});
 await new Promise(r=>setTimeout(r,100));
 await page.screenshot({path:resolve(import.meta.dirname,'../th15_web/artifacts/thprac-purple-browser-check/launcher-protocol-key-hud.png')});
 await rpc('keyboard',{code:'KeyZ',down:false});await rpc('keyboard',{code:'ArrowRight',down:false});
 await key('Backspace');
 await key('KeyU');await key('F1');
 await page.screenshot({path:resolve(import.meta.dirname,'../th15_web/artifacts/thprac-purple-browser-check/launcher-protocol-backspace-u.png')});
 await rpc('sync',{});
 const files=(await rpc('list',{})).files;
 assert.ok(files.some(f=>f.path==='th15.cfg'));
 assert.deepEqual(errors,[]);
 console.log(JSON.stringify({passed:true,runtime:runtime.url(),thprac:true,fontMounted:true,keyboardRpc:true,saveRpc:true}));
}finally{await browser.close();}
