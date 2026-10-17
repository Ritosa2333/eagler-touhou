import test from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,readdir,mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const root=resolve(import.meta.dirname,'../..');
test('Extra background presentation diagnostic',{timeout:240000},async()=>{
 assert(process.env.TH15_ORIGINAL_DAT&&process.env.EAGLER_FONT_ROOT);
 const fonts=(await readdir(process.env.EAGLER_FONT_ROOT)).filter(n=>n.endsWith('.bin'));
 const files=new Map([
  ['/module.mjs',resolve(root,'artifacts/sdl-application/th15-application.mjs')],
  ['/th15-application.wasm',resolve(root,'artifacts/sdl-application/th15-application.wasm')],
  ['/th15.dat',process.env.TH15_ORIGINAL_DAT],
  ...fonts.map(n=>['/fonts/'+n,resolve(process.env.EAGLER_FONT_ROOT,n)])
 ]);
 const server=createServer(async(req,res)=>{try{
  const path=new URL(req.url,'http://local').pathname;
  res.setHeader('Cross-Origin-Opener-Policy','same-origin');res.setHeader('Cross-Origin-Embedder-Policy','require-corp');
  if(path==='/'){res.setHeader('Content-Type','text/html');res.end('<canvas id="canvas" width="1280" height="960"></canvas><script type="module">import create from "/module.mjs";window.fixture=await create({canvas:document.querySelector("canvas")});</script>');return;}
  if(!files.has(path)){res.writeHead(404).end();return;}
  res.setHeader('Content-Type',path.endsWith('.mjs')?'text/javascript':'application/octet-stream');res.end(await readFile(files.get(path)));
 }catch(e){res.writeHead(500).end(String(e));}});
 await new Promise(r=>server.listen(0,'127.0.0.1',r));
 const browser=await launchBrowser({args:['--enable-unsafe-swiftshader']});
 try{
  const page=await browser.newPage();await page.goto('http://127.0.0.1:'+server.address().port);
  await page.waitForFunction(()=>window.fixture,null,{timeout:60000});
  await page.evaluate(async fonts=>{
   const c=fixture;c.FS.mkdir('/fonts');c.FS.mkdir('/save');
   for(const n of fonts)c.FS.writeFile('/fonts/'+n,new Uint8Array(await(await fetch('/fonts/'+n)).arrayBuffer()));
   c.FS.writeFile('/th15.dat',new Uint8Array(await(await fetch('/th15.dat')).arrayBuffer()));
   c._th15_music_enabled(0);c._th15_render_scale(2);
   if(!c._th15_initialize()||!c._th15_probe_extra_run())throw Error('Extra run initialization failed');
   c._th15_probe_presentation_enable(1);
  },fonts);
  const observations=[];
  for(let block=0;block<24;block++)observations.push(...await page.evaluate(()=>{
   const c=fixture,rows=[];c._th15_probe_protection(100000);
   const world=()=>Array.from(new Uint32Array(c.HEAPU8.buffer,c._th15_probe_world_state(),16));
   const pixels=()=>{const ptr=c._th15_probe_pixels();let hash=2166136261;for(const b of c.HEAPU8.subarray(ptr,ptr+640*480*4))hash=Math.imul(hash^b,16777619);return hash>>>0;};
   for(let n=0;n<100;n++){
    if(!c._th15_probe_tick())throw Error(c.UTF8ToString(c._th15_error()));
    const d=Array.from(new Float32Array(c.HEAPU8.buffer,c._th15_probe_background_diagnostics(),6));
    if(d[1]||d[4]||d[5]||n===99)rows.push({frame:c._th15_frame(),diagnostic:d});
    if(d[5]<0)throw Error('Extra UV matrix midpoint jumped away from its previous endpoint');
    if(d[0]){
     const before=JSON.stringify(world()),authored=n===99?pixels():null;
     if(!c._th15_probe_presentation_draw(.5))throw Error('Presentation unavailable');
     if(n===99){const half=pixels();c._th15_probe_presentation_draw(.5);if(pixels()!==half)throw Error('Midpoint pixels not idempotent');c._th15_probe_presentation_draw(1);if(pixels()!==authored)throw Error('Current endpoint differs from authored draw');}
     if(JSON.stringify(world())!==before)throw Error('Presentation mutated Extra gameplay state');
    }
   }return rows;
  }));
  const out=resolve(root,'artifacts/extra-background');await mkdir(out,{recursive:true});
  await page.screenshot({path:resolve(out,'extra.png')});
  assert(observations.some(r=>r.diagnostic[5]>0),'Must exercise actual Extra UV wrap boundaries');
  await writeFile(resolve(out,'diagnostic.json'),JSON.stringify({scope:'Original Extra, 2400 actual ticks and midpoint presentation draws; diagnostics are not a visual twitch oracle',fields:['cameraCommands','multiSampleCommands','matchedCameraSamples','maxCameraDelta','largeCameraElements','repeatUvWraps'],observations},null,2));
  console.log(JSON.stringify(observations.filter(r=>r.diagnostic[5]).slice(0,12)));
 }finally{await browser.close();await new Promise(r=>server.close(r));}
});
