import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {execFileSync} from 'node:child_process';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
import {root,report} from '../cpp/helpers.mjs';
import {launcherServer,startLauncherPage,runtimeFrame,rpc} from './launcher-fixture.mjs';
test('Production launcher Pointdevice retry displays the original patterned Restart backdrop',{timeout:300000},async()=>{
 const server=await launcherServer();let browser;const errors=[],observations=[];
 try{
  browser=await launchBrowser({args:['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']});
  const page=await startLauncherPage(browser,{viewport:{width:1280,height:960}});page.on('pageerror',e=>errors.push(String(e)));
  await page.goto(server.url,{timeout:90000});const manifest=await page.evaluate(async()=>await(await fetch('/manifest.json')).json());assert.equal(manifest.development,false);
  await page.locator('#changelogConfirm').click();await page.locator('button[data-game="th15"]').click();await page.locator('#launch').click();
  await page.waitForFunction(()=>document.querySelector('#gameFrame')?.contentWindow?.Module?._th15_frame?.()>=160,null,{timeout:150000});
  const runtime=runtimeFrame(page);await runtime.waitForFunction(()=>Module._th15_phase()===0);
  const frames=async n=>{const first=await runtime.evaluate(()=>Module._th15_frame());await runtime.waitForFunction(({first,n})=>Module._th15_frame()>=first+n,{first,n},{timeout:60000});};const key=async code=>{await rpc(page,'keyboard',{code,down:true});await frames(3);await rpc(page,'keyboard',{code,down:false});await frames(3);};
  await key('KeyZ');await frames(30);await key('ArrowUp');await key('KeyZ');await frames(30);
  await key('ArrowUp');await key('KeyZ');await frames(30);await key('KeyZ');
  await runtime.waitForFunction(()=>Module._th15_phase()===1,null,{timeout:30000});await frames(150);
  await key('Escape');await runtime.waitForFunction(()=>Module._th15_phase()===2,null,{timeout:10000});await frames(24);
  for(let n=0;n<3;n++)await key('ArrowDown');await key('KeyZ');await frames(42);await key('ArrowUp');await key('KeyZ');
  await runtime.waitForFunction(()=>Module._th15_phase()===1,null,{timeout:10000});await page.waitForTimeout(220);await runtime.evaluate(()=>Module._th15_loop_stop());
  const inspect=async label=>{const name='production-restart-'+label+'.png',path=resolve(root,'artifacts/cpp/verification',name);await runtime.locator('canvas').screenshot({path,timeout:120000});const pixels=JSON.parse(execFileSync('python',['-c',"import json,sys;from PIL import Image;im=Image.open(sys.argv[1]).convert('RGB');w,h=im.size;crop=im.crop((int(w*34/640),int(h*60/480),int(w*414/640),int(h*460/480)));p=list(crop.getdata());purple=sum(r>g*1.15 and b>g*1.15 and r+b>60 for r,g,b in p);green=sum(g>r*1.3 and g>b*1.3 and g>30 for r,g,b in p);print(json.dumps({'width':w,'height':h,'pixels':len(p),'purple':purple,'green':green}))",path],{encoding:'utf8',windowsHide:true}));observations.push({label,pixels,screenshot:name});console.log('Production Restart',JSON.stringify(observations.at(-1)));return pixels;};
  const overlay=await inspect('overlay');assert.ok(overlay.purple>overlay.pixels*.3,'Production Restart backdrop missing '+JSON.stringify(overlay));
  await runtime.evaluate(()=>Module._th15_loop_start());await page.waitForTimeout(1800);await runtime.evaluate(()=>Module._th15_loop_stop());const resumed=await inspect('resumed');assert.ok(resumed.green>resumed.pixels*.3,'Stage did not return after Restart');
  const state=await runtime.evaluate(()=>{const c=Module,p=c._th15_error();return {phase:c._th15_phase(),error:new TextDecoder().decode(c.HEAPU8.subarray(p,c.HEAPU8.indexOf(0,p))),hasProbe:typeof c._th15_probe_state==='function'||typeof c._application_test_death==='function'};});assert.equal(state.error,'');assert.equal(state.phase,1);assert.equal(state.hasProbe,false);assert.deepEqual(errors,[]);
  report('sdl-production-restart-background',{passed:true,manifestVersion:manifest.version,wasmSha256:manifest.execution.sha256,state,observations,scope:'Final production shared launcher with actual keyboard RPC and live 60Hz runtime; Pointdevice selected, pause menu Retry plus confirmation restores checkpoint. Real Canvas capture verifies the original patterned overlay and subsequent stage background. No developer probes, forced death, invulnerability or altered gameplay input.'});
 }finally{await browser?.close();await server.close();}
});
