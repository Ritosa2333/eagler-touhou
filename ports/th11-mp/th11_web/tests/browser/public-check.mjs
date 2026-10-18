import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {fileURLToPath} from 'node:url';
import {resolve} from 'node:path';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const root=fileURLToPath(new URL('../../',import.meta.url)),out=resolve(root,'artifacts/sdl-release/verification');mkdirSync(out,{recursive:true});
const base=process.env.TH11_PUBLIC_URL||'https://api.steinsgateon.com',expected=JSON.parse(readFileSync(resolve(root,'artifacts/sdl-release/site/manifest.json')));
const browser=await launchBrowser(),context=await browser.newContext({viewport:{width:390,height:844},isMobile:true,hasTouch:true,deviceScaleFactor:1}),page=await context.newPage(),errors=[];
page.on('pageerror',e=>errors.push(e.stack));page.setDefaultTimeout(300000);
const result={physicalDevice:false,url:base,originPort:Number(process.env.TH11_ORIGIN_PORT||3007),checkedAt:new Date().toISOString()};
try{
 const response=await context.request.get(base+'/manifest.json',{timeout:30000});assert.equal(response.status(),200);const actual=await response.json();
 for(const name of ['game','version','completeGame'])assert.equal(actual[name],expected[name]);
 assert.equal(actual.execution.sha256,expected.execution.sha256);assert.equal(actual.execution.loaderSha256,expected.execution.loaderSha256);
 console.log('Public manifest verified:',actual.version);
 await page.goto(base+'/',{waitUntil:'domcontentloaded',timeout:60000});await page.locator('button[data-game=th11]').waitFor();
 await page.locator('#changelogConfirm').waitFor({state:'visible',timeout:5000}).catch(()=>{});
 if(await page.locator('#changelogConfirm').isVisible())await page.locator('#changelogConfirm').click();
 await page.locator('button[data-game=th11]').click();await page.locator('#launch').click();
 console.log('Public launcher started; downloading runtime assets.');
 await page.waitForFunction(()=>document.querySelector('#gameFrame')?.contentWindow?.Module?._th11_frame()>100);
 if(await page.locator('#touchHelpClose').isVisible())await page.locator('#touchHelpClose').click();
 const frame=page.frames().find(f=>f.url().includes('/runtime/th11/th11.html'));assert.ok(frame);
 console.log('Public game initialized.');
 result.audio=await frame.evaluate(()=>Module.SDL3.audioContext.state);assert.equal(result.audio,'running');
 for(let n=0;n<4;++n){await page.keyboard.press('KeyZ',{delay:90});await page.waitForTimeout(500);}
 await frame.waitForFunction(()=>Module._th11_phase()===1&&Module._th11_frame()>120);
 await page.screenshot({path:resolve(out,'public-mobile-battle.png')});
 await page.locator('#touchEscape').click();await frame.waitForFunction(()=>Module._th11_phase()===2);const paused=await frame.evaluate(()=>Module._th11_frame());await page.waitForTimeout(350);assert.equal(await frame.evaluate(()=>Module._th11_frame()),paused);
 await page.locator('#touchEscape').click();await frame.waitForFunction(paused=>Module._th11_phase()===1&&Module._th11_frame()>paused,paused);
 // The release server serves manifest entries only, including through the tunnel.
 const blocked=[];for(const path of ['/th11.exe','/th11c.exe','/scoreth11.dat','/scripts/serve.mjs','/cloudflared-token.txt','/target.json']){const r=await context.request.get(base+path,{timeout:30000});assert.equal(r.status(),404,path);blocked.push(path);}
 assert.deepEqual(errors,[]);Object.assign(result,{passed:true,version:actual.version,wasm:actual.execution.sha256,publicResourceDownload:true,keyboardStart:true,mobilePauseResume:true,blocked});console.log(JSON.stringify(result));
}catch(e){result.error=e.stack;result.errors=errors;await page.screenshot({path:resolve(out,'public-failure.png')}).catch(()=>{});console.error(e);process.exitCode=1;}
finally{writeFileSync(resolve(out,'deployment.json'),JSON.stringify(result,null,2)+'\n');await browser.close();}
