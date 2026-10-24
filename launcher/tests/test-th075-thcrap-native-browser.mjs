// Non-publishable harness: all translated banks, script-control parity, glyphs,
// and all eleven native fighter loaders. Requires private DATA and built harness.
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {createReadStream} from 'node:fs';
import {resolve,extname} from 'node:path';
import {unzipSync} from 'fflate';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../lib/chromium-executable.mjs';
const title=resolve(process.env.TH075_RUNTIME_ROOT||'../th075'),language=process.env.TH075_TEST_LANGUAGE||'lang_zh-hans';
const prepared=resolve(process.env.TH075_LANGUAGE_PACKS||'../../prepared/th075-thcrap-static');
const dataFile=resolve(process.env.TH075_CONTENT_DATA||resolve(title,'assets-ogg/th075.data'));
const files=unzipSync(await readFile(resolve(prepared,'language',language+'.zip'))),manifest=JSON.parse(Buffer.from(files['manifest.json']));
const entries=manifest.files.map(f=>({path:f.path,bytes:files[f.path.slice(1)]})),output=resolve('artifacts/th075-thcrap');await mkdir(output,{recursive:true});
const html=`<canvas id="canvas" width="640" height="480"></canvas><script type="module">
import create from '/th075-harness.mjs';import {parseContentBundle} from '/content-bundle.mjs';
const bundle=parseContentBundle(await(await fetch('/th075.data')).arrayBuffer());
window.game=await create({canvas:document.querySelector('canvas'),resourceFiles:new Set(bundle.paths.map(p=>p.slice(1))),readAsset:async p=>(await bundle.read(p)).buffer});
for(const path of ['/fonts/original-glyphs.bin','/music/catalog.bin']){game.FS.mkdirTree(path.slice(0,path.lastIndexOf('/')));game.FS.writeFile(path,await bundle.read(path));}
const entries=await(await fetch('/entries.json')).json();for(let i=0;i<entries.length;i++){const path=entries[i];game.FS.mkdirTree(path.slice(0,path.lastIndexOf('/')));game.FS.writeFile(path,new Uint8Array(await(await fetch('/language/'+i)).arrayBuffer()));}
game.th075Language=${JSON.stringify(language)};game._th075_music_enabled(0);window.ready=await game.ccall('th075_initialize','number',['number'],[1],{async:true});</script>`;
const server=createServer(async(req,res)=>{const path=new URL(req.url,'http://test').pathname;
 if(path==='/'){res.setHeader('content-type','text/html');res.end(html);return;}
 if(path==='/entries.json'){res.setHeader('content-type','application/json');res.end(JSON.stringify(entries.map(e=>e.path)));return;}
 if(/^\/language\/\d+$/.test(path)){const entry=entries[Number(path.slice(10))];if(!entry){res.writeHead(404).end();return;}res.end(entry.bytes);return;}
 const names={'/th075-harness.mjs':'web/artifacts/sdl3/th075-harness.mjs','/th075-harness.wasm':'web/artifacts/sdl3/th075-harness.wasm','/content-bundle.mjs':'portable/content-bundle.mjs','/th075.data':dataFile};
 if(!names[path]){res.writeHead(404).end();return;}const file=resolve(title,names[path]);res.setHeader('content-type',extname(file)==='.mjs'?'text/javascript':extname(file)==='.wasm'?'application/wasm':'application/octet-stream');createReadStream(file).pipe(res);
});await new Promise(r=>server.listen(0,'127.0.0.1',r));let browser;
try{
 browser=await puppeteer.launch({executablePath:await findChromiumExecutable(),headless:true,args:['--use-angle=swiftshader','--enable-unsafe-swiftshader']});const page=await browser.newPage(),errors=[];page.on('pageerror',e=>errors.push(String(e)));await page.goto('http://127.0.0.1:'+server.address().port);await page.waitForFunction(()=>window.ready!==undefined,{timeout:120000});assert.equal(await page.evaluate(()=>ready),1);
 const checks=[];
 for(const file of manifest.files){const match=/^\/thcrap\/th075\/(images|text)\/(.+)\.bin$/.exec(file.path);if(!match)continue;
  const baseVariant=match[2].startsWith('base/'),path=baseVariant?match[2].slice(5):match[2],sprite=/^data\/character\/(\d{2})(2)?\.dat$/.exec(path),edition=!baseVariant&&(path.endsWith('.sce')||path==='musicroom.dat'||/^data\/system\/(select|selectchar|selectbg|replay)\.dat$/.test(path)||(sprite&&(sprite[2]?[4,5,6,7,8,10].includes(+sprite[1]):[3,10].includes(+sprite[1]))))?1:0;
  const kind=match[1]==='images'?0:path==='musicroom.dat'?2:1;
  const result=await page.evaluate(({path,edition,kind})=>game.ccall('th075_probe_language_resource','number',['string','number','number'],[path,edition,kind],{async:true}),{path,edition,kind});assert.ok(result>0,`${path}: native check ${result}`);checks.push({path,kind,result});
 }
 let timestamp=0;async function tick(){const result=await page.evaluate(async time=>{const ok=await game.ccall('th075_tick','number',['number'],[time],{async:true});return {ok,error:game.UTF8ToString(game._th075_error()),scene:game._th075_scene()};},timestamp+=1000/60);assert.equal(result.ok,1,result.error);return result.scene;}
 for(let id=0;id<11;id++){assert.equal(await page.evaluate(id=>game.ccall('th075_probe_battle','number',['number','number','number','number'],[5,id,(id+1)%11,0],{async:true}),id),1);let entered=false;for(let i=0;i<240;i++)if(await tick()===0){entered=true;break;}assert.ok(entered,'fighter '+id);for(let i=0;i<3;i++)await tick();if([0,3,10].includes(id))await page.screenshot({path:resolve(output,language+'-fighter-'+id+'.png')});console.log('fighter',id,'PASS');}
 // Actual score-entry and replay-menu scenes exercise both replay.dat editions.
 for(const scene of [10,5,7,10]){
  assert.equal(await page.evaluate(scene=>game.ccall('th075_probe_scene','number',['number'],[scene],{async:true}),scene),1,'enter scene '+scene);
  for(let i=0;i<5;i++)await tick();
  assert.equal(await page.evaluate(()=>game._th075_scene()),scene);
  await page.screenshot({path:resolve(output,language+'-scene-'+scene+'.png')});
 }
 for(const id of [0,1]){
  assert.equal(await page.evaluate(id=>game.ccall('th075_probe_battle','number',['number','number','number','number'],[3,id,(id+1)%11,0],{async:true}),id),1);
  for(let i=0;i<500;i++)await tick();
  assert.equal(await page.evaluate(()=>game._th075_scene()),0);
  await page.screenshot({path:resolve(output,language+'-story-'+id+'.png')});
 }
 const image=manifest.files.find(f=>f.path.startsWith('/thcrap/th075/images/'));
 if(image){const path=image.path.slice('/thcrap/th075/images/'.length,-4),sprite=/^data\/character\/(\d{2})(2)?\.dat$/.exec(path),edition=sprite&&(sprite[2]?[4,5,6,7,8,10].includes(+sprite[1]):[3,10].includes(+sprite[1]))?1:0;
  const negative=await page.evaluate(async({file,path,edition})=>{const original=game.FS.readFile(file),bad=original.slice();new DataView(bad.buffer).setUint32(8,0,true);game.FS.writeFile(file,bad);const crc=await game.ccall('th075_probe_language_resource','number',['string','number','number'],[path,edition,0],{async:true});game.FS.writeFile(file,original);return crc;},{file:image.path,path,edition});assert.equal(negative,-2,'Wrong original resource CRC must reject the language replacement');
 }
 assert.deepEqual(errors,[]);const report={language,checks,fighters:11,scoreAndReplayScenes:[10,5,7,10],scriptControlsUnchanged:true,allReplacementImagesDecode:true,allChangedTextRenders:true,wrongOriginalRejected:!!image,heapBytes:await page.evaluate(()=>game.HEAPU8.byteLength)};await writeFile(resolve(output,language+'-native-report.json'),JSON.stringify(report,null,2));console.log({language,banks:checks.length,fighters:11,heapBytes:report.heapBytes});
}finally{await browser?.close();await new Promise(r=>server.close(r));}
