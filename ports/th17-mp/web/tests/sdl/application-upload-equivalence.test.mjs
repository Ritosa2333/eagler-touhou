import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {applicationPage,applicationServer} from './application-fixture.mjs';
import {launchBrowser} from '../../../../../tools/browser/launch.mjs';
import {root,sha,report} from '../cpp/helpers.mjs';
test('Staged vertex uploads preserve replay pixels, state and ordered transparency',{timeout:3600000},async()=>{
 const skip=Number(process.env.TH17_PIXEL_SKIP||0),count=Number(process.env.TH17_PIXEL_FRAMES||1800),interval=Number(process.env.TH17_PIXEL_INTERVAL||120),label=process.env.TH17_PIXEL_LABEL||'upload-equivalence';
 const server=await applicationServer(),browser=await launchBrowser(),out=resolve(root,'artifacts/performance',label);await mkdir(out,{recursive:true});
 const bytes=await readFile(resolve(root,'../[th17] 东方鬼形兽（汉化版+日文版）/Th17_ud0134.rpy')),results=[],wasmSha256=sha(await readFile(resolve(root,'artifacts/sdl-application/th17-application.wasm')));
 try{
  const page=await applicationPage(browser,server,2);
  for(const stage of (process.env.TH17_PIXEL_STAGES||'1,2,3,4,5,6,7').split(',').map(Number)){
   const variants=[];
   for(const staged of [false,true]){
    await page.evaluate(({bytes,stage,staged})=>{
     fixture._application_close();initialize();fixture._application_test_staged_uploads(staged);
     if(stage===7){choose(5,10);fixture._application_unlock();step(0x102);ready(1);settle(30);choose(1,5);choose(0,6);choose(1,7);fixture._application_title_select(0);step(1);waitFor(()=>state()[0]===1);}
     else{fixture.FS.writeFile('/save/replay/th17_01.rpy',new Uint8Array(bytes));const p=fixture._malloc(32);fixture.HEAPU8.set(new TextEncoder().encode('th17_01.rpy\0'),p);if(!fixture._application_test_replay(p,stage))throw Error('Replay failed');fixture._free(p);waitFor(()=>state()[0]===1);}
     while(world()[0]===0)step();
    },{bytes:Array.from(bytes),stage,staged});
    for(let at=0;at<skip;at+=120)await page.evaluate(n=>{for(let i=0;i<n;i++)step();},Math.min(120,skip-at));
    const frames=[];
    for(let at=0;at<count;at+=interval){
     const n=Math.min(interval,count-at),result=await page.evaluate(async({stage,at,n})=>{
      for(let i=0;i<n;i++){if(stage===7){fixture._application_test_protection(60000);step(at===600&&i===0?2:0,0,1);}else step();}
      const gl=document.querySelector('canvas').getContext('webgl2'),pixels=new Uint8Array(1280*960*4),read=gl.getParameter(gl.READ_FRAMEBUFFER_BINDING);gl.bindFramebuffer(gl.READ_FRAMEBUFFER,null);gl.readPixels(0,0,1280,960,gl.RGBA,gl.UNSIGNED_BYTE,pixels);gl.bindFramebuffer(gl.READ_FRAMEBUFFER,read);
      const pixelHash=Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256',pixels)),v=>v.toString(16).padStart(2,'0')).join(''),p=fixture._application_graphics_stats()>>>2;
      return {world:world(),pixelHash,stats:Array.from(fixture.HEAPU32.subarray(p,p+17)),error:gl.getError()};
     },{stage,at,n});
     assert.equal(result.error,0);assert.equal(result.stats[7],0);frames.push(result);
     if(process.env.TH17_PIXEL_CAPTURE==='1')await page.locator('canvas').screenshot({path:resolve(out,`stage-${stage}-${skip+at+n}-${staged?'staged':'immediate'}.png`)});
    }
    variants.push(frames);await page.locator('canvas').screenshot({path:resolve(out,`stage-${stage}-${staged?'staged':'immediate'}.png`)});
   }
   await writeFile(resolve(out,`stage-${stage}-variants.json`),JSON.stringify(variants));
   for(let i=0;i<variants[0].length;i++){assert.deepEqual(variants[1][i].world,variants[0][i].world,`Stage ${stage} state ${i}`);assert.equal(variants[1][i].pixelHash,variants[0][i].pixelHash,`Stage ${stage} physical pixels ${i}`);}
   results.push({stage,skip,frames:count,checks:variants[0].length,variants});console.log(`stage ${stage}: state and pixels match`);await writeFile(resolve(out,'results.json'),JSON.stringify(results));
  }
  report(skip?'sdl-upload-cancellation-window':'sdl-upload-equivalence',{passed:true,wasmSha256,results,scope:'Same binary, immediate versus staged uploads; physical 1280x960 pixels and replay state. Actual measured stages, skip, frame count and sample interval are recorded per result; Extra uses protected scripted input.'});
 }finally{await browser.close();await server.close();}
});
