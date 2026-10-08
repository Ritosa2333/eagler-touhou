import test from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,sep} from 'node:path';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const site=process.env.TH15_HOST_SITE;
test('TH15 real assembled Launcher selects and installs each prepared language',{timeout:240000,skip:!site},async()=>{
 const root=resolve(site),out=resolve(import.meta.dirname,'../../artifacts/thcrap-launcher');await mkdir(out,{recursive:true});
 const mime={html:'text/html',js:'text/javascript',mjs:'text/javascript',json:'application/json',css:'text/css',wasm:'application/wasm',webp:'image/webp',svg:'image/svg+xml',woff2:'font/woff2'};
 const server=createServer(async(req,res)=>{try{const pathname=decodeURIComponent(new URL(req.url,'http://local').pathname),file=resolve(root,'.'+(pathname==='/'?'/index.html':pathname));if(!file.startsWith(root+sep)){res.writeHead(403).end();return;}res.setHeader('Cross-Origin-Opener-Policy','same-origin');res.setHeader('Cross-Origin-Embedder-Policy','require-corp');res.setHeader('Content-Type',mime[file.split('.').pop()]||'application/octet-stream');res.end(await readFile(file));}catch{res.writeHead(404).end();}});
 await new Promise(r=>server.listen(0,'127.0.0.1',r));const browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']}),observations=[];
 try{for(const language of ['ja','lang_zh-hans','lang_en']){
  const page=await browser.newPage({viewport:{width:1280,height:960}}),errors=[];page.on('pageerror',e=>errors.push(String(e)));await page.goto('http://127.0.0.1:'+server.address().port+'/?game=th15');
  await page.waitForFunction(()=>document.querySelector('#languageSelect')?.options.length===3,null,{timeout:30000});
  const options=await page.locator('#languageSelect option').evaluateAll(elements=>elements.map(e=>e.value));assert.deepEqual(new Set(options),new Set(['ja','lang_zh-hans','lang_en']));
  if(await page.locator('#firstUseNoticeDialog').evaluate(e=>e.open))await page.locator('#firstUseNoticeClose').click();
  await page.locator('#languageSelect').selectOption(language,{force:true});await page.locator('#musicSelect').selectOption('none',{force:true});
  await page.locator('#launch').evaluate(element=>element.click());
  try{await page.waitForFunction(()=>[...document.querySelectorAll('iframe')].some(f=>f.src.includes('/runtime/th15/'))||document.querySelector('#decisionDialog')?.open,null,{timeout:30000});if(await page.locator('#decisionDialog').evaluate(e=>e.open))await page.locator('#decisionConfirm').click();await page.waitForFunction(()=>[...document.querySelectorAll('iframe')].some(f=>f.src.includes('/runtime/th15/')),null,{timeout:30000});}catch(e){await page.screenshot({path:resolve(out,language+'-failed.png')});throw new Error(String(e)+' '+JSON.stringify({errors,launcher:await page.locator('body').innerText()}));}
  const frame=page.frames().find(f=>f.url().includes('/runtime/th15/'))||await page.waitForEvent('framenavigated',{predicate:f=>f.url().includes('/runtime/th15/'),timeout:30000});assert(frame);
  try{await frame.waitForFunction(()=>typeof Module!=='undefined'&&Module._th15_frame?.()>150,null,{timeout:30000});}catch(e){await page.screenshot({path:resolve(out,language+'-failed.png')});const diagnostic=await frame.evaluate(()=>({module:typeof Module,error:typeof Module==='undefined'?null:Module._th15_error?Module.UTF8ToString(Module._th15_error()):null,body:document.body.innerText}));throw new Error(String(e)+' '+JSON.stringify({diagnostic,errors,launcher:await page.locator('body').innerText()}));}
  const state=await frame.evaluate(()=>({phase:Module._th15_phase(),frame:Module._th15_frame(),font:Module.FS.analyzePath('/thcrap/th15/fonts/unifont-15.1.05-subset.otf').exists}));assert.equal(state.font,language!=='ja');assert.equal(state.phase,0);assert.deepEqual(errors,[]);
  await page.screenshot({path:resolve(out,language+'.png')});observations.push({language,...state});await page.close();
 }}finally{await browser.close();await new Promise(r=>server.close(r));}
 await writeFile(resolve(out,'report.json'),JSON.stringify({passed:true,scope:'Real Launcher catalog selection, pack installation and production Runtime startup; local validation site has no OGG',observations},null,2));
});
