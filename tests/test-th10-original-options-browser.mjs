// Real Launcher and TH10 Runtime. Use a fresh browser store and prepared private
// content. Exercises native configuration, byte persistence, deletion and reload.
import assert from 'node:assert/strict';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../lib/chromium-executable.mjs';
const base=process.env.EAGLER_TH10_TEST_URL||'http://127.0.0.1:8134/';
const hintFile=resolve(process.env.EAGLER_TH10_HINT_FILE||'tests/fixtures/th10-hint-user.txt');
const expected=Array.from(await readFile(hintFile));
const out=resolve(process.env.EAGLER_TH10_TEST_OUTPUT||'.codex-tmp/hint-evidence');await mkdir(out,{recursive:true});
const browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--no-first-run','--disable-extensions','--autoplay-policy=no-user-gesture-required','--use-angle=swiftshader','--enable-unsafe-swiftshader']});
const report={},errors=[];
try{
 const page=await browser.newPage();await page.setViewport({width:1280,height:1000});
 await page.emulateMediaFeatures([{name:'prefers-reduced-motion',value:'reduce'}]);
 page.on('pageerror',error=>errors.push(String(error)));
 // Older workspace maps use th10 rather than the canonical th10-eagler path.
 // Redirect only test acquisition, without changing production workspace config.
 if(process.env.EAGLER_TH10_CANONICAL_RUNTIME==='1'){
  await page.setRequestInterception(true);page.on('request',request=>{
   const url=request.url();request.continue(url.includes('/th10/build-eagler/')?{url:url.replace('/th10/build-eagler/','/th10-eagler/build-eagler/')}:{});
  });
 }
 async function select(){
  await page.waitForFunction(()=>window.__eaglerBoot?.done===true);
  if(await page.$eval('#firstUseNoticeDialog',e=>e.open)){
   await page.$eval('#firstUseNoticeCloseHint',e=>e.click());await page.waitForFunction(()=>!document.querySelector('#firstUseNoticeDialog').open);
  }
  await page.$eval('.game[data-game="th10"]',e=>e.click());
  await page.waitForSelector('#fileOptions',{visible:true});
 }
 async function launch(){
  await page.evaluate(()=>{window.th10FirstFrame=false;window.addEventListener('message',event=>{if(event.data?.game==='th10'&&event.data.event==='first-frame')window.th10FirstFrame=true;});});
  await page.$eval('#launch',e=>e.click());
  await page.waitForFunction(()=>{
   const decision=document.querySelector('#decisionDialog[open]');
   if(decision&&!decision.classList.contains('closing'))document.querySelector('#decisionConfirm')?.click();
   return window.th10FirstFrame===true;
  },{timeout:60000}).catch(async error=>{
   console.log('Launch diagnostic',await page.evaluate(()=>({status:document.querySelector('#playerStatus')?.textContent,decision:document.querySelector('#decisionDialog[open]')?.textContent,frames:Array.from(document.querySelectorAll('iframe')).map(e=>e.src)})));
   console.log('Native diagnostic',await Promise.all(page.frames().map(frame=>frame.evaluate(()=>({url:location.href,error:document.querySelector('#error')?.textContent,app:window.__th10Runtime?.app})).catch(()=>null))));
   throw error;
  });
  const runtime=page.frames().find(frame=>frame.url().includes('th10.html'));assert.ok(runtime);
  await runtime.waitForFunction(()=>window.__th10Runtime?.app>0,{timeout:30000});return runtime;
 }
 async function config(runtime){
  await runtime.evaluate(()=>window.__th10Runtime.command({command:'sync'}));
  return runtime.evaluate(()=>Array.from(Module.FS.readFile('/savesth10/jp/th10.cfg')));
 }
 await page.goto(base);await select();
 assert.equal(await page.$eval('#faithBarOption',e=>e.hidden),false);
 assert.equal(await page.$$eval('#hintFileTool [data-action]',es=>es.map(e=>e.dataset.action)).then(actions=>actions.join(',')),'import-hint,delete-hint');
 await page.$eval('#faithBarToggle',e=>e.click());
 assert.equal(await page.$eval('#faithBarToggle',e=>e.getAttribute('aria-checked')),'true');
 await page.select('#musicSelect','none');
 const [chooser]=await Promise.all([page.waitForFileChooser(),page.click('#hintFileTool [data-action="import-hint"]')]);await chooser.accept([hintFile]);
 await page.waitForFunction(()=>document.querySelector('#status')?.textContent.includes('已导入'),{timeout:120000});
 await page.reload();await select();
 assert.equal(await page.$eval('#faithBarToggle',e=>e.getAttribute('aria-checked')),'true');
 const buttonStyles=await page.evaluate(()=>{
  const style=selector=>{const value=getComputedStyle(document.querySelector(selector));return ['height','borderRadius','padding','fontSize','fontWeight','gap'].map(name=>value[name]);};
  return ['#saveFileTool [data-action="import-save"]','#hintFileTool [data-action="import-hint"]','#hintFileTool [data-action="delete-hint"]'].map(style);
 });
 assert.deepEqual(buttonStyles[1],buttonStyles[0]);assert.deepEqual(buttonStyles[2],buttonStyles[0]);
 report.buttons='existing-file-control-style';
 await page.screenshot({path:resolve(out,'settings.png')});
 let runtime=await launch();
 assert.deepEqual(await runtime.evaluate(()=>Array.from(Module.FS.readFile('/savesth10/jp/hint/hint_user.txt'))),expected);
 const on=await config(runtime);assert.ok(on[0x30]&0x80);assert.equal(on[0x22],1);
 report.upload={bytes:expected.length,reloaded:true,originalFaithFlag:true,originalHintMode:1};
 console.log('TH10 uploaded Hint and native configuration: PASS');
 // Enter Stage 1 through the native menu at the original fixed tick cadence.
 const stage=await runtime.evaluate(()=>{
  const rt=window.__th10Runtime,c=rt.core;c.sdl_loop_stop();
  const tick=n=>{for(let i=0;i<n;i++)c.sdl_loop_tick(rt.app,1/60,17);};
  const key=code=>{const bytes=new TextEncoder().encode(code+'\0'),p=c.graphics_allocate(bytes.length);new Uint8Array(c.memory.buffer,p,bytes.length).set(bytes);c.sdl_key(p,1);tick(3);c.sdl_key(p,0);c.graphics_free(p);tick(60);};
  tick(330);for(let i=0;i<5;i++)key('KeyZ');tick(120);
  c.sdl_loop_start(rt.app);return rt.status();
 });
 assert.equal(stage[1],1);assert.equal(stage[2],0);report.stage1=true;
 await (await runtime.$('canvas')).screenshot({path:resolve(out,'stage1-hint.png')});
 // A new page reload flushes the old frame's saved state before the delete action.
 await runtime.evaluate(()=>window.__th10Runtime.command({command:'sync'}));
 await page.reload();await select();
 await page.click('#hintFileTool [data-action="delete-hint"]');
 await page.waitForFunction(()=>document.querySelector('#status')?.textContent==='已删除 Hint',{timeout:120000});
 await page.$eval('#faithBarToggle',e=>e.click());
 await page.reload();await select();
 assert.equal(await page.$eval('#faithBarToggle',e=>e.getAttribute('aria-checked')),'false');
 runtime=await launch();
 const off=await config(runtime);assert.equal(off[0x30]&0x80,0);assert.equal(off[0x22],0);
 assert.equal(await runtime.evaluate(()=>['hint_user.txt','hint_auto.txt'].some(name=>Module.FS.analyzePath('/savesth10/jp/hint/'+name).exists)),false);
 report.delete={reloaded:true,originalFaithFlag:false,originalHintMode:0};
 await assert.rejects(runtime.evaluate(()=>window.__th10Runtime.command({command:'write',path:'hint/../th10.cfg',bytes:[1]})),/Invalid save path/);
 await assert.rejects(runtime.evaluate(()=>window.__th10Runtime.command({command:'write',path:'hint/other.txt',bytes:[1]})),/Invalid save path/);
 // Chinese localization retains the Japanese core, but native file access must
 // use the shell's CHS save root. Otherwise uploaded Hint files never reach it.
 // Use a fresh shell, matching the Launcher lifecycle (one native instance per
 // iframe). Reconstructing the SDL core inside an old instance is unsupported.
 const localizedPage=await browser.newPage();await localizedPage.goto(base);
 await localizedPage.evaluate(async()=>{
  const manifest=await (await fetch('/host-manifest.json')).json();
  const data=new URL(manifest.games.th10.gameData.source,location.href).href;
  window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await (await fetch(data)).arrayBuffer()});
  const frame=document.createElement('iframe');frame.id='hintChineseRuntime';
  frame.src='/th10-eagler/build-eagler/th10.html?managedData=1&gameGeneration=hint-test&runtimeEpoch=1&language=lang_zh-hans';
  document.body.append(frame);
 });
 await localizedPage.waitForFunction(()=>document.querySelector('#hintChineseRuntime')?.contentWindow?.__th10Runtime?.app===0,{timeout:60000});
 const localized=localizedPage.frames().find(frame=>frame.url().includes('th10.html'));
 const chinese=await localized.evaluate(async bytes=>{
  const rt=window.__th10Runtime,manifest=await (await fetch('/host-manifest.json')).json();
  await rt.command({command:'configure',language:'lang_zh-hans',music:'none',options:{faithBarEnabled:true},sharedResources:[
   {path:'/msgothic.ttc',url:new URL(manifest.shared.vanillaFont,location.origin+'/').href},
   {path:'/unifont.otf',url:new URL(manifest.shared.unicodeFont,location.origin+'/').href},
  ]});
  await rt.command({command:'write',path:'hint/hint_user.txt',bytes});
  await rt.command({command:'launch'});await rt.command({command:'sync'});
  const cfg=Module.FS.readFile('/savesth10/chs/th10.cfg');
  return {faithFlag:!!(cfg[0x30]&0x80),hintMode:cfg[0x22],bytes:Array.from(Module.FS.readFile('/savesth10/chs/hint/hint_user.txt'))};
 },expected);
 await localizedPage.close();
 assert.equal(chinese.faithFlag,true);assert.equal(chinese.hintMode,1);assert.deepEqual(chinese.bytes,expected);
 report.chineseSaveRoot=true;
 assert.deepEqual(errors,[]);report.pageErrors=errors;
 await writeFile(resolve(out,'report.json'),JSON.stringify(report,null,2)+'\n');console.log(JSON.stringify(report));
}finally{await browser.close();}
