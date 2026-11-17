import test from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {launchBrowser} from '../../../../../tools/browser/launch.mjs';

// Private resources are operator inputs, never downloaded or checked in.
const input=process.env.TH15_CAPTURE_INPUT;
const fonts=process.env.TH15_CAPTURE_FONTS;
const root=new URL('../../',import.meta.url);
test('real stages 3/6 with THPrac preserve only the current moving marker',{
 skip:!input||!fonts?'Set TH15_CAPTURE_INPUT (import tree) and TH15_CAPTURE_FONTS':false,
 timeout:600000
},async()=>{
 const paths=new Map([
  ['/module.mjs',new URL('artifacts/sdl-application/th15-application.mjs',root)],
  ['/gpu-diagnostics.mjs',new URL('sdl-runtime/gpu-diagnostics.mjs',root)],
  ['/th15-application.wasm',new URL('artifacts/sdl-application/th15-application.wasm',root)],
  ['/th15.dat',resolve(input,'games/th15/th15.dat')],
  ['/unifont.otf',resolve(input,'shared/unifont.otf')],
  ['/msgothic.ttc',resolve(input,'shared/msgothic.ttc')]
 ]);
 const fontNames=['cp932.bin'];
 for(const name of fontNames)paths.set('/fonts/'+name,resolve(fonts,name));
 const html='<canvas id="canvas" width="640" height="480"></canvas><script type="module">import create from "/module.mjs";import {installGpuDiagnostics} from "/gpu-diagnostics.mjs";const canvas=document.querySelector("canvas");const core=await create({canvas,eaglerOptions:{thpracEnabled:true,thpracLocale:"en"}});installGpuDiagnostics(core,canvas,{profile:"desktop-regression"});window.fixture=core;</script>';
 const server=createServer(async(req,res)=>{
  try{const path=new URL(req.url,'http://localhost').pathname;
   const bytes=path==='/'?html:paths.has(path)?await readFile(paths.get(path)):null;
   if(bytes===null){res.writeHead(404).end();return;}
   res.writeHead(200,{'Content-Type':path==='/'?'text/html':path.endsWith('.mjs')?'text/javascript':path.endsWith('.wasm')?'application/wasm':'application/octet-stream','Cross-Origin-Opener-Policy':'same-origin','Cross-Origin-Embedder-Policy':'require-corp'}).end(bytes);
  }catch(error){res.writeHead(500).end(String(error));}
 });
 await new Promise(r=>server.listen(0,'127.0.0.1',r));
 let browser;const observations=[],errors=[];
 try{
  browser=await launchBrowser();
  for(const [stage,scale,highRefresh,copies,flags] of [[3,1,false,true,0],[6,2,false,true,0],[6,2,true,true,0],[6,2,false,false,0],[6,2,false,true,1],[6,2,false,true,2]]){
   const page=await browser.newPage({viewport:{width:1280,height:720},isMobile:true,hasTouch:true,deviceScaleFactor:scale});
   page.on('pageerror',e=>errors.push(String(e)));
   await page.goto('http://127.0.0.1:'+server.address().port+'/');
   await page.waitForFunction(()=>window.fixture,{timeout:90000});
   await page.evaluate(async({stage,scale,highRefresh,copies,flags,fontNames})=>{
    const c=fixture;c.FS.mkdir('/fonts');c.FS.mkdir('/save');
    for(const file of ['th15.dat','unifont.otf','msgothic.ttc',...fontNames.map(n=>'fonts/'+n)])c.FS.writeFile('/'+file,new Uint8Array(await(await fetch('/'+file)).arrayBuffer()));
    const decode=p=>new TextDecoder().decode(c.HEAPU8.subarray(p,c.HEAPU8.indexOf(0,p)));
    window.step=()=>{if(!c._application_step(9,0,0,60,0))throw Error(decode(c._application_error()));};
    c._application_test_music_enabled(0);c._application_render_scale(scale);c._application_test_presentation_enable(highRefresh?1:0);
    th15GpuDiagnostics.configure({copies,flush:!!(flags&1),resetState:!!(flags&2)});
    if(!c._application_initialize(0))throw Error(decode(c._application_error()));
    if(!c._application_practice_initialize())throw Error('THPrac initialization failed');
    if(!c._application_test_capture_stage(stage))throw Error(decode(c._application_error()));
    const p=c._application_practice_state()>>>2;if(c.HEAP32[p]!==1)throw Error('THPrac not enabled');
   },{stage,scale,highRefresh,copies,flags,fontNames});
   const samples=[];
   for(let batch=0;batch<40;batch++){
    const sample=await page.evaluate(batch=>{
     const c=fixture;c._application_test_protection(60000);
     // Exercise real chapter banners and ImGui state, including menus never
     // explicitly opened by a player. Keep all original stage effects active.
     if(batch===4||batch===20)c._application_test_chapter_reward();
     for(let i=0;i<180;i++){step();c._application_practice_frame();}
     const s=Array.from(c.HEAP32.subarray(c._application_test_capture_state()>>>2,(c._application_test_capture_state()>>>2)+5));
     const pixels=c._application_pixels();let marker=0;
     for(let x=80;x<220;x++){const at=pixels+(s[1]*640+x)*4;
      if(Math.abs(c.HEAPU8[at]-19)<=1&&Math.abs(c.HEAPU8[at+1]-251)<=1&&Math.abs(c.HEAPU8[at+2]-7)<=1)marker++;
     }
     const gl=document.querySelector('canvas').getContext('webgl2'),error=gl.getError();
     if(error!==gl.NO_ERROR)throw Error('WebGL error '+error);
     return {batch,state:s,marker};
    },batch);
    // Real camera shake, masks and fades can partially cover the marker. They
    // must not enlarge it into a trail of previous frames.
    assert.ok(sample.marker<=9,'stage '+stage+' batch '+batch+' accumulated old marker pixels: '+sample.marker);
    samples.push(sample);
   }
   assert.ok(samples.some(s=>s.marker===8),'the current marker must be visible, not merely cleared/hidden');
   assert.ok(samples.some(s=>s.state[3]>100),'real projectile-heavy frames must be exercised');
   if(copies)assert.ok(samples.at(-1).state[2]>7000,'real game captures must use the new transfer path');
   else assert.equal(samples.at(-1).state[2],0,'baseline must use the original raster path');
   await mkdir(new URL('artifacts/cpp/verification/',root),{recursive:true});
   await page.locator('canvas').screenshot({path:fileURLToPath(new URL('artifacts/cpp/verification/capture-stage'+stage+'.png',root))});
   const diagnostics=await page.evaluate(()=>th15GpuDiagnostics.report());
   assert.ok(diagnostics.gpu?.version,'report must use the live rendering context');
   assert.equal(diagnostics.profile.copies,copies);
   observations.push({stage,scale,highRefresh,copies,flags,diagnostics,samples});
   await page.evaluate(()=>fixture._application_close());await page.close();
  }
  assert.deepEqual(errors,[]);
  await writeFile(new URL('artifacts/cpp/verification/mobile-capture.json',root),JSON.stringify({observations,scope:'Automated desktop GPU with mobile viewport/touch, real STD/ECL stages 3 and 6, THPrac, original chapter animation and a moving marker. Not physical Android acceptance.'},null,2));
 }finally{await browser?.close();await new Promise(r=>server.close(r));}
});
