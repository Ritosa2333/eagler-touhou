import test from 'node:test';
import assert from 'node:assert/strict';
import {createServer} from 'node:http';
import {readFile,readdir,mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const root=fileURLToPath(new URL('../../',import.meta.url));
for(const locked of [1,0])test('Actual browser retains native startup and visible stage transition (60Hz lock='+locked+')', {timeout:180000}, async()=>{
 assert.ok(process.env.TH15_ORIGINAL_DAT,'Set TH15_ORIGINAL_DAT');
 const fontRoot=resolve(process.env.EAGLER_FONT_ROOT||resolve(root,'assets/sdl-native/fonts'));
 const fonts=(await readdir(fontRoot)).filter(n=>n.endsWith('.bin'));
 assert.ok(fonts.includes('cp932.bin')&&fonts.includes('blend4444.bin'),'Measured runtime fonts are required');
 const files=new Map([['/module.mjs',resolve(root,'artifacts/sdl-application/th15-application.mjs')],['/th15-application.wasm',resolve(root,'artifacts/sdl-application/th15-application.wasm')],['/th15.dat',process.env.TH15_ORIGINAL_DAT],...fonts.map(n=>['/fonts/'+n,resolve(fontRoot,n)])]);
 const server=createServer(async(req,res)=>{try{const path=new URL(req.url,'http://localhost').pathname;res.setHeader('Cross-Origin-Opener-Policy','same-origin');res.setHeader('Cross-Origin-Embedder-Policy','require-corp');if(path==='/'){res.setHeader('Content-Type','text/html');res.end('<canvas id="canvas" width="1280" height="960"></canvas><script type="module">import create from "/module.mjs";window.events=[];window.fixture=await create({canvas:document.querySelector("canvas"),onGameFrame:(ok,ms,ticks)=>events.push({ok,ticks,time:performance.now()})});</script>');return;}const file=files.get(path);if(!file){res.writeHead(404).end();return;}res.setHeader('Content-Type',path.endsWith('.mjs')?'text/javascript':path.endsWith('.wasm')?'application/wasm':'application/octet-stream');res.end(await readFile(file));}catch(e){res.writeHead(500).end(String(e));}});
 await new Promise(r=>server.listen(0,'127.0.0.1',r));let browser;const errors=[];const output=resolve(root,'artifacts/cpp/verification');await mkdir(output,{recursive:true});
 try{
  browser=await launchBrowser();const page=await browser.newPage();page.on('pageerror',e=>errors.push(String(e)));await page.goto('http://127.0.0.1:'+server.address().port+'/');await page.waitForFunction(()=>window.fixture,null,{timeout:90000});
  const startup=await page.evaluate(async fonts=>{const c=fixture;c.FS.mkdir('/fonts');c.FS.mkdir('/save');for(const name of fonts)c.FS.writeFile('/fonts/'+name,new Uint8Array(await(await fetch('/fonts/'+name)).arrayBuffer()));c.FS.writeFile('/th15.dat',new Uint8Array(await(await fetch('/th15.dat')).arrayBuffer()));c._th15_music_enabled(0);c._th15_render_scale(2);window.started=performance.now();if(!c._th15_prepare_loading()||!c._th15_initialize())throw Error('Initialize failed');window.startupFrame=c._th15_frame();const remaining=c._th15_probe_loading_remaining();c._th15_loop_start();if(remaining>100){await new Promise(r=>setTimeout(r,Math.min(150,remaining/2)));if(c._th15_frame()!==startupFrame)throw Error('Startup advanced before buffer elapsed');}return {remaining,frame:startupFrame};},fonts);
  const initialLoading=await page.locator('canvas').screenshot({path:resolve(output,'loading-startup-initial.png')});
  await page.waitForFunction(()=>fixture._th15_probe_loading_remaining()<=1500,null,{timeout:10000});
  assert.equal(await page.evaluate(()=>fixture._th15_frame()),startup.frame,'Loading must not tick gameplay');
  const middleLoading=await page.locator('canvas').screenshot({path:resolve(output,'loading-startup-middle.png')});
  assert(!initialLoading.equals(middleLoading),'Original loading ANM must animate, not remain a frozen screenshot');
  await page.waitForFunction(()=>fixture._th15_frame()>startupFrame,null,{timeout:15000});startup.elapsed=await page.evaluate(()=>performance.now()-started);assert.equal(startup.remaining,3000,'Native 180-frame startup wait');assert(startup.elapsed>=3000,'Startup must not be skipped');
  await page.evaluate(locked=>fixture._th15_limit_presentation(locked),locked);
  await page.evaluate(()=>{const c=fixture;c._th15_loop_stop();window.state=()=>Array.from(c.HEAP32.subarray(c._th15_probe_state()/4,c._th15_probe_state()/4+16));window.world=()=>Array.from(c.HEAPU32.subarray(c._th15_probe_world_state()/4,c._th15_probe_world_state()/4+16));window.tick=()=>{if(!c._th15_probe_tick())throw Error('Tick failed');};window.frames=n=>{for(let i=0;i<n;i++)tick();};window.key=scan=>{c._th15_key(scan,1);tick();c._th15_key(scan,0);tick();};window.waitMenu=screen=>{for(let i=0;i<350;i++){tick();if(state()[8]===screen&&state()[9]===2)return;}throw Error('Menu timeout');};
   window.beginHold=stage=>{window.holdPromise=(async()=>{const before={remaining:c._th15_probe_loading_remaining(),pending:c._th15_probe_loading_pending(),frame:c._th15_frame()},start=performance.now();if(!before.pending||before.remaining)throw Error('Expected pending resource task without deadline');c._th15_loop_start();while(c._th15_frame()===before.frame)await new Promise(requestAnimationFrame);const after={elapsed:performance.now()-start,pending:c._th15_probe_loading_pending(),state:state(),world:world()};c._th15_loop_stop();if(after.pending||after.state[2]!==stage)throw Error('Preparation did not complete');const advanced=after.state[7]-before.frame;if(advanced<1||advanced>4||after.world[1]!==advanced)throw Error('Loading accumulated simulation ticks '+JSON.stringify({before,after}));return {before,after};})();};
   waitMenu(1);key(28);waitMenu(5);key(28);waitMenu(6);key(28);waitMenu(7);key(28);for(let i=0;i<250;i++){if(c._th15_probe_loading_pending()>0){beginHold(1);return;}tick();}throw Error('No entry loading');});
  const entry=await page.evaluate(()=>holdPromise);assert.equal(entry.before.remaining,0);
  await page.evaluate(()=>{fixture._th15_probe_protection(100000);frames(245);if(!fixture._th15_probe_complete_stage())throw Error('Complete stage failed');for(let i=0;i<140;i++){if(fixture._th15_probe_loading_pending()>0){
   const c=fixture,ptr=c._th15_probe_pixels(),pixels=c.HEAPU8.subarray(ptr,ptr+640*480*4);let visible=0;
   for(let j=0;j<pixels.length;j+=4)if(pixels[j]+pixels[j+1]+pixels[j+2]>24)visible++;
   if(visible<640*480*.1)throw Error('Inter-stage loading replaced the scene with a black screen: '+visible);
   window.transition={visible,total:640*480};beginHold(2);return;}tick();}throw Error('No next-stage loading');});
  const next=await page.evaluate(()=>holdPromise);assert.equal(next.before.remaining,0);assert.deepEqual(errors,[]);
  await page.locator('canvas').screenshot({path:resolve(output,'loading-next-stage.png')});
  await writeFile(resolve(output,'loading-transition-lock-'+locked+'.json'),JSON.stringify(await page.evaluate(()=>transition),null,2));
  await writeFile(resolve(output,'loading-buffer.json'),JSON.stringify({passed:true,startup,entry,next,scope:'Actual SDL browser RAF and original private archive/fonts; native 180-frame startup with animated ANM, resource-driven game loading, no accumulated gameplay ticks on resume. Music disabled; desktop Chromium. Native async resource workers are not claimed cycle-identical.'},null,2));console.log(JSON.stringify({startup:startup.elapsed,entry:entry.after.elapsed,next:next.after.elapsed}));
 }finally{await browser?.close();await new Promise(r=>server.close(r));}
});
