import test from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve,sep} from 'node:path';
import {createHash} from 'node:crypto';
import {unzipSync} from 'fflate';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const project=resolve(import.meta.dirname,'../..'),runtime=resolve(project,'../build-eagler');
const languages=process.env.TH15_LANGUAGE_PACKS;
test('TH15 production protocol mounts official language ZIPs, fonts and translated title/music', {timeout:240000},async()=>{
 assert(languages&&process.env.TH15_ORIGINAL_DAT,'Set TH15_LANGUAGE_PACKS and TH15_ORIGINAL_DAT');
 const data=await readFile(process.env.TH15_ORIGINAL_DAT);assert.equal(createHash('sha256').update(data).digest('hex'),'f3802e71db30f99b86d61b6e98b8325716c5cbae892547c563dded7837862ae7');
 const packs={};for(const language of ['lang_zh-hans','lang_en']){
  const bytes=await readFile(resolve(languages,'language',language+'.zip')),entries=unzipSync(bytes),manifest=JSON.parse(new TextDecoder().decode(entries['manifest.json']));
  packs[language]={url:'/pack/'+language,language,bytes:bytes.length,manifest,files:manifest.files.map(f=>({path:f.path,bytes:Array.from(entries[f.path.slice(1)])}))};
 }
 const parent=`<!doctype html><body><iframe id="runtime" style="width:1280px;height:960px;border:0"></iframe><script>
 window.events=[];window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await fetch('/th15.dat').then(r=>r.arrayBuffer())});
 window.addEventListener('message',e=>{if(e.data.event)events.push(e.data)});
 window.rpc=(command,args={})=>new Promise((ok,no)=>{const request=crypto.randomUUID(),f=document.querySelector('iframe'),timer=setTimeout(()=>no(Error('RPC timeout '+command)),30000);const listener=e=>{if(e.data.request!==request)return;clearTimeout(timer);removeEventListener('message',listener);e.data.ok?ok(e.data):no(Error(e.data.error));};addEventListener('message',listener);f.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th15',epoch:1,command,request,...args},location.origin)});
 document.querySelector('iframe').src='/runtime/th15.html?runtimeEpoch=1&managedData=1&gameGeneration=fixture';</script>`;
 const server=createServer(async(req,res)=>{try{
  const path=new URL(req.url,'http://local').pathname;res.setHeader('Cross-Origin-Opener-Policy','same-origin');res.setHeader('Cross-Origin-Embedder-Policy','require-corp');
  if(path==='/'){res.setHeader('Content-Type','text/html');res.end(parent);return;}
  if(path==='/th15.dat'){res.end(data);return;}
  if(path.startsWith('/pack/')){res.setHeader('Content-Type','application/json');res.end(JSON.stringify(packs[path.slice(6)]));return;}
  const file=resolve(runtime,'.'+path.slice('/runtime'.length));if(!path.startsWith('/runtime/')||!file.startsWith(runtime+sep)){res.writeHead(404).end();return;}
  res.setHeader('Content-Type',file.endsWith('.mjs')?'text/javascript':file.endsWith('.wasm')?'application/wasm':file.endsWith('.html')?'text/html':'application/octet-stream');res.end(await readFile(file));
 }catch(e){res.writeHead(500).end(String(e));}});
 await new Promise(r=>server.listen(0,'127.0.0.1',r));const url='http://127.0.0.1:'+server.address().port,output=resolve(project,'artifacts/thcrap-production');await mkdir(output,{recursive:true});
 const browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});const observations=[];
 try{for(const language of ['ja','lang_zh-hans','lang_en']){
  const page=await browser.newPage({viewport:{width:1320,height:1000}}),errors=[];page.on('pageerror',e=>errors.push(String(e)));await page.goto(url);await page.waitForFunction(()=>events.some(e=>e.event==='ready'),null,{timeout:45000});
  await page.evaluate(async language=>{let runtimePack;if(language!=='ja'){runtimePack=await fetch('/pack/'+language).then(r=>r.json());runtimePack.files=runtimePack.files.map(f=>({...f,bytes:new Uint8Array(f.bytes)}));}await rpc('configure',{language,music:'none',options:{touchEnabled:false},runtimePack});},language);
  const frame=page.frames().find(f=>f.url().includes('/runtime/'));
  // Capture the actual published startup canvas before synchronous resource
  // preparation replaces it; do not add a wait to the production launcher.
  await frame.evaluate(()=>{const original=Module._th15_prepare_loading;Module._th15_prepare_loading=(...args)=>{const result=original(...args);window.startupImage=Module.canvas.toDataURL('image/png');return result;};});
  await page.evaluate(()=>rpc('launch'));
  const startupImage=await frame.evaluate(()=>window.startupImage);
  assert.ok(startupImage?.startsWith('data:image/png;base64,'));
  await writeFile(resolve(output,language+'-loading.png'),Buffer.from(startupImage.split(',')[1],'base64'));
  await page.waitForFunction(()=>events.some(e=>e.event==='first-frame'),null,{timeout:45000});
  await frame.waitForFunction(()=>Module._th15_phase()===0&&Module._th15_frame()>150,null,{timeout:30000});
  const state=await frame.evaluate(()=>({phase:Module._th15_phase(),frame:Module._th15_frame(),probes:typeof Module._application_test_protection,packFont:Module.FS.analyzePath('/thcrap/th15/fonts/unifont-15.1.05-subset.otf').exists}));
  assert.equal(state.probes,'undefined');assert.equal(state.packFont,language!=='ja');
  if(language!=='ja')assert.equal(await frame.evaluate(()=>Module.FS.readFile('/thcrap/th15/runtime-language.txt',{encoding:'utf8'})),language);
  await page.screenshot({path:resolve(output,language+'-title.png')});
  const wait=async n=>{const tick=await frame.evaluate(()=>Module._th15_frame());await frame.waitForFunction(t=>Module._th15_frame()>=t,tick+n,{timeout:20000});};
  const key=async code=>{await page.evaluate(code=>rpc('keyboard',{code,down:true}),code);await wait(3);await page.evaluate(code=>rpc('keyboard',{code,down:false}),code);await wait(3);};
  for(let i=0;i<3;i++)await key('ArrowDown');await key('KeyZ');await wait(90);await page.screenshot({path:resolve(output,language+'-player-data.png')});
  await key('KeyZ');await wait(30);await page.screenshot({path:resolve(output,language+'-player-spells.png')});
  await key('Escape');await wait(60);await key('ArrowDown');await key('KeyZ');await wait(90);await page.screenshot({path:resolve(output,language+'-music.png')});
  // Track four reproduces the reported long English prose; confirm the actual
  // production renderer, then leave Music Room to catch leaked font resizing.
  for(let i=0;i<3;i++)await key('ArrowDown');await key('KeyZ');await wait(30);await key('KeyZ');await wait(60);
  await page.screenshot({path:resolve(output,language+'-music-long-details.png')});
  await key('Escape');await wait(60);
  for(let i=0;i<4;i++)await key('ArrowUp');await key('KeyZ');await wait(60);
  await key('KeyZ');await wait(60);await key('KeyZ');await wait(60);await key('KeyZ');
  await frame.waitForFunction(()=>Module._th15_phase()===1,null,{timeout:30000});await wait(210);
  await page.screenshot({path:resolve(output,language+'-stage-bgm.png')});
  const fatalEvents=await page.evaluate(()=>events.filter(e=>e.event==='error'));assert.deepEqual(fatalEvents,[]);
  assert.deepEqual(errors,[]);observations.push({language,...state});await page.close();
 }}finally{await browser.close();await new Promise(r=>server.close(r));}
 await writeFile(resolve(output,'report.json'),JSON.stringify({passed:true,scope:'Production Runtime protocol/startup canvas/title/Music Room comments/stage-one BGM banner smoke, not complete dialogue/bubble/ruby/ending or physical-device acceptance',observations},null,2));
});
