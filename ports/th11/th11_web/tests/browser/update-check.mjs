import assert from 'node:assert/strict';
import {mkdirSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';
import {releaseServer} from '../../../th09_web/scripts/release-server.mjs';
import {launchBrowser} from '../../../th10_web/scripts/native/browser-launch.mjs';
const root=fileURLToPath(new URL('../../',import.meta.url)),out=resolve(root,'artifacts/sdl3/browser/update');mkdirSync(out,{recursive:true});
let server=await releaseServer({root:resolve(root,'../th09_web/artifacts/sdl-release/site'),port:0});
const browser=await launchBrowser(),context=await browser.newContext(),page=await context.newPage(),result={physicalDevice:false};
page.setDefaultTimeout(120000);
const close=async()=>{server.netplay.close();server.server.closeAllConnections();await new Promise(r=>server.server.close(r));};
const dismiss=async()=>{const button=page.locator('#changelogConfirm');try{await button.waitFor({state:'visible',timeout:5000});await button.click();}catch(e){if(e.name!=='TimeoutError')throw e;}};
const launch=async()=>{await dismiss();await page.locator('button[data-game=th11]').click();await page.locator('#launch').click();await page.waitForFunction(()=>document.querySelector('#gameFrame')?.contentWindow?.Module?._th11_frame()>100);};
try{
 const port=server.server.address().port,url=server.url;
 await page.goto(url);await page.locator('button[data-game=th09]').waitFor();await dismiss();await page.evaluate(()=>navigator.serviceWorker.ready);await page.waitForFunction(()=>!!navigator.serviceWorker.controller);result.previousVersion=server.manifest.version;
 await close();server=await releaseServer({root:resolve(root,process.env.TH11_LAUNCHER_SITE||'artifacts/architecture-preview/site'),port});
 await page.evaluate(()=>navigator.serviceWorker.getRegistration().then(r=>r.update())).catch(e=>{if(!/context.*destroyed|navigation/i.test(e.message))throw e;});
 await page.locator('button[data-game=th11]').waitFor();result.updatedVersion=server.manifest.version;result.automaticShellReplacement=true;console.log('shell replacement passed');
 await launch();const frame=page.frames().find(f=>f.url().includes('/runtime/th11/th11.html'));assert.ok(frame);assert.equal(await frame.evaluate(()=>Module.SDL3.audioContext.state),'running');
 result.onlineLaunch=true;console.log('online launch passed');await frame.evaluate(()=>Module._th11_loop_stop());await page.reload();await page.locator('button[data-game=th11]').waitFor();
 await context.setOffline(true);await page.reload();await launch();result.offlineLaunch=true;
 await page.screenshot({path:resolve(out,'offline-title.png')});result.passed=true;console.log(JSON.stringify(result));
}catch(e){result.error=e.stack;await page.screenshot({path:resolve(out,'failure.png')}).catch(()=>{});console.error(e);process.exitCode=1;}
finally{writeFileSync(resolve(out,'report.json'),JSON.stringify(result,null,2)+'\n');await browser.close();await close();}
