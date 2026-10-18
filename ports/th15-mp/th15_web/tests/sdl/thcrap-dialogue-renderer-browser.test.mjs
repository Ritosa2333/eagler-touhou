import test from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,readdir,mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {unzipSync} from 'fflate';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const root=resolve(import.meta.dirname,'../..');

test('Translated dialogue measures through the real stage bridge and renders ruby without fatal errors',{timeout:180000},async()=>{
 assert(process.env.TH15_ORIGINAL_DAT&&process.env.TH15_LANGUAGE_PACKS&&process.env.EAGLER_FONT_ROOT);
 const fonts=(await readdir(process.env.EAGLER_FONT_ROOT)).filter(n=>n.endsWith('.bin'));
 const files=new Map([
  ['/module.mjs',resolve(root,'artifacts/sdl-application/th15-application.mjs')],
  ['/th15-application.wasm',resolve(root,'artifacts/sdl-application/th15-application.wasm')],
  ['/th15.dat',process.env.TH15_ORIGINAL_DAT],
  ...fonts.map(n=>['/fonts/'+n,resolve(process.env.EAGLER_FONT_ROOT,n)]),
 ]);
 const packs={};
 for(const language of ['lang_zh-hans','lang_en']){
  packs[language]=Object.entries(unzipSync(await readFile(resolve(process.env.TH15_LANGUAGE_PACKS,'language',language+'.zip'))))
   .filter(([name])=>name.startsWith('thcrap/')).map(([path,bytes])=>({path:'/'+path,bytes:Array.from(bytes)}));
 }
 const server=createServer(async(req,res)=>{try{
  const path=new URL(req.url,'http://local').pathname;
  res.setHeader('Cross-Origin-Opener-Policy','same-origin');res.setHeader('Cross-Origin-Embedder-Policy','require-corp');
  if(path==='/'){res.setHeader('Content-Type','text/html');res.end('<canvas id="canvas" width="1280" height="960"></canvas><script type="module">import create from "/module.mjs";window.fixture=await create({canvas:document.querySelector("canvas")});</script>');return;}
  if(path.startsWith('/pack/')){res.setHeader('Content-Type','application/json');res.end(JSON.stringify(packs[path.slice(6)]));return;}
  if(!files.has(path)){res.writeHead(404).end();return;}
  res.setHeader('Content-Type',path.endsWith('.mjs')?'text/javascript':'application/octet-stream');res.end(await readFile(files.get(path)));
 }catch(e){res.writeHead(500).end(String(e));}});
 await new Promise(r=>server.listen(0,'127.0.0.1',r));
 const browser=await launchBrowser({args:['--enable-unsafe-swiftshader']});
 const output=resolve(root,'artifacts/thcrap-dialogue-renderer');await mkdir(output,{recursive:true});
 const observations=[];
 try{for(const language of Object.keys(packs)){
  const page=await browser.newPage({viewport:{width:1320,height:1000}}),errors=[];
  page.on('pageerror',e=>errors.push(String(e)));await page.goto('http://127.0.0.1:'+server.address().port);
  await page.waitForFunction(()=>window.fixture,null,{timeout:60000});
  await page.evaluate(async({fonts,language})=>{
   const c=fixture;for(const dir of ['/fonts','/save'])c.FS.mkdir(dir);
   for(const name of fonts)c.FS.writeFile('/fonts/'+name,new Uint8Array(await(await fetch('/fonts/'+name)).arrayBuffer()));
   for(const file of await(await fetch('/pack/'+language)).json()){
    const dir=file.path.slice(0,file.path.lastIndexOf('/'));c.FS.mkdirTree(dir);c.FS.writeFile(file.path,new Uint8Array(file.bytes));
   }
   c.FS.writeFile('/th15.dat',new Uint8Array(await(await fetch('/th15.dat')).arrayBuffer()));
   c._th15_music_enabled(0);c._th15_render_scale(2);
   if(!c._th15_prepare_loading()||!c._th15_initialize())throw Error('Initialize failed');
   window.tick=()=>{if(!c._th15_probe_tick())throw Error('Actual application tick failed');};
   window.state=()=>Array.from(c.HEAP32.subarray(c._th15_probe_state()/4,c._th15_probe_state()/4+16));
   window.key=scan=>{c._th15_key(scan,1);tick();c._th15_key(scan,0);tick();};
   window.waitMenu=screen=>{for(let i=0;i<350;i++){tick();if(state()[8]===screen&&state()[9]===2)return;}throw Error('Menu timeout');};
   waitMenu(1);key(28);waitMenu(5);key(28);waitMenu(6);key(28);waitMenu(7);key(77);key(77);key(28);
   for(let i=0;i<350&&state()[0]!==1;i++)tick();if(state()[0]!==1)throw Error('Run did not start');
   for(let i=0;i<30;i++)tick();if(state()[4]!==2)throw Error('Fixture must select Sanae');c._th15_probe_protection(100000);
   window.extent=(text,font)=>{const bytes=new TextEncoder().encode(text+'\0'),p=c._malloc(bytes.length);c.HEAPU8.set(bytes,p);const result=c._th15_probe_dialogue_extent(p,font);c._free(p);return result;};
  },{fonts,language});
  const metrics=await page.evaluate(()=>[extent('',0),extent('我们是月兔的',0),extent('调查部队',0),extent('Eagle Rabbit',2)]);
  assert.equal(metrics[0],0);assert(metrics.slice(1).every(n=>n>0),'The stage service must forward font measurement');
  // Stage-one Sanae boss dialogue includes the "Eagle Rabbit" ruby row.
  assert.equal(await page.evaluate(()=>fixture._th15_probe_dialogue_request(0)),1);
  let complete=false,boxes=0;
  for(let i=0;i<120;i++){
   const result=await page.evaluate(()=>{const result=fixture._th15_probe_dialogue_advance();for(let i=0;i<30;i++)tick();return result;});
   assert(result>=0,'Ruby dialogue must not fail');boxes++;
   if(i===3||i===5||i===8)await page.screenshot({path:resolve(output,language+'-box-'+i+'.png')});
   if(!result){complete=true;break;}
  }
  assert(complete,'Dialogue must advance to completion');assert(boxes>3,'The fixture must paint actual dialogue boxes');assert.deepEqual(errors,[]);
  observations.push({language,metrics,boxes});await page.close();
 }}finally{await browser.close();await new Promise(r=>server.close(r));}
 await writeFile(resolve(output,'report.json'),JSON.stringify({passed:true,observations,scope:'Original stage-one Sanae assets, prepared Chinese/English MSG, real StageGameplay bridge, font/ruby painting and animation; private diagnostics, not all-stage acceptance'},null,2));
});
